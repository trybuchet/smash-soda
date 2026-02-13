#pragma once
#include <chrono>
#include <sstream>
#include <iomanip>

using namespace std::chrono;

/// <summary>
/// Stopwatch that counts down from a given time until it reaches 0.
/// </summary>
class StopwatchTimer {
private:
    std::chrono::steady_clock::time_point _startTime; ///< Time when the timer was started.
    std::chrono::steady_clock::time_point _pauseTime; ///< Time when the timer was paused.

    bool _isRunning = false; ///< True if the timer is currently running.
    bool _isPaused = false; ///< True if the timer is currently paused.

    int _totalMin = 0; ///< Total minutes the timer was set to.
    int _remainingTime = 0; ///< Remaining time in milliseconds.
    int _timeChange = 0; ///For use with the add/subtract minutes method

    void updateRemainingTime();

public:
    void start(int minutes);
    void stop();
    void pause();
    void resume();
    void reset();

    int getRemainingMs();
    int getRemainingSec();

    std::string getRemainingTime();

    void addMinutes(int minutes);

    void subtractMinutes(int minutes);

    bool isRunning();
    bool isFinished();
    bool isPaused();
    
};


