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
			src::severity_logger< severity_level > _glog;
		public:
			static statistics& get();
			statistics();
			void statistics::update_status(int keyframes, int mappoints, int semantic_mappoints);
			float cal_percent(int sub_amount, int total_amount);
			void reject_kf_for_busy();
			void reject_kf_by_tracking();
			void reject_kf_by_keyframeculling();
			void add_semantic_mappoint();
			void set_rejected_mappoints();
			void set_title(std::string title);
		private:
			int _reject_kf_by_tracking_for_busy_count;
			int _reject_kf_by_tracking_count;
			int _reject_kf_by_keyframeculling_count;
			int _local_semantic_map_points;
			int _rejected_mappoints;
		};
	}
} //namespace ORB_SLAM2