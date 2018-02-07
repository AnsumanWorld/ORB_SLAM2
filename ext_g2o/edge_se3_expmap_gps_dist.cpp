#include "edge_se3_expmap_gps_dist.hpp"
#include "util/util.hpp"

namespace g2o
{
EdgeSE3ExpmapGPSDist::EdgeSE3ExpmapGPSDist()
    : BaseBinaryEdge<1, double, VertexSE3Expmap, VertexSE3Expmap>()
{
    _measurement = 0.0;
    _information = Eigen::Matrix<double, 1, 1>::Zero();
}

bool EdgeSE3ExpmapGPSDist::read(std::istream& is)
{
    is >> _measurement;
    is >> _information(0, 0);
    return true;
}

bool EdgeSE3ExpmapGPSDist::write(std::ostream& os) const
{
    os << _measurement << " " << _information(0, 0);
    return os.good();
}

void EdgeSE3ExpmapGPSDist::setInformation(const Eigen::Matrix<double, 1, 1>& m)
{
    _information = m;
}

void EdgeSE3ExpmapGPSDist::setMeasurement(const double& m)
{
    _measurement = m;
}

void EdgeSE3ExpmapGPSDist::computeError()
{
    auto v1 = static_cast<const VertexSE3Expmap*>(_vertices[0]);
    auto v2 = static_cast<const VertexSE3Expmap*>(_vertices[1]);
    auto p1 = v1->estimate().inverse().translation();
    auto p2 = v2->estimate().inverse().translation();
    auto estim_dist = util::euclidean_distance(
        p1.x(), p1.y(), p1.z(), 
        p2.x(), p2.y(), p2.z());

    _error[0] = estim_dist - _measurement;
}

EdgeSE3ExpmapGPSDistWriteGnuplotAction::EdgeSE3ExpmapGPSDistWriteGnuplotAction()
    : WriteGnuplotAction(typeid(EdgeSE3ExpmapGPSDist).name())
{
}

HyperGraphElementAction* EdgeSE3ExpmapGPSDistWriteGnuplotAction::
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

    EdgeSE3ExpmapGPSDist* e = static_cast<EdgeSE3ExpmapGPSDist*>(element);
    VertexSE3Expmap* fromEdge = static_cast<VertexSE3Expmap*>(e->vertices()[0]);
    VertexSE3Expmap* toEdge = static_cast<VertexSE3Expmap*>(e->vertices()[1]);
    Vector6 fromV, toV;

    fromV = fromEdge->estimate().toMinimalVector();
    toV = toEdge->estimate().toMinimalVector();

    for (int i = 0; i < 6; i++)
    {
        *(params->os) << fromV[i] << " ";
    }
    for (int i = 0; i < 6; i++)
    {
        *(params->os) << toV[i] << " ";
    }
    *(params->os) << std::endl;
    return this;
}

#ifdef G2O_HAVE_OPENGL
EdgeSE3ExpmapGPSDistDrawAction::EdgeSE3ExpmapGPSDistDrawAction()
    : DrawAction(typeid(EdgeSE3ExpmapGPSDist).name())
{
}

HyperGraphElementAction* EdgeSE3ExpmapGPSDistDrawAction::
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

    EdgeSE3Expmap* e = static_cast<EdgeSE3Expmap*>(element);
    VertexSE3Expmap* fromEdge = static_cast<VertexSE3Expmap*>(e->vertices()[0]);
    VertexSE3Expmap* toEdge = static_cast<VertexSE3Expmap*>(e->vertices()[1]);
    if (!fromEdge || !toEdge)
        return this;
    glColor3f(1.0, 0.0, 0.0);
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glVertex3f(static_cast<float>(fromEdge->estimate().translation().x()),
               static_cast<float>(fromEdge->estimate().translation().y()),
               static_cast<float>(fromEdge->estimate().translation().z()));
    glVertex3f(static_cast<float>(toEdge->estimate().translation().x()),
               static_cast<float>(toEdge->estimate().translation().y()),
               static_cast<float>(toEdge->estimate().translation().z()));
    glEnd();
    glPopAttrib();
    return this;
}
#endif
} // namespace g2o