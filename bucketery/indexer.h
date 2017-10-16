#pragma once

#include "file.h"

#include "hole_manager.h"

class indexer
{
public:
	indexer(file& file_storage, hole_manager& hm, std::optional<unsigned long long>& index_header_pos);

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

		bool operator()(index_page_header const& l, index_page_header const& r);//comparator for std::set
	};

	void get_page_data_placement(std::vector<index_item> vec, index_page_header& iph);

private:
	std::set<index_page_header, index_page_header> pages_;

	hole_manager& hm_;
	file& file_;
};