#pragma once

// We can only support precompilation of boost unit test headers with MSVC++.
// Please use this pattern in each boost unit test framework based test suite.
#define BOOST_TEST_MODULE tests.boost.orb-slam2-ext
