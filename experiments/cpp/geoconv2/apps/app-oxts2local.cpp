#define _USE_MATH_DEFINES
#include <math.h>

#include "utils/slam_utils/include/CoordEcef.h"
#include "utils/slam_utils/include/CoordEnu.h"
#include "utils/slam_utils/include/CoordLocal.h"
#include "utils/slam_utils/include/CoordWgs84.h"
#include "utils/slam_utils/include/PoseWgs84.h"
#include "utils/slam_utils/include/SphericalCoordinates.h"
#include "utils/slam_utils/include/helper.h"

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include <exception>
#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace fs = boost::filesystem;

using geo_t = std::tuple<double, double, double>;
using rotation_t = std::tuple<double, double, double>;
using point3d_t = std::tuple<double, double, double>;
using sensor_data_t = std::tuple<geo_t, rotation_t>;

void check_existence(const std::initializer_list<fs::path>& paths) {
    std::for_each(paths.begin(), paths.end(), [&](const fs::path& path) {
        if (!fs::exists(path))
            throw std::runtime_error("invalid path:" + path.generic_string());
    });
}

std::vector<fs::path>
get_files(const fs::path& folder, const std::string& ext) {
    std::vector<fs::path> files;

    if (!fs::exists(folder) || !fs::is_directory(folder))
        throw std::runtime_error("invalid folder:" + folder.generic_string());

    auto range = boost::make_iterator_range(fs::directory_iterator(folder), {});
    for (auto& entry : range) {
        if (fs::is_regular_file(entry) && entry.path().extension() == ext)
            files.push_back(entry.path());
    }

    std::sort(files.begin(), files.end());
    return files;
}

std::vector<std::string> split(const std::string& s, const char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
        result.push_back(item);
    return result;
}

sensor_data_t get_sensor_data(const fs::path& file) {
    std::ifstream ifile(file.generic_string());
    if (!ifile.good())
        throw std::runtime_error("unable to open file:" +
                                 file.generic_string());

    std::string first_line;
    std::getline(ifile, first_line);
    auto values = split(first_line, ' ');

    if (values.size() != 30)
        throw std::runtime_error("invalid oxt file:" + file.generic_string());

    auto lati = std::stod(values[0]);
    auto longi = std::stod(values[1]);
    auto alti = std::stod(values[2]);
    auto roll = std::stod(values[3]);
    auto pitch = std::stod(values[4]);
    auto yaw = std::stod(values[5]);

    auto geo = geo_t(lati, longi, alti);
    auto rot = rotation_t(roll, pitch, yaw);
    return sensor_data_t(geo, rot);
}

void log_oxts_in_local(const std::vector<point3d_t>& points) {
    std::ofstream ofile("oxts-in-local.txt");

    for (auto point : points) {
        ofile << std::setprecision(15) << std::get<0>(point) << ' '
              << std::get<1>(point) << ' ' << std::get<2>(point) << '\n';
    }
}

void run_parser(const fs::path& oxts_folder) {
    check_existence({oxts_folder});
    auto oxts_files = get_files(oxts_folder, ".txt");

    rotation_t initial_rot;
    geo_t origin_geo;

    PoseWgs84 origin;

    std::vector<point3d_t> points_local;
    for (std::size_t i = 0; i < oxts_files.size(); ++i) {
        auto data = get_sensor_data(oxts_files[i]);
        auto geo = std::get<0>(data);
        auto rot = std::get<1>(data);

        if (i == 0) {
            std::cout << "Origin oxts file = " << oxts_files[i] << std::endl;
            std::cout << std::setprecision(15)
                      << "Origin GEO(lat, lon, alt) = " << std::get<0>(geo)
                      << ", " << std::get<1>(geo) << ", " << std::get<2>(geo)
                      << std::endl;

            std::cout
                << std::setprecision(15)
                << "Origin Orientation(yaw, pitch, roll) = " << std::get<2>(rot)
                << ", " << std::get<1>(rot) << ", " << std::get<0>(rot)
                << std::endl;

            origin = {
                deg2rad(std::get<0>(geo)), // latitude
                deg2rad(std::get<1>(geo)), // longitude
                std::get<2>(geo),          // altitude
                std::get<2>(rot),          // yaw
                std::get<1>(rot),          // pitch
                std::get<0>(rot)           // roll
            };
        }

        CoordWgs84 coord(deg2rad(std::get<0>(geo)), // latitude
                         deg2rad(std::get<1>(geo)), // longitude
                         std::get<2>(geo)           // altitude)
        );

        CoordLocal coord_local = coord.toCoordLocal(origin);

        points_local.push_back(
            std::make_tuple(coord_local.x, coord_local.y, coord_local.z));
    }

    log_oxts_in_local(points_local);
}

void run(int argc, char** argv) {
    if (argc != 2)
        throw std::invalid_argument("usage: app-oxts2local oxts-folder");

    fs::path oxts_folder = argv[1];
    run_parser(oxts_folder);
}

int main(int argc, char** argv) {
    try {
        run(argc, argv);
    } catch (std::exception& e) {
        std::cout << "caught exception:" << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "caught unknown exception" << std::endl;
        return 1;
    }

    return 0;
}
