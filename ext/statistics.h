#pragma once

#include <vector>
#include <iostream>
#include <string>
#include "Map.h"

namespace ORB_SLAM2
{
	namespace ext {

		class statistics
		{
		public:
			static statistics& get();
			statistics();
			void dump();
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
			void log_map_state(Map* mpMap);
			void write_row(const std::string  row_str);
			void write_col_list(std::vector <std::string > &column_list_str);
		private:
			int _reject_frame_for_lmt_busy{ 0 };
			int _reject_frame_by_tracking{ 0 };
			int _reject_lkf_by_keyframeculling{ 0 };
			int _semantic_lmp{ 0 };
			int _rejected_lmp{ 0 };
			int _semantic_frame{0};
			int _semantic_lkf{ 0 };
			int _rejected_semantic_lkf{ 0 };
			int _global_keyframes{ 0 };
			int _global_map_points{ 0 };
			//_semantic_gmp ->semantic global map pints
			int _semantic_gmp{ 0 };
		};
	}
} //namespace ORB_SLAM2