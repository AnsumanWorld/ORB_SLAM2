#pragma once

//#include "System.h"
#include <boost/function.hpp>
#include <iostream>
#include <vector>

namespace ORB_SLAM2
{
	class LocalMapping;
	class KeyFrame;
    namespace ext {
		class ext_local_mapping
		{
		public:
			ext_local_mapping(LocalMapping *local_mapping_ptr_,const std::string &strSettingPath);
			bool keyframe_filter( bool &start_local_ba);
			bool skip_keyframe(KeyFrame *current_kf);
			bool check_local_maping_status();
			void ext_run();
		private:
			int _localba_interval;
			int _semantic;
			LocalMapping *_local_mapping_ptr;
		};
	}
} //namespace ORB_SLAM2