#pragma warning(push)
#pragma warning(disable: 4265 4946 4100)

#include "WGCapture.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>

#include <Windows.Graphics.Capture.Interop.h>
#include <windows.graphics.directx.direct3d11.interop.h>

#include <dxgi.h>
#include <atomic>

MIDL_INTERFACE("A9B3D012-3DF2-4EE3-B8D1-8695F457D3C1")
IDirect3DDxgiInterfaceAccess : public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE GetInterface(REFIID iid, void** p) = 0;
};

#pragma warning(pop)

using namespace winrt;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Graphics::Capture;
using namespace winrt::Windows::Graphics::DirectX;
using namespace winrt::Windows::Graphics::DirectX::Direct3D11;

static IDirect3DDevice CreateWinRTDevice(ID3D11Device* d3dDevice)
{
	IDXGIDevice* dxgiDevice = nullptr;
	HRESULT hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	if (FAILED(hr) || !dxgiDevice) return nullptr;

	IInspectable* inspectable = nullptr;
	hr = CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice, &inspectable);
	dxgiDevice->Release();
	if (FAILED(hr) || !inspectable) return nullptr;

	IDirect3DDevice device{ nullptr };
	inspectable->QueryInterface(guid_of<IDirect3DDevice>(), put_abi(device));
	inspectable->Release();
	return device;
}

static GraphicsCaptureItem CreateItemForMonitor(HMONITOR hmon)
{
	auto factory = get_activation_factory<GraphicsCaptureItem>();
	auto interop = factory.as<IGraphicsCaptureItemInterop>();

	GraphicsCaptureItem item{ nullptr };
	check_hresult(interop->CreateForMonitor(
		hmon,
		guid_of<GraphicsCaptureItem>(),
		put_abi(item)));
	return item;
}

struct WGCapture::Impl
{
	IDirect3DDevice winrtDevice{ nullptr };
	GraphicsCaptureItem item{ nullptr };
	Direct3D11CaptureFramePool framePool{ nullptr };
	GraphicsCaptureSession session{ nullptr };
	GraphicsCaptureItem::Closed_revoker closedRevoker;

	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* d3dContext = nullptr;
	ID3D11Texture2D* stagingTexture = nullptr;
	int frameWidth = 0;
	int frameHeight = 0;
	SizeInt32 lastItemSize{};

	std::atomic<bool> active{ false };

	void release()
	{
		active = false;
		closedRevoker.revoke();
		if (session)
		{
			session.Close();
			session = nullptr;
		}
		if (framePool)
		{
			framePool.Close();
			framePool = nullptr;
		}
		item = nullptr;
		winrtDevice = nullptr;
		if (stagingTexture)
		{
			stagingTexture->Release();
			stagingTexture = nullptr;
		}
		if (d3dContext)
		{
			d3dContext->Release();
			d3dContext = nullptr;
		}
		d3dDevice = nullptr;
		frameWidth = 0;
		frameHeight = 0;
	}

	bool ensureStagingTexture(int w, int h)
	{
		if (stagingTexture && frameWidth == w && frameHeight == h)
			return true;

		if (stagingTexture)
		{
			stagingTexture->Release();
			stagingTexture = nullptr;
		}

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = w;
		desc.Height = h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		HRESULT hr = d3dDevice->CreateTexture2D(&desc, nullptr, &stagingTexture);
		if (FAILED(hr)) return false;

		frameWidth = w;
		frameHeight = h;
		return true;
	}
};

WGCapture::WGCapture() : _impl(new Impl()) {}

WGCapture::~WGCapture()
{
	stop();
	delete _impl;
}

bool WGCapture::isSupported()
{
	try
	{
		return GraphicsCaptureSession::IsSupported();
	}
	catch (...)
	{
		return false;
	}
}

bool WGCapture::start(ID3D11Device* device, ID3D11DeviceContext* context, HMONITOR monitor)
{
	stop();

	try
	{
		_impl->d3dDevice = device;
		_impl->d3dContext = context;
		context->AddRef();

		_impl->winrtDevice = CreateWinRTDevice(device);
		if (!_impl->winrtDevice) { _impl->release(); return false; }

		_impl->item = CreateItemForMonitor(monitor);
		if (!_impl->item) { _impl->release(); return false; }

		auto size = _impl->item.Size();
		_impl->lastItemSize = size;

		_impl->framePool = Direct3D11CaptureFramePool::CreateFreeThreaded(
			_impl->winrtDevice,
			DirectXPixelFormat::B8G8R8A8UIntNormalized,
			2,
			size
		);

		_impl->session = _impl->framePool.CreateCaptureSession(_impl->item);

		try { _impl->session.IsBorderRequired(false); }
		catch (...) {}

		_impl->closedRevoker = _impl->item.Closed(auto_revoke,
			[this](auto&&, auto&&) { _impl->active = false; });

		_impl->session.StartCapture();
		_impl->active = true;
		return true;
	}
	catch (...)
	{
		_impl->release();
		return false;
	}
}

void WGCapture::stop()
{
	_impl->release();
}

bool WGCapture::isActive() const
{
	return _impl->active;
}

ID3D11Texture2D* WGCapture::acquireFrame()
{
	if (!_impl->active) return nullptr;

	try
	{
		auto frame = _impl->framePool.TryGetNextFrame();
		if (!frame)
			return _impl->stagingTexture;

		auto surface = frame.Surface();
		auto contentSize = frame.ContentSize();

		IDirect3DDxgiInterfaceAccess* access = nullptr;
		HRESULT hr = winrt::get_unknown(surface)->QueryInterface(
			__uuidof(IDirect3DDxgiInterfaceAccess), (void**)&access);
		
		ID3D11Texture2D* frameTexture = nullptr;
		if (SUCCEEDED(hr) && access)
		{
			hr = access->GetInterface(IID_PPV_ARGS(&frameTexture));
			access->Release();
		}

		if (SUCCEEDED(hr) && frameTexture)
		{
			int w = contentSize.Width;
			int h = contentSize.Height;

			if (_impl->ensureStagingTexture(w, h))
			{
				D3D11_BOX box = {};
				box.right = (UINT)w;
				box.bottom = (UINT)h;
				box.back = 1;

				_impl->d3dContext->CopySubresourceRegion(
					_impl->stagingTexture, 0, 0, 0, 0,
					frameTexture, 0, &box);
			}
			frameTexture->Release();
		}

		frame.Close();

		auto itemSize = _impl->item.Size();
		if (contentSize.Width != itemSize.Width || contentSize.Height != itemSize.Height)
		{
			_impl->framePool.Recreate(
				_impl->winrtDevice,
				DirectXPixelFormat::B8G8R8A8UIntNormalized,
				2,
				itemSize);
			_impl->lastItemSize = itemSize;
		}

		return _impl->stagingTexture;
	}
	catch (...)
	{
		_impl->active = false;
		return nullptr;
	}
}

void WGCapture::getFrameSize(int& width, int& height) const
{
	width = _impl->frameWidth;
	height = _impl->frameHeight;
}
