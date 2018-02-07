#include <gtest/gtest.h>

#include "ext/GeodeticCoords.h"

/*
Reference: http://www.whoi.edu/marine/ndsf/cgi-bin/NDSFutility.cgi?form=0&from=LatLon&to=XY
*/

namespace ORB_SLAM2
{
namespace tests
{
TEST(test_coord_conv, geo_to_local_origin)
{
    using namespace ORB_SLAM2::ext;

    GeodeticCoords origin(12.0, 77.0, 0.0);
    GeodeticCoords cord(12.0, 77.0, 0.0);
    auto local = cord.toENU_WGS84(origin);

    EXPECT_NEAR(0.0, local.x, 1e-6);
    EXPECT_NEAR(0.0, local.y, 1e-6);
}

TEST(test_coord_conv, geo_to_local_normal)
{
    using namespace ORB_SLAM2::ext;
    
    GeodeticCoords origin(12.0, 77.0, 0.0);
    GeodeticCoords cord(12.001, 77.001, 0.0);
    auto local = cord.toENU_WGS84(origin);

    EXPECT_NEAR(108.902249, local.x, 1e-6);
    EXPECT_NEAR(110.622491, local.y, 1e-6);
}

TEST(test_coord_conv, local_to_geo_origin)
{
    using namespace ORB_SLAM2::ext;
    
    GeodeticCoords origin(12.0, 77.0, 0.0);
    cv::Point3d local_cord(0.0, 0.0, 0.0);

    GeodeticCoords local_to_geo;
    local_to_geo.fromENU_WGS84(local_cord, origin);

    EXPECT_NEAR(12.0, local_to_geo.latitude(), 1e-6);
    EXPECT_NEAR(77.0, local_to_geo.longitude(), 1e-6);
}

TEST(test_coord_conv, local_to_geo_normal)
{
    using namespace ORB_SLAM2::ext;
    
    GeodeticCoords origin(12.0, 77.0, 0.0);
    cv::Point3d local_cord(108.902249, 110.622491, 0.0);

    GeodeticCoords local_to_geo;
    local_to_geo.fromENU_WGS84(local_cord, origin);

    EXPECT_NEAR(12.001, local_to_geo.latitude(), 1e-6);
    EXPECT_NEAR(77.001, local_to_geo.longitude(), 1e-6);
}
} // namespace tests
} // namespace ORB_SLAM2
