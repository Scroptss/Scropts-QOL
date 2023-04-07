#include "includes.h"
#include <string>
#include "font.h"
#include <sstream>
#include <vector>
#include "host.h"
#include <TlHelp32.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//Present oPresent;
void* oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;


bool bForceHost;
bool bForceHostRan;
bool open = true;
bool bDivinium;
bool bDiviniumSpend;
bool bCrypto;
bool bCryptoSpend;
int UnlockTMR = clock();


bool should_ignore_msg(UINT msg)
{
	switch (msg)
	{
	case WM_MOUSEACTIVATE:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_MOUSEMOVE:
	case WM_NCHITTEST:
	case WM_MOUSEWHEEL:
	case WM_MOUSEHOVER:
	case WM_ACTIVATEAPP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_SETCURSOR:
		return true;
	default:
		return false;
	}
}

bool is_user_in_game()
{
	return Live_IsUserInGame(0);
}

bool local_client_is_in_game()
{
	return CL_IsLocalClientInGame(0);
}

bool is_in_game()
{
	return Com_IsInGame();
}

bool is_match_loaded()
{
	return (is_user_in_game() && local_client_is_in_game() && is_in_game());
}

bool AreWeInGameAndHosting() {

	bool rt = false;

	if (is_match_loaded() && Live_AreWeHost()) {
		return true;
	}
	return rt;

}

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	io.Fonts->AddFontFromMemoryTTF(CustomFont, 1, 16);
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	const auto should_ignore{ open && !ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) };

	if (should_ignore && should_ignore_msg(uMsg))
	{
		return true;
	}

	return CallWindowProcA(oWndProc, hWnd, uMsg, wParam, lParam);
}

std::vector<std::string> split(std::string str, char delimiter) {
	std::vector<std::string> internal;
	std::stringstream ss(str);
	std::string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	return internal;
}

bool is_open() {
	return open;
}

inline void RenamePreGame(std::string name)
{
	if (name.size() > 31)
		name.resize(31);

	strncpy_s((char*)(pUserData + 0x8), 32, name.c_str(), name.size());
	strncpy_s((char*)(pNameBuffer), 32, name.c_str(), name.size());
}

void rename_player(std::string nameBuffer) {
	strncpy_s(spoofName, sizeof(spoofName), nameBuffer.data(), -1);
	RenamePreGame(spoofName);
}

