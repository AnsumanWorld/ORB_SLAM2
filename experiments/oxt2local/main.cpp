#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include <Eigen/Core>
#include <Eigen/StdVector>

#include <chrono>
#include <exception>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#include "ext/GeodeticCoords.h"
#include "util/util.h"

namespace fs = boost::filesystem;

using geo_t = std::tuple<double, double, double>;
using rotation_t = std::tuple<double, double, double>;
using point3d_t = std::tuple<double, double, double>;
using sensor_data_t = std::tuple<geo_t, rotation_t>;

std::vector<fs::path> get_files(const fs::path& folder, const std::string& ext)
{
    std::vector<fs::path> files;

    if (!fs::exists(folder) || !fs::is_directory(folder))
        throw std::runtime_error("invalid folder:" + folder.generic_string());

    auto range = boost::make_iterator_range(fs::directory_iterator(folder), {});
    for(auto& entry : range)
    {
        if (fs::is_regular_file(entry) && entry.path().extension() == ext)
            files.push_back(entry.path());
    }

    std::sort(files.begin(), files.end());
    return files;
}

std::vector<std::string> split(const std::string& s, const char delim)
{
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
        result.push_back(item);
    return result;
}

sensor_data_t get_sensor_data(const fs::path& file)
{
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

void check_existence(const std::initializer_list<fs::path>& paths)
{
    std::for_each(paths.begin(), paths.end(), [&](const fs::path& path) {
        if (!fs::exists(path))
            throw std::runtime_error("invalid path:" + path.generic_string());
    });
}

void log_oxts_in_local(const std::vector<point3d_t>& points)
{
    std::ofstream ofile("oxts-in-local.txt");
    
    for(auto point : points)
    {
        ofile << std::setprecision(15) 
        << std::get<0>(point) << ' '
        << std::get<1>(point) << ' '
        << std::get<2>(point) << '\n';
    }
}

point3d_t rotate(const cv::Point3d& point, const rotation_t& rot)
{
    auto rotated = point;

    auto roll = util::rad_to_deg(std::get<0>(rot));
    auto pitch = util::rad_to_deg(std::get<1>(rot));
    auto yaw = util::rad_to_deg(std::get<2>(rot));

    yaw *= -1;
    // Rotate along x axis
    rotated.x = point.x;
    rotated.y = point.y * cos(roll) - point.z * sin(roll);
    rotated.z = point.y * sin(roll) + point.z * cos(roll);

    // Rotate along y axis
    rotated.x = point.x * cos(pitch) + point.z * sin(pitch);
    rotated.y = point.y;
    rotated.z = point.z * cos(pitch) - point.x * sin(pitch);

    // Rotate along z axis
    rotated.x = point.x * cos(yaw) - point.y * sin(yaw);
    rotated.y = point.x * sin(yaw) + point.y * cos(yaw);
    rotated.z = point.z;

    return point3d_t(rotated.x, rotated.y, rotated.z);
}

point3d_t axis_transform(const point3d_t& point)
{
    point3d_t ret;
    std::get<0>(ret) = std::get<1>(point) * -1; // cam_x = -sens_y;
    std::get<1>(ret) = std::get<2>(point) * -1; // cam_y = -sens_z;
    std::get<2>(ret) = std::get<0>(point); // cam_z = sens_x
    return ret;
}

void run_parser(const fs::path& oxts_folder)
{
    check_existence({oxts_folder});
    auto oxts_files = get_files(oxts_folder, ".txt");

    rotation_t initial_rot;
    geo_t origin_geo;

    using namespace ORB_SLAM2::ext;
    GeodeticCoords origin;
    
    std::vector<point3d_t> points_local;
    for (std::size_t i = 0; i < oxts_files.size(); ++i)
    {
        auto data = get_sensor_data(oxts_files[i]);
        auto geo = std::get<0>(data);
        auto rot = std::get<1>(data);

        auto lati = std::get<0>(geo);
        auto longi = std::get<1>(geo);
        auto alti = std::get<2>(geo);

        if(i == 0)
        {
            origin_geo = geo;
            initial_rot = rot;

            origin.setLatitude(std::get<0>(origin_geo));
            origin.setLongitude(std::get<1>(origin_geo));
            origin.setAltitude(std::get<2>(origin_geo));

            GeodeticCoords coord(lati, longi, alti);
            auto local = coord.toENU_WGS84(origin);
            auto rotated_point = rotate(local, initial_rot);
            auto transformed = axis_transform(rotated_point);
            points_local.push_back(transformed);
        }
        else
        {
            GeodeticCoords coord(lati, longi, alti);
            auto local = coord.toENU_WGS84(origin);
            auto rotated_point = rotate(local, initial_rot);
            auto transformed = axis_transform(rotated_point);
            points_local.push_back(transformed);
        }
    }

    log_oxts_in_local(points_local);
}

void run(int argc, char** argv)
{
    if(argc != 2)
        throw std::invalid_argument("usage: gps2local oxts-folder");
    
    fs::path oxts_folder = argv[1];
    run_parser(oxts_folder);
}

int main(int argc, char** argv)
{
    try
    {
        run(argc, argv);
    }
    catch(std::exception& e)
    {
        std::cout << "caught exception:" << e.what() << std::endl;
        return 1;
    }
    catch(...)
    {
        std::cout << "caught unknown exception" << std::endl;
        return 1;
    }

    return 0;
}
