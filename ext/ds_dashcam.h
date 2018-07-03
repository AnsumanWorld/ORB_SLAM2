#pragma once

#include "ext/messages.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include<iterator>
#include<opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"

using namespace std;

namespace ORB_SLAM2 {
	namespace ext {	

		struct gps_info_t
		{
			int			_frame_number;
			int64_t		_timestamp;
			std::string _gps_str;
			double		_longitude;
			double		_latitude;
		};

		struct ds_dashcam_args 
		{
			std::string _path_to_video;
			std::string _path_to_csv_file;

			ds_dashcam_args(int argc, char** argv);
			ds_dashcam_args();
		};

		class ds_dashcam : public boost::iterator_facade<
							ds_dashcam,
							std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t>,
							boost::single_pass_traversal_tag >
		{
			cv::VideoCapture _video_capture;
			std::uint64_t    _gps_index{0};
			std::uint64_t    _image_index{ 0 };
			bool             _data_source_status{false};
			bool             _gps_input_status{false};
			image_t          _image;
			double           _timestamp{ 0 };
			std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t> _item;
			ds_dashcam_args  _dashcam_args;
			std::vector<gps_info_t> _vec_gps_info;

		public:
			bool read_video(std::string video_file);
			void get_next_item();
			void load_next_frame();
			pos_info_opt_t load_next_gps();
			ds_dashcam(ds_dashcam_args& dashcam_args_);
			ds_dashcam(){}
			//iterator
			ds_dashcam begin();
			ds_dashcam end();
			bool equal(const ds_dashcam& other) const;
			void increment();
			std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t> & dereference() const;
		};

	}
}