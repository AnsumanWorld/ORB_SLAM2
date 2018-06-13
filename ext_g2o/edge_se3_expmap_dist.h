#pragma once

#include "g2o_types_slamext_api.h"

#include <g2o/core/base_binary_edge.h>
#include <g2o/core/hyper_graph_action.h>
#include <g2o/types/sba/types_six_dof_expmap.h>

#ifdef G2O_HAVE_OPENGL
#include "g2o/stuff/opengl_primitives.h"
#include "g2o/stuff/opengl_wrapper.h"
#endif

namespace g2o {
    class G2O_TYPES_SLAMEXT_API EdgeSE3ExpmapDist
        : public BaseBinaryEdge<1, double, VertexSE3Expmap, VertexSE3Expmap> {
    public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

        EdgeSE3ExpmapDist();
        virtual bool read(std::istream&);
        virtual bool write(std::ostream&) const;
        void computeError();
        virtual void setInformation(const Eigen::Matrix<double, 1, 1>&);
        virtual void setMeasurement(const double&);
    }; // class EdgeSE3ExpmapDist

    class G2O_TYPES_SLAMEXT_API EdgeSE3ExpmapDistWriteGnuplotAction
        : public WriteGnuplotAction {
    public:
        EdgeSE3ExpmapDistWriteGnuplotAction();
        virtual HyperGraphElementAction*
        operator()(HyperGraph::HyperGraphElement*,
                HyperGraphElementAction::Parameters*);
    };

    #ifdef G2O_HAVE_OPENGL
    class G2O_TYPES_SLAMEXT_API EdgeSE3ExpmapDistDrawAction
        : public DrawAction {
    public:
        EdgeSE3ExpmapDistDrawAction();
        virtual HyperGraphElementAction*
        operator()(HyperGraph::HyperGraphElement*,
                HyperGraphElementAction::Parameters*);
    };
    #endif
} // namespace g2o
