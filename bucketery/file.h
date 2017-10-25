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
		read_raw(reinterpret_cast<void*>(&data), start, sizeof(data));
	}

	template <class T>
	void write(T const& data, unsigned long long start)
	{
		write_raw(reinterpret_cast<void const*>(&data), start, sizeof(data));
	}

};