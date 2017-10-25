#pragma once

#include "file.h"

#include "hole_manager.h"

class indexer
{
public:
	explicit indexer(file& file_storage, hole_manager& hm, std::optional<unsigned long long>& index_header_pos);

	struct index_item
	{
		unsigned long long id;
		unsigned long long pos;
		size_t size;
	};

	void set_item(unsigned long long id, unsigned long long pos, size_t size);
	void remove_item(unsigned long long id);
	index_item get_item(unsigned long long id);

private:
	struct index_header
	{
		unsigned long long first_index_page;
		unsigned long long last_used_id;
	} header_;

	struct index_page_header
	{
		unsigned long long minid;
		unsigned long long maxid;
		unsigned long long next_index_page;
		unsigned long long position;
		size_t no_of_items;
		size_t no_of_items_reserved;
	};

	typedef boost::multi_index_container<
		index_page_header,
		boost::multi_index::indexed_by<
			boost::multi_index::ordered_unique<boost::multi_index::member<index_page_header, unsigned long long, &index_page_header::minid>>
		>
	> page_set;
	page_set pages_;

	page_set::iterator find_page_info(unsigned long long id);
	std::vector<index_item> load_page_data_info(page_set::iterator page);
	std::vector<index_item>::iterator find_item(std::vector<index_item>& data, unsigned long long id);

//	void update_page_info(page_set::iterator page_info);
	unsigned long long get_page_header_position(page_set::iterator page);
	void update_page_data_info(std::vector<index_item>& data, size_t start_element);

	void get_page_data_placement(std::vector<index_item> vec, index_page_header& iph);

	hole_manager& hm_;
	file& file_;
};