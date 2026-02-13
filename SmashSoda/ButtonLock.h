#pragma once

typedef struct LockedGamepadState
{
	unsigned int wButtons{ 0 };
	bool bLeftTrigger{ false };
	bool bRightTrigger{ false };
	bool sThumbLX{ false };
	bool sThumbLY{ false };
	bool sThumbRX{ false };
	bool sThumbRY{ false };
} LockedGamepadState;



