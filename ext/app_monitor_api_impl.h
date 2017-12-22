#pragma once

#include <boost/noncopyable.hpp>
#include "ext/app_monitor_api.h"
#include "ext/app_monitor.h"

namespace ORB_SLAM2 {
    namespace ext {

        class app_monitor_impl : public app_monitor_api, boost::noncopyable {
            app_monitor _monitor;
            void play() override
            {
                _monitor.send(system_event::play);
            }

            void stop() override
            {
                _monitor.send(system_event::stop);
            }

            void pause() override
            {
                _monitor.send(system_event::pause);
            }

            void request_wait() override
            {
                _monitor.request_wait();
            }

            bool is_playing() const override
            {
                return _monitor.is_state(state::playing);
            }
            
            bool is_stopped() const override 
            {
                return _monitor.is_state(state::stopped);
            }

            bool is_paused() const override
            {
                return _monitor.is_state(state::paused);
            }
        };
    }
}
