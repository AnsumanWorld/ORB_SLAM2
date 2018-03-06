#pragma once

#include "ext/messages.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <iostream>

namespace fs = boost::filesystem;
using namespace std;
using boost::property_tree::ptree;

enum class data_source_type { semantic = 0, gps = 1 };
class semantic_data_source;
template <class output_t, class input_t>
class data_source {
public:	
	virtual void get_next(output_t &) = 0;
	static data_source* make_data_source(data_source_type choice, input_t input_data)
	{
		switch (choice)
		{
		case data_source_type::semantic:
			return new semantic_data_source(input_data);
			break;
		default:
			return nullptr;
		}
	}
	virtual ~data_source() {}
};

class semantic_data_source :public data_source< ORB_SLAM2::ext::slam_input_t, std::string> {
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
	virtual void get_next(ORB_SLAM2::ext::slam_input_t &slam_input)
	{
		bool found_semantic = false;
		if (true == semantic_info_status)
		{
			if (semantic_info_start != semantic_info_end)
			{
				std::string image_name = semantic_info_start->first;
				auto &traffic_sign_arr = semantic_info_start->second;
				std::vector<ORB_SLAM2::ext::traffic_sign> traffic_signs;
				BOOST_FOREACH(boost::property_tree::ptree::value_type &node, traffic_sign_arr.get_child("traffic_signs"))
				{
					ORB_SLAM2::ext::traffic_sign t;
					t.class_id = node.second.get<int>("class_id");
					t.confidence = node.second.get<double>("confidence");
					std::vector<double> r;
					for (auto &temppt : node.second.get_child("rectangle")) {
						r.push_back(temppt.second.get_value < double >());
					}

					transform_rect(r, t.box);
					traffic_signs.push_back(t);
					semantic_info_start++;
				}
				std::get<ORB_SLAM2::ext::time_point_t>(slam_input)= stoul(image_name);
				std::get<ORB_SLAM2::ext::tsr_info_opt_t>(slam_input)= traffic_signs;
			}
		}
	}
	semantic_data_source(std::string jsonFilename)
	{
		semantic_info_status = read_semantic_info(jsonFilename);
	}

	void transform_rect(std::vector<double> &rect_arr, cv::Rect& box, bool is_absolute = false)
	{
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

	~semantic_data_source()
	{
		semantic_info_status = false;
	}
};

template <class output_t, class input_t>
class data_source_container{
	//std::tuple<ext::image_t, ext::time_point_t, ext::tsr_info_opt_t, ext::pos_info_opt_t> slam_input;
	output_t slam_input;
	vector< data_source<output_t, input_t>* > data_source_list;
	public:

	void add_data_source(data_source_type choice, input_t input_data){
		data_source_list.push_back(data_source<output_t, input_t>::make_data_source(choice,input_data));
	}

	output_t& get_next()
	{
		for(auto data_src_obj : data_source_list){
			data_src_obj->get_next(slam_input);
		}
		return slam_input;
	}
	~data_source_container()
	{
		for(auto data_src_obj : data_source_list)
			delete data_src_obj;
	}

};