void draw() {
		

	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	vec2_t res = { desktop.right, desktop.bottom };

	auto& style = ImGui::GetStyle();

	ImGui::SetNextWindowSize(ImVec2(548.0f, 620.0f), ImGuiCond_Once);

	style.Colors[ImGuiCol_Separator] = ImColor(225, 225, 225, 225);
	style.Colors[ImGuiCol_SeparatorActive] = ImColor(225, 225, 225, 255);
	style.Colors[ImGuiCol_SeparatorHovered] = ImColor(97, 97, 97, 199);
	style.Colors[ImGuiCol_PlotLines] = ImColor(210, 210, 210, 255);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImColor(255, 200, 0, 255);
	style.Colors[ImGuiCol_PlotHistogram] = ImColor(210, 210, 210, 255);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImColor(255, 200, 0, 255);
	style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
	style.Colors[ImGuiCol_Border] = ImColor(225, 225, 225, 225);
	style.Colors[ImGuiCol_WindowBg] = ImColor(19, 19, 19, 255);
	style.Colors[ImGuiCol_TitleBg] = ImColor(0, 0, 0, 110);
	style.Colors[ImGuiCol_TitleBgActive] = ImColor(0, 0, 0, 110);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 110);
	style.Colors[ImGuiCol_Button] = ImColor(20, 20, 20, 200);			
	style.Colors[ImGuiCol_ButtonActive] = ImColor(140, 140, 140, 255);	
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(56, 56, 56, 255);
	style.Colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255, 255);
	style.Colors[ImGuiCol_FrameBg] = ImColor(255, 255, 255, 14);
	style.Colors[ImGuiCol_FrameBgActive] = ImColor(0, 0, 0, 255);
	style.Colors[ImGuiCol_FrameBgHovered] = ImColor(18, 18, 18, 255);
	style.Colors[ImGuiCol_Header] = ImColor(255, 255, 255, 79);			
	style.Colors[ImGuiCol_HeaderActive] = ImColor(161, 161, 161, 255);
	style.Colors[ImGuiCol_HeaderHovered] = ImColor(181, 181, 181, 204);
	style.Colors[ImGuiCol_ResizeGrip] = ImColor(24, 24, 24, 255);		
	style.Colors[ImGuiCol_ResizeGripActive] = ImColor(24, 24, 24, 255);	
	style.Colors[ImGuiCol_ResizeGripHovered] = ImColor(24, 24, 24, 255);
	style.Colors[ImGuiCol_SliderGrab] = ImColor(255, 255, 255, 255);
	style.Colors[ImGuiCol_SliderGrabActive] = ImColor(173, 173, 173, 255);
	style.Colors[ImGuiCol_Tab] = ImColor(85, 85, 85, 220);
	style.Colors[ImGuiCol_MenuBarBg] = ImColor(36, 36, 36, 255);
	style.Colors[ImGuiCol_TabHovered] = ImColor(131, 131, 131, 204);
	style.Colors[ImGuiCol_TabActive] = ImColor(181, 181, 181, 255);
	style.Colors[ImGuiCol_TabUnfocused] = ImColor(150, 150, 150, 248);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImColor(35, 67, 108, 255);
	style.Colors[ImGuiCol_TextSelectedBg] = ImColor(100, 100, 100, 255);


	style.WindowPadding = ImVec2(8, 8);
	style.WindowRounding = 12;
	style.FramePadding = ImVec2(5, 4);
	style.FrameRounding = 5.0f;
	style.ItemSpacing = ImVec2(3, 4);
	style.ItemInnerSpacing = ImVec2(3, 4);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 21.0f;
	style.ScrollbarSize = 10.0f;
	style.ScrollbarRounding = 5.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 5.0f;
	style.WindowTitleAlign.x = 0.50f;
	style.FrameBorderSize = 0.0f;
	style.WindowBorderSize = 2.0f;
	style.ChildBorderSize = 1.0f;
	style.ItemInnerSpacing = ImVec2(3, 4);
	style.TabRounding = 5.0f;

	

	if (is_open()) {


		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		ImDrawList* drawlist = ImGui::GetBackgroundDrawList();
		drawlist->AddRectFilled(ImVec2(0, 0), ImVec2(desktop.right, desktop.bottom), IM_COL32(0, 0, 0, 150));

	}

	if (is_open()) {

		*(int*)((DWORD64)GetModuleHandleA(NULL) + 0x17E6F335) = 0; //

		ImGui::SetNextWindowSize(ImVec2(500.0f, 300.0f), ImGuiCond_Once);

		ImGui::Begin("QOL for BO3 - Scropts (F5)", &open);

		ImGui::BeginTabBar("##main");

		if (ImGui::BeginTabItem("Zombies")) {

			ImGui::BeginChild("ZM##ZM", ImGui::GetContentRegionAvail());

			ImGui::Text("Host Status: ");
			ImGui::SameLine();
			if (AreWeInGameAndHosting()) {
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Host");
			}
			else {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Client");
			}

			if (ImGui::Button("Force Host")) {
				bForceHost = !bForceHost;
			}

			if (bForceHost && !bForceHostRan) {
				host::setforcehostdvars(true);
				bForceHostRan = true;
			}

			if (!bForceHost && bForceHostRan) {
				host::setforcehostdvars(false);
				bForceHostRan = false;
			}

			ImGui::SameLine();
			ImGui::Dummy(ImVec2(5, 0));
			ImGui::SameLine();
			if (bForceHost) {
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Enabled");
			}
			else {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Disabled");
			}

			ImGui::Checkbox("Loop Divinium", &bDivinium);

			ImGui::SameLine();

			ImGui::Dummy(ImVec2(10, 0));

			ImGui::SameLine();

			ImGui::Checkbox("Spend Divinium", &bDiviniumSpend);

			const char* GobblegumPackIndex[10]{
					"Pack 1",
					"Pack 2",
					"Pack 3",
					"Pack 4",
					"Pack 5",
					"Pack 6",
					"Pack 7",
					"Pack 8",
					"Pack 9",
					"Pack 10"
			};

			static int BuffIndex;
			static int PackIndex;
			static int GobbleId;

			ImGui::SetNextItemWidth(250);
			ImGui::Combo("Gobblegum Pack", &PackIndex, GobblegumPackIndex, IM_ARRAYSIZE(GobblegumPackIndex));

			const char* GobblegumBuffIndex[5]{
				"Slot 1",
				"Slot 2",
				"Slot 3",
				"Slot 4",
				"Slot 5"
			};

			ImGui::SetNextItemWidth(250);
			ImGui::Combo("Slot", &BuffIndex, GobblegumBuffIndex, IM_ARRAYSIZE(GobblegumBuffIndex));

			ImGui::InputInt("Item ID##GOBBLE", &GobbleId);

			if (ImGui::Button("Set Gobblegum"))
			{

				CACRoot Buffer;
				eGameModes GameMode = Com_SessionMode_GetGameMode();
				eModes Mode = Com_SessionMode_GetMode();
				CACType cacType = LiveStats_Loadouts_GetCACTypeForMode(Mode, GameMode);
				auto CACroot = LiveStats_Loadouts_GetCACRoot(&Buffer, 0, cacType);
				SetGobblegum(&Buffer, PackIndex, BuffIndex, GobbleId);
			}

			ImGui::EndChild();

			ImGui::EndTabItem();

		}

		if (ImGui::BeginTabItem("Multiplayer")) {

			ImGui::BeginChild("MP", ImGui::GetContentRegionAvail());

			if (ImGui::Button("Set Godmode Specialist")) {

				((__int64(__cdecl*)(unsigned int ControllerIndex, unsigned int eModes, unsigned int statsLocation, unsigned int CharacterIndex))(ProcessBase + 0xAD860))(0, 1, 0, 10);
				//SetCharacterIndex(0, 1, 10);
			}

			if (ImGui::Button("Launch MP Game")) {
				Cbuf_AddText(0, "launchgame");
			}

			/*ImGui::SameLine();*/

			if (ImGui::Button("Make Custom Game Earn XP")) {

				if (AreWeInGameAndHosting()) {

					__int64 sSessionModeState = ProcessBase + 0x168ED7F4;
					*(__int32*)sSessionModeState = 129;
					hooks::Notify(0, "XP Earned in this match will stick.");
				}
				else {
					hooks::Notify(0, "You must be hosting a custom game!");
				}

			}

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Make sure you enable this AFTER the 10 second countdown!");
			}

			const char* characters[11]{
			"Ruin",
			"Outrider",
			"Prophet",
			"Battery",
			"Seraph",
			"Nomad",
			"Reaper",
			"Spectre",
			"Firebreak",
			"Blackjack",
			"GodMode"
			};

			static int weaponIndex = 0;
			static int camoIndex = 0;
			static int characterIndex = 0;
			static int headIndex = 0;

			//ImGui::InputInt("Character Index", &characterIndex);
			ImGui::SetNextItemWidth(250);
			ImGui::Combo("Specialist", &characterIndex, characters, IM_ARRAYSIZE(characters));
			//ImGui::InputInt("Character Head Index", &headIndex);
			ImGui::SetNextItemWidth(250);
			ImGui::InputInt("Showcase Weapon ID", &weaponIndex);
			ImGui::SetNextItemWidth(250);
			ImGui::InputInt("Showcase Weapon Camo ID", &camoIndex);

			if (ImGui::Button("Set Showcase Weapon")) {

				static auto DDLContextForLocation = ((DDLContext * (__cdecl*)(__int64 ControllerIndex, unsigned int sessionMode, int location))(ProcessBase + 0x1EAA3B0))(0, MODE_MULTIPLAYER, CUSTOMIZATION_STATS_LOCATION_STATS);

				Variant setVariant{};
				setVariant.weaponIndex = weaponIndex;
				setVariant.camoIndex = camoIndex;
				setVariant.paintjobIndex = -1;
				setVariant.paintjobSlot = -1;


				LiveStats_SetShowcaseWeapon(MODE_MULTIPLAYER, CUSTOMIZATION_STATS_LOCATION_STATS, DDLContextForLocation, CAC_TYPE_INVALID, characterIndex, setVariant);
			}

			ImGui::Checkbox("Loop Cryptokeys", &bCrypto);

			ImGui::SameLine();

			ImGui::Dummy(ImVec2(10, 0));

			ImGui::SameLine();

			ImGui::Checkbox("Spend Cryptokeys", &bCryptoSpend);

			ImGui::Dummy(ImVec2(0.0f, 2.0f));

			static int ItemId;
			static int ClassIndex;
			static int BuffIndex;
			static int PackIndex;

			ImGui::Combo("Class Index", &class_sel, customclass, IM_ARRAYSIZE(customclass));

			ImGui::InputInt("Item ID", &ItemId);

			ImGui::Combo("Slot Name", &slotItem, slots, IM_ARRAYSIZE(slots));

			ClassIndex = class_sel;

			if (ImGui::Button("Set Class Item"))
			{

				CACRoot Buffer{};
				eGameModes GameMode = Com_SessionMode_GetGameMode();
				eModes Mode = Com_SessionMode_GetMode();
				CACType cacType = LiveStats_Loadouts_GetCACTypeForMode(Mode, GameMode);
				auto CACroot = LiveStats_Loadouts_GetCACRoot(&Buffer, 0, cacType);
				__int64 LoadoutSlot = BG_UnlockablesGetLoadoutSlotFromString(slots[slotItem]);
				SetClass(&Buffer, (unsigned int)ClassIndex, LoadoutSlot, ItemId);

			}

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Make sure you are in the Create-A-Class menu.");
			}

			ImGui::EndChild();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Tools")) {

			ImGui::Dummy(ImVec2(5, 5));

			static std::string dvar_input = "";

			bool bDvar = ImGui::InputTextWithHint("##DVAR", "DVAR", &dvar_input, ImGuiInputTextFlags_EnterReturnsTrue);

			ImGui::SameLine();

			if (ImGui::Button("Send##DVAR") || bDvar)
			{
				std::vector<std::string> sep = split(dvar_input, ' ');

				if (!sep[0].empty())
					Dvar_SetFromString(sep[0].c_str(), sep[1].c_str(), true);
				else {
					hooks::Notify(4, "Insert a Valid Dvar.");
				}
			}

			static std::string cbuf_input = "";

			bool bCbuf = ImGui::InputTextWithHint("##CBUF", "CBUF", &cbuf_input, ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();

			if (ImGui::Button("Send##CBUF") || bCbuf)
			{
				Cbuf_AddText(0, cbuf_input.data());
			}

			static std::string clan_input;
			//ImGui::SetNextItemWidth(250);
			ImGui::InputTextWithHint("##CLAN", "CLAN", &clan_input);
			ImGui::SameLine();
			if (ImGui::Button("Set##CLAN"))
			{
				if (clan_input.data() != "")
					LiveStats_SetClanTagText(0, clan_input.data());
			}

			ImGui::InputTextWithHint("##NAME", "SPOOF-NAME", &nameBuffer);
			ImGui::SameLine();

			if (ImGui::Button("Set##NAME"))
			{
				if (nameBuffer != "")
					rename_player(nameBuffer);
			}			

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Info")) {

			ImGui::Text("For a list of Item IDs and other information please join the discord below.");

			ImGui::Dummy(ImVec2(10, 10));
			ImGui::Text("Server: ");
			//ImGui::SameLine();
			std::string ServerText = "discord.gg/GVK2jTbAnd";
			ImGui::InputText("##Server", &ServerText,ImGuiInputTextFlags_ReadOnly);
			//ImGui::TextColored(ImVec4(0, 1, 0, 1), "discord.gg/GVK2jTbAnd");
			ImGui::SameLine();			

			ImGui::Dummy(ImVec2(10, 10));
						

			ImGui::Dummy(ImVec2(10, 10));
			ImGui::Text("Version: 1.9");

			ImGui::EndTabItem();
		}

		
		ImGui::EndTabBar();		

		ImGui::End();
	}

	else {
		*(int*)((DWORD64)GetModuleHandleA(NULL) + 0x17E6F335) = 1; // ur mom
	}
}

