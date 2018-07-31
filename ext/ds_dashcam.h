#pragma once

#include "ext/messages.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include<iterator>
#include<opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include "utils/utils.h"

using namespace std;

namespace ORB_SLAM2 {
    namespace ext {	

        struct ds_dashcam_args 
        {
            std::string _path_to_video;

            ds_dashcam_args(int argc, char** argv)
            {
                if (argc < 4) {
                    std::cout << "Usage: ./run_dashcam_monocular <path_to_vocabulary> <path_to_camera_settings> <path_to_video>" << std::endl;
                }
                _path_to_video = argv[3];
            }

            ds_dashcam_args()
            {
            }
        };

        class ds_dashcam : public boost::iterator_facade<
                            ds_dashcam,
                            const std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t>,
                            boost::single_pass_traversal_tag >
        {
            cv::VideoCapture _video_capture;
            std::uint64_t    _gps_index{0};
            std::int64_t    _image_index{-1};
            image_t          _next_image;
            double           _next_timestamp{ 0 };
            std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t> _item;
            ds_dashcam_args  _dashcam_args;            
            std::vector<utils::gps_subtitle> _gps_subtitles;
            utils::gps_info _org_gps;
            
            //read video file
            void read_video(std::string video_file)
            {
                if (false == _video_capture.open(video_file))
                    throw std::runtime_error(video_file + " is failed to open");
            }
            
            //collect next image and gps data in _item
            void  get_next_item()
            {
                image_t cur_image;
                double cur_timestamp=0,diff_timestamp = 0;
                std::tie(cur_image, cur_timestamp, diff_timestamp) = get_next_frame();
                if (false == cur_image.empty())
                    _item = std::make_tuple(diff_timestamp, cur_image, boost::none, get_next_gps(cur_timestamp));
            }

            //collect next from from video
            std::tuple<image_t, double, double>  get_next_frame()
            {
                image_t cur_image;
                double cur_timestamp = 0;
                double time_diff = 0;

                if (false == _next_image.empty())
                {
                    cur_image = _next_image;
                    cur_timestamp = _next_timestamp;
                }
                else
                {
                    if (_video_capture.read(cur_image))
                    {
                        cur_timestamp = _video_capture.get(CV_CAP_PROP_POS_MSEC);
                    }
                }

                if (false == cur_image.empty())
                {                    
                    _image_index++;
                    if (_video_capture.read(_next_image))
                    {
                        _next_timestamp = _video_capture.get(CV_CAP_PROP_POS_MSEC);
                        time_diff = _next_timestamp - cur_timestamp;
                    }                   
                }
                else
                {
                    _video_capture.release();
                    _image_index = 0;
                }
                   
                return std::make_tuple(cur_image, cur_timestamp, time_diff);
            }

            //collect next gps from gps vector
            pos_info_opt_t get_next_gps(double cur_timestamp)
            {
                pos_info_opt_t cur_gps_ds;

                if (_gps_index < _gps_subtitles.size())
                {
                    if ((int64_t)cur_timestamp >= _gps_subtitles[_gps_index]._timestamp)
                    {     
                        utils::gps_info gps;
                        utils::lon_lat_from_string(_gps_subtitles[_gps_index]._gps_str, gps._lat, gps._lon);
                        if ((_org_gps._lat != 0) || (_org_gps._lon !=0))
                            _org_gps = gps;

                        cur_gps_ds = utils::get_pos_info(_org_gps, gps);
                        _gps_index++;
                        get_next_valid_gps();
                    }
                }
                return cur_gps_ds;
            }

            //move to valid gps subtitle
            void get_next_valid_gps()
            {
                while (_gps_index < _gps_subtitles.size())
                {
                    if (utils::valid_gps(_gps_subtitles[_gps_index]._gps_str) == true)
                        break;
                    _gps_index++;
                }
            }
		public:

            //read video file and read gps from video
            ds_dashcam::ds_dashcam(const ds_dashcam_args& dashcam_args_):_dashcam_args(dashcam_args_)
            {
                read_video(_dashcam_args._path_to_video);
                utils::extract_gps_from_video(_dashcam_args._path_to_video,_gps_subtitles);  
                get_next_valid_gps();
                _image_index = 0;
            }

            ds_dashcam(const ds_dashcam& obj) :_dashcam_args(obj._dashcam_args)
            {
                if (obj._image_index != -1)
                {
                    _image_index = 0;
                    read_video(_dashcam_args._path_to_video);
                    copy(obj._gps_subtitles.begin(), obj._gps_subtitles.end(), back_inserter(_gps_subtitles));
                    get_next_valid_gps();
                    get_next_item();
                }
            }
            ds_dashcam(){}

            bool equal(const ds_dashcam& other) const
            {
                return _image_index == other._image_index;
            }

            void increment()
            {
                get_next_item();
            }

            auto& dereference() const
            {
                return _item;
            }
        };

    }
}