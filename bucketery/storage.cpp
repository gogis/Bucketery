#include "pch.h"

#include "storage.h"

#include "constants.h"
#include "errors.h"


storage::storage(std::wstring const& file_name) : file_(file_name)
{
	struct storage_header
	{
		wchar_t sign[4];
		unsigned char version_maj;
		unsigned char version_min;
		unsigned long long index_header_pos;
	}header;

	hm_.data_present<storage_header>(0);

	if (file_.get_size() == 0)
	{
		header.sign[0] = L'B';
		header.sign[1] = L'u';
		header.sign[2] = L'c';
		header.sign[3] = L'k';
		header.version_maj = ver_maj;
		header.version_min = ver_min;

		std::optional<unsigned long long> index_header_pos;
		index_ = std::make_unique<indexer>(file_, hm_, index_header_pos);

		header.index_header_pos = *index_header_pos;

		file_.write(header, 0);
	}
	else
	{
		file_.read(header, 0);
		if (header.sign[0] != L'B' || header.sign[1] != L'u' || header.sign[2] != L'c' || header.sign[3] != L'k')
			throw invalid_data_file();

		if (header.version_maj != ver_maj || header.version_min != ver_min)
			throw invalid_data_file_version();

		std::optional<unsigned long long> index_header_pos = header.index_header_pos;
		index_ = std::make_unique<indexer>(file_, hm_, index_header_pos);
	}
}

storage::~storage()
{
}

unsigned long long storage::save_data(void const* data, size_t size)
{
	auto id = index_->get_new_id();
	save_data(id, data, size);
	return id;
}

void storage::save_data(unsigned long long data_id, void const* data, size_t size)
{
	auto reserve = hm_.reserve(size);
	file_.write_raw(data, reserve->start(), size);
	index_->set_item(data_id, reserve->start(), size);
	reserve->filled();
	file_.flush();
}

void storage::load_data(unsigned long long data_id, void* data, size_t size, size_t start_at)
{
	if (size == 0)
		return;

	auto item_info = index_->get_item(data_id);

	if (start_at > item_info.size || item_info.size - start_at < size)
		throw not_enough_data();

	file_.read_raw(data, item_info.pos + start_at, size);
}

size_t storage::get_data_size(unsigned long long data_id)
{
	return index_->get_item(data_id).size;
}

void storage::delete_data(unsigned long long data_id)
{
	index_->remove_item(data_id);
}
