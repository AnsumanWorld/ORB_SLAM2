#pragma once

#include "ext/slam_object.h"
#include <boost/range/iterator_range.hpp>
#include <chrono>

namespace ORB_SLAM2 {
    namespace ext {

        // Ensure that we wait for at least there is next tracking wont be called before its corresponding frame timestamp elapsed
        void throttle(double actual_track_time, double expected_track_time)
        {
            if (actual_track_time < expected_track_time)
            {
                int wait_time = expected_track_time - actual_track_time;
                std::this_thread::sleep_for(std::chrono::microseconds(wait_time));
            }
        }

        template<class TDataSourceItr, typename... TArgs>
        auto make_data_source(TArgs&&... args_)
        {
            return boost::make_iterator_range(TDataSourceItr{ std::forward<TArgs>(args_)... }, TDataSourceItr{});
        }

        //The iterator pair variant:
        template<class TDataSourceItr>
        void run_tracking(slam_object& sys_, TDataSourceItr beg_, TDataSourceItr end_, double max_fps_)
        {
            double prev_expected_tracking_time = 0;

            for (auto const& item : std::make_pair(beg_, end_)) {
                sys_.request_wait();
                std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

                sys_.track(item);

                std::chrono::steady_clock::time_point stop_time = std::chrono::steady_clock::now();
                double actual_tracking_time = std::chrono::duration_cast<std::chrono::duration<double> >(stop_time - start_time).count();
                throttle(actual_tracking_time, 1.0 / max_fps_);
            }
        }

        //The Range variant:
        template<class TDataSource>
        void run_tracking(slam_object& sys_, TDataSource data_source_,double max_fps_)
        {
            for (auto &item : data_source_) {
                sys_.request_wait();
                std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

                sys_.track(item);

                std::chrono::steady_clock::time_point stop_time = std::chrono::steady_clock::now();
                double actual_tracking_time = std::chrono::duration_cast<std::chrono::duration<double> >(stop_time - start_time).count();	
                throttle(actual_tracking_time, 1.0/ max_fps_);
            }
        }
    }
}