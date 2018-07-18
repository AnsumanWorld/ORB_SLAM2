#include "utils.h"
#include "geo_convertor/include/CoordEcef.h"
#include "geo_convertor/include/CoordEnu.h"
#include "geo_convertor/include/CoordLocal.h"
#include "geo_convertor/include/CoordWgs84.h"
#include "geo_convertor/include/PoseWgs84.h"
#include "geo_convertor/include/SphericalCoordinates.h"
#include <fstream>
#include <iostream>
namespace utils
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

void dump_data(std::string filename, CoordLocal &coord_local)
{
    std::fstream out;
    out.open(filename, std::fstream::out | std::fstream::app);
    out << -coord_local.y <<" " << coord_local.x << " " << -coord_local.z<<std::endl;
    out.close();
}

ORB_SLAM2::ext::pos_info get_pos_info(gps_info org_gps_, gps_info cur_lgps_)
{
    PoseWgs84 origin(deg_to_rad(org_gps_._lat), deg_to_rad(org_gps_._lon), org_gps_._alt, org_gps_._yaw, org_gps_._pitch, org_gps_._roll);
    CoordWgs84 coord(deg_to_rad(cur_lgps_._lat), deg_to_rad(cur_lgps_._lon), cur_lgps_._alt);
    CoordLocal coord_local(coord.toCoordLocal(origin));
    dump_data("kitty_07.txt", coord_local);
    return { Eigen::Vector3d(-coord_local.y,coord_local.x ,-coord_local.z) ,(Eigen::Matrix3d::Identity() * 0.01) };
}

} // namespace utils
