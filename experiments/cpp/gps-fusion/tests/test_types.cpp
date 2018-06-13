#include "precompile.hpp"

#include "types.hpp"

namespace ORB_SLAM2 {
namespace experiments {
namespace tests {
TEST(types_test, pose_write_with_geo) {
    using namespace ORB_SLAM2::experiments;
    pose p;
    p.x(1.0);
    p.y(2.0);
    p.z(3.0);
    p.a(4.0);
    p.b(5.0);
    p.c(6.0);
    p.d(7.0);
    p.latitude(8.0);
    p.longitude(9.0);
    std::stringstream ss;
    p.write(ss);

    std::string expected("1 2 3 4 5 6 7 8 9 ");
    EXPECT_EQ(expected, ss.str());
}

TEST(types_test, pose_read_with_geo) {
    std::string input("9999 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0");
    std::stringstream ss(input);

    using namespace ORB_SLAM2::experiments;
    pose p;
    p.read(ss);

    EXPECT_DOUBLE_EQ(1.0, p.x());
    EXPECT_DOUBLE_EQ(2.0, p.y());
    EXPECT_DOUBLE_EQ(3.0, p.z());
    EXPECT_DOUBLE_EQ(4.0, p.a());
    EXPECT_DOUBLE_EQ(5.0, p.b());
    EXPECT_DOUBLE_EQ(6.0, p.c());
    EXPECT_DOUBLE_EQ(7.0, p.d());
    EXPECT_DOUBLE_EQ(8.0, p.latitude());
    EXPECT_DOUBLE_EQ(9.0, p.longitude());
}
} // namespace tests
} // namespace experiments
} // namespace ORB_SLAM2
