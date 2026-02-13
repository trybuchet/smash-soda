#include "DX11.h"
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

#define SAFE_RELEASE(x) {if(x != nullptr) { x->Release(); x = nullptr; }}
#define CLEAN_ON_FAIL(hr) {if (FAILED(hr)) goto CLEANUP;}

D3D_FEATURE_LEVEL gFeatureLevels[] = {
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1,
};
UINT gNumFeatureLevels = 6;
DXGI_OUTPUT_DESC _lOutputDesc;
DXGI_OUTDUPL_DESC _lOutputDuplDesc;
D3D11_MAPPED_SUBRESOURCE _resource;

// Simple fullscreen quad shaders for scaling
const char* g_ScaleShaderCode = R"(
struct VS_INPUT {
    float2 pos : POSITION;
    float2 tex : TEXCOORD0;
};

struct PS_INPUT {
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

PS_INPUT VSMain(VS_INPUT input) {
    PS_INPUT output;
    output.pos = float4(input.pos, 0.0f, 1.0f);
    output.tex = input.tex;
    return output;
}

Texture2D sourceTexture : register(t0);
SamplerState samplerLinear : register(s0);

float4 PSMain(PS_INPUT input) : SV_TARGET {
    return sourceTexture.Sample(samplerLinear, input.tex);
}
)";


void DX11::clear()
{
	SAFE_RELEASE(_deskDupl);
	SAFE_RELEASE(_lAcquiredDesktopImage);
	releaseScaledRenderTarget();
}

void DX11::releaseScaledRenderTarget()
{
	SAFE_RELEASE(_scaledTexture);
	SAFE_RELEASE(_scaledRTV);
	SAFE_RELEASE(_sourceSRV);
	SAFE_RELEASE(_samplerState);
	SAFE_RELEASE(_vertexShader);
	SAFE_RELEASE(_pixelShader);
	SAFE_RELEASE(_inputLayout);
	SAFE_RELEASE(_vertexBuffer);
	_scalingInitialized = false;
}

bool DX11::recover()
{
	return createDuplication();
}

bool DX11::recover2()
{
	bool success = false;

	if (_gpus.size() <= 0 || _currentGPU >= _gpus.size()) return success;

	HRESULT hr;
	IDXGIAdapter1* dxgiAdapter = _gpus[_currentGPU].adapter;
	IDXGIOutput* dxgiOutput = nullptr;
	IDXGIOutput1* dxgiOutput1 = nullptr;
	IDXGIDevice* dxgiDevice = nullptr;

	hr = D3D11CreateDevice(
		dxgiAdapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION,
		&_device,
		nullptr,
		&_context
	);
	CLEAN_ON_FAIL(hr);

	hr = dxgiAdapter->EnumOutputs(_currentScreen, &dxgiOutput);
	CLEAN_ON_FAIL(hr);

	hr = dxgiOutput->QueryInterface(__uuidof(dxgiOutput1), reinterpret_cast<void**>(&dxgiOutput1));
	CLEAN_ON_FAIL(hr);

	hr = _device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
	CLEAN_ON_FAIL(hr);

	dxgiOutput1->DuplicateOutput(dxgiDevice, &_deskDupl);
	CLEAN_ON_FAIL(hr);

	if (_deskDupl != nullptr)
	{
		_deskDupl->GetDesc(&_lOutputDuplDesc);
		_desc.Width = _lOutputDuplDesc.ModeDesc.Width;
		_desc.Height = _lOutputDuplDesc.ModeDesc.Height;
		_desc.Format = _lOutputDuplDesc.ModeDesc.Format;
		_desc.ArraySize = 1;
		_desc.BindFlags = 0;
		_desc.MiscFlags = 0;
		_desc.SampleDesc.Count = 1;
		_desc.SampleDesc.Quality = 0;
		_desc.MipLevels = 1;
		_desc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
		_desc.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;
	}

	success = true;

CLEANUP:
	//SAFE_RELEASE(_device);
	//SAFE_RELEASE(_context);
	//SAFE_RELEASE(dxgiAdapter);
	SAFE_RELEASE(dxgiOutput);
	SAFE_RELEASE(dxgiOutput1);
	SAFE_RELEASE(dxgiDevice);

	return success;
}

