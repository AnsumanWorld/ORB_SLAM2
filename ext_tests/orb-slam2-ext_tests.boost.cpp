#include "stdafx.h"
// We can only support pre-compilation of boost unit test headers with MSVC++.
// Please use this pattern in each boost unit test framework based test suite
#ifndef _MSC_VER
#include "orb-slam2-ext_tests.boost.h" // This include directive may appear in only one TU! 
#include <boost/test/unit_test.hpp>
#endif