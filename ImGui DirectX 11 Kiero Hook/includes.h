#pragma once
#include <Windows.h>
#include "defs.h"
#include <d3d11.h>
#include <dxgi.h>
#include <chrono>
#include <time.h>
#include <fstream>
#include <string>
#include <xstring>
#include <algorithm>
using namespace std::chrono_literals;
using namespace std::literals;
#include "utils.hpp"
#include "main_hook.h"
#include "detours/detours.h"
#pragma comment(lib,"detours/detours.lib")
#include <intrin.h>
#include "kiero/kiero.h"
#include "kiero/minhook/include/MinHook.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_stdlib.h"
#include "structs.h"
#include "common.h"
#include <cstdint>
#include <cstddef>
#include <wtypes.h>
#include <Psapi.h>

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;