bool hGetDLCCheck() {
	return true;
}

void DLCSpoof() {

	DWORD oldprotect;
	__int64 vTable = *(__int64*)(ProcessBase + 0x10BBCBC0);
	VirtualProtect((LPVOID)(*(__int64*)vTable + 56i64), 8, PAGE_EXECUTE_READWRITE, &oldprotect);
	*(__int64*)(*(__int64*)vTable + 56i64) = (__int64)hGetDLCCheck;

}



__int64 hLiveSteam_FilterPersonaName(char* buffer, int size, char asciionly)
{
	Unhook(oLiveSteam_FilterPersonaName, hLiveSteam_FilterPersonaName);
	return 0;
}

char* hGetPersonaName(DWORD_PTR _this)
{
	if (spoofName[0] != 0)
	{
		if ((DWORD_PTR)_ReturnAddress() == pGetPersonaNameReturn)
		{
			Hook(oLiveSteam_FilterPersonaName, hLiveSteam_FilterPersonaName);
		}

		return spoofName;
	}

	return oGetPersonaName(_this);
}

void on_frame()
{
	static bool en = false;

	static bool enabled;

	if (is_match_loaded() && !enabled)
	{
		/*MH_CreateHook((void*)(dwProcessBase + 0x143A600), hkdwInstantDispatchMessage, (void**)&dwInstantDispatchMessage);
		MH_EnableHook((void*)(dwProcessBase + 0x143A600));*/

		/*MH_CreateHook((void*)(dwProcessBase + 0x1337360), hkCg_Obituary, (void**)&Cg_Obituary);
		MH_EnableHook((void*)(dwProcessBase + 0x1337360));*/

		enabled = true;
	}
	else if (!is_in_game() && enabled)
	{
		/*MH_RemoveHook((void*)(dwProcessBase + 0x143A600));
		MH_DisableHook((void*)(dwProcessBase + 0x143A600));*/

		//MH_RemoveHook((void*)(dwProcessBase + 0x1337360));//Cl_DeathMessagePrint
		//MH_DisableHook((void*)(dwProcessBase + 0x1337360));

		enabled = false;

	}

}

