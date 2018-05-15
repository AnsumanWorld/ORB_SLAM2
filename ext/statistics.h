#pragma once

#include <atomic>
#include <vector>
#include <opencv2/core/types.hpp>

#include "Map.h"
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <iostream>
#include <string>

namespace ORB_SLAM2
{
	namespace ext {

		namespace logging = boost::log;
		namespace src = boost::log::sources;
		namespace expr = boost::log::expressions;
		namespace sinks = boost::log::sinks;

		class stat_collector :
			public sinks::basic_sink_backend<
			sinks::combine_requirements<
			sinks::synchronized_feeding,                                        /*< we will have to store internal data, so let's require frontend to synchronize feeding calls to the backend >*/
			sinks::flushing                                                     /*< also enable flushing support >*/
			>::type
			>
		{
		private:
			// The file to write the collected information to
			std::ofstream m_csv_file;
		public:
			// The constructor initializes the internal data
			explicit stat_collector(std::string &file_name):m_csv_file(file_name, std::ios::out)
			{ if (!m_csv_file.is_open()) throw std::runtime_error("could not open the CSV file"); }

			// The function consumes the log records that come from the frontend
			void consume(logging::record_view const& rec) { m_csv_file << rec[expr::smessage]; }
			// The function flushes the file
			void flush() { m_csv_file.flush(); }
			~stat_collector() { flush(); m_csv_file.close(); }
			// Complete sink type
			typedef sinks::synchronous_sink< stat_collector > sink_t;

			static void init_logging(std::string filename)
			{
				boost::shared_ptr< logging::core > core = logging::core::get();
				boost::shared_ptr< stat_collector > backend(new stat_collector(filename));
				boost::shared_ptr< sink_t > sink(new sink_t(backend));
				core->add_sink(sink);
			}
		};

		class statistics
		{
		public:
			static statistics& get();
			statistics();
			void update_status(int global_keyframes_, int global_map_points_, int semantic_global_map_points_);
			float cal_percent(int sub_amount, int total_amount);
			//lmt ->local mapping thread
			void reject_frame_for_lmt_busy();
			void reject_frame_by_tracking();
			//lkf ->local key frame
			void reject_lkf_by_keyframeculling();
			void rejected_semantic_lkf_by_keyframeculling();
			void add_semantic_lmp();
			void add_semantic_frame();
			void add_semantic_lkf();
			void set_rejected_lmp();
		private:
			int _reject_frame_for_lmt_busy{ 0 };
			int _reject_frame_by_tracking{ 0 };
			int _reject_lkf_by_keyframeculling{ 0 };
			int _semantic_lmp{ 0 };
			int _rejected_lmp{ 0 };
			int _semantic_frame{0};
			int _semantic_lkf{ 0 };
			int _rejected_semantic_lkf{ 0 };
		};
	}
} //namespace ORB_SLAM2