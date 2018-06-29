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

        struct pos_info {
            Eigen::Vector3d pos;
            Eigen::Matrix3d covariance;
		};

		using gps_pair_t = std::pair<long unsigned int, pos_info >;
        using traffic_sign_map_t = std::map<long unsigned int, std::vector<traffic_sign> >;
        using traffic_sign_vec_t = std::vector<traffic_sign>;
        using time_point_t = double/*std::chrono::time_point<double>*/;
        using image_t = cv::Mat;
        using tsr_info_opt_t = boost::optional<std::vector<traffic_sign>>;
        using pos_info_opt_t = boost::optional<pos_info>;
		using semantic_info_t = std::tuple <long unsigned int, double, tsr_info_opt_t,  pos_info_opt_t>;
    }
} // namespace ORB_SLAM
