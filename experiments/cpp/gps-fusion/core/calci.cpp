#include "precompile.hpp"

#include "calci.hpp"
#include "macros.hpp"
#include "util/util.hpp"

namespace ORB_SLAM2 {
namespace experiments {
calci::calci(const std::vector<pose>& poses_)
    : _min_value(1.0e-9), _poses(poses_) {
    initialize();
}

void calci::initialize() {
    LOG_SCOPE;
    auto n = _poses.size();

    _x_gps.reserve(n);
    _y_gps.reserve(n);

    _x_vo.reserve(n);
    _y_vo.reserve(n);
    _z_vo.reserve(n);

    _qa_vo.reserve(n);
    _qb_vo.reserve(n);
    _qc_vo.reserve(n);
    _qd_vo.reserve(n);

    _roll.reserve(n);
    _pitch.reserve(n);
    _yaw.reserve(n);

    for (const auto& pose : _poses) {
        if (pose.has_gps()) {
            _x_gps.push_back(pose.latitude());
            _y_gps.push_back(pose.longitude());
        }

        _x_vo.push_back(pose.x());
        _y_vo.push_back(pose.y());
        _z_vo.push_back(pose.z());

        _qa_vo.push_back(pose.a());
        _qb_vo.push_back(pose.b());
        _qc_vo.push_back(pose.c());
        _qd_vo.push_back(pose.d());

        Eigen::Quaterniond q(pose.a(), pose.b(), pose.c(), pose.d());
        auto vec = q.toRotationMatrix().eulerAngles(0, 1, 2);

        _roll.push_back(vec[0]);
        _pitch.push_back(vec[1]);
        _yaw.push_back(vec[2]);
    }
}

Eigen::Matrix2d calci::info_mat_gps_xy() {
    LOG_SCOPE;
    Eigen::Matrix2d inv{Eigen::Matrix2d::Identity()};

    inv(0, 0) = 1 / std::max(util::variance(_x_gps), _min_value);
    inv(1, 1) = 1 / std::max(util::variance(_y_gps), _min_value);

    return inv;
}

Eigen::Matrix2d calci::info_mat_vo_xy() {
    LOG_SCOPE;
    Eigen::Matrix2d inv{Eigen::Matrix2d::Identity()};

    inv(0, 0) = 1 / std::max(util::variance(_x_vo), _min_value);
    inv(1, 1) = 1 / std::max(util::variance(_y_vo), _min_value);

    return inv;
}

Eigen::Matrix<double, 6, 6> calci::info_mat_vo_se3() {
    LOG_SCOPE;
    Eigen::Matrix<double, 6, 6> inv{Eigen::Matrix<double, 6, 6>::Identity()};

    inv(0, 0) = 1 / std::max(util::variance(_x_vo), _min_value);
    inv(1, 1) = 1 / std::max(util::variance(_y_vo), _min_value);
    inv(2, 2) = 1 / std::max(util::variance(_z_vo), _min_value);
    inv(3, 3) = 1 / std::max(util::variance(_roll), _min_value);
    inv(4, 4) = 1 / std::max(util::variance(_pitch), _min_value);
    inv(5, 5) = 1 / std::max(util::variance(_yaw), _min_value);

    return inv;
}

Eigen::Matrix<double, 6, 6> calci::info_mat_gps_se3() {
    LOG_SCOPE;
    Eigen::Matrix<double, 6, 6> inv{Eigen::Matrix<double, 6, 6>::Identity()};

    inv(0, 0) = 1 / std::max(util::variance(_x_gps), _min_value);
    inv(1, 1) = 1 / std::max(util::variance(_y_gps), _min_value);
    inv(2, 2) = 1 / std::max(util::variance(_z_vo), _min_value);
    inv(3, 3) = 1 / std::max(util::variance(_roll), _min_value);
    inv(4, 4) = 1 / std::max(util::variance(_pitch), _min_value);
    inv(5, 5) = 1 / std::max(util::variance(_yaw), _min_value);

    return inv;
}
} // namespace experiments
} // namespace ORB_SLAM2
