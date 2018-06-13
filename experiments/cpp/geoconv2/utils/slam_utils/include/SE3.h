#pragma once
#define _USE_MATH_DEFINES

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <cmath>
#include "YawPitchRoll.h"

class Point3;
class PoseLocal;

class SE3 : public Eigen::Affine3d {
public:
    SE3();
    // Input angles in radians
    SE3(double x, double y, double z, double yaw, double pitch, double roll);
    SE3(const Eigen::Affine3d& other);
    SE3(const PoseLocal& pose);

    void initFromYawPitchRoll(double yaw, double pitch, double roll);

    YawPitchRoll toYawPitchRoll() const;
    PoseLocal toPoseLocal() const;
};