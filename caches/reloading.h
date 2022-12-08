#pragma once

#include <atomic>
#include <thread>
#include <chrono>

#ifndef CACHES_TRACE
#define CACHES_TRACE(fmt, ...)
#endif

namespace caches
{
template <typename T>
class Reloading {
  public:
    template <typename... Args>
    Reloading(time_t interval, Args... args)
        : ping(args...), pong(args...), m_isping(true), m_reloading(false), m_interval(interval), m_reloaded_time(0)
    {
        ping.Reload();
        m_reloaded_time = time(nullptr);
        CACHES_TRACE("reloading procedure finished, activated = ping");
    }

    void SetInterval(time_t seconds)
    {
        m_interval = seconds;
    }

    T &GetActivated()
    {
        // start thread to switch config only if no worker running in background
        if (!m_reloading && m_reloaded_time + m_interval <= time(nullptr)) {
            bool expected = false;
            if (m_reloading.compare_exchange_strong(expected, true, std::memory_order_release,
                                                    std::memory_order_relaxed)) {
                std::thread([this]() {
                    auto elasped = [starttime = std::chrono::steady_clock::now()]() {
                        auto duration = std::chrono::steady_clock::now() - starttime;
                        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
                    };

                    // CACHES_TRACE("reloading worker started in background");
                    if (m_isping.load()) {
                        if (pong.Reload() == 0) {
                            m_isping.store(false, std::memory_order_relaxed);
                            CACHES_TRACE("reloading succeed, activated = pong, elasped = %lld us", elasped());
                        } else {
                            // non-zero will returned if no changes or has errors
                            // CACHES_TRACE("reloading slienced for a while 'cause failure, activated = ping, elasped =
                            // %lld us", elasped());
                        }
                    } else {
                        if (ping.Reload() == 0) {
                            m_isping.store(true, std::memory_order_relaxed);
                            CACHES_TRACE("reloading succeed, activated = ping, elasped = %lld us", elasped());
                        } else {
                            // non-zero will returned if no changes or has errors
                            // CACHES_TRACE("reloading slienced for a while 'cause failure, activated = pong, elasped =
                            // %lld us", elasped());
                        }
                    }
                    m_reloaded_time = time(nullptr); // update reloading time each attempt
                    m_reloading.store(false, std::memory_order_relaxed);
                    // CACHES_TRACE("reloading procedure finished, elasped = %lld us", elasped());
                }).detach(); // detach to reload in background
            } else {
                // do nothing 'cause there is an worker running in background
                // CACHES_TRACE("reloading worker started, no worker created this time");
            }
        }

        return m_isping.load() ? ping : pong;
    }

  private:
    T ping, pong;                       // double buffering for reloading
    std::atomic_bool m_isping;          // true if ping selected
    std::atomic_bool m_reloading;       // true if reloading in background
    time_t m_interval, m_reloaded_time; // reloading interval and last reloaded time
};

#define ONCE_RUNNABLE()                                                                                        \
    ({                                                                                                         \
        bool expected = false;                                                                                 \
        static std::atomic_bool running{false};                                                                \
        running.compare_exchange_strong(expected, true, std::memory_order_release, std::memory_order_relaxed); \
    })
} // namespace caches
