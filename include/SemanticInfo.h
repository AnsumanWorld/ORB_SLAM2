/**
* This file is part of ORB-SLAM2.
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef SEMANTIC_INFO_H
#define SEMANTIC_INFO_H

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
	using time_point_t = double/*std::chrono::time_point<double>*/;
	using image_t = cv::Mat;

	// the equivalent of the JSON object we use today...
	struct tsr_info {
		traffic_sign_map_t interested_object;
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

#define GET_IMG(sensor_data) std::get<0>(sensor_data)
#define GET_TIMESTAMP(sensor_data) std::get<1>(sensor_data)
#define GET_SENSOR_INFO(sensor_data) std::get<2>(sensor_data)
#define GET_TSR(sensor_data) std::get<2>(sensor_data).tsr.get()
#define GET_POS(sensor_data) std::get<2>(sensor_data).pos.get()
#define GET_TRAFICSIGN(sensor_data) std::get<2>(sensor_data).tsr.get().interested_object
#define GET_TRAFICSIGN_ITERATOR(sensor_data,Frameid) std::get<2>(sensor_data).tsr.get().interested_object.find(Frameid)
#define FIND_TRAFICSIGN(sensor_data,Frameid) ( std::get<2>(sensor_data).tsr.get().interested_object.find(Frameid) != std::get<2>(sensor_data).tsr.get().interested_object.end() )

}// namespace ORB_SLAM

#endif // SEMANTIC_INFO_H
