#pragma once

#include "ext/slam_object.h"
#include <chrono>

namespace ORB_SLAM2 {
    namespace ext {

        //it will ensure that next tracking wont be called before its corresponding frame timestamp elapsed
        void wait(double actual_tracking_time, double cur_expected_tracking_time, double default_wait_time)
        {
            int wait_time = 0;

            if (!cur_expected_tracking_time)
                cur_expected_tracking_time = default_wait_time;

            //if vehicle drive faster, there is a chance of low connections among frames in terms of keypoints
            if (actual_tracking_time < cur_expected_tracking_time)
            {
                wait_time = (cur_expected_tracking_time - actual_tracking_time) * default_wait_time;
                std::this_thread::sleep_for(std::chrono::microseconds(wait_time));
            }
        }

        //The iterator pair variant:
        template<class TDataSourceItr>
        void run_tracking(slam_object& sys_, TDataSourceItr beg_, TDataSourceItr end_, double default_wait_time = 0)
        {
            double prev_expected_tracking_time = 0;

            for (auto const& item : std::make_pair(beg_, end_)) {
                sys_.request_wait();
                std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

                sys_.track(item);

                std::chrono::steady_clock::time_point stop_time = std::chrono::steady_clock::now();
                double actual_tracking_time = std::chrono::duration_cast<std::chrono::duration<double> >(stop_time - start_time).count();
                wait(actual_tracking_time, std::get<ext::time_point_t>(item));
            }
        }

        //The Range variant:
        template<class TDataSource>
        void run_tracking(slam_object& sys_, TDataSource data_source_,double default_wait_time= 0)
        {
            for (auto &item : data_source_) {
                sys_.request_wait();
                std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

                sys_.track(item);

                std::chrono::steady_clock::time_point stop_time = std::chrono::steady_clock::now();
                double actual_tracking_time = std::chrono::duration_cast<std::chrono::duration<double> >(stop_time - start_time).count();				
                wait(actual_tracking_time, std::get<ext::time_point_t>(item), default_wait_time);
            }
        }
    }
}