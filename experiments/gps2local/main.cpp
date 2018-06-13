#include <fstream>
#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <sstream>
#include <iomanip>

#include "ext/GeodeticCoords.h"
#include "util/util.h"

std::vector<std::string> split(const std::string &s, const char delim)
{
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
        result.push_back(item);
    return result;
}

int main(int argc, char** argv)
{
    if(argc != 3)
        throw std::invalid_argument("usage: gps2local gps.txt rotation-in-deg");

    std::string file = argv[1];
    auto theta = std::stod(argv[2]);
    using namespace ORB_SLAM2::ext;

    std::ifstream ifile(file.c_str());
    if(!ifile.good())
        throw std::runtime_error("unable to open file:" + file);

    std::ofstream ofile("gps2local.txt");
    std::string eachline;
    while(std::getline(ifile, eachline))
    {
        auto items = split(eachline, ' ');
        
        if(items.size() != 3)
            throw std::runtime_error("invalid line");

        auto lati = std::stod(items[0]);
        auto longi = std::stod(items[1]);
        auto alti  = std::stod(items[2]);
        static GeodeticCoords origin(lati, longi, alti);
        
        GeodeticCoords coord(lati, longi, alti);
        
        auto local = coord.toENU_WGS84(origin);
        auto rotated = local;

        // // Rotate along x axis
        // rotated.x = local.x;
        // rotated.y = local.y * cos(theta) - local.z * sin(theta);
        // rotated.z = local.y * sin(theta) + local.z * cos(theta);

        // // Rotate along y axis
        // rotated.x = local.x * cos(theta) + local.z * sin(theta);
        // rotated.y = local.y;
        // rotated.z = local.z * cos(theta) - local.x * sin(theta);

        // Rotate along z axis
        rotated.x = local.x * cos(theta) - local.y * sin(theta);
        rotated.y = local.x * sin(theta) + local.y * cos(theta);
        rotated.z = local.z;
        ofile << std::setprecision(15) << rotated.x << " " << rotated.y << " " << rotated.z << std::endl;
    }
   
    return 0;
}