bool DX11::clearAndRecover()
{
	_mutex.lock();
	clear();
	bool result = recover();
	_mutex.unlock();
	return result;
}

void DX11::enumGPUS()
{
	HRESULT hr;

	IDXGIFactory1* dxgiFactory = nullptr;
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
	if (FAILED(hr))
	{
		if (dxgiFactory != nullptr) dxgiFactory->Release();
		return;
	}

	vector<GPU>::iterator it;
	for (it = _gpus.begin(); it != _gpus.end(); ++it)
	{
		if ((*it).adapter != nullptr)
		{
			(*it).adapter->Release();
		}
	}
	_gpus.clear();
	_gpuNames.clear();

	IDXGIAdapter1* dxgiAdapter = nullptr;
	for (UINT i = 0; i < 20; i++)
	{
		hr = dxgiFactory->EnumAdapters1(i, &dxgiAdapter);
		if (FAILED(hr))
		{
			break;
		}

		DXGI_ADAPTER_DESC1 desc;
		dxgiAdapter->GetDesc1(&desc);

		_gpus.push_back(GPU(dxgiAdapter, desc));

		std::string wname = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(desc.Description);
		_gpuNames.push_back("[" + to_string(i) + "] " + wname);
	}

	if (_currentGPU >= _gpus.size())
	{
		_currentGPU = 0;
	}

	if (dxgiFactory != nullptr) dxgiFactory->Release();
}

bool DX11::init()
{
	_currentScreen = Config::cfg.video.monitor;
	_currentGPU = Config::cfg.video.adapter;
	enumGPUS();

	D3D_FEATURE_LEVEL lFeatureLevel;

	IDXGIAdapter* idxgiAdapter = nullptr;
	if (!_gpus.empty() && _currentGPU < _gpus.size())
	{
		idxgiAdapter = _gpus[_currentGPU].adapter;
	}

	HRESULT hr = D3D11CreateDevice(
		idxgiAdapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		nullptr,
		0,
		gFeatureLevels,
		gNumFeatureLevels,
		D3D11_SDK_VERSION,
		&_device,
		&lFeatureLevel,
		&_context);

	if (FAILED(hr) || !_device)
		return false;

	fetchScreenList();
	if (_screens.empty() || _currentScreen >= _screens.size()) {
		_currentScreen = 0;
	}

	return createDuplication();
}


bool DX11::captureScreen(ParsecDSO* ps)
{
	if (!_deskDupl)
	{
		if (!clearAndRecover())
		{
			return false;
		}
	}

	_mutex.lock();

	HRESULT hr;
	IDXGIResource* lDesktopResource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO lFrameInfo;

	hr = _deskDupl->AcquireNextFrame(4, &lFrameInfo, &lDesktopResource);
	if (FAILED(hr)) {
		// Only release frame if we actually acquired one (not on timeout/error)
		if (lDesktopResource != nullptr) {
			lDesktopResource->Release();
			_deskDupl->ReleaseFrame();
		}

		_mutex.unlock();
		if (hr != DXGI_ERROR_WAIT_TIMEOUT)
		{
			clearAndRecover();
		}

		return false;
	}

	// Release the previous acquired image before getting a new one
	SAFE_RELEASE(_lAcquiredDesktopImage);

	hr = lDesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&_lAcquiredDesktopImage);
	lDesktopResource->Release(); // Done with this, release it
	lDesktopResource = nullptr;

	if (FAILED(hr) || _lAcquiredDesktopImage == nullptr)
	{
		_deskDupl->ReleaseFrame();
		_mutex.unlock();
		return false;
	}

	// Check if we need to scale the frame
	ID3D11Texture2D* frameToSubmit = _lAcquiredDesktopImage;
	if (isScalingEnabled())
	{
		ID3D11Texture2D* scaledFrame = scaleTexture(_lAcquiredDesktopImage);
		if (scaledFrame != nullptr)
		{
			frameToSubmit = scaledFrame;
		}
	}

	ParsecHostD3D11SubmitFrame(ps, 0, _device, _context, frameToSubmit);

	_deskDupl->ReleaseFrame();

	_mutex.unlock();
	return true;
}

