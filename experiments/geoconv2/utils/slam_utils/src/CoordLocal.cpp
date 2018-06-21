#include "CoordLocal.h"
#include <math.h>
#include "CoordEnu.h"
#include "CoordWgs84.h"
#include "PoseWgs84.h"
#include "SphericalCoordinates.h"

CoordLocal::CoordLocal()
    : x(std::numeric_limits<double>::quiet_NaN())
    , y(std::numeric_limits<double>::quiet_NaN())
    , z(std::numeric_limits<double>::quiet_NaN())
{
}

CoordLocal::CoordLocal(double x, double y, double z) : x(x), y(y), z(z)
{
}

CoordEnu CoordLocal::toEnu(const PoseWgs84& reference) const
{
    // Rotate the coordinate according to reference rotation
    const double& phi = reference.getRoll();
    const double& psi = reference.getHeading();
    const double& theta = reference.getPitch();
    const double sinphi = sin(phi);
    const double cosphi = cos(phi);
    const double sinpsi = sin(psi);
    const double cospsi = cos(psi);
    const double sintheta = sin(theta);
    const double costheta = cos(theta);

    const double east = cospsi * costheta * x +
                        (sinpsi * cospsi * sintheta - cosphi * sinpsi) * y +
                        (cosphi * cospsi * sintheta + sinphi * sinpsi) * z;
    const double north = sinpsi * costheta * x +
                         (sinphi * sinpsi * sintheta + cosphi * cospsi) * y +
                         (cosphi * sinpsi * sintheta - sinphi * cospsi) * z;
    const double up =
        -sintheta * x + sinphi * costheta * y + cosphi * costheta * z;

    return CoordEnu(east, north, up);
}

CoordWgs84 CoordLocal::toWgs84(const PoseWgs84& reference) const
{
    // Transform to local ENU frame and then to WGS 84
    return toEnu(reference).toWgs84(reference);
}

SphericalCoordinates CoordLocal::toSphericalCoordinates() const
{
    const double dist = sqrt(x * x + y * y + z * z);
    const double incl = asin(z / dist);
    const double azi = atan2(y, x);
    return SphericalCoordinates(dist, incl, azi);
}

double CoordLocal::distTo(const CoordLocal& other) const
{
    return sqrt(pow(other.x - x, 2) + pow(other.y - y, 2) +
                pow(other.z - z, 2));
}

double CoordLocal::headingTo(const CoordLocal& other) const
{
    const double dx = other.x - x;
    const double dy = other.y - y;
    return atan2(dy, dx);
}

double CoordLocal::elevationTo(const CoordLocal& other) const
{
    const double r = distTo(other);
    const double dz = other.z - z;
    return asin(dz / r);
}

bool CoordLocal::operator==(const CoordLocal& other) const
{
    return x == other.x && y == other.y && z == other.z;
}

bool CoordLocal::operator!=(const CoordLocal& other) const
{
    return !(*this == other);
}

bool CoordLocal::isInitialized() const
{
    return (x == x && y == y && z == z);
}
