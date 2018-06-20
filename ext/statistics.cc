#include "statistics.h"
#include <iostream>
#include <string>
#include <boost/format.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

namespace ORB_SLAM2
{
	namespace ext 
	{
		class stat_collector :
			public boost::log::sinks::basic_sink_backend<
			boost::log::sinks::combine_requirements<
			boost::log::sinks::synchronized_feeding,                                        /*< we will have to store internal data, so let's require frontend to synchronize feeding calls to the backend >*/
			boost::log::sinks::flushing                                                     /*< also enable flushing support >*/
			>::type
			>
		{
		private:
			// The file to write the collected information to
			std::ofstream m_csv_file;
		public:
			// The constructor initializes the internal data
		        stat_collector(std::string &file_name) :m_csv_file(file_name, std::ios::out)
			{
				if (!m_csv_file.is_open()) throw std::runtime_error("could not open the CSV file");
			}

			// The function consumes the log records that come from the frontend
			void consume(boost::log::record_view const& rec)
			{
				m_csv_file << rec[boost::log::expressions::smessage];
			}
			// The function flushes the file
			void flush()
			{
				m_csv_file.flush();
			}
			~stat_collector()
			{
				flush();
				m_csv_file.close();
			}
			typedef boost::log::sinks::synchronous_sink< stat_collector > sink_t;
			static void init_logging(std::string filename)
			{
				boost::shared_ptr< boost::log::core > core = boost::log::core::get();
				boost::shared_ptr< stat_collector > backend(new stat_collector(filename));
				boost::shared_ptr< sink_t > sink(new sink_t(backend));
				core->add_sink(sink);
			}
		};

		statistics& statistics::get()
		{
			static statistics instance;
			return instance;
		}
		statistics::statistics()
		{
			stat_collector::init_logging("orbslam.csv");
		}

		void statistics::reject_frame_for_lmt_busy()
		{
			_reject_frame_for_lmt_busy++;
		}

		void statistics::reject_frame_by_tracking()
		{
			_reject_frame_by_tracking++;
		}

		void statistics::reject_lkf_by_keyframeculling()
		{
			_reject_lkf_by_keyframeculling++;
		}

		void statistics::add_semantic_lmp()
		{
			_semantic_lmp++;
		}

		void statistics::add_semantic_frame()
		{
			_semantic_frame++;
		}
		void statistics::add_semantic_lkf()
		{
			_semantic_lkf++;
		}
		void statistics::rejected_semantic_lkf_by_keyframeculling()
		{
			_rejected_semantic_lkf++;
		}
		void statistics::set_rejected_lmp()
		{
			_rejected_lmp++;
		}

		float statistics::cal_percent(int sub_amount , int total_amount)
		{
			return (float(sub_amount * 100) / total_amount);
		}

		void statistics::write_col_list(std::vector <std::string > &column_list_str)
		{
			boost::log::sources::logger lg;
			for (auto item : column_list_str)
			{
				BOOST_LOG(lg) << item << ",";
			}
			BOOST_LOG(lg) << std::endl;
		}

		void statistics::write_row(const std::string  row_str)
		{
			boost::log::sources::logger lg;
			BOOST_LOG(lg) << row_str << std::endl;
		}

		void statistics::log_map_state(Map* mpMap)
		{
			vector<KeyFrame*> vec_kf = mpMap->GetAllKeyFrames();
			_global_keyframes = vec_kf.size();
			vector<MapPoint*> vec_mappoints = mpMap->GetAllMapPoints();
			_global_map_points = vec_mappoints.size();

			for (auto map_point : vec_mappoints)
			{
				if (map_point)
				{
					if (map_point->is_semantic())
						_semantic_gmp++;
				}
			}
		}