vector<string> DX11::listScreens()
{
	return _screens;
}

void DX11::setScreen(UINT index)
{
	if (index < _screens.size())
	{
		_currentScreen = index;
		Config::cfg.video.monitor = index;
		Config::cfg.Save();
		clearAndRecover();
	}
}

UINT DX11::getScreen()
{
	return _currentScreen;
}

vector<string> DX11::listGPUs()
{
	return _gpuNames;
}

void DX11::setGPU(size_t index)
{
	if (index < _gpus.size())
	{
		_mutex.lock();
		_currentGPU = index;
		Config::cfg.video.adapter = index;
		Config::cfg.Save();
		fetchScreenList();
		init();
		_mutex.unlock();
	}
}

size_t DX11::getGPU()
{
	return _currentGPU;
}

void DX11::fetchScreenList()
{
	HRESULT hr;

	IDXGIDevice* lDxgiDevice = 0;
	hr = _device->QueryInterface(__uuidof(IDXGIDevice), (void**)&lDxgiDevice);

	if (FAILED(hr))
	{
		if (lDxgiDevice != nullptr) lDxgiDevice->Release();
		return;
	}

	IDXGIAdapter* lDxgiAdapter;
	hr = lDxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&lDxgiAdapter);

	if (FAILED(hr))
	{
		if (lDxgiAdapter != nullptr) lDxgiAdapter->Release();
		return;
	}

	lDxgiDevice->Release();

	_screens.clear();
	UINT screen = 0;
	IDXGIOutput* lDxgiOutput;
	while (lDxgiAdapter->EnumOutputs(screen, &lDxgiOutput) != DXGI_ERROR_NOT_FOUND)
	{
		hr = lDxgiOutput->GetDesc(&_lOutputDesc);

		if (FAILED(hr))
		{
			_screens.push_back(
				string() + "[" + to_string(screen) + "] " + "Unknown Adapter"
			);
			screen++;
			continue;
		}

		std::string name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(_lOutputDesc.DeviceName);
		int width = _lOutputDesc.DesktopCoordinates.right - _lOutputDesc.DesktopCoordinates.left;
		int height = _lOutputDesc.DesktopCoordinates.bottom - _lOutputDesc.DesktopCoordinates.top;
		_screens.push_back("[" + to_string(screen) + "] " + name + " (" + to_string(width) + "x" + to_string(height) + ")");

		lDxgiOutput->Release();

		screen++;
	}

	lDxgiAdapter->Release();
}

ID3D11Device* DX11::getDevice()
{
	return _device;
}

ID3D11DeviceContext* DX11::getContext()
{
	return _context;
}

IDXGIOutputDuplication* DX11::getDuplication()
{
	return _deskDupl;
}

