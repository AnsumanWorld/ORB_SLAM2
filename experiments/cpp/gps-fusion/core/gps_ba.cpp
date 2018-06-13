#include "precompile.hpp"

#include "g2o_custom/types_custom.hpp"

#include "gps_ba.hpp"
#include "macros.hpp"

namespace ORB_SLAM2 {
namespace experiments {
gps_ba::gps_ba(const config& cfg) : _cfg(cfg) {
    initialize();
}

void gps_ba::fuse_gps() {
    LOG_SCOPE;
    log_camera_poses(_cfg.get_gps_ba_cfg().cam_before_file().c_str());

    auto fuse_option_ = _cfg.get_gps_ba_cfg().fuse_option();
    if (fuse_option_ == "gps_prior")
        ammend_graph_gps_prior();
    else if (fuse_option_ == "gps_dist")
        ammend_graph_gps_dist();
    else if (fuse_option_ == "gps_dist_prior")
        ammend_graph_gps_prior_and_dist();
    else
        throw std::invalid_argument("unknown fuse option");

    optimize_graph();
    log_camera_poses(_cfg.get_gps_ba_cfg().cam_after_file().c_str());
    log_gps_poses();
}

void gps_ba::initialize() {
    LOG_SCOPE;
    auto solver = new g2o::OptimizationAlgorithmLevenberg(
        g2o::make_unique<g2o::BlockSolverX>(
            g2o::make_unique<
                g2o::LinearSolverEigen<g2o::BlockSolverX::PoseMatrixType>>()));
    _optimizer.setAlgorithm(solver);
    _optimizer.setVerbose(false);

    auto is_loaded =
        _optimizer.load(_cfg.get_gps_ba_cfg().input_g2o_file().c_str());

    if (!is_loaded)
        throw std::runtime_error("unable to load g2o file");

    _info_mat_vo_se3 = _cfg.info_mat_vo_se3();
    _info_mat_gps_xy = _cfg.info_mat_gps_xy();
    _info_mat_gps_dist = _cfg.info_mat_gps_dist();
}

void gps_ba::optimize_graph() {
    LOG_SCOPE;
    auto is_saved_before =
        _optimizer.save(_cfg.get_gps_ba_cfg().before_g2o_file().c_str());
    if (!is_saved_before)
        throw std::runtime_error("unable to save g2o file");

    _optimizer.initializeOptimization();
    _optimizer.optimize(_cfg.optimizer_iterations());

    auto is_saved_after =
        _optimizer.save(_cfg.get_gps_ba_cfg().after_g2o_file().c_str());
    if (!is_saved_after)
        throw std::runtime_error("unable to save g2o file");
}

void gps_ba::ammend_graph_gps_prior() {
    g2o::VertexSE3Expmap* last_cam{nullptr};
    auto n = _optimizer.vertices().size();
    for (decltype(n) i = 0; i < n; ++i) {
        auto vertex = _optimizer.vertex(i);

        auto curr_cam = dynamic_cast<g2o::VertexSE3Expmap*>(vertex);
        if (nullptr == curr_cam)
            continue;

        if (0 == (i % _cfg.get_gps_ba_cfg().gps_frame_frequency())) {
            store_gps_value(curr_cam);
            add_gps_prior(curr_cam);
        }

        if (last_cam != nullptr && _cfg.get_gps_ba_cfg().add_vo_edge())
            add_vo_edge(last_cam, curr_cam);

        last_cam = curr_cam;
    }
}

void gps_ba::ammend_graph_gps_dist() {
    LOG_SCOPE;
    g2o::VertexSE3Expmap* last_cam{nullptr};
    g2o::VertexSE3Expmap* last_gps_cam{nullptr};
    auto n = _optimizer.vertices().size();
    for (decltype(n) i = 0; i < n; ++i) {
        auto vertex = _optimizer.vertex(i);

        auto curr_cam = dynamic_cast<g2o::VertexSE3Expmap*>(vertex);
        if (nullptr == curr_cam)
            continue;
        if (0 == (i % _cfg.get_gps_ba_cfg().gps_frame_frequency())) {
            store_gps_value(curr_cam);

            if (last_gps_cam != nullptr)
                add_gps_dist_edge(last_gps_cam, curr_cam);
            last_gps_cam = curr_cam;
        }

        if (last_cam != nullptr && _cfg.get_gps_ba_cfg().add_vo_edge())
            add_vo_edge(last_cam, curr_cam);

        last_cam = curr_cam;
    }
}

void gps_ba::ammend_graph_gps_prior_and_dist() {
    LOG_SCOPE;
    g2o::VertexSE3Expmap* last_cam{nullptr};
    g2o::VertexSE3Expmap* last_gps_cam{nullptr};
    auto n = _optimizer.vertices().size();
    for (decltype(n) i = 0; i < n; ++i) {
        auto vertex = _optimizer.vertex(i);

        auto curr_cam = dynamic_cast<g2o::VertexSE3Expmap*>(vertex);
        if (nullptr == curr_cam)
            continue;
        if (0 == (i % _cfg.get_gps_ba_cfg().gps_frame_frequency())) {
            add_gps_prior(curr_cam);
            store_gps_value(curr_cam);

            if (last_gps_cam != nullptr)
                add_gps_dist_edge(last_gps_cam, curr_cam);
            last_gps_cam = curr_cam;
        }

        if (last_cam != nullptr && _cfg.get_gps_ba_cfg().add_vo_edge())
            add_vo_edge(last_cam, curr_cam);

        last_cam = curr_cam;
    }
}

void gps_ba::add_vo_edge(g2o::VertexSE3Expmap* last_cam,
                         g2o::VertexSE3Expmap* curr_cam) {
    auto cam_edge = new g2o::EdgeSE3Expmap();
    cam_edge->setVertex(0, last_cam);
    cam_edge->setVertex(1, curr_cam);

    cam_edge->setMeasurement(curr_cam->estimate() *
                             last_cam->estimate().inverse());

    cam_edge->setInformation(_info_mat_vo_se3);

    auto is_added = _optimizer.addEdge(cam_edge);
    if (!is_added)
        throw std::runtime_error("edge not added");
}

void gps_ba::add_gps_prior(g2o::VertexSE3Expmap* curr_cam) {
    auto trans_world = curr_cam->estimate().inverse().translation();
    auto gps_x_world = trans_world.x() * _cfg.get_gps_ba_cfg().gps_offset_x();
    auto gps_y_world = trans_world.y() * _cfg.get_gps_ba_cfg().gps_offset_y();
    Eigen::Vector2d measur(gps_x_world, gps_y_world);

    auto gps_edge = new g2o::EdgeSE3ExpmapPrior2D();
    gps_edge->setMeasurement(measur);
    gps_edge->setInformation(_info_mat_gps_xy);
    gps_edge->setVertex(0, curr_cam);

    auto is_added = _optimizer.addEdge(gps_edge);
    if (!is_added)
        throw std::runtime_error("edge not added");
}

void gps_ba::add_gps_dist_edge(g2o::VertexSE3Expmap* last_gps_cam,
                               g2o::VertexSE3Expmap* curr_gps_cam) {
    auto cam_p1 = last_gps_cam->estimate().inverse().translation();
    auto cam_p2 = curr_gps_cam->estimate().inverse().translation();

    auto gps_x_1 = cam_p1.x() * _cfg.get_gps_ba_cfg().gps_offset_x();
    auto gps_y_1 = cam_p1.y() * _cfg.get_gps_ba_cfg().gps_offset_y();

    auto gps_x_2 = cam_p2.x() * _cfg.get_gps_ba_cfg().gps_offset_x();
    auto gps_y_2 = cam_p2.y() * _cfg.get_gps_ba_cfg().gps_offset_y();

    auto dist_edge = new g2o::EdgeSE3ExpmapDistance();
    dist_edge->setVertex(0, last_gps_cam);
    dist_edge->setVertex(1, curr_gps_cam);

    // Q: Why GPS dist == eucledean dist?
    // A: This is only for experiment as actual geo is not there.
    auto gps_dist =
        util::euclidean_distance(gps_x_1, gps_y_1, gps_x_2, gps_y_2);

    dist_edge->setMeasurement(gps_dist);
    dist_edge->setInformation(_info_mat_gps_dist);

    auto is_added = _optimizer.addEdge(dist_edge);
    if (!is_added)
        throw std::runtime_error("edge not added");
}

void gps_ba::log_camera_poses(const std::string& filename) {
    std::ofstream ofile(filename.c_str());
    auto n = _optimizer.vertices().size();
    for (decltype(n) i = 0; i < n; ++i) {
        auto curr_cam =
            dynamic_cast<g2o::VertexSE3Expmap*>(_optimizer.vertex(i));
        if (nullptr == curr_cam)
            continue;

        g2o::Vector7 vec;
        vec = curr_cam->estimate().inverse().toVector();

        for (auto i = 0; i < vec.size(); ++i)
            ofile << vec[i] << _cfg.log_seperator();
        ofile << std::endl;
    }
}

void gps_ba::log_gps_poses() {
    std::ofstream ofile(_cfg.get_gps_ba_cfg().gps_position_file().c_str());
    for (auto& gps : _gps_values)
        ofile << gps.first << _cfg.log_seperator() << gps.second << std::endl;
}

void gps_ba::store_gps_value(g2o::VertexSE3Expmap* curr_cam) {
    auto trans_world = curr_cam->estimate().inverse().translation();
    auto gps_x_world = trans_world.x() * _cfg.get_gps_ba_cfg().gps_offset_x();
    auto gps_y_world = trans_world.y() * _cfg.get_gps_ba_cfg().gps_offset_y();

    _gps_values.push_back(std::make_pair(gps_x_world, gps_y_world));
}
} // namespace experiments
} // namespace ORB_SLAM2
