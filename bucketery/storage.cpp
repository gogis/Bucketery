#include "pch.h"

#include "storage.h"

#include "constants.h"

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
			throw std::runtime_error("Invalid data file specified");

		if (header.version_maj != ver_maj || header.version_min != ver_min)
			throw std::runtime_error("Invalid data file version");

		std::optional<unsigned long long> index_header_pos = header.index_header_pos;
		index_ = std::make_unique<indexer>(file_, hm_, index_header_pos);
	}
}

storage::~storage()
{
}

unsigned long long storage::save_data(void const* /*data*/, size_t /*size*/)
{
	return 0;
}

void storage::save_data(unsigned long long /*data_id*/, void const* /*data*/, size_t /*size*/)
{
}

void storage::load_data(unsigned long long /*data_id*/, void* /*data*/, size_t /*size*/, size_t /*start_at*/)
{
}

size_t storage::get_data_size(unsigned long long /*data_id*/)
{
	return 0;
}

void storage::delete_data(unsigned long long /*data_id*/)
{
}
