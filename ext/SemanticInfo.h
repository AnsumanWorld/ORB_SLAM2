#pragma once

#include<string>
#include<opencv2/core/core.hpp>
#include <vector>
#include <map>
#include <iostream>
#include <boost/optional.hpp>
#include <chrono>
#include <vector>

namespace ORB_SLAM2
{
    struct traffic_sign
    {
        int classid;
        float confidence;
        cv::Rect roi;
    };
    using traffic_sign_map_t = std::map<long unsigned int, std::vector<traffic_sign> >;
	using traffic_sign_vec_t = std::vector<traffic_sign>;
	using time_point_t = double/*std::chrono::time_point<double>*/;
	using image_t = cv::Mat;

	// the equivalent of the JSON object we use today...
	struct tsr_info {
		traffic_sign_map_t interested_object;

		bool find_trafficsign(double frameid)
		{
			return interested_object.find(frameid) != interested_object.end();
		}

		traffic_sign_vec_t get_traffic_sign(double frameid)
		{
			traffic_sign_vec_t ts;
			if(find_trafficsign(frameid))
				ts = interested_object.find(frameid)->second;
			return ts;
		}
	};

	// vector, matrix are aliases to Eigen types
	struct pos_info {
		// longitude, latitude, altitude?
		std::vector<int> position;
		//symmetric_matrix<3, 3> covariance;
	};

	struct sensor_info {
		boost::optional<tsr_info> tsr;
		boost::optional<pos_info> pos;

	};
}// namespace ORB_SLAM
