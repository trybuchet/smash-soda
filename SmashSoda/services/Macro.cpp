#include "Macro.h"

/// <summary>
/// Constructor
/// </summary>
Macro::Macro()
{
}

/// <summary>
/// Initializes Macro module.
/// </summary>
void Macro::init() {
}

/// <summary>
/// Runs any enqueued macro commands.
/// </summary>
void Macro::run() {

	// Buttons in queue?
	if (cmdList.size() > 0) {

		// Press in
		if (!cmdList.front().pressed) {

			ParsecGamepadButtonMessage btn = createButtonMessage(cmdList.front().button, true);
			sendButtonForAll(btn);

			cmdList.front().pressed = true;

		}
		else {

			// Depress
			ParsecGamepadButtonMessage btn = createButtonMessage(cmdList.front().button, false);

			if (cmdList.front().padIndex != -1) {
				sendButtonForPad(cmdList.front().padIndex, btn);
			}
			else

				if (cmdList.front().guestId != -1) {
					sendButtonForGuest(cmdList.front().guestId, btn);
				}
				else {
					sendButtonForAll(btn);
				}

			// Remove from queue
			cmdList.erase(cmdList.begin());

		}

	}

}

/// <summary>
/// Enqueues a button press for everyone.
/// </summary>
/// <param name="button"></param>
void Macro::pressButtonForAll(ParsecGamepadButton button) {

	// Create new command
	Macro::Cmd cmd = Macro::Cmd();
	cmd.button = button;

	// Enqueue
	cmdList.push_back(cmd);

}

/// <summary>
/// Enqueues a button press for a specified gamepad.
/// </summary>
/// <param name="button"></param>
void Macro::pressButtonForPad(int padIndex, ParsecGamepadButton button) {

	// Create new command
	Macro::Cmd cmd = Macro::Cmd();
	cmd.padIndex = padIndex;
	cmd.button = button;

	// Enqueue
	cmdList.push_back(cmd);

}

/// <summary>
/// Enqueues a button press for everyone.
/// </summary>
/// <param name="button"></param>
void Macro::pressButtonForGuest(int guestIndex, ParsecGamepadButton button) {

	// Create new command
	Macro::Cmd cmd = Macro::Cmd();
	cmd.guestId = guestIndex;
	cmd.button = button;

	// Enqueue
	cmdList.push_back(cmd);

}

/// <summary>
/// Press a button for ALL connected guests in your room.
/// </summary>
/// <param name="button">The button to press.</param>
void Macro::sendButtonForAll(ParsecGamepadButtonMessage button) {

	ParsecMessage message = {};
	message.type = ParsecMessageType::MESSAGE_GAMEPAD_BUTTON;
	message.gamepadButton = button;

	// Press button for other pads
	std::vector<AGamepad*>::iterator gi = GamepadClient::instance.gamepads.begin();
	for (; gi != GamepadClient::instance.gamepads.end(); ++gi) {
		GamepadClient::instance.sendButtonMessageForPad((*gi), button);
	}

}

/// <summary>
/// Press a button for specified pad.
/// </summary>
/// <param name="button">The button to press.</param>
void Macro::sendButtonForPad(int padIndex, ParsecGamepadButtonMessage button) {

	ParsecMessage message = {};
	message.type = ParsecMessageType::MESSAGE_GAMEPAD_BUTTON;
	message.gamepadButton = button;

	// Press button for other pad
	GamepadClient::instance.sendButtonMessageForPad(GamepadClient::instance.getGamepad(padIndex), button);

}

/// <summary>
/// Press a button for guest's pad.
/// </summary>
/// <param name="button">The button to press.</param>
void Macro::sendButtonForGuest(int guestId, ParsecGamepadButtonMessage button) {

	ParsecMessage message = {};
	message.type = ParsecMessageType::MESSAGE_GAMEPAD_BUTTON;
	message.gamepadButton = button;

	// Press button for other pad
	std::vector<AGamepad*>::iterator gi = GamepadClient::instance.gamepads.begin();
	for (; gi != GamepadClient::instance.gamepads.end(); ++gi) {
		if ((*gi)->owner.guest.userID == guestId) {
			GamepadClient::instance.sendButtonMessageForPad((*gi), button);
			return;
		}
	}

}

/// <summary>
/// Creates a new button gamepad message.
/// </summary>
/// <param name="button">The button to press.</param>
/// <param name="in">Is the button pressed in or not.</param>
/// <returns>ParsecGamepadButtonMessage</returns>
ParsecGamepadButtonMessage Macro::createButtonMessage(ParsecGamepadButton button, bool in) {

	ParsecGamepadButtonMessage msg = {};
	msg.button = button;
	msg.pressed = in;

	return msg;

}