bool DX11::createDuplication()
{
	HRESULT hr;

	if (!_device) return false;

	IDXGIDevice* lDxgiDevice;
	hr = _device->QueryInterface(__uuidof(IDXGIDevice), (void**)&lDxgiDevice);
	if (FAILED(hr)) return false;

	IDXGIAdapter* lDxgiAdapter;
	hr = lDxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&lDxgiAdapter);
	lDxgiDevice->Release();
	if (FAILED(hr)) return false;

	IDXGIOutput* lDxgiOutput;
	hr = lDxgiAdapter->EnumOutputs(_currentScreen, &lDxgiOutput);
	lDxgiAdapter->Release();
	if (FAILED(hr)) return false;

	hr = lDxgiOutput->GetDesc(&_lOutputDesc);
	if (FAILED(hr)) {
		lDxgiOutput->Release();
		return false;
	}

	IDXGIOutput1* lDxgiOutput1;
	hr = lDxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&lDxgiOutput1);
	lDxgiOutput->Release();
	if (FAILED(hr)) return false;

	hr = lDxgiOutput1->DuplicateOutput(_device, &_deskDupl);
	lDxgiOutput1->Release();
	if (FAILED(hr)) return false;

	_deskDupl->GetDesc(&_lOutputDuplDesc);
	_desc.Width = _lOutputDuplDesc.ModeDesc.Width;
	_desc.Height = _lOutputDuplDesc.ModeDesc.Height;
	_desc.Format = _lOutputDuplDesc.ModeDesc.Format;
	_desc.ArraySize = 1;
	_desc.BindFlags = 0;
	_desc.MiscFlags = 0;
	_desc.SampleDesc.Count = 1;
	_desc.SampleDesc.Quality = 0;
	_desc.MipLevels = 1;
	_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	_desc.Usage = D3D11_USAGE_STAGING;

	return true;
}

vector<uint8_t> DX11::_convertTextureToBmp(ID3D11Texture2D* texture, int& width, int& height)
{
    D3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);

    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;

    ID3D11Texture2D* stagingTexture = nullptr;
    HRESULT hr = _device->CreateTexture2D(&desc, NULL, &stagingTexture);
    if (FAILED(hr)) {
        return {};
    }

    _context->CopyResource(stagingTexture, texture);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    hr = _context->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mappedResource);
    if (FAILED(hr)) {
        stagingTexture->Release();
        return {};
    }

    width = desc.Width;
    height = desc.Height;
    const int bytesPerPixel = 4; // BGRA

    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;

    bmfh.bfType = 0x4D42; // 'BM'
    bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (width * height * bytesPerPixel);
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth = width;
    bmih.biHeight = -height; // top-down
    bmih.biPlanes = 1;
    bmih.biBitCount = 32;
    bmih.biCompression = BI_RGB;
    bmih.biSizeImage = 0;
    bmih.biXPelsPerMeter = 0;
    bmih.biYPelsPerMeter = 0;
    bmih.biClrUsed = 0;
    bmih.biClrImportant = 0;

    std::vector<uint8_t> bmpData(bmfh.bfSize);
    uint8_t* pData = bmpData.data();

    memcpy(pData, &bmfh, sizeof(bmfh));
    pData += sizeof(bmfh);
    memcpy(pData, &bmih, sizeof(bmih));
    pData += sizeof(bmih);

    uint8_t* src = reinterpret_cast<uint8_t*>(mappedResource.pData);
    for (int y = 0; y < height; ++y) {
        memcpy(pData, src + y * mappedResource.RowPitch, width * bytesPerPixel);
        pData += width * bytesPerPixel;
    }

    _context->Unmap(stagingTexture, 0);
    stagingTexture->Release();

    return bmpData;
}

