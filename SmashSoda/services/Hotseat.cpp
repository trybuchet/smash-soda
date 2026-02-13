#include "../Hosting.h"
extern Hosting g_hosting;
#include "Hotseat.h"
#include "../core/GamepadClient.h"

Hotseat Hotseat::instance = Hotseat();

/**
 * @brief Constructor for the Hotseat class.
 */
Hotseat::Hotseat() {
    reminderTimer = new StopwatchTimer();
}

// --------------------- MAIN FUNCTIONS ---------------------

/**
 * @brief Starts the hotseat session by initializing and running the hotseat thread.
 * Ensures that only one instance of the hotseat thread is running at a time.
 */
void Hotseat::start() {

    // Start the hotseat thread
    if (!running) {

        // Start reminder timer
        if (Config::cfg.hotseat.reminderInterval > 0) {
            reminderTimer->start(Config::cfg.hotseat.reminderInterval);
        }

        g_hosting.logMessage("[HOTSEAT] Hotseat started.");
        running = true;
        hotseatThread = std::thread([&] {

	            while (running) {
					vector<int> staleUsers;

	                // For each hotseat user
	                for (HotseatUser& user : Hotseat::instance.users) {

						// Is the user seated?
						if (user.inSeat) {
							// Update stopwatch state before checking completion.
							user.stopwatch->getRemainingSec();

							// Has the user's playtime ended?
							if (user.stopwatch->isFinished()) {

								// Pause the user
								user.inSeat = false;
	                            user.cooldown = true;
	                            user.cooldownTimer->start(Config::cfg.hotseat.resetTime);
	                            user.timeLastPlayed = getCurrentTimestamp();

                            // Strip pads
                            GamepadClient::instance.strip(user.userId);

							// Log user paused
							log(user.userName + " has run out of time. They have " + getCooldownRemaining(user.userId) + " to wait.");

	                        }

						} else if (user.cooldown) {
							// Update cooldown timer state before checking completion.
							user.cooldownTimer->getRemainingSec();

							if (user.cooldownTimer->isFinished()) {

	                            user.stopwatch->start(Config::cfg.hotseat.playTime);
	                            user.stopwatch->pause();
	                            user.cooldown = false;
	                            user.cooldownTimer->stop();
	                            user.timeLastPlayed = getCurrentTimestamp();
	                        }

						}

					// Check if the user has been inactive for too long
	                    else if (getMinutesDifference(user.timeLastPlayed, getCurrentTimestamp()) >= Config::cfg.hotseat.resetTime) {

	                        staleUsers.push_back(user.userId);

	                    }

					}

					for (int userId : staleUsers) {
						Hotseat::instance.removeUser(userId);
					}

                // Reminder intervals
                if (Config::cfg.hotseat.reminderInterval > 0 && reminderTimer->isFinished()) {
                    int numUsers = 0;
                    std::string reminder = "\n";
					for (HotseatUser& user : Hotseat::instance.users) {
						if (user.inSeat) {
							reminder += user.userName + " has " + getUserTimeRemaining(user.userId) + " remaining.\n";
                            numUsers++;
						}
					}
					if (numUsers > 0) {
						log(reminder);
					}
					reminderTimer->start(Config::cfg.hotseat.reminderInterval);
				}

                std::this_thread::sleep_for(std::chrono::seconds(1));
                
            }
            });
        hotseatThread.detach();
    }
}

/**
 * @brief Stops the hotseat session, terminating the hotseat thread.
 */
void Hotseat::stop() {

	// Stop the hotseat thread
	if (running) {
		g_hosting.logMessage("[HOTSEAT] Hotseat stopped.");
		running = false;
        reminderTimer->stop();
	}

}

// --------------------- USER FUNCTIONS ---------------------

/**
 * @brief Creates a new user for the hotseat session and initializes their playtime.
 * @param id Unique identifier for the user.
 * @param name Name of the user.
 */
void Hotseat::_createUser(int id, std::string name) {
    // Create a new user
    HotseatUser user;
    user.userId = id;
    user.userName = name;
    user.inSeat = false;
    user.timeLastPlayed = getCurrentTimestamp();
    user.stopwatch = new StopwatchTimer();
    user.cooldownTimer = new StopwatchTimer();

    // Add the user to the list
    users.push_back(user);
}

/**
 * @brief Checks if a user exists in the hotseat session.
 * @param id Unique identifier for the user.
 * @param name Name of the user.
 * @return True if the user exists; otherwise, false.
 */
