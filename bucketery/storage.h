#pragma once

#include "file.h"

class storage
{
public:
	storage(std::wstring const& file_name);
	~storage();

	unsigned long long save_data(void const* data, size_t size);
	void save_data(unsigned long long data_id, void const* data, size_t size);
	void load_data(void* data, size_t size, size_t start_at = 0);

	size_t get_data_size(unsigned long long data_id);
	void delete_data(unsigned long long data_id);

private:
	file file_;
};