vector<uint8_t> DX11::getScreenshot(int& width, int& height)
{
    ID3D11Texture2D* textureToCopy = nullptr;

    _mutex.lock();

    // Scenario 1: Hosting is active.
    if (_lAcquiredDesktopImage) {
        textureToCopy = _lAcquiredDesktopImage;
        textureToCopy->AddRef();
        _mutex.unlock();
    }
    // Scenario 2: Hosting is not active.
    else {
        _mutex.unlock(); // Unlock before loop

		for (int i = 0; i < 10; i++) {
			_mutex.lock();

			if (!_deskDupl) {
				_mutex.unlock();
				if (!clearAndRecover()) return {};
				_mutex.lock();
				if (!_deskDupl) { _mutex.unlock(); return {}; }
			}

			DXGI_OUTDUPL_FRAME_INFO frameInfo;
			IDXGIResource* desktopResource = nullptr;
			HRESULT hr = _deskDupl->AcquireNextFrame(100, &frameInfo, &desktopResource);

			if (SUCCEEDED(hr) && frameInfo.LastPresentTime.QuadPart > 0) {
				hr = desktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&textureToCopy));
				desktopResource->Release();
				_deskDupl->ReleaseFrame();
				_mutex.unlock();
				
				if (SUCCEEDED(hr)) {
					// SUCCESS CASE
					vector<uint8_t> bmpData = _convertTextureToBmp(textureToCopy, width, height);
					textureToCopy->Release();
					return bmpData;
				}
			} else {
				// FAILURE CASE
				if (desktopResource) desktopResource->Release();
				_deskDupl->ReleaseFrame();
				_mutex.unlock();
				if (hr != DXGI_ERROR_WAIT_TIMEOUT) {
					clearAndRecover();
				}
			}
			
			Sleep(30);
		}

		// Looped 10 times and failed every time.
		return {};
    }

    if (!textureToCopy) {
        return {};
    }

    vector<uint8_t> bmpData = _convertTextureToBmp(textureToCopy, width, height);
    textureToCopy->Release();
    return bmpData;
}

// Resolution scaling methods
bool DX11::setTargetResolution(int width, int height)
{
	_mutex.lock();
	
	// If same as current, do nothing
	if (_targetWidth == width && _targetHeight == height && _scalingInitialized)
	{
		_mutex.unlock();
		return true;
	}
	
	// Setting to 0,0 disables scaling
	if (width <= 0 || height <= 0)
	{
		_targetWidth = 0;
		_targetHeight = 0;
		releaseScaledRenderTarget();
		_mutex.unlock();
		return true;
	}
	
	_targetWidth = width;
	_targetHeight = height;
	
	// Create the scaled render target
	bool result = createScaledRenderTarget(width, height);
	
	_mutex.unlock();
	return result;
}

void DX11::getTargetResolution(int& width, int& height)
{
	width = _targetWidth;
	height = _targetHeight;
}

void DX11::getNativeResolution(int& width, int& height)
{
	width = _desc.Width;
	height = _desc.Height;
}

bool DX11::isScalingEnabled()
{
	return _scalingInitialized && _targetWidth > 0 && _targetHeight > 0 && 
	       (_targetWidth != (int)_desc.Width || _targetHeight != (int)_desc.Height);
}

