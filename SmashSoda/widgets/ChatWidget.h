#pragma once

#define _WINSOCKAPI_

#include "ToggleIconButtonWidget.h"
#include "TitleTooltipWidget.h"
#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../Hosting.h"
#include "Stopwatch.h"
#include <functional>
#include "Widget.h"
#include "../services/ThemeController.h"
#include "../models/Theme.h"

class ChatWidget : public Widget
{
public:
	ChatWidget(Hosting& hosting, function<void(void)> onMessageCallback = nullptr);
	bool render(bool &showWindow);

	const ImVec2 DEFAULT_BUTTON_SIZE = ImVec2(40, 40);

	#define LOG_BUFFER_LEN 16383
	#define SEND_BUFFER_LEN 1023

private:
	bool renderTopBar(bool& isWindowLocked, bool& isClearChat);

	bool isDirty();
	void sendMessage();
	bool setSendBuffer(const char* value);
	void toClipboard(const string& message);

	// Dependency injection
	Hosting& _hosting;

	// Attributes
	string _logBuffer;
	char _sendBuffer[SEND_BUFFER_LEN];
	char _lastBuffer[SEND_BUFFER_LEN];
	char _previewBuffer[SEND_BUFFER_LEN];
	vector<string>& _chatLog;
	size_t _messageCount;
	function<void(void)> _onMessageCallback;
	bool _focusSendInputNextFrame = true;
};


