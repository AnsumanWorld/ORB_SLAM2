#define _USE_MATH_DEFINES
#include <math.h>

#include "utils/slam_utils/include/CoordEcef.h"
#include "utils/slam_utils/include/CoordEnu.h"
#include "utils/slam_utils/include/CoordLocal.h"
#include "utils/slam_utils/include/CoordWgs84.h"
#include "utils/slam_utils/include/PoseWgs84.h"
#include "utils/slam_utils/include/SphericalCoordinates.h"
#include "utils/slam_utils/include/helper.h"

#include <iostream>

int main(int argc, char** argv)
{
    PoseWgs84 origin(deg2rad(12.0), deg2rad(77.0), 0.0, 0.0, 0.0, 0.0);
    CoordWgs84 coord(deg2rad(12.001), deg2rad(77.001), 0.0);
    CoordLocal coord_local(coord.toCoordLocal(origin));

    std::cout << coord_local.x << ", " << coord_local.y << ", " << coord_local.z
              << std::endl;

    return 0;
}
