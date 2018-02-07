#pragma once

#include "precompile.hpp"

namespace ORB_SLAM2
{
namespace experiments
{
class block_info
{
public:
    explicit block_info(const std::string& name);
    ~block_info();

private:
    std::chrono::steady_clock::time_point _begin;
    std::string _name;
}; // class block_info
} // namespace experiments
} // namespace ORB_SLAM2
