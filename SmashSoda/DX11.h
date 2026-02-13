#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <iostream>
//#include <atlbase.h>
#include <atlcomcli.h>
#include "parsec-dso.h"
#include <vector>
#include <string>
#include <mutex>
#include "Core/Config.h"
#include <locale>
#include <codecvt>

using namespace std;

class DX11
{
	class GPU
	{
	public:
		GPU(IDXGIAdapter1* adapter, DXGI_ADAPTER_DESC1 desc) : adapter(adapter), desc(desc) {}
		IDXGIAdapter1* adapter;
		DXGI_ADAPTER_DESC1 desc;
	};


public:
	void clear();
	bool recover();
	bool recover2();
	bool clearAndRecover();
	void enumGPUS();
	bool init();
	bool captureScreen(ParsecDSO *ps);
	
	vector<string> listScreens();
	void setScreen(UINT index);
	UINT getScreen();
	
	vector<string> listGPUs();
	void setGPU(size_t index);
	size_t getGPU();

	ID3D11Device* getDevice();
	ID3D11DeviceContext* getContext();
	IDXGIOutputDuplication* getDuplication();
	bool createDuplication();
	vector<uint8_t> getScreenshot(int& width, int& height);

	// Resolution scaling
	bool setTargetResolution(int width, int height);
	void getTargetResolution(int& width, int& height);
	void getNativeResolution(int& width, int& height);
	bool isScalingEnabled();

private:
	void fetchScreenList();
	vector<uint8_t> _convertTextureToBmp(ID3D11Texture2D* texture, int& width, int& height);
	bool createScaledRenderTarget(int width, int height);
	void releaseScaledRenderTarget();
	ID3D11Texture2D* scaleTexture(ID3D11Texture2D* source);
	
	mutex _mutex;

	// Windows
	HWND hwnd;

	// D3D11
	ID3D11Device* _device = nullptr;
	ID3D11DeviceContext* _context = nullptr;
	IDXGIOutputDuplication* _deskDupl = nullptr;

	D3D11_TEXTURE2D_DESC d3TexDesc, _desc;
	ID3D11Texture2D* _lAcquiredDesktopImage = nullptr;
	vector<string> _screens;

	UINT _currentScreen = 0;
	size_t _currentGPU = 0;

	vector<GPU> _gpus;
	vector<string> _gpuNames;

	// Resolution scaling resources
	int _targetWidth = 0;
	int _targetHeight = 0;
	ID3D11Texture2D* _scaledTexture = nullptr;
	ID3D11RenderTargetView* _scaledRTV = nullptr;
	ID3D11ShaderResourceView* _sourceSRV = nullptr;
	ID3D11SamplerState* _samplerState = nullptr;
	ID3D11VertexShader* _vertexShader = nullptr;
	ID3D11PixelShader* _pixelShader = nullptr;
	ID3D11InputLayout* _inputLayout = nullptr;
	ID3D11Buffer* _vertexBuffer = nullptr;
	bool _scalingInitialized = false;
};


