#include "pch.h"

#include "win_file.h"

#ifdef _WIN32

system_file::system_file(std::wstring const& file_name)
{
	file_handle_ = CreateFileW(file_name.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, NULL, NULL);
	if (file_handle_ == INVALID_HANDLE_VALUE)
		throw std::system_error(GetLastError(), std::system_category());
}

system_file::~system_file()
{
	CloseHandle(file_handle_);
}

void system_file::read_raw(void* data, unsigned long long start, size_t size)
{
	if (size > static_cast<DWORD>(-1))
		throw std::system_error(EOVERFLOW, std::generic_category(), std::string("Cannot read more than 0xffffffff amount of bytes at once"));

	LARGE_INTEGER pos;
	pos.QuadPart = start;
	BOOL res = SetFilePointerEx(file_handle_, pos, NULL, FILE_BEGIN);
	if (res == 0)
		throw std::system_error(GetLastError(), std::system_category());

	res = ReadFileEx(file_handle_, data, static_cast<DWORD>(size), NULL, NULL);
	if (res == 0)
		throw std::system_error(GetLastError(), std::system_category());
}

void system_file::write_raw(void const* data, unsigned long long start, size_t size)
{
	if (size > static_cast<DWORD>(-1))
		throw std::system_error(EOVERFLOW, std::generic_category(), std::string("Cannot write more than 0xffffffff amount of bytes at once"));

	LARGE_INTEGER pos;
	pos.QuadPart = start;
	BOOL res = SetFilePointerEx(file_handle_, pos, NULL, FILE_BEGIN);
	if (res == 0)
		throw std::system_error(GetLastError(), std::system_category());

	DWORD useless;
	res = WriteFile(file_handle_, data, static_cast<DWORD>(size), &useless, NULL);
	if (res == 0)
		throw std::system_error(GetLastError(), std::system_category());
}

void system_file::flush()
{
	BOOL res = FlushFileBuffers(file_handle_);
	if (res == 0)
		throw std::system_error(GetLastError(), std::system_category());
}

unsigned long long system_file::get_size()
{
	return 0;
}

#endif