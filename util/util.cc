#include "util.h"

namespace util
{
double deg_to_rad(const double& deg)
{
    return deg * M_PI / 180.0;
}

double rad_to_deg(const double& rad)
{
    return rad * 180.0 / M_PI;
}

double geo_haversine_distance(const double& lat1d, const double& lon1d,
                           const double& lat2d, const double& lon2d)
{
    const static auto earth_radius = 6371000.0; //m
    auto lat1r = deg_to_rad(lat1d);
    auto lon1r = deg_to_rad(lon1d);
    auto lat2r = deg_to_rad(lat2d);
    auto lon2r = deg_to_rad(lon2d);
    auto u = sin((lat2r - lat1r) / 2);
    auto v = sin((lon2r - lon1r) / 2);
    return 2.0 * earth_radius *
           asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
}

double geo_euclidean_distance(const double& lat1d, const double& lon1d,
                    const double& lat2d, const double& lon2d)
{
    const static auto earth_radius = 6371000.0; //m
    auto slat = deg_to_rad(lat1d);
    auto slon = deg_to_rad(lon1d);
    auto elat = deg_to_rad(lat2d);
    auto elon = deg_to_rad(lon2d);

    auto start_x = earth_radius * cos(slat) * cos(slon);
    auto start_y = earth_radius * cos(slat) * sin(slon);
    auto start_z = earth_radius * sin(slat);

    auto end_x = earth_radius * cos(elat) * cos(elon);
    auto end_y = earth_radius * cos(elat) * sin(elon);
    auto end_z = earth_radius * sin(elat);

    return euclidean_distance(start_x, start_y, start_z,
                                end_x, end_y, end_z);
}

double euclidean_distance(
    const double& x1, const double& y1, const double& z1,
    const double& x2, const double& y2, const double& z2)
{
    auto delta_x = x1 - x2;
    auto delta_y = y1 - y2;
    auto delta_z = z1 - z2;
    auto sum_of_sqr = pow(delta_x, 2) + pow(delta_y, 2) + pow(delta_z, 2);
    return sqrt(sum_of_sqr);
}

std::pair<double, double> estimate_geo(
    const double& latitude, 
    const double& longitude,
    const double& dx, 
    const double& dy)
{
    const static auto r_earth = 6371000.0;
    auto new_latitude = latitude + (dy / r_earth) * (180.0 / M_PI);
    auto new_longitude = longitude + (dx / r_earth) * (180.0 / M_PI) / cos(latitude * M_PI / 180.0);
    return std::make_pair(new_latitude, new_longitude);
}
} // namespace util
