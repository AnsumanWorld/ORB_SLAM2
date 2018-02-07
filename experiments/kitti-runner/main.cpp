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

#include "System.h"
#include "ext/app_monitor_api_impl.h"
#include "ext/messages.h"

namespace fs = boost::filesystem;

using sensor_data_t = std::tuple<Eigen::Vector3d, Eigen::Matrix3d, ORB_SLAM2::ext::orientation_t>;
/*
0: Eigen::Vector3d(latitude, longitude, altitude)
1: Eigen::Matrix3d: covariance
3: ext::orientation_t
*/

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

std::vector<sensor_data_t> get_gt_values(const fs::path& file)
{
    std::ifstream ifile(file.generic_string());
    if (!ifile.good())
        throw std::runtime_error("unable to open file:" +
                                 file.generic_string());

    std::vector<sensor_data_t> gt_values;
    std::string each_line;
    while(std::getline(ifile, each_line))
    {
        auto values = split(each_line, ' ');

        if (values.size() != 12)
            throw std::runtime_error("invalid gt file:" + file.generic_string());

        auto x = std::stod(values[3]);
        auto y = std::stod(values[7]);
        auto z = std::stod(values[11]);

        auto cov = 1.0 / 100.0;
        Eigen::Matrix3d inf = Eigen::Matrix3d::Identity() * cov;
        ORB_SLAM2::ext::orientation_t orient = std::make_tuple(0.0, 0.0, 0.0);
        auto pos = Eigen::Vector3d(x, y, z);
        gt_values.push_back(std::make_tuple(pos, inf, orient));
    }

    return gt_values;
}

sensor_data_t get_sensor_data_from_oxts(const fs::path& file)
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

    auto accuracy = std::stod(values[23]);
    auto cov = 1.0 / 3000.0;
    Eigen::Matrix3d inf = Eigen::Matrix3d::Identity() * cov;
    ORB_SLAM2::ext::orientation_t orient = std::make_tuple(roll, pitch, yaw);
    auto pos = Eigen::Vector3d(lati, longi, alti);
    return std::make_tuple(pos, inf, orient);
}

void check_existence(const std::initializer_list<fs::path>& paths)
{
    std::for_each(paths.begin(), paths.end(), [&](const fs::path& path) {
        if (!fs::exists(path))
            throw std::runtime_error("invalid path:" + path.generic_string());
    });
}

void check_file(const std::initializer_list<fs::path>& paths)
{
    std::for_each(paths.begin(), paths.end(), [&](const fs::path& path) {
        if (!fs::is_regular_file(path))
            throw std::runtime_error("not a regular file:" + path.generic_string());
    });
}

void check_directory(const std::initializer_list<fs::path>& paths)
{
    std::for_each(paths.begin(), paths.end(), [&](const fs::path& path) {
        if (!fs::is_directory(path))
            throw std::runtime_error("not a directory:" + path.generic_string());
    });
}

void track(ORB_SLAM2::System& slam, const ORB_SLAM2::ext::slam_input_t& input)
{
    // Simulate ~10 fps for KITTI using sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    slam.TrackMonocular(input);
}

void feed_from_gt(ORB_SLAM2::System& slam, const fs::path& image_folder, 
    const fs::path& gt)
{
    auto image_files = get_files(image_folder, ".png");
    auto gt_values = get_gt_values(gt);

    if (image_files.size() != gt_values.size())
        throw std::runtime_error("image_files count != gt_values count");

    for (std::size_t i = 0; i < image_files.size(); ++i)
    {
        auto image = cv::imread(image_files[i].generic_string(),
                        CV_LOAD_IMAGE_UNCHANGED);

        if (image.empty())
            throw std::runtime_error("unable to load the image:" +
                                     image_files[i].generic_string());

        auto sensor_values = gt_values.at(i);
        ORB_SLAM2::ext::pos_info posi;
        posi.pos = std::get<0>(sensor_values);
        posi.covariance = std::get<1>(sensor_values);
        posi.orient = std::get<2>(sensor_values);

        auto ts = boost::none;
        ORB_SLAM2::ext::time_point_t timestamp = i;
        auto input = std::make_tuple(timestamp, image, ts, posi);
        track(slam, input);
    }
}

void feed_from_oxts(ORB_SLAM2::System& slam, const fs::path& image_folder,
           const fs::path& oxts_folder)
{
    auto image_files = get_files(image_folder, ".png");
    auto oxts_files = get_files(oxts_folder, ".txt");

    if (image_files.size() != oxts_files.size())
        throw std::runtime_error("image_files count != oxts_files count");

    for (std::size_t i = 0; i < image_files.size(); ++i)
    {
        if(image_files[i].stem() != oxts_files[i].stem())
            throw std::runtime_error("image and oxt filenames are not matching");

        auto image = cv::imread(image_files[i].generic_string(),
                                CV_LOAD_IMAGE_UNCHANGED);

        if (image.empty())
            throw std::runtime_error("unable to load the image:" +
                                     image_files[i].generic_string());

        auto sensor_values = get_sensor_data_from_oxts(oxts_files[i]);
        ORB_SLAM2::ext::pos_info posi;
        posi.pos = std::get<0>(sensor_values);
        posi.covariance = std::get<1>(sensor_values);
        posi.orient = std::get<2>(sensor_values);

        auto ts = boost::none;
        ORB_SLAM2::ext::time_point_t timestamp = i;
        auto input = std::make_tuple(timestamp, image, ts, posi);
        track(slam, input);
    }
}

void run_slam(const std::string& mode, const fs::path& voc, const fs::path& settings,
              const fs::path& image_folder, const fs::path& sensor_source)
{
    check_existence({voc, settings, image_folder, sensor_source});
    check_file({voc, settings});
    check_directory({image_folder});
    ORB_SLAM2::ext::app_monitor_impl app_monitor_inst;
    ORB_SLAM2::ext::app_monitor_api* app_monitor = &app_monitor_inst;

    ORB_SLAM2::System slam(voc.generic_string(), settings.generic_string(),
                           ORB_SLAM2::System::MONOCULAR, true, app_monitor);

    if(mode == "gt")
    {
        check_file({sensor_source});
        feed_from_gt(slam, image_folder, sensor_source);
    }
    else if(mode == "oxts")
    {
        check_directory({sensor_source});
        feed_from_oxts(slam, image_folder, sensor_source);
    }
    else
    {
        throw std::invalid_argument("invalid mode:" + mode);
    }

    slam.Shutdown();
    slam.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectoryTUM.txt");
    slam.SaveTrajectoryKITTI("KeyFrameTrajectoryKITTI.txt");
}

void wait_for_key()
{
    char x{'0'};
    std::cout << "Press any char and enter to continue!" << std::endl;
    std::cin >> x;
}

void run(int argc, char** argv)
{
    // wait_for_key(); // Use Attach to process and press Key
    
    // std::ofstream out("kitti-runner.log");
    // std::cout.rdbuf(out.rdbuf());
    // std::ios_base::sync_with_stdio(false);
    // std::cin.tie(NULL);

    if (argc != 6)
        throw std::invalid_argument(
            "usage: kitti_runner mode(gt|oxts) vocabulary settings image_folder "
            "sensor_source(groundtruth|oxts_folder)");

    std::string mode{argv[1]};
    fs::path voc = argv[2];
    fs::path settings = argv[3];
    fs::path image_folder = argv[4];
    fs::path sensor_source = argv[5];

    run_slam(mode, voc, settings, image_folder, sensor_source);
}

int main(int argc, char** argv)
{
    try
    {
        run(argc, argv);
    }
    catch (std::exception& e)
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
