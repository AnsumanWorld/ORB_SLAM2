#pragma once

#include "precompile.hpp"

#include "config.hpp"
#include "g2o_custom/types_custom.hpp"
#include "types.hpp"

namespace ORB_SLAM2
{
namespace experiments
{
namespace tests
{
class test_gps_fuser;
}

class gps_fuser
{
    friend class tests::test_gps_fuser;

public:
    explicit gps_fuser(const config&);
    void fuse_gps();

private:
    void initialize();
    void set_kernel(g2o::OptimizableGraph::Edge*);
    void optimize_graph();
    void calculate_info_mat();
    void read_pose_tum(std::istream&);
    void optimize_using_xy();
    void optimize_using_se3exp();
    void optimize_using_se3();
    void optimize_using_xy_distance();
    void optimize_using_xy_distance_and_prior();
    void optimize_using_se3exp_distance();

    /* 2D XY */
    void construct_graph_xy();
    g2o::OptimizableGraph::Vertex*
    add_vertex_xy(const pose&, const std::size_t&);
    void add_edge_gps_xy(const pose&, const std::size_t&);
    void add_edge_vo_xy(g2o::OptimizableGraph::Vertex*,
                        g2o::OptimizableGraph::Vertex*);
    void recover_poses_xy();

    /* SE3Expmap */
    void construct_graph_se3exp();
    void recover_poses_se3exp();
    g2o::OptimizableGraph::Vertex*
    add_vertex_se3exp(const pose&, const std::size_t&);
    void add_edge_gps_se3exp(const pose&, const std::size_t&);
    void add_edge_vo_se3exp(g2o::OptimizableGraph::Vertex*,
                            g2o::OptimizableGraph::Vertex*);

    /* SE3 */
    void construct_graph_se3();
    void recover_poses_se3();
    g2o::OptimizableGraph::Vertex*
    add_vertex_se3(const pose&, const std::size_t&);
    void add_edge_gps_se3(const pose&, const std::size_t&);
    void add_edge_vo_se3(g2o::OptimizableGraph::Vertex*,
                         g2o::OptimizableGraph::Vertex*);

    /* XY - GPS Distance constraint(DC)*/
    void construct_graph_xy_dc();
    void add_edge_xy_dc(const std::size_t&, const std::size_t&);

    /* XY Distance constraint and prior */
    void construct_graph_xy_prior_dc();

    /* SE3Exp - Distance constraint*/
    void construct_graph_se3exp_dc();
    void add_edge_se3exp_dc(const std::size_t&, const std::size_t&);

    const config& _cfg;
    Eigen::Matrix<double, 6, 6> _info_mat_gps_se3;
    Eigen::Matrix<double, 6, 6> _info_mat_vo_se3;
    Eigen::Matrix2d _info_mat_gps_xy;
    Eigen::Matrix2d _info_mat_vo_xy;
    Eigen::Matrix<double, 1, 1> _info_mat_gps_dist;
    g2o::SparseOptimizer _optimizer;
    std::vector<pose> _poses;
    std::size_t _last_gps_vertex_id;
    std::size_t _invalid_vertex_id;
}; // class gps_fuser
} // namespace experiments
} // namespace ORB_SLAM2
