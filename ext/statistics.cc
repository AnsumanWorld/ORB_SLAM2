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
		statistics::statistics():_reject_kf_by_tracking_for_busy_count(0),
			_reject_kf_by_tracking_count(0),
			_reject_kf_by_keyframeculling_count(0),
			_local_semantic_map_points(0),
			_rejected_mappoints(0)
		{
			logging::add_file_log
			(
				keywords::file_name = "orb_slam2_statistics.log"
			);
			logging::add_common_attributes();
		}

		void statistics ::reject_kf_for_busy()
		{
			_reject_kf_by_tracking_for_busy_count++;
		}

		void statistics::reject_kf_by_tracking()
		{
			_reject_kf_by_tracking_count++;
		}

		void statistics::reject_kf_by_keyframeculling()
		{
			_reject_kf_by_keyframeculling_count++;
		}

		void statistics::add_semantic_mappoint()
		{
			_local_semantic_map_points++;
		}

		void statistics::set_rejected_mappoints()
		{
			_rejected_mappoints ++;
		}

		float statistics::cal_percent(int sub_amount , int total_amount)
		{
			return (float(sub_amount * 100) / total_amount);
		}

		void statistics::set_title(std::string title)
		{
			BOOST_LOG(_glog) << "=========================================================================================================================================";
			BOOST_LOG(_glog) << "*** "<< title<<" ***";
			BOOST_LOG(_glog) << "=========================================================================================================================================";
		}

		void statistics::update_status(int keyframes_, int map_points_, int semantic_mappoints_)
		{
			int total_frames = Frame::nNextId;
			int local_keyframes = KeyFrame::nNextId;
			int local_map_points = MapPoint::nNextId;
			int reject_kf_by_monocular_initialization = (local_keyframes - _reject_kf_by_tracking_count - _reject_kf_by_keyframeculling_count);
			set_title("ORB_SLAM2 status");
			BOOST_LOG(_glog) << "Total frames                           = " << std::to_string(total_frames);
			BOOST_LOG(_glog) << "local key-frames                       = " << local_keyframes << "(" << cal_percent(local_keyframes, total_frames) << "%)";
			BOOST_LOG(_glog) << "keyframes                              = " << std::to_string(keyframes_) << "(" << cal_percent(keyframes_, total_frames) << ")%";
			BOOST_LOG(_glog) << "local Map-points                       = " << std::to_string(local_map_points);
			BOOST_LOG(_glog) << "Map points                             = " << std::to_string(map_points_) << "(" << cal_percent(map_points_, local_map_points) << "% of local Map-points)";
			BOOST_LOG(_glog) << "rejected key-frames                    = " << _reject_kf_by_tracking_count+ _reject_kf_by_keyframeculling_count+ reject_kf_by_monocular_initialization << "(" << cal_percent((_reject_kf_by_tracking_count + _reject_kf_by_keyframeculling_count + reject_kf_by_monocular_initialization), total_frames) << "% of local key-frame)";
			BOOST_LOG(_glog) << "rejected map-points(MapPointCulling()) = " << _rejected_mappoints << "(" << cal_percent((_rejected_mappoints), local_map_points) << "% of local map-points)";
			if (_local_semantic_map_points)
			{
				BOOST_LOG(_glog) << "local semantic Map-points              = " << std::to_string(_local_semantic_map_points) << "(" << cal_percent(_local_semantic_map_points, local_map_points) << "% of local Map-points)";
				BOOST_LOG(_glog) << "semantic Map-points                    = " << std::to_string(semantic_mappoints_) << "(" << cal_percent(semantic_mappoints_, _local_semantic_map_points) << "% local semantic Map-points)";
			}

			BOOST_LOG(_glog) << "=========================================================================================================================================";
			set_title("factors of key-frames rejection");
			BOOST_LOG(_glog) << "1. key-frames rejected in tracking thread(NeedNewKeyFrame())   =" << _reject_kf_by_tracking_count << "(" << cal_percent((_reject_kf_by_tracking_count), total_frames) << "% of local key-frame)";
			BOOST_LOG(_glog) << "   rejected key frame for local mapping busy                   = " << _reject_kf_by_tracking_for_busy_count <<"("<<cal_percent(_reject_kf_by_tracking_for_busy_count, total_frames)<<"% of local key-frame)";
			BOOST_LOG(_glog) << "   rejected key-frame without local mapping busy               = " << _reject_kf_by_tracking_count- _reject_kf_by_tracking_for_busy_count << "(" << cal_percent((_reject_kf_by_tracking_count- _reject_kf_by_tracking_for_busy_count), total_frames) << "% of local key-frame)";
			BOOST_LOG(_glog) << "=========================================================================================================================================";
			BOOST_LOG(_glog) << "2. rejected keyframe by localmapping thread(KeyFrameCulling()) =" << _reject_kf_by_keyframeculling_count << "(" << cal_percent(_reject_kf_by_keyframeculling_count, total_frames) << "%)";
			BOOST_LOG(_glog) << "=========================================================================================================================================";
			BOOST_LOG(_glog) << "3. rejected key-frame while initialization                     =" << reject_kf_by_monocular_initialization << "(" << cal_percent(reject_kf_by_monocular_initialization, total_frames) << "%)";
			BOOST_LOG(_glog) << "=========================================================================================================================================";
			BOOST_LOG(_glog) << "=========================================================================================================================================\n";
			set_title("NOTES:");
			BOOST_LOG(_glog) << "local key-frames     = " << "total keyframe created in orb-slam2";
			BOOST_LOG(_glog) << "key-frames           = " << "local keyframe - rendundant local keyframe";
			BOOST_LOG(_glog) << "local Map-points     = " << "total map-points created in orb-slam2";
			BOOST_LOG(_glog) << "Map-points           = " << "local map points - bad local map points";
			BOOST_LOG(_glog) << "bad local map points = " << "local map-points is either not visible(overlapped) or it has less observer ";
			BOOST_LOG(_glog) << "total rejected key-frames in tracking = " << "rejected key frame for local mapping busy" << " + " << "rejected key-frame without local mapping busy";
			BOOST_LOG(_glog) << "while initialization phase orb-slam2 create two keyframe with its id = 0. This is created from vocabulary";
			BOOST_LOG(_glog) << "=========================================================================================================================================";
		}
	}
}