#pragma once

class file
{
public:
	file(std::wstring const& file_name);
	~file();

	void read(void* data, size_t start, size_t size);
	void write(void const* data, size_t start, size_t size);

	void flush();

private:
	HANDLE file_handle_;
	std::mutex file_lock_;
};