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

int main(int argc, char** argv) {
    PoseWgs84 origin(deg2rad(12.0), deg2rad(77.0), 0.0, 0.0, 0.0, 0.0);

    CoordLocal coord(108.902, 110.622, -0.00189494);
    CoordWgs84 coord_wgs84(coord.toWgs84(origin));
    std::cout << rad2deg(coord_wgs84.getLat()) << ", " << rad2deg(coord_wgs84.getLon()) << ", " << coord_wgs84.getAlt()
              << std::endl;

    return 0;
}
