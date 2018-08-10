#pragma once
// We can only support precompilation of boost unit test headers with MSVC++.
// Please use this pattern in each boost unit test framework based test suite.
#ifdef _MSC_VER
#include "orb-slam2-ext_tests.boost.h"
#include <boost/test/unit_test.hpp>
#endif
#include "System.h"
#include <Eigen/Core>
#include <Eigen/StdVector>
#include <opencv2/core.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/noncopyable.hpp>
#include <memory>
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <chrono>
#include <iterator>
#include <iostream>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <algorithm>
#include <queue>
#include <cassert>
#include <typeinfo>