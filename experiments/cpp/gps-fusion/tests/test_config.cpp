#include "precompile.hpp"

#include "config.hpp"

namespace ORB_SLAM2 {
namespace experiments {
namespace tests {
TEST(config_test, json_parsing) {
    using namespace ORB_SLAM2::experiments;

    std::string json_data = R"(
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
                [1.01, 1.01],
                [1.01, 1.01]
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
    std::stringstream ss(json_data);
    config cfg(ss);

    EXPECT_EQ(cfg.get_gps_fusion_cfg().fuse_option(), "xy");
    EXPECT_EQ(cfg.get_gps_ba_cfg().cam_before_file(), "cam-before.txt");
    EXPECT_EQ(cfg.log_seperator(), " ");
    Eigen::Matrix2d gps_mat = cfg.info_mat_gps_xy();
    for (auto i = 0; i < 2; ++i) {
        for (auto j = 0; j < 2; ++j) {
            EXPECT_DOUBLE_EQ(1.01, gps_mat(i, j));
        }
    }
}
} // namespace tests
} // namespace experiments
} // namespace ORB_SLAM2
