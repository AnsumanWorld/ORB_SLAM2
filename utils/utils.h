#pragma once

#include <utility>
#include <vector>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <Eigen/Core>
#include <Eigen/StdVector>
#include <ext/messages.h>


namespace utils
{
    struct gps_subtitle
    {
        int			_frame_number;
        int64_t		_timestamp;
        std::string _gps_str;
    };
    
    struct gps_info {
        double _lat{ 0 };
        double _lon{ 0 };
        double _alt{ 0 };
        double _roll{ 0 };
        double _pitch{ 0 };
        double _yaw{ 0 };
    };
            
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

    ORB_SLAM2::ext::pos_info get_pos_info(gps_info org_gps_,gps_info cur_lgps_);        
    bool extract_gps_from_video(const std::string &video_file_, std::vector<gps_subtitle> &gps_subtitles_);
    bool lon_lat_from_string(const std::string &src, double &long_val, double &lat_val);

}

