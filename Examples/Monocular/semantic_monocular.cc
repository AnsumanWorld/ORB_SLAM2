
#include "System.h"
#include "ext/app_monitor_api_impl.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
#include<thread>
#include<opencv2/core/core.hpp>
#include <vector>
#include <map>

#include <boost/filesystem.hpp>
#include <vector>
#include <cinttypes>
#include <stdexcept>
#include "semantic_monocular.hpp"


struct input_args {
    std::string path_to_vocabulary;
    std::string path_to_camera_settings;
    std::string path_to_image_folder;
    std::string path_to_json_file;
};

input_args parse_input_arguments(int argc, char** argv)
{
    if (argc < 4) {
        throw std::runtime_error(
            "Usage: ./semantic_monocular path_to_vocabulary path_to_camera_settings path_to_image_folder path_to_jsonfile");
    }

    return input_args{
        argv[1],
        argv[2],
        argv[3],
        argv[4]
    };
}

class slam_object {
    ORB_SLAM2::System _slam;

    void shutdown()
    {
        // Stop all threads
        _slam.Shutdown();

        // Save camera trajectory
        _slam.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");
    }
public:
    ORB_SLAM2::System& get()
    {
        return _slam;
    }

    ORB_SLAM2::System const& get() const
    {
        return _slam;
    }

    slam_object(input_args const& args_, ORB_SLAM2::ext::app_monitor_api* monitor_)
        : _slam{
            args_.path_to_vocabulary,
            args_.path_to_camera_settings,
            ORB_SLAM2::System::MONOCULAR,
            monitor_,
            true}
    {}

    ~slam_object()
    {
        shutdown();
    }
};


int run_slam_loop(int argc, char** argv)
{
    try {
        auto args = parse_input_arguments(argc, argv);
        std::vector<fs::path> image_files;
        std::copy(fs::directory_iterator(args.path_to_image_folder), fs::directory_iterator(),
                  std::back_inserter(image_files));
        std::sort(image_files.begin(), image_files.end());

        // Create SLAM system. It initializes all system threads and gets ready to process frames.

        ORB_SLAM2::ext::app_monitor_impl app_monitor_inst;
        ORB_SLAM2::ext::app_monitor_api* app_monitor = &app_monitor_inst;

        slam_object slam{args, app_monitor};
        auto slam_data_source =
            data_source< ORB_SLAM2::ext::traffic_sign_map_t, std::string>::make_data_source(data_source_type::semantic, args.path_to_json_file);
        std::uint64_t time = 0;
        auto it = slam_data_source->get_data_source_iter();
        auto traffic_signs_map = it->next();

        for (auto const& file : image_files) {
            app_monitor->request_wait();

            auto image = cv::imread(file.generic_string(), CV_LOAD_IMAGE_UNCHANGED);
            double timestamp = static_cast<double>(time);
            if (image.empty()) {
                throw std::runtime_error("Failed to load image!");
            }
            if ((false == traffic_signs_map.empty()) && traffic_signs_map.end() != traffic_signs_map.find(time)) {
                slam.get().TrackMonocular(std::make_tuple(timestamp,image,traffic_signs_map[time],ORB_SLAM2::ext::pos_info_opt_t() ));
                traffic_signs_map = it->next();
            } else
                slam.get().TrackMonocular(image, timestamp);
            time++;
        }
    } catch (std::exception const& ex_) {

        std::cerr << "Exception: " << ex_.what() << std::endl;

        return 1;
    }

    return 0;
}

int main(int argc, char** argv)
{
    return run_slam_loop(argc, argv);
}
