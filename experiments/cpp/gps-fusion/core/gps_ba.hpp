#pragma once

#include "precompile.hpp"

#include "config.hpp"

namespace ORB_SLAM2 {
namespace experiments {
class gps_ba {
public:
    explicit gps_ba(const config&);
    void fuse_gps();

private:
    void initialize();
    void optimize_graph();
    void ammend_graph_gps_prior();
    void ammend_graph_gps_dist();
    void ammend_graph_gps_prior_and_dist();
    void add_vo_edge(g2o::VertexSE3Expmap*, g2o::VertexSE3Expmap*);
    void add_gps_prior(g2o::VertexSE3Expmap*);
    void add_gps_dist_edge(g2o::VertexSE3Expmap*, g2o::VertexSE3Expmap*);
    void log_camera_poses(const std::string&);
    void log_gps_poses();
    void store_gps_value(g2o::VertexSE3Expmap*);

    const config& _cfg;
    Eigen::Matrix<double, 6, 6> _info_mat_vo_se3;
    Eigen::Matrix2d _info_mat_gps_xy;
    Eigen::Matrix<double, 1, 1> _info_mat_gps_dist;
    g2o::SparseOptimizer _optimizer;
    std::vector<std::pair<double, double>> _gps_values;
}; // class gps_ba
} // namespace experiments
} // namespace ORB_SLAM2
