#include "pch.h"

#include "win_file.h"

file::file(std::wstring const& file_name)
{
	file_handle_ = CreateFileW(file_name.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, NULL, NULL);
	if (file_handle_ == INVALID_HANDLE_VALUE)
		throw std::system_error(GetLastError(), std::system_category());
}

file::~file()
{
	CloseHandle(file_handle_);
}

void file::read(void* data, size_t start, size_t size)
{
	if (size > static_cast<DWORD>(-1))
		throw std::system_error(EOVERFLOW, std::generic_category(), std::string("Cannot read more than 0xffffffff amount of bytes at once"));

	std::unique_lock<std::mutex> lg(file_lock_);

	LARGE_INTEGER pos;
	pos.QuadPart = start;
	BOOL res = SetFilePointerEx(file_handle_, pos, NULL, FILE_BEGIN);
	if (res == 0)
		throw std::system_error(GetLastError(), std::system_category());

	res = ReadFileEx(file_handle_, data, static_cast<DWORD>(size), NULL, NULL);
	if (res == 0)
		throw std::system_error(GetLastError(), std::system_category());
}

void file::write(void const* data, size_t start, size_t size)
{
	if (size > static_cast<DWORD>(-1))
		throw std::system_error(EOVERFLOW, std::generic_category(), std::string("Cannot write more than 0xffffffff amount of bytes at once"));

	std::unique_lock<std::mutex> lg(file_lock_);

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

void file::flush()
{
	std::unique_lock<std::mutex> lg(file_lock_);

	BOOL res = FlushFileBuffers(file_handle_);
	if (res == 0)
		throw std::system_error(GetLastError(), std::system_category());
}