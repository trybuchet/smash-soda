#pragma once

// Standard Library
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <random>

// Windows
#define _WINSOCKAPI_
#include <winsock2.h>
#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <tchar.h>
#include <WinINet.h>

// ImGui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/implot.h"

// SDL
#include <SDL.h>

// Project Headers
#include "resource.h"
#include "Hosting.h"
#include "Texture.h"
#include "globals/AppIcons.h"
#include "globals/AppFonts.h"
#include "globals/AppColors.h"
