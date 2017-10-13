#pragma once

#include "file.h"

class hole_manager
{
public:
	hole_manager();

	class reservation
	{
		friend hole_manager;

		reservation(reservation const&) = delete;
		void operator=(reservation const&) = delete;
	private:
		reservation(hole_manager& hm, size_t size);

	public:
		~reservation();

		unsigned long long start();
		void filled();

	private:
		hole_manager& hm_;
		unsigned long long start_at_;
		size_t size_;
		bool filled_;
	};

	reservation&& reserve(size_t size);
	unsigned long long  insert(size_t size);
	void remove(unsigned long long start_at, size_t size);

private:
	struct hole
	{
		unsigned long long position;
		size_t size;
	};

	struct by_size {};
	struct by_pos {};

	boost::multi_index::multi_index_container<
		hole,
		boost::multi_index::indexed_by<
			boost::multi_index::ordered_non_unique<boost::multi_index::tag<by_size>, boost::multi_index::member<hole, size_t, &hole::size>>,
			boost::multi_index::ordered_unique<boost::multi_index::tag<by_pos>, boost::multi_index::member<hole, unsigned long long, &hole::position>>
		>
	> holes_;

	unsigned long long last_place_;
};
