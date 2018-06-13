#include "pos_adder.h"

namespace ORB_SLAM2 {
    namespace ext {
        pos_adder::pos_adder(g2o::SparseOptimizer& opt):_optimizer(opt)
        {
        }

        void pos_adder::add_pos_prior(g2o::VertexSE3Expmap* cam,
            const pos_info_opt_t& curr_gps)
        {
            if(!curr_gps)
                return;
            auto gps_edge = new g2o::EdgeSE3ExpmapPrior3D();
            gps_edge->setMeasurement(curr_gps.get().pos);

            Eigen::Matrix3d inf = curr_gps.get().covariance.inverse();
            gps_edge->setInformation(inf);
            gps_edge->setVertex(0, cam);

            bool is_added = _optimizer.addEdge(gps_edge);
            if(!is_added)
                throw std::runtime_error("unable to add edge");
        }
    } // namespace ext
} // namespace ORB_SLAM2
