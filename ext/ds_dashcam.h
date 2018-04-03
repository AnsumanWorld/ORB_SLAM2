#pragma once

#include "ext/messages.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include<iterator>
#include <boost/lexical_cast.hpp>
#include<opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

namespace fs = boost::filesystem;
using namespace std;
using boost::property_tree::ptree;

namespace ORB_SLAM2 {
	namespace ext {
		enum class dash_cam_csv_column { date_col = 1, time_col = 2, lat_col = 5,long_col = 6 };
		typedef boost::tokenizer<boost::escaped_list_separator<char>> tokenizer;
		struct ds_dashcam_args {
			std::string _path_to_video;
			std::string _path_to_csv_file;

			ds_dashcam_args(int argc, char** argv)
			{
				if (argc < 5) {
					throw std::runtime_error("Usage: ./run_dashcam_monocular <path_to_vocabulary> <path_to_camera_settings> <path_to_video> <path_to_csv_file>");
				}
				_path_to_video = argv[3];
				_path_to_csv_file = (argc >= 5) ? argv[4] : "";
			}
			ds_dashcam_args()
			{
			}
		};

		class ds_dashcam : public boost::iterator_facade<
							ds_dashcam,
							std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t>,
							boost::single_pass_traversal_tag >
		{
			cv::VideoCapture _video_capture;
			ifstream _gpsfile;
			std::vector<fs::path> _gps_files;
			std::uint64_t _gps_index{0};
			std::uint64_t _image_index{ 0 };
			boost::optional<gps_pair_t> _cur_gps_ds;
			bool _data_source_status{false};
			bool _gps_input_status{false};
			image_t _image;
			double _timestamp{ 0 };
			std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t> _item;
			ds_dashcam_args _dashcam_args;

		public:

			bool read_video(std::string video_file)
			{
				bool video_status = false;
				video_status = _video_capture.open(video_file);
				if (video_status)
					load_next_frame();					
				else
					throw std::runtime_error("failed to open " + video_file);
				return video_status;
			}

			bool read_gps(std::string gps_file_path)
			{
				bool gps_status = false;
				_gpsfile.open(gps_file_path);
				gps_status = _gpsfile.is_open();
				if (gps_status)
				{
					std::string temp;
					std::getline(_gpsfile, temp); // read header
				}					
				else
					throw std::runtime_error("failed to read gps file = "+ gps_file_path);
				return gps_status;
			}
			//there should be some logic to find the correct match between current frame and the gps data
			bool is_gps_matched()
			{
				return true;
			}
			void  get_next_item()
			{
				if (false == _image.empty())
				{
					image_t image = _image;
					double diff_timestamp = _timestamp;

					load_next_frame();
					diff_timestamp = diff_timestamp - _timestamp;

					pos_info_opt_t gps_ds;
					if (_gps_input_status && is_gps_matched())
					{
						gps_ds = (*_cur_gps_ds).second;
						get_next_gps();
					}
					_item = std::make_tuple(diff_timestamp, image, boost::none, gps_ds);
					_image_index++;
				}
				else
					_image_index = 0;
			}

			void load_next_frame()
			{
				_video_capture >> _image;
				_timestamp = _video_capture.get(CV_CAP_PROP_POS_MSEC);
			}
			void get_next_gps()
			{		
				string line;
				if (_gps_input_status)
				{
					if (std::getline(_gpsfile, line))
					{
						tokenizer tok(line);
						std::uint64_t timestamp = 0;
						long double lat = 0, lon = 0, alt = 0;
						int column = 0;
						string time_str;
						for (tokenizer::iterator it = tok.begin(); it != tok.end(); ++it, column++)
						{							
							switch ((dash_cam_csv_column)column)
							{
							case dash_cam_csv_column::date_col:
								time_str = *it;
								break;
							case dash_cam_csv_column::time_col:
								time_str +=*it;
								break;
							case dash_cam_csv_column::lat_col:
									lat = stod(*it);
								break;
							case dash_cam_csv_column::long_col:
									lon = stod(*it);
								break;
							}		
						}
						time_str = time_str.c_str() +1; //skip space character
						if (time_str.length() >= 19)
						{
							struct tm tm;
							tm.tm_year = stoi(time_str.substr(0, 4)) - 1900;
							tm.tm_mon = stoi(time_str.substr(5, 2)) - 1;
							tm.tm_mday = stoi(time_str.substr(8, 2));

							tm.tm_hour = stoi(time_str.substr(11, 2));
							tm.tm_min = stoi(time_str.substr(14, 2));
							tm.tm_sec = stoi(time_str.substr(17, 2));
							timestamp = mktime(&tm);
						}
						_cur_gps_ds = std::make_pair(timestamp, pos_info{ Eigen::Vector3d(lat ,lon ,alt) ,(Eigen::Matrix3d::Random()) });
					}
					else
					{
						_gps_input_status = false;
						_gpsfile.close();
					}
				}				
				
			}

			ds_dashcam(ds_dashcam_args& dashcam_args_) : _dashcam_args(dashcam_args_)
			{
				read_video(dashcam_args_._path_to_video);
				_gps_input_status = read_gps(dashcam_args_._path_to_csv_file);
				get_next_item();
			}
			ds_dashcam()
			{
			}
			//iterator
			ds_dashcam begin()
			{
				if (_data_source_status)
					return ds_dashcam(_dashcam_args);

				return ds_dashcam();
			}
			ds_dashcam end() { return ds_dashcam(); }
			bool equal(const ds_dashcam& other) const { return _image_index == other._image_index; }
			void increment() { get_next_item(); }
			std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t> & dereference() const { return (std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t> & const)_item; }

		};

	}
}