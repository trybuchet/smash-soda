// TimeT.h - Calls the passed function if the minimum time interval has elapsed
#ifndef TIME_T_H_
#define TIME_T_H_

#include <chrono>

/// Tracks the time since execution() was called, and only
/// Calls the passed function if the minimum time interval has elapsed
/// @see https://stackoverflow.com/questions/2808398/easily-measure-elapsed-time for the code I based this on
template<typename TimeT = std::chrono::milliseconds>
struct periodic
{
    periodic(TimeT duration = TimeT(1))
        : start(std::chrono::system_clock::now())
        , period_duration(duration)
        , previous_duration(TimeT::zero())
    {};

    template<typename F, typename ...Args>
    TimeT execution(F func, Args&&... args)
    {
        auto duration = std::chrono::duration_cast<TimeT>
            (std::chrono::system_clock::now() - start);
        if (duration > previous_duration + period_duration)
        {
            std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
            previous_duration = duration;
        }
        return duration;
    }

    std::chrono::time_point<std::chrono::system_clock> start;
    // The minimum duration to wait before the function can be called again
    TimeT period_duration;
    // The duration between startup and the last time the function was called
    TimeT previous_duration;
};

#endif // TIME_T_H_


