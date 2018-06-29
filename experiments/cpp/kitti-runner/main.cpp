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
#include <iostream>
#include <string>
#include <vector>

#include "config.h"

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

void check_existence(const std::vector<fs::path>& paths) {
    std::for_each(paths.begin(), paths.end(), [&](const fs::path& path) {
        if (!fs::exists(path))
            throw std::runtime_error("invalid path:" + path.generic_string());
    });
}

void check_file(const std::vector<fs::path>& paths) {
    std::for_each(paths.begin(), paths.end(), [&](const fs::path& path) {
        if (!fs::is_regular_file(path))
            throw std::runtime_error("not a regular file:" +
                                     path.generic_string());
    });
}

void check_directory(const std::vector<fs::path>& paths) {
    std::for_each(paths.begin(), paths.end(), [&](const fs::path& path) {
        if (!fs::is_directory(path))
            throw std::runtime_error("not a directory:" +
                                     path.generic_string());
    });
}

void track(ORB_SLAM2::System& slam, slam_input_t const& input,
           config const& cfg) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(cfg.frame_interval_ms()));
    slam.TrackMonocular(input);
}

void feed_images(ORB_SLAM2::System& slam, config const& cfg) {
    auto image_files = get_files(cfg.images_dir(), ".png");

    for (auto image_file : image_files) {
        auto image =
            cv::imread(image_file.generic_string(), CV_LOAD_IMAGE_UNCHANGED);

        if (image.empty())
            throw std::runtime_error("unable to load the image:" +
                                     image_file.generic_string());

        auto input = std::make_tuple(0.0, image, boost::none, boost::none);
        track(slam, input, cfg);
    }
}

void feed_from_gt(ORB_SLAM2::System& slam, config const& cfg) {
    auto image_files = get_files(cfg.images_dir(), ".png");
    auto gt_values = get_sensor_values(cfg.ext_pose_data());

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

        if (i > cfg.min_init_frames_with_ext_pose() &&
            skipped_frames < cfg.ext_pose_skip_freq()) {
            input = std::make_tuple(i, image, boost::none, boost::none);
            ++skipped_frames;
        } else {
            input = std::make_tuple(i, image, boost::none, posi);
            skipped_frames = 0;
        }

        track(slam, input, cfg);
    }
}

void run_slam(config const& cfg) {
    check_existence({cfg.vocabulary_path(), cfg.settings_path(),
                     cfg.images_dir(), cfg.ext_pose_data()});
    check_file(
        {cfg.vocabulary_path(), cfg.settings_path(), cfg.ext_pose_data()});
    check_directory({cfg.images_dir()});
    ORB_SLAM2::ext::app_monitor_impl app_monitor_inst;
    ORB_SLAM2::ext::app_monitor_api* app_monitor = &app_monitor_inst;

    ORB_SLAM2::System slam(cfg.vocabulary_path().generic_string(),
                           cfg.settings_path().generic_string(),
                           ORB_SLAM2::System::MONOCULAR, true, app_monitor);

    if (cfg.use_ext_pose()) {
        check_file({cfg.ext_pose_data()});
        feed_from_gt(slam, cfg);
    } else
        feed_images(slam, cfg);

    slam.Shutdown();
    slam.SaveTrajectoryKITTI(cfg.output_traj_path().generic_string().c_str());
}

void wait_for_key() {
    char x{'0'};
    std::cout << "Press any char and enter to continue!" << std::endl;
    std::cin >> x;
    std::cout << "Recieved key input " << x << " continuing..." << std::endl;
}

void run(int argc, char** argv) {
    config cfg(argc, argv);

    if (cfg.wait_for_stdin())
        wait_for_key();

    run_slam(cfg);
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
