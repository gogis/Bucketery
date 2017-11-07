#pragma once

struct data_not_present : public std::runtime_error
{
	data_not_present(unsigned long long id) : std::runtime_error(dynamic_cast<std::stringstream&>(std::stringstream() << "The data with id " << id << " does not exist").str()){}
};

struct invalid_data_file : public std::runtime_error
{
	invalid_data_file() : std::runtime_error("Invalid data file specified"){}
};

struct invalid_data_file_version : public std::runtime_error
{
	invalid_data_file_version() : std::runtime_error("Invalid data file version") {}
};

struct not_enough_data : public std::runtime_error
{
	not_enough_data() : std::runtime_error("More data requested than present") {}
};

struct data_file_corrupted : public std::runtime_error
{
	data_file_corrupted(): std::runtime_error("Data file is corrupted") {}
};

struct buffer_not_large_enough : public std::runtime_error
{
	buffer_not_large_enough() : std::runtime_error("Provided buffer is smaller than the size of data requested") {}
};