#pragma once

#include <g2o/types/sba/types_six_dof_expmap.h>
#include <g2o/core/sparse_optimizer.h>

#include "ext_g2o/types_slamext.h"
#include "ext/messages.h"
#include "utils/utils.h"

namespace ORB_SLAM2 {
    namespace ext {

        /// <summary> Add unary constraint for a camera pose <summary/>
        void add_pos_prior(g2o::SparseOptimizer& opt_,
                        g2o::VertexSE3Expmap* cam_,
                        pos_info_opt_t const& pos_)
        {
            if (!pos_)
                return;

            auto prior_edge_ = new g2o::EdgeSE3ExpmapPrior3D();
            prior_edge_->setMeasurement(pos_.get().pos);

            Eigen::Matrix3d inf_ = pos_.get().covariance.inverse();
            prior_edge_->setInformation(inf_);
            prior_edge_->setVertex(0, cam_);

            bool is_added_ = opt_.addEdge(prior_edge_);
            if (!is_added_)
                throw std::runtime_error("unable to add edge");
        }

        /// <summary> Add displacement constraint between two camera poses <summary/>
        void add_pos_displacement(g2o::SparseOptimizer& opt_,
                                  g2o::VertexSE3Expmap* from_,
                                  g2o::VertexSE3Expmap* to_,
                                  pos_info_opt_t const& from_pos_,
                                  pos_info_opt_t const& to_pos_)
        {
            if(!from_pos_ || !to_pos_)
                return;

            auto dist_edge_ = new g2o::EdgeSE3ExpmapDist();
            dist_edge_->setVertex(0, from_);
            dist_edge_->setVertex(1, to_);

            auto dist_ = utils::euclidean_distance(
                from_pos_.get().pos.x(), from_pos_.get().pos.y(), from_pos_.get().pos.z(),
                to_pos_.get().pos.x(), to_pos_.get().pos.y(), to_pos_.get().pos.z());

            dist_edge_->setMeasurement(dist_);

            // TODO: 3x3 covariance matrix to 1x1, use unscented transform
            auto cov_ = std::max(from_pos_.get().covariance(0, 0),
                                 to_pos_.get().covariance(0, 0));

            Eigen::Matrix<double, 1, 1> inf_{ 1.0 / cov_ };
            dist_edge_->setInformation(inf_);

            bool is_added = opt_.addEdge(dist_edge_);
            if(!is_added)
                throw std::runtime_error("unable to add edge");
        }
    } // namespace ext
} // namespace ORB_SLAM2
