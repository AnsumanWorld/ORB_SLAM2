#pragma once

#include "g2o_types_slamext_api.h"

#include <g2o/core/base_unary_edge.h>
#include <g2o/types/sba/types_six_dof_expmap.h>

namespace g2o
{
class G2O_TYPES_SLAMEXT_API EdgeSE3ExpmapPrior3D
    : public BaseUnaryEdge<3, Eigen::Vector3d, VertexSE3Expmap>
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

    EdgeSE3ExpmapPrior3D();
    virtual bool read(std::istream&);
    virtual bool write(std::ostream&) const;
    void computeError();
}; // class EdgeSE3ExpmapPrior3D
} // namespace g2o
