#include "Point3.h"
#include "SE3.h"

Point3::Point3() : Eigen::Vector3d(Eigen::Vector3d::Zero()) {
}

Point3::Point3(double x, double y, double z) : Eigen::Vector3d(x, y, z) {
}

Point3::Point3(const Eigen::Vector3d& v) : Eigen::Vector3d(v) {
}

Eigen::Vector4d Point3::toHomogeneous() const {
    Eigen::Vector4d out((*this)[0], (*this)[1], (*this)[2], 1.0);
    return Eigen::Vector4d();
}
