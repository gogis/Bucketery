#include "pch.h"

#include "indexer.h"

#include "constants.h"

indexer::indexer(file& file_storage, hole_manager& hm, std::optional<unsigned long long>& index_header_pos) : file_(file_storage), hm_(hm)
{
	if (!index_header_pos)
	{
		index_item data_item;
		data_item.id = 1;
		data_item.pos = 0;
		data_item.size = 0;

		auto di_pos = hm_.insert<index_item>();
		file_.write(data_item, di_pos);

		index_page_header page_header;
		page_header.minid = 1;
		page_header.maxid = 1;
		page_header.next_index_page = 0;
		page_header.position = di_pos;
		page_header.no_of_items = 1;

		auto iph_pos = hm_.insert<index_page_header>();
		file_.write(page_header, iph_pos);
		pages_.insert(page_header);

		header_.first_index_page = iph_pos;
		header_.last_used_id = 0;

		auto ih_pos = hm_.insert<index_header>();
		file_.write(header_, ih_pos);
		index_header_pos = ih_pos;

		file_.flush();
	}
	else
	{
		hm_.data_present<index_header>(*index_header_pos);
		file_.read(header_, *index_header_pos);

		std::vector<index_item> items_data;

		index_page_header page_header;
		hm_.data_present<index_page_header>(header_.first_index_page);
		file_.read(page_header, header_.first_index_page);
		pages_.insert(page_header);
		hm_.data_present(page_header.position, sizeof(index_item) * page_header.no_of_items);
		get_page_data_placement(items_data, page_header);

		while (page_header.next_index_page != 0)
		{
			hm_.data_present<index_page_header>(page_header.next_index_page);
			file_.read(page_header, page_header.next_index_page);
			pages_.insert(page_header);
			hm_.data_present(page_header.position, sizeof(index_item) * page_header.no_of_items);
			get_page_data_placement(items_data, page_header);
		}
	}
}

//items_data - is just a reading buffer. It is used only to avoid reallocation
void indexer::get_page_data_placement(std::vector<index_item> items_data, index_page_header& iph)
{
	if (iph.no_of_items == 0)
		return;

	if(items_data.size() < iph.no_of_items)
		items_data.resize(iph.no_of_items);

	file_.read_raw(&items_data[0], iph.position, sizeof(index_item) * iph.no_of_items);

	for (auto& item : items_data)
		hm_.data_present(item.pos, item.size);
}

indexer::page_set::iterator indexer::find_page(unsigned long long id)
{
	assert(id != 0);

	auto found_page = --pages_.upper_bound(id);
	return found_page;
}

std::vector<indexer::index_item> indexer::load_page(indexer::page_set::iterator page)
{
	std::vector<index_item> page_data;
	page_data.resize(page->no_of_items);

	file_.read_raw(&page_data[0], page->position, sizeof(index_item) * page->no_of_items);
	return page_data;
}

std::vector<indexer::index_item>::iterator indexer::find_item(std::vector<indexer::index_item>& data, unsigned long long id)
{
	std::function<unsigned long long(index_item&)> deref = [](index_item& item) {return item.id; };
	auto it1 = boost::make_transform_iterator(data.begin(), deref);
	auto it2 = boost::make_transform_iterator(data.end(), deref);
	return std::lower_bound(it1, it2, id).base();
}

void indexer::set_item(unsigned long long id, unsigned long long /*pos*/, size_t /*size*/)
{
	auto page_info = find_page(id);
	auto page_data = load_page(page_info);
	auto item = find_item(page_data, id);

	if (item->id == id)
	{
	}
	else
	{
	}
}

void indexer::remove_item(unsigned long long /*id*/)
{
}

indexer::index_item indexer::get_item(unsigned long long /*id*/)
{
	return index_item();
}
