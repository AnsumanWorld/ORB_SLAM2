#include "System.h"
#include "config.h"
#include "ext/app_monitor_api_impl.h"

#include <boost/lexical_cast.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>

#include <chrono>
#include <iostream>
#include <string>

void dump_map(const ORB_SLAM2::ext::map_export_t& map_) {
    LOG_MSG << "Saving map..." << std::endl;
    const char delim{' '};
    const char nl{'\n'};
    const int precision{15};

    auto kfmap = std::get<ORB_SLAM2::ext::keyframe_map_t>(map_);
    auto lnmap = std::get<ORB_SLAM2::ext::landmark_map_t>(map_);
    auto obs = std::get<ORB_SLAM2::ext::observations_t>(map_);
    auto origin = std::get<ORB_SLAM2::ext::pos_info_opt_t>(map_);

    std::ofstream cam_file("CameraPose.log");

    for (auto item : kfmap) {
        auto cam_pose = std::get<ORB_SLAM2::ext::camera_pose_t>(item.second);
        auto trans = std::get<Eigen::Vector3f>(cam_pose);
        auto rot = std::get<Eigen::Quaternionf>(cam_pose);
        auto cov = std::get<ORB_SLAM2::ext::cov66_t>(item.second);

        cam_file << std::fixed << std::setprecision(precision) << item.first
                 << delim << trans.x() << delim << trans.y() << delim
                 << trans.z() << delim << rot.w() << delim << rot.x() << delim
                 << rot.y() << delim << rot.z();

        for (decltype(cov.rows()) i = 0; i < cov.rows(); ++i)
            for (decltype(cov.cols()) j = 0; j < cov.cols(); ++j)
                cam_file << delim << cov(i, j) << delim;
        cam_file << nl;
    }

    std::ofstream mp_file("MapPoints.log");

    for (auto item : lnmap) {
        auto class_id = std::get<ORB_SLAM2::ext::class_id_t>(item.second);
        auto trans = std::get<ORB_SLAM2::ext::landmark_pos_t>(item.second);
        auto cov = std::get<ORB_SLAM2::ext::cov33_t>(item.second);
        mp_file << std::fixed << std::setprecision(precision) << item.first
                << delim << class_id << delim << trans.x() << delim << trans.y()
                << delim << trans.z();

        for (decltype(cov.rows()) i = 0; i < cov.rows(); ++i)
            for (decltype(cov.cols()) j = 0; j < cov.cols(); ++j)
                mp_file << delim << cov(i, j) << delim;
        mp_file << nl;
    }

    std::ofstream obs_file("Observations.log");

    for (auto item : obs) {
        obs_file << std::fixed << std::setprecision(precision) << item.first
                 << delim;

        for (auto fid : item.second)
            obs_file << fid << delim;

        obs_file << nl;
    }

    if (origin) {
        std::ofstream origin_file("Origin.log");
        origin_file << std::fixed << std::setprecision(precision)
                    << origin.get().pos.x() << delim << origin.get().pos.y()
                    << delim << origin.get().pos.z() << nl;
    }

    LOG_MSG << "Map Saved" << std::endl;
}

void wait_for_key() {
    char x{'0'};
    LOG_MSG << "Press any char and enter to continue!" << std::endl;
    std::cin >> x;
    LOG_MSG << "Received key input " << x << " continuing..." << std::endl;
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

bool is_natural_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) {
                             return !std::isdigit(c);
                         }) == s.end();
}

std::vector<fs::path>
get_files(const fs::path& folder, const std::vector<std::string>& ext) {
    std::vector<fs::path> files;

    if (!fs::exists(folder) || !fs::is_directory(folder))
        throw std::runtime_error("invalid folder:" + folder.generic_string());

    auto range = boost::make_iterator_range(fs::directory_iterator(folder), {});
    for (auto& entry : range) {
        if (fs::is_regular_file(entry) &&
            std::find(std::begin(ext), std::end(ext),
                      entry.path().extension()) != std::end(ext))
            files.push_back(entry.path());
    }

    std::sort(files.begin(), files.end());
    return files;
}

void track(const config& cfg, ORB_SLAM2::System& slam, const cv::Mat& image) {
    // static std::size_t fcount{0};
    // LOG_MSG << "Tracking frame:" << ++fcount << '\n';

    auto input = std::make_tuple(0.0, image, boost::none, boost::none);

    slam.TrackMonocular(input);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(cfg.frame_interval_ms()));
}

void feed_from_images(const config& cfg, ORB_SLAM2::System& slam) {
    auto image_files =
        get_files(cfg.image_source(), {".bmp", ".png", ".jpg", ".jpeg"});

    for (auto image_file : image_files) {
        auto image =
            cv::imread(image_file.generic_string(), CV_LOAD_IMAGE_UNCHANGED);

        if (image.empty())
            throw std::runtime_error("unable to load the image:" +
                                     image_file.generic_string());

        track(cfg, slam, image);
    }
}

void feed_from_video(const config& cfg, ORB_SLAM2::System& slam,
                     cv::VideoCapture&& video) {
    if (!video.isOpened())
        throw std::runtime_error("unable to open video:" +
                                 cfg.image_source().generic_string());

    while (true) {
        cv::Mat image;
        if (!video.read(image) || image.empty())
            break;

        track(cfg, slam, image);
    }
}

void run_slam(const config& cfg) {
    check_existence({cfg.vocabulary_path(), cfg.settings_path()});
    check_file({cfg.vocabulary_path(), cfg.settings_path()});

    ORB_SLAM2::ext::app_monitor_impl app_monitor_inst;
    ORB_SLAM2::ext::app_monitor_api* app_monitor = &app_monitor_inst;

    ORB_SLAM2::System slam(cfg.vocabulary_path().generic_string(),
                           cfg.settings_path().generic_string(),
                           ORB_SLAM2::System::MONOCULAR, cfg.show_viewer(),
                           app_monitor);

    if (fs::is_regular_file(cfg.image_source()))
        feed_from_video(cfg, slam,
                        cv::VideoCapture(cfg.image_source().generic_string()));
    else if (fs::is_directory(cfg.image_source()))
        feed_from_images(cfg, slam);
    else if (is_natural_number(cfg.image_source().generic_string()))
        feed_from_video(cfg, slam,
                        cv::VideoCapture(boost::lexical_cast<int>(
                            cfg.image_source().generic_string())));
    else
        throw std::runtime_error("invalid image source:" +
                                 cfg.image_source().generic_string());

    slam.Shutdown();
    // slam.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectoryTUM.log");
    // slam.SaveTrajectoryKITTI("FrameTrajectoryKITTI.log");
    
    std::chrono::steady_clock::time_point begin =
        std::chrono::steady_clock::now();
    auto map_ = slam.GetMapData();
    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();

    LOG_MSG
        << "Time for GetMapData (ms) = "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
               .count()
        << std::endl;
    dump_map(map_);
}

void run(int argc, char** argv) {
    config cfg(argc, argv);

    if (cfg.wait_for_stdin())
        wait_for_key();

    run_slam(cfg);
}

int main(int argc, char** argv) {
    LOG_MSG << "Starting " << argv[0] << std::endl;

    auto ret_val{EXIT_SUCCESS};
    try {
        run(argc, argv);
    } catch (std::exception& e) {
        LOG_MSG << "caught exception:" << e.what() << std::endl;
        ret_val = EXIT_FAILURE;
    } catch (...) {
        LOG_MSG << "caught unknown exception" << std::endl;
        ret_val = EXIT_FAILURE;
    }

    LOG_MSG << "Exiting " << argv[0] << std::endl;
    return ret_val;
}
