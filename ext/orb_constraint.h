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
		class keyframe_constraint
		{
		public:
			keyframe_constraint(std::string &strSettingPath);
			bool keyframe_filter(LocalMapping &LocalMapping_obj, bool &start_local_ba);
			bool skip_keyframe(KeyFrame *current_kf);
		private:
			int _localba_interval;
			int _semantic;
		};
	}
} //namespace ORB_SLAM2