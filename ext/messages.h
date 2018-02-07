#pragma once

#include <opencv2/core/core.hpp>
#include <boost/optional.hpp>
#include <Eigen/Core>
#include <Eigen/StdVector>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <chrono>
#include <vector>

namespace ORB_SLAM2 {
    namespace ext {
        struct traffic_sign {
            int class_id;
            float confidence;
            cv::Rect box;
        };

        using orientation_t = std::tuple<double, double, double>; // roll, pitch, yaw
        // vector, matrix are aliases to Eigen types
        struct pos_info {
            Eigen::Vector3d pos;
            Eigen::Matrix3d covariance;
            orientation_t orient;
        };

        using traffic_sign_map_t = std::map<long unsigned int, std::vector<traffic_sign> >;
        using traffic_sign_vec_t = std::vector<traffic_sign>;
        using time_point_t = double/*std::chrono::time_point<double>*/;
        using image_t = cv::Mat;
        using tsr_info_opt_t = boost::optional<std::vector<traffic_sign>>;
        using pos_info_opt_t = boost::optional<pos_info>;
        using slam_input_t = std::tuple<ext::time_point_t, ext::image_t, ext::tsr_info_opt_t, ext::pos_info_opt_t>;
    }
} // namespace ORB_SLAM
