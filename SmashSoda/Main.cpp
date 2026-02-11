// Dear ImGui: standalone example application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#define _WINSOCKAPI_

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_colors.h"
#include "ImGui/implot.h"
#include <d3d11.h>
#include <tchar.h>
#include <Windows.h>
#include <WinINet.h>
#include <iostream>
#include <string>
#include <SDL.h>
#include "resource.h"
#include "Hosting.h"
#include "Texture.h"
#include "Globals/AppIcons.h"
#include "Globals/AppFonts.h"
#include "Globals/AppColors.h"
#include "Widgets/LoginWidget.h"
#include "Widgets/NavBar.h"
#include "Widgets/HostInfoWidget.h"
#include "Widgets/HostSettingsWidget.h"
#include "Widgets/ChatWidget.h"
#include "Widgets/LogWidget.h"
#include "Widgets/GuestListWidget.h"
#include "Widgets/GamepadsWidget.h"
#include "Widgets/StylePickerWidget.h"
#include "Widgets/AudioSettingsWidget.h"
#include "Widgets/VideoWidget.h"
#include "Widgets/InfoWidget.h"
#include "Widgets/VersionWidget.h"
#include "Widgets/MasterOfPuppetsWidget.h"
#include "Widgets/SettingsWidget.h"
#include "Widgets/ButtonLockWidget.h"
#include "Widgets/LibraryWidget.h"
#include "Widgets/OverlayWidget.h"
#include "Widgets/HotseatWidget.h"
#include "Widgets/KeyboardMapWidget.h"
#include "Widgets/DeveloperWidget.h"
#include "Widgets/BackgroundWidget.h"
#include "widgets/StreamingWidget.h"
#include "services/ThemeController.h"
#include "services/ScreenshotService.h"

