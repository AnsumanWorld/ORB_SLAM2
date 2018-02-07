#pragma once

#include "precompile.hpp"

namespace g2o
{
class G2O_TYPES_CUSTOM_API EdgePointXYDistance
    : public BaseBinaryEdge<1, double, VertexPointXY, VertexPointXY>
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

    EdgePointXYDistance();
    virtual bool read(std::istream&);
    virtual bool write(std::ostream&) const;
    void computeError();
    virtual void setInformation(const Eigen::Matrix<double, 1, 1>&);
    virtual void setMeasurement(const double&);
}; // class EdgePointXYDistance

class G2O_TYPES_CUSTOM_API EdgePointXYDistanceWriteGnuplotAction
    : public WriteGnuplotAction
{
public:
    EdgePointXYDistanceWriteGnuplotAction();
    virtual HyperGraphElementAction*
    operator()(HyperGraph::HyperGraphElement*,
               HyperGraphElementAction::Parameters*);
};

#ifdef G2O_HAVE_OPENGL
class G2O_TYPES_CUSTOM_API EdgePointXYDistanceDrawAction : public DrawAction
{
public:
    EdgePointXYDistanceDrawAction();
    virtual HyperGraphElementAction*
    operator()(HyperGraph::HyperGraphElement*,
               HyperGraphElementAction::Parameters*);
};
#endif
} // namespace g2o
