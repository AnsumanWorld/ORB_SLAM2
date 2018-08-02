#pragma once

#include "ext/messages.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include<iterator>
#include <boost/lexical_cast.hpp>
#include "utils/utils.h"
#include <boost/program_options.hpp>
#include "utils/ds_image.h"

using namespace boost::program_options;
namespace fs = boost::filesystem;
using namespace std;
using boost::property_tree::ptree;

namespace ORB_SLAM2 {
    namespace ext {
        //read tsr info from txt file
        struct tsr_parser {
            std::string _image_name;
            std::string _time_point;
            std::vector<traffic_sign> _vec_tsr_info;
            std::vector< std::vector<int> > _vec_cov;
            std::vector<double> _vec_pos;
            int _img_width{ 1280 };
            int _img_height{ 720 };

            tsr_parser(int img_width_, int img_height_) 
                :_img_width(img_width_)
                , _img_height(img_height_)
            {
            }
            tsr_parser()
            {
            }
            void get_next_item(std::string linestr)
            {
                std::stringstream ss;
                ss << linestr;

                boost::property_tree::ptree pt;
                _vec_tsr_info.clear();
                _vec_cov.clear();
                _vec_pos.clear();

                boost::property_tree::read_json(ss, pt);
                _image_name = pt.get<std::string>("image");
                _time_point = pt.get<std::string>("time_point");
                if (pt.get_optional<std::string>("tsr_info"))
                {
                    BOOST_FOREACH(boost::property_tree::ptree::value_type &node, pt.get_child("tsr_info"))
                    {
                        double box[4];
                        int index = 0;
                        for (auto &temp : node.second.get_child("rectangle")) {
                            box[index]= temp.second.get_value < double >();
                            index++;
                        }

                        _vec_tsr_info.push_back(traffic_sign{ 
                            node.second.get<int>("class_id"), 
                            node.second.get<float>("confidence"),
                            transform_rect(box)
                            });
                    }
                }
                if (pt.get_optional<std::string>("pos_info"))
                {
                    int rows = 0;
                    BOOST_FOREACH(boost::property_tree::ptree::value_type &node, pt.get_child("pos_info.cov"))
                    {
                        std::vector<int> cov;
                        for (boost::property_tree::ptree::value_type &cell : node.second)
                        {
                            cov.push_back(cell.second.get_value<int>());
                        }
                        _vec_cov.push_back(cov);
                    }
                    BOOST_FOREACH(boost::property_tree::ptree::value_type &node, pt.get_child("pos_info.pos"))
                    {
                        _vec_pos.push_back(node.second.get_value<double>());
                    }
                }
            }

            cv::Rect transform_rect(double rect_arr[],bool is_absolute = false)
            {
                cv::Rect box;
                const int min_roi_width = 90;
                const int min_roi_height = 90;


                if (true == is_absolute) {
                    box.x = rect_arr[0];
                    box.y = rect_arr[1];
                    box.width = rect_arr[2] - rect_arr[0];
                    box.height = rect_arr[3] - rect_arr[1];
                }
                else {
                    int ymin = int(rect_arr[0] * _img_height);
                    int xmin = int(rect_arr[1] * _img_width);
                    int ymax = int(rect_arr[2] * _img_height);
                    int xmax = int(rect_arr[3] * _img_width);
                    box.x = xmin;
                    box.y = ymin;
                    box.width = xmax - xmin;
                    box.height = ymax - ymin;
                }
                return box;
            }
        };

        struct ds_tsr_args {
            variables_map _vm;
            options_description desc{ "Options" };
            ds_tsr_args(int argc, char** argv)
            {
                try{                    
                    desc.add_options()
                        ("help,h", "Help screen")
                        ("orbvoc,o", value<std::string>()->required(), "orb vocabulary")
                        ("setting,s", value<std::string>()->required(), "camera_settings")
                        ("image,i", value<std::string>(), "image_folder")
                        ("video,v", value<std::string>(), "video file")
                        ("tsr,t", value<std::string>()->required(), "tsr file");

                    store(parse_command_line(argc, argv, desc), _vm);
                    notify(_vm);
                    if (!_vm.count("image") && !_vm.count("video"))
                        throw std::runtime_error("--image or --video is required");
                }
                catch (const error &ex) {
                    if (_vm.count("help"))
                        std::cout << desc << '\n';
                    else
                        std::cout << ex.what() << "\nFor help: ./run_dashcam --help" << std::endl;
                    throw std::runtime_error("exiting application");
                }
            }
            const std::string get_val(const std::string &name) const
            {
                if (_vm.count(name))
                    return _vm[name].as<std::string>();
                return "";
            }
            ds_tsr_args()
            {
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
        };

