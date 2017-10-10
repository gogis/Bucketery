#pragma once

#include "file.h"

class hole_manager
{
public:
	hole_manager(file& file_storage);

	unsigned long long find_place(size_t size);
	void file_inserted(unsigned long long start_at, size_t size);
	void file_removed(unsigned long long start_at, size_t size);

private:
	file& file_;

	struct hole
	{
		unsigned long long position;
		size_t size;
	};

	struct by_size {};
	struct by_pos {};

	mi::multi_index_container<
		hole,
		mi::indexed_by<
			mi::ordered_non_unique<mi::tag<by_size>, mi::member<hole, size_t, &hole::size>>,
			mi::ordered_unique<mi::tag<by_pos>, mi::member<hole, unsigned long long, &hole::position>>
		>
	> holes_;

	unsigned long long last_place_;
};
