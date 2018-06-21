#include "PoseWgs84.h"
#include <limits>
#include "CoordLocal.h"
#include "PoseEnu.h"
#include "PoseLocal.h"

PoseWgs84::PoseWgs84()
    : CoordWgs84()
    , heading(std::numeric_limits<double>::quiet_NaN())
    , pitch(std::numeric_limits<double>::quiet_NaN())
    , roll(std::numeric_limits<double>::quiet_NaN())
{
}

PoseWgs84::PoseWgs84(double lat, double lon, double alt)
    : CoordWgs84(lat, lon, alt)
{
}

PoseWgs84::PoseWgs84(double lat, double lon, double alt, double heading,
                     double pitch, double roll)
    : CoordWgs84(lat, lon, alt), heading(heading), pitch(pitch), roll(roll)
{
}

PoseEnu PoseWgs84::toPoseEnu(const CoordWgs84& reference) const
{
    CoordEnu c(this->toEnu(reference));
    return PoseEnu(c.east, c.north, c.up, heading, pitch, roll);
}

// TODO
// PoseLocal PoseWgs84::toPoseLocal(const PoseWgs84 & reference) const
//{
//    // Get coordinate in local frame
//    CoordLocal c(this->toCoordLocal(reference));
//    // Rotate angles around orientation of reference pose
//    // For this purpose, multiply both rotation matrices
//    return PoseLocal();
//}

double PoseWgs84::getHeading() const
{
    return heading;
}
double PoseWgs84::getPitch() const
{
    return pitch;
}
double PoseWgs84::getRoll() const
{
    return roll;
}

bool PoseWgs84::isInitialized() const
{
    return (CoordWgs84::isInitialized() && heading == heading &&
            pitch == pitch && roll == roll);
}
