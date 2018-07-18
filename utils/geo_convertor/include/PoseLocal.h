#pragma once

#include "CoordLocal.h"

class CoordWgs84;

class PoseLocal : public CoordLocal {
public:
    PoseLocal();
    PoseLocal(double x, double y, double z);
    PoseLocal(double x, double y, double z, double heading, double pitch,
              double roll);

    double heading;
    double pitch;
    double roll;
};