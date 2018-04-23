#include "statistics.h"
#include <iostream>
#include <string>

namespace ORB_SLAM2
{
	namespace ext {

			statistics& statistics::get()
			{
				static statistics instance;
				return instance;
			}

			statistics::statistics():reject_frame_for_local_mapping_busy(0)
			{
				
				logging::add_file_log
				(
					keywords::file_name = "orb_slam2_statistics_%N.log"
					// This makes the sink to write log records that look like this:
					// YYYY-MM-DD HH:MI:SS: <normal> A normal severity message
					// YYYY-MM-DD HH:MI:SS: <error> An error severity message
					//keywords::format =
					//(
						//expr::stream << expr::smessage
							//<< expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
							//<< "<" << logging::trivial::severity
							//<< "> " << expr::smessage
					//)
				);
				logging::add_common_attributes();
			}

			void statistics :: reject_frame_to_keyframe(bool is_localmapping_busy)
			{
				if (true == is_localmapping_busy)
					reject_frame_for_local_mapping_busy++;
			}
			float statistics::cal_percent(int sub_amount , int total_amount)
			{
				return (float(sub_amount * 100) / total_amount);
			}
			void statistics::update_orbslam_status(int total_frames, int total_keyframes, int total_mappoints, int total_semantic_mappoints)
			{
				BOOST_LOG(glog) << "=========================================================================================================================================";
				BOOST_LOG(glog) << "\n\n*** ORB_SLAM2 status ***";
				BOOST_LOG(glog) << "=========================================================================================================================================";
				BOOST_LOG(glog) << "***frames  ***";
				BOOST_LOG(glog) << "Total frames = " << std::to_string(total_frames) << " Total keyframe = " << std::to_string(total_keyframes) << " selection percentage = " << cal_percent(total_keyframes, total_frames);
				BOOST_LOG(glog) << "rejected percentage for local mapping busy = " << cal_percent(reject_frame_for_local_mapping_busy, total_frames) << "% rejected percentage for other = " << cal_percent(total_frames- reject_frame_for_local_mapping_busy, total_frames)<<"%";

				//BOOST_LOG(glog) << "Total keyframe = " << std::to_string(total_keyframes);
				BOOST_LOG(glog) << "***Map-points  ***";
				BOOST_LOG(glog) << "Total Map-points = " << std::to_string(total_mappoints) << " Total semantic Map-points = " << std::to_string(total_semantic_mappoints) << " selection percentage = " << cal_percent(total_semantic_mappoints, total_mappoints) << "%";
				//BOOST_LOG(glog) << "Total Map-points = " << std::to_string(total_semantic_mappoints);
				BOOST_LOG(glog) << "=========================================================================================================================================";
			}
	}
}