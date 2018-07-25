#include <stdint.h>
#include <Eigen/StdVector>
#include <iostream>

#include <g2o/core/block_solver.h>
#include <g2o/core/optimization_algorithm_gauss_newton.h>
#include <g2o/core/solver.h>
#include <g2o/core/sparse_optimizer.h>
#include <g2o/solvers/cholmod/linear_solver_cholmod.h>
#include <g2o/stuff/sampler.h>

#include <g2o/core/optimization_algorithm_levenberg.h>
#include <g2o/core/robust_kernel_impl.h>
#include <g2o/solvers/dense/linear_solver_dense.h>
#include <g2o/solvers/eigen/linear_solver_eigen.h>

#include <g2o/core/base_binary_edge.h>
#include <g2o/core/base_unary_edge.h>
#include <g2o/core/block_solver.h>
#include <g2o/core/factory.h>
#include <g2o/core/optimization_algorithm_levenberg.h>
#include <g2o/core/robust_kernel_impl.h>
#include <g2o/solvers/dense/linear_solver_dense.h>
#include <g2o/solvers/eigen/linear_solver_eigen.h>
#include <g2o/types/sba/types_sba.h>
#include <g2o/types/sba/types_six_dof_expmap.h>
#include <g2o/types/sim3/types_seven_dof_expmap.h>
#include <g2o/types/slam2d/edge_pointxy.h>
#include <g2o/types/slam2d/edge_xy_prior.h>
#include <g2o/types/slam2d/vertex_point_xy.h>
#include <g2o/types/slam3d/edge_se3.h>
#include <g2o/types/slam3d/edge_se3_prior.h>
#include <g2o/types/slam3d/isometry3d_gradients.h>

int main(int argc, char** argv) {
    if (2 != argc)
        throw std::invalid_argument("usage:" + std::string(argv[0]) +
                                    " file.g2o");

    g2o::SparseOptimizer optimizer;
    optimizer.setVerbose(false);

    auto solver = new g2o::OptimizationAlgorithmLevenberg(
        g2o::make_unique<g2o::BlockSolverX>(
            g2o::make_unique<
                g2o::LinearSolverCholmod<g2o::BlockSolverX::PoseMatrixType>>()));
    optimizer.setAlgorithm(solver);

    auto is_loaded = optimizer.load(argv[1]);
    if (!is_loaded)
        throw std::runtime_error("unable to load g2o file");

    if (!optimizer.verifyInformationMatrices())
        throw std::runtime_error("optimizer_ verifyInformationMatrices failed");

    optimizer.initializeOptimization();
    optimizer.setVerbose(true);
    optimizer.optimize(2);

    // forcing linker to use sba types library
    g2o::VertexSE3Expmap camera;
    g2o::VertexSBAPointXYZ landmark;

    std::vector<std::pair<int, int>> block_indices;
    for (size_t i = 0; i < optimizer.activeVertices().size(); i++) {
        g2o::OptimizableGraph::Vertex* v = optimizer.activeVertices()[i];
        if (v->hessianIndex() >= 0) {
            block_indices.push_back(
                std::make_pair(v->hessianIndex(), v->hessianIndex()));
        }
        if (v->hessianIndex() > 0) {
            block_indices.push_back(
                std::make_pair(v->hessianIndex() - 1, v->hessianIndex()));
        }
    }

    g2o::SparseBlockMatrix<Eigen::MatrixXd> spinv;
    if (optimizer.computeMarginals(spinv, block_indices)) {
        for (size_t i = 0; i < optimizer.activeVertices().size(); i++) {
            g2o::OptimizableGraph::Vertex* v = optimizer.activeVertices()[i];
            std::cout << "Vertex id:" << v->id() << std::endl;
            if (v->hessianIndex() >= 0) {
                std::cout << "inv block :" << v->hessianIndex() << ", "
                          << v->hessianIndex() << std::endl;
                std::cout << spinv.block(v->hessianIndex(), v->hessianIndex())->eval();
                std::cout << std::endl;
            }
            if (v->hessianIndex() > 0) {
                std::cout << "inv block :" << v->hessianIndex() - 1 << ", "
                          << v->hessianIndex() << std::endl;
                std::cout
                    << spinv.block(v->hessianIndex() - 1, v->hessianIndex())->eval();
                std::cout << std::endl;
            }
        }
    }

    return 0;
}
