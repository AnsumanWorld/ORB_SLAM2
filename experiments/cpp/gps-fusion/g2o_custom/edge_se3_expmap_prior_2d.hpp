#pragma once

#include "precompile.hpp"

namespace g2o {
class G2O_TYPES_CUSTOM_API EdgeSE3ExpmapPrior2D
    : public BaseUnaryEdge<2, Eigen::Vector2d, VertexSE3Expmap> {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

    EdgeSE3ExpmapPrior2D();
    virtual bool read(std::istream&);
    virtual bool write(std::ostream&) const;
    void computeError();
}; // class EdgeSE3ExpmapPrior2D
} // namespace g2o
