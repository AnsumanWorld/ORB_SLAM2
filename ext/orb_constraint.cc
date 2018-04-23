#include "System.h"
#include "orb_constraint.h"


namespace ORB_SLAM2
{
	namespace ext {
		keyframe_constraint::keyframe_constraint(std::string &strSettingPath):_localba_interval(3), _semantic(0)
		{
			cv::FileStorage fSettings(strSettingPath, cv::FileStorage::READ);
			_localba_interval = fSettings["keyframe.localba_interval"];
			_semantic = fSettings["keyframe.semantic"];
		}
		
		bool keyframe_constraint::skip_keyframe(KeyFrame *current_kf)
		{
			bool skip_keyframe_status = true;
			if (current_kf)
			{
				//key frame contain semantic information
				if(_semantic && std::get<ext::tsr_info_opt_t>(current_kf->mCurFrame._sensor_input))
					skip_keyframe_status = true;
			}
				
			return skip_keyframe_status;
		}

		bool keyframe_constraint::keyframe_filter(LocalMapping &LocalMapping_obj,bool &start_local_ba)
		{
			static int kf_count = 0;
			bool skip_keyframe_status = false;		
			start_local_ba = false;
			do{
				if (LocalMapping_obj.CheckNewKeyFrames())
				{
					LocalMapping_obj.ProcessNewKeyFrame();
					skip_keyframe_status = skip_keyframe(LocalMapping_obj.mpCurrentKeyFrame);
				}
			} while (false == skip_keyframe_status && LocalMapping_obj.CheckNewKeyFrames());
			if (false == skip_keyframe_status)
				kf_count++;
			if (kf_count == _localba_interval)
			{
				kf_count = 0;
				start_local_ba = true;
			}
				
			return skip_keyframe_status;
		}

	}
}