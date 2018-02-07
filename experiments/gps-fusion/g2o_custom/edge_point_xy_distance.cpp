#include "precompile.hpp"

#include "edge_point_xy_distance.hpp"
#include "utils/utils.hpp"

namespace g2o
{
EdgePointXYDistance::EdgePointXYDistance()
    : BaseBinaryEdge<1, double, VertexPointXY, VertexPointXY>()
{
    _measurement = 0.0;
    _information = Eigen::Matrix<double, 1, 1>::Zero();
}

bool EdgePointXYDistance::read(std::istream& is)
{
    is >> _measurement;
    is >> _information(0, 0);
    return true;
}

bool EdgePointXYDistance::write(std::ostream& os) const
{
    os << _measurement << " " << _information(0, 0);
    return os.good();
}

void EdgePointXYDistance::setInformation(const Eigen::Matrix<double, 1, 1>& m)
{
    _information = m;
}

void EdgePointXYDistance::setMeasurement(const double& m)
{
    _measurement = m;
}

void EdgePointXYDistance::computeError()
{
    auto v1 = static_cast<const VertexPointXY*>(_vertices[0]);
    auto v2 = static_cast<const VertexPointXY*>(_vertices[1]);
    auto p1 = v1->estimate();
    auto p2 = v2->estimate();

    using namespace ORB_SLAM2::experiments;
    auto estim_dist = utils::euclidean_distance(p1.x(), p1.y(), p2.x(), p2.y());

    _error[0] = estim_dist - _measurement;
}

EdgePointXYDistanceWriteGnuplotAction::EdgePointXYDistanceWriteGnuplotAction()
    : WriteGnuplotAction(typeid(EdgePointXYDistance).name())
{
}

HyperGraphElementAction* EdgePointXYDistanceWriteGnuplotAction::
operator()(HyperGraph::HyperGraphElement* element,
           HyperGraphElementAction::Parameters* params_)
{
    if (typeid(*element).name() != _typeName)
        return 0;
    WriteGnuplotAction::Parameters* params =
        static_cast<WriteGnuplotAction::Parameters*>(params_);
    if (!params->os)
    {
        std::cerr << __PRETTY_FUNCTION__ << ": warning, on valid os specified"
                  << std::endl;
        return 0;
    }

    EdgePointXYDistance* e = static_cast<EdgePointXYDistance*>(element);
    if (e->numUndefinedVertices())
        return this;
    VertexPointXY* fromEdge = static_cast<VertexPointXY*>(e->vertex(0));
    VertexPointXY* toEdge = static_cast<VertexPointXY*>(e->vertex(1));
    *(params->os) << fromEdge->estimate().x() << " " << fromEdge->estimate().y()
                  << std::endl;
    *(params->os)
        << toEdge->estimate().x() << " " << toEdge->estimate().y() << std::endl;
    *(params->os) << std::endl;
    return this;
}

#ifdef G2O_HAVE_OPENGL
EdgePointXYDistanceDrawAction::EdgePointXYDistanceDrawAction()
    : DrawAction(typeid(EdgePointXYDistance).name())
{
}

HyperGraphElementAction* EdgePointXYDistanceDrawAction::
operator()(HyperGraph::HyperGraphElement* element,
           HyperGraphElementAction::Parameters* params_)
{
    if (typeid(*element).name() != _typeName)
        return 0;
    refreshPropertyPtrs(params_);
    if (!_previousParams)
        return this;

    if (_show && !_show->value())
        return this;

    EdgePointXYDistance* e = static_cast<EdgePointXYDistance*>(element);
    VertexPointXY* fromEdge = static_cast<VertexPointXY*>(e->vertex(0));
    VertexPointXY* toEdge = static_cast<VertexPointXY*>(e->vertex(1));
    if (!fromEdge || !toEdge)
        return this;
    glColor3f(1.0, 0.0, 0.0);
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glVertex2f(static_cast<float>(fromEdge->estimate().x()),
               static_cast<float>(fromEdge->estimate().y()));
    glVertex2f(static_cast<float>(toEdge->estimate().x()),
               static_cast<float>(toEdge->estimate().y()));
    glEnd();
    glPopAttrib();
    return this;
}
#endif
} // namespace g2o
