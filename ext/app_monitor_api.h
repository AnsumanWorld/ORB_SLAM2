#pragma once

#include <memory>

namespace ORB_SLAM2 {
    namespace ext {
        struct app_monitor_api {
            /// send event to application monitor 
            virtual void play() = 0;
            virtual void stop() = 0;
            virtual void pause() = 0;

            // Will try to wait on current thread
            virtual void request_wait() = 0;
        protected:
            ~app_monitor_api() {}
        };
    }
}