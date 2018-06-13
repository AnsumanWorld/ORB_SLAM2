#include "precompile.hpp"

#include "edge_se3_expmap_distance.hpp"
#include "util/util.hpp"

namespace g2o {
EdgeSE3ExpmapDistance::EdgeSE3ExpmapDistance()
    : BaseBinaryEdge<1, double, VertexSE3Expmap, VertexSE3Expmap>() {
    _measurement = 0.0;
    _information = Eigen::Matrix<double, 1, 1>::Zero();
}

bool EdgeSE3ExpmapDistance::read(std::istream& is) {
    is >> _measurement;
    is >> _information(0, 0);
    return true;
}

bool EdgeSE3ExpmapDistance::write(std::ostream& os) const {
    os << _measurement << " " << _information(0, 0);
    return os.good();
}

void EdgeSE3ExpmapDistance::setInformation(
    const Eigen::Matrix<double, 1, 1>& m) {
    _information = m;
}

void EdgeSE3ExpmapDistance::setMeasurement(const double& m) {
    _measurement = m;
}

void EdgeSE3ExpmapDistance::computeError() {
    auto v1 = static_cast<const VertexSE3Expmap*>(_vertices[0]);
    auto v2 = static_cast<const VertexSE3Expmap*>(_vertices[1]);
    auto p1 = v1->estimate().inverse().translation();
    auto p2 = v2->estimate().inverse().translation();
    using namespace ORB_SLAM2::experiments;
    auto estim_dist = util::euclidean_distance(p1.x(), p1.y(), p2.x(), p2.y());

    _error[0] = estim_dist - _measurement;
}

EdgeSE3ExpmapDistanceWriteGnuplotAction::EdgeSE3ExpmapDistanceWriteGnuplotAction()
    : WriteGnuplotAction(typeid(EdgeSE3ExpmapDistance).name()) {
}

HyperGraphElementAction* EdgeSE3ExpmapDistanceWriteGnuplotAction::
operator()(HyperGraph::HyperGraphElement* element,
           HyperGraphElementAction::Parameters* params_) {
    if (typeid(*element).name() != _typeName)
        return 0;
    WriteGnuplotAction::Parameters* params =
        static_cast<WriteGnuplotAction::Parameters*>(params_);
    if (!params->os) {
        std::cerr << __PRETTY_FUNCTION__ << ": warning, on valid os specified"
                  << std::endl;
        return 0;
    }

    EdgeSE3ExpmapDistance* e = static_cast<EdgeSE3ExpmapDistance*>(element);
    VertexSE3Expmap* fromEdge = static_cast<VertexSE3Expmap*>(e->vertices()[0]);
    VertexSE3Expmap* toEdge = static_cast<VertexSE3Expmap*>(e->vertices()[1]);
    Vector6 fromV, toV;

    fromV = fromEdge->estimate().toMinimalVector();
    toV = toEdge->estimate().toMinimalVector();

    for (int i = 0; i < 6; i++) {
        *(params->os) << fromV[i] << " ";
    }
    for (int i = 0; i < 6; i++) {
        *(params->os) << toV[i] << " ";
    }
    *(params->os) << std::endl;
    return this;
}

#ifdef G2O_HAVE_OPENGL
EdgeSE3ExpmapDistanceDrawAction::EdgeSE3ExpmapDistanceDrawAction()
    : DrawAction(typeid(EdgeSE3ExpmapDistance).name()) {
}

HyperGraphElementAction* EdgeSE3ExpmapDistanceDrawAction::
operator()(HyperGraph::HyperGraphElement* element,
           HyperGraphElementAction::Parameters* params_) {
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