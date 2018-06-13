#pragma once

#include "precompile.hpp"

#include "types.hpp"

namespace ORB_SLAM2 {
namespace experiments {
class calci {
public:
    explicit calci(const std::vector<pose>&);
    Eigen::Matrix2d info_mat_gps_xy();
    Eigen::Matrix2d info_mat_vo_xy();
    Eigen::Matrix<double, 6, 6> info_mat_vo_se3();
    Eigen::Matrix<double, 6, 6> info_mat_gps_se3();

private:
    void initialize();

    double _min_value;

    std::vector<pose> _poses;
    // GPS
    std::vector<double> _x_gps;
    std::vector<double> _y_gps;

    // VO Translation
    std::vector<double> _x_vo;
    std::vector<double> _y_vo;
    std::vector<double> _z_vo;

    // VO Rotation
    std::vector<double> _qa_vo;
    std::vector<double> _qb_vo;
    std::vector<double> _qc_vo;
    std::vector<double> _qd_vo;

    // VO Rotation in Euler
    std::vector<double> _roll;
    std::vector<double> _pitch;
    std::vector<double> _yaw;
}; // class calci
} // namespace experiments
} // namespace ORB_SLAM2
