#pragma once

#include "ext/SemanticInfo.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include<iostream>
# include <boost/iterator/iterator_adaptor.hpp>
#include <type_traits>

namespace fs = boost::filesystem;
using namespace std;
using boost::property_tree::ptree;

template <class Value>
class data_source_iter
	: public boost::iterator_adaptor<
	data_source_iter<Value>                // Derived
	, Value*                          // Base
	, boost::use_default              // Value
	, boost::forward_traversal_tag    // CategoryOrTraversal
	>
{
private:
	struct enabler {};  // a private type avoids misuse

	typedef boost::iterator_adaptor<
		data_source_iter<Value>, Value*, boost::use_default, boost::forward_traversal_tag
	> super_t;

public:
	data_source_iter()
		: super_t(0) {}

	explicit data_source_iter(Value* p)
		: super_t(p) {}

	template <class OtherValue>
	data_source_iter(
		data_source_iter<OtherValue> const& other
		, typename std::enable_if<std::is_convertible<OtherValue*, Value*>::value, enabler>::type = enabler()
	)
		: super_t(other.base()) {}

private: // GCC2 can't grant friendship to template member functions    
	friend class boost::iterator_core_access;  
	void increment() { this->base_reference() = this->base()->next(); }
};

enum class data_source_type { semantic = 0, gps = 1 };
class semantic_data_source;
template <class output_t, class input_t>
class data_source {
public:
	
	virtual output_t next() = 0;
	static std::unique_ptr<data_source> make_data_source(data_source_type choice, input_t input_data)
	{
		switch (choice)
		{
		case data_source_type::semantic:
			return std::make_unique<semantic_data_source>(input_data);
			break;
		default:
			return nullptr;
		}

	}

	data_source_iter< data_source > get_data_source_iter()
	{
		return data_source_iter< data_source >(this);
	}
	virtual ~data_source() {}
};

class semantic_data_source :public data_source< ORB_SLAM2::ext::traffic_sign_map_t, std::string> {
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
	virtual ORB_SLAM2::ext::traffic_sign_map_t next()
	{
		bool found_semantic = false;
		ORB_SLAM2::ext::traffic_sign_map_t SemanticObjGrp;
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
					t.classid = node.second.get<int>("class_id");
					t.confidence = node.second.get<double>("confidence");
					std::vector<double> r;
					for (auto &temppt : node.second.get_child("rectangle")) {
						r.push_back(temppt.second.get_value < double >());
					}

					transform_rect(r, t.roi);
					traffic_signs.push_back(t);
					semantic_info_start++;
				}
				SemanticObjGrp.insert({ stoul(image_name), traffic_signs });
			}
		}
		return SemanticObjGrp;
	}

	semantic_data_source(std::string jsonFilename)
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
		}
		else {
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

	~semantic_data_source()
	{
		semantic_info_status = false;
	}
};





