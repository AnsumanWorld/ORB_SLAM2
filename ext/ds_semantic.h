#pragma once

#include "ext/messages.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include<iterator>
#include <boost/lexical_cast.hpp>

namespace fs = boost::filesystem;
using namespace std;
using boost::property_tree::ptree;

namespace ORB_SLAM2 {
    namespace ext {
		//read semantic info from txt file
		struct semantic_data {
			std::string _image_name;
			std::string _time_point;
			int _classid;
			double _confidence;
			std::vector<double> _vec_tsr_info;
			std::vector< std::vector<int> > _vec_cov;
			std::vector<  std::vector<double> > _vec_pos;

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
						_classid = node.second.get<int>("class_id");
						_confidence = node.second.get<double>("confidence");
						for (auto &temp : node.second.get_child("rectangle")) {
							_vec_tsr_info.push_back(temp.second.get_value < double >());
						}
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
						std::vector<double> pos;
						pos.push_back(node.second.get_value<double>());
						_vec_pos.push_back(pos);
					}
				}
			}
		};

		struct ds_semantic_args {
			std::string _path_to_image_folder;
			std::string _path_to_json_file;

			ds_semantic_args(int argc, char** argv)
			{
				if (argc < 4) {
					throw std::runtime_error("Usage: ./run_semantic <path_to_vocabulary> <path_to_camera_settings> <path_to_image_folder> <path_to_json_file>");
				}
				_path_to_image_folder = argv[3];
				_path_to_json_file = (argc >= 5) ? argv[4] : "";
			}
			ds_semantic_args()
			{
			}
		};

		class ds_semantic : public boost::iterator_facade<
							ds_semantic,
							std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t>,
							boost::single_pass_traversal_tag > 
		{
			std::vector<fs::path> _image_files;
			std::fstream _jsonfile;
			std::uint64_t _image_index{0};
			//contain semantic info of image_index
			semantic_info_t _cur_semantic_ds; 
			bool _json_input_status{false};
			std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t> _item;
			ds_semantic_args _semantic_args;
			int img_width{ 1280 };
			int img_height{ 720 };

			void read_image_files(std::string path_to_image_folder_)
			{
				if (false == path_to_image_folder_.empty())
				{
					if (fs::exists(path_to_image_folder_) && fs::is_directory(path_to_image_folder_))
					{
						std::copy(fs::directory_iterator(path_to_image_folder_), fs::directory_iterator(),
							std::back_inserter(_image_files));
						if (_image_files.size())
						{
							std::sort(_image_files.begin(), _image_files.end());

							//read first image and set image width and height
							image_t image = cv::imread(_image_files[0].generic_string(), CV_LOAD_IMAGE_UNCHANGED);
							img_width = image.cols;
							img_height = image.rows;
						}
					}
					else
						throw std::runtime_error(path_to_image_folder_ + " is not exist or it is not a directory");

				}					
			}

			void read_jsonparser(std::string jsonFilename)
			{
				if (false == jsonFilename.empty())
				{
					_jsonfile.open(jsonFilename, std::fstream::in);
					if (false == _jsonfile.is_open())
						throw std::runtime_error("Unable to open json file "+ jsonFilename);
					else
					{
						_json_input_status = true;
						get_next_semantic();
						
					}						
				}
			}

			int extract_numfromstr(std::string name)
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
			void get_next_item()
			{				
				if (_image_index < _image_files.size())
				{
					int cur_image_number = extract_numfromstr(_image_files[_image_index].generic_string());
					if (cur_image_number != -1)
					{
						image_t image = cv::imread(_image_files[_image_index].generic_string(), CV_LOAD_IMAGE_UNCHANGED);
						time_point_t timestamp = 0;

						tsr_info_opt_t tsr_ds = boost::none;
						pos_info_opt_t gps_ds = boost::none;;

						if (_json_input_status && std::get<0>(_cur_semantic_ds) == cur_image_number)
						{
							timestamp = std::get<1>(_cur_semantic_ds); //current timestamp is treated as image index for orb-sam2
							tsr_ds = std::get<2>(_cur_semantic_ds);
							gps_ds = std::get<3>(_cur_semantic_ds);
							get_next_semantic();
							timestamp = std::get<1>(_cur_semantic_ds) - timestamp;
						}
						_item = std::make_tuple(timestamp, image, tsr_ds, gps_ds);
					}
					_image_index++;
				}
				else
					_image_index = 0;
			}

			void get_next_semantic()
			{
				if (true == _json_input_status)
				{
					std::string line;
					if (std::getline(_jsonfile, line))
					{
						semantic_data semantic_data_obj;
						semantic_data_obj.get_next_item(line);
						//semantic_data_obj.print_data();
						std::get<0>(_cur_semantic_ds) = stol(semantic_data_obj._image_name);
						std::get<1>(_cur_semantic_ds) = stod(semantic_data_obj._time_point);
						std::get<2>(_cur_semantic_ds) = boost::none;
						std::get<3>(_cur_semantic_ds) = boost::none;

						if (semantic_data_obj._vec_tsr_info.size())
						{
							std::vector<ORB_SLAM2::ext::traffic_sign> traffic_signs;
							ORB_SLAM2::ext::traffic_sign t;
							t.class_id = semantic_data_obj._classid;
							t.confidence = semantic_data_obj._confidence;
							transform_rect(semantic_data_obj._vec_tsr_info, t.box);
							traffic_signs.push_back(t);
							std::get<2>(_cur_semantic_ds) = traffic_signs;
						}
						if (semantic_data_obj._vec_cov.size() && semantic_data_obj._vec_pos.size())
						{
							int row = 0;

							Eigen::Matrix3d cov;
							for (auto cov_row : semantic_data_obj._vec_cov)
							{
								int col = 0;
								for (auto cov_col : cov_row)
								{
									cov(row, col) = cov_col;
									col++;
								}
								row++;
							}
							row = 0;
							Eigen::Vector3d pos;
							for (auto pos_row : semantic_data_obj._vec_pos)
							{
								int col = 0;
								for (auto pos_col : pos_row)
								{
									pos(row, col) = pos_col;
									col++;
								}
								row++;
							}
							std::get<3>(_cur_semantic_ds) = pos_info{ pos,cov };
						}
					}
					else
					{
						_json_input_status = false;
						_jsonfile.close();
					}					
				}
			}

			ds_semantic(ds_semantic_args semantic_args_) : _semantic_args(semantic_args_)
			{
				read_image_files(semantic_args_._path_to_image_folder);
				read_jsonparser(semantic_args_._path_to_json_file);
				get_next_item();
			}
			ds_semantic()
			{
			}

			void transform_rect(std::vector<double> &rect_arr, cv::Rect& box, bool is_absolute = false)
			{
				const int min_roi_width = 90;
				const int min_roi_height = 90;

				if (true == is_absolute) {
					box.x = rect_arr[0];
					box.y = rect_arr[1];
					box.width = rect_arr[2] - rect_arr[0];
					box.height = rect_arr[3] - rect_arr[1];
				}
				else {
					int ymin = int(rect_arr[0] * img_height);
					int xmin = int(rect_arr[1] * img_width);
					int ymax = int(rect_arr[2] * img_height);
					int xmax = int(rect_arr[3] * img_width);
					box.x = xmin;
					box.y = ymin;
					box.width = xmax - xmin;
					box.height = ymax - ymin;
				}
			}

			//iterator
			ds_semantic begin()
			{
				return ds_semantic(_semantic_args);
			}
			ds_semantic end() { return ds_semantic(); }
			bool equal(const ds_semantic& other) const { return _image_index == other._image_index; }
			void increment() { get_next_item(); }
			std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t> & dereference() const { return (std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t> & const)_item; }

		};
		
	}
}