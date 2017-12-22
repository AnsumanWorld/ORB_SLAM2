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
                _monitor.play();
            }

            void stop() override
            {
                _monitor.stop();
            }

            void pause() override
            {
                _monitor.pause();
            }

            void request_wait() override
            {
                _monitor.request_wait();
            }
        };
    }
}
