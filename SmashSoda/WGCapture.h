#pragma once

#include <d3d11.h>

class WGCapture
{
public:
	WGCapture();
	~WGCapture();

	static bool isSupported();

	bool start(ID3D11Device* device, ID3D11DeviceContext* context, HMONITOR monitor);
	void stop();
	bool isActive() const;

	ID3D11Texture2D* acquireFrame();
	void getFrameSize(int& width, int& height) const;

private:
	struct Impl;
	Impl* _impl;
};
