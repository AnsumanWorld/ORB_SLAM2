#pragma once

#include "ext/messages.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include<iterator>
#include<opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include "utils/utils.h"
#include <boost/program_options.hpp>

using namespace boost::program_options;
using namespace std;

namespace ORB_SLAM2 {
    namespace ext {	

        struct ds_dashcam_args 
        {
            variables_map _vm;
            options_description desc{ "Options" };

            ds_dashcam_args(int argc, char** argv)
            {
                try{
                    
                    desc.add_options()
                        ("help,h", "Help screen")
                        ("orbvoc,o", value<std::string>()->required(), "orb vocabulary")
                        ("setting,s", value<std::string>()->required(), "camera settings")
                        ("video,v", value<std::string>()->required(), "video file");

                    store(parse_command_line(argc, argv, desc), _vm);
                    notify(_vm);
                }
                catch(const error &ex){
                    if (_vm.count("help"))
                        std::cout << desc << '\n';
                    else
                        std::cout << ex.what() << "\nFor help: ./run_dashcam --help"  << std::endl;
                    throw std::runtime_error("exiting application");
                }
            }
            const std::string get_val(const std::string &name)
            {
                if (_vm.count(name))
                    return _vm[name].as<std::string>();
                return "";
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
                image_t image;
                double timestamp = 0;
                _video_capture >> image;
                if (false == image.empty())
                {
                    timestamp = _video_capture.get(CV_CAP_PROP_POS_MSEC);
                    _item = std::make_tuple(timestamp, image, boost::none, get_next_gps(timestamp));
                }
                else
                    _image_index = -1;                                      
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
                read_video(_dashcam_args.get_val("video"));
                utils::extract_gps_from_video(_dashcam_args.get_val("video"),_gps_subtitles);
                _image_index = 0;
            }

            ds_dashcam(const ds_dashcam& obj) :_dashcam_args(obj._dashcam_args)
            {
                if (obj._image_index != -1)
                {
                    _image_index = 0;
                    read_video(_dashcam_args.get_val("video"));
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