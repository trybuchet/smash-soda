#pragma once

#include <string>
#include "Widget.h"
using namespace std;

class StreamingWidget : public Widget
{
public:

	// Constructor
	StreamingWidget();

	bool render(bool& showWindow);
};