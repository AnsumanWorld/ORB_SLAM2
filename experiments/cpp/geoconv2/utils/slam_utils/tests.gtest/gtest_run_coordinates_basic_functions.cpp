#include <gtest/gtest.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "utils/slam_utils/include/CoordEcef.h"
#include "utils/slam_utils/include/CoordEnu.h"
#include "utils/slam_utils/include/CoordLocal.h"
#include "utils/slam_utils/include/CoordWgs84.h"
#include "utils/slam_utils/include/PoseWgs84.h"
#include "utils/slam_utils/include/SphericalCoordinates.h"
#include "utils/slam_utils/include/helper.h"

// Naming conventions:

// file name = test suite name

namespace tests {            // <- all tests live in this namespace
namespace tests_gtest_slam { // <- namespace name = project name

// <- fixture name = gtest_<test suite name>
class gtest_run_coordinates_basic_functions : public ::testing::Test {
protected:
    void SetUp() override {
        // do some fixture setup here
    }

    void TearDown() override {
        // do some fixture teardown here
    }
};

TEST_F(gtest_run_coordinates_basic_functions, coordwgs84_constructor) {
    CoordWgs84 unitialized;
    CoordWgs84 coord(1.948284, 2.284827, 42.3);
    EXPECT_EQ(1.948284, coord.getLat());
    EXPECT_EQ(2.284827, coord.getLon());
    EXPECT_EQ(42.3, coord.getAlt());
    EXPECT_EQ(false, unitialized.isInitialized());
}

TEST_F(gtest_run_coordinates_basic_functions, coord_transformation) {
    CoordWgs84 coord1(47.8 * M_PI / 180.0, 23.29 * M_PI / 180.0, 290.0);
    CoordWgs84 coord2(47.42 * M_PI / 180.0, 22.49 * M_PI / 180.0, 290.0);
    CoordEcef coord1_ecef = coord1.toEcef();
    CoordEnu coord1_enu_A = coord1_ecef.toEnu(coord2);
    CoordEnu coord1_enu_B = coord1.toEnu(coord2);
    CoordEcef coord1_ecef2 = coord1_enu_A.toEcef(coord2);
    CoordWgs84 coord1_toEnuAndBack = coord1_ecef2.toWgs84();
    CoordWgs84 coord1_toEnuAndBack_direct = coord1_enu_B.toWgs84(coord2);
    CoordWgs84 coord1_toEcefAndBack = coord1_ecef.toWgs84();

    // Check conversion from Wgs84 to ECEF
    // Source for comparison values:
    // http://www.sysense.com/products/ecef_lla_converter/index.html
    EXPECT_NEAR(3942646.92, coord1_ecef.X, 0.1);
    EXPECT_NEAR(1697156.33, coord1_ecef.Y, 0.1);
    EXPECT_NEAR(4702182.44, coord1_ecef.Z, 0.1);

    // Check conversion from ECEF to ENU
    CoordEcef coord3(6300000.0, 0.0, 0.0);  // On the equator and prime meridian
    CoordEcef coord4(6300000.0, 0.0, 1.16); // 1.16m north of equator
    CoordWgs84 coord3_wgs84 = coord3.toWgs84();
    CoordEnu coord4_enu = coord4.toEnu(coord3);
    EXPECT_DOUBLE_EQ(0.0, coord4_enu.east);
    EXPECT_DOUBLE_EQ(1.16, coord4_enu.north);
    EXPECT_DOUBLE_EQ(0.0, coord4_enu.up);

    CoordEnu coord3_ref3 = coord3.toEnu(coord3);
    EXPECT_DOUBLE_EQ(0.0, coord3_ref3.east);
    EXPECT_DOUBLE_EQ(0.0, coord3_ref3.north);
    EXPECT_DOUBLE_EQ(0.0, coord3_ref3.up);

    EXPECT_DOUBLE_EQ(0.0, coord3_wgs84.getLat());
    EXPECT_DOUBLE_EQ(0.0, coord3_wgs84.getLon());
    EXPECT_NEAR(-78137.0, coord3_wgs84.getAlt(), 1e-7);

    EXPECT_DOUBLE_EQ(coord1.getLat(), coord1_toEcefAndBack.getLat());
    EXPECT_DOUBLE_EQ(coord1.getLon(), coord1_toEcefAndBack.getLon());
    EXPECT_NEAR(coord1.getAlt(), coord1_toEcefAndBack.getAlt(), 1e-7);

    EXPECT_DOUBLE_EQ(coord1.getLat(), coord1_toEnuAndBack.getLat());
    EXPECT_DOUBLE_EQ(coord1.getLon(), coord1_toEnuAndBack.getLon());
    EXPECT_NEAR(coord1.getAlt(), coord1_toEnuAndBack.getAlt(), 1e-7);

    EXPECT_DOUBLE_EQ(coord1.getLat(), coord1_toEnuAndBack_direct.getLat());
    EXPECT_DOUBLE_EQ(coord1.getLon(), coord1_toEnuAndBack_direct.getLon());
    EXPECT_NEAR(coord1.getAlt(), coord1_toEnuAndBack_direct.getAlt(), 1e-7);

    // Initialize in ENU frame, transform to WGS84 and back
    CoordEnu coord5_enu(2747.329, 93248.0, 93.02);
    CoordWgs84 coord5_wgs84(coord5_enu.toWgs84(coord1));
    CoordEnu coord5_enu_back(coord5_wgs84.toEnu(coord1));

    EXPECT_NEAR(coord5_enu.east, coord5_enu_back.east, 1e-5);
    EXPECT_NEAR(coord5_enu.north, coord5_enu_back.north, 1e-5);
    EXPECT_NEAR(coord5_enu.up, coord5_enu_back.up, 1e-5);

    const double up = 93.02;
    CoordEnu coord6_enu(0.0017, 0.034, up);
    CoordWgs84 coord6_wgs84(coord6_enu.toWgs84(coord1));

    EXPECT_NEAR(coord6_wgs84.getLat(), coord1.getLat(), 1e-5);
    EXPECT_NEAR(coord6_wgs84.getLon(), coord1.getLon(), 1e-5);
    EXPECT_NEAR(coord6_wgs84.getAlt(), coord1.getAlt() + up, 1e-5);
}

TEST_F(gtest_run_coordinates_basic_functions, pose_transformation) {
    PoseWgs84 p1(47.8 * M_PI / 180.0, 23.29 * M_PI / 180.0, 290.0, 0.0, 0.0,
                 0.0);
    // Transform to local frame with unit orientation
    CoordWgs84 coord2(47.42 * M_PI / 180.0, 22.49 * M_PI / 180.0, 294.0);
    CoordEnu coord2_enu(coord2.toEnu(p1));
    CoordLocal coord2_local(coord2.toCoordLocal(p1));

    EXPECT_DOUBLE_EQ(coord2_enu.east, coord2_local.x);
    EXPECT_DOUBLE_EQ(coord2_enu.north, coord2_local.y);
    EXPECT_DOUBLE_EQ(coord2_enu.up, coord2_local.z);

    // Transform to local frame rotated around up axis
    PoseWgs84 p3(47.8 * M_PI / 180.0, 23.29 * M_PI / 180.0, 290.0, M_PI / 2.0,
                 0.0, 0.0);
    CoordEnu coord4_enu(2747.329, 3248.0, 93.02);
    CoordWgs84 coord4_wgs84(coord4_enu.toWgs84(p3));
    CoordLocal coord4_local(coord4_wgs84.toCoordLocal(p3));

    EXPECT_NEAR(coord4_enu.east, -coord4_local.y, 1e-5);
    EXPECT_NEAR(coord4_enu.north, coord4_local.x, 1e-5);
    EXPECT_NEAR(coord4_enu.up, coord4_local.z, 1e-5);
}

TEST_F(gtest_run_coordinates_basic_functions, coord_inclination) {
    const CoordLocal c1(0, 0, 0);
    // 45 degree slope
    const CoordLocal c2(1, 0, 1);
    EXPECT_DOUBLE_EQ(c1.elevationTo(c2), M_PI / 4.0);
    // Both in x-y-plane
    const CoordLocal c3(1, 4.3, 0);
    EXPECT_DOUBLE_EQ(c1.elevationTo(c3), 0.0);
    // Other above ego coordinate
    const CoordLocal c4(0, 0, 3.0);
    EXPECT_DOUBLE_EQ(c1.elevationTo(c4), M_PI / 2.0);
}

TEST_F(gtest_run_coordinates_basic_functions, spherical_coordinates) {
    // 45 degree slope
    const CoordLocal c2(1, 0, 1);
    const SphericalCoordinates c2_sph = c2.toSphericalCoordinates();
    EXPECT_DOUBLE_EQ(c2_sph.distance, sqrt(2));
    EXPECT_DOUBLE_EQ(c2_sph.azimuth, 0);
    EXPECT_DOUBLE_EQ(c2_sph.inclination, deg2rad(45.0));
    // Point in x-y-plane
    const CoordLocal c3(2, 2, 0);
    const SphericalCoordinates c3_sph = c3.toSphericalCoordinates();
    EXPECT_DOUBLE_EQ(c3_sph.distance, sqrt(2.0 * 2.0 + 2.0 * 2.0));
    EXPECT_DOUBLE_EQ(c3_sph.azimuth, deg2rad(45.0));
    EXPECT_DOUBLE_EQ(c3_sph.inclination, 0.0);
    // On vertical axis
    const CoordLocal c4(0, 0, 3.0);
    const SphericalCoordinates c4_sph = c4.toSphericalCoordinates();
    EXPECT_DOUBLE_EQ(c4_sph.distance, 3.0);
    EXPECT_DOUBLE_EQ(c4_sph.azimuth, 0.0);
    EXPECT_DOUBLE_EQ(c4_sph.inclination, deg2rad(90.0));
}
} // namespace tests_gtest_slam
} // namespace tests