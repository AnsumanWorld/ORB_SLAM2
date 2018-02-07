#include "precompile.hpp"

#include "gps_fuser.hpp"
#include "test_gps_fuser.hpp"

namespace ORB_SLAM2
{
namespace experiments
{
namespace tests
{
test_gps_fuser::test_gps_fuser()
{
    _json_data = R"(
        {
            "gps_fusion": {
                "after_g2o_file": "after-fusion.g2o",
                "before_g2o_file": "before-fusion.g2o",
                "fix_initial_vertex": false,
                "fuse_option": "xy",
                "huber_threshold": 2.447652,
                "input_trajectory_file": "experiments/gps-fusion/data/every-frame-gps.txt",
                "output_trajectory_file": "KeyFrameTrajectoryOptimized.txt"
            },
            "gps_ba": {
                "add_vo_edge": true,
                "after_g2o_file": "after-ba.g2o",
                "before_g2o_file": "before-ba.g2o",
                "cam_after_file": "cam-after.txt",
                "cam_before_file": "cam-before.txt",
                "fuse_option": "gps_dist",
                "gps_frame_frequency" : 1,
                "gps_offset_x": 1.0,
                "gps_offset_y": 1.05,
                "gps_position_file": "gps.txt",
                "input_g2o_file": "experiments/gps-fusion/data/before-lba-119.g2o"
            },
            "calculate_variance": false,
            "log_seperator": " ",
            "optimizer_iterations": 10,
            "info_mat_gps_dist": 0.0,
            "info_mat_gps_xy": [
                [0.0001, 0],
                [0, 0.0001]
            ],
            "info_mat_vo_xy": [
                [1.001, 0],
                [0, 1.001]
            ],
            "info_mat_gps_se3": [
                [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
            ],
            "info_mat_vo_se3": [
                [1.001, 0.0, 0.0, 0.0, 0.0, 0.0],
                [0.0, 1.001, 0.0, 0.0, 0.0, 0.0],
                [0.0, 0.0, 1.001, 0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 1.001, 0.0, 0.0],
                [0.0, 0.0, 0.0, 0.0, 1.001, 0.0],
                [0.0, 0.0, 0.0, 0.0, 0.0, 1.001]
            ]
        }
    )";
}

test_gps_fuser::~test_gps_fuser()
{
}

void test_gps_fuser::SetUp()
{
}

void test_gps_fuser::TearDown()
{
}

void test_gps_fuser::test_read_pose_tum()
{
    using namespace ORB_SLAM2::experiments;

    std::stringstream cfg_stream(_json_data);
    config cfg(cfg_stream);
    gps_fuser fuser_(cfg);
    std::string p1("9999 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0");
    std::string p2("9999 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0");
    std::stringstream ss;
    ss << p1 << std::endl;
    ss << p2 << std::endl;
    fuser_.read_pose_tum(ss);
    EXPECT_EQ(2, fuser_._poses.size());
}

void test_gps_fuser::test_construct_graph_unary()
{
    using namespace ORB_SLAM2::experiments;

    std::stringstream cfg_stream(_json_data);
    config cfg(cfg_stream);

    std::string p1("9999 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0");
    std::string p2("9999 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0");
    std::string p3("9999 1.0 2.0 3.0 4.0 5.0 6.0 7.0 0.0 0.0");
    {
        std::stringstream ss;
        ss << p1 << std::endl;
        ss << p2 << std::endl;

        gps_fuser fuser_(cfg);
        fuser_.read_pose_tum(ss);
        fuser_.construct_graph_xy();
        auto v_count = fuser_._optimizer.vertices().size();
        auto e_count = fuser_._optimizer.edges().size();
        EXPECT_EQ(v_count, 2);
        EXPECT_EQ(e_count, 3);
    }

    {
        std::stringstream ss;
        ss << p1 << std::endl;
        gps_fuser fuser_(cfg);
        fuser_.read_pose_tum(ss);
        fuser_.construct_graph_xy();
        auto v_count = fuser_._optimizer.vertices().size();
        auto e_count = fuser_._optimizer.edges().size();
        EXPECT_EQ(v_count, 1);
        EXPECT_EQ(e_count, 1);
    }

    {
        std::stringstream ss;
        ss << p3 << std::endl;
        gps_fuser fuser_(cfg);
        fuser_.read_pose_tum(ss);
        fuser_.construct_graph_xy();
        auto v_count = fuser_._optimizer.vertices().size();
        auto e_count = fuser_._optimizer.edges().size();
        EXPECT_EQ(v_count, 1);
        EXPECT_EQ(e_count, 0);
    }
}

void test_gps_fuser::test_construct_graph_binary_dist()
{
    using namespace ORB_SLAM2::experiments;

    std::stringstream cfg_stream(_json_data);
    config cfg(cfg_stream);

    std::string p1("9999 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0");
    std::string p2("9999 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0");
    std::string p3("9999 1.0 2.0 3.0 4.0 5.0 6.0 7.0 0.0 0.0");
    {
        std::stringstream ss;
        ss << p1 << std::endl;
        ss << p2 << std::endl;
        ss << p3 << std::endl;
        gps_fuser fuser_(cfg);
        fuser_.read_pose_tum(ss);
        fuser_.construct_graph_xy_dc();
        auto v_count = fuser_._optimizer.vertices().size();
        auto e_count = fuser_._optimizer.edges().size();
        EXPECT_EQ(v_count, 3);
        EXPECT_EQ(e_count, 3);
    }
}

void test_gps_fuser::test_construct_graph_xy_prior_dc()
{
    std::stringstream cfg_stream(_json_data);
    config cfg(cfg_stream);

    std::string p1("9999 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0");
    std::string p2("9999 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0");
    std::string p3("9999 1.0 2.0 3.0 4.0 5.0 6.0 7.0 0.0 0.0");
    {
        std::stringstream ss;
        ss << p1 << std::endl;
        ss << p2 << std::endl;
        ss << p3 << std::endl;
        gps_fuser fuser_(cfg);
        fuser_.read_pose_tum(ss);
        fuser_.construct_graph_xy_prior_dc();
        auto v_count = fuser_._optimizer.vertices().size();
        auto e_count = fuser_._optimizer.edges().size();
        EXPECT_EQ(v_count, 3);
        EXPECT_EQ(e_count, 5);
    }
}

TEST_F(test_gps_fuser, test_construct_graph_unary)
{
    test_construct_graph_unary();
}

TEST_F(test_gps_fuser, test_construct_graph_binary_dist)
{
    test_construct_graph_binary_dist();
}

TEST_F(test_gps_fuser, test_read_pose_tum)
{
    test_read_pose_tum();
}

TEST_F(test_gps_fuser, test_construct_graph_xy_prior_dc)
{
    test_construct_graph_xy_prior_dc();
}
} // namespace tests
} // namespace experiments
} // namespace ORB_SLAM2
