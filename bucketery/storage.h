#pragma once

#include "file.h"

class storage
{
public:
	storage(std::wstring const& file_name);
	~storage();

	unsigned long long save_data(void const* data, size_t size);
	void save_data(unsigned long long data_id, void const* data, size_t size);
	void load_data(unsigned long long data_id, void* data, size_t size, size_t start_at = 0);

	template <class T>
	unsigned long long save_data(T const& data)
	{
		return save_data(reinterpret_cast<void const*>(&data), sizeof(data));
	}

	template <class T>
	void save_data(unsigned long long data_id, T const& data)
	{
		save_data(data_id, reinterpret_cast<void const*>(&data), sizeof(data));
	}

	template <class T>
	void load_data(unsigned long long data_id, T& data)
	{
		load_data(data_id, reinterpret_cast<void*>(&data), sizeof(data));
	}

	size_t get_data_size(unsigned long long data_id);
	void delete_data(unsigned long long data_id);

private:
	file file_;
};