#pragma once

#include "precompile.hpp"

#include "log.hpp"
#include "utils/utils.hpp"

#ifndef __FILENAME__
#ifdef _WIN32
#define __FILENAME__ \
    (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif /*WIN32*/
#endif /*__FILENAME__*/

#define CALL_SITE                                     \
    std::string(std::string(__FILENAME__) \
+ "(" + std::to_string(__LINE__) +                    \
                ")" \
+ ":" + ORB_SLAM2::experiments::utils::tid_to_string( \
                    std::this_thread::get_id()) +     \
                \
+":" + std::string(__FUNCTION__) +                    \
                ":")
#define LOG_SCOPE ORB_SLAM2::experiments::block_info __block_info__(CALL_SITE)

#define LOG_MSG std::cout << CALL_SITE
#define LOG_WARNING std::cout << CALL_SITE
#define LOG_ERROR std::cerr << CALL_SITE
