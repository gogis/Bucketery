#pragma once

//#include <cstdio>
#include <string>
#include <mutex>
#include <map>
#include <set>
#include <optional>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#ifdef _WIN32
#include "win.h"
#endif

namespace mi = boost::multi_index;