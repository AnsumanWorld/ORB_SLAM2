/**
* This file is part of ORB-SLAM2.
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/

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

namespace fs = boost::filesystem;

using namespace std;
using boost::property_tree::ptree;

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

class semantic_info {
	bool semantic_info_status = false;
	boost::property_tree::ptree pt;
	ptree::iterator semantic_info_start;
	ptree::iterator semantic_info_end;
	const int img_width = 1280;
	const int img_height = 720;
	const int min_roi_width = 90;
	const int min_roi_height = 90;	
	
	bool read_semantic_info(std::string jsonFilename)
	{
		bool read_status = false;
		std::fstream jsonfile(jsonFilename);
		if (false == jsonfile.is_open()) {
			cout << "Unable to open json file" << endl;
			return read_status;
		}
		boost::property_tree::read_json(jsonfile, pt);
		jsonfile.close();
		
		semantic_info_start = pt.begin();
		semantic_info_end = pt.end();
		
		if (semantic_info_start != semantic_info_end)
			read_status = true;
		return read_status;
	}
public:	
    semantic_info(std::string jsonFilename)
    {
		semantic_info_status = read_semantic_info(jsonFilename);
    }
	
	void transform_rect(std::vector<double> &rect_arr, cv::Rect& roi, bool is_absolute = false)
	{
		if (true == is_absolute) {
			roi.x = rect_arr[0];
			roi.y = rect_arr[1];
			roi.width = rect_arr[2] - rect_arr[0];
			roi.height = rect_arr[3] - rect_arr[1];
		} else {
			int ymin = int(rect_arr[0] * img_height);
			int xmin = int(rect_arr[1] * img_width);
			int ymax = int(rect_arr[2] * img_height);
			int xmax = int(rect_arr[3] * img_width);
			roi.x = xmin;
			roi.y = ymin;
			roi.width = xmax - xmin;
			roi.height = ymax - ymin;
		}
	}
	
	bool get_next_semantic(std::uint64_t frameindex, std::vector<ORB_SLAM2::traffic_sign> &traffic_signs,bool search_from_first=false)
	{
		bool found_semantic = false;
		
		if(true == semantic_info_status)
		{
			if(true == search_from_first)
			{
				semantic_info_start = pt.begin();
				semantic_info_end = pt.end();
			}
			if (semantic_info_start != semantic_info_end)
			{
				std::string image_name = semantic_info_start->first;
				if (stoul(image_name) == frameindex)
				{
					found_semantic = true;
					auto &traffic_sign_arr = semantic_info_start->second;
					BOOST_FOREACH(boost::property_tree::ptree::value_type &node, traffic_sign_arr.get_child("traffic_signs"))
					{
						ORB_SLAM2::traffic_sign t;
						t.classid = node.second.get<int>("class_id");
						t.confidence = node.second.get<double>("confidence");
						std::vector<double> r;
						for (auto &temppt : node.second.get_child("rectangle")) {
							r.push_back(temppt.second.get_value < double >());
						}
						
						transform_rect(r, t.roi);
						traffic_signs.push_back(t);
					}
					semantic_info_start++;
					
				}
			}
		}
		return found_semantic;
	}
	
    ~semantic_info()
    {
		semantic_info_status=false;
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

        ORB_SLAM2::traffic_sign_map_t traffic_signs;
        // Create SLAM system. It initializes all system threads and gets ready to process frames.

        ORB_SLAM2::ext::app_monitor_impl app_monitor_inst;
        ORB_SLAM2::ext::app_monitor_api* app_monitor = &app_monitor_inst;

        slam_object slam{args, app_monitor};
		semantic_info semantic_info_obj{args.path_to_json_file};
        std::uint64_t time = 0;

        for (auto const& file : image_files) {
            app_monitor->request_wait();

            auto image = cv::imread(file.generic_string(), CV_LOAD_IMAGE_UNCHANGED);
			double timestamp = static_cast<double>(time);
            if (image.empty()) {
                throw std::runtime_error("Failed to load image!");
            }
			std::vector<ORB_SLAM2::traffic_sign> traffic_signs;
			
			if ( true == semantic_info_obj.get_next_semantic(time,traffic_signs) )
			{
				ORB_SLAM2::tsr_info tsr;
				tsr.interested_object.insert(std::make_pair(time, traffic_signs));
				ORB_SLAM2::sensor_info sensor_input;
				sensor_input.tsr = tsr;
				//ORB_SLAM2::time_point_t timestamp(time);// (std::chrono::milliseconds(time));
				slam.get().TrackMonocular(std::make_tuple(image, timestamp, sensor_input));
			}
			else
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
