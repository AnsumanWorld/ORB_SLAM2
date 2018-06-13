#pragma once

#include <g2o/types/sba/types_six_dof_expmap.h>
#include <g2o/core/sparse_optimizer.h>

#include "ext/messages.h"
#include "ext_g2o/types_slamext.h"

namespace ORB_SLAM2 {
    namespace ext {
        class pos_adder {
        public:
            explicit pos_adder(g2o::SparseOptimizer&);
            void add_pos_prior(g2o::VertexSE3Expmap*, const pos_info_opt_t&);
            
        private:
            g2o::SparseOptimizer& _optimizer;
        }; // class pos_adder
    } // namespace ext
} // namespace ORB_SLAM2
