#pragma once

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

#include <random>
#include <thread>
#include <utility>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

namespace ORB_SLAM2 {
namespace experiments {
class util {
public:
    static double variance(const std::vector<double>&);
    static double deg_to_rad(const double&);
    static double
    geo_haversine_distance(const double& lat1d, const double& lon1d,
                           const double& lat2d, const double& lon2d);
    static double euclidean_distance(const double& x1, const double& y1,
                                     const double& x2, const double& y2);
    static std::vector<std::pair<double, double>>
    generate_circle(const std::size_t& points, const double& radius,
                    const double& center_x, const double& center_y);
    static bool has_suffix(const std::string&, const std::string&);
    static std::string tid_to_string(const std::thread::id&);
}; // class util
} // namespace experiments
} // namespace ORB_SLAM2
