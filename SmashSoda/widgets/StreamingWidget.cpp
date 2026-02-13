#include "StreamingWidget.h"

// Constructor
StreamingWidget::StreamingWidget() {
}

bool StreamingWidget::render(bool& showWindow) {
	startWidget("Streaming", showWindow, 422, 10, 800, 1000, 350, 400);

	startBody(true);
	endBody();

	startFooter();
	endFooter();

	endWidget();
	return true;
}