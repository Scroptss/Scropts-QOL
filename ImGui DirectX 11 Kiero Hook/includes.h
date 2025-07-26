#pragma once
#include <Windows.h>
#include <cstdint>
#include <cstddef>
#include <wtypes.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <d3d11.h>
#include <dxgi.h>
#include "detours/detours.h"
#include <intrin.h>
#include <fstream>
#include <unordered_map>
#include "minhook/include/MinHook.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_stdlib.h"
#include "imgui/ImGuiNotify.hpp"
#include "spoof.h"
#include "structs.h"
#include "font.hpp"
#include "FontAwesome/fa_solid_900.h"
#include "FontAwesome/font_awesome_5.h"
#include "FontAwesome/IconsFontAwesome6.h"
#include "common.h"
#include "Utils.hpp"
#include "hooks.h"


typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;