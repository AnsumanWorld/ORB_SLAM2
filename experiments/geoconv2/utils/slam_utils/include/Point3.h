#pragma once

#include <Eigen/Dense>

class SE3;

class Point3 : public Eigen::Vector3d
{
public:
    Point3();
    Point3(double x, double y, double z);
    Point3(const Eigen::Vector3d& v);

    Eigen::Vector4d toHomogeneous() const;
};