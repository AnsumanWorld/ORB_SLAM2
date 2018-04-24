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
			void update_orbslam_status(int total_frames, int total_keyframes, int total_mappoints, int total_semantic_mappoints);
			float cal_percent(int sub_amount, int total_amount);
			void reject_kf_by_tracking_for_busy();
			void reject_kf_by_tracking();
			void reject_kf_by_keyframeculling();
			void total_kf_by_tracking();
			int reject_kf_by_tracking_for_busy_count;
			int reject_kf_by_tracking_count;
			int reject_kf_by_keyframeculling_count;
			int total_kf_by_tracking_count;
		};
	}
} //namespace ORB_SLAM2