#include "edge_se3_expmap_prior_3d.h"

namespace g2o {
    EdgeSE3ExpmapPrior3D::EdgeSE3ExpmapPrior3D()
        : BaseUnaryEdge<3, Eigen::Vector3d, VertexSE3Expmap>()
    {
        setMeasurement(Eigen::Vector3d::Zero());
        setInformation(Eigen::Matrix3d::Zero());
    }

    void EdgeSE3ExpmapPrior3D::computeError()
    {
        auto v = static_cast<const VertexSE3Expmap*>(_vertices[0]);
        auto estimated = v->estimate().inverse().translation();
        _error = estimated - _measurement;
    }

    bool EdgeSE3ExpmapPrior3D::read(std::istream& is)
    {
        Eigen::Vector3d p;
        is >> p[0] >> p[1] >> p[2];;
        setMeasurement(p);
        for (int i = 0; i < 3; ++i) {
            for (int j = i; j < 3; ++j) {
                is >> information()(i, j);
            }
        }
        return true;
    }

    bool EdgeSE3ExpmapPrior3D::write(std::ostream& os) const
    {
        auto p = measurement();
        os << p[0] << " " << p[1] << " " << p[2];
        for (int i = 0; i < 3; ++i) {
            for (int j = i; j < 3; ++j) {
                os << " " << information()(i, j);
            }
        }
        return os.good();
    }
} // namespace g2o
