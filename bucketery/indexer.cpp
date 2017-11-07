#include "pch.h"

#include "indexer.h"

#include "constants.h"
#include "errors.h"

indexer::item const indexer::last_item_in_sequence_ = { 0, 1, 0 };

indexer::indexer(file& file_storage, hole_manager& hm, std::optional<unsigned long long>& index_header_pos) : file_(file_storage), hm_(hm)
{
	if (!index_header_pos)
	{
		std::vector<item> initial_items;
		initial_items.resize(2);

		initial_items[0].id = 0;
		initial_items[0].pos = 0;
		initial_items[0].size = 0;

		initial_items[1] = last_item_in_sequence_;

		auto items_pos = hm_.insert<item>(page_reserved_indices);
		file_.write(initial_items, items_pos);

		page_header page_header;
		page_header.minid = 0;
		page_header.next_page_header_pos = 0;
		page_header.items_pos = items_pos;
		page_header.no_of_items_reserved = page_reserved_indices;
		page_header.no_of_items = 1;

		auto page_header_pos = hm_.insert<page_header_on_disk>();
		file_.write<page_header_on_disk>(page_header, page_header_pos);
		pages_.insert(page_header);

		header_.first_page_header_pos = page_header_pos;
		header_.new_id = 0;

		auto header_pos = hm_.insert<header>();
		file_.write(header_, header_pos);
		index_header_pos = header_pos_ = header_pos;

		file_.flush();
	}
	else
	{
		hm_.data_present<header>(*index_header_pos);
		file_.read(header_, *index_header_pos);

		std::vector<item> items;
		auto page = init_load_page(items, header_.first_page_header_pos);

		while (page->next_page_header_pos != 0)
			page = init_load_page(items, page->next_page_header_pos);

		header_pos_ = *index_header_pos;
	}
}

//items_data - is just a reading buffer. It is used only to avoid reallocation
indexer::page_set::iterator indexer::init_load_page(std::vector<item>& vec, unsigned long long pos)
{
	hm_.data_present<page_header_on_disk>(pos);

	page_header ph;
	file_.read(ph, pos);

	if (ph.no_of_items_reserved == 0)
		throw data_file_corrupted();

	bool loaded = false;
	size_t elements_loaded = ph.no_of_items_reserved;

	file_.read(vec, ph.items_pos, elements_loaded);

	size_t i = 0;
	while (!loaded)
	{
		if (i > elements_loaded)
		{
			//Number of items reserved is wrong, probably write error. Look for the last item
			item it;
			file_.read(it, pos + sizeof(item) * elements_loaded);
			if (it.id != 0)
			{
				hm_.data_present(it.pos, it.size);
				++elements_loaded;
				++i;
			}
			else
				loaded = true;
		}
		else
		{
			if (vec[i].id != last_item_in_sequence_.id || vec[i].pos != last_item_in_sequence_.pos || vec[i].size != last_item_in_sequence_.size)
			{
				hm_.data_present(vec[i].pos, vec[i].size);
				++i;
			}
			else
				loaded = true;
		}
	}

	hm_.data_present<item>(ph.items_pos, elements_loaded);

	ph.minid = vec[0].id;
	ph.no_of_items = i;

	auto page_inserted = pages_.insert(ph);
	if (!page_inserted.second)
		throw data_file_corrupted();

	return page_inserted.first;
}

indexer::page_set::iterator indexer::find_page(unsigned long long id)
{
	auto found_page = --pages_.upper_bound(id);
	return found_page;
}

std::vector<indexer::item> indexer::load_page_items(indexer::page_set::iterator page)
{
	std::vector<item> page_data;
	page_data.resize(page->no_of_items);

	file_.read(page_data, page->items_pos);
	return page_data;
}

std::vector<indexer::item>::iterator indexer::find_item(std::vector<indexer::item>& data, unsigned long long id)
{
	std::function<unsigned long long(item&)> deref = [](item& item) {return item.id; };
	auto it1 = boost::make_transform_iterator(data.begin(), deref);
	auto it2 = boost::make_transform_iterator(data.end(), deref);
	return std::lower_bound(it1, it2, id).base();
}

void indexer::set_item(unsigned long long id, unsigned long long pos, size_t size)
{
	auto page = find_page(id);
	auto page_items = load_page_items(page);
	auto it = find_item(page_items, id);
	size_t item_pos = std::distance(page_items.begin(), it);

	if (it != page_items.end() && it->id == id)
	{
		item old_item = *it;
		it->pos = pos;
		it->size = size;
		file_.write(*it, page->items_pos + sizeof(item) * item_pos);
		hm_.remove(old_item.pos, old_item.size);
	}
	else
	{
		item new_item{ id, pos, size };
		it = page_items.insert(it, new_item);

		if (page_items.size() > max_indices_to_divide)
		{
			//Divide page in two
			assert(false); //Not yet supported
		}
		else
		{
			//TODO Look for empty openings in page data

			page_items.push_back(last_item_in_sequence_);
			file_.write_raw(&page_items[item_pos], page->items_pos + item_pos * sizeof(item), (page_items.size() - item_pos) * sizeof(item));
		}
	}

	if (id >= header_.new_id)
	{
		header_.new_id = id + 1;
		file_.write(header_, header_pos_);
	}
}

