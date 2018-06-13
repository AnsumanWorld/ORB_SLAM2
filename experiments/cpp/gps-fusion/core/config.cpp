#include "precompile.hpp"

#include "config.hpp"

namespace ORB_SLAM2 {
namespace experiments {
config::config(std::istream& ifile) {
    read_json(ifile);
}

void config::read_json(std::istream& ifile) {
    boost::property_tree::ptree root;
    boost::property_tree::read_json(ifile, root);

    const auto& gps_fusion_root = root.get_child("gps_fusion");
    _gps_fusion_cfg.read_json(gps_fusion_root);

    const auto& gps_ba_root = root.get_child("gps_ba");
    _gps_ba_cfg.read_json(gps_ba_root);

    _calculate_variance = root.get<bool>("calculate_variance");
    _optimizer_iterations = root.get<int>("optimizer_iterations");
    _log_seperator = root.get<std::string>("log_seperator");
    _info_mat_gps_dist[0] = root.get<double>("info_mat_gps_dist");

    extract_matrix<Eigen::Matrix<double, 6, 6>>(root, "info_mat_gps_se3",
                                                _info_mat_gps_se3);
    extract_matrix<Eigen::Matrix<double, 6, 6>>(root, "info_mat_vo_se3",
                                                _info_mat_vo_se3);
    extract_matrix<Eigen::Matrix2d>(root, "info_mat_gps_xy", _info_mat_gps_xy);
    extract_matrix<Eigen::Matrix2d>(root, "info_mat_vo_xy", _info_mat_vo_xy);
}

bool config::calculate_variance() const {
    return _calculate_variance;
}

const int& config::optimizer_iterations() const {
    return _optimizer_iterations;
}

const std::string& config::log_seperator() const {
    return _log_seperator;
}

const Eigen::Matrix2d& config::info_mat_gps_xy() const {
    return _info_mat_gps_xy;
}

const Eigen::Matrix2d& config::info_mat_vo_xy() const {
    return _info_mat_vo_xy;
}

const Eigen::Matrix<double, 6, 6>& config::info_mat_gps_se3() const {
    return _info_mat_gps_se3;
}

const Eigen::Matrix<double, 6, 6>& config::info_mat_vo_se3() const {
    return _info_mat_vo_se3;
}

const Eigen::Matrix<double, 1, 1>& config::info_mat_gps_dist() const {
    return _info_mat_gps_dist;
}

const gps_fusion_cfg& config::get_gps_fusion_cfg() const {
    return _gps_fusion_cfg;
}

const gps_ba_cfg& config::get_gps_ba_cfg() const {
    return _gps_ba_cfg;
}

const std::string& gps_fusion_cfg::fuse_option() const {
    return _fuse_option;
}

const std::string& gps_fusion_cfg::input_trajectory_file() const {
    return _input_trajectory_file;
}

const std::string& gps_fusion_cfg::output_trajectory_file() const {
    return _output_trajectory_file;
}

const std::string& gps_fusion_cfg::after_g2o_file() const {
    return _after_g2o_file;
}

const std::string& gps_fusion_cfg::before_g2o_file() const {
    return _before_g2o_file;
}

const double& gps_fusion_cfg::huber_threshold() const {
    return _huber_threshold;
}

bool gps_fusion_cfg::fix_initial_vertex() const {
    return _fix_initial_vertex;
}

void gps_fusion_cfg::read_json(const boost::property_tree::ptree& root) {
    _fuse_option = root.get<std::string>("fuse_option");
    _input_trajectory_file = root.get<std::string>("input_trajectory_file");
    _output_trajectory_file = root.get<std::string>("output_trajectory_file");
    _after_g2o_file = root.get<std::string>("after_g2o_file");
    _before_g2o_file = root.get<std::string>("before_g2o_file");
    _huber_threshold = root.get<double>("huber_threshold");
    _fix_initial_vertex = root.get<bool>("fix_initial_vertex");
}

const double& gps_ba_cfg::gps_offset_x() const {
    return _gps_offset_x;
}

const double& gps_ba_cfg::gps_offset_y() const {
    return _gps_offset_y;
}

const std::string& gps_ba_cfg::cam_before_file() const {
    return _cam_before_file;
}

const std::string& gps_ba_cfg::cam_after_file() const {
    return _cam_after_file;
}

const std::string& gps_ba_cfg::fuse_option() const {
    return _fuse_option;
}

const std::string& gps_ba_cfg::gps_position_file() const {
    return _gps_position_file;
}

const std::string& gps_ba_cfg::input_g2o_file() const {
    return _input_g2o_file;
}

const std::string& gps_ba_cfg::before_g2o_file() const {
    return _before_g2o_file;
}

const std::string& gps_ba_cfg::after_g2o_file() const {
    return _after_g2o_file;
}

bool gps_ba_cfg::add_vo_edge() const {
    return _add_vo_edge;
}

const std::size_t& gps_ba_cfg::gps_frame_frequency() const {
    return _gps_frame_frequency;
}

void gps_ba_cfg::read_json(const boost::property_tree::ptree& root) {
    _gps_offset_x = root.get<double>("gps_offset_x");
    _gps_offset_y = root.get<double>("gps_offset_y");
    _cam_before_file = root.get<std::string>("cam_before_file");
    _cam_after_file = root.get<std::string>("cam_after_file");
    _gps_position_file = root.get<std::string>("gps_position_file");
    _fuse_option = root.get<std::string>("fuse_option");
    _input_g2o_file = root.get<std::string>("input_g2o_file");
    _before_g2o_file = root.get<std::string>("before_g2o_file");
    _after_g2o_file = root.get<std::string>("after_g2o_file");
    _add_vo_edge = root.get<bool>("add_vo_edge");
    _gps_frame_frequency = root.get<std::size_t>("gps_frame_frequency");
}
} // namespace experiments
} // namespace ORB_SLAM2