bool DX11::createScaledRenderTarget(int width, int height)
{
	// Release any existing resources
	releaseScaledRenderTarget();
	
	if (!_device || !_context)
		return false;
	
	HRESULT hr;
	
	// Create the scaled texture as a render target
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	
	hr = _device->CreateTexture2D(&texDesc, nullptr, &_scaledTexture);
	if (FAILED(hr))
		return false;
	
	// Create render target view
	hr = _device->CreateRenderTargetView(_scaledTexture, nullptr, &_scaledRTV);
	if (FAILED(hr))
	{
		releaseScaledRenderTarget();
		return false;
	}
	
	// Create sampler state for bilinear filtering
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	hr = _device->CreateSamplerState(&sampDesc, &_samplerState);
	if (FAILED(hr))
	{
		releaseScaledRenderTarget();
		return false;
	}
	
	// Compile vertex shader
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3DCompile(g_ScaleShaderCode, strlen(g_ScaleShaderCode), nullptr, nullptr, nullptr,
	                "VSMain", "vs_4_0", 0, 0, &vsBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob) errorBlob->Release();
		releaseScaledRenderTarget();
		return false;
	}
	
	hr = _device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &_vertexShader);
	if (FAILED(hr))
	{
		vsBlob->Release();
		releaseScaledRenderTarget();
		return false;
	}
	
	// Create input layout
	D3D11_INPUT_ELEMENT_DESC inputLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	hr = _device->CreateInputLayout(inputLayout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &_inputLayout);
	vsBlob->Release();
	if (FAILED(hr))
	{
		releaseScaledRenderTarget();
		return false;
	}
	
	// Compile pixel shader
	ID3DBlob* psBlob = nullptr;
	hr = D3DCompile(g_ScaleShaderCode, strlen(g_ScaleShaderCode), nullptr, nullptr, nullptr,
	                "PSMain", "ps_4_0", 0, 0, &psBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob) errorBlob->Release();
		releaseScaledRenderTarget();
		return false;
	}
	
	hr = _device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &_pixelShader);
	psBlob->Release();
	if (FAILED(hr))
	{
		releaseScaledRenderTarget();
		return false;
	}
	
	// Create vertex buffer for fullscreen quad
	struct Vertex { float x, y, u, v; };
	Vertex vertices[] = {
		{ -1.0f,  1.0f, 0.0f, 0.0f },  // Top-left
		{  1.0f,  1.0f, 1.0f, 0.0f },  // Top-right
		{ -1.0f, -1.0f, 0.0f, 1.0f },  // Bottom-left
		{  1.0f, -1.0f, 1.0f, 1.0f },  // Bottom-right
	};
	
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.ByteWidth = sizeof(vertices);
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	
	hr = _device->CreateBuffer(&bufDesc, &initData, &_vertexBuffer);
	if (FAILED(hr))
	{
		releaseScaledRenderTarget();
		return false;
	}
	
	_scalingInitialized = true;
	return true;
}

ID3D11Texture2D* DX11::scaleTexture(ID3D11Texture2D* sourceTexture)
{
	if (!_scalingInitialized || !sourceTexture || !_scaledTexture)
		return nullptr;
	
	HRESULT hr;
	
	// Create shader resource view for source texture
	D3D11_TEXTURE2D_DESC srcDesc;
	sourceTexture->GetDesc(&srcDesc);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = srcDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	
	// Release old SRV if exists
	SAFE_RELEASE(_sourceSRV);
	
	hr = _device->CreateShaderResourceView(sourceTexture, &srvDesc, &_sourceSRV);
	if (FAILED(hr))
		return nullptr;
	
	// Save current render target state
	ID3D11RenderTargetView* prevRTV = nullptr;
	ID3D11DepthStencilView* prevDSV = nullptr;
	_context->OMGetRenderTargets(1, &prevRTV, &prevDSV);
	
	D3D11_VIEWPORT prevViewport;
	UINT numViewports = 1;
	_context->RSGetViewports(&numViewports, &prevViewport);
	
	// Set up viewport for scaled texture
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)_targetWidth;
	viewport.Height = (float)_targetHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	_context->RSSetViewports(1, &viewport);
	
	// Set render target
	_context->OMSetRenderTargets(1, &_scaledRTV, nullptr);
	
	// Set up pipeline
	_context->IASetInputLayout(_inputLayout);
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	UINT stride = sizeof(float) * 4;
	UINT offset = 0;
	_context->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	
	_context->VSSetShader(_vertexShader, nullptr, 0);
	_context->PSSetShader(_pixelShader, nullptr, 0);
	_context->PSSetShaderResources(0, 1, &_sourceSRV);
	_context->PSSetSamplers(0, 1, &_samplerState);
	
	// Draw fullscreen quad
	_context->Draw(4, 0);
	
	// Unbind SRV to allow texture to be used elsewhere
	ID3D11ShaderResourceView* nullSRV = nullptr;
	_context->PSSetShaderResources(0, 1, &nullSRV);
	
	// Restore previous render target state
	_context->OMSetRenderTargets(1, &prevRTV, prevDSV);
	_context->RSSetViewports(1, &prevViewport);
	
	if (prevRTV) prevRTV->Release();
	if (prevDSV) prevDSV->Release();
	
	return _scaledTexture;
}