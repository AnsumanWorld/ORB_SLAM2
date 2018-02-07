#include "ext/GeodeticCoords.h"

#include <iostream>

/*
Reference: http://www.whoi.edu/marine/ndsf/cgi-bin/NDSFutility.cgi?form=0&from=LatLon&to=XY
*/

int main(int, char**)
{
    using namespace ORB_SLAM2::ext;

    GeodeticCoords origin(12.0, 77.0, 0.0);

    /* geo to local */
    GeodeticCoords cord(12.001, 77.001, 0.0);
    std::cout << "toENU_WGS84 = " << cord.toENU_WGS84(origin) << std::endl;

    /* local to geo */
    cv::Point3d local_cord(108.902, 110.622, 0.0);
    GeodeticCoords local_to_geo;
    local_to_geo.fromENU_WGS84(local_cord, origin);
    std::cout << "fromENU_WGS84.latitude = " << local_to_geo.latitude() << std::endl;
    std::cout << "fromENU_WGS84.longitude = " << local_to_geo.longitude() << std::endl;
    std::cout << "fromENU_WGS84.altitude = " << local_to_geo.altitude() << std::endl;
    
    return 0;
}
