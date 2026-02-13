#include "StopwatchTimer.h"

/// <summary>
/// Start the timer with the given minutes
/// </summary>
/// <param name="minutes"></param>
void StopwatchTimer::start(int minutes) {
    _totalMin = minutes; //35 minutes
    _remainingTime = minutes * 60 * 1000;
    _timeChange = 0;
    _startTime = steady_clock::now();
    _isRunning = true;
    _isPaused = false;
}

/// <summary>
/// Stops the timer
/// </summary>
void StopwatchTimer::stop() {
    _isRunning = false;
    _isPaused = false;
}

/// <summary>
/// Pauses the timer and stores the remaining time
/// </summary>
void StopwatchTimer::pause() { 
    _pauseTime = steady_clock::now();
    _isPaused = true;
    _isRunning = false;

    auto elapsedTime = duration_cast<milliseconds>(_pauseTime - _startTime);
    _remainingTime = (std::max)(0, _totalMin * 60 * 1000 - static_cast<int>(elapsedTime.count()));

}

/// <summary>
/// Resumes the timer with the remaining time
/// </summary>
void StopwatchTimer::resume() {
    _startTime += steady_clock::now() - _pauseTime;
    _isPaused = false;
    _isRunning = true;
}

/// <summary>
/// Resets the timer to the initial time
/// </summary>
void StopwatchTimer::reset() {
    _isRunning = false;
    _isPaused = false;
    _remainingTime = _totalMin * 60 * 1000;
}

/// <summary>
/// Returns the remaining time in milliseconds
/// </summary>
/// <returns></returns>
int StopwatchTimer::getRemainingMs() {
    updateRemainingTime();
    return _remainingTime;
}

/// <summary>
/// Returns the remaining time in seconds
/// </summary>
/// <returns></returns>
int StopwatchTimer::getRemainingSec() {
	updateRemainingTime();
	return _remainingTime / 1000;
}

/// <summary>
/// Returns the remaining time in the format "mm:ss"
/// </summary>
/// <returns></returns>
std::string StopwatchTimer::getRemainingTime() {
    updateRemainingTime();
    std::stringstream ss;

    // Get the remaining minutes
    auto remainingMin = _remainingTime / 60000;
    ss << std::setfill('0') << std::setw(2) << remainingMin << "m:";
	// Get the remaining seconds
	auto remainingSec = (_remainingTime / 1000) % 60;
    ss << std::setfill('0') << std::setw(2) << remainingSec << "s";

    return ss.str();
}

/// <summary>
/// Adds the given minutes to the remaining time
/// </summary>
/// <param name="minutes"></param>
void StopwatchTimer::addMinutes(int minutes) {
    if (minutes <= 0) {
        return;
    }

    const int deltaMs = minutes * 60 * 1000;
    _totalMin += minutes;

    // When paused/stopped, keep remaining time in sync immediately.
    if (!_isRunning || _isPaused) {
        _remainingTime += deltaMs;
    }
}

/// <summary>
/// Subtracts the given minutes from the remaining time
/// </summary>
/// <param name="minutes"></param>
void StopwatchTimer::subtractMinutes(int minutes) {
    if (minutes <= 0) {
        return;
    }

    const int deltaMs = minutes * 60 * 1000;
    _totalMin = (std::max)(0, _totalMin - minutes);

    // When paused/stopped, keep remaining time in sync immediately.
    if (!_isRunning || _isPaused) {
        _remainingTime = (std::max)(0, _remainingTime - deltaMs);
    }
}

/// <summary>
/// Returns true if the timer is running
/// </summary>
/// <returns></returns>
bool StopwatchTimer::isRunning() {
	return _isRunning;
}

/// <summary>
/// Returns true if the timer has finished
/// </summary>
/// <returns></returns>
bool StopwatchTimer::isFinished() {
	return _remainingTime <= 0;
}

/// <summary>
/// Returns false if the timer is running
/// </summary>
/// <returns></returns>
bool StopwatchTimer::isPaused() {
	return _isPaused;
}

/// <summary>
/// Updates the remaining time if the timer is running
/// </summary>
void StopwatchTimer::updateRemainingTime() {
    if (_isRunning && !_isPaused) {
        auto elapsedTime = duration_cast<milliseconds>(steady_clock::now() - _startTime);
        _remainingTime = (std::max)(0, _totalMin * 60 * 1000 - static_cast<int>(elapsedTime.count()) + _timeChange);

        // Stop the timer if the remaining time is 0
        if (_remainingTime <= 0) {
			_isRunning = false;
		}
    }
}

