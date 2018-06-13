#include "System.h"
#include "ext/app_monitor_api_impl.h"
#include "ext/messages.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
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

namespace fs = boost::filesystem;

using sensor_data_t = std::tuple<Eigen::Vector3d, Eigen::Matrix3d>;
/*
0: Eigen::Vector3d(latitude, longitude, altitude)
1: Eigen::Matrix3d: covariance
*/

using slam_input_t =
    std::tuple<ORB_SLAM2::ext::time_point_t, ORB_SLAM2::ext::image_t,
               ORB_SLAM2::ext::tsr_info_opt_t, ORB_SLAM2::ext::pos_info_opt_t>;

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

std::vector<sensor_data_t> get_sensor_values(const fs::path& file) {
    std::ifstream ifile(file.generic_string());
    if (!ifile.good())
        throw std::runtime_error("unable to open file:" +
                                 file.generic_string());

    std::vector<sensor_data_t> gt_values;
    std::string each_line;
    while (std::getline(ifile, each_line)) {
        auto values = split(each_line, ' ');

        if (values.size() != 12)
            throw std::runtime_error("invalid gt file:" +
                                     file.generic_string());

        auto x = std::stod(values[3]);
        auto y = std::stod(values[7]);
        auto z = std::stod(values[11]);

        auto cov = 1.0 / 100.0;
        Eigen::Matrix3d inf = Eigen::Matrix3d::Identity() * cov;
        auto pos = Eigen::Vector3d(x, y, z);
        gt_values.push_back(std::make_tuple(pos, inf));
    }

    return gt_values;
}

void check_existence(const std::initializer_list<fs::path>& paths) {
    std::for_each(paths.begin(), paths.end(), [&](const fs::path& path) {
        if (!fs::exists(path))
            throw std::runtime_error("invalid path:" + path.generic_string());
    });
}

void check_file(const std::initializer_list<fs::path>& paths) {
    std::for_each(paths.begin(), paths.end(), [&](const fs::path& path) {
        if (!fs::is_regular_file(path))
            throw std::runtime_error("not a regular file:" +
                                     path.generic_string());
    });
}

void check_directory(const std::initializer_list<fs::path>& paths) {
    std::for_each(paths.begin(), paths.end(), [&](const fs::path& path) {
        if (!fs::is_directory(path))
            throw std::runtime_error("not a directory:" +
                                     path.generic_string());
    });
}

void track(ORB_SLAM2::System& slam, const slam_input_t& input) {
    // Simulate ~10 fps for KITTI using sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    slam.TrackMonocular(input);
}

void feed_images(ORB_SLAM2::System& slam, const fs::path& image_folder) {
    auto image_files = get_files(image_folder, ".png");

    for (auto image_file : image_files) {
        auto image =
            cv::imread(image_file.generic_string(), CV_LOAD_IMAGE_UNCHANGED);

        if (image.empty())
            throw std::runtime_error("unable to load the image:" +
                                     image_file.generic_string());

        auto input = std::make_tuple(0.0, image, boost::none, boost::none);
        track(slam, input);
    }
}

void feed_from_gt(ORB_SLAM2::System& slam, const fs::path& image_folder,
                  const fs::path& gt, const std::size_t& init_frames_with_gps,
                  const std::size_t& skip_frequency) {
    auto image_files = get_files(image_folder, ".png");
    auto gt_values = get_sensor_values(gt);

    if (image_files.size() != gt_values.size())
        throw std::runtime_error("image_files count != gt_values count");

    std::size_t skipped_frames = 0;
    for (std::size_t i = 0; i < image_files.size(); ++i) {
        auto image = cv::imread(image_files[i].generic_string(),
                                CV_LOAD_IMAGE_UNCHANGED);

        if (image.empty())
            throw std::runtime_error("unable to load the image:" +
                                     image_files[i].generic_string());

        auto sensor_values = gt_values.at(i);
        ORB_SLAM2::ext::pos_info posi;
        posi.pos = std::get<0>(sensor_values);
        posi.covariance = std::get<1>(sensor_values);

        slam_input_t input;

        if (i > init_frames_with_gps && skipped_frames < skip_frequency) {
            input = std::make_tuple(i, image, boost::none, boost::none);
            ++skipped_frames;
        } else {
            input = std::make_tuple(i, image, boost::none, posi);
            skipped_frames = 0;
        }

        track(slam, input);
    }
}

void run_slam(const std::string& option, const fs::path& voc,
              const fs::path& settings, const fs::path& image_folder,
              const fs::path& sensor_source, const fs::path& output_traj,
              const std::size_t& init_frames_with_gps,
              const std::size_t& skip_frequency) {
    check_existence({voc, settings, image_folder, sensor_source});
    check_file({voc, settings, sensor_source});
    check_directory({image_folder});
    ORB_SLAM2::ext::app_monitor_impl app_monitor_inst;
    ORB_SLAM2::ext::app_monitor_api* app_monitor = &app_monitor_inst;

    ORB_SLAM2::System slam(voc.generic_string(), settings.generic_string(),
                           ORB_SLAM2::System::MONOCULAR, true, app_monitor);

    check_file({sensor_source});

    if ("gps" == option)
        feed_from_gt(slam, image_folder, sensor_source, init_frames_with_gps,
                     skip_frequency);
    else if ("no-gps" == option)
        feed_images(slam, image_folder);
    else
        throw std::runtime_error("invalid option:" + option);

    slam.Shutdown();
    slam.SaveTrajectoryKITTI(output_traj.generic_string().c_str());
}

void wait_for_key() {
    char x{'0'};
    std::cout << "Press any char and enter to continue!" << std::endl;
    std::cin >> x;
}

void run(int argc, char** argv) {
    // wait_for_key(); // Use Attach to process and press Key

    // std::ofstream out("kitti-runner.log");
    // std::cout.rdbuf(out.rdbuf());
    // std::ios_base::sync_with_stdio(false);
    // std::cin.tie(NULL);

    if (argc != 9)
        throw std::invalid_argument(
            "usage: kitti_runner option(gps|no-gps) vocabulary settings "
            "image_folder sensor_source(groundtruth) output_traj min_init_gps "
            "skip_frequency");

    std::string option = argv[1];
    fs::path voc = argv[2];
    fs::path settings = argv[3];
    fs::path image_folder = argv[4];
    fs::path sensor_source = argv[5];
    fs::path output_traj = argv[6];
    std::size_t init_frames_with_gps =
        boost::lexical_cast<std::size_t>(argv[7]);
    std::size_t skip_frequency = boost::lexical_cast<std::size_t>(argv[8]);

    run_slam(option, voc, settings, image_folder, sensor_source, output_traj,
             init_frames_with_gps, skip_frequency);
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
