#pragma once

#include <opencv2/core/core.hpp>

#include <boost/optional.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>
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

        using traffic_sign_map_t = std::map<long unsigned int, std::vector<traffic_sign> >;
        using traffic_sign_vec_t = std::vector<traffic_sign>;
        using time_point_t = double/*std::chrono::time_point<double>*/;
        using image_t = cv::Mat;
        using tsr_info_opt_t = boost::optional<std::vector<traffic_sign>>;
        using pos_info_opt_t = boost::optional<pos_info>;

        /// Input type
        using slam_input_t = std::tuple<ext::time_point_t, ext::image_t, ext::tsr_info_opt_t, ext::pos_info_opt_t>;

        /// Export sub types
        using camera_pose_t = std::tuple<Eigen::Vector3f, Eigen::Quaternionf>;
        using landmark_pos_t = Eigen::Vector3f;
        using frame_id_t = uint64_t;
        using landmark_id_t = uint64_t;
        using class_id_t = int;
        using cov66_t = Eigen::Matrix<double, 6, 6>;
        using cov33_t = Eigen::Matrix<double, 3, 3>;
        using keyframe_map_t = std::map<frame_id_t, std::tuple<camera_pose_t, cov66_t>>;
        using landmark_map_t = std::map<landmark_id_t, std::tuple<class_id_t, landmark_pos_t, cov33_t>>;
        using observations_t = std::map<landmark_id_t, std::set<frame_id_t>>;
        using keyframe_list_t = std::list<std::tuple<frame_id_t, image_t, pos_info_opt_t>>;

        // Export types
        using map_export_t = std::tuple<keyframe_map_t, landmark_map_t, observations_t, pos_info_opt_t>;
        using sfm_export_t = std::tuple<keyframe_list_t, cv::Matx33d>;
    }
} // namespace ORB_SLAM
