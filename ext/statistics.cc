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

			statistics::statistics():reject_kf_by_tracking_for_busy_count(0),
				                   reject_kf_by_tracking_count(0),
				                   reject_kf_by_keyframeculling_count(0),
								   total_kf_by_tracking_count(0)
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

			void statistics :: reject_kf_by_tracking_for_busy()
			{
				reject_kf_by_tracking_for_busy_count++;
			}
			void statistics::reject_kf_by_tracking()
			{
				reject_kf_by_tracking_count++;
			}
			void statistics::reject_kf_by_keyframeculling()
			{
				reject_kf_by_keyframeculling_count++;
			}
			void statistics::total_kf_by_tracking()
			{
				total_kf_by_tracking_count++;
			}
			float statistics::cal_percent(int sub_amount , int total_amount)
			{
				return (float(sub_amount * 100) / total_amount);
			}
			void statistics::update_orbslam_status(int total_frames, int total_keyframes, int total_mappoints, int total_semantic_mappoints)
			{
				BOOST_LOG(glog) << "=========================================================================================================================================";
				BOOST_LOG(glog) << "*** ORB_SLAM2 status ***";
				BOOST_LOG(glog) << "=========================================================================================================================================";
				BOOST_LOG(glog) << "***frames and keyframe info ***";
				BOOST_LOG(glog) << "Total frames = " << std::to_string(total_frames);
				BOOST_LOG(glog) << "Total keyframe= " << std::to_string(total_keyframes) << "(" << cal_percent(total_keyframes, total_frames)<<")%";
				BOOST_LOG(glog) << "local key frame created by tracking = " << total_kf_by_tracking_count << "(" << cal_percent(total_kf_by_tracking_count, total_frames) << "%)";
				BOOST_LOG(glog) << "=========================================================================================================================================";

				BOOST_LOG(glog) << "reject keyframe by tracking(NeedNewKeyFrame()) -:";
				BOOST_LOG(glog) << "   rejected key frame by tracking = " << "rejected key frame for local mapping busy" << " + " << "rejected key frame by tracking except local mapping busy case";
				BOOST_LOG(glog) << "   rejected key frame by tracking = " << reject_kf_by_tracking_count << "(" << cal_percent((reject_kf_by_tracking_count), total_frames) << "%)";
				BOOST_LOG(glog) << "     rejected key frame for local mapping busy = " << reject_kf_by_tracking_for_busy_count <<"("<<cal_percent(reject_kf_by_tracking_for_busy_count, total_frames)<<"%)";
				BOOST_LOG(glog) << "     rejected key frame by tracking except local mapping busy case= " << reject_kf_by_tracking_count- reject_kf_by_tracking_for_busy_count << "(" << cal_percent((reject_kf_by_tracking_count- reject_kf_by_tracking_for_busy_count), total_frames) << "%)";
				BOOST_LOG(glog) << "=========================================================================================================================================";
				BOOST_LOG(glog) << "reject keyframe by localmapping(KeyFrameCulling()) -:";
				BOOST_LOG(glog) << "   rejected key frame by keyframe culling(extra 2 which autocreated by slam in initialization) = " << reject_kf_by_keyframeculling_count << "(" << cal_percent(reject_kf_by_keyframeculling_count, total_frames) << "%)";
				BOOST_LOG(glog) << "=========================================================================================================================================";
				int reject_kf_by_monocular_initialization = (total_frames - reject_kf_by_tracking_count - reject_kf_by_keyframeculling_count - total_keyframes);
				BOOST_LOG(glog) << "reject keyframe by other factor -:";
				BOOST_LOG(glog) << "   rejected key frame by initialization = " << reject_kf_by_monocular_initialization << "("<<cal_percent(reject_kf_by_monocular_initialization, total_frames) <<"%)";
				BOOST_LOG(glog) << "=========================================================================================================================================";
				BOOST_LOG(glog) << "reject keyframe(aggr) -:";
				int rejected_keyframe = reject_kf_by_tracking_count + reject_kf_by_keyframeculling_count + reject_kf_by_monocular_initialization;
				BOOST_LOG(glog) << "   rejected key frame("<< rejected_keyframe <<") = " << "rejected key frame by tracking ("<< reject_kf_by_tracking_count <<")" << " + " << "rejected key frame by keyframe culling("<< reject_kf_by_keyframeculling_count<<")" <<" + "<< "rejected key frame by initialization("<< reject_kf_by_monocular_initialization<<")";

				BOOST_LOG(glog) << "=========================================================================================================================================";
				BOOST_LOG(glog) << "***Map-points***";
				BOOST_LOG(glog) << "Total optimized Map-points = " << std::to_string(total_mappoints) << " Total optimized semantic Map-points = " << std::to_string(total_semantic_mappoints) << " selection percentage = " << cal_percent(total_semantic_mappoints, total_mappoints) << "%";
				BOOST_LOG(glog) << "=========================================================================================================================================";
			}
	}
}