#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef SAFE_CALL
#define SAFE_CALL(fn) fn
#endif

#ifndef E
#define E(s) s
#endif

using PresentFn = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