void initializeHook() {

	//Hooks hook;

	//hook.Initialize();

	THREADENTRY32 te32;
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (hThreadSnap)
	{
		te32.dwSize = sizeof(THREADENTRY32);

		if (!Thread32First(hThreadSnap, &te32))
		{
			CloseHandle(hThreadSnap);
		}
		else {
			do
			{
				if (te32.th32OwnerProcessID == GetCurrentProcessId() && te32.th32ThreadID != GetCurrentThreadId())
				{
					HANDLE hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, 0, te32.th32ThreadID);
					if (hThread)
					{
						CONTEXT context;
						context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
						SuspendThread(hThread);

						if (GetThreadContext(hThread, &context))
						{
							context.Dr0 = (uintptr_t)(ProcessBase + 0x1EC75D0); //HkShortenName
							context.Dr1 = (uintptr_t)(ProcessBase + 0x143A600); //hkDwInstantMessage							
							context.Dr7 = (1 << 0) | (1 << 2) | (1 << 4) | (1 << 6);

							SetThreadContext(hThread, &context);
						}

						ResumeThread(hThread);
						CloseHandle(hThread);
					}
				}
			} while (Thread32Next(hThreadSnap, &te32));
			CloseHandle(hThreadSnap);
		}
	}
	SetUnhandledExceptionFilter(hooks::hookHandler);
}


bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			DLCSpoof();			
			oGetPersonaName = (tGetPersonaName)HookVMT(ISteamFriends, (DWORD_PTR)&hGetPersonaName, 0);
			init = true;
		}

		else
			return decltype(&hkPresent)(oPresent)(pSwapChain, SyncInterval, Flags);
	}

	/*if (GetAsyncKeyState(VK_F1) & 1)
	{
		Cbuf_AddText(0, "give minigun");
	}

	if (GetAsyncKeyState(VK_F2) & 1)
	{
		Cbuf_AddText(0, "god");
	}

	if (GetAsyncKeyState(VK_F3) & 1)
	{
		Cbuf_AddText(0, "noclip");
	}

	if (GetAsyncKeyState(VK_F4) & 1)
	{
		Cbuf_AddText(0, "give ammo");
	}*/

	bMatchLoaded = is_match_loaded();

	if (GetAsyncKeyState(VK_F5) & 1)
	{
		open = !open;
	}

	on_frame();

	if (bDivinium) {
		if (clock() - UnlockTMR > 50) {
			char buf_cmd[255];
			sprintf_s(buf_cmd, "%c %u %u", 120, 3, 250);
			SV_GameSendServerCommand(0, 1, buf_cmd);
			UnlockTMR = clock();
		}
	}

	if (bCrypto) {		
		GiveLootToSelf(0, 1, 1500);
		UnlockTMR = clock();		
	}

	if (bCryptoSpend) {
		if (bCrypto) {
			bCrypto = false;
		}
		
		Loot_BuyCrate(0, 1, 2);
		
	}

	if (bDiviniumSpend) {
		/*if (bDivinium) {
			bDivinium = false;
		}*/
		
		Loot_SpendVials(0, 3);
		
	}

	ImGui::GetIO().MouseDrawCursor = open;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	draw();

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return decltype(&hkPresent)(oPresent)(pSwapChain, SyncInterval, Flags);
}

HookReturn initalize()
{
	__int64 DirectXThing = *(__int64*)(ProcessBase + 0xF4B6858);
	__int64 DirectXVtableThing = *(__int64*)(DirectXThing);
	oPresent = (void*)(*(__int64*)(DirectXVtableThing + 0x40));
	DWORD old;
	VirtualProtect((LPVOID)(DirectXVtableThing + 0x40), 8, PAGE_EXECUTE_READWRITE, &old);
	*(__int64*)(DirectXVtableThing + 0x40) = (__int64)hkPresent;
	VirtualProtect((LPVOID)(DirectXVtableThing + 0x40), 8, old, &old);

	return Success;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	dllbase = hMod;
	if (dwReason == DLL_PROCESS_ATTACH)
	{		
		initializeHook();
		initalize();
		MH_Initialize();
	}
	return TRUE;
}