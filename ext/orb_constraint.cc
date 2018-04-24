#include "System.h"
#include "orb_constraint.h"
#include "Optimizer.h"

namespace ORB_SLAM2
{
	namespace ext {
		ext_local_mapping::ext_local_mapping(LocalMapping *local_mapping_ptr_, const std::string &strSettingPath) :
			_local_mapping_ptr(local_mapping_ptr_), _localba_interval(3), _semantic(0)
		{
			cv::FileStorage fSettings(strSettingPath, cv::FileStorage::READ);
			_localba_interval = fSettings["keyframe.localba_interval"];
			_semantic = fSettings["keyframe.semantic"];
		}

		bool ext_local_mapping::skip_keyframe(KeyFrame *current_kf)
		{
			bool skip_keyframe_status = true;
			if (current_kf)
			{
				//key frame contain semantic information
				if (_semantic && std::get<ext::tsr_info_opt_t>(current_kf->mCurFrame._sensor_input))
					skip_keyframe_status = true;
			}

			return skip_keyframe_status;
		}

		bool ext_local_mapping::keyframe_filter(bool &start_local_ba)
		{
			static int kf_count = 0;
			bool skip_keyframe_status = false;
			start_local_ba = false;
			do {
				if (_local_mapping_ptr->CheckNewKeyFrames())
				{
					_local_mapping_ptr->ProcessNewKeyFrame();
					skip_keyframe_status = skip_keyframe(_local_mapping_ptr->mpCurrentKeyFrame);
				}
			} while (false == skip_keyframe_status && _local_mapping_ptr->CheckNewKeyFrames());
			if (false == skip_keyframe_status)
				kf_count++;
			if (kf_count == _localba_interval)
			{
				kf_count = 0;
				start_local_ba = true;
			}

			return skip_keyframe_status;
		}

		bool ext_local_mapping::check_local_maping_status()
		{
			bool status = true;
			if (_local_mapping_ptr->Stop())
			{
				// Safe area to stop
				while (_local_mapping_ptr->isStopped() && !_local_mapping_ptr->CheckFinish())
				{
					// usleep(3000);
					std::this_thread::sleep_for(std::chrono::milliseconds(3));
				}
				if (_local_mapping_ptr->CheckFinish())
					status = false;
			}
			_local_mapping_ptr->ResetIfRequested();

			if (_local_mapping_ptr->CheckFinish())
				status = false;
			return status;
		}
		void ext_local_mapping::ext_run()
		{
			_local_mapping_ptr->mbFinished = false;
			bool start_local_ba = false;
			while (1)
			{
				if (keyframe_filter(start_local_ba))
				{
					// Check recent MapPoints
					_local_mapping_ptr->MapPointCulling();

					// Triangulate new MapPoints
					_local_mapping_ptr->CreateNewMapPoints();

					// Find more matches in neighbor keyframes and fuse point duplications
					if (start_local_ba)
						_local_mapping_ptr->SearchInNeighbors();

					_local_mapping_ptr->mbAbortBA = false;

					if (start_local_ba && !_local_mapping_ptr->stopRequested())
					{
						// Local BA
						if (_local_mapping_ptr->mpMap->KeyFramesInMap() > 2)
							Optimizer::LocalBundleAdjustment(_local_mapping_ptr->mpCurrentKeyFrame, &_local_mapping_ptr->mbAbortBA, _local_mapping_ptr->mpMap);

						// Check redundant local Keyframes
						_local_mapping_ptr->KeyFrameCulling();
					}

					_local_mapping_ptr->mpLoopCloser->InsertKeyFrame(_local_mapping_ptr->mpCurrentKeyFrame);
					if (false == check_local_maping_status())
						break;
				}
			}
			_local_mapping_ptr->SetFinish();
		}
	}
}