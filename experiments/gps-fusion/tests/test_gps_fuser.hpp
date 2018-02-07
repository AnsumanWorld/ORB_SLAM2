#pragma once

#include "precompile.hpp"

namespace ORB_SLAM2
{
namespace experiments
{
namespace tests
{
class test_gps_fuser : public ::testing::Test
{
public:
    test_gps_fuser();
    ~test_gps_fuser();
    void SetUp() override;
    void TearDown() override;
    void test_construct_graph_unary();
    void test_construct_graph_binary_dist();
    void test_construct_graph_xy_prior_dc();
    void test_read_pose_tum();

private:
    std::string _json_data;
};
} // namespace tests
} // namespace experiments
} // namespace ORB_SLAM2
