#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ext/GeodeticCoords.h"

std::vector<std::string> split(const std::string& s, const char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
        result.push_back(item);
    return result;
}

int main(int argc, char** argv) {
    if (argc != 5)
        throw std::invalid_argument("usage: gt2gps gt.txt lati longi alti");

    std::string file = argv[1];
    auto lati = std::stod(argv[2]);
    auto longi = std::stod(argv[3]);
    auto alti = std::stod(argv[4]);
    using namespace ORB_SLAM2::ext;
    GeodeticCoords origin(lati, longi, alti);

    std::ifstream ifile(file.c_str());
    if (!ifile.good())
        throw std::runtime_error("unable to open file:" + file);

    std::ofstream ofile("gt2gps.txt");
    std::string eachline;
    while (std::getline(ifile, eachline)) {
        auto items = split(eachline, ' ');

        if (items.size() != 12)
            throw std::runtime_error("invalid line");

        auto x = std::stod(items[3]);
        auto y = std::stod(items[7]);
        auto z = std::stod(items[11]);

        cv::Point3d local_cord(x, z, y);

        GeodeticCoords local_to_geo;
        local_to_geo.fromENU_WGS84(local_cord, origin);

        ofile << std::setprecision(15) << x << " " << y << " " << z;
        ofile << " " << local_to_geo.latitude() << " "
              << local_to_geo.longitude() << " " << local_to_geo.altitude()
              << std::endl;
    }

    return 0;
}
