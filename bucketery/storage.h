#pragma once

#include "file.h"
#include "indexer.h"
#include "hole_manager.h"

class storage
{
public:
	explicit storage(std::wstring const& file_name);
	~storage();

	unsigned long long save_data(void const* data, size_t size);
	void save_data(unsigned long long data_id, void const* data, size_t size);
	void load_data(unsigned long long data_id, void* data, size_t size, size_t start_at = 0);

	template <class T>
	unsigned long long save_data(T const& data)
	{
		static_assert(std::is_pod<T>::value); //Only POD supported
		return save_data(reinterpret_cast<void const*>(&data), sizeof(data));
	}

	template<> unsigned long long save_data(std::string const& data)
	{
		return save_data(data.c_str(), data.size());
	}

	template <class T>
	void save_data(unsigned long long data_id, T const& data)
	{
		static_assert(std::is_pod<T>::value); //Only POD supported
		save_data(data_id, reinterpret_cast<void const*>(&data), sizeof(data));
	}

	template<>
	void save_data(unsigned long long data_id, std::string const& data)
	{
		save_data(data_id, data.c_str(), data.size());
	}

	template <class T>
	void load_data(unsigned long long data_id, T& data)
	{
		static_assert(std::is_pod<T>::value); //Only POD supported
		load_data(data_id, reinterpret_cast<void*>(&data), sizeof(data));
	}

	template<> void storage::load_data(unsigned long long data_id, std::string& data)
	{
		std::vector<char> vdata;
		size_t size = get_data_size(data_id);
		vdata.resize(size);
		load_data(data_id, &vdata[0], size);
		data.assign(vdata.begin(), vdata.end());
	}

	size_t get_data_size(unsigned long long data_id);
	void delete_data(unsigned long long data_id);

private:
	file file_;
	hole_manager hm_;
	std::unique_ptr<indexer> index_;
};


