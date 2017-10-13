#include "pch.h"

#include "hole_manager.h"

hole_manager::hole_manager() : last_place_(0)
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
	assert(start_at < last_place_);
	assert(holes_.get<by_pos>().lower_bound(start_at) == holes_.get<by_pos>().end()
		|| holes_.get<by_pos>().lower_bound(start_at)->position >= start_at + size); //There are some holes in specified region

	bool is_prev = false;
	bool is_next = false;

	auto found_hole = holes_.get<by_pos>().lower_bound(start_at + size);
	decltype(found_hole) prev_hole, next_hole;
	if (found_hole != holes_.get<by_pos>().begin())
	{
		prev_hole = std::prev(found_hole);
		if (prev_hole->position + prev_hole->size == start_at)
			is_prev = true;
	}

	if (found_hole != holes_.get<by_pos>().end())
	{
		if (found_hole->position == start_at + size)
		{
			is_next = true;
			next_hole = found_hole;
		}
	}

	if (!is_next && last_place_ == start_at + size)
	{
		if (is_prev)
		{
			last_place_ = prev_hole->position;
			holes_.get<by_pos>().erase(prev_hole);
		}
		else
			last_place_ = start_at;
		return;
	}

	if (is_next && is_prev)
	{
		hole new_hole{ prev_hole->position, prev_hole->size + size + next_hole->size };
		holes_.get<by_pos>().erase(prev_hole);
		holes_.get<by_pos>().erase(next_hole);
		holes_.insert(new_hole);
	}

	if (is_next && !is_prev)
	{
		hole new_hole{ start_at, size + next_hole->size };
		holes_.get<by_pos>().erase(next_hole);
		holes_.insert(new_hole);
	}

	if (!is_next && is_prev)
	{
		hole new_hole{ prev_hole->position, prev_hole->size + size };
		holes_.get<by_pos>().erase(prev_hole);
		holes_.insert(new_hole);
	}

	if (!is_next && !is_prev)
	{
		hole new_hole{ start_at, size };
		holes_.insert(new_hole);
	}
}

hole_manager::reservation&& hole_manager::reserve(size_t size)
{
	return std::move(reservation(*this, size));
}
