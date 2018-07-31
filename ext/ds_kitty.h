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
#include <sstream>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "utils/utils.h"

namespace fs = boost::filesystem;
using namespace std;
using boost::property_tree::ptree;
using namespace boost::posix_time;

namespace ORB_SLAM2 {
    namespace ext {
        struct ds_kitty_args {
            std::string _path_to_image_folder;
            std::string _path_to_timestamp_file;
            std::string _path_to_gps_folder;

            ds_kitty_args(int argc, char** argv)
            {
                if (argc < 4) 
                    throw std::runtime_error( "Usage: ./run_kitty <path_to_vocabulary> <path_to_camera_settings> <path_to_image_folder> <_path_to_timestamp_file> <path_to_gps_folder>");
                _path_to_image_folder = argv[3];
                _path_to_timestamp_file = (argc >= 5) ? argv[4] : "";
                _path_to_gps_folder =  (argc >= 6) ? argv[5] : "";
            }
            ds_kitty_args()
            {
            }
        };

        class ds_kitty : public boost::iterator_facade<
                        ds_kitty,
                        const std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t>,
                        boost::single_pass_traversal_tag >
        {
            std::vector<fs::path> _image_files;
            std::vector<fs::path> _gps_files;
            std::ifstream _timestamp_file;
            std::int64_t _image_index{-1};
            bool _gps_input_status{false};
            ds_kitty_args _kitty_args;
            std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t> _item;
            utils::gps_info _org_gps;

            void read_image_files(std::string path_to_image_folder_)
            {
                if (false == path_to_image_folder_.empty())
                {
                    if (fs::exists(path_to_image_folder_))
                    {
                        std::copy(fs::directory_iterator(path_to_image_folder_), fs::directory_iterator(),
                            std::back_inserter(_image_files));
                        if (_image_files.size())
                            std::sort(_image_files.begin(), _image_files.end());
                    }
                    else
                        throw std::runtime_error(path_to_image_folder_ + " is not exist");
                }
            }

            void read_timestamp_files(std::string path_to_timestamp_file_)
            {
                if (false == path_to_timestamp_file_.empty())
                {
                    _timestamp_file.open(path_to_timestamp_file_);
                    if (false == _timestamp_file.is_open())
                        throw std::runtime_error("failed to read timestamp file = " + path_to_timestamp_file_);
                }
            }

            bool read_gps_files(std::string path_to_gps_folder_)
            {
                bool read_gps_status = false;
                if (false == path_to_gps_folder_.empty())
                {
                    if (fs::exists(path_to_gps_folder_))
                    {
                        std::copy(fs::directory_iterator(path_to_gps_folder_), fs::directory_iterator(),
                            std::back_inserter(_gps_files));
                        if (_gps_files.size())
                        {
                            std::sort(_gps_files.begin(), _gps_files.end());
                            get_next_gps();
                            read_gps_status = true;
                        }
                    }
                    else
                        throw std::runtime_error("failed to read gps file = " + path_to_gps_folder_);
                }
                else
                    std::cout << " gps is not provided" << std::endl;

                return read_gps_status;
            }

        public:

            void get_next_item()
            {
                if (_image_index < _image_files.size())
                {
                    image_t image = cv::imread(_image_files[_image_index].generic_string(), CV_LOAD_IMAGE_UNCHANGED);
                    _item = std::make_tuple(get_next_timestamp(), image, boost::none, boost::none);
                    _image_index++;
                }
                else
                    _image_index = 0;
            }

            double get_next_timestamp()
            {
                std::string line_str;
                double time_stamp = 0;

                if (std::getline(_timestamp_file, line_str))
                {
                    //there are two timestamp format provided by kitty
                    //in one timestamp format yyyy-mm-26 hh:mm:ss
                    if (line_str.find(":") != std::string::npos)
                    {
                        ptime t(time_from_string(line_str));
                        time_stamp = t.time_of_day().total_microseconds();
                    }
                    else
                        time_stamp = stod(line_str);
                }
                else
                    _timestamp_file.close();
                return time_stamp;
            }

            pos_info_opt_t get_next_gps()
            {
                pos_info_opt_t cur_gps_ds;
                if (_gps_input_status)
                {
                    std::string line_str;
                    
                    ifstream gpsfile(_gps_files[_image_index].generic_string());
                    if (gpsfile.is_open() && std::getline(gpsfile, line_str))
                    {
                        std::stringstream ss(line_str);
                        utils::gps_info gps;
                        ss >> gps._lat >> gps._lon >> gps._alt>> gps._roll>> gps._pitch>> gps._yaw;

                        if (!_image_index)
                            _org_gps = gps;

                        cur_gps_ds = utils::get_pos_info(_org_gps, gps);
                    }
                    gpsfile.close();
                }
                return cur_gps_ds;
            }

            ds_kitty(const ds_kitty_args& kitty_args_):_kitty_args(kitty_args_)
            {
                read_image_files(_kitty_args._path_to_image_folder);
                read_timestamp_files(_kitty_args._path_to_timestamp_file);
                _gps_input_status = read_gps_files(_kitty_args._path_to_gps_folder);
                _image_index = 0;
            }

            ds_kitty(const ds_kitty& obj) :_kitty_args(obj._kitty_args)
            {
                if (obj._image_index != -1)
                {
                    _image_index = 0;
                    copy(obj._image_files.begin(), obj._image_files.end(), back_inserter(_image_files));
                    copy(obj._gps_files.begin(), obj._gps_files.end(), back_inserter(_gps_files));
                    read_timestamp_files(_kitty_args._path_to_timestamp_file);
                    _gps_input_status = obj._gps_input_status;
                    get_next_item();
                }
            }
            ds_kitty(){}
            ~ds_kitty()
            {
                _timestamp_file.close();
            }
            bool equal(const ds_kitty& other) const { 
                return _image_index == other._image_index; 
            }
            void increment() { 
                get_next_item(); 
            }
            auto& dereference() const { 
                return _item; 
            }
        };

    }
}