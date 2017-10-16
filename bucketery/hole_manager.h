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

		class constructor_tag {}; //to be able to create reservation object only within hole_manager

	public:
		reservation(constructor_tag, hole_manager& hm, size_t size);
		~reservation();

		unsigned long long start();
		void filled();

	private:
		hole_manager& hm_;
		unsigned long long start_at_;
		size_t size_;
		bool filled_;
	};

	std::unique_ptr<reservation> reserve(size_t size);
	template <class T> std::unique_ptr<reservation> reserve()
	{
		return reserve(sizeof(T));
	}

	unsigned long long insert(size_t size);
	template <class T> unsigned long long insert()
	{
		return insert(sizeof(T));
	}

	void data_present(unsigned long long pos, size_t size);
	template <class T> void data_present(unsigned long long pos)
	{
		data_present(pos, sizeof(T));
	}

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
