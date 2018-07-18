#pragma once

#include "CoordEnu.h"

class CoordWgs84;
class CoordEcef;

class PoseEnu : public CoordEnu {
public:
    PoseEnu();
    PoseEnu(double e, double n, double u, double heading, double pitch,
            double roll);

    // Heading east:0, north:pi/2, west:pi (counter-clockwise)
    double heading;
    double pitch;
    double roll;
};