        class ds_tsr : public boost::iterator_facade<
                            ds_tsr,
                            const std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t>,
                            boost::single_pass_traversal_tag > 
        {
            std::fstream _jsonfile;
            std::int64_t _gps_index{0};
            std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t> _item;
            std::string tsr_file;
            utils::gps_info _org_gps;
            utils::ds_image _ds_image_ins;
            tsr_parser _tsr_parser_ins;

            void read_jsonparser(std::string jsonFilename)
            {
                if (false == jsonFilename.empty())
                {
                    _jsonfile.open(jsonFilename, std::fstream::in);
                    if (false == _jsonfile.is_open())
                        throw std::runtime_error("Unable to open json file "+ jsonFilename);
                }
            }

            int extract_num(std::string name)
            {
                int extract_value = -1;
                auto start_index = name.rfind("/");
                auto end_index = name.rfind(".");
                if ((start_index != std::string::npos) && (end_index != std::string::npos))
                {
                    start_index += 1;
                    extract_value = boost::lexical_cast<int>(name.substr(start_index, end_index - start_index));
                }
                return extract_value;
            }
        public:

            pos_info_opt_t extract_gps(const std::vector<double> &pos_vec_)
            {
                pos_info_opt_t cur_gps_ds;
                if (pos_vec_.size())
                {
                    utils::gps_info gps;
                    gps._lat = pos_vec_[0];
                    gps._lon = pos_vec_[1];
                    gps._alt = pos_vec_[2];

                    if (!_gps_index)
                        _org_gps = gps;
                    else
                        _gps_index++;
                    cur_gps_ds = utils::get_pos_info(_org_gps, gps);
                }
                return cur_gps_ds;
            }

            tsr_info_opt_t extract_tsr(const std::vector<traffic_sign> &tsr_vec_)
            {
                tsr_info_opt_t cur_tsr_info;
                if (tsr_vec_.size())     
                    cur_tsr_info = tsr_vec_;
                return cur_tsr_info;
            }
            slam_input_t get_next_tsr()
            {
                std::string line;
                if (std::getline(_jsonfile, line))
                {
                    _tsr_parser_ins.get_next_item(line);                    
                    return std::make_tuple(
                        stod(_tsr_parser_ins._time_point), 
                        cv::Mat(), 
                        extract_tsr(_tsr_parser_ins._vec_tsr_info), 
                        extract_gps(_tsr_parser_ins._vec_pos));
                }
                return std::make_tuple(0, cv::Mat(), boost::none, boost::none);
            }

            ds_tsr(const ds_tsr_args& ds_args_) 
                :tsr_file(ds_args_.get_val("tsr"))
                , _ds_image_ins(ds_args_.get_source()
                , ds_args_._is_video_input())
                , _tsr_parser_ins((*_ds_image_ins).cols,(*_ds_image_ins).rows)
            {
                read_jsonparser(tsr_file);
            }
            ds_tsr(const ds_tsr& obj) :tsr_file(obj.tsr_file), _ds_image_ins(obj._ds_image_ins)
            {
                if (false == obj._ds_image_ins.is_empty())
                {
                    read_jsonparser(tsr_file);
                    increment();
                }
            }
            ds_tsr()
            {
            }
            ~ds_tsr()
            {
                _jsonfile.close();
            }


            bool equal(const ds_tsr& other) const { 
                return _ds_image_ins.is_empty();
            }
            void increment() { 
                time_point_t timestamp = 0;
                tsr_info_opt_t tsr_ds;
                pos_info_opt_t gps_ds;
                std::tie(timestamp, std::ignore, tsr_ds, gps_ds) = get_next_tsr();
                _item = std::make_tuple(timestamp, *_ds_image_ins, tsr_ds, gps_ds);
                _ds_image_ins++;
            }
            auto& dereference() const { 
                return _item; 
            }

        };
        
    }
}