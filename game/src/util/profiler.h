#pragma once

#ifdef DEBUG
#define ENABLE_PROFILE
#endif

#include <vector>
#include <string>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <format>
#include <algorithm>


namespace profiler {
    constexpr std::size_t MAX_DURATIONS = 128;

    class Profiler {
    private:
        std::string m_name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
        uint64_t m_duration = 0;
        double m_max_avg_duration = 0;
        uint64_t m_old_durations[MAX_DURATIONS] = {0};
        std::size_t m_duration_idx = 0;

    public:
        explicit Profiler(const std::string &name): m_name(name) {}

        /// @brief  Start the timer
        void start() { m_start = std::chrono::high_resolution_clock::now(); }

        /// @brief  Reset cumulative duration and start the timer
        void reset_and_start() {
            reset();
            start();
        }

        /// @brief  Reset the timer
        void reset() { m_duration = 0; }

        /// @brief End the timer
        void end() {
            auto finish = std::chrono::high_resolution_clock::now();
            m_duration += static_cast<uint64_t>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(finish - m_start).count());
        }

        /// @brief Commit, reset stats and return a summary debug string
        /// @return Summary str
        std::string summarize() {
            m_old_durations[m_duration_idx] = m_duration;
            m_duration_idx = (m_duration_idx + 1) % MAX_DURATIONS;
            auto current_dur = m_duration;
            reset();

            double avg_duration = 0.0;
            for (std::size_t i = 0; i < MAX_DURATIONS; i++)
                avg_duration += static_cast<double>(m_old_durations[i]);
            avg_duration /= MAX_DURATIONS;
            m_max_avg_duration = std::max(avg_duration, m_max_avg_duration);
            return std::format("[{:16}]: frame: {:<12.5f} us | avg: {:<12.5f} us | max avg: {:<12.5f}",
                m_name, current_dur / 1000.0, avg_duration / 1000.0, m_max_avg_duration / 1000.0);
        }
    };

#ifdef ENABLE_PROFILE
    inline std::vector<Profiler> profilers;
#endif

    inline void add(const std::string &name) {
    #ifdef ENABLE_PROFILE
        profilers.emplace_back(name);
    #endif
    }

    inline void start(std::size_t idx) {
    #ifdef ENABLE_PROFILE
        profilers.at(idx).start();
    #endif
    }

    inline void reset_and_start(std::size_t idx) {
    #ifdef ENABLE_PROFILE
        profilers.at(idx).reset_and_start();
    #endif
    }

    inline void reset(std::size_t idx) {
    #ifdef ENABLE_PROFILE
        profilers.at(idx).reset();
    #endif
    }

    inline void end(std::size_t idx) {
    #ifdef ENABLE_PROFILE
        profilers.at(idx).end();
    #endif
    }

    inline std::string summarize(std::size_t idx) {
    #ifdef ENABLE_PROFILE
        return profilers.at(idx).summarize();
    #else
        return "";
    #endif
    }

} // namespace profiler