using namespace std;

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
Hosting g_hosting;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{

    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();

    // Load config
    Config::cfg.Load();

    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0L;
    wc.cbWndExtra = 0L;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = _T("Smash Soda");
    wc.hIconSm = NULL;
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(
        wc.lpszClassName, _T("Smash Soda"), WS_OVERLAPPEDWINDOW,
        Config::cfg.video.windowX, Config::cfg.video.windowY,
        Config::cfg.video.windowW, Config::cfg.video.windowH,
        NULL, NULL, wc.hInstance, NULL
    );

    g_hosting.mainWindow = hwnd;

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // =====================================================================
    // 
    //  Initialize modules
    // 
    // =====================================================================
    AppIcons::init(g_pd3dDevice);
    AppFonts::init(io);
    AppColors::init();
    g_hosting.init();

    // Apply default theme
    ThemeController::getInstance().applyTheme(Config::cfg.general.theme);

    HostSettingsWidget hostSettingsWindow(g_hosting, [&hwnd](bool isRunning) {
        SetWindowTextW(hwnd, isRunning ? L"⚫ [LIVE] Smash Soda" : L"Smash Soda");
        });
    LoginWidget loginWindow(g_hosting, hostSettingsWindow);
    LogWidget logWindow(g_hosting);
    GuestListWidget guestsWindow(g_hosting);
    GamepadsWidget gamepadsWindow(g_hosting);
    AudioSettingsWidget audioSettingswidget(g_hosting);
    VideoWidget videoWidget(g_hosting);
    HostInfoWidget hostInfoWidget(g_hosting);
    MasterOfPuppetsWidget masterOfPuppets;
    SettingsWidget settingsWidget(g_hosting);
    ButtonLockWidget buttonLockWidget(g_hosting);
    LibraryWidget libraryWidget(g_hosting);
    OverlayWidget overlayWidget(g_hosting);
    HotseatWidget hotseatWidget(g_hosting);
    VersionWidget versionWidget;
    DeveloperWidget developerWidget(g_hosting);
    BackgroundWidget backgroundWidget;
    StreamingWidget streamingWidget;

    //ChatWidget chatWindow(g_hosting);
    KeyboardMapWidget keyMapWidget(g_hosting); //-- CodeSomnia Add Start--

    FLASHWINFO fi;
    fi.cbSize = sizeof(FLASHWINFO);
    fi.hwnd = hwnd;
    fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
    fi.uCount = 0;
    fi.dwTimeout = 0;
    ChatWidget chatWindow(g_hosting, [&hwnd, &fi]() {
        if (Config::cfg.general.flashWindow) {
            FlashWindowEx(&fi);
        };

        if (Config::cfg.chat.messageNotification) {
            try {
                PlaySound(TEXT("./SFX/new_message.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
            }
            catch (const std::exception&) {}
        }
    });

    //ITaskbarList3* m_pTaskBarlist;
    //CoCreateInstance(
    //    CLSID_TaskbarList, NULL, CLSCTX_ALL,
    //    IID_ITaskbarList3, (void**)&m_pTaskBarlist);
    //m_pTaskBarlist->SetProgressState(hwnd, TBPF_ERROR);
    //m_pTaskBarlist->SetProgressValue(hwnd, 1, 2);

    ImVec4 clear_color = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    //clear_color = ImGui::loadStyle(0);

    bool showHostSettings = Config::cfg.widgets.host;
    bool showChat = Config::cfg.widgets.chat;
    bool showLog = Config::cfg.widgets.log;
    bool showGuests = Config::cfg.widgets.guests;
    bool showGamepads = Config::cfg.widgets.gamepads;
    bool showMasterOfPuppets = Config::cfg.widgets.masterOfPuppets;
    bool showAudio = Config::cfg.widgets.audio;
    bool showVideo = Config::cfg.widgets.video;
    bool showStyles = true;
    bool showInfo = false;
    bool showLogin = Cache::cache.showParsecLogin;
    bool showSettings = Config::cfg.widgets.settings;
    bool showButtonLock = Config::cfg.widgets.buttonLock;
    bool showLibrary = Config::cfg.widgets.library;
    bool showOverlay = Config::cfg.widgets.overlay;
    bool showHotseat = Config::cfg.widgets.hotseat;
    bool showKeyMap = Config::cfg.widgets.keyMapper; //-- CodeSomnia Add --
    bool showDevTools = Config::cfg.widgets.devTools;
	bool showStream = Config::cfg.widgets.stream;

    auto syncWidgetVisibility = [](bool show, bool& cfgValue) {
        if (cfgValue != show) {
            cfgValue = show;
            return true;
        }
        return false;
    };

    // =====================================================================
    //  Soda Arcade
    // =====================================================================
    if (Arcade::instance.loadCredentials()) {
        Arcade::instance.checkToken(Arcade::instance.credentials.token);
    }

    if (!Config::cfg.developer.skipUpdateCheck && Cache::cache.checkForUpdates()) {
        versionWidget.showUpdate = true;
    }

    // =====================================================================
    //  Register Hotkeys
    // =====================================================================
    for (int i = 0; i < Config::cfg.hotkeys.keys.size(); i++) {
        RegisterHotKey(hwnd, i, MOD_CONTROL|MOD_NOREPEAT, Config::cfg.hotkeys.keys[i].key);
    }

    // =====================================================================

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;

            // Hotkeys
            if (msg.message == WM_HOTKEY) {
                // Loop through Config::cfg.hotkey.keys (index is the hotkey id)
                for (int i = 0; i < Config::cfg.hotkeys.keys.size(); i++) {
                    if (msg.wParam == i) {
						g_hosting.sendHostMessage(Config::cfg.hotkeys.keys[i].command.c_str());
					}
				}
            }

        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // =====================================================================
        // 
        //  Window rendering
        // 
        // =====================================================================
        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

        versionWidget.render();
        if (versionWidget.showUpdate) {
            if (versionWidget.renderUpdateWindow()) {
                ::PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
        }
        else if (Config::cfg.arcade.showLogin) {
            versionWidget.renderLoginWindow();
        }
        else if (Config::cfg.overlay.update) {
            versionWidget.renderDownloadWindow();
        }

        if (Cache::cache.showParsecLogin) {
            loginWindow.render(Cache::cache.showParsecLogin);
        }
        else
        {
            if (showHostSettings)       hostSettingsWindow.render(showHostSettings, hwnd);
            if (showChat)               chatWindow.render(showChat);
            if (showLog)                logWindow.render(showLog);
            if (showGuests)             guestsWindow.render(showGuests);
            if (showGamepads)           gamepadsWindow.render(showGamepads);
            if (showMasterOfPuppets)    masterOfPuppets.render(showMasterOfPuppets);
            if (showAudio)              audioSettingswidget.render(showAudio);
            if (showVideo)              videoWidget.render(showVideo);
            if (showInfo)               InfoWidget::render(showInfo);
            if (showSettings)           settingsWidget.render(showSettings);
            if (showButtonLock)         buttonLockWidget.render(showButtonLock);
            if (showLibrary)            libraryWidget.render(showLibrary);
            if (showOverlay)            overlayWidget.render(showOverlay);
            if (showHotseat)            hotseatWidget.render(showHotseat);
            if (showDevTools)           developerWidget.render(showDevTools);
			if (showStream)             streamingWidget.render(showStream);

            //-- CodeSomnia Add Start--
            if (showKeyMap)
                keyMapWidget.render(showKeyMap);
            //-- CodeSomnia Add End--
            // 
            //-- CodeSomnia Moidified Start--

            NavBar::render(
                g_hosting,
                showLogin, showHostSettings, showGamepads, showMasterOfPuppets, showChat,
                showGuests, showLog, showAudio, showVideo, showInfo, showSettings,
                showButtonLock, showLibrary, showOverlay, showHotseat,
			    showKeyMap, showDevTools, showStream
            );

            //-- CodeSomnia Moidified End--
            hostInfoWidget.render();

            backgroundWidget.render();

            bool widgetsChanged = false;
            widgetsChanged |= syncWidgetVisibility(showHostSettings, Config::cfg.widgets.host);
            widgetsChanged |= syncWidgetVisibility(showChat, Config::cfg.widgets.chat);
            widgetsChanged |= syncWidgetVisibility(showLog, Config::cfg.widgets.log);
            widgetsChanged |= syncWidgetVisibility(showGuests, Config::cfg.widgets.guests);
            widgetsChanged |= syncWidgetVisibility(showGamepads, Config::cfg.widgets.gamepads);
            widgetsChanged |= syncWidgetVisibility(showMasterOfPuppets, Config::cfg.widgets.masterOfPuppets);
            widgetsChanged |= syncWidgetVisibility(showAudio, Config::cfg.widgets.audio);
            widgetsChanged |= syncWidgetVisibility(showVideo, Config::cfg.widgets.video);
            widgetsChanged |= syncWidgetVisibility(showSettings, Config::cfg.widgets.settings);
            widgetsChanged |= syncWidgetVisibility(showButtonLock, Config::cfg.widgets.buttonLock);
            widgetsChanged |= syncWidgetVisibility(showLibrary, Config::cfg.widgets.library);
            widgetsChanged |= syncWidgetVisibility(showOverlay, Config::cfg.widgets.overlay);
            widgetsChanged |= syncWidgetVisibility(showHotseat, Config::cfg.widgets.hotseat);
            widgetsChanged |= syncWidgetVisibility(showKeyMap, Config::cfg.widgets.keyMapper);
            widgetsChanged |= syncWidgetVisibility(showDevTools, Config::cfg.widgets.devTools);
            widgetsChanged |= syncWidgetVisibility(showStream, Config::cfg.widgets.stream);

            if (widgetsChanged) {
                Config::cfg.Save();
            }
        }

        //if (showStyles)         StylePickerWidget::render();

        //ImGui::ShowDemoWindow();

        // =====================================================================
        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        //g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
        static UINT presentFlags = 0;
        if (g_pSwapChain->Present(1, presentFlags) == DXGI_STATUS_OCCLUDED) {
            presentFlags = DXGI_PRESENT_TEST;
            Sleep(4);
        }
        else {
            presentFlags = 0;
        }
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    // Make sure all pads completely removed
    GamepadClient::instance.disconnectAllGamepads();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    // Unregister Hotkeys
    for (int i = 0; i < Config::cfg.hotkeys.keys.size(); i++) {
		UnregisterHotKey(hwnd, i+1);
	}

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    switch (msg)
    {
    case WM_MOVE:
        if (g_hosting.mainWindow == hWnd) {
			RECT windowRect;
            if (GetWindowRect(hWnd, &windowRect))
            {
				Config::cfg.video.windowX = windowRect.left;
				Config::cfg.video.windowY = windowRect.top;
			}
		}
		break;
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }

        if (g_hosting.mainWindow == hWnd) {
			RECT windowRect;
            if (GetWindowRect(hWnd, &windowRect))
            {
				Config::cfg.video.windowW = windowRect.right - windowRect.left;
				Config::cfg.video.windowH = windowRect.bottom - windowRect.top;
			}
		}
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_QUIT:
    case WM_DESTROY:
        g_hosting.release();
        RECT windowRect;
        /*if (GetWindowRect(hWnd, &windowRect))
        {
            Config::cfg.video.windowX = windowRect.left;
            Config::cfg.video.windowY = windowRect.top;
            Config::cfg.video.windowW = windowRect.right - windowRect.left;
            Config::cfg.video.windowH = windowRect.bottom - windowRect.top;
            Config::cfg.Save();
        }*/
        Config::cfg.Save();
        Sleep(1000);
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
