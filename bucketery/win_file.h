#pragma once

class system_file
{
protected:
	explicit system_file(std::wstring const& file_name);

public:
	~system_file();

	void read_raw(void* data, unsigned long long start, size_t size);
	void write_raw(void const* data, unsigned long long start, size_t size);

	unsigned long long get_size();

	void flush();

private:
	HANDLE file_handle_;
};