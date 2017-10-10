#include "pch.h"

#include "hole_manager.h"

hole_manager::hole_manager(file& file_storage) : file_(file_storage), last_place_(0)
{
}

unsigned long long hole_manager::find_place(size_t size)
{
	auto hole_it = holes_.get<by_size>().lower_bound(size);
	if (hole_it == holes_.get<by_size>().end())
		return last_place_;
	else
		return hole_it->position;
}

void hole_manager::file_inserted(unsigned long long start_at, size_t size)
{
}

void hole_manager::file_removed(unsigned long long start_at, size_t size)
{
}