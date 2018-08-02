#pragma once

#include "ext/messages.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include<iterator>
#include<opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include "utils/utils.h"
#include <boost/program_options.hpp>
#include "utils/ds_image.h"

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
                        ("image,i", value<std::string>(), "image_folder")
                        ("video,v", value<std::string>(), "video file");

                    store(parse_command_line(argc, argv, desc), _vm);
                    notify(_vm);
                    if(!_vm.count("image") && !_vm.count("video"))
                        throw std::runtime_error("--image or --video is required");
                }
                catch(const error &ex){
                    if (_vm.count("help"))
                        std::cout << desc << '\n';
                    else
                        std::cout << ex.what() << "\nFor help: ./run_dashcam --help"  << std::endl;
                    throw std::runtime_error("exiting application");
                }
            }
            const std::string get_val(const std::string &name) const
            {
                if (_vm.count(name))
                    return _vm[name].as<std::string>();
                return "";
            }
            bool _is_video_input() const
            {
                return _vm.count("video");
            }
            const std::string get_source() const
            {
                if (_vm.count("image"))
                    return _vm["image"].as<std::string>();
                return _vm["video"].as<std::string>();
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
            std::uint64_t    _gps_index{0};
            std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t> _item;           
            std::vector<utils::gps_subtitle> _gps_subtitles;
            utils::gps_info _org_gps;
            utils::ds_image _ds_image_ins;
            
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
                    }
                }
                return cur_gps_ds;
            }

		public:

            //read video file and read gps from video
            ds_dashcam::ds_dashcam(const ds_dashcam_args& dashcam_args_)
                :_ds_image_ins(dashcam_args_.get_source()
                ,dashcam_args_._is_video_input())
            {
                if(dashcam_args_._is_video_input())
                    utils::extract_gps_from_video(dashcam_args_.get_val("video"),_gps_subtitles);
            }

            ds_dashcam(const ds_dashcam& obj):_ds_image_ins(obj._ds_image_ins)
            {
                if (false == _ds_image_ins.is_empty())
                {
                    copy(obj._gps_subtitles.begin(), obj._gps_subtitles.end(), back_inserter(_gps_subtitles));
                    increment();
                }
            }
            ds_dashcam(){}

            bool equal(const ds_dashcam& other) const
            {
                return _ds_image_ins.is_empty();
            }

            void increment()
            {
                double timestamp = _ds_image_ins.get_timestamp();
                _item = std::make_tuple(timestamp, *_ds_image_ins, boost::none, get_next_gps(timestamp));
                _ds_image_ins++;
            }

            auto& dereference() const
            {
                return _item;
            }
        };

    }
}