		void statistics::dump()
		{
			int total_frames = Frame::nNextId;
			int local_keyframes = KeyFrame::nNextId;
			int local_map_points = MapPoint::nNextId;
			
			write_row("orb-slam2 info");
			std::vector <std::string > column_list_str = { "Total frames" , "Total lkf (% Total frames)" , "Total gkf (% Total lkf)" , "Total lmp","Total gmp(% Total lmp)" };
			write_col_list(column_list_str);

			column_list_str.clear();
			column_list_str.push_back(std::to_string(total_frames));
			std::string str = (boost::format("%1%(%2%)") % local_keyframes % cal_percent(local_keyframes, total_frames)).str();
			column_list_str.push_back(str);
			str = (boost::format("%1%(%2%)") % _global_keyframes % cal_percent(_global_keyframes, local_keyframes)).str();
			column_list_str.push_back(str);
			column_list_str.push_back(std::to_string(local_map_points));
			str = (boost::format("%1%(%2%)") % _global_map_points % cal_percent(_global_map_points, local_map_points)).str();
			column_list_str.push_back(str);
			write_col_list(column_list_str);

			write_row("\nRejected frames in tracking thread");
			std::vector <std::string > column_list_str1 = { "rejected frames(% Total frames)" , "rejected frames for lmt busy(% Total frames)" , "rejected frames without lmt busy(% Total frames)" , "rejected frames by initialization(% Total frames)" };
			write_col_list(column_list_str1);
			column_list_str1.clear();

			str = (boost::format("%1%(%2%)") % _reject_frame_by_tracking % cal_percent(_reject_frame_by_tracking, total_frames)).str();
			column_list_str1.push_back(str);
			str = (boost::format("%1%(%2%)") % _reject_frame_for_lmt_busy % cal_percent(_reject_frame_for_lmt_busy, total_frames)).str();
			column_list_str1.push_back(str);
			int rejected_frame_without_lm_busy = _reject_frame_by_tracking - _reject_frame_for_lmt_busy;
			str = (boost::format("%1%(%2%)") % rejected_frame_without_lm_busy % cal_percent(rejected_frame_without_lm_busy, total_frames)).str();
			column_list_str1.push_back(str);
			int reject_kf_by_initialization = (total_frames - local_keyframes - _reject_frame_by_tracking);
			str = (boost::format("%1%(%2%)") % reject_kf_by_initialization % cal_percent(reject_kf_by_initialization, total_frames)).str();
			column_list_str1.push_back(str);
			write_col_list(column_list_str1);

			write_row("\nRejected lkf and lmp in lmt");
			std::vector <std::string > column_list_str2 = { "rejected lkf by KeyFrameCulling()(% lkf)" ,  "rejected lmp by MapPointCulling()(% lmp)" };
			write_col_list(column_list_str2);
			column_list_str2.clear();
			str = (boost::format("%1%(%2%)") % _reject_lkf_by_keyframeculling % cal_percent(_reject_lkf_by_keyframeculling, local_keyframes)).str();
			column_list_str2.push_back(str);
			str = (boost::format("%1%(%2%)") % _rejected_lmp % cal_percent(_rejected_lmp, local_map_points)).str();
			column_list_str2.push_back(str);
			write_col_list(column_list_str2);

			if (_semantic_frame)
			{
				write_row("\nsemantic info");
				std::vector <std::string > column_list_str3 = { "Total semantic frames(% Total frames)" , "Total semantic lkf (% Total semantic frames)" , "Total semantic gkf (% Total semantic lkf)" , "Total semantic lmp","Total semantic gmp(% Total semantic lmp)" };
				write_col_list(column_list_str3);
				column_list_str3.clear();
				str = (boost::format("%1%(%2%)") % _semantic_frame % cal_percent(_semantic_frame, total_frames)).str();
				column_list_str3.push_back(str);
				str = (boost::format("%1%(%2%)") % _semantic_lkf % cal_percent(_semantic_lkf, _semantic_frame)).str();
				column_list_str3.push_back(str);
				int semantic_kf = (_semantic_lkf - _rejected_semantic_lkf);
				str = (boost::format("%1%(%2%)") % semantic_kf % cal_percent(semantic_kf, _semantic_lkf)).str();
				column_list_str3.push_back(str);
				column_list_str3.push_back(std::to_string(_semantic_lmp));
				str = (boost::format("%1%(%2%)") % _semantic_gmp % cal_percent(_semantic_gmp, _semantic_lmp)).str();
				column_list_str3.push_back(str);
				write_col_list(column_list_str3);

				write_row("\nRejected semanitc lkf and lmp in lmt");
				std::vector <std::string > column_list_str4 = {"rejected semantic lkf by KeyFrameCulling()(% lkf)" , "rejected semantic lmp by MapPointCulling()(% semantic lmp)" };
				write_col_list(column_list_str4);
				column_list_str4.clear();
				str = (boost::format("%1%(%2%)") % _rejected_semantic_lkf % cal_percent(_rejected_semantic_lkf, _semantic_lkf)).str();
				column_list_str4.push_back(str);
				int rejected_semantic_map_poitnts = _semantic_lmp - _semantic_gmp;
				str = (boost::format("%1%(%2%)") % rejected_semantic_map_poitnts % cal_percent(rejected_semantic_map_poitnts, _semantic_lmp)).str();
				column_list_str4.push_back(str);
				write_col_list(column_list_str4);
			}

			write_row("\n\nNOTES:");
			write_row("lkf = local key-frame(total key-frame created in orb-slam2)");
			write_row("gkf = global key-frame(local key-frame - rendundant local key-frame)");			
			write_row("lmp = local map-point(total map-points created in orb-slam2");
			write_row("gmp = global map-point(optimized local map-point");
			write_row("lmt = local mapping thread");
		}
	}
}
