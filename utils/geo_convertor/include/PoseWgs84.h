#pragma once

#include <limits>

#include "CoordWgs84.h"

class CoordLocal;
class PoseLocal;
class PoseEnu;

class PoseWgs84 : public CoordWgs84 {
public:
    PoseWgs84();
    PoseWgs84(double lat, double lon, double alt = 0);
    PoseWgs84(double lat, double lon, double alt, double heading, double pitch,
              double roll);

    PoseEnu toPoseEnu(const CoordWgs84& reference) const;
    // PoseLocal toPoseLocal(const PoseWgs84& reference) const;
    // CoordEnu toEnu(const CoordWgs84& reference) const;

    double getHeading() const;
    double getPitch() const;
    double getRoll() const;
    bool isInitialized() const override;

private:
    double heading;
    double pitch;
    double roll;
};