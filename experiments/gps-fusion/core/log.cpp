#include "precompile.hpp"

#include "log.hpp"

namespace ORB_SLAM2
{
namespace ch = std::chrono;
namespace experiments
{
block_info::block_info(const std::string& name) : _name(name)
{
    _begin = ch::steady_clock::now();
}

block_info::~block_info()
{
    auto end = ch::steady_clock::now();
    auto time_taken = ch::duration_cast<ch::milliseconds>(end - _begin).count();
    std::cout << _name << ':' << time_taken << " ms\n";
}
} // namespace experiments
} // namespace ORB_SLAM2
