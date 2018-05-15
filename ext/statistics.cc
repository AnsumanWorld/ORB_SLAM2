#include "statistics.h"
#include <iostream>
#include <string>
#include <boost/format.hpp>

namespace ORB_SLAM2
{
	namespace ext {
		statistics& statistics::get()
		{
			static statistics instance;
			return instance;
		}
		statistics::statistics()
		{
			stat_collector::init_logging("orbslam.csv");
		}

		void statistics ::reject_frame_for_lmt_busy()
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

		void write_col_list(std::vector <std::string > &column_list_str)
		{
			src::logger lg;
			for (auto item : column_list_str)
			{
				BOOST_LOG(lg) << item << ",";
			}
			BOOST_LOG(lg) << std::endl;
		}

		void write_row(const std::string  row_str)
		{
			src::logger lg;
			BOOST_LOG(lg) << row_str << std::endl;
		}

		void statistics::update_status(int global_keyframes_, int global_map_points_, int semantic_global_map_points_)
		{
			int total_frames = Frame::nNextId;
			int local_keyframes = KeyFrame::nNextId;
			int local_map_points = MapPoint::nNextId;
			
			src::logger lg;
			write_row("orb-slam2 info");
			std::vector <std::string > column_list_str = { "Total frames" , "Total L.K.F (% Total frames)" , "Total G.K.F (% Total L.K.F)" , "Total L.M.P","Total G.M.P(% Total L.M.P)" };
			write_col_list(column_list_str);

			column_list_str.clear();
			column_list_str.push_back(std::to_string(total_frames));
			std::string str = (boost::format("%1%(%2%)") % local_keyframes % cal_percent(local_keyframes, total_frames)).str();
			column_list_str.push_back(str);
			str = (boost::format("%1%(%2%)") % global_keyframes_ % cal_percent(global_keyframes_, local_keyframes)).str();
			column_list_str.push_back(str);
			column_list_str.push_back(std::to_string(local_map_points));
			str = (boost::format("%1%(%2%)") % global_map_points_ % cal_percent(global_map_points_, local_map_points)).str();
			column_list_str.push_back(str);
			write_col_list(column_list_str);

			write_row("\nRejected frames in tracking thread");
			std::vector <std::string > column_list_str1 = { "rejected frames(% Total frames)" , "rejected frames for L.M.T busy(% Total frames)" , "rejected frames without L.M.T busy(% Total frames)" , "rejected frames by initialization(% Total frames)" };
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

			write_row("\nRejected local key-frame and local map-point in Local Mapping thread");
			std::vector <std::string > column_list_str2 = { "rejected L.K.F by KeyFrameCulling()(% L.K.F)" ,  "rejected L.M.P by MapPointCulling()(% L.M.P)" };
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
				std::vector <std::string > column_list_str3 = { "Total semantic frames(% Total frames)" , "Total semantic L.K.F (% Total semantic frames)" , "Total semantic G.K.F (% Total semantic L.K.F)" , "Total semantic L.M.P","Total semantic G.M.P(% Total semantic L.M.P)" };
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
				str = (boost::format("%1%(%2%)") % semantic_global_map_points_ % cal_percent(semantic_global_map_points_, _semantic_lmp)).str();
				column_list_str3.push_back(str);
				write_col_list(column_list_str3);

				write_row("\nRejected semanitc local key-frame and local map-point in Local Mapping thread");
				std::vector <std::string > column_list_str4 = {"rejected semantic L.K.F by KeyFrameCulling()(% L.K.F)" , "rejected semantic L.M.P by MapPointCulling()(% semantic L.M.P)" };
				write_col_list(column_list_str4);
				column_list_str4.clear();
				str = (boost::format("%1%(%2%)") % _rejected_semantic_lkf % cal_percent(_rejected_semantic_lkf, _semantic_lkf)).str();
				column_list_str4.push_back(str);
				int rejected_semantic_map_poitnts = _semantic_lmp - semantic_global_map_points_;
				str = (boost::format("%1%(%2%)") % rejected_semantic_map_poitnts % cal_percent(rejected_semantic_map_poitnts, _semantic_lmp)).str();
				column_list_str4.push_back(str);
				write_col_list(column_list_str4);
			}

			write_row("\n\nNOTES:");
			write_row("L.K.F = local key-frame(total key-frame created in orb-slam2)");
			write_row("G K.F = global key-frame(local key-frame - rendundant local key-frame)");			
			write_row("L.M.P = local map-point(total map-points created in orb-slam2");
			write_row("G.M.P = global map-point(optimized local map-point");
			write_row("L.M.T = local mapping thread");
		}
	}
}