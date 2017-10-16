#include "pch.h"

#include "hole_manager.h"

#include "constants.h"

hole_manager::hole_manager() : last_place_(0)
{
}

hole_manager::reservation::reservation(constructor_tag, hole_manager& hm, size_t size) : hm_(hm), size_(size), filled_(false)
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

void hole_manager::data_present(unsigned long long pos, size_t size)
{
	if (size == 0)
		return;

	//Apply padding
	size_t actual_size = size % file_padding > 0 ? (size / file_padding + 1) * file_padding : size;

	if (pos == last_place_)
	{
		last_place_ += actual_size;
		return;
	}

	if (pos > last_place_)
	{
		hole new_hole{ last_place_, pos - last_place_};
		holes_.insert(new_hole);
		last_place_ = pos + actual_size;
		return;
	};

	auto found_hole = holes_.get<by_pos>().lower_bound(pos);
	hole found_hole_data = *found_hole;

	if (found_hole != holes_.get<by_pos>().end() && found_hole->position == pos)
	{
		assert(found_hole->size >= actual_size); //Hole in the specified place is too small

		if (found_hole->size == actual_size)
		{
			holes_.get<by_pos>().erase(found_hole);
			return;
		}

		hole new_hole{ pos + actual_size, found_hole->size - actual_size };
		holes_.get<by_pos>().erase(found_hole);
		holes_.insert(new_hole);
		return;
	}

	assert(found_hole != holes_.get<by_pos>().begin()); //There is no hole in that data region

	--found_hole;
	found_hole_data = *found_hole;

	assert(found_hole->position <= pos && found_hole->position + found_hole->size >= pos + actual_size); //There is no hole in that region

	hole new_left_hole{ found_hole->position, pos - found_hole->position };
	std::optional<hole> new_right_hole;

	if (pos + actual_size < found_hole->position + found_hole->size)
		new_right_hole = { pos + actual_size, found_hole->position + found_hole->size - pos - actual_size };

	holes_.get<by_pos>().erase(found_hole);
	holes_.insert(new_left_hole);
	if (new_right_hole)
		holes_.insert(*new_right_hole);
}

unsigned long long hole_manager::insert(size_t size)
{
	if (size == 0)
		return 0;

	//Apply padding
	size_t actual_size = size % file_padding > 0 ? (size / file_padding + 1) * file_padding : size;

	auto found_hole = holes_.get<by_size>().lower_bound(actual_size);
	if (found_hole == holes_.get<by_size>().end())
	{
		auto result = last_place_;
		last_place_ += actual_size;
		return result;
	}

	auto result = found_hole->position;
	if (found_hole->size == actual_size)
	{
		holes_.get<by_size>().erase(found_hole);
		return result;
	}

	hole new_hole{ found_hole->position + actual_size, found_hole->size - actual_size };
	holes_.get<by_size>().erase(found_hole);
	holes_.insert(new_hole);
	return result;
}

void hole_manager::remove(unsigned long long start_at, size_t size)
{
	if (size == 0)
		return;

	//Apply padding
	size_t actual_size = size % file_padding > 0 ? (size / file_padding + 1) * file_padding : size;

	assert(start_at < last_place_);
	assert(holes_.get<by_pos>().lower_bound(start_at) == holes_.get<by_pos>().end()
		|| holes_.get<by_pos>().lower_bound(start_at)->position >= start_at + actual_size); //There are some holes in specified region

	bool is_prev = false;
	bool is_next = false;

	auto found_hole = holes_.get<by_pos>().lower_bound(start_at + actual_size);
	decltype(found_hole) prev_hole, next_hole;
	if (found_hole != holes_.get<by_pos>().begin())
	{
		prev_hole = std::prev(found_hole);
		if (prev_hole->position + prev_hole->size == start_at)
			is_prev = true;
	}

	if (found_hole != holes_.get<by_pos>().end())
	{
		if (found_hole->position == start_at + actual_size)
		{
			is_next = true;
			next_hole = found_hole;
		}
	}

	if (!is_next && last_place_ == start_at + actual_size)
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
		hole new_hole{ prev_hole->position, prev_hole->size + actual_size + next_hole->size };
		holes_.get<by_pos>().erase(prev_hole);
		holes_.get<by_pos>().erase(next_hole);
		holes_.insert(new_hole);
	}

	if (is_next && !is_prev)
	{
		hole new_hole{ start_at, actual_size + next_hole->size };
		holes_.get<by_pos>().erase(next_hole);
		holes_.insert(new_hole);
	}

	if (!is_next && is_prev)
	{
		hole new_hole{ prev_hole->position, prev_hole->size + actual_size };
		holes_.get<by_pos>().erase(prev_hole);
		holes_.insert(new_hole);
	}

	if (!is_next && !is_prev)
	{
		hole new_hole{ start_at, actual_size };
		holes_.insert(new_hole);
	}
}

std::unique_ptr<hole_manager::reservation> hole_manager::reserve(size_t size)
{
	return std::make_unique<hole_manager::reservation>(hole_manager::reservation::constructor_tag{}, *this, size);
}
