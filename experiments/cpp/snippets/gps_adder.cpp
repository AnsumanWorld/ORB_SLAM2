#include "gps_adder.h"
#include "gps_cfg.h"

namespace ORB_SLAM2 {
namespace ext {
gps_adder::gps_adder(g2o::SparseOptimizer& opt) : _optimizer(opt) {
    _gps_origin = gps_cfg::get().gps_origin();
    _gps_ba_fuse_option = gps_cfg::get().ba_fuse_option();
    _add_vo_edge = gps_cfg::get().add_vo_edge();
    _pose_optimization = gps_cfg::get().pose_optimization();
    _gps_source = gps_cfg::get().source_gps();

    _last_cam_with_gps_fixed_kf = nullptr;
    _last_gps_fixed_kf = boost::none;
    _last_cam_with_gps_local_kf = nullptr;
    _last_gps_local_kf = boost::none;
    _last_cam_with_gps_gba = nullptr;
    _last_gps_gba = boost::none;
}

Eigen::Vector3d
gps_adder::rotate(const Eigen::Vector3d& point, const ext::orientation_t& rot) {
    Eigen::Vector3d rotated = point;

    auto roll = util::rad_to_deg(std::get<0>(rot));
    auto pitch = util::rad_to_deg(std::get<1>(rot));
    auto yaw = util::rad_to_deg(std::get<2>(rot));

    yaw *= -1;
    // Rotate along x axis
    rotated[0] = point.x();
    rotated[1] = point.y() * cos(roll) - point.z() * sin(roll);
    rotated[2] = point.y() * sin(roll) + point.z() * cos(roll);

    // // Rotate along y axis
    rotated[0] = point.x() * cos(pitch) + point.z() * sin(pitch);
    rotated[1] = point.y();
    rotated[2] = point.z() * cos(pitch) - point.x() * sin(pitch);

    // Rotate along z axis
    rotated[0] = point.x() * cos(yaw) - point.y() * sin(yaw);
    rotated[1] = point.x() * sin(yaw) + point.y() * cos(yaw);
    rotated[2] = point.z();

    return Eigen::Vector3d(rotated.x(), rotated.y(), rotated.z());
}

Eigen::Vector3d gps_adder::axis_transform(const Eigen::Vector3d& point) {
    Eigen::Vector3d ret;
    ret[0] = point.y() * -1; // cam_x = -sens_y;
    ret[1] = point.z() * -1; // cam_y = -sens_z;
    ret[2] = point.x();      // cam_z = sens_x
    return ret;
}

Eigen::Vector3d gps_adder::to_local(pos_info_opt_t& curr_gps) {
    Eigen::Vector3d ret;
    if (gps_source::oxts == _gps_source) {
        auto origin_geo = _gps_origin.get().pos;
        auto initial_rot = _gps_origin.get().orient;

        GeodeticCoords origin(origin_geo.x(), origin_geo.y(), origin_geo.z());
        auto curr_geo = curr_gps.get().pos;
        GeodeticCoords cord(curr_geo.x(), curr_geo.y(), curr_geo.z());
        auto local_geo = cord.toENU_WGS84(origin);

        auto rotated_point =
            rotate(Eigen::Vector3d(local_geo.x, local_geo.y, local_geo.z),
                   initial_rot);
        ret = axis_transform(rotated_point);
    } else if (gps_source::ground_truth == _gps_source)
        ret = curr_gps.get().pos;

    return ret;
}

double gps_adder::distance(pos_info_opt_t& last_gps, pos_info_opt_t& curr_gps) {
    double ret{0.0};

    if (gps_source::oxts == _gps_source) {
        ret = util::geo_euclidean_distance(last_gps.get().pos.x(),
                                           last_gps.get().pos.y(),
                                           curr_gps.get().pos.x(),
                                           curr_gps.get().pos.y());
    } else if (gps_source::ground_truth == _gps_source) {
        ret = util::euclidean_distance(
            last_gps.get().pos.x(), last_gps.get().pos.y(),
            last_gps.get().pos.z(), curr_gps.get().pos.x(),
            curr_gps.get().pos.y(), curr_gps.get().pos.z());
    }

    return ret;
}

Eigen::Matrix<double, 1, 1>
gps_adder::calculate_inf_of_dist_edge(pos_info_opt_t& last_gps,
                                      pos_info_opt_t& curr_gps) {
    auto cov = std::max(last_gps.get().covariance(0, 0),
                        curr_gps.get().covariance(0, 0));
    auto inf = 1.0 / cov;
    return Eigen::Matrix<double, 1, 1>(inf);
}

void gps_adder::add_gps_prior(g2o::VertexSE3Expmap* cam,
                              pos_info_opt_t& curr_gps) {
    auto gps_edge = new g2o::EdgeSE3ExpmapPrior3D();
    auto measur = to_local(curr_gps);
    gps_edge->setMeasurement(measur);

    Eigen::Matrix3d inf = curr_gps.get().covariance.inverse();
    gps_edge->setInformation(inf);
    gps_edge->setVertex(0, cam);

    bool is_added = _optimizer.addEdge(gps_edge);
    if (!is_added)
        throw std::runtime_error("unable to add edge");
}

void gps_adder::add_vo_edge(g2o::VertexSE3Expmap* last_cam,
                            g2o::VertexSE3Expmap* curr_cam) {
    auto cam_edge = new g2o::EdgeSE3Expmap();
    cam_edge->setVertex(0, last_cam);
    cam_edge->setVertex(1, curr_cam);
    cam_edge->setMeasurement(curr_cam->estimate() *
                             last_cam->estimate().inverse());
    cam_edge->setInformation(Eigen::Matrix<double, 6, 6>::Identity());

    bool is_added = _optimizer.addEdge(cam_edge);
    if (!is_added)
        throw std::runtime_error("unable to add edge");
}

void gps_adder::add_gps_dist_edge(g2o::VertexSE3Expmap* last_cam_with_gps,
                                  g2o::VertexSE3Expmap* curr_cam_with_gps,
                                  pos_info_opt_t& last_gps,
                                  pos_info_opt_t& curr_gps) {
    auto dist_edge = new g2o::EdgeSE3ExpmapGPSDist();
    dist_edge->setVertex(0, last_cam_with_gps);
    dist_edge->setVertex(1, curr_cam_with_gps);
    auto gps_distance = distance(last_gps, curr_gps);
    dist_edge->setMeasurement(gps_distance);

    auto inf = calculate_inf_of_dist_edge(last_gps, curr_gps);
    dist_edge->setInformation(inf);

    bool is_added = _optimizer.addEdge(dist_edge);
    if (!is_added)
        throw std::runtime_error("unable to add edge");
}

void gps_adder::add_gps(g2o::VertexSE3Expmap* last_cam_with_gps,
                        g2o::VertexSE3Expmap* curr_cam_with_gps,
                        pos_info_opt_t& last_gps, pos_info_opt_t& curr_gps) {
    if (gps_option::unary_gps_edge == _gps_ba_fuse_option)
        add_gps_prior(curr_cam_with_gps, curr_gps);
    else if (gps_option::binary_gps_edge == _gps_ba_fuse_option) {
        if (nullptr != last_cam_with_gps && last_gps)
            add_gps_dist_edge(last_cam_with_gps, curr_cam_with_gps, last_gps,
                              curr_gps);
    } else if (gps_option::unary_and_binary_gps_edge == _gps_ba_fuse_option) {
        add_gps_prior(curr_cam_with_gps, curr_gps);
        if (nullptr != last_cam_with_gps && last_gps)
            add_gps_dist_edge(last_cam_with_gps, curr_cam_with_gps, last_gps,
                              curr_gps);
    }
}

void gps_adder::add_gps_to_pose_optimization(g2o::VertexSE3Expmap* vSE3,
                                             pos_info_opt_t curr_gps) {
    if (_pose_optimization && curr_gps)
        add_gps_prior(vSE3, curr_gps);
}

void gps_adder::add_vo_edges_to_ba() {
    if (!_add_vo_edge)
        return;

    g2o::VertexSE3Expmap* last_cam{nullptr};
    auto n = _optimizer.vertices().size();
    for (decltype(n) i = 0; i < n; ++i) {
        auto vertex = _optimizer.vertex(i);
        auto curr_cam = dynamic_cast<g2o::VertexSE3Expmap*>(vertex);
        if (nullptr == curr_cam)
            continue;
        if (last_cam != nullptr)
            add_vo_edge(last_cam, curr_cam);

        last_cam = curr_cam;
    }
}

void gps_adder::add_gps_to_ba_for_local_kf(g2o::VertexSE3Expmap* vSE3,
                                           pos_info_opt_t curr_gps) {
    if (!curr_gps)
        return;

    add_gps(_last_cam_with_gps_local_kf, vSE3, _last_gps_local_kf, curr_gps);

    _last_cam_with_gps_local_kf = vSE3;
    _last_gps_local_kf = curr_gps;
}

void gps_adder::add_gps_to_ba_for_fixed_kf(g2o::VertexSE3Expmap* vSE3,
                                           pos_info_opt_t curr_gps) {
    if (!curr_gps)
        return;

    add_gps(_last_cam_with_gps_fixed_kf, vSE3, _last_gps_fixed_kf, curr_gps);

    _last_cam_with_gps_fixed_kf = vSE3;
    _last_gps_fixed_kf = curr_gps;
}

void gps_adder::add_gps_to_gba(g2o::VertexSE3Expmap* vSE3,
                               pos_info_opt_t curr_gps) {
    if (!curr_gps)
        return;

    add_gps(_last_cam_with_gps_gba, vSE3, _last_gps_gba, curr_gps);

    _last_cam_with_gps_gba = vSE3;
    _last_gps_gba = curr_gps;
}
} // namespace ext
} // namespace ORB_SLAM2
