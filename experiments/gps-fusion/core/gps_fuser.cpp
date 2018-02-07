#include "precompile.hpp"

#include "calci.hpp"
#include "gps_fuser.hpp"
#include "macros.hpp"
#include "utils/utils.hpp"

namespace ORB_SLAM2
{
namespace experiments
{
gps_fuser::gps_fuser(const config& cfg) : _cfg(cfg)
{
    initialize();
}

void gps_fuser::fuse_gps()
{
    std::ifstream ifile(
        _cfg.get_gps_fusion_cfg().input_trajectory_file().c_str());

    if (false == ifile.good())
        throw std::runtime_error("unable to open file");

    read_pose_tum(ifile);

    std::string option = _cfg.get_gps_fusion_cfg().fuse_option();
    if (option == "xy")
        optimize_using_xy();
    else if (option == "se3exp")
        optimize_using_se3exp();
    else if (option == "se3")
        optimize_using_se3();
    else if (option == "xy_dist")
        optimize_using_xy_distance();
    else if (option == "xy_dist_prior")
        optimize_using_xy_distance_and_prior();
    else if (option == "se3exp_dist")
        optimize_using_se3exp_distance();
    else
        throw std::invalid_argument("invalid option passed");
}

void gps_fuser::read_pose_tum(std::istream& ifile)
{
    LOG_SCOPE;
    std::string each_line;
    while (std::getline(ifile, each_line))
    {
        std::stringstream ss(each_line);
        pose pose_;
        pose_.read(ss);
        _poses.push_back(pose_);
    }
}

void gps_fuser::optimize_graph()
{
    LOG_SCOPE;
    _optimizer.save(_cfg.get_gps_fusion_cfg().before_g2o_file().c_str());
    _optimizer.initializeOptimization();
    _optimizer.optimize(_cfg.optimizer_iterations());
    _optimizer.save(_cfg.get_gps_fusion_cfg().after_g2o_file().c_str());
}

void gps_fuser::initialize()
{
    LOG_SCOPE;

    auto solver = new g2o::OptimizationAlgorithmLevenberg(
        g2o::make_unique<g2o::BlockSolverX>(
            g2o::make_unique<
                g2o::LinearSolverEigen<g2o::BlockSolverX::PoseMatrixType>>()));
    _optimizer.setAlgorithm(solver);
    _optimizer.setVerbose(false);

    auto offset = new g2o::ParameterSE3Offset();
    offset->setId(0);
    _optimizer.addParameter(offset);

    _invalid_vertex_id = std::numeric_limits<std::size_t>::max();
    _last_gps_vertex_id = _invalid_vertex_id;
}

void gps_fuser::set_kernel(g2o::OptimizableGraph::Edge* e)
{
    auto rk = new g2o::RobustKernelHuber;
    e->setRobustKernel(rk);
    rk->setDelta(_cfg.get_gps_fusion_cfg().huber_threshold());
}

void gps_fuser::calculate_info_mat()
{
    LOG_SCOPE;
    if (_cfg.calculate_variance())
    {
        calci calc_(_poses);
        _info_mat_gps_xy = calc_.info_mat_gps_xy();
        _info_mat_gps_se3 = calc_.info_mat_gps_se3();
        _info_mat_vo_xy = calc_.info_mat_vo_xy();
        _info_mat_vo_se3 = calc_.info_mat_vo_se3();
    }
    else
    {
        _info_mat_gps_xy = _cfg.info_mat_gps_xy();
        _info_mat_gps_se3 = _cfg.info_mat_gps_se3();
        _info_mat_vo_xy = _cfg.info_mat_vo_xy();
        _info_mat_vo_se3 = _cfg.info_mat_vo_se3();
        _info_mat_gps_dist = _cfg.info_mat_gps_dist();
    }

    LOG_MSG << "\n---_info_mat_gps_xy\n" << _info_mat_gps_xy << "\n---\n";
    LOG_MSG << "\n---_info_mat_gps_se3\n" << _info_mat_gps_se3 << "\n---\n";
    LOG_MSG << "\n---_info_mat_vo_xy\n" << _info_mat_vo_xy << "\n---\n";
    LOG_MSG << "\n---_info_mat_vo_se3\n" << _info_mat_vo_se3 << "\n---\n";
    LOG_MSG << "\n---_info_mat_gps_dist\n" << _info_mat_gps_dist << "\n---\n";
}

void gps_fuser::add_edge_gps_xy(const pose& p, const std::size_t& vertex_id)
{
    auto vertex = _optimizer.vertex(vertex_id);
    Eigen::Vector2d gps_measur(p.latitude(), p.longitude());

    auto gps_edge = new g2o::EdgeXYPrior();
    gps_edge->setMeasurement(gps_measur);
    gps_edge->setInformation(_info_mat_gps_xy);
    gps_edge->setVertex(0, vertex);
    set_kernel(gps_edge);

    auto is_added = _optimizer.addEdge(gps_edge);
    if (!is_added)
        throw std::runtime_error("edge not added");
}

void gps_fuser::add_edge_gps_se3(const pose& p, const std::size_t& vertex_id)
{
    auto vertex = dynamic_cast<g2o::VertexSE3*>(_optimizer.vertex(vertex_id));
    if (!vertex)
        throw std::bad_cast();

    Eigen::Isometry3d measur{vertex->estimate()};
    measur.translation()[0] = p.latitude();
    measur.translation()[1] = p.longitude();

    auto gps_edge = new g2o::EdgeSE3Prior();
    gps_edge->setMeasurement(measur);
    gps_edge->setInformation(_info_mat_gps_se3);
    gps_edge->setVertex(0, vertex);
    gps_edge->setParameterId(0, 0);
    set_kernel(gps_edge);

    auto is_added = _optimizer.addEdge(gps_edge);
    if (!is_added)
        throw std::runtime_error("edge not added");
}

void gps_fuser::add_edge_gps_se3exp(const pose& p, const std::size_t& vertex_id)
{
    auto vertex =
        dynamic_cast<g2o::VertexSE3Expmap*>(_optimizer.vertex(vertex_id));
    if (!vertex)
        throw std::bad_cast();

    Eigen::Vector2d measur(p.latitude(), p.longitude());
    auto gps_edge = new g2o::EdgeSE3ExpmapPrior2D();
    gps_edge->setMeasurement(measur);
    gps_edge->setInformation(_info_mat_gps_xy);
    gps_edge->setVertex(0, vertex);
    set_kernel(gps_edge);

    auto is_added = _optimizer.addEdge(gps_edge);
    if (!is_added)
        throw std::runtime_error("edge not added");
}

void gps_fuser::add_edge_vo_xy(g2o::OptimizableGraph::Vertex* v0,
                               g2o::OptimizableGraph::Vertex* v1)
{
    auto cam_edge = new g2o::EdgePointXY();
    cam_edge->setVertex(0, v0);
    cam_edge->setVertex(1, v1);
    set_kernel(cam_edge);

    auto is_set = cam_edge->setMeasurementFromState();
    if (!is_set)
        throw std::runtime_error("setMeasurementFromState not supported");

    cam_edge->setInformation(_info_mat_vo_xy);

    auto is_added = _optimizer.addEdge(cam_edge);
    if (!is_added)
        throw std::runtime_error("edge not added");
}

void gps_fuser::add_edge_vo_se3(g2o::OptimizableGraph::Vertex* v0,
                                g2o::OptimizableGraph::Vertex* v1)
{
    auto cam_edge = new g2o::EdgeSE3();
    cam_edge->setVertex(0, v0);
    cam_edge->setVertex(1, v1);
    set_kernel(cam_edge);

    auto is_set = cam_edge->setMeasurementFromState();
    if (!is_set)
        throw std::runtime_error("setMeasurementFromState not supported");

    cam_edge->setInformation(_info_mat_vo_se3);

    auto is_added = _optimizer.addEdge(cam_edge);
    if (!is_added)
        throw std::runtime_error("edge not added");
}

void gps_fuser::add_edge_vo_se3exp(g2o::OptimizableGraph::Vertex* v0,
                                   g2o::OptimizableGraph::Vertex* v1)
{
    auto cam_edge = new g2o::EdgeSE3Expmap();
    cam_edge->setVertex(0, v0);
    cam_edge->setVertex(1, v1);
    set_kernel(cam_edge);

    auto vi = dynamic_cast<g2o::VertexSE3Expmap*>(v0);
    auto vj = dynamic_cast<g2o::VertexSE3Expmap*>(v1);
    if (!vi || !vj)
        throw std::bad_cast();

    cam_edge->setMeasurement(vj->estimate() * vi->estimate().inverse());
    cam_edge->setInformation(_info_mat_vo_se3);

    auto is_added = _optimizer.addEdge(cam_edge);
    if (!is_added)
        throw std::runtime_error("edge not added");
}

g2o::OptimizableGraph::Vertex*
gps_fuser::add_vertex_xy(const pose& p, const std::size_t& vertex_id)
{
    auto vertex = new g2o::VertexPointXY();
    Eigen::Vector2d vec(p.x(), p.y());
    vertex->setEstimate(vec);
    vertex->setId(vertex_id);

    // if (0 == vertex_id)
    //     vertex->setFixed(true);

    vertex->setMarginalized(false);

    auto is_added = _optimizer.addVertex(vertex);
    if (!is_added)
        throw std::runtime_error("vertex not added");

    return vertex;
}

g2o::OptimizableGraph::Vertex*
gps_fuser::add_vertex_se3(const pose& p, const std::size_t& vertex_id)
{
    auto vertex = new g2o::VertexSE3();
    Eigen::Quaterniond quat(p.a(), p.b(), p.c(), p.d());
    Eigen::Vector3d trans(p.x(), p.y(), p.z());
    g2o::SE3Quat se3_quat(quat, trans);
    auto estim = g2o::internal::fromSE3Quat(se3_quat);
    vertex->setEstimate(estim);
    vertex->setId(vertex_id);

    if (0 == vertex_id)
        vertex->setFixed(true);

    vertex->setMarginalized(false);

    auto is_added = _optimizer.addVertex(vertex);
    if (!is_added)
        throw std::runtime_error("vertex not added");

    return vertex;
}

g2o::OptimizableGraph::Vertex*
gps_fuser::add_vertex_se3exp(const pose& p, const std::size_t& vertex_id)
{
    auto vertex = new g2o::VertexSE3Expmap();
    Eigen::Quaterniond quat(p.a(), p.b(), p.c(), p.d());
    Eigen::Vector3d trans(p.x(), p.y(), p.z());
    g2o::SE3Quat se3_quat(quat, trans);
    vertex->setEstimate(se3_quat);
    vertex->setId(vertex_id);

    if (0 == vertex_id)
        vertex->setFixed(true);

    vertex->setMarginalized(false);

    auto is_added = _optimizer.addVertex(vertex);
    if (!is_added)
        throw std::runtime_error("vertex not added");

    return vertex;
}

void gps_fuser::recover_poses_xy()
{
    LOG_SCOPE;
    std::ofstream output_file(
        _cfg.get_gps_fusion_cfg().output_trajectory_file().c_str());
    for (std::size_t i = 0; i < _optimizer.vertices().size(); ++i)
    {
        auto vertex = dynamic_cast<g2o::VertexPointXY*>(_optimizer.vertex(i));
        if (!vertex)
            throw std::bad_cast();

        auto v = vertex->estimate();

        for (auto i = 0; i < v.size(); ++i)
            output_file << v[i] << _cfg.log_seperator();
        output_file << '\n';
    }
}

void gps_fuser::recover_poses_se3()
{
    LOG_SCOPE;
    std::ofstream output_file(
        _cfg.get_gps_fusion_cfg().output_trajectory_file().c_str());
    for (std::size_t i = 0; i < _optimizer.vertices().size(); ++i)
    {
        auto vertex = dynamic_cast<g2o::VertexSE3*>(_optimizer.vertex(i));
        if (!vertex)
            throw std::bad_cast();

        auto v = g2o::internal::toVectorQT(vertex->estimate());

        for (auto i = 0; i < v.size(); ++i)
            output_file << v[i] << _cfg.log_seperator();
        output_file << '\n';
    }
}

void gps_fuser::recover_poses_se3exp()
{
    LOG_SCOPE;
    std::ofstream output_file(
        _cfg.get_gps_fusion_cfg().output_trajectory_file().c_str());
    for (std::size_t i = 0; i < _optimizer.vertices().size(); ++i)
    {
        auto vertex = dynamic_cast<g2o::VertexSE3Expmap*>(_optimizer.vertex(i));
        if (!vertex)
            throw std::bad_cast();

        auto v = vertex->estimate().toVector();

        for (auto i = 0; i < v.size(); ++i)
            output_file << v[i] << _cfg.log_seperator();
        output_file << '\n';
    }
}

void gps_fuser::optimize_using_xy()
{
    LOG_SCOPE;
    calculate_info_mat();
    construct_graph_xy();
    optimize_graph();
    recover_poses_xy();
}

void gps_fuser::optimize_using_se3()
{
    LOG_SCOPE;
    calculate_info_mat();
    construct_graph_se3();
    optimize_graph();
    recover_poses_se3();
}

void gps_fuser::optimize_using_se3exp()
{
    LOG_SCOPE;
    calculate_info_mat();
    construct_graph_se3exp();
    optimize_graph();
    recover_poses_se3exp();
}

void gps_fuser::optimize_using_xy_distance()
{
    LOG_SCOPE;
    calculate_info_mat();
    construct_graph_xy_dc();
    optimize_graph();
    recover_poses_xy();
}

void gps_fuser::optimize_using_xy_distance_and_prior()
{
    LOG_SCOPE;
    calculate_info_mat();
    construct_graph_xy_prior_dc();
    optimize_graph();
    recover_poses_xy();
}

void gps_fuser::optimize_using_se3exp_distance()
{
    LOG_SCOPE;
    calculate_info_mat();
    construct_graph_se3exp_dc();
    optimize_graph();
    recover_poses_se3exp();
}

void gps_fuser::construct_graph_xy()
{
    LOG_SCOPE;
    g2o::OptimizableGraph::Vertex* prev_vertex{nullptr};
    std::size_t vertex_id{0};
    for (const auto& pose : _poses)
    {
        auto curr_vertex = add_vertex_xy(pose, vertex_id);
        if (pose.has_gps())
            add_edge_gps_xy(pose, vertex_id);
        if (prev_vertex)
            add_edge_vo_xy(prev_vertex, curr_vertex);
        prev_vertex = curr_vertex;
        ++vertex_id;
    }
}

void gps_fuser::construct_graph_se3()
{
    LOG_SCOPE;
    g2o::OptimizableGraph::Vertex* prev_vertex{nullptr};
    std::size_t vertex_id{0};
    for (const auto& pose : _poses)
    {
        auto curr_vertex = add_vertex_se3(pose, vertex_id);
        if (pose.has_gps())
            add_edge_gps_se3(pose, vertex_id);
        if (prev_vertex)
            add_edge_vo_se3(prev_vertex, curr_vertex);
        prev_vertex = curr_vertex;
        ++vertex_id;
    }
}

void gps_fuser::construct_graph_se3exp()
{
    LOG_SCOPE;
    g2o::OptimizableGraph::Vertex* prev_vertex{nullptr};
    std::size_t vertex_id{0};
    for (const auto& pose : _poses)
    {
        auto curr_vertex = add_vertex_se3exp(pose, vertex_id);
        if (pose.has_gps())
            add_edge_gps_se3exp(pose, vertex_id);
        if (prev_vertex)
            add_edge_vo_se3exp(prev_vertex, curr_vertex);
        prev_vertex = curr_vertex;
        ++vertex_id;
    }
}

void gps_fuser::construct_graph_xy_dc()
{
    LOG_SCOPE;
    g2o::OptimizableGraph::Vertex* prev_vertex{nullptr};
    std::size_t vertex_id{0};
    for (const auto& pose : _poses)
    {
        auto curr_vertex = add_vertex_xy(pose, vertex_id);

        if (_last_gps_vertex_id != _invalid_vertex_id && pose.has_gps())
            add_edge_xy_dc(_last_gps_vertex_id, vertex_id);

        if (prev_vertex)
            add_edge_vo_xy(prev_vertex, curr_vertex);

        if (pose.has_gps())
            _last_gps_vertex_id = vertex_id;

        prev_vertex = curr_vertex;
        ++vertex_id;
    }
}

void gps_fuser::construct_graph_xy_prior_dc()
{
    LOG_SCOPE;
    g2o::OptimizableGraph::Vertex* prev_vertex{nullptr};
    std::size_t vertex_id{0};
    for (const auto& pose : _poses)
    {
        auto curr_vertex = add_vertex_xy(pose, vertex_id);

        if (_last_gps_vertex_id != _invalid_vertex_id && pose.has_gps())
            add_edge_xy_dc(_last_gps_vertex_id, vertex_id);

        if (prev_vertex)
            add_edge_vo_xy(prev_vertex, curr_vertex);

        if (pose.has_gps())
        {
            _last_gps_vertex_id = vertex_id;
            add_edge_gps_xy(pose, vertex_id);
        }

        prev_vertex = curr_vertex;
        ++vertex_id;
    }
}

void gps_fuser::construct_graph_se3exp_dc()
{
    LOG_SCOPE;
    g2o::OptimizableGraph::Vertex* prev_vertex{nullptr};
    std::size_t vertex_id{0};
    for (const auto& pose : _poses)
    {
        auto curr_vertex = add_vertex_se3exp(pose, vertex_id);

        if (_last_gps_vertex_id != _invalid_vertex_id && pose.has_gps())
            add_edge_se3exp_dc(_last_gps_vertex_id, vertex_id);

        if (prev_vertex)
            add_edge_vo_se3exp(prev_vertex, curr_vertex);

        if (pose.has_gps())
            _last_gps_vertex_id = vertex_id;

        prev_vertex = curr_vertex;
        ++vertex_id;
    }
}

void gps_fuser::add_edge_xy_dc(const std::size_t& v1_id,
                               const std::size_t& v2_id)
{
    auto v0 = _optimizer.vertex(v1_id);
    auto v1 = _optimizer.vertex(v2_id);

    auto dist_edge = new g2o::EdgePointXYDistance();
    dist_edge->setVertex(0, v0);
    dist_edge->setVertex(1, v1);
    set_kernel(dist_edge);

    auto p1 = _poses.at(v1_id);
    auto p2 = _poses.at(v2_id);

    // Q: Why GPS dist == eucledean dist?
    // A: This is only for experiment as actual geo is not there.
    auto gps_dist = utils::euclidean_distance(p1.latitude(), p1.longitude(),
                                              p2.latitude(), p2.longitude());

    dist_edge->setMeasurement(gps_dist);
    dist_edge->setInformation(_info_mat_gps_dist);

    auto is_added = _optimizer.addEdge(dist_edge);
    if (!is_added)
        throw std::runtime_error("edge not added");
}

void gps_fuser::add_edge_se3exp_dc(const std::size_t& v1_id,
                                   const std::size_t& v2_id)
{
    auto v0 = _optimizer.vertex(v1_id);
    auto v1 = _optimizer.vertex(v2_id);

    auto dist_edge = new g2o::EdgeSE3ExpmapDistance();
    dist_edge->setVertex(0, v0);
    dist_edge->setVertex(1, v1);
    set_kernel(dist_edge);

    auto p1 = _poses.at(v1_id);
    auto p2 = _poses.at(v2_id);

    // Q: Why GPS dist == eucledean dist?
    // A: This is only for experiment as actual geo is not there.
    auto gps_dist = utils::euclidean_distance(p1.latitude(), p1.longitude(),
                                              p2.latitude(), p2.longitude());

    dist_edge->setMeasurement(gps_dist);
    dist_edge->setInformation(_info_mat_gps_dist);

    auto is_added = _optimizer.addEdge(dist_edge);
    if (!is_added)
        throw std::runtime_error("edge not added");
}
} // namespace experiments
} // namespace ORB_SLAM2
