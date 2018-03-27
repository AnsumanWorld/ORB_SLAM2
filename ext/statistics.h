#pragma once

#include <atomic>
#include <vector>
#include <opencv2/core/types.hpp>

#include "Map.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
using namespace logging::trivial;

namespace ORB_SLAM2
{
    namespace ext {
		class statistics
		{
			src::severity_logger< severity_level > glog;
		public:
			// currently using singleton
			// todo: use dependency injection in users of this class
			static statistics& get();
			statistics();

			void update_frame_count(const std::size_t&);
			void update_keyframe_count(const std::size_t&);

			void update_frame_keypoint_stats(const std::vector<cv::KeyPoint>&);
			void update_keyframe_keypoint_stats(const std::vector<cv::KeyPoint>&);

			void update_mappoint_count(const std::size_t&);
			void update_semantic_mappoint_count(const std::size_t&);

			void update_map_stats(Map*);
			void print_stats();
			void print_border(string msg);
			void update_tracking_status(int imageid,string TrackingStateArr, int tracking_org_keypoints, int tracking_undist_keypoints, int tracking_mappoints, int total_keyframes, int total_mappoints,bool isKfcreated);
			void update_orbslam_status(int total_keyframes, int total_mappoints,string frameIdlist);
		private:
			double percent(const std::atomic<std::size_t>&,
				const std::atomic<std::size_t>&);
			double percent_diff(const std::atomic<std::size_t>&orignal,
				const std::atomic<std::size_t>&increase);

			std::atomic<std::size_t> _frames;
			std::atomic<std::size_t> _keyframes;

			std::atomic<std::size_t> _frame_keypoints;
			std::atomic<std::size_t> _keyframe_keypoints;

			std::atomic<std::size_t> _frame_semantic_kp;
			std::atomic<std::size_t> _keyframe_semantic_kp;

			std::atomic<std::size_t> _mp;
			std::atomic<std::size_t> _semantic_mp;

			std::atomic<std::size_t> _map_mp_count;
			std::atomic<std::size_t> _map_kf_count;

			std::atomic<std::size_t> _map_kp_count;
			std::atomic<std::size_t> _map_semantic_kp_count;
			std::atomic<std::size_t> _map_semantic_mps;

			std::atomic<std::size_t> _prev_total_mappoints;
			std::atomic<std::size_t> _prev_total_keyframes;
		};
	}
} //namespace ORB_SLAM2