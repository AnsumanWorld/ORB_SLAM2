#include "precompile.hpp"

namespace ORB_SLAM2
{
namespace experiments
{
namespace tests
{
TEST(test_g2o, test_EdgeSE3ProjectXYZ_read)
{
    using namespace ORB_SLAM2::experiments;

    g2o::SparseOptimizer optimizer;
    std::string sample_g2o =
        R"(EDGE_SE3_PROJECT_XYZ:EXPMAP 13160 86 758.486 233.045  0.334898 0 0.334898 759.149 750.265 621.608 392.603
    EDGE_SE3_PROJECT_XYZ:EXPMAP 13160 85 791.103 229.577  0.232568 0 0.232568 759.149 750.265 621.608 392.603)";

    std::stringstream ss(sample_g2o);
    bool is_loaded = optimizer.load(ss);
    optimizer.initializeOptimization();
    ASSERT_TRUE(is_loaded);

    for (auto edge : optimizer.edges())
    {
        auto edge_lm = dynamic_cast<g2o::EdgeSE3ProjectXYZ*>(edge);
        if (edge_lm != nullptr)
        {
            EXPECT_DOUBLE_EQ(edge_lm->fx, 759.149);
            EXPECT_DOUBLE_EQ(edge_lm->fy, 750.265);
            EXPECT_DOUBLE_EQ(edge_lm->cx, 621.608);
            EXPECT_DOUBLE_EQ(edge_lm->cy, 392.603);
        }
    }
}
} // namespace tests
} // namespace experiments
} // namespace ORB_SLAM2
