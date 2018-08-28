#pragma once

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#ifndef __FILENAME__
#ifdef _WIN32
#define __FILENAME__ \
    (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif /*WIN32*/
#endif /*__FILENAME__*/

#define MSG_STREAM std::cout
#define WARNING_STREAM std::cerr
#define ERROR_STREAM std::cerr

namespace log_util {
    namespace ch = std::chrono;
    class block_info {
        public:
            explicit block_info(const std::string& name): _name(name) {
                _begin = ch::steady_clock::now();
            }
            ~block_info() {
                auto end = ch::steady_clock::now();
                auto time_taken = ch::duration_cast<ch::milliseconds>(end - _begin).count();
                MSG_STREAM << _name << ':' << time_taken << " ms\n";
            }

        private:
            std::chrono::steady_clock::time_point _begin;
            std::string _name;
    }; // class block_info

    std::string tid_to_string(const std::thread::id& tid) {
        std::stringstream ss;
        ss << tid;
        return ss.str();
    }
}

#define CALL_SITE                                                            \
    std::string(std::string(__FILENAME__) + "(" + std::to_string(__LINE__) + \
                ")" + ":" +                                                  \
                log_util::tid_to_string(std::this_thread::get_id()) +        \
                +":" + std::string(__FUNCTION__) + ":")

#define LOG_SCOPE log_util::block_info __block_info__(CALL_SITE)
#define LOG_MSG MSG_STREAM << CALL_SITE
#define LOG_WARNING WARNING_STREAM << CALL_SITE
#define LOG_ERROR ERROR_STREAM << CALL_SITE
