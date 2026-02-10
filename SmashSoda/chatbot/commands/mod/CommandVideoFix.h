#pragma once

#include "ACommand.h"
#include "../../../DX11.h"

class CommandVideoFix : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandVideoFix object
	 * 
	 * @param dx11
	 */
	CommandVideoFix(const char* msg, Guest& sender, DX11 &dx11)
		: ACommand(msg, sender), _dx11(dx11)
	{}

	/**
	 * @brief Run the command
	 * 
	 * @return true
	 * @return false
	 */
	bool run() override {
		setReply("Refreshing Directx11...\0");
		_dx11.recover();
		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!videofix" };
	}

protected:
	DX11& _dx11;
};


