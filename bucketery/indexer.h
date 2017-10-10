#pragma once

#include "file.h"

class indexer
{
public:
	indexer(file& file_storage);

private:
	file& file_;
};