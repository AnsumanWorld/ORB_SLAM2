#include "CoordWgs84.h"
#include <limits>
#include "CoordEcef.h"
#include "CoordEnu.h"
#include "CoordLocal.h"
#include "PoseWgs84.h"
#include "coord_constants.h"

CoordWgs84::CoordWgs84()
    : lat(std::numeric_limits<double>::quiet_NaN())
    , lon(std::numeric_limits<double>::quiet_NaN())
    , alt(std::numeric_limits<double>::quiet_NaN())
{
}

CoordWgs84::CoordWgs84(double lat, double lon, double alt)
    : lat(lat), lon(lon), alt(alt)
{
}

CoordEcef CoordWgs84::toEcef() const
{
    const double N =
        pow(constants::a, 2) /
        sqrt(pow(constants::a * cos(lat), 2) + pow(constants::b * sin(lat), 2));
    const double X = (N + alt) * cos(lat) * cos(lon);
    const double Y = (N + alt) * cos(lat) * sin(lon);
    const double Z = (pow(constants::b / constants::a, 2) * N + alt) * sin(lat);
    return CoordEcef(X, Y, Z);
}

CoordEnu CoordWgs84::toEnu(const CoordWgs84& reference) const
{
    return this->toEcef().toEnu(reference.toEcef(), reference);
}

CoordLocal CoordWgs84::toCoordLocal(const PoseWgs84& reference) const
{
    // Get coordinate in ENU frame and then transform it to local frame
    return toEnu(reference).toCoordLocal(reference);
}

double CoordWgs84::getLat() const
{
    return lat;
}
double CoordWgs84::getLon() const
{
    return lon;
}
double CoordWgs84::getAlt() const
{
    return alt;
}
bool CoordWgs84::isInitialized() const
{
    return (lat == lat && lon == lon && alt == alt);
}

bool CoordWgs84::operator==(const CoordWgs84& other) const
{
    return lat == other.lat && lon == other.lon && alt == other.alt;
}

bool CoordWgs84::operator!=(const CoordWgs84& other) const
{
    return !(*this == other);
}
