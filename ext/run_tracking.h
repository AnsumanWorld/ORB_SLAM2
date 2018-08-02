#pragma once

#include "ext/slam_object.h"
#include <boost/range/iterator_range.hpp>
#include <chrono>

namespace ORB_SLAM2 {
    namespace ext {
        namespace detail {
            // Ensure that we wait for at least 
            void wait_for(double wait_time_)
            {
                if (wait_time_ > 0.) {
                    int wait_time = wait_time_;
                    std::this_thread::sleep_for(std::chrono::microseconds(wait_time));
                }
            }
        }

        template<class TDataSourceItr, typename... TArgs>
        auto make_data_source(TArgs&&... args_)
        {
            return boost::make_iterator_range(TDataSourceItr{ std::forward<TArgs>(args_)... }, TDataSourceItr{});
        }

        // The iterator variant:
        template<class TDataSourceItr>
        void run_tracking(slam_object& sys_, TDataSourceItr beg_, TDataSourceItr end_, double max_fps_)
        {
            double prev_expected_tracking_time = 0;

            for (; beg_ != end_;  ++beg_) {

                sys_.request_wait();
                std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

                sys_.track(*beg_);

                std::chrono::steady_clock::time_point stop_time = std::chrono::steady_clock::now();
                double actual_tracking_time = std::chrono::duration_cast<std::chrono::duration<double> >(stop_time - start_time).count();
                detail::wait_for(1.0 / max_fps_ - actual_tracking_time);
            }
        }

        // The Range variant:
        template<class TDataSource>
        void run_tracking(slam_object& sys_, TDataSource data_source_, double max_fps_)
        {
            run_tracking(sys_, std::begin(data_source_), std::end(data_source_), max_fps_);
        }
    }
}