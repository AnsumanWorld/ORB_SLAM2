#pragma once

#include <opencv2/core/core.hpp>
#include <boost/optional.hpp>
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
            cv::Rect roi;
        };

        // vector, matrix are aliases to Eigen types
        struct pos_info {
            // longitude, latitude, altitude?
            std::vector<int> position;
            //symmetric_matrix<3, 3> covariance;
        };

        using traffic_sign_map_t = std::map<long unsigned int, std::vector<traffic_sign> >;
        using traffic_sign_vec_t = std::vector<traffic_sign>;
        using time_point_t = double/*std::chrono::time_point<double>*/;
        using image_t = cv::Mat;
        using tsr_info_opt_t = boost::optional<std::vector<traffic_sign>>;
        using pos_info_opt_t = boost::optional<pos_info>;
    }
} // namespace ORB_SLAM

