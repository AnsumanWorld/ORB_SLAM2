#pragma once

#include "System.h"
#include "ext/app_monitor_api_impl.h"


namespace ORB_SLAM2 {
    namespace ext {
        class slam_object {
			ORB_SLAM2::ext::app_monitor_api* _app_monitor;
			ORB_SLAM2::ext::app_monitor_impl app_monitor_inst;
            ORB_SLAM2::System _slam;

            void shutdown()
            {
                // Stop all threads
                _slam.Shutdown();

                // Save camera trajectory
                _slam.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");
            }
        public:
            ORB_SLAM2::System& get()
            {
                return _slam;
            }

            ORB_SLAM2::System const& get() const
            {
                return _slam;
            }

            void request_wait()
            {
                _app_monitor->request_wait();
            }
            void track(std::tuple<ext::time_point_t, ext::image_t, ext::tsr_info_opt_t, ext::pos_info_opt_t> const& item)
            {
				_slam.TrackMonocular(item);
            }

			slam_object(int argc, char** argv)
                : _app_monitor(&app_monitor_inst),
				_slam{
					argv[1],
					argv[2],
                    ORB_SLAM2::System::MONOCULAR,
					true,
					_app_monitor }
            {}

            ~slam_object()
            {
                shutdown();
            }
        };
    }
}