#pragma once
#include <iostream>
#include <boost/algorithm/string.hpp>

namespace utils {
    enum class gps_offset {
        active_gps = 9,
        date,time, 
        long_val, 
        long_direction, 
        lat_val, 
        lat_direction 
    };


    //convert gps value(degree minutes and second(dms) to degree decimal(dd))
    double decimal_degrees_from_nmea_string(const std::string &gps_str)
    {
        double gps_val = stod(gps_str);
        return (((int)gps_val / 100) + ( (gps_val - (((int)gps_val / 100) * 100)) / 60));
    }

    bool valid_gps(const std::string &src)
    {
        std::vector<std::string> result;
        boost::split(result, src, boost::is_any_of(","));
        return ((20 <= result.size()) && (!result[(int)gps_offset::active_gps].compare("A")));
    }

    bool lon_lat_from_string(const std::string &src, double &long_val, double &lat_val)
    {
        bool gps_status = false;
        int long_direction = 1;
        int lat_direction = 1;

        std::vector<std::string> result;
        boost::split(result, src, boost::is_any_of(","));
        
        //check for character A or V from gps string
        //A for active(is good)
        //V for void(there's an unexpected problem with the position reading.)
        if (valid_gps(src))
        {
            std::string long_str = result[(int)gps_offset::long_val];
            std::string lat_str = result[(int)gps_offset::lat_val];

            if (!result[(int)gps_offset::long_direction].compare("S"))
                long_direction = -1;

            if (!result[(int)gps_offset::lat_direction].compare("W"))
                lat_direction = -1;

            if ((false == long_str.empty()) && (false == lat_str.empty()))
            {
                gps_status = true;
                long_val = decimal_degrees_from_nmea_string(long_str) * long_direction;
                lat_val = decimal_degrees_from_nmea_string(lat_str) * lat_direction;
            }

        }
        return gps_status;
    }
}