#pragma once

#include "precompile.hpp"

namespace ORB_SLAM2 {
namespace experiments {
class gps_fusion_cfg {
public:
    const std::string& fuse_option() const;
    const std::string& input_trajectory_file() const;
    const std::string& output_trajectory_file() const;
    const std::string& after_g2o_file() const;
    const std::string& before_g2o_file() const;
    const double& huber_threshold() const;
    bool fix_initial_vertex() const;
    void read_json(const boost::property_tree::ptree&);

private:
    std::string _fuse_option;
    std::string _input_trajectory_file;
    std::string _output_trajectory_file;
    std::string _after_g2o_file;
    std::string _before_g2o_file;
    double _huber_threshold;
    bool _fix_initial_vertex;
};

class gps_ba_cfg {
public:
    const double& gps_offset_x() const;
    const double& gps_offset_y() const;
    const std::string& cam_before_file() const;
    const std::string& cam_after_file() const;
    const std::string& gps_position_file() const;
    const std::string& fuse_option() const;
    const std::string& input_g2o_file() const;
    const std::string& before_g2o_file() const;
    const std::string& after_g2o_file() const;
    bool add_vo_edge() const;
    const std::size_t& gps_frame_frequency() const;
    void read_json(const boost::property_tree::ptree&);

private:
    double _gps_offset_x;
    double _gps_offset_y;
    std::string _cam_before_file;
    std::string _cam_after_file;
    std::string _gps_position_file;
    std::string _fuse_option;
    std::string _input_g2o_file;
    std::string _before_g2o_file;
    std::string _after_g2o_file;
    bool _add_vo_edge;
    std::size_t _gps_frame_frequency;
};

class config {
public:
    explicit config(std::istream&);
    bool calculate_variance() const;
    const int& optimizer_iterations() const;
    const std::string& log_seperator() const;
    const Eigen::Matrix<double, 1, 1>& info_mat_gps_dist() const;
    const Eigen::Matrix<double, 6, 6>& info_mat_gps_se3() const;
    const Eigen::Matrix<double, 6, 6>& info_mat_vo_se3() const;
    const Eigen::Matrix2d& info_mat_gps_xy() const;
    const Eigen::Matrix2d& info_mat_vo_xy() const;
    const gps_fusion_cfg& get_gps_fusion_cfg() const;
    const gps_ba_cfg& get_gps_ba_cfg() const;

private:
    void read_json(std::istream&);
    template <typename T>
    void
    extract_matrix(const boost::property_tree::ptree&, const std::string&, T&);

    bool _calculate_variance;
    int _optimizer_iterations;
    std::string _log_seperator;
    Eigen::Matrix<double, 1, 1> _info_mat_gps_dist;
    Eigen::Matrix<double, 6, 6> _info_mat_gps_se3;
    Eigen::Matrix<double, 6, 6> _info_mat_vo_se3;
    Eigen::Matrix2d _info_mat_gps_xy;
    Eigen::Matrix2d _info_mat_vo_xy;
    gps_fusion_cfg _gps_fusion_cfg;
    gps_ba_cfg _gps_ba_cfg;
}; // class config

template <typename T>
void config::extract_matrix(const boost::property_tree::ptree& root,
                            const std::string& name, T& data) {
    auto x = 0;
    for (auto& row : root.get_child(name)) {
        auto y = 0;
        for (auto& cell : row.second) {
            data(x, y) = cell.second.get_value<double>();
            ++y;
        }
        ++x;
    }
}
} // namespace experiments
} // namespace ORB_SLAM2