bool Hotseat::checkUser(int id, string name) {

    // Find user
	    HotseatUser* user = getUser(id);
	    if (user != nullptr) {
			if (user->cooldown) {
				return false;
			}
			
	        // Does the user have time remaining?
	        if (user->stopwatch->getRemainingSec() > 0) {
				return true;
			}

        return false;

	}

    return true;
}

/**
 * @brief Removes a user from the hotseat session.
 * @param id Unique identifier for the user.
 */
void Hotseat::removeUser(int id) {

    // Remove HotseatUser from the list
    HotseatUser* user = getUser(id);
    if (user != nullptr) {
		for (int i = 0; i < users.size(); i++) {
			if (users[i].userId == id) {
				users.erase(users.begin() + i);
				break;
			}
		}
	}

}

// --------------------- SEAT FUNCTIONS ---------------------

/**
 * @brief Seats a user in the hotseat.
 * @param id Unique identifier for the user.
 * @param name Name of the user.
 * @return True if seating is successful; otherwise, false.
 */
bool Hotseat::seatUser(int id, string name) {

    // Find user
	HotseatUser* user = getUser(id);
		if (user != nullptr) {

		// Is the user already seated?
		if (user->inSeat) {
			return false;
		}

			// Seat the user
			if (user->cooldown) {
				return false;
			}
			user->inSeat = true;
	        user->timeLastPlayed = getCurrentTimestamp();
			log(name + " is now in the hotseat. They have " + getUserTimeRemaining(user->userId) + " remaining.");

	        // Resume paused timer when available; otherwise start a fresh play window.
	        if (user->stopwatch->isPaused()) {
	            user->stopwatch->resume();
	        }
	        else if (!user->stopwatch->isRunning()) {
	            if (user->stopwatch->getRemainingSec() <= 0) {
	                user->stopwatch->start(Config::cfg.hotseat.playTime);
	            }
	        }

		return true;
	} else {
		
        // Create a new user
        _createUser(id, name);

        // Seat the user
        user = getUser(id);
        user->inSeat = true;
        user->stopwatch->start(Config::cfg.hotseat.playTime);
        user->timeLastPlayed = getCurrentTimestamp();
        log(name + " is now in the hotseat. They have " + getUserTimeRemaining(user->userId) + " remaining.");

		return true;

	}

	return false;

}

/**
 * @brief Pauses the playtime of a user currently in the hotseat.
 * @param id Unique identifier for the user.
 */
void Hotseat::pauseUser(int id) {

    // Find user
    HotseatUser* user = getUser(id);
    if (user != nullptr) {

		// Is the user seated?
			if (user->inSeat) {

				// Pause the stopwatch
				user->stopwatch->pause();
	            user->inSeat = false;
            user->timeLastPlayed = getCurrentTimestamp();

			// Log user paused
			log(user->userName + " has left the hotseat. They have " + getUserTimeRemaining(user->userId) + " remaining.");

		}

	}

}

/**
 * @brief Extends the playtime for a user in the hotseat.
 * @param id Unique identifier for the user.
 * @param minutes Number of minutes to add to the user's playtime.
 */
void Hotseat::extendUserPlaytime(int id, long minutes) {

}

/**
 * @brief Reduces the playtime for a user in the hotseat.
 * @param id Unique identifier for the user.
 * @param minutes Number of minutes to subtract from the user's playtime.
 */
void Hotseat::reduceUserPlaytime(int id, long minutes) {

}

/**
 * @brief Increases the cooldown time for a user after their playtime ends.
 * @param id Unique identifier for the user.
 * @param minutes Number of minutes to add to the cooldown time.
 */
void Hotseat::increaseUserCooldown(int id, long minutes) {

}

/**
 * @brief Decreases the cooldown time for a user.
 * @param id Unique identifier for the user.
 * @param minutes Number of minutes to subtract from the cooldown time.
 */
void Hotseat::decreaseUserCooldown(int id, long minutes) {

}

void Hotseat::applySettingsDelta(int playDeltaMinutes, int resetDeltaMinutes, int reminderIntervalMinutes) {
	for (HotseatUser& user : users) {
		if (playDeltaMinutes != 0) {
			if (playDeltaMinutes > 0) {
				user.stopwatch->addMinutes(playDeltaMinutes);
			}
			else {
				user.stopwatch->subtractMinutes(-playDeltaMinutes);
			}
		}

		if (user.cooldown && resetDeltaMinutes != 0) {
			if (resetDeltaMinutes > 0) {
				user.cooldownTimer->addMinutes(resetDeltaMinutes);
			}
			else {
				user.cooldownTimer->subtractMinutes(-resetDeltaMinutes);
			}
		}
	}

	// Restart reminder cadence using latest interval.
	if (running) {
		if (reminderIntervalMinutes > 0) {
			reminderTimer->start(reminderIntervalMinutes);
		}
		else {
			reminderTimer->stop();
		}
	}
}

