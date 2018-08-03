#pragma once

#include "ext/slam_object.h"
#include <boost/range/iterator_range.hpp>
#include <chrono>
#include <cassert>

namespace ORB_SLAM2 {
    namespace ext {
        namespace detail {

            /// <summary>Ensure we do not exceed given frame rate.</summary>
            /// <param name="duration_">Duration of current operation</param>
            /// <param name="min_duration_">Minimum duration = reciprocal value of max. frames per second</param>
            template<class T, class U>
            void wait_if_fps_exceeded(T const& duration_, U const& min_duration_)
            {
                if (duration_ < min_duration_)
                    std::this_thread::sleep_for(min_duration_ - duration_);
            }
        }

        /// <summary>Create a data source range of given type.</summary>
        template<class TDataSourceItr, typename... TArgs>
        auto make_data_source(TArgs&&... args_)
        {
            return boost::make_iterator_range(
                TDataSourceItr{ std::forward<TArgs>(args_)... }, 
                TDataSourceItr{});
        }

        /// <summary>Run tracking helper function. Will pull data from given range until the end is reached.</summary>
        /// <param name='sys_'>The initialized SLAM system object</param>  
        /// <param name='beg_'>Begin of the data source iterator</param>  
        /// <param name='end_'>End of the data source iterator</param>  
        /// <param name='max_fps_'>Maximum frame rate allowed in the tracking thread</param>
        template<class TDataSourceItr>
        void run_tracking(slam_object& sys_, TDataSourceItr beg_, TDataSourceItr end_, int max_fps_)
        {
            assert(max_fps_ > 0);
            using clock_t = std::chrono::steady_clock;

            const auto c_min_duration = 
                std::chrono::duration_cast<clock_t::duration>(std::chrono::duration<double>{1. / max_fps_});

            for (; beg_ != end_;  ++beg_) {
                sys_.request_wait();

                auto start_time = clock_t::now();
                sys_.track(*beg_);
                auto duration = clock_t::now() - start_time;

                detail::wait_if_fps_exceeded(duration, c_min_duration);
            }
        }

        /// <summary>Run tracking helper function. Will pull data from given range until the end is reached.</summary>
        /// <param name='sys_'>The initialized SLAM system object</param>  
        /// <param name='data_source_'>The data source range object</param>  
        /// <param name='max_fps_'>Maximum frame rate allowed in the tracking thread</param>
        template<class TDataSource>
        void run_tracking(slam_object& sys_, TDataSource data_source_, int max_fps_)
        {
            run_tracking(sys_, std::begin(data_source_), std::end(data_source_), max_fps_);
        }
    }
}