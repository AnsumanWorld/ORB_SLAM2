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

			statistics::statistics()
				: _frames(0)
				, _keyframes(0)
				, _frame_keypoints(0)
				, _keyframe_keypoints(0)
				, _frame_semantic_kp(0)
				, _keyframe_semantic_kp(0)
				, _mp(0)
				, _semantic_mp(0)
				, _map_mp_count(0)
				, _map_kf_count(0)
				, _map_kp_count(0)
				, _map_semantic_kp_count(0)
				, _map_semantic_mps(0)
				, _prev_total_keyframes(0)
				, _prev_total_mappoints(0)
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

			void statistics::update_frame_count(const std::size_t& count)
			{
				_frames += count;
			}

			void statistics::update_keyframe_count(const std::size_t& count)
			{
				_keyframes += count;
			}

			void statistics::update_frame_keypoint_stats(
				const std::vector<cv::KeyPoint>& kps)
			{
				auto semantic_kp = std::count_if(kps.begin(),
					kps.end(),
					[](const cv::KeyPoint& kp) { return kp.class_id != -1; });

				_frame_keypoints += kps.size();
				_frame_semantic_kp += semantic_kp;
			}

			void statistics::update_keyframe_keypoint_stats(
				const std::vector<cv::KeyPoint>& kps)
			{
				auto semantic_kp = std::count_if(kps.begin(),
					kps.end(),
					[](const cv::KeyPoint& kp) { return kp.class_id != -1; });

				_keyframe_keypoints += kps.size();
				_keyframe_semantic_kp += semantic_kp;
			}

			void statistics::update_mappoint_count(const std::size_t& count)
			{
				_mp += count;
			}

			void statistics::update_semantic_mappoint_count(const std::size_t& count)
			{
				_semantic_mp += count;
			}

			void statistics::update_map_stats(Map* map)
			{
				_map_mp_count += map->MapPointsInMap();
				_map_kf_count += map->KeyFramesInMap();

				auto kfs = map->GetAllKeyFrames();
				auto mps = map->GetAllMapPoints();

				_map_semantic_mps += std::count_if(mps.begin(), mps.end(),
					[](MapPoint* mp) { return mp->is_semantic(); });

				std::for_each(kfs.begin(), kfs.end(),
					[this](KeyFrame* kf)
				{
					this->_map_kp_count += kf->mvKeys.size();
					this->_map_semantic_kp_count += std::count_if(kf->mvKeys.begin(),
						kf->mvKeys.end(),
						[](const cv::KeyPoint& kp) { return kp.class_id != -1; });
				});
			}
			void statistics::print_border(string msg)
			{
				BOOST_LOG(glog) << "============================================================================================================";
				BOOST_LOG(glog) << msg;
				BOOST_LOG(glog) << "============================================================================================================";
			}
			void statistics::print_stats()
			{
				const std::string seperator = " || ";
				const std::string newline = "";
				std::string frame_stats;
				BOOST_LOG_NAMED_SCOPE("*** FRAME TO KEYFRAME ***");
				frame_stats += "Frames = " + std::to_string(_frames)
					+ seperator + "Key-frames = " + std::to_string(_keyframes)
					+ seperator + "Percent = " + std::to_string(percent(_keyframes, _frames))
					+ newline;

				std::string kp_stats;
				kp_stats += "Frames Key-points = " + std::to_string(_frame_keypoints)
					+ seperator + "Key-frames Key-points = " + std::to_string(_keyframe_keypoints)
					+ seperator + "Percent = "
					+ std::to_string(percent(_keyframe_keypoints, _frame_keypoints))
					+ newline;

				std::string semantic_kp_stats;
				semantic_kp_stats += "Frames Semantic Key-points = " + std::to_string(_frame_semantic_kp)
					+ seperator + "Key-frames Semantic Key-points = "
					+ std::to_string(_keyframe_semantic_kp)
					+ seperator + "Percent = "
					+ std::to_string(percent(_keyframe_semantic_kp, _frame_semantic_kp))
					+ newline;

				std::string mp_stats;
				mp_stats += "Map-points = " + std::to_string(_mp)
					+ seperator + "Semantic Map-points = " + std::to_string(_semantic_mp)
					+ seperator + "Percent = " + std::to_string(percent(_semantic_mp, _mp))
					+ newline;

				std::string kf_kp_mp_stats;
				kf_kp_mp_stats += "Key-Frames Key-points = " + std::to_string(_keyframe_keypoints)
					+ seperator + "Map-points = " + std::to_string(_mp)
					+ seperator + "Percent = " + std::to_string(percent(_mp, _keyframe_keypoints))
					+ newline;

				std::string kf_semantic_kp_mp_stats;
				kf_semantic_kp_mp_stats += "Key-Frames Semantic Key-points = "
					+ std::to_string(_keyframe_semantic_kp)
					+ seperator + "Semantic Map-points = " + std::to_string(_semantic_mp)
					+ seperator + "Percent = "
					+ std::to_string(percent(_semantic_mp, _keyframe_semantic_kp))
					+ newline;

				std::string map_kf_stats;
				map_kf_stats += "Key-Frames = " + std::to_string(_keyframes)
					+ seperator + "Map Key-frames = " + std::to_string(_map_kf_count)
					+ seperator + "Percent = "
					+ std::to_string(percent(_map_kf_count, _keyframes))
					+ newline;

				std::string map_kp_stats;
				map_kp_stats += "Key-frames Key-points = "
					+ std::to_string(_keyframe_keypoints)
					+ seperator + "Map Key-frames Key-points = " + std::to_string(_map_kp_count)
					+ seperator + "Percent = "
					+ std::to_string(percent(_map_kp_count, _keyframe_keypoints))
					+ newline;

				std::string map_semantic_kp_stats;
				map_semantic_kp_stats += "Key-frames Semantic Key-points = "
					+ std::to_string(_keyframe_semantic_kp)
					+ seperator + "Map Semantic Key-points = " + std::to_string(_map_semantic_kp_count)
					+ seperator + "Percent = "
					+ std::to_string(percent(_map_semantic_kp_count, _keyframe_semantic_kp))
					+ newline;

				std::string map_mp_stats;
				map_mp_stats += "Map-points = " + std::to_string(_mp)
					+ seperator + "Map Map-points = " + std::to_string(_map_mp_count)
					+ seperator + "Percent = "
					+ std::to_string(percent(_map_mp_count, _mp))
					+ newline;

				std::string map_semantic_kp_mp_stats;
				map_semantic_kp_mp_stats += "Map Semantic Key-points = "
					+ std::to_string(_map_semantic_kp_count)
					+ seperator + "Map Semantic Map-points = "
					+ std::to_string(_map_semantic_mps) + seperator
					+ std::to_string(percent(_map_semantic_mps, _map_semantic_kp_count))
					+ newline;

				print_border(frame_stats);
				print_border(kp_stats);
				print_border(semantic_kp_stats);
				print_border(mp_stats);
				print_border(kf_kp_mp_stats);
				print_border(kf_semantic_kp_mp_stats);

				print_border(map_kf_stats);
				print_border(map_semantic_kp_mp_stats);
				//print_border("*** Debug Statistics ***", frame_stats);
				//BOOST_LOG_NAMED_SCOPE("*** Debug Statistics ***");
				//BOOST_LOG_SEV(glog, trace) << "\n*** Debug Statistics ***\n";
				//BOOST_LOG_SEV(glog, info) << frame_stats;
				//BOOST_LOG_SEV(glog, info) << kp_stats;
				//BOOST_LOG_SEV(glog, info) << semantic_kp_stats;
				//BOOST_LOG_SEV(glog, info) << mp_stats;
				//BOOST_LOG_SEV(glog, info) << kf_kp_mp_stats;
				//BOOST_LOG_SEV(glog, info) << kf_semantic_kp_mp_stats;

				//BOOST_LOG_NAMED_SCOPE("*** Point Cloud Statistics ***");
				//BOOST_LOG_SEV(glog, trace) << "\n*** Point Cloud Statistics ***\n";
				//BOOST_LOG_SEV(glog, info) << map_kf_stats;
				//BOOST_LOG_SEV(glog, info) << map_kp_stats;
				//BOOST_LOG_SEV(glog, info) << map_mp_stats;
				//BOOST_LOG_SEV(glog, info) << map_semantic_kp_mp_stats;
			}

			double statistics::percent(
				const std::atomic<std::size_t>& nr, const std::atomic<std::size_t>& dr)
			{
				if (dr == 0)
					return 0.0;
				return (100.0 * nr) / dr;
			}

			double statistics::percent_diff(
				const std::atomic<std::size_t>& orignal, const std::atomic<std::size_t>& increase)
			{
				if (orignal == 0)
					return 0.0;
				return (100.0 * (increase- orignal)) / orignal;
			}

			void statistics::update_tracking_status(int imageid,string TrackingStateArr, int frame_org_keypoints, int frame_undist_keypoints, int frame_mappoints,int total_keyframes, int total_mappoints,bool isKfcreated)
			{
				const std::string seperator = " || ";
				const std::string newline = "";
				string bool_str[] = { "false","true" };
				std::string frame_stats;
				BOOST_LOG(glog) << "=========================================================================================================================================";
				BOOST_LOG(glog) << "*** FRAME = KEYPOINTS AND MAP ***";
				BOOST_LOG(glog) << "=========================================================================================================================================";
				BOOST_LOG(glog) << "FrameId(ImageNumber) = " << std::to_string(imageid);
				BOOST_LOG(glog) << "Tracking state = " << TrackingStateArr;
				BOOST_LOG(glog) << "current frame org Key-points = " << std::to_string(frame_org_keypoints);
				BOOST_LOG(glog) << "current frame undistorted Key-points = " << std::to_string(frame_undist_keypoints);
				BOOST_LOG(glog) << "current frame Map-points = " << std::to_string(frame_mappoints);
				BOOST_LOG(glog) << "Total keyframe = " << std::to_string(total_keyframes);
				BOOST_LOG(glog) << "Total Map-points = " << std::to_string(total_mappoints);
				BOOST_LOG(glog) << "key frame created from this frame = " << std::boolalpha << isKfcreated;		
				if(_prev_total_keyframes <= total_keyframes)
					BOOST_LOG(glog) << "increase in Key-points = " << std::to_string(std::abs(int(total_keyframes - _prev_total_keyframes)));
				else
					BOOST_LOG(glog) << "decrease in Key-points = " << std::to_string(std::abs(int(total_keyframes - _prev_total_keyframes)));
				if (_prev_total_mappoints < total_mappoints)
					BOOST_LOG(glog) << "increase in map-points = " << std::to_string(std::abs(int(total_mappoints - _prev_total_mappoints)));
				else
					BOOST_LOG(glog) << "decrease in map-points = " << std::to_string(std::abs(int(total_mappoints - _prev_total_mappoints)));

				BOOST_LOG(glog) << "=========================================================================================================================================";
				_prev_total_keyframes = total_keyframes;
				_prev_total_mappoints = total_mappoints;
			}
			void statistics::update_orbslam_status(int total_keyframes, int total_mappoints,string frameIdlist)
			{
				BOOST_LOG(glog) << "=========================================================================================================================================";
				BOOST_LOG(glog) << "\n\n*** ORB_SLAM2 status ***";
				BOOST_LOG(glog) << "=========================================================================================================================================";
				BOOST_LOG(glog) << "Total keyframe = " << std::to_string(total_keyframes);
				BOOST_LOG(glog) << "Total Map-points = " << std::to_string(total_mappoints);
				if (false == frameIdlist.empty())
					BOOST_LOG(glog) << frameIdlist;

				BOOST_LOG(glog) << "=========================================================================================================================================";
			}
	}
}