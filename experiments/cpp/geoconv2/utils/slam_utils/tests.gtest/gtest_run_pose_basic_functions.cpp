#include <gtest/gtest.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "utils/slam_utils/include/Point3.h"
#include "utils/slam_utils/include/SE3.h"
#include "utils/slam_utils/include/YawPitchRoll.h"
#include "utils/slam_utils/include/helper.h"

// Naming conventions:

// file name = test suite name

namespace tests {            // <- all tests live in this namespace
namespace tests_gtest_slam { // <- namespace name = project name

// <- fixture name = gtest_<test suite name>
class gtest_run_pose_basic_functions : public ::testing::Test {
protected:
    void SetUp() override {
        // do some fixture setup here
    }

    void TearDown() override {
        // do some fixture teardown here
    }
};

TEST_F(gtest_run_pose_basic_functions, test_deg2rad) {
    EXPECT_DOUBLE_EQ(deg2rad(0), 0);
    EXPECT_DOUBLE_EQ(deg2rad(90), 0.5 * M_PI);
    EXPECT_DOUBLE_EQ(deg2rad(-90), -0.5 * M_PI);
    EXPECT_DOUBLE_EQ(deg2rad(180), M_PI);
    EXPECT_DOUBLE_EQ(deg2rad(45), 0.25 * M_PI);

    EXPECT_DOUBLE_EQ(rad2deg(0), 0);
    EXPECT_DOUBLE_EQ(rad2deg(0.5 * M_PI), 90);
    EXPECT_DOUBLE_EQ(rad2deg(-0.5 * M_PI), -90);
    EXPECT_DOUBLE_EQ(rad2deg(M_PI), 180);
    EXPECT_DOUBLE_EQ(rad2deg(0.25 * M_PI), 45);

    double x = 348.2421;
    EXPECT_DOUBLE_EQ(rad2deg(deg2rad(x)), x);
    x = 0.04824;
    EXPECT_DOUBLE_EQ(rad2deg(deg2rad(x)), x);
    x = 172.29;
    EXPECT_DOUBLE_EQ(rad2deg(deg2rad(x)), x);
    x = 15.2948;
    EXPECT_DOUBLE_EQ(rad2deg(deg2rad(x)), x);
}

TEST_F(gtest_run_pose_basic_functions, test_wrap2pi) {
    EXPECT_DOUBLE_EQ(wrap2pi(0), 0);
    EXPECT_DOUBLE_EQ(wrap2pi(2 * M_PI), 0);
    EXPECT_NEAR(rad2deg(wrap2pi(deg2rad(-361))), -1, 1e-10);
    EXPECT_DOUBLE_EQ(rad2deg(wrap2pi(deg2rad(53))), 53);
    EXPECT_DOUBLE_EQ(rad2deg(wrap2pi(deg2rad(179))), 179);
    EXPECT_DOUBLE_EQ(rad2deg(wrap2pi(deg2rad(180))), 180);
    EXPECT_DOUBLE_EQ(rad2deg(wrap2pi(deg2rad(181))), -179);
    EXPECT_NEAR(rad2deg(wrap2pi(deg2rad(361))), 1, 1e-10);
    EXPECT_NEAR(rad2deg(wrap2pi(deg2rad(539))), 179, 1e-10);
    EXPECT_NEAR(rad2deg(wrap2pi(deg2rad(540))), 180, 1e-10);
    EXPECT_NEAR(rad2deg(wrap2pi(deg2rad(541))), -179, 1e-10);
}

TEST_F(gtest_run_pose_basic_functions, test_se3) {
    SE3 se3(0, 0, 0, 0, 0, 0);
    YawPitchRoll ypr = se3.toYawPitchRoll();
    EXPECT_DOUBLE_EQ(ypr.yaw, 0.0);
    EXPECT_DOUBLE_EQ(ypr.pitch, 0.0);
    EXPECT_DOUBLE_EQ(ypr.roll, 0.0);

    se3 = SE3(0, 0, 0, deg2rad(90), 0, 0);
    ypr = se3.toYawPitchRoll();
    EXPECT_DOUBLE_EQ(ypr.yaw, deg2rad(90));
    EXPECT_DOUBLE_EQ(ypr.pitch, 0.0);
    EXPECT_DOUBLE_EQ(ypr.roll, 0.0);

    se3 = SE3(0, 0, 0, 0, deg2rad(70), 0);
    ypr = se3.toYawPitchRoll();
    EXPECT_DOUBLE_EQ(ypr.yaw, 0.0);
    EXPECT_DOUBLE_EQ(ypr.pitch, deg2rad(70));
    EXPECT_DOUBLE_EQ(ypr.roll, 0.0);

    se3 = SE3(0, 0, 0, 0, 0, deg2rad(70));
    ypr = se3.toYawPitchRoll();
    EXPECT_DOUBLE_EQ(ypr.yaw, 0.0);
    EXPECT_DOUBLE_EQ(ypr.pitch, 0.0);
    EXPECT_DOUBLE_EQ(ypr.roll, deg2rad(70));

    double yaw = deg2rad(37.5);
    double pitch = deg2rad(17.4);
    double roll = deg2rad(49.02);
    se3 = SE3(0, 0, 0, yaw, pitch, roll);
    ypr = se3.toYawPitchRoll();
    EXPECT_NEAR(ypr.yaw, yaw, 1e-10);
    EXPECT_NEAR(ypr.pitch, pitch, 1e-10);
    EXPECT_NEAR(ypr.roll, roll, 1e-10);

    yaw = deg2rad(0.0429);
    pitch = deg2rad(38.392);
    roll = deg2rad(5.02);
    se3 = SE3(0, 0, 0, yaw, pitch, roll);
    ypr = se3.toYawPitchRoll();
    EXPECT_NEAR(ypr.yaw, yaw, 1e-10);
    EXPECT_NEAR(ypr.pitch, pitch, 1e-10);
    EXPECT_NEAR(ypr.roll, roll, 1e-10);

    double x = 398, y = 39.20, z = 0.05;
    se3 = SE3(x, y, z, yaw, pitch, roll);
    Eigen::Vector3d vec = se3.translation();
    EXPECT_DOUBLE_EQ(vec[0], x);
    EXPECT_DOUBLE_EQ(vec[1], y);
    EXPECT_DOUBLE_EQ(vec[2], z);

    SE3 se3_2(x, y, z, 0, 0, 0);
    SE3 se3_3 = se3_2 * se3;
    vec = se3_3.translation();
    EXPECT_DOUBLE_EQ(vec[0], 2 * x);
    EXPECT_DOUBLE_EQ(vec[1], 2 * y);
    EXPECT_DOUBLE_EQ(vec[2], 2 * z);

    se3 = SE3(x, y, z, deg2rad(45), 0, 0);
    se3_3 = se3 * se3_2;
    vec = se3_3.translation();
    EXPECT_DOUBLE_EQ(vec[0], sqrt(0.5) * x - sqrt(0.5) * y + x);
    EXPECT_DOUBLE_EQ(vec[1], sqrt(0.5) * x + sqrt(0.5) * y + y);
    EXPECT_DOUBLE_EQ(vec[2], 2 * z);

    vec = Eigen::Vector3d(394.39, 934.234, 3.110);
    se3 = SE3(x, y, z, yaw, pitch, roll);
    Eigen::Vector3d vec_out = se3.inverse() * se3 * vec;
    EXPECT_NEAR(vec_out[0], vec[0], 1e-10);
    EXPECT_NEAR(vec_out[1], vec[1], 1e-10);
    EXPECT_NEAR(vec_out[2], vec[2], 1e-10);
    vec_out = se3 * se3.inverse() * vec;
    EXPECT_NEAR(vec_out[0], vec[0], 1e-10);
    EXPECT_NEAR(vec_out[1], vec[1], 1e-10);
    EXPECT_NEAR(vec_out[2], vec[2], 1e-10);
}
} // namespace tests_gtest_slam
} // namespace tests