#include "precompile.hpp"

#include "utils/utils.hpp"

namespace ORB_SLAM2
{
namespace experiments
{
namespace tests
{
TEST(utils_test, variance_simple)
{
    using namespace ORB_SLAM2::experiments;
    std::vector<double> data{1.0, 2.0, 3.0, 4.0, 5.0};
    EXPECT_DOUBLE_EQ(2.0, utils::variance(data));
}

TEST(utils_test, euclidean_distance)
{
    using namespace ORB_SLAM2::experiments;
    {
        auto dist = utils::euclidean_distance(1.0, 1.0, 1.0, 1.0);
        EXPECT_DOUBLE_EQ(0.0, dist);
    }
    {
        auto dist = utils::euclidean_distance(-7.0, -4.0, 17.0, 6.5);
        EXPECT_NEAR(26.196374, dist, 0.0001);
    }
}

TEST(utils_test, geo_haversine_distance)
{
    using namespace ORB_SLAM2::experiments;
    {
        auto dist = utils::geo_haversine_distance(0.0, 0.0, 0.0, 0.0);
        EXPECT_DOUBLE_EQ(0.0, dist);
    }
    {
        auto dist =
            utils::geo_haversine_distance(12.958380, 77.545251, 48.324749, 11.451503);
        EXPECT_NEAR(7175.1216, dist, 0.0001);
    }
}
} // namespace tests
} // namespace experiments
} // namespace ORB_SLAM2
