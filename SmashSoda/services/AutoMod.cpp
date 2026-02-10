#include "../Hosting.h"
extern Hosting g_hosting;
#include "AutoMod.h"

AutoMod AutoMod::instance = AutoMod::AutoMod();

/// <summary>
/// Constructor
/// </summary>
AutoMod::AutoMod() {

}

/// <summary>
/// Start the AutoMod thread
/// </summary>
void AutoMod::Start() {

	// If not already running
	if (isRunning) {
		return;
	}

	// Start the thread lambda
	isRunning = true;
	thread([this]() {

		// Loop
		while (isRunning) {

			// Loop through muted users
			for (int i = 0; i < mutedUsers.size(); i++) {

				// Get the user
				MutedUser* user = &mutedUsers[i];

				// Check if the user is still muted
				if (user->stopwatch->isFinished()) {

					// Log the unmute
					Log(user->userName + " has been unmuted.");

					// Unmute the user
					UnmuteUser(user->userId);

				}

			}

			// Has there been more than 5 messages in the last 2 seconds?
			if (lastMessages.size() > 5) {

				// Get the last message
				ChatMessage lastMessage = lastMessages.back();

				// Check if the same user has sent 5 messages in the last 2 seconds
				int count = 0;
				for (int i = lastMessages.size() - 1; i >= 0; i--) {
					if (lastMessages[i].userId == lastMessage.userId) {
						count++;
					}
				}

				// They spammin', mute 'em
				if (count >= 5) {
					MuteUser(lastMessage.userId, lastMessage.userName, 5);
				}

			}

			// Clear the last messages
			lastMessages.clear();

			// Sleep
			this_thread::sleep_for(chrono::seconds(2));

		}

	}).detach();

}

/// <summary>
/// Stop the AutoMod thread
/// </summary>
void AutoMod::Stop() {

	isRunning = false;

}

/// <summary>
/// Record last chat message
/// </summary>
/// <param name="userId"></param>
/// <param name="message"></param>
void AutoMod::RecordMessage(int userId, string userName, string message) {

	// Create a new message
	ChatMessage msg;
	msg.userId = userId;
	msg.userName = userName;
	msg.message = message;

	// Add the message to the list
	lastMessages.push_back(msg);

}

/// <summary>
/// Mute a user (in minutes)
/// </summary>
/// <param name="userId"></param>
/// <param name="userName"></param>
/// <param name="duration"></param>
void AutoMod::MuteUser(int userId, string userName, int duration) {

	// Check if the user is already muted
	MutedUser* user = GetMutedUser(userId);
	if (user != nullptr) {
		return;
	}

	// Create a new muted user
	MutedUser mutedUser;
	mutedUser.userId = userId;
	mutedUser.userName = userName;
	mutedUser.stopwatch->setDuration(duration * 60 * 1000);
	mutedUser.stopwatch->start();

	// Add the user to the muted list
	mutedUsers.push_back(mutedUser);

	// Log the mute
	Log("Muted " + userName + " for " + to_string(duration) + " minutes.");

}

/// <summary>
/// Unmute a user
/// </summary>
/// <param name="userId"></param>
void AutoMod::UnmuteUser(int userId) {

	// Remove the user from the muted list
	for (int i = 0; i < mutedUsers.size(); i++) {
		if (mutedUsers[i].userId == userId) {
			mutedUsers.erase(mutedUsers.begin() + i);
			break;
		}
	}

}

/// <summary>
/// Is the user muted?
/// </summary>
/// <param name="userId"></param>
bool AutoMod::isMuted(int userId) {

	// Get the muted user
	MutedUser* user = GetMutedUser(userId);
	if (user != nullptr) {
		return true;
	}

	return false;

}

/// <summary>
/// Get a muted user
/// </summary>
/// <param name="userId"></param>
/// <returns></returns>
AutoMod::MutedUser* AutoMod::GetMutedUser(int userId) {
	
	// Loop through muted users
	for (int i = 0; i < mutedUsers.size(); i++) {
		if (mutedUsers[i].userId == userId) {
			return &mutedUsers[i];
		}
	}

	return nullptr;

}

/// <summary>
/// Log a message and print in chat.
/// </summary>
/// <param name="message"></param>
void AutoMod::Log(string message) {

	// Format message
	string msg = Config::cfg.chatbotName;
	msg += message;

	// Print message
	g_hosting.logMessage(msg);
	g_hosting.broadcastChatMessage(msg);

}