void indexer::remove_item(unsigned long long id)
{
/*	auto page_info = find_page_info(id);
	auto page_data_info = load_page_data_info(page_info);
	auto item = find_item(page_data_info, id);

	if (item->id != id)
		throw data_not_present(id);

	index_item item_data = *item;
	size_t item_pos = std::distance(page_data_info.begin(), item);

	page_data_info.erase(item);
	update_page_data_info(page_data_info, item_pos);
	hm_.remove(item_data.pos, item_data.size);*/
}

indexer::item indexer::get_item(unsigned long long id)
{
	auto page = find_page(id);
	auto page_items = load_page_items(page);
	auto item = find_item(page_items, id);

	if (item == page_items.end() || item->id != id)
		throw data_not_present(id);

	return *item;
}

void indexer::update_page_data(page_set::iterator page_info, std::vector<item>& new_data)
{
/*	auto reserve = hm_.reserve(page_info->no_of_items + page_reserved_indices);
	file_.write(new_data, reserve->start());

	page_header new_page;
	new_page.minid = new_data[0].id;
	new_page.next_index_page = page_info->next_index_page;
	new_page.no_of_items = new_data.size();
	new_page.no_of_items_reserved = page_info->no_of_items + page_reserved_indices;
	new_page.position = reserve->start();

	unsigned long long page_header_pos = get_page_header_position(page_info);
	file_.write<page_header_on_disk>(new_page, page_header_pos);

	pages_.replace(page_info, new_page);
	reserve->filled();*/
}

unsigned long long indexer::get_page_header_position(page_set::iterator page)
{
	if (page == pages_.begin())
		return header_.first_page_header_pos;
	else
		return std::prev(page)->next_page_header_pos;
}

void indexer::update_page_data_info(std::vector<item>& data, size_t start_element)
{
/*
	//Currently this function should support only adding/removing/updating a single element
	//Bulk operations will be supported with transactions

	auto page_info = find_page_info(data[start_element].id);
	assert(page_info->no_of_items == data.size() || page_info->no_of_items + 1 == data.size() || page_info->no_of_items == data.size() + 1);

	//We assume that if 'data' size did not change we are updating only the single element
	if(data.size() == page_info->no_of_items)
	{
		file_.write(data[start_element], page_info->position + sizeof(index_item) * start_element);
		return;
	}

	//We are adding an element
	if (data.size() > page_info->no_of_items)
	{
		//We have enough space reserved
		if (data.size() <= page_info->no_of_items_reserved)
		{
			file_.write_raw(&data[start_element], page_info->position + sizeof(index_item) * start_element, (data.size() - start_element) * sizeof(index_item));
			return;
		}

		//We do not have enough space reserved. Need to reserve more
		//Try to find blank space after updated entry to shift next entries to it
		std::function<size_t(index_item&)> deref = [](index_item& item) {return item.size; };
		auto it1 = boost::make_transform_iterator(data.begin() + start_element, deref);
		auto it2 = boost::make_transform_iterator(data.end(), deref);
		auto empty = std::find(it1, it2, 0).base();
		if (empty != data.end())
		{
			data.erase(empty);
			file_.write_raw(&data[start_element], page_info->position + sizeof(index_item) + start_element, sizeof(index_item) * (empty - (data.begin() + start_element)));
			return;
		}

		//We don't need to split the page
		if (page_info->no_of_items + page_reserved_indices < max_indices_to_divide)
		{
			update_page_data(page_info, data);
			return;
		}

		//Splitting the page

		//First try to eliminate all blank spaces
		std::vector<index_item> new_data;
		for (auto& item : data)
			if (item.size != 0) new_data.push_back(item);

		//Check if we still need to split the page
		if (new_data.size() <= page_info->no_of_items_reserved)
		{
			//No need, good
			update_page_data(page_info, new_data);
			return;
		}

		//No way to avoid it. Just split the bastard!
		auto start1 = data.cbegin();
		auto end1 = std::next(data.cbegin(), data.size() / 2);
		auto start2 = std::next(end1);
		auto end2 = data.cend();

		auto reserve1 = hm_.reserve(start1, end1);
		auto reserve2 = hm_.reserve(start2, end2);

		index_page_header page_info1, page_info2;
		auto reserve_page2 = hm_.reserve<index_page_header>();
					
		page_info1.maxid = (end1 - 1)->id;
		page_info1.minid = start1->id;
		page_info1.next_index_page = reserve_page2->start();
		page_info1.no_of_items = std::distance(start1, end1);
		page_info1.no_of_items_reserved = page_info1.no_of_items;
		page_info1.position = reserve1->start();

		page_info2.maxid = (end2 - 1)->id;
		page_info2.minid = start2->id;
		page_info2.next_index_page = page_info->next_index_page;
		page_info2.no_of_items = std::distance(start2, end2);
		page_info2.no_of_items_reserved = page_info2.no_of_items;
		page_info2.position = reserve2->start();

		file_.write(page_info2, reserve_page2->start());
		file_.write(start1, end1, reserve1->start());
		file_.write(start2, end2, reserve2->start());

		unsigned long long page1_header_position = get_page_header_position(page_info);

		auto pos_in_pages = pages_.insert(page_info2);
		try
		{
			file_.write(page_info1, page1_header_position);
		}
		catch (...)
		{
			pages_.erase(pos_in_pages.first);
			throw;
		}
		pages_.replace(page_info, page_info1);

		reserve1->filled();
		reserve2->filled();
		reserve_page2->filled();
		return;
	}

	//We are removing an element
	assert(false); //Not supported yet
	*/
}

unsigned long long indexer::get_new_id()
{
	return header_.new_id;
}