/**
 * @brief Checks if a user is currently seated in the hotseat.
 * @param id Unique identifier for the user.
 * @return True if the user is in the seat; otherwise, false.
 */
bool Hotseat::inSeat(int id) {

    // Find user
    HotseatUser* user = getUser(id);
    if (user != nullptr) {
        return user->inSeat;
    }

    return false;
}

// --------------------- GETTERS ---------------------

/**
 * @brief Retrieves a user by their ID.
 * @param id Unique identifier for the user.
 * @return Pointer to the HotseatUser if found; otherwise, nullptr.
 */
Hotseat::HotseatUser* Hotseat::getUser(int id) {
    for (HotseatUser& user : users) {
        if (user.userId == id) {
            return &user;
        }
    }
    return nullptr;
}

/**
 * @brief Gets the remaining playtime for a user in the hotseat.
 * @param id Unique identifier for the user.
 * @return String representation of the user's remaining playtime.
 */
string Hotseat::getUserTimeRemaining(int id) {

    // Formats the time remaining to 00m 00s format
    HotseatUser* user = getUser(id);
    if (user != nullptr) {
		int remainingSec = user->stopwatch->getRemainingSec();
		int minutes = remainingSec / 60;
		int seconds = remainingSec % 60;
		return std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
	}

    return "";
}

/**
 * @brief Gets the remaining cooldown time for a user.
 * @param id Unique identifier for the user.
 * @return String representation of the user's cooldown time remaining.
 */
string Hotseat::getCooldownRemaining(int id) {

    // Formats the time remaining to 00m 00s format
    HotseatUser* user = getUser(id);
    if (user != nullptr) {
        int remainingSec = user->cooldownTimer->getRemainingSec();
        int minutes = remainingSec / 60;
        int seconds = remainingSec % 60;
        return std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
    }

    return "";
}

// --------------------- HELPER FUNCTIONS ---------------------

/**
 * @brief Gets the current timestamp.
 * @return Current time in seconds since the epoch.
 */
std::time_t Hotseat::getCurrentTimestamp() {

    // Get the current time point
    auto currentTime = std::chrono::system_clock::now();

    // Convert the time point to a duration since the epoch
    auto durationSinceEpoch = currentTime.time_since_epoch();

    // Convert the duration to seconds
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(durationSinceEpoch);

    // Return the timestamp as time_t type
    return seconds.count();
}

/**
 * @brief Calculates the difference in minutes between two timestamps.
 * @param timestamp1 The first timestamp.
 * @param timestamp2 The second timestamp.
 * @return Difference in minutes between the two timestamps.
 */
int Hotseat::getMinutesDifference(std::time_t timestamp1, std::time_t timestamp2) {

    // Calculate the difference in seconds
    std::time_t difference = timestamp2 - timestamp1;

    // Convert the difference to minutes
    int minutesDifference = difference / 60;

    return minutesDifference;
}

/**
 * @brief Adds a specified number of minutes to a timestamp.
 * @param timestamp The initial timestamp.
 * @param minutesToAdd Number of minutes to add.
 * @return New timestamp after adding the specified minutes.
 */
std::time_t Hotseat::addMinutesToTimestamp(std::time_t timestamp, int minutesToAdd) {

    // Convert the minutes to seconds
    std::time_t secondsToAdd = minutesToAdd * 60;

    // Add the seconds to the timestamp
    std::time_t newTimestamp = timestamp + secondsToAdd;

    return newTimestamp;
}

/**
 * @brief Subtracts a specified number of minutes from a timestamp.
 * @param timestamp The initial timestamp.
 * @param minutesToSubtract Number of minutes to subtract.
 * @return New timestamp after subtracting the specified minutes.
 */
std::time_t Hotseat::subtractMinutesFromTimestamp(std::time_t timestamp, int minutesToSubtract) {

    // Convert the minutes to seconds
    std::time_t secondsToSubtract = minutesToSubtract * 60;

    // Subtract the seconds from the timestamp
    std::time_t newTimestamp = timestamp - secondsToSubtract;

    return newTimestamp;
}

/**
 * @brief Logs a message with the hotseat prefix and broadcasts it.
 * @param message The message to log and broadcast.
 */
void Hotseat::log(string message) {

    // Format message
    string msg = "[HOTSEAT] ";
    msg += message;

    // Print message
    g_hosting.logMessage(msg);
    g_hosting.broadcastChatMessage(msg);
}


