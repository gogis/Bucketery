#pragma once

#include "file.h"

class hole_manager
{
public:
	hole_manager(file& file_storage);

	unsigned long long find_place(size_t size);
	void file_inserted(unsigned long long start_at, size_t size);
	void file_removed(unsigned long long strat_at, size_t size);

private:
	file& file_;

	struct hole
	{
		unsigned long long start;
		size_t size;
	};

	std::multimap<size_t, unsigned long long> hole_; //1 - size of the hole, 2 - starting position in the file
};
