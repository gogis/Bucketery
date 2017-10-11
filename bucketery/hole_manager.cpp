#include "pch.h"

#include "hole_manager.h"

hole_manager::hole_manager(file& file_storage) : file_(file_storage), last_place_(0)
{
}

hole_manager::reservation::reservation(hole_manager& hm, size_t size) : hm_(hm), size_(size), filled_(false)
{
	start_at_ = hm_.insert(size_);
}

hole_manager::reservation::~reservation()
{
	if (!filled_)
		hm_.remove(start_at_, size_);
}

void hole_manager::reservation::filled()
{
	assert(!filled_);

	filled_ = true;
}

unsigned long long hole_manager::reservation::start()
{
	return start_at_;
}

unsigned long long hole_manager::insert(size_t size)
{
	auto found_hole = holes_.get<by_size>().lower_bound(size);
	if (found_hole == holes_.get<by_size>().end())
	{
		auto result = last_place_;
		last_place_ += size;
		return result;
	}

	auto result = found_hole->position;
	if (found_hole->size == size)
	{
		holes_.get<by_size>().erase(found_hole);
		return result;
	}

	hole new_hole{ found_hole->position + size, found_hole->size - size };
	holes_.get<by_size>().erase(found_hole);
	holes_.insert(new_hole);
	return result;
}

void hole_manager::remove(unsigned long long start_at, size_t size)
{
}

hole_manager::reservation&& hole_manager::reserve(size_t size)
{
	return std::move(reservation(*this, size));
}
