#include "System.h"
#include "ext/app_monitor_api_impl.h"
#include "ext/messages.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/combine.hpp>

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

using slam_input_t = std::pair<cv::Mat, cv::Mat>;

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
    slam.TrackRGBD(input.first, input.second, 0.0);
}

void feed_images(ORB_SLAM2::System& slam, config const& cfg) {
    auto image_files = get_files(cfg.images_dir(), ".png");
    auto depth_files = get_files(cfg.depth_dir(), ".png");

    for(auto item : boost::combine(image_files, depth_files)) {
        fs::path image_path;
        fs::path depth_path;
        boost::tie(image_path, depth_path) = item;

        auto image =
            cv::imread(image_path.generic_string(), CV_LOAD_IMAGE_UNCHANGED);
        if (image.empty())
            throw std::runtime_error("unable to load the image:" +
                                     image_path.generic_string());

        auto depth =  cv::imread(depth_path.generic_string(), CV_LOAD_IMAGE_UNCHANGED);

        if (depth.empty())
            throw std::runtime_error("unable to load the image:" +
                                     depth_path.generic_string());

        auto input = std::make_pair(image, depth);
            track(slam, input, cfg);
    }
}

void run_slam(config const& cfg) {
    check_existence({cfg.vocabulary_path(), cfg.settings_path(),
                     cfg.images_dir()});
    check_file(
        {cfg.vocabulary_path(), cfg.settings_path()});
    check_directory({cfg.images_dir()});
    check_directory({cfg.depth_dir()});

    ORB_SLAM2::ext::app_monitor_impl app_monitor_inst;
    ORB_SLAM2::ext::app_monitor_api* app_monitor = &app_monitor_inst;

    ORB_SLAM2::System slam(cfg.vocabulary_path().generic_string(),
                           cfg.settings_path().generic_string(),
                           ORB_SLAM2::System::RGBD, true, app_monitor);
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
        return EXIT_FAILURE;
    } catch (...) {
        std::cout << "caught unknown exception" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
