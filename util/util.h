#pragma once

#include <utility>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

namespace util
{
double deg_to_rad(const double&);
double rad_to_deg(const double&);
double geo_haversine_distance(const double& lat1d, const double& lon1d,
                    const double& lat2d, const double& lon2d);
double geo_euclidean_distance(const double& lat1d, const double& lon1d,
                    const double& lat2d, const double& lon2d);

double euclidean_distance(
    const double& x1, const double& y1, const double& z1,
    const double& x2, const double& y2, const double& z2);
std::pair<double, double> estimate_geo(
    const double& latitude, 
    const double& longitude,
    const double& dx, 
    const double& dy);
}
