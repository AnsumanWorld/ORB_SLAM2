#pragma once

#include <atomic>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <queue>
#include <iostream>

namespace ORB_SLAM2 {
    namespace ext {
        enum class state {
            playing,
            paused,
            stopped
        };

        enum class system_event {
            undefined,
            play,
            pause,
            stop
        };

        class app_monitor {
            mutable state _state;
            const std::thread::id _id;
            std::mutex _lock;
            std::condition_variable _cv;

            using handler_t = std::function<void(system_event)>;
            using map_element_t = std::pair<state, handler_t>;

            /// state transition functions
            map_element_t _state_processor[3] = {
                { 
                    state::paused, [&](system_event ev_) {
                        switch (ev_) {
                            case system_event::play:
                                _state = state::playing;
                                break;
                            case system_event::pause:
                                _state = state::paused;
                                break;
                            case system_event::stop:
                                _state = state::stopped;
                                break;
                            default:
                                // no other valid transitions
                                break;
                        }
                    }
                },
                {
                    state::playing, [&](system_event ev_) {
                        switch (ev_) {
                            case system_event::play:
                                _state = state::playing;
                                break;
                            case system_event::pause:
                                _state = state::paused;
                                break;
                            case system_event::stop:
                                _state = state::paused;
                                break;
                            default:
                                // no other valid transitions
                                break;
                        }
                    }
                },
                {
                    state::stopped, [&](system_event ev_) {
                        switch (ev_) {
                            case system_event::play:
                                _state = state::playing;
                                break;
                            case system_event::pause:
                                _state = state::paused;
                                break;
                            case system_event::stop:
                                _state = state::stopped;
                                break;
                            default:
                                // no other valid transitions
                                break;
                        }
                    }
                }
            };

            void throw_if_on_wrong_thread() const
            {
                if (std::this_thread::get_id() != _id) {
                    throw std::runtime_error("Wrong thread access!");
                }
            }

            handler_t const& get_state_handler() const
            {
                auto handler = std::find_if(
                    std::begin(_state_processor),
                    std::end(_state_processor),
                    [&](auto const& elem_) {
                        return elem_.first == _state;
                    });
                if (handler == std::end(_state_processor)) {
                    throw std::runtime_error("Illegal state!");
                }

                return handler->second;
            }

            void handle_event(system_event ev_)
            {
                {
                    std::unique_lock<std::mutex> lock{_lock};
                    // handle event, perform state transition if any
                    get_state_handler()(ev_);
                }
                _cv.notify_all();
            }
        public:
            app_monitor()
                : _state{state::stopped}
                , _id{std::this_thread::get_id()}
            {
            }

            void play()
            {
                handle_event(system_event::play);
            }

            void pause()
            {
                handle_event(system_event::pause);
            }

            void stop()
            {
                handle_event(system_event::stop);
            }

            /// May be only invoked on tracking thread!
            void request_wait()
            {
                throw_if_on_wrong_thread();

                {
                    std::unique_lock<std::mutex> lock{_lock};
                    while (_state != state::playing) {
                        // The predicate variant seems to be dysfunctional...?!
                        _cv.wait(lock/*, [&]() { return _state == state::playing; }*/);
                    }
                }
            }
        };
    }
}