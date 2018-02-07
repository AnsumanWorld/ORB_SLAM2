#include "utils.hpp"

namespace ORB_SLAM2
{
namespace experiments
{
double utils::variance(const std::vector<double>& x)
{
    using namespace boost::accumulators;
    accumulator_set<double, stats<tag::variance>> acc;

    for (auto item : x)
        acc(item);

    return boost::accumulators::variance(acc);
}

double utils::deg_to_rad(const double& deg)
{
    return deg * M_PI / 180.0;
}

double utils::geo_haversine_distance(const double& lat1d, const double& lon1d,
                           const double& lat2d, const double& lon2d)
{
    auto earth_radius = 6371.0;
    auto lat1r = deg_to_rad(lat1d);
    auto lon1r = deg_to_rad(lon1d);
    auto lat2r = deg_to_rad(lat2d);
    auto lon2r = deg_to_rad(lon2d);
    auto u = sin((lat2r - lat1r) / 2);
    auto v = sin((lon2r - lon1r) / 2);
    return 2.0 * earth_radius *
           asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
}

double utils::euclidean_distance(const double& x1, const double& y1,
                                 const double& x2, const double& y2)
{
    auto delta_x = x1 - x2;
    auto delta_y = y1 - y2;
    auto sum_of_sqr = pow(delta_x, 2) + pow(delta_y, 2);
    return sqrt(sum_of_sqr);
}

std::vector<std::pair<double, double>>
utils::generate_circle(const std::size_t& points, const double& radius,
                       const double& center_x, const double& center_y)
{
    std::vector<std::pair<double, double>> circle_points;
    circle_points.reserve(points);

    auto slice = 2 * M_PI / points;
    for (std::size_t i = 0; i < points; ++i)
    {
        auto angle = slice * i;
        auto px = center_x + radius * cos(angle);
        auto py = center_y + radius * sin(angle);
        auto point = std::make_pair(px, py);
        circle_points.push_back(point);
    }

    return circle_points;
}

bool utils::has_suffix(const std::string& str, const std::string& suffix)
{
    std::size_t index = str.find(suffix, str.size() - suffix.size());
    return (index != std::string::npos);
}

std::string utils::tid_to_string(const std::thread::id& tid)
{
    std::stringstream ss;
    ss << tid;
    return ss.str();
}
} // namespace experiments
} // namespace ORB_SLAM2
