#pragma once

#include "precompile.hpp"

namespace g2o
{
class G2O_TYPES_CUSTOM_API EdgeSE3ExpmapDistance
    : public BaseBinaryEdge<1, double, VertexSE3Expmap, VertexSE3Expmap>
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

    EdgeSE3ExpmapDistance();
    virtual bool read(std::istream&);
    virtual bool write(std::ostream&) const;
    void computeError();
    virtual void setInformation(const Eigen::Matrix<double, 1, 1>&);
    virtual void setMeasurement(const double&);
}; // class EdgeSE3ExpmapDistance

class G2O_TYPES_CUSTOM_API EdgeSE3ExpmapDistanceWriteGnuplotAction
    : public WriteGnuplotAction
{
public:
    EdgeSE3ExpmapDistanceWriteGnuplotAction();
    virtual HyperGraphElementAction*
    operator()(HyperGraph::HyperGraphElement*,
               HyperGraphElementAction::Parameters*);
};

#ifdef G2O_HAVE_OPENGL
class G2O_TYPES_CUSTOM_API EdgeSE3ExpmapDistanceDrawAction : public DrawAction
{
public:
    EdgeSE3ExpmapDistanceDrawAction();
    virtual HyperGraphElementAction*
    operator()(HyperGraph::HyperGraphElement*,
               HyperGraphElementAction::Parameters*);
};
#endif
} // namespace g2o
