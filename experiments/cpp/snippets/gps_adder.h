#pragma once

#include <g2o/core/sparse_optimizer.h>
#include <g2o/types/sba/types_six_dof_expmap.h>

#include "ext/GeodeticCoords.h"
#include "ext/gps_cfg.h"
#include "ext/messages.h"
#include "ext_g2o/types_slamext.hpp"
#include "utils/utils.hpp"

namespace ORB_SLAM2 {
namespace ext {
class gps_adder {
public:
    explicit gps_adder(g2o::SparseOptimizer&);
    void add_gps_to_pose_optimization(g2o::VertexSE3Expmap*, pos_info_opt_t);
    void add_gps_to_ba_for_local_kf(g2o::VertexSE3Expmap*, pos_info_opt_t);
    void add_gps_to_ba_for_fixed_kf(g2o::VertexSE3Expmap*, pos_info_opt_t);

    void add_gps_to_gba(g2o::VertexSE3Expmap*, pos_info_opt_t);
    void add_vo_edges_to_ba();

private:
    void add_gps_prior(g2o::VertexSE3Expmap*, pos_info_opt_t&);
    void add_vo_edge(g2o::VertexSE3Expmap*, g2o::VertexSE3Expmap*);
    void add_gps_dist_edge(g2o::VertexSE3Expmap*, g2o::VertexSE3Expmap*,
                           pos_info_opt_t&, pos_info_opt_t&);
    void add_gps(g2o::VertexSE3Expmap*, g2o::VertexSE3Expmap*, pos_info_opt_t&,
                 pos_info_opt_t&);
    Eigen::Vector3d to_local(pos_info_opt_t&);
    double distance(pos_info_opt_t&, pos_info_opt_t&);
    Eigen::Matrix<double, 1, 1>
    calculate_inf_of_dist_edge(pos_info_opt_t&, pos_info_opt_t&);
    Eigen::Vector3d
    rotate(const Eigen::Vector3d& point, const ext::orientation_t& rot);
    Eigen::Vector3d axis_transform(const Eigen::Vector3d& point);

    g2o::SparseOptimizer& _optimizer;
    ext::pos_info_opt_t _gps_origin;
    gps_option _gps_ba_fuse_option;
    gps_source _gps_source;
    bool _add_vo_edge;
    bool _pose_optimization;
    g2o::VertexSE3Expmap* _last_cam_with_gps_fixed_kf;
    pos_info_opt_t _last_gps_fixed_kf;
    g2o::VertexSE3Expmap* _last_cam_with_gps_local_kf;
    pos_info_opt_t _last_gps_local_kf;
    g2o::VertexSE3Expmap* _last_cam_with_gps_gba;
    pos_info_opt_t _last_gps_gba;
}; // class gps_adder
} // namespace ext
} // namespace ORB_SLAM2
