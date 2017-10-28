#pragma once

#define NOMINMAX

#include <string>
#include <mutex>
#include <map>
#include <set>
#include <optional>
#include <iterator>
#include <type_traits>
#include <sstream>
#include <memory>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/iterator/transform_iterator.hpp>

#ifdef _WIN32
#include "win.h"
#endif

namespace mi = boost::multi_index;