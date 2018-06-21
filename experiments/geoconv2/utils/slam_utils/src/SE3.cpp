#include "SE3.h"
#include "Point3.h"
#include "PoseLocal.h"

SE3::SE3() : Eigen::Affine3d(Eigen::Affine3d::Identity())
{
}

SE3::SE3(double x, double y, double z, double yaw, double pitch, double roll)
    : Eigen::Affine3d(Eigen::Translation3d(x, y, z))
{
    initFromYawPitchRoll(yaw, pitch, roll);
}

SE3::SE3(const Eigen::Affine3d& other) : Eigen::Affine3d(other)
{
}

SE3::SE3(const PoseLocal& pose)
    : Eigen::Affine3d(Eigen::Translation3d(pose.x, pose.y, pose.z))
{
    initFromYawPitchRoll(pose.heading, pose.pitch, pose.roll);
}

void SE3::initFromYawPitchRoll(double yaw, double pitch, double roll)
{
    // Following the transformation from YawPitchRoll to Quaternion from
    // Blanco, “A Tutorial on SE(3) Transformation Parameterizations and
    // on-Manifold Optimization.”
    const double cosyaw2 = cos(yaw / 2.0);
    const double sinyaw2 = sin(yaw / 2.0);
    const double cospitch2 = cos(pitch / 2.0);
    const double sinpitch2 = sin(pitch / 2.0);
    const double cosroll2 = cos(roll / 2.0);
    const double sinroll2 = sin(roll / 2.0);

    const double qw =
        cosroll2 * cospitch2 * cosyaw2 + sinroll2 * sinpitch2 * sinyaw2;
    const double qx =
        sinroll2 * cospitch2 * cosyaw2 - cosroll2 * sinpitch2 * sinyaw2;
    const double qy =
        cosroll2 * sinpitch2 * cosyaw2 + sinroll2 * cospitch2 * sinyaw2;
    const double qz =
        cosroll2 * cospitch2 * sinyaw2 - sinroll2 * sinpitch2 * cosyaw2;

    const Eigen::Quaterniond q(qw, qx, qy, qz);
    (*this) *= q;
}

YawPitchRoll SE3::toYawPitchRoll() const
{
    // Following the transformation from RotationMatrix to YawPitchRoll from
    // Blanco, “A Tutorial on SE(3) Transformation Parameterizations and
    // on-Manifold Optimization.” yaw phi, pitch chi, roll psi
    const Eigen::Matrix3d& rot = this->rotation();
    const double pitch =
        atan2(-rot(2, 0), sqrt(pow(rot(0, 0), 2) + pow(rot(1, 0), 2)));
    double yaw, roll;
    if (pitch == -M_PI)
    {
        yaw = atan2(-rot(1, 2), -rot(0, 2));
        roll = 0;
    }
    else if (pitch == M_PI)
    {
        yaw = atan2(rot(1, 2), rot(0, 2));
        roll = 0;
    }
    else
    {
        yaw = atan2(rot(1, 0), rot(0, 0));
        roll = atan2(rot(2, 1), rot(2, 2));
    }
    return YawPitchRoll(yaw, pitch, roll);
}

PoseLocal SE3::toPoseLocal() const
{
    const YawPitchRoll ypr(toYawPitchRoll());
    const Eigen::Vector3d vec(translation());
    return PoseLocal(vec[0], vec[1], vec[2], ypr.yaw, ypr.pitch, ypr.roll);
}
