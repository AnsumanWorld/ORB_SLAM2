#include "precompile.hpp"

#include "edge_se3_expmap_prior_2d.hpp"

namespace g2o
{
EdgeSE3ExpmapPrior2D::EdgeSE3ExpmapPrior2D()
    : BaseUnaryEdge<2, Eigen::Vector2d, VertexSE3Expmap>()
{
    setMeasurement(Eigen::Vector2d::Identity());
    setInformation(Eigen::Matrix2d::Identity());
}

void EdgeSE3ExpmapPrior2D::computeError()
{
    auto v = static_cast<const VertexSE3Expmap*>(_vertices[0]);
    auto xyz = v->estimate().inverse().translation();
    _error[0] = xyz[0] - _measurement[0];
    _error[1] = xyz[1] - _measurement[1];
}

bool EdgeSE3ExpmapPrior2D::read(std::istream& is)
{
    Eigen::Vector2d p;
    is >> p[0] >> p[1];
    setMeasurement(p);
    for (int i = 0; i < 2; ++i)
    {
        for (int j = i; j < 2; ++j)
        {
            is >> information()(i, j);
        }
    }
    return true;
}

bool EdgeSE3ExpmapPrior2D::write(std::ostream& os) const
{
    auto p = measurement();
    os << p[0] << " " << p[1];
    for (int i = 0; i < 2; ++i)
    {
        for (int j = i; j < 2; ++j)
        {
            os << " " << information()(i, j);
        }
    }
    return os.good();
}
} // namespace g2o
