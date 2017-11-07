#pragma once

#ifdef _WIN32
#include "win_file.h"
#endif

class file : public system_file
{
public:
	explicit file(std::wstring const& file_name) : system_file(file_name) {}

	template <class T>
	void read(T& data, unsigned long long start)
	{
		read_raw(reinterpret_cast<void*>(&data), start, sizeof(T));
	}

	template <class T>
	void read(std::vector<T>& data, unsigned long long start, size_t elements)
	{
		if (elements > data.size())
			data.resize(elements);
		read_raw(reinterpret_cast<void*>(&data[0]), start, sizeof(T) * elements);
	}

	template <class T>
	void read(std::vector<T>& data, unsigned long long start)
	{
		read(data, start, data.size());
	}

	template <class T>
	void write(T const& data, unsigned long long start)
	{
		write_raw(reinterpret_cast<void const*>(&data), start, sizeof(data));
	}

	template <class T>
	void write(std::vector<T> const& data, unsigned long long start, size_t elements)
	{
		if (elements > data.size())
			throw buffer_not_large_enough();
		write_raw(reinterpret_cast<void const*>(&data[0]), start, sizeof(T) * elements);
	}

	template <class T>
	void write(std::vector<T> const& data, unsigned long long start)
	{
		write(data, start, data.size());
	}

	template<class It, typename = std::enable_if<std::is_same<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>::value, int>::type>
	void write(It it1, It it2, unsigned long long start)
	{
		assert(&*it2 - &*it1 == it2 - it1); //Make sure iterators are from contiguous container
		write_raw(reinterpret_cast<void const*>(&*it1), start, sizeof(It::value_type) * std::distance(it1, it2));
	}
};