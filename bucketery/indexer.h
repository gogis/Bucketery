#pragma once

#include "file.h"

#include "hole_manager.h"

class indexer
{
public:
	explicit indexer(file& file_storage, hole_manager& hm, std::optional<unsigned long long>& header_pos);

	struct item
	{
		unsigned long long id;
		unsigned long long pos;
		size_t size;
	};

	item get_item(unsigned long long id);
	void set_item(unsigned long long id, unsigned long long pos, size_t size);
	void remove_item(unsigned long long id);

	unsigned long long get_new_id();

private:
	struct header
	{
		unsigned long long first_page_header_pos;
		unsigned long long new_id;
	} header_;

	struct page_header_on_disk
	{
		unsigned long long next_page_header_pos;
		unsigned long long items_pos;
		size_t no_of_items_reserved;
	};

	struct page_header : public page_header_on_disk
	{
		unsigned long long minid;
		size_t no_of_items;
	};

	typedef boost::multi_index_container<
		page_header,
		boost::multi_index::indexed_by<
			boost::multi_index::ordered_unique<boost::multi_index::member<page_header, unsigned long long, &page_header::minid>>
		>
	> page_set;
	page_set pages_;

	unsigned long long get_page_header_position(page_set::iterator page);

	page_set::iterator find_page(unsigned long long id);
	std::vector<item> load_page_items(page_set::iterator page);
	std::vector<item>::iterator find_item(std::vector<item>& data, unsigned long long id);

	void update_page_data(page_set::iterator page_info, std::vector<item>& new_data);
	void update_page_data_info(std::vector<item>& data, size_t start_element);

	page_set::iterator init_load_page(std::vector<item>& vec, unsigned long long pos);

	hole_manager& hm_;
	file& file_;
	unsigned long long header_pos_;

	static item const last_item_in_sequence_;
};