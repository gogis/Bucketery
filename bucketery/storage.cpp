#include "pch.h"

#include "storage.h"

storage::storage(std::wstring const& file_name) : file_(file_name)
{
}

storage::~storage()
{
}

unsigned long long storage::save_data(void const* data, size_t size)
{
	return 0;
}

void storage::save_data(unsigned long long data_id, void const* data, size_t size)
{
}

void storage::load_data(unsigned long long data_id, void* data, size_t size, size_t start_at)
{
}

size_t storage::get_data_size(unsigned long long data_id)
{
	return 0;
}

void storage::delete_data(unsigned long long data_id)
{
}
