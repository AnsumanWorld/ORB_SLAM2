#include "SphericalCoordinates.h"
#include <cmath>
#include "CoordLocal.h"

SphericalCoordinates::SphericalCoordinates() : distance(0)
{
}

SphericalCoordinates::SphericalCoordinates(double dist, double inclination,
                                           double azimuth)
    : distance(dist), inclination(inclination), azimuth(azimuth){};

CoordLocal SphericalCoordinates::toCoordLocal() const
{
    const double x = distance * cos(inclination) * cos(azimuth);
    const double y = distance * cos(inclination) * sin(azimuth);
    const double z = distance * sin(inclination);
    return CoordLocal(x, y, z);
};
