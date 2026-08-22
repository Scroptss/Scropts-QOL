#include "includes.h"
#include <string>
#include <sstream>
#include <vector>
#include "host.h"
#include "servers.h"
#include "players.h"
#include <d3dcompiler.h>
#include <wincodec.h>
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "windowscodecs.lib")

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

UINT OpenKeybind = VK_F5;
int selectedBind = 4;

using Present_t = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
using ResizeBuffers_t = HRESULT(__stdcall*)(
	IDXGISwapChain*,
	UINT,
	UINT,
	UINT,
	DXGI_FORMAT,
	UINT
	);

Present_t oPresent = nullptr;
ResizeBuffers_t oResizeBuffers = nullptr;

ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
inline ID3D11PixelShader* g_emblemMaskPixelShader = nullptr;
ID3D11RenderTargetView* mainRenderTargetView = nullptr;

IDXGISwapChain* gSwapChain = nullptr;
HWND window = nullptr;
WNDPROC oWndProc = nullptr;

bool init = false;
bool renderTargetReady = false;



typedef unsigned __int64 ull;
typedef ull             uint64;
#define _QWORD uint64

bool bDisclaimerShown = false;
bool bNotifications = true;
bool bForceHost;
bool bForceHostRan;
bool bFirstGumRan;

bool bClassEditor;
bool bDivinium;
bool bBurnDupes;
bool bBurnGum;
bool bLoginReward = true;
bool bDiviniumSpend;
bool bCrypto;
bool bCryptoSpend;
bool bProtectStats;
bool bProtectStatsRan;
bool bCompleteEE;
bool bArena;
int UnlockTMR = clock();
std::string sPackName;
std::string sEmblemName;
bool customImportRename;
std::string customImportName;
std::string EmblemRename;


int minRank = 0;
static int iCryptoAmt = 48;
static int icon = 0;
static int rankXp = 0;
static int pLevel = 0;
static int pPrestige = 0;
static int ParagonRank = 36;
static int paragonRankXp = 0;
static int iArenaPoints = 0;
static int iLootSpeed = 50;
static int iScore = 0;
static int iKills = 0;
static int iDeaths = 0;
static int iAssists = 0;
static int iHeadshots = 0;
static int iTeamKills = 0;
static int iSuicides = 0;
static int iTimePlayedAllies = 0;
static int iTimePlayedOpFor = 0;
static int iTimePlayedOther = 0;
static int iTimePlayedTotal = 0;
static int iKD = 0;
static int iWins = 0;
static int iLosses = 0;
static int iTies = 0;
static int iCurWinStreak = 0;
static int iWL = 0;
static int iHits = 0;
static int iMisses = 0;
static int iTotalShots = 0;
static int iAccuracy = 0;
static int iTotalGamesPlayed = 0;
static int iHighestRound = 0;
static int iTotalRounds = 0;

static int map = 0;
static int setRound = 1;
uintptr_t spoofAddr = 0;

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

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
		return true;
	default:
		return false;
	}
}

BOOLEAN MaskCompare(uintptr_t buffer, LPCSTR pattern, LPCSTR mask) {
	for (auto b = reinterpret_cast<PBYTE>(buffer); *mask; ++pattern, ++mask, ++b) {
		if (*mask == 'x' && *reinterpret_cast<LPCBYTE>(pattern) != *b) {
			return FALSE;
		}
	}

	return TRUE;
}

uintptr_t FindSpoof()
{

	if (spoofAddr <= 0) {
		MODULEINFO info = { 0 };
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));

		bool found = false;
		while (!found) {
			PVOID base = info.lpBaseOfDll;
			DWORD size = info.SizeOfImage;
			LPCSTR pattern = "\xFF\x26";
			LPCSTR mask = "xx";

			size -= static_cast<DWORD>(strlen(mask));

			for (auto i = 0UL; i < size; ++i) {
				auto addr = reinterpret_cast<uintptr_t>(base) + i;
				if (MaskCompare(addr, pattern, mask)) {
					uintptr_t relative = addr - ProcessBase;
					if (relative < (uintptr_t)(ProcessBase + 0x2000000))
						if (MaskCompare((uintptr_t)(addr - 0x2), "\xFF", "x") || MaskCompare((uintptr_t)(addr - 0x3), "\xFF", "x") || MaskCompare((uintptr_t)(addr - 0x3), "\xFF", "x") || MaskCompare((uintptr_t)(addr - 0x4), "\xFF", "x") || MaskCompare((uintptr_t)(addr - 0x5), "\xE8", "x") || MaskCompare((uintptr_t)(addr - 0x6), "\xFF", "x") || MaskCompare((uintptr_t)(addr - 0x7), "\xFF", "x")) {
							spoofAddr = addr;
							return addr;
						}
				}
			}
		}

		return 0;
	}
	return spoofAddr;
}

const void* spoof_t = (const void*)FindSpoof();

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();


	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
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

void drawTracers() {

	if (bTracers) {

		ImDrawList* drawlist = ImGui::GetBackgroundDrawList();

		for (auto& tracer : tracers)
		{
			if (tracer.opacity <= 0.0f) continue;

			ImColor main_color = ImColor(uiColor);
			if (bUIRgb) {
				main_color = mainRgb();
			}

			drawlist->AddLine(ImVec2(tracer.start_screen.x, tracer.start_screen.y), ImVec2(tracer.end_screen.x, tracer.end_screen.y), ImColor(main_color.Value.x, main_color.Value.y, main_color.Value.z, tracer.opacity), 1);

			drawlist->AddLine(ImVec2(tracer.end_screen.x - tracer.radius, tracer.end_screen.y - tracer.radius), ImVec2(tracer.end_screen.x + tracer.radius, tracer.end_screen.y + tracer.radius), ImColor(main_color.Value.x, main_color.Value.y, main_color.Value.z, tracer.opacity), 1);
			drawlist->AddLine(ImVec2(tracer.end_screen.x - tracer.radius, tracer.end_screen.y + tracer.radius), ImVec2(tracer.end_screen.x + tracer.radius, tracer.end_screen.y - tracer.radius), ImColor(main_color.Value.x, main_color.Value.y, main_color.Value.z, tracer.opacity), 1);


			tracer.opacity -= flTracerTime;
		}
	}
}

const char* getItemNameforItemID(int ItemId) {

	const char* ItemName = "";
	auto mode = Com_SessionMode_GetMode();
	auto getItemName = BG_UnlockablesGetItemName(mode, ItemId);
	if (getItemName != nullptr) {
		ItemName = UI_SafeTranslateString(getItemName);
	}

	return ItemName;

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

bool get_column_value_for_row_from_path(std::string path, std::int32_t row, std::int32_t column, std::int32_t* value_ptr)
{
	StringTable* string_table = nullptr;
	StringTable_GetAsset(path.c_str(), &string_table);

	if (value_ptr && string_table)
	{
		auto search_result = StringTable_GetColumnValueForRow(string_table, row, column);
		auto column_value = std::stoi(search_result);

		*value_ptr = column_value;
		return true;
	}

	return false;
}

std::string get_column_value_for_row_from_path(std::string path, std::int32_t row, std::int32_t column)
{
	StringTable* string_table = nullptr;
	StringTable_GetAsset(path.c_str(), &string_table);

	if (string_table)
	{
		std::string res = StringTable_GetColumnValueForRow(string_table, row, column);
		return res;
	}

	return "";
}

//void unlock_loot_items()
//{
//	LiveInventory_GetPlayerInventory(CONTROLLER_INDEX_FIRST)->itemFlags = 4;
//
//	for (auto i{ 0 }; i < LiveInventory_GetPlayerInventory(CONTROLLER_INDEX_FIRST)->itemCount; i++)
//	{
//		if (auto loot_item{ LiveInventory_GetPlayerInventoryItemId(CONTROLLER_INDEX_FIRST, i) }; loot_item)
//		{
//			loot_item->quantity = 1;
//		}
//	}
//}

void spoofPlayerInventory() {

	LiveInventory_GetPlayerInventory(CONTROLLER_INDEX_FIRST)->itemCount = 2914;

	for (auto i{ 0 }; i < 2890; i++)
	{
		if (i > 2865)
		{
			auto item_id = 0;
			if (get_column_value_for_row_from_path("gamedata/tables/common/inventory_items.csv", i - 2865, 2, &item_id))
			{
				LiveInventory_GetPlayerInventory(CONTROLLER_INDEX_FIRST)->lootItem[i].itemId = item_id;
				LiveInventory_GetPlayerInventory(CONTROLLER_INDEX_FIRST)->lootItem[i].quantity = iBlackmarketAmt;
			}
		}

		else if (i > 2818)
		{
			auto item_id = 0;
			if (get_column_value_for_row_from_path("gamedata/loot/zmlootitems.csv", i - 2818, 1, &item_id))
			{
				LiveInventory_GetPlayerInventory(CONTROLLER_INDEX_FIRST)->lootItem[i].itemId = item_id;
				LiveInventory_GetPlayerInventory(CONTROLLER_INDEX_FIRST)->lootItem[i].quantity = iBlackmarketAmt;
			}
		}

		else
		{
			auto item_id = 0;
			if (get_column_value_for_row_from_path("gamedata/loot/mplootitems.csv", i, 1, &item_id))
			{
				LiveInventory_GetPlayerInventory(CONTROLLER_INDEX_FIRST)->lootItem[i].itemId = item_id;
				LiveInventory_GetPlayerInventory(CONTROLLER_INDEX_FIRST)->lootItem[i].quantity = iBlackmarketAmt;
			}
		}
	}
}

int getComboItemSize(bool isPrimary) {

	switch (iComboBoxType) {
	case CBC_WEAPONTYPE:
		return 131;
		break;
	case CBC_ATTACHMENTS: {
		int itemIndex = secondarySelectedItems[0];

		if (isPrimary) {
			itemIndex = primarySelectedItems[0];
		}
		return BG_UnlockablesGetNumItemAttachments(Com_SessionMode_GetMode(), itemIndex);
		break;
	}
	case CBC_CAMOS:
		return 138;
		break;
	case CBC_RETICLES:
		return 40;
		break;
	case CBC_PERK:
		return 22;
		break;
	case CBC_BONUSCARD:
		return 9;
		break;
	case CBC_KILLSTREAK:
		return 28;
		break;
	case CBC_EXTRA:
		return 50;
		break;
	}
	return 0;
}

const char* getComboItemName(int index, bool isPrimary, int type) {

	std::string baseName;
	std::string displayName;

	switch (type) {

	case CBC_ATTACHMENTS:
	{
		int itemIndex = secondarySelectedItems[0];

		if (isPrimary) {
			itemIndex = primarySelectedItems[0];
		}

		baseName = BG_UnlockablesGetItemAttachmentDisplayName(Com_SessionMode_GetMode(), itemIndex, index);

		if (!baseName.empty()) {
			return UI_SafeTranslateString(baseName.c_str());
		}
		break;
	}
	case CBC_CAMOS:
	{
		auto camoIndex = index + 45;

		baseName = get_column_value_for_row_from_path("gamedata/weapons/common/attachmenttable.csv", camoIndex, 3);

		if (!baseName.empty()) {
			return UI_SafeTranslateString(baseName.c_str());
		}

		break;
	}
	case CBC_RETICLES:
	{
		auto reticleIndex = index + 186;

		baseName = get_column_value_for_row_from_path("gamedata/weapons/common/attachmenttable.csv", reticleIndex, 3);

		if (!baseName.empty()) {
			return UI_SafeTranslateString(baseName.c_str());
		}
		break;
	}
	case CBC_WEAPONTYPE:
	{
		auto weaponIndex = index + 2;

		baseName = get_column_value_for_row_from_path("gamedata/stats/mp/mp_statstable.csv", weaponIndex, 3);

		if (!baseName.empty()) {
			return UI_SafeTranslateString(baseName.c_str());
		}
		break;
	}
	case CBC_PERK:
	{
		auto perkIndex = index + 134;

		baseName = get_column_value_for_row_from_path("gamedata/stats/mp/mp_statstable.csv", perkIndex, 3);

		if (!baseName.empty()) {
			return UI_SafeTranslateString(baseName.c_str());
		}
		break;
	}
	case CBC_BONUSCARD:
	{
		auto cardIndex = index + 158;

		baseName = get_column_value_for_row_from_path("gamedata/stats/mp/mp_statstable.csv", cardIndex, 3);

		if (!baseName.empty()) {
			return UI_SafeTranslateString(baseName.c_str());
		}
		break;
	}
	case CBC_KILLSTREAK:
	{
		auto streakIndex = index + 175;

		baseName = get_column_value_for_row_from_path("gamedata/stats/mp/mp_statstable.csv", streakIndex, 3);

		if (!baseName.empty()) {
			return UI_SafeTranslateString(baseName.c_str());
		}
		break;
	}
	case CBC_EXTRA:
	{
		auto extraIndex = index + 204;

		baseName = get_column_value_for_row_from_path("gamedata/stats/mp/mp_statstable.csv", extraIndex, 3);

		if (!baseName.empty()) {
			return UI_SafeTranslateString(baseName.c_str());
		}
		break;
	}
	default:
		return nullptr;
	}
	return nullptr;
}

void unlock_secret_items()
{
	for (auto i{ 0 }; i < 256; i++)
	{
		auto item_info = &get_item_infos(Com_SessionMode_GetMode())->itemInfo[i];

		item_info->flags = 4;
		item_info->requiredLevel = 1;
		item_info->requiredPrestige = 1;
	}
}

const char* intToConstCharPtr(int num) {
	std::string str = std::to_string(num);
	char* cstr = new char[str.length() + 1];
	std::strcpy(cstr, str.c_str());
	return cstr;
}

__int64 GetStatsBuffer(int type) {
	if (type <= 1) {
		return LiveStats_Core_GetDDLContext(0, GetSessionState());
	}
	else {
		return LiveStats_Core_GetStableDDLContext(0, GetSessionState());
	}
}

void resetFileshareMedia() {

	//for (int i = 0; i < 32; i++) {
	//	// Variants
	//	auto DDLRootState = Storage_GetDDLRootState((StorageFileType)24);
	//	const char* path[8]{};
	//	char toState[2000]{};
	//	auto storageContext = Storage_GetDDLContext(0, 24, i);

	//	path[0] = "variant";
	//	path[1] = intToConstCharPtr(i);
	//	path[2] = "readOnly";

	//	if (DDL_MoveToPath((__int64)DDLRootState, toState, 3, path)) {
	//		DDL_SetInt((__int64)toState, (__int64)storageContext, 0);
	//		ZeroMemory(toState, sizeof(toState));
	//	}
	//	Storage_Write(0, 24, i);

	//}

	for (int j = 0; j != 32; j++) {
		auto fromState = Storage_GetDDLRootState(26);
		const char* path[8]{};
		char toState[2000]{};
		auto storageContext = Storage_GetDDLContext(0, 26, 0);

		path[0] = "emblemList";
		path[1] = intToConstCharPtr(j);
		path[2] = "readOnly";

		if (DDL_MoveToPath((__int64)fromState, toState, 3, path)) {
			DDL_SetInt((__int64)toState, (__int64)storageContext, 0);
			ZeroMemory(toState, sizeof(toState));
		}
		Storage_Write(0, 26, 0);
	}

	//for (int k = 0; k < 32; k++) {
	//	// Paintjob
	//	auto fromState = Storage_GetDDLRootState((StorageFileType)25);
	//	const char* path[8]{};
	//	char toState[2000]{};
	//	auto storageContext = Storage_GetDDLContext(0, 25, k);

	//	path[0] = "weaponPaintjob";
	//	path[1] = intToConstCharPtr(k);
	//	path[2] = "readOnly";

	//	if (DDL_MoveToPath((__int64)fromState, toState, 3, path)) {
	//		DDL_SetInt((__int64)toState, (__int64)storageContext, 0);
	//		ZeroMemory(toState, sizeof(toState));
	//	}
	//	Storage_Write(0, 25, k);
	//}


}

// General

void unlockCallingCards() {

	for (int i = 0; i < 800; i++) {

		__int64 RootState = LiveStats_Core_GetRootDDLState(Com_SessionMode_GetMode());
		const char* path[8]{};
		__int64 DDLContext = GetStatsBuffer(0);
		char toState[2000]{};

		path[0] = "emblemStats";
		path[1] = "backgrounds";
		path[2] = intToConstCharPtr(i);
		path[3] = "purchased";

		if (DDL_MoveToPath(RootState, toState, 4, path)) {
			DDL_SetUInt((__int64)toState, DDLContext, 1);
			ZeroMemory(toState, sizeof(toState));
		}
		path[3] = "unlocked";

		if (DDL_MoveToPath(RootState, toState, 4, path)) {
			DDL_SetUInt((__int64)toState, DDLContext, 1);
			ZeroMemory(toState, sizeof(toState));
		}

		path[1] = "icons";
		path[3] = "purchased";

		if (DDL_MoveToPath(RootState, toState, 4, path)) {
			DDL_SetUInt((__int64)toState, DDLContext, 1);
			ZeroMemory(toState, sizeof(toState));
		}
		path[3] = "unlocked";

		if (DDL_MoveToPath(RootState, toState, 4, path)) {
			DDL_SetUInt((__int64)toState, DDLContext, 1);
			ZeroMemory(toState, sizeof(toState));
		}


	}

}

void unlockClassSlots() {

	for (int i = 1; i < 6; i++) {
		__int64 RootState = LiveStats_Core_GetRootDDLState(Com_SessionMode_GetMode());
		const char* path[8]{};
		__int64 DDLContext = GetStatsBuffer(0);
		char toState[2000]{};

		path[0] = "prestigetokens";
		path[1] = intToConstCharPtr(i);
		path[2] = "tokentype";
		path[3] = "prestige_extra_cac";

		if (DDL_MoveToPath(RootState, toState, 4, path)) {
			//logPath(4, path);
			DDL_SetUInt((__int64)toState, DDLContext, 3);
			ZeroMemory(toState, sizeof(toState));
		}

	}

}

void setMaxTokens() {

	__int64 RootState = LiveStats_Core_GetRootDDLState(Com_SessionMode_GetMode());
	const char* path[8]{};
	__int64 DDLContext = GetStatsBuffer(0);
	char toState[2000]{};

	path[0] = "unlocks";
	path[1] = "0";

	if (DDL_MoveToPath(RootState, toState, 2, path)) {
		DDL_SetUInt((__int64)toState, DDLContext, 255);
	}

}

void UnlockAllAchievements() {

	// CP Achievements

	LiveAchievements_GiveAchievement("CP_COMPLETE_PROLOGUE");
	LiveAchievements_GiveAchievement("CP_COMPLETE_NEWWORLD");
	LiveAchievements_GiveAchievement("CP_COMPLETE_BLACKSTATION");
	LiveAchievements_GiveAchievement("CP_COMPLETE_BIODOMES");
	LiveAchievements_GiveAchievement("CP_COMPLETE_SGEN");
	LiveAchievements_GiveAchievement("CP_COMPLETE_VENGEANCE");
	LiveAchievements_GiveAchievement("CP_COMPLETE_RAMSES");
	LiveAchievements_GiveAchievement("CP_COMPLETE_INFECTION");
	LiveAchievements_GiveAchievement("CP_COMPLETE_AQUIFER");
	LiveAchievements_GiveAchievement("CP_COMPLETE_LOTUS");
	LiveAchievements_GiveAchievement("CP_HARD_COMPLETE");
	LiveAchievements_GiveAchievement("CP_REALISTIC_COMPLETE");
	LiveAchievements_GiveAchievement("CP_CAMPAIGN_COMPLETE");
	LiveAchievements_GiveAchievement("CP_FIREFLIES_KILL");
	LiveAchievements_GiveAchievement("CP_UNSTOPPABLE_KILL");
	LiveAchievements_GiveAchievement("CP_FLYING_WASP_KILL");
	LiveAchievements_GiveAchievement("CP_TIMED_KILL");
	LiveAchievements_GiveAchievement("CP_ALL_COLLECTIBLES");
	LiveAchievements_GiveAchievement("CP_DIFFERENT_GUN_KILL");
	LiveAchievements_GiveAchievement("CP_ALL_DECORATIONS");
	LiveAchievements_GiveAchievement("CP_ALL_WEAPON_CAMOS");
	LiveAchievements_GiveAchievement("CP_CONTROL_QUAD");
	LiveAchievements_GiveAchievement("CP_MISSION_COLLECTIBLES");
	LiveAchievements_GiveAchievement("CP_DISTANCE_KILL");
	LiveAchievements_GiveAchievement("CP_OBSTRUCTED_KILL");
	LiveAchievements_GiveAchievement("CP_MELEE_COMBO_KILL");
	LiveAchievements_GiveAchievement("CP_COMPLETE_WALL_RUN");
	LiveAchievements_GiveAchievement("CP_TRAINING_GOLD");
	LiveAchievements_GiveAchievement("CP_COMBAT_ROBOT_KILL");
	LiveAchievements_GiveAchievement("CP_KILL_WASPS");
	LiveAchievements_GiveAchievement("CP_CYBERCORE_UPGRADE");
	LiveAchievements_GiveAchievement("CP_ALL_WEAPON_ATTACHMENTS");
	LiveAchievements_GiveAchievement("CP_TIMED_STUNNED_KILL");
	LiveAchievements_GiveAchievement("CP_UNLOCK_DOA");

	// MP Achievements

	LiveAchievements_GiveAchievement("MP_REACH_SERGEANT");
	LiveAchievements_GiveAchievement("MP_REACH_ARENA");
	LiveAchievements_GiveAchievement("MP_SPECIALIST_MEDALS");
	LiveAchievements_GiveAchievement("MP_MULTI_KILL_MEDALS");

	// ZM Achievements

	LiveAchievements_GiveAchievement("ZM_COMPLETE_RITUALS");
	LiveAchievements_GiveAchievement("ZM_SPOT_SHADOWMAN");
	LiveAchievements_GiveAchievement("ZM_GOBBLE_GUM");
	LiveAchievements_GiveAchievement("ZM_STORE_KILL");
	LiveAchievements_GiveAchievement("ZM_ROCKET_SHIELD_KILL");
	LiveAchievements_GiveAchievement("ZM_CIVIL_PROTECTOR");
	LiveAchievements_GiveAchievement("ZM_WINE_GRENADE_KILL");
	LiveAchievements_GiveAchievement("ZM_MARGWA_KILL");
	LiveAchievements_GiveAchievement("ZM_PARASITE_KILL");
	LiveAchievements_GiveAchievement("ZM_CASTLE_EE");
	LiveAchievements_GiveAchievement("ZM_CASTLE_ALL_BOWS");
	LiveAchievements_GiveAchievement("ZM_CASTLE_MINIGUN_MURDER");
	LiveAchievements_GiveAchievement("ZM_CASTLE_UPGRADED_BOW");
	LiveAchievements_GiveAchievement("ZM_CASTLE_MECH_TRAPPER");
	LiveAchievements_GiveAchievement("ZM_CASTLE_SPIKE_REVIVE");
	LiveAchievements_GiveAchievement("ZM_CASTLE_WALL_RUNNER");
	LiveAchievements_GiveAchievement("ZM_CASTLE_ELECTROCUTIONER");
	LiveAchievements_GiveAchievement("ZM_CASTLE_WUNDER_TOURIST");
	LiveAchievements_GiveAchievement("ZM_CASTLE_WUNDER_SNIPER");
	LiveAchievements_GiveAchievement("ZM_ISLAND_COMPLETE_EE");
	LiveAchievements_GiveAchievement("ZM_ISLAND_DRINK_WINE");
	LiveAchievements_GiveAchievement("ZM_ISLAND_CLONE_REVIVE");
	LiveAchievements_GiveAchievement("ZM_ISLAND_OBTAIN_SKULL");
	LiveAchievements_GiveAchievement("ZM_ISLAND_WONDER_KILL");
	LiveAchievements_GiveAchievement("ZM_ISLAND_STAY_UNDERWATER");
	LiveAchievements_GiveAchievement("ZM_ISLAND_THRASHER_RESCUE");
	LiveAchievements_GiveAchievement("ZM_ISLAND_ELECTRIC_SHIELD");
	LiveAchievements_GiveAchievement("ZM_ISLAND_DESTROY_WEBS");
	LiveAchievements_GiveAchievement("ZM_ISLAND_EAT_FRUIT");
	LiveAchievements_GiveAchievement("ZM_STALINGRAD_NIKOLAI");
	LiveAchievements_GiveAchievement("ZM_STALINGRAD_WIELD_DRAGON");
	LiveAchievements_GiveAchievement("ZM_STALINGRAD_TWENTY_ROUNDS");
	LiveAchievements_GiveAchievement("ZM_STALINGRAD_RIDE_DRAGON");
	LiveAchievements_GiveAchievement("ZM_STALINGRAD_LOCKDOWN");
	LiveAchievements_GiveAchievement("ZM_STALINGRAD_BEAM_KILL");
	LiveAchievements_GiveAchievement("ZM_STALINGRAD_STRIKE_DRAGON");
	LiveAchievements_GiveAchievement("ZM_STALINGRAD_FAFNIR_KILL");
	LiveAchievements_GiveAchievement("ZM_STALINGRAD_AIR_ZOMBIES");
	LiveAchievements_GiveAchievement("ZM_STALINGRAD_SOLO_TRIALS");
	LiveAchievements_GiveAchievement("ZM_GENESIS_EE");
	LiveAchievements_GiveAchievement("ZM_GENESIS_SUPER_EE");
	LiveAchievements_GiveAchievement("ZM_GENESIS_PACKECTOMY");
	LiveAchievements_GiveAchievement("ZM_GENESIS_KEEPER_ASSIST");
	LiveAchievements_GiveAchievement("ZM_GENESIS_DEATH_RAY");
	LiveAchievements_GiveAchievement("ZM_GENESIS_GRAND_TOUR");
	LiveAchievements_GiveAchievement("ZM_GENESIS_WARDROBE_CHANGE");
	LiveAchievements_GiveAchievement("ZM_GENESIS_WONDERFUL");
	LiveAchievements_GiveAchievement("ZM_GENESIS_CONTROLLED_CHAOS");
	LiveAchievements_GiveAchievement("DLC2_ZOMBIE_ALL_TRAPS");
	LiveAchievements_GiveAchievement("DLC2_ZOM_LUNARLANDERS");
	LiveAchievements_GiveAchievement("DLC2_ZOM_FIREMONKEY");
	LiveAchievements_GiveAchievement("DLC4_ZOM_TEMPLE_SIDEQUEST");
	LiveAchievements_GiveAchievement("DLC4_ZOM_SMALL_CONSOLATION");
	LiveAchievements_GiveAchievement("DLC5_ZOM_CRYOGENIC_PARTY");
	LiveAchievements_GiveAchievement("DLC5_ZOM_GROUND_CONTROL");
	LiveAchievements_GiveAchievement("ZM_DLC4_TOMB_SIDEQUEST");
	LiveAchievements_GiveAchievement("ZM_DLC4_OVERACHIEVER");
	LiveAchievements_GiveAchievement("ZM_PROTOTYPE_I_SAID_WERE_CLOSED");
	LiveAchievements_GiveAchievement("ZM_ASYLUM_ACTED_ALONE");
	LiveAchievements_GiveAchievement("ZM_THEATER_IVE_SEEN_SOME_THINGS");
}


// Campaign

struct MissionWithAccolades_Raw
{
	const char* rootMapName;        // 0x00
	uint8_t numAccolades;           // 0x08
	uint8_t pad_09[0x17];           // 0x09
	uint8_t accoladeIndices[0x10];  // 0x20 maybe
};

void DumpMissionAccoladeTable()
{
	auto table = reinterpret_cast<uint8_t*>(OFFSET(0x41DA710));

	for (int i = 0; i < 0x80; i++)
	{
		auto entry = table + (i * 0x30);

		auto rootMapName = *reinterpret_cast<const char**>(entry + 0x00);
		if (!rootMapName)
			continue;

		auto num = *reinterpret_cast<uint8_t*>(entry + 0x08);

		utils::write_Debug("Mission[%d] rootMapName=%s numAccolades=%u", i, rootMapName, num);

		for (int j = 0; j < num; j++)
		{
			auto idx_20 = *reinterpret_cast<uint8_t*>(entry + 0x20 + j);
			auto idx_10 = *reinterpret_cast<uint8_t*>(entry + 0x10 + j);
			auto idx_18 = *reinterpret_cast<uint8_t*>(entry + 0x18 + j);

			utils::write_Debug("  j=%d idx+10=%u idx+18=%u idx+20=%u", j, idx_10, idx_18, idx_20);
		}
	}
}

void UnlockAllMedals() {
	using namespace std;

	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	auto a1 = GetStatsBuffer(0);
	path[0] = "PlayerCPDecorations";
	char result[2000];

	for (int i = 0; i < 12; i++) {
		path[1] = intToConstCharPtr(i);
		path[2] = "medalEarned";
		DDL_MoveToPath(tmp, result, 3, path);
		DDL_SetUInt((__int64)result, a1, 1);
		ZeroMemory(result, size(result));

	}
	LiveStorage_UploadStatsForController(0);

}

void UnlockAllAccolades() {

	using namespace std;

	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	auto a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsByMap";
	char result[2000];

	for (int i = 0; i < 11; i++) {
		path[1] = cpmapnames[i];
		path[2] = "accolades";
		for (int j = 0; j < 33; j++) {
			auto item = std::to_string(j);
			path[3] = item.c_str();
			path[4] = "state";
			DDL_MoveToPath(tmp, result, 5, path);
			DDL_SetUInt((__int64)result, a1, 1);
			ZeroMemory(result, size(result));
			path[4] = "value";
			DDL_MoveToPath(tmp, result, 5, path);
			DDL_SetUInt((__int64)result, a1, 9999999);
			ZeroMemory(result, size(result));
			path[4] = "highestValue";
			DDL_MoveToPath(tmp, result, 5, path);
			DDL_SetUInt((__int64)result, a1, 9999999);
			ZeroMemory(result, size(result));
		}
	}
	LiveStorage_UploadStatsForController(0);

}

void UnlockAllCollectibles() {
	using namespace std;

	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	auto a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsByMap";
	char result[2000];

	for (int i = 0; i < 11; i++) {
		path[1] = cpmapnames[i];
		path[2] = "collectibles";
		for (int j = 0; j < 6; j++) {
			auto item = std::to_string(j);
			path[3] = item.c_str();
			DDL_MoveToPath(tmp, result, 4, path);
			DDL_SetUInt((__int64)result, a1, 1);
			ZeroMemory(result, size(result));
		}

		path[2] = "allCollectiblesCollected";
		DDL_MoveToPath(tmp, result, 3, path);
		DDL_SetUInt((__int64)result, a1, 1);
		ZeroMemory(result, size(result));
	}
	LiveStorage_UploadStatsForController(0);

}

void CompleteAllMissions() {
	// completedDifficulties
	using namespace std;

	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	auto a1 = GetStatsBuffer(0);

	char result[2000];

	for (int i = 0; i < 11; i++) {
		path[0] = "PlayerStatsByMap";
		path[1] = cpmapnames[i];
		path[2] = "hasBeenCompleted";
		DDL_MoveToPath(tmp, result, 3, path);
		DDL_SetUInt((__int64)result, a1, 1);
		ZeroMemory(result, size(result));

		for (int d = 0; d < 4; d++)
		{
			auto diff = std::to_string(d);
			path[0] = "PlayerStatsByMap";
			path[1] = cpmapnames[i];
			path[2] = "completedDifficulties";
			path[3] = diff.c_str();

			if (DDL_MoveToPath(tmp, result, 4, path))
				DDL_SetUInt((__int64)result, a1, 1);

			ZeroMemory(result, sizeof(result));
		}

		// PlayerLevelStats/<index>/bestPlaythrough/levelScore

		path[0] = "PlayerLevelStats";
		path[1] = levels[i];
		path[2] = "bestPlaythrough";
		path[3] = "levelScore";
		DDL_MoveToPath(tmp, result, 4, path);
		DDL_SetUInt((__int64)result, a1, 99999999);
		ZeroMemory(result, size(result));

		path[0] = "PlayerLevelStats";
		path[1] = levels[i];
		path[2] = "HighestDifficulty";
		DDL_MoveToPath(tmp, result, 3, path);
		DDL_SetUInt((__int64)result, a1, 3);
		ZeroMemory(result, size(result));

		path[0] = "PlayerLevelStats";
		path[1] = levels[i];
		path[2] = "challengesComplete";
		DDL_MoveToPath(tmp, result, 3, path);
		DDL_SetUInt((__int64)result, a1, 10);
		ZeroMemory(result, size(result));

	}
	LiveStorage_UploadStatsForController(0);

}

void UnlockAllMusic() {
	using namespace std;
	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	auto a1 = GetStatsBuffer(0);
	char result[2000];

	for (int i = 0; i < 32; i++) {
		path[0] = "musicUnlocks";
		path[1] = intToConstCharPtr(i);
		DDL_MoveToPath(tmp, result, 2, path);
		DDL_SetUInt((__int64)result, a1, 1);
		ZeroMemory(result, size(result));
	}
	LiveStorage_UploadStatsForController(0);
}

// Arena 

void unlockArenaStats()
{
	auto root = LiveStats_Core_GetRootDDLState(Com_SessionMode_GetMode());
	auto ctx = GetStatsBuffer(0);

	char state[2000]{};

	for (int i = 0; i < 2; i++)
	{
		const char* path[3] =
		{
			"arenastats",
			intToConstCharPtr(i),
			"points"
		};

		if (DDL_MoveToPath(root, state, 3, path))
			DDL_SetUInt((__int64)state, ctx, 100);
	}

	for (int j = 0; j < 12; j++)
	{
		const char* path[2] =
		{
			"arenachallengeseasons",
			intToConstCharPtr(j)
		};

		if (DDL_MoveToPath(root, state, 2, path))
			DDL_SetUInt((__int64)state, ctx, 1);
	}
}

// MP


void setStatbyName(const char* statName, int value)
{
	if (!statName || !*statName)
		return;

	auto root = LiveStats_Core_GetRootDDLState(GetSessionState());
	auto ctx = GetStatsBuffer(0);
	if (!root || !ctx)
		return;

	const char* path[3] =
	{
		"PlayerStatsList",
		statName,
		bArena ? "arenaValue" : "statValue"
	};

	char state[2000]{};
	if (!DDL_MoveToPath(root, state, 3, path))
	{
		utils::write_Debug(
			"[StatEditor] DDL_MoveToPath failed: PlayerStatsList/%s/%s",
			statName,
			path[2]);
		return;
	}

	DDL_SetUInt(reinterpret_cast<__int64>(state), ctx, value);
}

void setGroupStats() {
	auto mode = Com_SessionMode_GetMode();
	__int64 RootState = LiveStats_Core_GetRootDDLState(mode);
	__int64 DDLContext = GetStatsBuffer(0);
	char toState[2000]{};

	const char* groups[] = {
		"weapon_grenade", "weapon_pistol", "weapon_smg", "weapon_assault",
		"weapon_lmg", "weapon_cqb", "weapon_sniper", "weapon_launcher",
		"weapon_knife", "killstreak"
	};

	const char* challenges[] = {
		"used", "headshots", "longshot_kill", "noAttKills", "noPerkKills",
		"multikill_2", "killstreak_5", "challenges", "kill_enemy_one_bullet_shotgun",
		"kill_enemy_one_bullet_sniper", "revenge_kill", "loadedKills",
		"destroyed_aircraft", "direct_hit_kills", "destroyed_aircraft_under20s",
		"destroy_5_killstreak_vehicle", "destroy_2_killstreaks_rapidly",
		"destroy_aitank_or_setinel", "destroy_turret", "backstabber_kill",
		"kill_enemy_when_injured", "kill_enemy_with_their_weapon", "kills"
	};

	const char* path[8]{};
	path[0] = "groupstats";
	path[2] = "stats";

	for (const char* group : groups) {
		path[1] = group;

		for (const char* challenge : challenges) {
			path[3] = challenge;

			path[4] = "statValue";
			if (DDL_MoveToPath(RootState, toState, 5, path)) {
				DDL_SetUInt((__int64)toState, DDLContext, 999999);
			}
			ZeroMemory(toState, sizeof(toState));

			path[4] = "challengeValue";
			if (DDL_MoveToPath(RootState, toState, 5, path)) {
				DDL_SetUInt((__int64)toState, DDLContext, 999999);
			}
			ZeroMemory(toState, sizeof(toState));
		}
	}
}

struct StatEntry {
	const char* name;
	unsigned value;
	bool goldAlso;
};

int lastIntResult = 0;

void SetValue(__int64 root, __int64 ctx, char* toState, const std::initializer_list<const char*> path, unsigned val) {
	const char* arr[8];
	int depth = 0;

	for (auto p : path) {
		arr[depth++] = p;
	}

	if (DDL_MoveToPath(root, toState, depth, arr)) {
		lastIntResult = DDL_SetUInt((__int64)toState, ctx, val);
		ZeroMemory(toState, 2000);
	}
}

void GetValue(const std::vector<std::string>& path) {
	auto root = LiveStats_Core_GetRootDDLState(Com_SessionMode_GetMode());
	auto ctx = GetStatsBuffer(0);
	char toState[2000]{};
	const char* arr[8];
	int depth = 0;

	for (const auto& p : path) {
		if (depth < 8) {
			arr[depth++] = p.c_str();
		}
	}
	ImGui::InsertNotification({ ImGuiToastType::None, 2000, "%s %s %s", arr[0], arr[1], arr[2] });

	if (DDL_MoveToPath(root, toState, depth, arr)) {
		lastIntResult = DDL_GetUInt((__int64)toState, ctx);
		ZeroMemory(toState, 2000);
	}
}

bool SetUIntPath(__int64 root, __int64 ctx, const std::vector<const char*>& path, uint32_t value)
{
	char state[2000]{};

	const char** pathData = const_cast<const char**>(path.data());

	if (!DDL_MoveToPath(root, state, static_cast<int>(path.size()), pathData))
		return false;

	DDL_SetUInt((__int64)state, ctx, value);
	return true;
}

int GetSpecialistIndex(const char* name)
{
	static const char* heroes[] =
	{
		"heroes_mercenary",
		"heroes_outrider",
		"heroes_technomancer",
		"heroes_battery",
		"heroes_enforcer",
		"heroes_trapper",
		"heroes_reaper",
		"heroes_spectre",
		"heroes_firebreak"
	};

	if (!name || !name[0])
		return 0;

	for (int i = 0; i <= 8; i++)
	{
		if (strcmp(name, heroes[i]) == 0)
			return i;
	}

	return 0;
}

void SetChallengeValue(
	__int64 root,
	__int64 ctx,
	const char* category,
	const char* object,
	const char* stat,
	uint32_t value)
{
	if (!category || !category[0] || !stat || !stat[0])
		return;

	if (!_stricmp(category, "global"))
	{
		SetUIntPath(root, ctx,
			{
				"playerstatslist",
				stat,
				"challengeValue"
			}, value);

		return;
	}

	if (!_stricmp(category, "group"))
	{
		SetUIntPath(root, ctx,
			{
				"groupstats",
				object,
				"stats",
				stat,
				"challengeValue"
			}, value);

		return;
	}

	if (!_stricmp(category, "gamemode"))
	{
		SetUIntPath(root, ctx,
			{
				"PlayerStatsByGameType",
				object,
				stat,
				"challengeValue"
			}, value);

		return;
	}

	if (!_stricmp(category, "item") || !_stricmp(category, "weapon"))
		/*{
			uint32_t itemIndex = GetItemIndexFromStatTable(object);

			SetUIntPath(root, ctx,
				{
					"itemstats",
					intToConstCharPtr(itemIndex),
					"stats",
					stat,
					"challengeValue"
				}, value);

			return;
		}*/

		if (!_stricmp(category, "attachment"))
		{
			SetUIntPath(root, ctx,
				{
					"attachments",
					object,
					"stats",
					stat,
					"challengeValue"
				}, value);

			return;
		}

	if (!_stricmp(category, "specialist") || !_stricmp(category, "hero"))
	{
		uint32_t specialistIndex = GetSpecialistIndex(object);

		SetUIntPath(root, ctx,
			{
				"specialiststats",
				intToConstCharPtr(specialistIndex),
				"stats",
				stat,
				"challengeValue"
			}, value);

		return;
	}
}

void UnlockMilestoneTable(
	const char* tableName,
	int start,
	int end,
	int valueColumn,
	int categoryColumn,
	int statColumn,
	int objectColumn = 13)
{
	StringTable* table = nullptr;
	StringTable_GetAsset(tableName, &table);

	if (!table)
		return;

	auto root = LiveStats_Core_GetRootDDLState(Com_SessionMode_GetMode());
	auto ctx = GetStatsBuffer(0);

	for (int row = start; row <= end; row++)
	{
		const char* rowStr = intToConstCharPtr(row);

		const char* valueStr = StringTable_Lookup(table, 0, rowStr, valueColumn);
		const char* category = StringTable_Lookup(table, 0, rowStr, categoryColumn);
		const char* stat = StringTable_Lookup(table, 0, rowStr, statColumn);
		const char* objects = StringTable_Lookup(table, 0, rowStr, objectColumn);

		if (!valueStr || !valueStr[0] || !category || !category[0] || !stat || !stat[0])
			continue;

		uint32_t value = std::strtoul(valueStr, nullptr, 10);

		std::vector<std::string> splitObjects = split(objects ? objects : "", ' ');

		if (splitObjects.empty())
		{
			SetChallengeValue(root, ctx, category, nullptr, stat, value);
		}
		else
		{
			for (auto& obj : splitObjects)
			{
				if (!obj.empty())
					SetChallengeValue(root, ctx, category, obj.c_str(), stat, value);
			}
		}
	}
}

void setGameTypeStats() {

	auto root = LiveStats_Core_GetRootDDLState(Com_SessionMode_GetMode());
	auto ctx = GetStatsBuffer(0);
	char toState[2000]{};

	const char* gameModes[] = {
		"ball", "conf", "ctf", "dem", "dm", "dom", "escort", "gun",
		"hcconf", "hcctf", "hcdem", "hcdm", "hcdom", "hchq", "hckoth",
		"hcsd", "hctdm", "hq", "koth", "sas", "sd", "tdm", "clean"
	};

	for (const char* mode : gameModes) {

		for (int i = 1; i < 8; i++)
		{
			std::string challengeName = "challenge" + std::to_string(i);
			SetValue(root, ctx, toState, { "playerStatsByGameType", mode, challengeName.c_str(), "statValue" }, 99999999);
			SetValue(root, ctx, toState, { "playerStatsByGameType", mode, challengeName.c_str(), "challengeValue" }, 99999999);
		}

		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "assists", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "assists", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "cur_win_streak", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "cur_win_streak", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "deaths", "statValue" }, 1);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "deaths", "challengeValue" }, 1);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "defends", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "defends", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "kdratio", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "kdratio", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "kill_streak", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "kill_streak", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "kills", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "kills", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "losses", "statValue" }, 0);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "losses", "challengeValue" }, 0);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "offends", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "offends", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "score", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "score", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "ties", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "ties", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "top3", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "top3", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "top3team", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "top3team", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "topplayer", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "topplayer", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "win_streak", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "win_streak", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "wins", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "wins", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "wlratio", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "wlratio", "challengeValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "lifetime_score", "statValue" }, 99999999);
		SetValue(root, ctx, toState, { "playerStatsByGameType", mode, "lifetime_score", "challengeValue" }, 99999999);

	}

	SetValue(root, ctx, toState, { "playerStatsList", "score", "challengeValue" }, 99999999);
	SetValue(root, ctx, toState, { "playerStatsList", "score", "statValue" }, 99999999);
}

struct WeaponStatConfig
{
	int purchased = 3;
	int xp = 665535;
	int plevel = 15;

	int statValue = 7195000;
	int challengeValue = 75000;

	int accuracy = 100;
	int kills = 500000;
	int hits = 500000;
	int misses = 500000;
	int headshots = 500000;
	int assists = 500000;
	int used = 500000;
	int kills_while_active = 500000;
	int assist_score = 500000;
	int kill_enemy_when_injured = 500000;
	int kill_enemy_with_their_weapon = 500000;
	int killstreak_5 = 500000;
	int revenge_kill = 500000;
	int longshot_kill = 500000;
	int noAttKills = 500000;
	int loadedKills = 500000;
	int destroyed_aircraft = 500000;
	int direct_hit_kills = 500000;
	int destroyed_aircraft_under20s = 500000;
	int destroy_5_killstreak_vehicle = 500000;
	int destroy_2_killstreaks_rapidly = 500000;
	int destroy_aitank_or_setinel = 500000;
	int destroy_turret = 500000;
	int kill_enemy_one_bullet_sniper = 500000;
	int kill_enemy_one_bullet_shotgun = 500000;
	int noPerkKills = 500000;
	int challenges_tu = 500000;
	int hacks = 500000;
	int destroyed = 500000;
	int blindkills = 500000;
	int stunkills = 500000;
	int capture = 500000;
	int projectiles = 500000;

	int genericHigh = 500000;
	int genericLow = 6;

	int challengeBase = 50000;
};

static WeaponStatConfig g_WeaponStats;

void SetWeaponStats(int weaponIndex)
{
	auto root = LiveStats_Core_GetRootDDLState(Com_SessionMode_GetMode());
	auto ctx = GetStatsBuffer(0);
	char toState[2000]{};

	std::vector<const char*> base = { "itemstats", intToConstCharPtr(weaponIndex) };

	SetValue(root, ctx, toState, { base[0], base[1], "purchased" }, g_WeaponStats.purchased);
	SetValue(root, ctx, toState, { base[0], base[1], "xp" }, g_WeaponStats.xp);
	SetValue(root, ctx, toState, { base[0], base[1], "plevel" }, g_WeaponStats.plevel);

	for (int i = 0; i < 3; i++)
		SetValue(root, ctx, toState, { base[0], base[1], "isproversionunlocked" }, g_WeaponStats.purchased);

	auto setStat = [&](const char* name, int statVal, int chalVal)
		{
			SetValue(root, ctx, toState, { base[0], base[1], "stats", name, "statValue" }, statVal);
			SetValue(root, ctx, toState, { base[0], base[1], "stats", name, "challengeValue" }, chalVal);
		};

	setStat("headshots", g_WeaponStats.headshots, g_WeaponStats.headshots);
	setStat("assists", g_WeaponStats.assists, g_WeaponStats.assists);
	setStat("kills", g_WeaponStats.kills, g_WeaponStats.kills);
	setStat("used", g_WeaponStats.used, g_WeaponStats.used);
	setStat("hits", g_WeaponStats.hits, g_WeaponStats.hits);
	setStat("misses", g_WeaponStats.misses, g_WeaponStats.misses);

	setStat("challenges", g_WeaponStats.genericLow, g_WeaponStats.genericLow);
	setStat("backstabber_kill", 75000, 10);
	setStat("accuracy", g_WeaponStats.accuracy, 1);
	setStat("kills_while_active", g_WeaponStats.kills_while_active, 1);
	setStat("assist_score", g_WeaponStats.assist_score, 1);
	setStat("kill_enemy_when_injured", g_WeaponStats.kill_enemy_when_injured, 1);
	setStat("revenge_kill", g_WeaponStats.revenge_kill, 1);
	setStat("kill_enemy_with_their_weapon", g_WeaponStats.kill_enemy_with_their_weapon, 1);
	setStat("killstreak_5", g_WeaponStats.killstreak_5, 1);
	setStat("longshot_kill", g_WeaponStats.longshot_kill, 1);
	setStat("noAttKills", g_WeaponStats.noAttKills, 1);
	setStat("loadedKills", g_WeaponStats.loadedKills, 1);
	setStat("destroyed_aircraft", g_WeaponStats.destroyed_aircraft, 1);
	setStat("direct_hit_kills", g_WeaponStats.direct_hit_kills, 1);
	setStat("destroyed_aircraft_under20s", g_WeaponStats.destroyed_aircraft_under20s, 1);
	setStat("destroy_5_killstreak_vehicle", g_WeaponStats.destroy_5_killstreak_vehicle, 1);
	setStat("destroy_2_killstreaks_rapidly", g_WeaponStats.destroy_2_killstreaks_rapidly, 1);
	setStat("destroy_aitank_or_setinel", g_WeaponStats.destroy_aitank_or_setinel, 1);
	setStat("destroy_turret", g_WeaponStats.destroy_turret, 1);
	setStat("kill_enemy_one_bullet_sniper", g_WeaponStats.kill_enemy_one_bullet_sniper, 1);
	setStat("kill_enemy_one_bullet_shotgun", g_WeaponStats.kill_enemy_one_bullet_shotgun, 1);
	setStat("noPerkKills", g_WeaponStats.noPerkKills, 1);
	setStat("hack_enemy_target", g_WeaponStats.hacks, 1);
	setStat("combatRecordStat", g_WeaponStats.destroyed, 1);
	setStat("combatRecordStat", g_WeaponStats.blindkills, 1);
	setStat("combatRecordStat", g_WeaponStats.stunkills, 1);
	setStat("capture_objective_in_smoke", g_WeaponStats.capture, 1);
	setStat("destroy_explosive_with_trophy", g_WeaponStats.projectiles, 1);
	setStat("challenges_tu", g_WeaponStats.challenges_tu, 1);

	for (int i = 0; i < 8; i++)
	{
		char buf[32];
		sprintf_s(buf, "challenge%d", i);

		SetValue(root, ctx, toState, { base[0], base[1], "stats", buf }, g_WeaponStats.challenges_tu);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", buf, "challengeValue" }, g_WeaponStats.challenges_tu);
	}

	LiveStorage_UploadStatsForController(0);
}

void setMaxWeapons() {
	auto root = LiveStats_Core_GetRootDDLState(Com_SessionMode_GetMode());
	auto ctx = GetStatsBuffer(0);
	char toState[2000]{};

	for (int i = 0; i < 256; i++) {
		std::vector<const char*> base = { "itemstats", intToConstCharPtr(i) };

		SetValue(root, ctx, toState, { base[0], base[1], "purchased" }, 3);
		SetValue(root, ctx, toState, { base[0], base[1], "xp" }, 665535);
		SetValue(root, ctx, toState, { base[0], base[1], "plevel" }, 15);

		for (int j = 0; j < 3; j++)
			SetValue(root, ctx, toState, { base[0], base[1], "isproversionunlocked" }, 3);

		SetValue(root, ctx, toState, { base[0], base[1], "stats", "headshots", "statValue" }, 7195000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "headshots", "challengeValue" }, 75000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "assists", "statValue" }, 7195000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "assists", "challengeValue" }, 75000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kills", "statValue" }, 7195000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kills", "challengeValue" }, 75000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "used", "statValue" }, 7195000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "used", "challengeValue" }, 75000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "challenges", "statValue" }, 6);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "challenges", "challengeValue" }, 6);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "backstabber_kill", "statValue" }, 75000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "backstabber_kill", "challengeValue" }, 10);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "accuracy", "statValue" }, 100);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "accuracy", "challengeValue" }, 1);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kills_while_active", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kills_while_active", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "assist_score", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "assist_score", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kill_enemy_when_injured", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kill_enemy_when_injured", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "revenge_kill", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "revenge_kill", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kill_enemy_with_their_weapon", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kill_enemy_with_their_weapon", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "killstreak_5", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "killstreak_5", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "longshot_kill", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "longshot_kill", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "noAttKills", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "noAttKills", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "loadedKills", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "loadedKills", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "multikill_2", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "multikill_2", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "destroyed_aircraft", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "destroyed_aircraft", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "direct_hit_kills", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "direct_hit_kills", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "destroyed_aircraft_under20s", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "destroyed_aircraft_under20s", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "destroy_5_killstreak_vehicle", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "destroy_5_killstreak_vehicle", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "destroy_2_killstreaks_rapidly", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "destroy_2_killstreaks_rapidly", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "destroy_aitank_or_setinel", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "destroy_aitank_or_setinel", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "destroy_turret", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "destroy_turret", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kill_enemy_one_bullet_sniper", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kill_enemy_one_bullet_sniper", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kill_enemy_one_bullet_shotgun", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kill_enemy_one_bullet_shotgun", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "noPerkKills", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "noPerkKills", "challengeValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "challenges_tu", "statValue" }, 500000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "challenges_tu", "challengeValue" }, 500000);

		for (int j = 0; j < 8; j++) {
			char buf[32];
			sprintf_s(buf, "challenge%d", j);
			SetValue(root, ctx, toState, { base[0], base[1], "stats", buf }, 50000);
			SetValue(root, ctx, toState, { base[0], base[1], "stats", buf, "challengeValue" }, 50000);
		}
	}

	std::vector<StatEntry> entries = {
		{ "weapons_mastery", 6, false },
		{ "secondary_mastery", 3, false },
		{ "secondary_mastery_launcher", 3, false },
		{ "secondary_mastery_pistol", 3, false },
		{ "tscc_challenge_mastery", 2, false },
		{ "weapons_mastery_assault", 3, false },
		{ "weapons_mastery_cqb", 3, false },
		{ "weapons_mastery_lmg", 3, false },
		{ "weapons_mastery_smg", 3, false },
		{ "weapons_mastery_sniper", 3, false },

		{ "ar_famas", 3, true },
		{ "ar_garand", 3, true },
		{ "ar_peacekeeper", 3, true },
		{ "ar_pulse", 3, true },
		{ "bowie_knife", 3, true },
		{ "launcher_multi", 3, true },
		{ "lmg_infinite", 3, true },
		{ "melee_bat", 3, true },
		{ "melee_boneglass", 3, true },
		{ "melee_boxing", 3, true },
		{ "melee_butterfly", 3, true },
		{ "melee_chainsaw", 3, true },
		{ "melee_crescent", 3, true },
		{ "melee_crowbar", 3, true },
		{ "melee_dagger", 3, true },
		{ "melee_fireaxe", 3, true },
		{ "melee_improvise", 3, true },
		{ "melee_katana", 3, true },
		{ "melee_knuckles", 3, true },
		{ "melee_mace", 3, true },
		{ "melee_nunchuks", 3, true },
		{ "melee_prosthetic", 3, true },
		{ "melee_shockbaton", 3, true },
		{ "melee_shovel", 3, true },
		{ "melee_sword", 3, true },
		{ "melee_wrench", 3, true },
		{ "pistol_energy", 3, true },
		{ "pistol_shotgun", 3, true },
		{ "shotgun_energy", 3, true },
		{ "smg_mp40", 3, true },
		{ "smg_nailgun", 3, true },
		{ "smg_rechamber", 3, true },
		{ "sniper_double", 3, true },
		{ "sniper_quickscope", 3, true },
		{ "special_crossbow", 3, true },
		{ "special_discgun", 3, true },

		{ "z_weap_01", 3, true },
		{ "z_weap_02", 3, true },
		{ "z_weap_03", 3, true },
		{ "z_weap_04", 3, true },
		{ "z_weap_05", 3, true },
		{ "z_weap_06", 3, true },
		{ "z_weap_07", 3, true },
		{ "z_weap_08", 3, true },
		{ "z_weap_09", 3, true },
		{ "z_weap_10", 3, true },
		{ "z_weap_11", 3, true },
		{ "z_weap_12", 3, true },
		{ "z_weap_13", 3, true },
		{ "z_weap_14", 3, true },
		{ "z_weap_15", 3, true }
	};

	for (auto& e : entries) {
		SetValue(root, ctx, toState, { "playerstatslist", e.name, "statValue" }, e.value);
		SetValue(root, ctx, toState, { "playerstatslist", e.name, "challengeValue" }, e.value);

		if (e.goldAlso) {
			std::string gold = std::string(e.name) + "_gold";
			std::string diamond = std::string(e.name) + "_for_diamond";
			SetValue(root, ctx, toState, { "playerstatslist", gold.c_str(), "statValue" }, e.value);
			SetValue(root, ctx, toState, { "playerstatslist", gold.c_str(), "challengeValue" }, e.value);
			SetValue(root, ctx, toState, { "playerstatslist", diamond.c_str(), "challengeValue" }, e.value);
		}
	}

	LiveStorage_UploadStatsForController(0);
}

void unlockSpecialistOutfits() {
	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	char result[2000];

	for (int l = 0; l < 12; l++)
	{
		std::ostringstream temp;
		temp << l;
		std::string converted(temp.str());

		path[0] = "specialiststats";
		path[1] = converted.c_str();
		path[2] = "stats";
		const char* p3[23] = { "kills","kills_ability", "kills_weapon", "multikill_ability", "multikill_weapon", "kill_one_game_ability", "kill_one_game_weapon", "challenge1", "challenge2", "challenge3", "challenge4", "challenge5", "challenge6", "challenge7", "challenge8", "challenge9", "challenge10", "challenge11", "challenge12", "transmission1", "transmission2", "transmission3", "transmission4" };

		for (int k = 0; k < 23; k++)
		{
			path[3] = p3[k];
			path[4] = "statValue";
			DDL_MoveToPath(tmp, result, 5, path);
			DDL_SetUInt((__int64)result, a1, 1010050000);
			ZeroMemory(result, sizeof(result));
			path[4] = "challengeValue";
			DDL_MoveToPath(tmp, result, 5, path);
			DDL_SetUInt((__int64)result, a1, 1010050000);
			ZeroMemory(result, sizeof(result));
		}
		LiveStorage_UploadStatsForController(0);
	}
}

static int GetParagonBaseForMode(int mode)
{
	switch (mode)
	{
	case MODE_MULTIPLAYER:
		return 56;

	case MODE_ZOMBIES:
		return 36;

	default:
		return 0;
	}
}

static int VisibleParagonToStored(int mode, int visibleLevel)
{
	const int base = GetParagonBaseForMode(mode);

	if (base <= 0)
		return 0;

	return visibleLevel - base;
}

static int StoredParagonToVisible(int mode, int storedRank)
{
	const int base = GetParagonBaseForMode(mode);

	if (base <= 0)
		return storedRank;

	return storedRank + base;
}

void setPrestige(int rank) {
	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	char result[2000];
	int depth = 3;
	path[0] = "PlayerStatsList";
	path[1] = "plevel";
	path[2] = "statValue";
	if (bArena) {
		path[0] = "arenaStats";
		path[1] = "points";
		path[2] = "";
		depth = 2;
	}

	DDL_MoveToPath(tmp, result, depth, path);
	DDL_SetUInt((__int64)result, a1, rank);
	ZeroMemory(result, sizeof(result));
}

void setpLevel(int rank) {
	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	char result[2000];

	path[1] = "rank";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, rank);
	ZeroMemory(result, sizeof(result));
}

void setpLevelXP(int rank) {
	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	char result[2000];

	path[1] = "rankxp";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, rank);
	ZeroMemory(result, sizeof(result));
}

void setMasterRank(int visibleParagonLevel)
{
	const int mode = Com_SessionMode_GetMode();

	const int storedParagonRank = VisibleParagonToStored(mode, visibleParagonLevel);

	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 statsBuffer = GetStatsBuffer(0);

	path[0] = "PlayerStatsList";
	path[1] = "paragon_rank";
	path[2] = bArena ? "arenaValue" : "statValue";

	char result[2000]{};

	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt(reinterpret_cast<__int64>(result), statsBuffer, storedParagonRank);
}

void setMasterXP(int rank) {
	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	char result[2000];

	path[1] = "paragon_rankxp";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, rank);
	ZeroMemory(result, sizeof(result));
}


/*
arenastats()
{
	for(i = 0; i < 9; i++)
	{
		self setdstat("arenastats", i, "season", i);
		self setdstat("arenastats", i, "points", 0xFFFFFF);
		self setdstat("arenastats", i, "matchstartpoints", 0xFFFFFF);
		self setdstat("arenastats", i, "skill", 99999.0);
		self setdstat("arenastats", i, "winstreak", 0xFFFFFF);
		self setdstat("arenastats", i, "wins", 0xFFFFFF);
	}

	self setdstat("arenaperseasonstats", "season", ArenaGetSlot());
	self setdstat("arenaperseasonstats", "points", 0xFFFFFF);
	self setdstat("arenaperseasonstats", "matchstartpoints", 0xFFFFFF);
	self setdstat("arenaperseasonstats", "skill", 99999.0);
	self setdstat("arenaperseasonstats", "winstreak", 0xFFFFFF);
	self setdstat("arenaperseasonstats", "wins", 0xFFFFFF);

	self setdstat("arenabest", "season", ArenaGetSlot());
	self setdstat("arenabest", "points", 0xFFFFFF);
	self setdstat("arenabest", "matchstartpoints", 0xFFFFFF);
	self setdstat("arenabest", "skill", 99999.0);
	self setdstat("arenabest", "winstreak", 0xFFFFFF);
	self setdstat("arenabest", "wins", 0xFFFFFF);

	self setdstat("playerstatslist", "arena_max_rank", "statvalue", 55);
	self setdstat("playerstatslist", "arena_max_rank", "challengevalue", 65535);
	self setdstat("playerstatslist", "arena_max_rank", "arenavalue", 55);
	self setdstat("playerstatslist", "arena_season_wins", "statvalue", 65535);
	self setdstat("playerstatslist", "arena_season_wins", "challengevalue", 65535);
	self setdstat("playerstatslist", "arena_season_wins", "arenavalue", 65535);
	self setdstat("playerstatslist", "arena_season_challenge_earned_tally", "statvalue", 65535);
	self setdstat("playerstatslist", "arena_season_challenge_earned_tally", "challengevalue", 65535);
	self setdstat("playerstatslist", "arena_season_challenge_earned_tally", "arenavalue", 65535);

	for(i = 0; i < 12; i++)
		self setdstat("arenachallengeseasons", i, i);

	self setdstat("playerstatslist", "plevel", "arenavalue", 55);
	self setdstat("playerstatslist", "rank", "arenavalue", 55);
	self setdstat("playerstatslist", "rankxp", "arenavalue", 1000000);

	wait .1;
	uploadstats(self);
}*/

void setStats() {

	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	char result[2000];

	if (bArena) {

		for (int i = 0; i < 9; i++)
		{
			auto seasonStr = std::to_string(i);
			SetValue(tmp, a1, result, { "arenastats", seasonStr.c_str(), "season" }, i);
			SetValue(tmp, a1, result, { "arenastats", seasonStr.c_str(), "points" }, 100);

		}

		/*SetValue(tmp, a1, result, { "playerstatslist", "arena_max_rank", "statvalue" }, 55);
		SetValue(tmp, a1, result, { "playerstatslist", "arena_max_rank", "challengevalue" }, 65535);
		SetValue(tmp, a1, result, { "playerstatslist", "arena_max_rank", "arenavalue" }, 55);
		SetValue(tmp, a1, result, { "playerstatslist", "arena_season_wins", "statvalue" }, 65535);
		SetValue(tmp, a1, result, { "playerstatslist", "arena_season_wins", "challengevalue" }, 65535);
		SetValue(tmp, a1, result, { "playerstatslist", "arena_season_wins", "arenavalue" }, 65535);
		SetValue(tmp, a1, result, { "playerstatslist", "arena_season_challenge_earned_tally", "statvalue" }, 65535);
		SetValue(tmp, a1, result, { "playerstatslist", "arena_season_challenge_earned_tally", "challengevalue" }, 65535);
		SetValue(tmp, a1, result, { "playerstatslist", "arena_season_challenge_earned_tally", "arenavalue" }, 65535);*/

		for (int j = 0; j < 12; j++) {
			SetValue(tmp, a1, result, { "arenachallengeseasons", std::to_string(j).c_str() }, j);
		}
		return;

	}



	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	path[1] = "total_games_played";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iTotalGamesPlayed);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iTotalGamesPlayed);
	ZeroMemory(result, sizeof(result));


	path[2] = "statValue";
	path[1] = "highest_round_reached";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iHighestRound);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iHighestRound);
	ZeroMemory(result, sizeof(result));


	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	path[1] = "time_played_total";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iTimePlayedTotal);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iTimePlayedTotal);
	ZeroMemory(result, sizeof(result));

	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	path[1] = "cur_win_streak";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iCurWinStreak);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iCurWinStreak);
	ZeroMemory(result, sizeof(result));

	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	path[1] = "score";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iScore);
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iScore);
	ZeroMemory(result, sizeof(result));

	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	path[1] = "kills";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iKills);
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iKills);
	ZeroMemory(result, sizeof(result));

	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	path[1] = "deaths";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iDeaths);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iDeaths);
	ZeroMemory(result, sizeof(result));

	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	path[1] = "headshots";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iHeadshots);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iHeadshots);
	ZeroMemory(result, sizeof(result));

	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	path[1] = "wlratio";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iWL);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iWL);
	ZeroMemory(result, sizeof(result));

	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	path[1] = "kdratio";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iKD);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, iKD);
	ZeroMemory(result, sizeof(result));

	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	path[1] = "challenges";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, 50000);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, 50000);
	ZeroMemory(result, sizeof(result));
	LiveStorage_UploadStatsForController(0);

}

/*
void completeDailyChallenges()
{

	StringTable* table = nullptr;
	StringTable_GetAsset("gamedata/stats/zm/statsmilestones4.csv", &table);

	auto root = LiveStats_Core_GetRootDDLState(GetSessionState());
	auto ctx = GetStatsBuffer(0);

	char stateBuf[2000]{};
	const char* path[3]{};

	int rowCount = table->rowCount;

	for (int row = 0; row < rowCount; row++)
	{
		const char* challengeName = StringTable_GetColumnValueForRow(table, row, 4);

		if (!challengeName || !challengeName[0])
			continue;

		const char* targetStr = StringTable_GetColumnValueForRow(table, row, 2);

		int value = 50000;

		if (targetStr && targetStr[0])
			value = std::stoi(targetStr);

		if (!value) value = 50000;

		path[0] = "playerStatsList";
		path[1] = challengeName;
		path[2] = "statValue";

		if (DDL_MoveToPath(root, stateBuf, 3, path))
			DDL_SetUInt((__int64)stateBuf, ctx, value);
	}

	LiveStorage_UploadStatsForController(0);

}*/

void completeDailyChallenges()
{
	StringTable* table = nullptr;
	StringTable_GetAsset("gamedata/stats/zm/statsmilestones4.csv", &table);

	auto root = LiveStats_Core_GetRootDDLState(GetSessionState());
	auto ctx = GetStatsBuffer(0);

	char stateBuf[2000]{};
	const char* path[3]{};

	int rowCount = table->rowCount;

	for (int row = 2; row < rowCount; row++)
	{
		const char* challengeName = StringTable_GetColumnValueForRow(table, row, 4);

		if (!challengeName || !challengeName[0])
			continue;

		const char* targetStr = StringTable_GetColumnValueForRow(table, row, 2);

		int value = 50000;

		if (targetStr && targetStr[0])
			value = std::stoi(targetStr);

		if (!value) value = 50000;

		path[0] = "playerStatsList";
		path[1] = challengeName;
		path[2] = "statValue";

		if (DDL_MoveToPath(root, stateBuf, 3, path)) {
			DDL_SetUInt((__int64)stateBuf, ctx, value);
		}

		path[2] = "challengeValue";

		if (DDL_MoveToPath(root, stateBuf, 3, path))
			DDL_SetUInt((__int64)stateBuf, ctx, value);

	}

	LiveStorage_UploadStatsForController(0);

}

void setStatMilestones(const char* mode, const char* csv)
{
	StringTable* table = nullptr;

	std::string fullPath = std::string("gamedata/stats/") + mode + "/" + csv;

	StringTable_GetAsset(fullPath.c_str(), &table);

	auto root = LiveStats_Core_GetRootDDLState(GetSessionState());
	auto ctx = GetStatsBuffer(0);

	char stateBuf[2000]{};
	const char* path[3]{};

	int rowCount = table->rowCount;

	for (int row = 0; row < rowCount; row++)
	{
		const char* challengeName = StringTable_GetColumnValueForRow(table, row, 4);

		if (!challengeName || !challengeName[0])
			continue;

		const char* scope = StringTable_GetColumnValueForRow(table, row, 3);
		if (!scope || _stricmp(scope, "global") != 0)
			continue;

		const char* targetStr = StringTable_GetColumnValueForRow(table, row, 2);

		int value = 50000;

		if (targetStr && targetStr[0])
			value = std::stoi(targetStr);

		if (!value) value = 50000;

		path[0] = "playerStatsList";
		path[1] = challengeName;
		path[2] = "statValue";

		if (DDL_MoveToPath(root, stateBuf, 3, path))
			DDL_SetUInt((__int64)stateBuf, ctx, value);

		path[2] = "challengeValue";

		if (DDL_MoveToPath(root, stateBuf, 3, path))
			DDL_SetUInt((__int64)stateBuf, ctx, value);
	}



	LiveStorage_UploadStatsForController(0);
}

//void setChallenges() {
//	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
//	auto a1 = GetStatsBuffer(0);
//	const char* path[8];
//	const char* mp[71] = { "kill_with_pickup","kill_while_wallrunning", "kill_while_in_air", "kill_while_sliding","kill_while_mantling","kill_enemy_thats_wallrunning","kill_enemy_that_in_air","kill_while_underwater","kill_after_doublejump_out_of_water","kill_while_sliding_from_doublejump","kill_while_wallrunning_2_walls","destroy_equipment_with_bullet", "bouncingbetty_planted","bouncingbetty_pickedup", "bouncingbetty_devil_planted", "bouncingbetty_devil_pickedup","bouncingbetty_holly_planted","bouncingbetty_holly_pickedup","ballistic_knives_pickedup","wallbuy_weapons_purchased","zdogs_killed","zraps_killed", "zwasp_killed", "zspiders_killed", "zthrashers_killed", "zsentinel_killed", "zraz_killed", "zdog_rounds_finished", "specialty_armorvest_drank","specialty_quickrevive_drank", "specialty_fastreload_drank", "specialty_additionalprimaryweapon_drank","specialty_staminup_drank","specialty_doubletap2_drank","specialty_widowswine_drank","specialty_deadshot_drank","specialty_electriccherry_drank","specialty_electriccherry_drank", "zombie_hunter_kill_headshot","zombie_hunter_kill_melee", "zombie_hunter_kill_crawler", "zombie_hunter_kill_packapunch","zombie_hunter_kill_trap","zombie_hunter_kill_explosives","zombie_hunter_explosion_multikill","zombie_hunter_blast_furnace","zombie_hunter_dead_wire","zombie_hunter_fire_works","zombie_hunter_thunder_wall","zombie_hunter_turned","zombie_hunter_mastery", "survivalist_buy_magic_box", "survivalist_buy_perk", "survivalist_buy_wallbuy", "survivalist_buy_door", "survivalist_revive", "survivalist_survive_rounds", "survivalist_craftable", "survivalist_board", "survivalist_powerup", "survivalist_mastery", "gum_gobbler_consume", "gum_gobbler_powerups", "gum_gobbler_alchemical_antithesis", "gum_gobbler_anywhere_but_here", "gum_gobbler_burned_out", "gum_gobbler_ephemeral_enhancement", "gum_gobbler_phoenix_up", "gum_gobbler_sword_flay", "gum_gobbler_wall_power", "gum_gobbler_mastery" };
//	char result[2000];
//	for (int i = 0; i < 71; i++) {
//		path[0] = "playerstatslist";
//		path[1] = mp[i];
//		path[2] = "statValue";
//		DDL_MoveToPath(tmp, result, 3, path);
//		DDL_SetUInt((__int64)result, a1, 50000);
//		path[2] = "challengeValue";
//		DDL_MoveToPath(tmp, result, 3, path);
//		DDL_SetUInt((__int64)result, a1, 50000);
//	}
//	LiveStorage_UploadStatsForController(0);
//}

void unlockContracts(int start, int end, int type)
{
	StringTable* table = nullptr;
	StringTable_GetAsset(
		"gamedata/tables/mp/mp_contracttable.csv",
		&table
	);

	if (!table)
		return;

	auto root = LiveStats_Core_GetRootDDLState(
		Com_SessionMode_GetMode()
	);

	auto ctx = GetStatsBuffer(0);

	if (!root || !ctx)
		return;

	for (int contract = start; contract < end; ++contract)
	{
		std::string row = std::to_string(contract);

		const char* contractID =
			StringTable_Lookup(table, 0, row.c_str(), 0);

		const char* challengeValue =
			StringTable_Lookup(table, 0, row.c_str(), 2);

		const char* completionStat1 =
			StringTable_Lookup(table, 0, row.c_str(), 7);

		const char* completionStat2 =
			StringTable_Lookup(table, 0, row.c_str(), 8);

		const char* extraPath =
			StringTable_Lookup(table, 0, row.c_str(), 9);

		DDLState contractsState{};
		DDLState contractState{};
		DDLState fieldState{};

		if (DDL_MoveToName(root, (char*)&contractsState, "contracts") &&
			DDL_MoveToIndex(&contractsState, &contractState, type))
		{

			if (DDL_MoveToName(
				(__int64)&contractState,
				(char*)&fieldState,
				"active"))
			{
				DDL_SetUInt(
					reinterpret_cast<__int64>(&fieldState),
					ctx,
					1
				);
			}

			if (contractID && *contractID)
			{
				if (DDL_MoveToName(
					(__int64)&contractState,
					(char*)&fieldState,
					"index"))
				{
					DDL_SetUInt(
						reinterpret_cast<__int64>(&fieldState),
						ctx,
						std::strtoul(contractID, nullptr, 10)
					);
				}
			}

			if (challengeValue && *challengeValue)
			{
				if (DDL_MoveToName(
					(__int64)&contractState,
					(char*)&fieldState,
					"progress"))
				{
					DDL_SetUInt(
						reinterpret_cast<__int64>(&fieldState),
						ctx,
						std::strtoul(challengeValue, nullptr, 10)
					);
				}
			}

			if (DDL_MoveToName(
				(__int64)&contractState,
				(char*)&fieldState,
				"award_given"))
			{
				DDL_SetUInt(
					reinterpret_cast<__int64>(&fieldState),
					ctx,
					1
				);
			}
		}

		if (extraPath && *extraPath)
		{
			auto parts = split(extraPath, ' ');

			if (parts.size() >= 2 &&
				!parts[0].empty() &&
				!parts[1].empty())
			{
				const int extraIndex =
					static_cast<int>(
						std::strtoul(
							parts[1].c_str(),
							nullptr,
							10
						)
						);

				DDLState extraArray{};
				DDLState extraElement{};

				if (DDL_MoveToName(
					root,
					(char*)&extraArray,
					parts[0].c_str()) &&
					DDL_MoveToIndex(
						&extraArray,
						&extraElement,
						extraIndex))
				{
					DDL_SetUInt(
						reinterpret_cast<__int64>(&extraElement),
						ctx,
						1
					);
				}
			}
		}


		//
		// Column 7 completion stat
		//

		if (completionStat1 && *completionStat1)
		{
			const char* statPath[] =
			{
				"playerstatslist",
				completionStat1,
				"statvalue"
			};

			DDLState statState{};

			if (DDL_MoveToPath(
				root,
				(char*)&statState,
				3,
				statPath))
			{
				DDL_SetUInt(
					reinterpret_cast<__int64>(&statState),
					ctx,
					1
				);
			}
		}


		//
		// Column 8 completion stat
		//

		if (completionStat2 && *completionStat2)
		{
			const char* statPath[] =
			{
				"playerstatslist",
				completionStat2,
				"statvalue"
			};

			DDLState statState{};

			if (DDL_MoveToPath(
				root,
				(char*)&statState,
				3,
				statPath))
			{
				DDL_SetUInt(
					reinterpret_cast<__int64>(&statState),
					ctx,
					1
				);
			}
		}
	}
}

void resetCrypto() {
	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	auto a1 = GetStatsBuffer(0);
	char result[2000];
	DDL_MoveToName(tmp, result, "mp_loot_xp_due");
	DDL_SetUInt((__int64)result, a1, 0);
	LiveStorage_UploadStatsForController(0);
}

void setMaxAttachments() {
	auto root = LiveStats_Core_GetRootDDLState(Com_SessionMode_GetMode());
	auto ctx = GetStatsBuffer(0);
	char toState[2000]{};


	for (int i = 0; i < 44; i++) {
		std::vector<const char*> base = { "Attachments", BG_GetAttachmentName(i) };

		SetValue(root, ctx, toState, { base[0], base[1], "purchased" }, 3);
		SetValue(root, ctx, toState, { base[0], base[1], "xp" }, 665535);
		SetValue(root, ctx, toState, { base[0], base[1], "plevel" }, 15);

		for (int j = 0; j < 3; j++)
			SetValue(root, ctx, toState, { base[0], base[1], "isproversionunlocked" }, 3);

		SetValue(root, ctx, toState, { base[0], base[1], "stats", "headshots", "statValue" }, 7195000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "headshots", "challengeValue" }, 75000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "assists", "statValue" }, 7195000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "assists", "challengeValue" }, 75000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kills", "statValue" }, 7195000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kills", "challengeValue" }, 75000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "challenges", "statValue" }, 6);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "challenges", "challengeValue" }, 6);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "backstabber_kill", "statValue" }, 75000);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "backstabber_kill", "challengeValue" }, 10);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "accuracy", "statValue" }, 100);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "accuracy", "challengeValue" }, 1);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "multikill_2", "statValue" }, 99999);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "multikill_2", "challengeValue" }, 99999);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "longshot_kill", "statValue" }, 99999);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "longshot_kill", "challengeValue" }, 99999);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "killstreak_5_attachment", "statValue" }, 99999);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "killstreak_5_attachment", "challengeValue" }, 99999);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kill_thermal_through_smoke", "statValue" }, 99999);
		SetValue(root, ctx, toState, { base[0], base[1], "stats", "kill_thermal_through_smoke", "challengeValue" }, 99999);

		for (int j = 0; j < 8; j++) {
			char buf[32];
			sprintf_s(buf, "challenge%d", j);
			SetValue(root, ctx, toState, { base[0], base[1], "stats", buf }, 50000);
			SetValue(root, ctx, toState, { base[0], base[1], "stats", buf, "challengeValue" }, 50000);
		}
	}

}

// ZM

static void setMapEE(int map) {

	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	auto a1 = GetStatsBuffer(0);
	const char* path[8];
	const char* zmMaps[12] = { "darkops_zod_ee", "darkops_factory_ee", "darkops_castle_ee", "darkops_island_ee", "darkops_stalingrad_ee", "darkops_genesis_ee", "darkops_zod_super_ee", "darkops_factory_super_ee", "darkops_castle_super_ee", "darkops_island_super_ee", "darkops_stalingrad_super_ee", "darkops_genesis_super_ee" };
	char result[2000];

	int amt = (setRound) ? 50000 : 0;

	if (map > 6) map = 6;

	path[0] = "playerstatslist";
	path[1] = zmMaps[map];
	path[2] = "statValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amt);
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amt);

	path[0] = "playerstatslist";
	path[1] = zmMaps[map + 6];
	path[2] = "statValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amt);
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amt);

}

static void setAllMapEE() {

	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	auto a1 = GetStatsBuffer(0);
	const char* path[8];
	const char* zmMaps[12] = { "darkops_zod_ee", "darkops_factory_ee", "darkops_castle_ee", "darkops_island_ee", "darkops_stalingrad_ee", "darkops_genesis_ee", "darkops_zod_super_ee", "darkops_factory_super_ee", "darkops_castle_super_ee", "darkops_island_super_ee", "darkops_stalingrad_super_ee", "darkops_genesis_super_ee" };
	char result[2000];

	for (int i = 0; i < 12; i++) {
		path[0] = "playerstatslist";
		path[1] = zmMaps[i];
		path[2] = "statValue";
		DDL_MoveToPath(tmp, result, 3, path);
		DDL_SetUInt((__int64)result, a1, 50000);
		path[2] = "challengeValue";
		DDL_MoveToPath(tmp, result, 3, path);
		DDL_SetUInt((__int64)result, a1, 50000);
	}
	LiveStorage_UploadStatsForController(0);
}

void setMapStat(int map, int round) {
	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "playerstatsbymap";
	path[2] = "stats";
	path[3] = "highest_round_reached";
	path[4] = "StatValue";
	char result[2000];

	path[1] = zmmapnames[map];
	DDL_MoveToPath(tmp, result, 5, path);
	DDL_SetUInt((__int64)result, a1, round);
	ZeroMemory(result, sizeof(result));
	LiveStorage_UploadStatsForController(0);
}

void setMaxMapStats(int round) {
	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "playerstatsbymap";
	path[2] = "stats";
	path[3] = "highest_round_reached";
	path[4] = "StatValue";
	char result[2000];

	for (int l = 0; l < 14; l++) {
		path[1] = zmmapnames[l];
		DDL_MoveToPath(tmp, result, 5, path);
		DDL_SetUInt((__int64)result, a1, round);
		ZeroMemory(result, sizeof(result));
	}
	LiveStorage_UploadStatsForController(0);
}




// Dvar

void runDvars(setDvar_e type) {

	switch (type) {
	case GUARANTEE_SPECIALIST_THEME:
		Dvar_SetFromString("loot_bribecrate_dwid", "3", true);
		break;
	case GUARANTEE_CALLING_CARD:
		Dvar_SetFromString("loot_bribecrate_dwid", "4", true);
		break;
	case GURANTEE_FIRST_PLACE_TAUNT:
		Dvar_SetFromString("loot_bribecrate_dwid", "5", true);
		break;
	case GUARANTEE_SPECIALIST_GESTURE:
		Dvar_SetFromString("loot_bribecrate_dwid", "6", true);
		break;
	case GUARANTEE_ATTACHMENT_VARIANT:
		Dvar_SetFromString("loot_bribecrate_dwid", "7", true);
		break;
	case GUARANTEE_CALLING_CARD_2:
		Dvar_SetFromString("loot_bribecrate_dwid", "8", true);
		break;
	case GUARANTEE_BATTERY_ITEM:
		Dvar_SetFromString("loot_bribecrate_dwid", "20", true);
		break;
	case GUARANTEE_FIREBREAK_ITEM:
		Dvar_SetFromString("loot_bribecrate_dwid", "21", true);
		break;
	case GUARANTEE_NOMAD_ITEM:
		Dvar_SetFromString("loot_bribecrate_dwid", "22", true);
		break;
	case GUARANTEE_OUTRIDER_ITEM:
		Dvar_SetFromString("loot_bribecrate_dwid", "23", true);
		break;
	case GUARANTEE_PROPHET_ITEM:
		Dvar_SetFromString("loot_bribecrate_dwid", "24", true);
		break;
	case GUARANTEE_REAPER_ITEM:
		Dvar_SetFromString("loot_bribecrate_dwid", "25", true);
		break;
	case GUARANTEE_RUIN_ITEM:
		Dvar_SetFromString("loot_bribecrate_dwid", "26", true);
		break;
	case GUARANTEE_SERAPH_ITEM:
		Dvar_SetFromString("loot_bribecrate_dwid", "27", true);
		break;
	case GUARANTEE_SPECTRE_ITEM:
		Dvar_SetFromString("loot_bribecrate_dwid", "28", true);
		break;
	case GUARANTEE_THREE_SPECIALIST_OUTFIT:
		Dvar_SetFromString("loot_bribecrate_dwid", "52", true);
		break;
	case GUARANTEE_THREE_TAUNTS_AND_GESTURES:
		Dvar_SetFromString("loot_bribecrate_dwid", "53", true);
		break;
	case GUARANTEE_THREE_OF_A_KIND_BACKGROUND:
		Dvar_SetFromString("loot_bribecrate_dwid", "40", true);
		break;
	case GUARANTEE_THREE_ATTACHMENT_VARIANT:
		Dvar_SetFromString("loot_bribecrate_dwid", "57", true);
		break;
	case SUPPLY_DROP_TRIPLE_PLAY:

		break;
	case ZMHD_GOBBLEGUM_REWARDS:
		Dvar_SetFromString("zmhd_gobblegum_popup_ps4_enabled", "1", true);
		Dvar_SetFromString("zmhd_gobblegum_popup_xb1_enabled", "1", true);
		break;
	case SUPPLY_DROP_GRAND_SLAM:
		Dvar_SetFromString("loot_limitedTimeItemPromo_active", "1", true);
		Dvar_SetFromString("loot_limitedTimeItemPromo_promoString", "Scropts", true);
		Dvar_SetFromString("loot_limitedTimeItemPromo_promoIcon", "t7_icon_blackmarket_promo_grandslam_green", true);
		Dvar_SetFromString("loot_grandSlam_active", "1", true);
		break;
	case SUPPLY_DROP_NO_DUPE:

		break;
	case SUPPLY_DROP_NO_DUPE_20:
		Dvar_SetFromString("loot_noDupeRare20Bundle_active", "1", true);
		Dvar_SetFromString("loot_noduperare20bundle_drop_id", "99062", true);
		Dvar_SetFromString("loot_noduperare20bundle_dwid", "15", true);
		break;
	case ZM_GOBBLEGUM_CHALLENGE:

		break;
	case FILESHARE_ALLOWDOWNLOAD:
		Dvar_SetFromString("fileshareAllowDownload", "1", true);
		Dvar_SetFromString("fileshareAllowDownloadingOthersFiles", "1", true);
		Dvar_SetFromString("fileshareAllowPaintjobDownload", "1", true);
		Dvar_SetFromString("fileshareAllowVariantDownload", "1", true);
		Dvar_SetFromString("fileshareAllowEmblemDownload", "1", true);
		Dvar_SetFromString("fileshareXuidOverride", LiveUser_GetXuidString(0), true);
		break;
	case FILESHARE_DISABLEDOWNLOAD:
		Dvar_SetFromString("fileshareAllowDownload", "0", true);
		Dvar_SetFromString("fileshareAllowDownloadingOthersFiles", "0", true);
		Dvar_SetFromString("fileshareAllowPaintjobDownload", "0", true);
		Dvar_SetFromString("fileshareAllowVariantDownload", "0", true);
		Dvar_SetFromString("fileshareAllowEmblemDownload", "0", true);
		//Dvar_SetFromString("fileshareXuidOverride", LiveUser_GetXuidString(0), true);
		break;
	case FORCEHOST_ENABLE:
		Dvar_SetFromString("party_minPlayers", "1", true);
		Dvar_SetFromString("lobbyDedicatedSearchSkip", "1", true);
		Dvar_SetFromString("lobbySearchTeamSize", "1", true);
		Dvar_SetFromString("lobbySearchSkip", "1", true);
		Dvar_SetFromString("lobbyMergeDedicatedEnabled", "0", true);
		Dvar_SetFromString("lobbySearchListenCountries", "0,103,6,5,8,13,16,23,25,32,34,24,37,42,44,50,71,74,76,75,82,84,88,31,90,18,35", true);
		Dvar_SetFromString("excellentPing", "3", true);
		Dvar_SetFromString("goodPing", "4", true);
		Dvar_SetFromString("terriblePing", "5", true);
		Dvar_SetFromString("migration_forceHost", "1", true);
		Dvar_SetFromString("migration_minclientcount", "12", true);
		Dvar_SetFromString("party_connectToOthers", "0", true);
		Dvar_SetFromString("party_dedicatedOnly", "0", true);
		Dvar_SetFromString("party_dedicatedMergeMinPlayers", "12", true);
		Dvar_SetFromString("party_forceMigrateAfterRound", "0", true);
		Dvar_SetFromString("party_forceMigrateOnMatchStartRegression", "0", true);
		Dvar_SetFromString("party_joinInProgressAllowed", "1", true);
		Dvar_SetFromString("allowAllNAT", "1", true);
		Dvar_SetFromString("party_keepPartyAliveWhileMatchmaking", "1", true);
		Dvar_SetFromString("party_mergingEnabled", "0", true);
		Dvar_SetFromString("party_neverJoinRecent", "1", true);
		Dvar_SetFromString("party_readyPercentRequired", ".25", true);
		Dvar_SetFromString("partyMigrate_disabled", "1", true);
		break;
	case FORCEHOST_DISABLE:
		Dvar_SetFromString("party_minPlayers", "6", true);
		Dvar_SetFromString("lobbyDedicatedSearchSkip", "0", true);
		Dvar_SetFromString("lobbySearchTeamSize", "0", true);
		Dvar_SetFromString("lobbySearchSkip", "0", true);
		Dvar_SetFromString("lobbyMergeDedicatedEnabled", "1", true);
		Dvar_SetFromString("lobbySearchListenCountries", "0", true);
		Dvar_SetFromString("excellentPing", "30", true);
		Dvar_SetFromString("goodPing", "100", true);
		Dvar_SetFromString("terriblePing", "500", true);
		Dvar_SetFromString("migration_forceHost", "0", true);
		Dvar_SetFromString("migration_minclientcount", "2", true);
		Dvar_SetFromString("party_connectToOthers", "1", true);
		Dvar_SetFromString("party_dedicatedOnly", "0", true);
		Dvar_SetFromString("party_dedicatedMergeMinPlayers", "2", true);
		Dvar_SetFromString("party_forceMigrateAfterRound", "0", true);
		Dvar_SetFromString("party_forceMigrateOnMatchStartRegression", "0", true);
		Dvar_SetFromString("party_joinInProgressAllowed", "1", true);
		Dvar_SetFromString("allowAllNAT", "1", true);
		Dvar_SetFromString("party_keepPartyAliveWhileMatchmaking", "1", true);
		Dvar_SetFromString("party_mergingEnabled", "1", true);
		Dvar_SetFromString("party_neverJoinRecent", "0", true);
		Dvar_SetFromString("party_readyPercentRequired", "1.0", true);
		Dvar_SetFromString("partyMigrate_disabled", "0", true);
		break;
	default:
		break;
	}

}


// CAC

void setClassSetItem(int itemId, int iComboBoxtype, int classSetIndex, int classIndex, const char* slotName) {

	int item = itemId;

	switch (iComboBoxType) {
	case CBC_ATTACHMENTS:
		break;
	case CBC_WEAPONTYPE:
		if (itemId >= 73)
			item += 1;
		if (itemId == 131)
			item = 147;
		break;
	case CBC_RETICLES:
		item += 140;
		break;
	case CBC_PERK:
		item += 148;
		break;
	case CBC_BONUSCARD:
		item += 178;
		break;
	case CBC_KILLSTREAK:
		item += 198;
		if (item > 208) {
			item += 1;
		}
		break;
	case CBC_EXTRA:
		item += 228;
		break;
	default:
		break;
	}

	eGameModes a = Com_SessionMode_GetGameMode();
	eModes a1 = Com_SessionMode_GetMode();
	auto classSetType = LiveStats_ClassSets_GetClassSetTypeForMode(a1, a);

	BG_UnlockablesSetClassSetItem(CONTROLLER_INDEX_0, classSetType, classSetIndex, classIndex, slotName, item);

}

void setClassSlotByName(int itemId, int classIndex, const char* slotName) {

	CACRoot Buffer{};
	eGameModes a = Com_SessionMode_GetGameMode();
	eModes a1 = Com_SessionMode_GetMode();
	CACType a3 = LiveStats_Loadouts_GetCACTypeForMode(a1, a);
	auto CACroot = LiveStats_Loadouts_GetCACRoot(&Buffer, 0, a3);
	__int64 a4 = BG_UnlockablesGetLoadoutSlotFromString(slotName);
	BG_UnlockablesSetItemIndex(&Buffer, (unsigned int)classIndex, a4, itemId);

}

void setClassItem(int ItemId, int slotItem, int classIndex) {

	CACRoot Buffer{};
	eGameModes a = Com_SessionMode_GetGameMode();
	eModes a1 = Com_SessionMode_GetMode();
	CACType a3 = LiveStats_Loadouts_GetCACTypeForMode(a1, a);
	auto CACroot = LiveStats_Loadouts_GetCACRoot(&Buffer, 0, a3);
	__int64 a4 = BG_UnlockablesGetLoadoutSlotFromString(slots[slotItem]);
	BG_UnlockablesSetItemIndex(&Buffer, (unsigned int)classIndex, a4, ItemId);
	ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "setClassItem: Successfully set item %i in %s (Class index %i)", ItemId, slots[slotItem], classIndex });

}

void setGobblePackName(int gobblePackIndex, std::string gobblePackName) {

	CACRoot Buffer{};
	eGameModes a = Com_SessionMode_GetGameMode();
	eModes a1 = Com_SessionMode_GetMode();
	CACType a3 = LiveStats_Loadouts_GetCACTypeForMode(a1, a);
	auto CACroot = LiveStats_Loadouts_GetCACRoot(&Buffer, 0, a3);
	BG_UnlockablesSetBubbleGumPackName(&Buffer, gobblePackIndex, gobblePackName.c_str());
}

void setClassName(int classSetIndex, int classIndex, std::string className) {

	eGameModes a = Com_SessionMode_GetGameMode();
	eModes a1 = Com_SessionMode_GetMode();
	ClassSetType_t a3 = LiveStats_ClassSets_GetClassSetTypeForMode(a1, a);

	std::string cmd = "";

	switch (a3) {
	case CLASS_SET_TYPE_MP_PUBLIC:
		cmd = "storagewriteddl class_sets_mp publicmatchclasssets " + std::to_string(classSetIndex) + " customclassname " + std::to_string(classIndex) + " \"" + className + "\"";
		//ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "%s", cmd.c_str()});
		break;
	case CLASS_SET_TYPE_MP_CUSTOM:
		cmd = "storagewriteddl class_sets_mp custommatchclasssets " + std::to_string(classSetIndex) + " customclassname " + std::to_string(classIndex) + " \"" + className + "\"";
		//ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "%s", cmd.c_str()});
		break;
	case CLASS_SET_TYPE_MP_ARENA:
		cmd = "storagewriteddl class_sets_mp leaguematchclasssets " + std::to_string(classSetIndex) + " customclassname " + std::to_string(classIndex) + " \"" + className + "\"";
		//ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "%s", cmd.c_str()});
		break;
	default:
		cmd = "";
		break;
	}

	Cbuf_AddText(0, cmd.c_str());


}

static CACSlot primarySlotsTable[10] =
{
	{ primaryMenuNames[0], primarySlots[0], true, CBC_WEAPONTYPE, primaryPreviews, primarySelectedItems, primarySelectedSlot },
	{ primaryMenuNames[1], primarySlots[1], true, CBC_ATTACHMENTS, primaryPreviews, primarySelectedItems, primarySelectedSlot },
	{ primaryMenuNames[2], primarySlots[2], true, CBC_ATTACHMENTS, primaryPreviews, primarySelectedItems, primarySelectedSlot },
	{ primaryMenuNames[3], primarySlots[3], true, CBC_ATTACHMENTS, primaryPreviews, primarySelectedItems, primarySelectedSlot },
	{ primaryMenuNames[4], primarySlots[4], true, CBC_ATTACHMENTS, primaryPreviews, primarySelectedItems, primarySelectedSlot },
	{ primaryMenuNames[5], primarySlots[5], true, CBC_ATTACHMENTS, primaryPreviews, primarySelectedItems, primarySelectedSlot },
	{ primaryMenuNames[6], primarySlots[6], true, CBC_CAMOS, primaryPreviews, primarySelectedItems, primarySelectedSlot },
	{ primaryMenuNames[7], primarySlots[7], true, CBC_RETICLES, primaryPreviews, primarySelectedItems, primarySelectedSlot },
	{ primaryMenuNames[8], primarySlots[8], true, CBC_ATTACHMENTS, primaryPreviews, primarySelectedItems, primarySelectedSlot },
	{ primaryMenuNames[9], primarySlots[9], true, CBC_ATTACHMENTS, primaryPreviews, primarySelectedItems, primarySelectedSlot },
};

static CACSlot secondarySlotsTable[10] =
{
	{ secondaryMenuNames[0], secondarySlots[0], false, CBC_WEAPONTYPE, secondaryPreviews, secondarySelectedItems, secondarySelectedSlot },
	{ secondaryMenuNames[1], secondarySlots[1], false, CBC_ATTACHMENTS, secondaryPreviews, secondarySelectedItems, secondarySelectedSlot },
	{ secondaryMenuNames[2], secondarySlots[2], false, CBC_ATTACHMENTS, secondaryPreviews, secondarySelectedItems, secondarySelectedSlot },
	{ secondaryMenuNames[3], secondarySlots[3], false, CBC_ATTACHMENTS, secondaryPreviews, secondarySelectedItems, secondarySelectedSlot },
	{ secondaryMenuNames[4], secondarySlots[4], false, CBC_ATTACHMENTS, secondaryPreviews, secondarySelectedItems, secondarySelectedSlot },
	{ secondaryMenuNames[5], secondarySlots[5], false, CBC_ATTACHMENTS, secondaryPreviews, secondarySelectedItems, secondarySelectedSlot },
	{ secondaryMenuNames[6], secondarySlots[6], false, CBC_CAMOS, secondaryPreviews, secondarySelectedItems, secondarySelectedSlot },
	{ secondaryMenuNames[7], secondarySlots[7], false, CBC_RETICLES, secondaryPreviews, secondarySelectedItems, secondarySelectedSlot },
	{ secondaryMenuNames[8], secondarySlots[8], false, CBC_ATTACHMENTS, secondaryPreviews, secondarySelectedItems, secondarySelectedSlot },
	{ secondaryMenuNames[9], secondarySlots[9], false, CBC_ATTACHMENTS, secondaryPreviews, secondarySelectedItems, secondarySelectedSlot },
};

static CACSlot offhandSlotsTable[8] =
{
	{ offhandMenuNames[0], offhandSlots[0], false, CBC_EXTRA, offhandPreviews, offhandSelectedItems, offhandSelectedSlot },
	{ offhandMenuNames[1], offhandSlots[1], false, CBC_EXTRA, offhandPreviews, offhandSelectedItems, offhandSelectedSlot },
	{ offhandMenuNames[2], offhandSlots[2], false, CBC_EXTRA, offhandPreviews, offhandSelectedItems, offhandSelectedSlot },
	{ offhandMenuNames[3], offhandSlots[3], false, CBC_EXTRA, offhandPreviews, offhandSelectedItems, offhandSelectedSlot },
	{ offhandMenuNames[4], offhandSlots[4], false, CBC_EXTRA, offhandPreviews, offhandSelectedItems, offhandSelectedSlot },
	{ offhandMenuNames[5], offhandSlots[5], false, CBC_EXTRA, offhandPreviews, offhandSelectedItems, offhandSelectedSlot },
	{ offhandMenuNames[6], offhandSlots[6], false, CBC_EXTRA, offhandPreviews, offhandSelectedItems, offhandSelectedSlot },
	{ offhandMenuNames[7], offhandSlots[7], false, CBC_EXTRA, offhandPreviews, offhandSelectedItems, offhandSelectedSlot },
};

static CACSlot wildcardSlotsTable[9] =
{
	{ wildcardMenuNames[0], wildcardSlots[0], false, CBC_PERK, wildcardPreviews, wildcardSelectedItems, wildcardSelectedSlot },
	{ wildcardMenuNames[1], wildcardSlots[1], false, CBC_PERK, wildcardPreviews, wildcardSelectedItems, wildcardSelectedSlot },
	{ wildcardMenuNames[2], wildcardSlots[2], false, CBC_PERK, wildcardPreviews, wildcardSelectedItems, wildcardSelectedSlot },
	{ wildcardMenuNames[3], wildcardSlots[3], false, CBC_PERK, wildcardPreviews, wildcardSelectedItems, wildcardSelectedSlot },
	{ wildcardMenuNames[4], wildcardSlots[4], false, CBC_PERK, wildcardPreviews, wildcardSelectedItems, wildcardSelectedSlot },
	{ wildcardMenuNames[5], wildcardSlots[5], false, CBC_PERK, wildcardPreviews, wildcardSelectedItems, wildcardSelectedSlot },
	{ wildcardMenuNames[6], wildcardSlots[6], false, CBC_PERK, wildcardPreviews, wildcardSelectedItems, wildcardSelectedSlot },
	{ wildcardMenuNames[7], wildcardSlots[7], false, CBC_BONUSCARD, wildcardPreviews, wildcardSelectedItems, wildcardSelectedSlot },
	{ wildcardMenuNames[8], wildcardSlots[8], false, CBC_BONUSCARD, wildcardPreviews, wildcardSelectedItems, wildcardSelectedSlot },
};

void DrawSlotTable(CACSlot* table, int count, int classSetIndex, int classIndex)
{
	for (int j = 0; j < count; j++)
	{
		auto& slot = table[j];

		ImGui::Text(slot.label);

		if (slot.previews[j].empty())
			slot.previews[j] = "None";

		std::string comboId = "##combo_" + std::to_string(j);
		std::string buttonId = "Send##btn_" + std::to_string(j);

		if (ImGui::BeginCombo(comboId.c_str(), slot.previews[j].c_str()))
		{
			int limit = getComboItemSize(slot.isPrimary);

			for (int i = 0; i < limit; i++)
			{
				const char* name = getComboItemName(i, slot.isPrimary, iComboBoxType);

				if (!name) continue;

				bool selected = (i == slot.selectedItems[j]);

				std::string label = std::string(name) + "##" + std::to_string(i);

				if (ImGui::Selectable(label.c_str(), selected))
				{
					slot.previews[j] = name;
					slot.selectedItems[j] = i;
					slot.selectedSlots[j] = iComboBoxType;
				}
			}

			ImGui::EndCombo();
		}

		ImGui::SameLine();

		if (ImGui::Button(buttonId.c_str()))
		{
			setClassSetItem(
				slot.selectedItems[j],
				slot.selectedSlots[j],
				classSetIndex,
				classIndex,
				slot.slotName
			);
		}
	}
}


static int tab = 0;

const int tabCount = 9;

bool jumped = false;

std::filesystem::path MakeUniquePath(const std::filesystem::path& desiredPath)
{
	if (!std::filesystem::exists(desiredPath))
		return desiredPath;

	const auto parent = desiredPath.parent_path();
	const auto stem = desiredPath.stem().string();
	const auto extension = desiredPath.extension().string();

	for (int i = 1; i < 10000; ++i)
	{
		std::filesystem::path candidate =
			parent / (stem + " (" + std::to_string(i) + ")" + extension);

		if (!std::filesystem::exists(candidate))
			return candidate;
	}

	const auto timestamp =
		std::to_string(std::chrono::system_clock::now().time_since_epoch().count());

	return parent / (stem + "_" + timestamp + extension);
}

static void ClampInt(int& value, int minValue, int maxValue)
{
	if (value < minValue)
		value = minValue;

	if (value > maxValue)
		value = maxValue;
}

static bool InputIntClamped(const char* label, int* value, int minValue, int maxValue)
{
	const bool changed = ImGui::InputInt(label, value);

	ClampInt(*value, minValue, maxValue);

	return changed;
}

static int StringToIntSafe(const char* value, int fallback = 0)
{
	if (!value || !*value)
		return fallback;

	return atoi(value);
}

static StringTable* GetRankTableForMode(int mode)
{
	StringTable* table = nullptr;

	if (mode == MODE_CAMPAIGN) {
		StringTable_GetAsset("gamedata/tables/cp/cp_ranktable.csv", &table);
	}
	else if (mode == MODE_MULTIPLAYER) {
		StringTable_GetAsset("gamedata/tables/mp/mp_ranktable.csv", &table);
	}
	else if (mode == MODE_ZOMBIES) {
		StringTable_GetAsset("gamedata/tables/zm/zm_ranktable.csv", &table);
	}

	return table;
}

static StringTable* GetParagonRankTableForMode(int mode)
{
	StringTable* table = nullptr;

	if (mode == MODE_MULTIPLAYER) {
		StringTable_GetAsset("gamedata/tables/mp/mp_paragonranktable.csv", &table);
	}
	else if (mode == MODE_ZOMBIES) {
		StringTable_GetAsset("gamedata/tables/zm/zm_paragonranktable.csv", &table);
	}

	return table;
}

static StringTable* GetRankIconTableForMode(int mode)
{
	StringTable* table = nullptr;

	if (mode == MODE_CAMPAIGN) {
		StringTable_GetAsset("gamedata/tables/cp/cp_rankicontable.csv", &table);
	}
	else if (mode == MODE_MULTIPLAYER) {
		StringTable_GetAsset("gamedata/tables/mp/mp_paragonrankicontable.csv", &table);
	}
	else if (mode == MODE_ZOMBIES) {
		StringTable_GetAsset("gamedata/tables/zm/zm_paragonrankicontable.csv", &table);
	}

	return table;
}

static bool RankIconExistsForMode(int mode, int icon)
{
	StringTable* table = GetRankIconTableForMode(mode);
	if (!table)
		return false;

	char lookup[16]{};
	sprintf_s(lookup, "%d", icon);

	const char* result = StringTable_Lookup(table, 0, lookup, 0);

	return result && *result;
}

static int GetRankXPFromTable(int mode, int level)
{
	StringTable* table = GetRankTableForMode(mode);
	if (!table)
		return 0;

	char lookup[16]{};
	sprintf_s(lookup, "%d", level);

	const char* xpValue = StringTable_Lookup(table, 0, lookup, 7);

	return StringToIntSafe(xpValue, 0);
}

static int GetParagonXPFromTable(int mode, int visibleParagonLevel)
{
	StringTable* table = GetParagonRankTableForMode(mode);
	if (!table)
		return 0;

	char lookup[16]{};
	sprintf_s(lookup, "%d", visibleParagonLevel);

	const char* xpValue = StringTable_Lookup(table, 13, lookup, 2);

	return StringToIntSafe(xpValue, 0);
}

static void UpdateRankXPFromInputs(int mode, int level, int paragonRank, int& rankXp, int& paragonRankXp)
{
	rankXp = GetRankXPFromTable(mode, level);

	if (mode == MODE_CAMPAIGN) {
		paragonRankXp = 0;
	}
	else {
		paragonRankXp = GetParagonXPFromTable(mode, paragonRank);
	}
}

static int GetParagonInternalRankFromTable(int mode, int visibleParagonRank)
{
	StringTable* table = GetParagonRankTableForMode(mode);
	if (!table)
		return visibleParagonRank - minRank;

	char lookup[16]{};
	sprintf_s(lookup, "%d", visibleParagonRank);

	const char* internalValue = StringTable_Lookup(table, 13, lookup, 0);

	return StringToIntSafe(internalValue, visibleParagonRank - minRank);
}

static void SetDDLUInt(const char* statName, int value)
{
	auto root = LiveStats_Core_GetRootDDLState(GetSessionState());
	auto statsBuffer = GetStatsBuffer(0);

	const char* path[3]{};
	path[0] = "PlayerStatsList";
	path[1] = statName;
	path[2] = bArena ? "arenaValue" : "statValue";

	char result[2000]{};

	DDL_MoveToPath(root, result, 3, path);
	DDL_SetUInt(reinterpret_cast<__int64>(result), statsBuffer, value);
}

void setAllRanks()
{
	const int mode = Com_SessionMode_GetMode();

	ClampInt(pPrestige, 0, 11);

	if (mode == MODE_MULTIPLAYER)
		ClampInt(pLevel, 0, 54);
	else if (mode == MODE_ZOMBIES)
		ClampInt(pLevel, 0, 34);
	else if (mode == MODE_CAMPAIGN)
		ClampInt(pLevel, 0, 19);

	ClampInt(ParagonRank, minRank, 1000);

	rankXp = GetRankXPFromTable(mode, pLevel);

	if (mode == MODE_CAMPAIGN) {
		paragonRankXp = 0;
	}
	else {
		paragonRankXp = GetParagonXPFromTable(mode, ParagonRank);
	}

	const int internalParagonRank =
		mode == MODE_CAMPAIGN
		? 0
		: GetParagonInternalRankFromTable(mode, ParagonRank);

	SetDDLUInt("plevel", pPrestige);
	SetDDLUInt("rank", pLevel);
	SetDDLUInt("rankxp", rankXp);

	if (mode != MODE_CAMPAIGN) {
		SetDDLUInt("paragon_rank", internalParagonRank);
		SetDDLUInt("paragon_rankxp", paragonRankXp);
	}

	LiveStorage_UploadStatsForController(0);
}


#pragma pack(push, 1)

struct EmblemFileHeader
{
	char magic[8];              // "T7EMBLM"
	uint32_t version;           // 1
	uint32_t emblemSize;        // sizeof(DumpCompositeEmblem)
	uint32_t nameLength;        // bytes, no null terminator
	uint32_t crc32;             // CRC over name bytes + emblem bytes
};

#pragma pack(pop)

static_assert(sizeof(EmblemFileHeader) == 0x18);

constexpr char T7_EMBLEM_MAGIC[8] = { 'T', '7', 'E', 'M', 'B', 'L', 'M', '\0' };
constexpr uint32_t T7_EMBLEM_VERSION = 1;

inline uint32_t Crc32Update(uint32_t crc, const void* data, size_t size)
{
	const auto* bytes = static_cast<const uint8_t*>(data);

	crc = ~crc;

	for (size_t i = 0; i < size; ++i)
	{
		crc ^= bytes[i];

		for (int bit = 0; bit < 8; ++bit)
		{
			const uint32_t mask = 0u - (crc & 1u);
			crc = (crc >> 1) ^ (0xEDB88320u & mask);
		}
	}

	return ~crc;
}

inline uint32_t Crc32(const void* data, size_t size)
{
	return Crc32Update(0, data, size);
}

inline uint32_t Crc32EmblemPayload(std::string_view name, const CompositeEmblem& emblem)
{
	uint32_t crc = 0;

	crc = Crc32Update(crc, name.data(), name.size());
	crc = Crc32Update(crc, &emblem, sizeof(emblem));

	return crc;
}

inline std::string SanitizeFileName(std::string name)
{
	if (name.empty())
		name = "Unnamed";

	for (char& c : name)
	{
		switch (c)
		{
		case '<':
		case '>':
		case ':':
		case '"':
		case '/':
		case '\\':
		case '|':
		case '?':
		case '*':
			c = '_';
			break;
		default:
			break;
		}
	}

	while (!name.empty() && (name.back() == ' ' || name.back() == '.'))
		name.pop_back();

	if (name.empty())
		name = "Unnamed";

	return name;
}

bool WriteEmblemFile(
	const std::filesystem::path& outPath,
	std::string_view emblemName,
	const CompositeEmblem& emblem)
{
	std::filesystem::path finalPath = outPath;

	if (finalPath.extension() != ".emblem")
	{
		const std::string safeName = SanitizeFileName(std::string(emblemName));
		finalPath /= safeName + ".emblem";
	}

	const auto parent = finalPath.parent_path();

	if (!parent.empty())
	{
		std::error_code ec;
		std::filesystem::create_directories(parent, ec);

		if (ec)
			return false;
	}

	finalPath = MakeUniquePath(finalPath);

	std::string storedName(emblemName);

	if (storedName.empty())
		storedName = finalPath.stem().string();

	if (storedName.size() > 255)
		storedName.resize(255);

	EmblemFileHeader header{};
	std::memcpy(header.magic, T7_EMBLEM_MAGIC, sizeof(header.magic));
	header.version = T7_EMBLEM_VERSION;
	header.emblemSize = static_cast<uint32_t>(sizeof(CompositeEmblem));
	header.nameLength = static_cast<uint32_t>(storedName.size());
	header.crc32 = Crc32EmblemPayload(storedName, emblem);

	std::ofstream file(finalPath, std::ios::binary | std::ios::trunc);

	if (!file)
		return false;

	file.write(reinterpret_cast<const char*>(&header), sizeof(header));

	if (!storedName.empty())
		file.write(storedName.data(), static_cast<std::streamsize>(storedName.size()));

	file.write(reinterpret_cast<const char*>(&emblem), sizeof(emblem));

	return file.good();
}

void InitDDLState(DDLState& state)
{
	std::memset(&state, 0, sizeof(state));
	state.arrayIndex = -1;
}

int GetEmblemInt(
	DDLState* root,
	DDLContext* ctx,
	int slot,
	const char* field,
	int fallback = 0)
{
	if (!root || !ctx || slot < 0 || slot >= 32 || !field)
		return fallback;

	DDLState state{};
	InitDDLState(state);

	const char* path[8]{};
	path[0] = "emblemList";
	path[1] = intToConstCharPtr(slot);
	path[2] = field;

	if (!DDL_MoveToPath((__int64)root, (char*)&state, 3, path))
		return fallback;

	return DDL_GetUInt((__int64)&state, (__int64)ctx);
}

bool SetEmblemInt(
	DDLState* root,
	DDLContext* ctx,
	int slot,
	const char* field,
	int value)
{
	DDLState state{};
	InitDDLState(state);
	const char* path[8];
	path[0] = "emblemList";
	path[1] = intToConstCharPtr(slot);
	path[2] = field;

	if (!DDL_MoveToPath((__int64)root, (char*)&state, 3, path))
		return false;

	DDL_SetInt((__int64)&state, (__int64)ctx, value);
	return true;
}

bool SetEmblemString(
	DDLState* root,
	DDLContext* ctx,
	int slot,
	const char* field,
	const char* value)
{
	DDLState state{};
	InitDDLState(state);
	const char* path[8];
	path[0] = "emblemList";
	path[1] = intToConstCharPtr(slot);
	path[2] = field;

	if (!DDL_MoveToPath((__int64)root, (char*)&state, 3, path))
		return false;

	DDL_SetString((__int64)&state, (__int64)ctx, value);
	return true;
}

bool RenameEmblem(int slot, std::string emblemName)
{
	DDLContext* ddlCtx = Storage_GetDDLContext(0, 26, 0);
	DDLState* rootState = Storage_GetDDLRootState(26);

	return SetEmblemString(rootState, ddlCtx, slot, "emblemName", emblemName.c_str());
}

std::string GetEmblemString(
	DDLState* root,
	DDLContext* ctx,
	int slot,
	const char* field)
{
	if (!root || !ctx || slot < 0 || slot >= 32 || !field)
		return {};

	DDLState state{};
	InitDDLState(state);

	const char* path[8]{};
	path[0] = "emblemList";
	path[1] = intToConstCharPtr(slot);
	path[2] = field;

	if (!DDL_MoveToPath((__int64)root, (char*)&state, 3, path))
		return {};

	const char* value = DDL_GetString((__int64)&state, (__int64)ctx);

	if (!value)
		return {};

	return value;
}

std::string GetEmblemName(int slot)
{

	constexpr StorageFileType STORAGE_EMBLEMS = static_cast<StorageFileType>(26);

	DDLState* root = Storage_GetDDLRootState(STORAGE_EMBLEMS);
	DDLContext* ctx = Storage_GetDDLContext(0, STORAGE_EMBLEMS, 0);

	std::string name = GetEmblemString(root, ctx, slot, "emblemName");
}

int GetHighestEmblemSortIndex(DDLState* root, DDLContext* ctx)
{
	int maxSort = 0;

	for (int i = 0; i < 32; ++i)
	{
		DDLState state{};
		InitDDLState(state);
		const char* path[8];
		path[0] = "emblemList";
		path[1] = intToConstCharPtr(i);
		path[2] = "sortIndex";

		if (DDL_MoveToPath((__int64)root, (char*)&state, 3, path)) {
			const int sort = DDL_GetUInt((__int64)&state, (__int64)ctx);
			if (sort > maxSort)
				maxSort = sort;
		}
	}

	return maxSort;
}

inline int GetUsedEmblemLayerCount(const CompositeEmblem& emblem)
{
	const auto* raw = reinterpret_cast<const uint8_t*>(&emblem);

	int highestUsed = -1;

	for (int i = 0; i < 64; ++i)
	{
		const uint8_t* layer = raw + i * 0x60;

		const int16_t materialID = *reinterpret_cast<const int16_t*>(layer + 0x00);
		const int16_t icon = *reinterpret_cast<const int16_t*>(layer + 0x5C);

		if (materialID != -1 || icon != -1)
			highestUsed = i;
	}

	return highestUsed + 1;
}

inline bool HasAnyEmblemLayers(const CompositeEmblem& emblem)
{
	return GetUsedEmblemLayerCount(emblem) > 0;
}

std::filesystem::path GetGameDirectory()
{
	wchar_t path[MAX_PATH]{};

	const DWORD len = GetModuleFileNameW(nullptr, path, MAX_PATH);

	if (len == 0 || len >= MAX_PATH)
		return std::filesystem::current_path();

	std::filesystem::path exePath(path);
	return exePath.parent_path();
}

std::filesystem::path GetEmblemsDirectory()
{
	return GetGameDirectory() / "emblems";
}

bool IsEmblemSlotUsed(DDLState* root, DDLContext* ctx, int slot)
{
	DDLState state{};
	InitDDLState(state);
	const char* path[8];
	path[0] = "emblemList";
	path[1] = intToConstCharPtr(slot);
	path[2] = "isUsed";

	if (!DDL_MoveToPath((__int64)root, (char*)&state, 3, path))
		return true;

	return DDL_GetUInt((__int64)&state, (__int64)ctx) != 0;
}

struct ReadEmblemFileResult
{
	std::string name;
	CompositeEmblem emblem{};
};

bool ReadEmblemFile(
	const std::filesystem::path& path,
	ReadEmblemFileResult& out)
{
	std::ifstream file(path, std::ios::binary);

	if (!file)
		return false;

	EmblemFileHeader header{};
	file.read(reinterpret_cast<char*>(&header), sizeof(header));

	if (!file)
		return false;

	if (std::memcmp(header.magic, T7_EMBLEM_MAGIC, sizeof(header.magic)) != 0)
		return false;

	if (header.version != T7_EMBLEM_VERSION)
		return false;

	if (header.emblemSize != sizeof(CompositeEmblem))
		return false;

	if (header.nameLength > 255)
		return false;

	std::string name;
	name.resize(header.nameLength);

	if (header.nameLength > 0)
		file.read(name.data(), static_cast<std::streamsize>(header.nameLength));

	if (!file)
		return false;

	CompositeEmblem emblem{};
	file.read(reinterpret_cast<char*>(&emblem), sizeof(emblem));

	if (!file)
		return false;

	const uint32_t actualCrc = Crc32EmblemPayload(name, emblem);

	if (actualCrc != header.crc32)
		return false;

	out.name = std::move(name);
	out.emblem = emblem;

	return true;
}

struct EmblemFileEntry
{
	std::filesystem::path path;
	std::string fileName;
	std::string displayName;
};

struct EmblemImportBrowser
{
	std::vector<EmblemFileEntry> files;
	int selectedIndex = -1;
};

static EmblemImportBrowser g_emblemImportBrowser;

struct EmblemSlotEntry
{
	int slot = -1;
	int sortIndex = 0;
	bool isUsed = false;

	std::string name;
	std::string displayName;
};

struct EmblemExportBrowser
{
	std::vector<EmblemSlotEntry> slots;
	int selectedIndex = -1;
	bool showEmptySlots = false;
};

static EmblemExportBrowser g_emblemExportBrowser;

void RefreshEmblemFileList()
{
	g_emblemImportBrowser.files.clear();
	g_emblemImportBrowser.selectedIndex = -1;

	const auto dir = GetEmblemsDirectory();

	std::error_code ec;
	std::filesystem::create_directories(dir, ec);

	if (ec)
		return;

	for (const auto& entry : std::filesystem::directory_iterator(dir, ec))
	{
		if (ec)
			break;

		if (!entry.is_regular_file())
			continue;

		const auto& path = entry.path();

		if (path.extension() != ".emblem")
			continue;

		EmblemFileEntry item{};
		item.path = path;
		item.fileName = path.filename().string();

		ReadEmblemFileResult preview{};

		if (ReadEmblemFile(path, preview) && !preview.name.empty())
			item.displayName = preview.name + " (" + item.fileName + ")";
		else
			item.displayName = item.fileName + " (invalid)";

		g_emblemImportBrowser.files.push_back(std::move(item));
	}

	std::sort(
		g_emblemImportBrowser.files.begin(),
		g_emblemImportBrowser.files.end(),
		[](const EmblemFileEntry& a, const EmblemFileEntry& b)
		{
			return a.fileName < b.fileName;
		}
	);

	if (!g_emblemImportBrowser.files.empty())
		g_emblemImportBrowser.selectedIndex = 0;
}

void RefreshEmblemSlotList(ControllerIndex_t controller)
{
	g_emblemExportBrowser.slots.clear();
	g_emblemExportBrowser.selectedIndex = -1;

	constexpr StorageFileType STORAGE_EMBLEMS = static_cast<StorageFileType>(26);

	DDLState* root = Storage_GetDDLRootState(STORAGE_EMBLEMS);
	DDLContext* ctx = Storage_GetDDLContext(controller, STORAGE_EMBLEMS, 0);

	if (!root || !ctx)
		return;

	for (int slot = 0; slot < 32; ++slot)
	{
		const bool isUsed = IsEmblemSlotUsed(root, ctx, slot);

		if (!g_emblemExportBrowser.showEmptySlots && !isUsed)
			continue;

		const int sortIndex = GetEmblemInt(root, ctx, slot, "sortIndex", 0);

		std::string name = GetEmblemString(root, ctx, slot, "emblemName");

		if (name.empty())
			name = "Slot_" + std::to_string(slot);

		EmblemSlotEntry entry{};
		entry.slot = slot;
		entry.sortIndex = sortIndex;
		entry.isUsed = isUsed;
		entry.name = name;

		char display[256]{};
		std::snprintf(
			display,
			sizeof(display),
			"%sSlot %02d | %s",
			isUsed ? "" : "[Empty] ",
			slot,
			name.c_str()
		);

		entry.displayName = display;

		g_emblemExportBrowser.slots.push_back(std::move(entry));
	}

	std::sort(
		g_emblemExportBrowser.slots.begin(),
		g_emblemExportBrowser.slots.end(),
		[](const EmblemSlotEntry& a, const EmblemSlotEntry& b)
		{
			if (a.isUsed != b.isUsed)
				return a.isUsed > b.isUsed;

			if (a.sortIndex != b.sortIndex)
				return a.sortIndex < b.sortIndex;

			return a.slot < b.slot;
		}
	);

	if (!g_emblemExportBrowser.slots.empty())
		g_emblemExportBrowser.selectedIndex = 0;
}

bool ImportEmblemFileBackend(
	ControllerIndex_t controller,
	int slot,
	const std::filesystem::path& filePath,
	bool allowOverwrite)
{
	constexpr StorageFileType STORAGE_EMBLEMS = static_cast<StorageFileType>(26);

	if (slot < 0 || slot >= 32)
		return false;

	ReadEmblemFileResult imported{};

	if (!ReadEmblemFile(filePath, imported))
		return false;

	DDLContext* ddlCtx = Storage_GetDDLContext(controller, STORAGE_EMBLEMS, 0);
	DDLState* rootState = Storage_GetDDLRootState(STORAGE_EMBLEMS);

	if (!ddlCtx || !rootState)
		return false;

	if (!allowOverwrite && IsEmblemSlotUsed(rootState, ddlCtx, slot))
		return false;

	const int maxSort = GetHighestEmblemSortIndex(rootState, ddlCtx);

	std::string importName = imported.name;

	if (customImportRename && !customImportName.empty())
		importName = customImportName;

	if (importName.empty())
		importName = filePath.stem().string();

	if (importName.size() > 255)
		importName.resize(255);

	if (!SetEmblemString(rootState, ddlCtx, slot, "emblemName", importName.c_str()))
		return false;

	if (!SetEmblemInt(rootState, ddlCtx, slot, "isUsed", 1))
		return false;

	if (!SetEmblemInt(rootState, ddlCtx, slot, "sortIndex", maxSort + 1))
		return false;

	SetEmblemInt(rootState, ddlCtx, slot, "readOnly", 0);

	DDLState emblemState{};
	InitDDLState(emblemState);

	const char* path[8]{};
	path[0] = "emblemList";
	path[1] = intToConstCharPtr(slot);
	path[2] = "emblem";

	if (!DDL_MoveToPath((__int64)rootState, (char*)&emblemState, 3, path))
		return false;

	const int layerCount = 64;

	BG_PaintshopWriteDDL(
		(__int64)&emblemState,
		(__int64)ddlCtx,
		(__int64)&imported.emblem,
		layerCount
	);

	return Storage_Write(controller, STORAGE_EMBLEMS, 0);
}

bool ExportEmblemBackend(
	ControllerIndex_t controller,
	int slot,
	const std::filesystem::path& outPath)
{
	constexpr StorageFileType STORAGE_EMBLEMS = static_cast<StorageFileType>(26);
	constexpr int CUSTOMIZATION_TYPE_EMBLEM = 3;

	if (slot < 0 || slot >= 32)
		return false;

	CompositeEmblem emblem{};
	BG_InitCompositeEmblem(&emblem);

	Live_Emblems_GetEmblemData(
		controller,
		CUSTOMIZATION_TYPE_EMBLEM,
		slot,
		STORAGE_EMBLEMS,
		&emblem
	);

	DDLContext* ddlCtx = Storage_GetDDLContext(controller, STORAGE_EMBLEMS, 0);
	DDLState* rootState = Storage_GetDDLRootState(STORAGE_EMBLEMS);

	const std::string name = GetEmblemString(rootState, ddlCtx, slot, "emblemName");

	return WriteEmblemFile(outPath, name, emblem);
}

// ---------------------------------- Emblem Renderer ---------------------------------------

namespace EmblemPreview
{
	inline constexpr std::uintptr_t COMPOSITE_JOBS_RVA = 0x52CA460;

	inline constexpr std::ptrdiff_t GFXIMAGE_SRV_OFFSET = 0xA8;

	inline constexpr int COMPOSITE_JOB_COUNT = 16;
	inline constexpr int EMBLEM_LAYER_CAPACITY = 64;
	inline constexpr std::size_t EMBLEM_LAYER_SIZE = 0x60;
	inline constexpr std::uint32_t EMBLEM_PREVIEW_FLAGS = 0x80000000u;

	inline std::uint64_t g_nextPreviewAllowedMs = 0;
	inline constexpr std::uint64_t PREVIEW_SUBMIT_COOLDOWN_MS = 300;
	inline bool g_wasInGameForPreview = false;
	inline bool g_didPostMatchPreviewReset = false;
	inline std::uint64_t g_lobbyStableSinceMs = 0;

	inline CompositeJob* GetCompositeJobs()
	{
		return reinterpret_cast<CompositeJob*>(OFFSET(COMPOSITE_JOBS_RVA));
	}

	inline int FindJobSlotById_NoLock(CompositeJobID id)
	{
		if (id == 0)
			return -1;

		CompositeJob* jobs = GetCompositeJobs();

		for (int i = 0; i < COMPOSITE_JOB_COUNT; ++i)
		{
			if (jobs[i].id == id)
				return i;
		}

		return -1;
	}


	enum class SourceKind : std::uint8_t
	{
		None,
		BackendSlot,
		EmblemFile
	};

	struct PreviewJob
	{
		bool active = false;
		bool ready = false;

		bool cancelRequested = false;
		bool releaseRequested = false;

		CompositeJobID id = 0;
		int jobSlot = -1;

		GfxImage* image = nullptr;

		std::uint64_t submitTimeMs = 0;
		std::uint64_t lastLogMs = 0;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> previewSrv;

		SourceKind sourceKind = SourceKind::None;
		int backendSlot = -1;
		std::filesystem::path filePath;
	};

	struct PendingPreview
	{
		bool pending = false;

		CompositeEmblem emblem{};
		SourceKind sourceKind = SourceKind::None;
		int backendSlot = -1;
		std::filesystem::path filePath;
	};

	inline std::uint64_t NowMs()
	{
		return GetTickCount64();
	}

	inline constexpr std::uint64_t MIN_CANCEL_AGE_MS = 750;
	inline constexpr std::uint64_t HARD_CANCEL_AGE_MS = 15000;

	inline PendingPreview g_pendingPreview{};

	inline PreviewJob g_job{};

	inline bool g_previewMenuOpen = false;
	inline bool g_lifecycleReleaseRequested = false;

	struct PrivatePreviewTexture
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

		UINT width = 0;
		UINT height = 0;
		DXGI_FORMAT textureFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN;

		bool valid() const
		{
			return texture && srv;
		}

		void reset()
		{
			srv.Reset();
			texture.Reset();
			width = 0;
			height = 0;
			textureFormat = DXGI_FORMAT_UNKNOWN;
			srvFormat = DXGI_FORMAT_UNKNOWN;
		}
	};

	inline PrivatePreviewTexture g_privatePreview{};
	inline bool g_hasPrivatePreview = false;

	inline bool EnsurePrivatePreviewTexture(
		ID3D11Texture2D* srcTex,
		ID3D11ShaderResourceView* srcSrv)
	{
		if (!pDevice || !srcTex || !srcSrv)
			return false;

		D3D11_TEXTURE2D_DESC srcDesc{};
		srcTex->GetDesc(&srcDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC srcSrvDesc{};
		srcSrv->GetDesc(&srcSrvDesc);

		DXGI_FORMAT srvFormat = srcSrvDesc.Format;
		if (srvFormat == DXGI_FORMAT_UNKNOWN)
			srvFormat = srcDesc.Format;

		if (g_privatePreview.valid() &&
			g_privatePreview.width == srcDesc.Width &&
			g_privatePreview.height == srcDesc.Height &&
			g_privatePreview.textureFormat == srcDesc.Format &&
			g_privatePreview.srvFormat == srvFormat)
		{
			return true;
		}

		g_privatePreview.reset();

		D3D11_TEXTURE2D_DESC dstDesc{};
		dstDesc.Width = srcDesc.Width;
		dstDesc.Height = srcDesc.Height;
		dstDesc.MipLevels = 1;
		dstDesc.ArraySize = 1;
		dstDesc.Format = srcDesc.Format;
		dstDesc.SampleDesc.Count = 1;
		dstDesc.SampleDesc.Quality = 0;
		dstDesc.Usage = D3D11_USAGE_DEFAULT;
		dstDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		dstDesc.CPUAccessFlags = 0;
		dstDesc.MiscFlags = 0;

		if (FAILED(pDevice->CreateTexture2D(&dstDesc, nullptr, g_privatePreview.texture.GetAddressOf())))
			return false;

		D3D11_SHADER_RESOURCE_VIEW_DESC dstSrvDesc{};
		dstSrvDesc.Format = srvFormat;
		dstSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		dstSrvDesc.Texture2D.MostDetailedMip = 0;
		dstSrvDesc.Texture2D.MipLevels = 1;

		if (FAILED(pDevice->CreateShaderResourceView(
			g_privatePreview.texture.Get(),
			&dstSrvDesc,
			g_privatePreview.srv.GetAddressOf())))
		{
			g_privatePreview.reset();
			return false;
		}

		g_privatePreview.width = srcDesc.Width;
		g_privatePreview.height = srcDesc.Height;
		g_privatePreview.textureFormat = srcDesc.Format;
		g_privatePreview.srvFormat = srvFormat;

		return true;
	}

	inline ID3D11ShaderResourceView* GetSRVFromGfxImage(GfxImage* image)
	{
		if (!image)
			return nullptr;

		return *reinterpret_cast<ID3D11ShaderResourceView**>(
			reinterpret_cast<std::uint8_t*>(image) + GFXIMAGE_SRV_OFFSET
			);
	}

	inline bool CopyGfxImageToPrivatePreview(GfxImage* image)
	{
		if (!image || !pContext)
			return false;

		ID3D11ShaderResourceView* gameSrv = GetSRVFromGfxImage(image);

		if (!gameSrv)
			return false;

		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		gameSrv->GetResource(resource.GetAddressOf());

		if (!resource)
			return false;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> srcTex;

		if (FAILED(resource.As(&srcTex)) || !srcTex)
			return false;

		if (!EnsurePrivatePreviewTexture(srcTex.Get(), gameSrv))
			return false;

		pContext->CopySubresourceRegion(
			g_privatePreview.texture.Get(),
			0,
			0,
			0,
			0,
			srcTex.Get(),
			0,
			nullptr
		);

		pContext->Flush();

		g_hasPrivatePreview = true;
		return true;
	}

	inline ID3D11ShaderResourceView* GetReadyPreviewSRV()
	{
		return g_hasPrivatePreview && g_privatePreview.valid()
			? g_privatePreview.srv.Get()
			: nullptr;
	}


	// ---------------------------------- PNG Export ---------------------------------------

	inline std::string g_pngExportFileName = "emblem";
	inline int g_pngExportWidth = 1200;
	inline int g_pngExportHeight = 736;
	inline int g_pngExportPreset = 2;

	inline std::filesystem::path GetExportsDirectory()
	{
		return GetGameDirectory() / "exports";
	}

	inline std::string SanitizePngFileName(std::string fileName)
	{
		fileName = std::filesystem::path(fileName).filename().string();

		for (char& c : fileName)
		{
			const unsigned char uc = static_cast<unsigned char>(c);
			if (uc < 32 || c == '<' || c == '>' || c == ':' || c == '"' ||
				c == '/' || c == '\\' || c == '|' || c == '?' || c == '*')
			{
				c = '_';
			}
		}

		while (!fileName.empty() && (fileName.back() == ' ' || fileName.back() == '.'))
			fileName.pop_back();

		if (fileName.empty())
			fileName = "emblem";

		std::filesystem::path p(fileName);
		if (_stricmp(p.extension().string().c_str(), ".png") != 0)
			fileName += ".png";

		return fileName;
	}

	inline bool CopyPrivatePreviewToBgra(
		std::vector<std::uint8_t>& pixels,
		UINT& width,
		UINT& height)
	{
		pixels.clear();
		width = 0;
		height = 0;

		if (!pDevice || !pContext || !g_hasPrivatePreview || !g_privatePreview.valid())
			return false;

		D3D11_TEXTURE2D_DESC srcDesc{};
		g_privatePreview.texture->GetDesc(&srcDesc);

		const bool rgba =
			srcDesc.Format == DXGI_FORMAT_R8G8B8A8_TYPELESS ||
			srcDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM ||
			srcDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		const bool bgra =
			srcDesc.Format == DXGI_FORMAT_B8G8R8A8_TYPELESS ||
			srcDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM ||
			srcDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

		const bool bgrx =
			srcDesc.Format == DXGI_FORMAT_B8G8R8X8_TYPELESS ||
			srcDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM ||
			srcDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

		if (!rgba && !bgra && !bgrx)
		{
			utils::write_Debug(
				"[EmblemPNG] unsupported preview DXGI format=%u",
				static_cast<unsigned>(srcDesc.Format)
			);
			return false;
		}

		D3D11_TEXTURE2D_DESC stagingDesc = srcDesc;
		stagingDesc.MipLevels = 1;
		stagingDesc.ArraySize = 1;
		stagingDesc.Usage = D3D11_USAGE_STAGING;
		stagingDesc.BindFlags = 0;
		stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingDesc.MiscFlags = 0;
		stagingDesc.SampleDesc.Count = 1;
		stagingDesc.SampleDesc.Quality = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> staging;
		if (FAILED(pDevice->CreateTexture2D(&stagingDesc, nullptr, staging.GetAddressOf())))
			return false;

		pContext->CopyResource(staging.Get(), g_privatePreview.texture.Get());

		D3D11_MAPPED_SUBRESOURCE mapped{};
		const HRESULT mapHr = pContext->Map(
			staging.Get(),
			0,
			D3D11_MAP_READ,
			0,
			&mapped
		);

		if (FAILED(mapHr))
			return false;

		width = srcDesc.Width;
		height = srcDesc.Height;
		pixels.resize(static_cast<std::size_t>(width) * height * 4);

		for (UINT y = 0; y < height; ++y)
		{
			const auto* srcRow = static_cast<const std::uint8_t*>(mapped.pData) +
				static_cast<std::size_t>(y) * mapped.RowPitch;
			auto* dstRow = pixels.data() + static_cast<std::size_t>(y) * width * 4;

			for (UINT x = 0; x < width; ++x)
			{
				const auto* src = srcRow + static_cast<std::size_t>(x) * 4;
				auto* dst = dstRow + static_cast<std::size_t>(x) * 4;

				if (rgba)
				{
					// WIC input below is BGRA, so swap R/B while copying.
					dst[0] = src[2];
					dst[1] = src[1];
					dst[2] = src[0];
					dst[3] = src[3];
				}
				else
				{
					dst[0] = src[0];
					dst[1] = src[1];
					dst[2] = src[2];
					dst[3] = bgrx ? 255 : src[3];
				}
			}
		}

		pContext->Unmap(staging.Get(), 0);
		return true;
	}

	inline bool SaveBgraToPngWic(
		const std::filesystem::path& outputPath,
		const std::vector<std::uint8_t>& pixels,
		UINT sourceWidth,
		UINT sourceHeight,
		UINT outputWidth,
		UINT outputHeight)
	{
		if (pixels.empty() || sourceWidth == 0 || sourceHeight == 0 ||
			outputWidth == 0 || outputHeight == 0)
		{
			return false;
		}

		const HRESULT coHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		const bool shouldUninitialize = SUCCEEDED(coHr);

		if (FAILED(coHr) && coHr != RPC_E_CHANGED_MODE)
			return false;

		bool success = false;

		do
		{
			Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
			if (FAILED(CoCreateInstance(
				CLSID_WICImagingFactory,
				nullptr,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(factory.GetAddressOf()))))
			{
				break;
			}

			Microsoft::WRL::ComPtr<IWICBitmap> sourceBitmap;
			const UINT sourceStride = sourceWidth * 4;
			const UINT sourceBufferSize = sourceStride * sourceHeight;

			if (FAILED(factory->CreateBitmapFromMemory(
				sourceWidth,
				sourceHeight,
				GUID_WICPixelFormat32bppBGRA,
				sourceStride,
				sourceBufferSize,
				const_cast<BYTE*>(reinterpret_cast<const BYTE*>(pixels.data())),
				sourceBitmap.GetAddressOf())))
			{
				break;
			}

			Microsoft::WRL::ComPtr<IWICBitmapScaler> scaler;
			IWICBitmapSource* bitmapSource = sourceBitmap.Get();

			if (sourceWidth != outputWidth || sourceHeight != outputHeight)
			{
				if (FAILED(factory->CreateBitmapScaler(scaler.GetAddressOf())))
					break;

				if (FAILED(scaler->Initialize(
					sourceBitmap.Get(),
					outputWidth,
					outputHeight,
					WICBitmapInterpolationModeFant)))
				{
					break;
				}

				bitmapSource = scaler.Get();
			}

			Microsoft::WRL::ComPtr<IWICStream> stream;
			if (FAILED(factory->CreateStream(stream.GetAddressOf())))
				break;

			// Ensure a previous file with this name is replaced by the explicit Save action.
			std::error_code removeEc;
			std::filesystem::remove(outputPath, removeEc);

			if (FAILED(stream->InitializeFromFilename(outputPath.c_str(), GENERIC_WRITE)))
				break;

			Microsoft::WRL::ComPtr<IWICBitmapEncoder> encoder;
			if (FAILED(factory->CreateEncoder(
				GUID_ContainerFormatPng,
				nullptr,
				encoder.GetAddressOf())))
			{
				break;
			}

			if (FAILED(encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache)))
				break;

			Microsoft::WRL::ComPtr<IWICBitmapFrameEncode> frame;
			Microsoft::WRL::ComPtr<IPropertyBag2> properties;

			if (FAILED(encoder->CreateNewFrame(frame.GetAddressOf(), properties.GetAddressOf())))
				break;

			if (FAILED(frame->Initialize(properties.Get())))
				break;

			if (FAILED(frame->SetSize(outputWidth, outputHeight)))
				break;

			WICPixelFormatGUID encoderFormat = GUID_WICPixelFormat32bppBGRA;
			if (FAILED(frame->SetPixelFormat(&encoderFormat)))
				break;

			Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
			IWICBitmapSource* writeSource = bitmapSource;

			if (!IsEqualGUID(encoderFormat, GUID_WICPixelFormat32bppBGRA))
			{
				if (FAILED(factory->CreateFormatConverter(converter.GetAddressOf())))
					break;

				if (FAILED(converter->Initialize(
					bitmapSource,
					encoderFormat,
					WICBitmapDitherTypeNone,
					nullptr,
					0.0,
					WICBitmapPaletteTypeCustom)))
				{
					break;
				}

				writeSource = converter.Get();
			}

			if (FAILED(frame->WriteSource(writeSource, nullptr)))
				break;

			if (FAILED(frame->Commit()))
				break;

			if (FAILED(encoder->Commit()))
				break;

			success = true;
		} while (false);

		if (shouldUninitialize)
			CoUninitialize();

		return success;
	}

	inline bool ExportCurrentPreviewPng(
		const std::string& requestedFileName,
		int requestedWidth,
		int requestedHeight,
		std::filesystem::path* savedPath = nullptr)
	{
		if (!g_hasPrivatePreview || !g_privatePreview.valid())
			return false;

		if (requestedWidth < 1 || requestedHeight < 1 ||
			requestedWidth > 8192 || requestedHeight > 8192)
		{
			return false;
		}

		const auto exportDir = GetExportsDirectory();
		std::error_code ec;
		std::filesystem::create_directories(exportDir, ec);
		if (ec)
			return false;

		const auto fileName = SanitizePngFileName(requestedFileName);
		const auto outputPath = exportDir / fileName;

		std::vector<std::uint8_t> pixels;
		UINT sourceWidth = 0;
		UINT sourceHeight = 0;

		if (!CopyPrivatePreviewToBgra(pixels, sourceWidth, sourceHeight))
			return false;

		const bool ok = SaveBgraToPngWic(
			outputPath,
			pixels,
			sourceWidth,
			sourceHeight,
			static_cast<UINT>(requestedWidth),
			static_cast<UINT>(requestedHeight)
		);

		utils::write_Debug(
			"[EmblemPNG] save=%d src=%ux%u dst=%dx%d path=%s",
			ok ? 1 : 0,
			sourceWidth,
			sourceHeight,
			requestedWidth,
			requestedHeight,
			outputPath.string().c_str()
		);

		if (ok && savedPath)
			*savedPath = outputPath;

		return ok;
	}

	inline void DrawPngExportControls()
	{
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::TextUnformatted("Save Preview as PNG");
		ImGui::TextDisabled(
			"Source texture: %u x %u",
			g_privatePreview.width,
			g_privatePreview.height
		);

		ImGui::InputTextWithHint(
			"##EmblemPngFileName",
			"File name...",
			&g_pngExportFileName
		);

		const char* exportPresets[] =
		{
			"300 x 184",
			"600 x 368",
			"1200 x 736",
			"2400 x 1472",
			"8175 x 5014",
			"Custom"
		};

		if (ImGui::Combo(
			"Dimensions",
			&g_pngExportPreset,
			exportPresets,
			IM_ARRAYSIZE(exportPresets)))
		{
			switch (g_pngExportPreset)
			{
			case 0: g_pngExportWidth = 300;  g_pngExportHeight = 184;  break;
			case 1: g_pngExportWidth = 600;  g_pngExportHeight = 368;  break;
			case 2: g_pngExportWidth = 1200; g_pngExportHeight = 736;  break;
			case 3: g_pngExportWidth = 2400; g_pngExportHeight = 1472; break;
			case 4: g_pngExportWidth = 8175; g_pngExportHeight = 5014; break;
			default: break;
			}
		}

		ImGui::SetNextItemWidth(120.0f);
		if (ImGui::InputInt("Width", &g_pngExportWidth))
			g_pngExportPreset = 5;

		ImGui::SameLine();
		ImGui::SetNextItemWidth(120.0f);
		if (ImGui::InputInt("Height", &g_pngExportHeight))
			g_pngExportPreset = 5;

		g_pngExportWidth = (std::max)(1, (std::min)(g_pngExportWidth, 8192));
		g_pngExportHeight = (std::max)(1, (std::min)(g_pngExportHeight, 8192));

		if (ImGui::Button("Save PNG"))
		{
			std::filesystem::path savedPath;
			const bool ok = ExportCurrentPreviewPng(
				g_pngExportFileName,
				g_pngExportWidth,
				g_pngExportHeight,
				&savedPath
			);

			ImGui::InsertNotification({
				ok ? ImGuiToastType::Success : ImGuiToastType::Error,
				3000,
				ok ? "Saved PNG to exports folder" : "Failed to save PNG"
				});
		}

		ImGui::SameLine();
		ImGui::TextDisabled("exports/%s", SanitizePngFileName(g_pngExportFileName).c_str());
	}

	inline CompositeJob* GetTrackedJob_NoLock()
	{
		if (!g_job.active || g_job.id == 0)
			return nullptr;

		if (g_job.jobSlot < 0 || g_job.jobSlot >= COMPOSITE_JOB_COUNT)
			return nullptr;

		CompositeJob* jobs = GetCompositeJobs();
		CompositeJob* job = &jobs[g_job.jobSlot];

		if (job->id != g_job.id)
			return nullptr;

		return job;
	}

	inline std::uint64_t GetCurrentJobAgeMs()
	{
		if (!g_job.active || g_job.submitTimeMs == 0)
			return 0;

		return NowMs() - g_job.submitTimeMs;
	}

	inline bool SetPreviewSRVFromGfxImage(GfxImage* image)
	{
		g_job.previewSrv.Reset();

		ID3D11ShaderResourceView* rawSrv = GetSRVFromGfxImage(image);

		if (!rawSrv)
			return false;

		const HRESULT hr = rawSrv->QueryInterface(
			__uuidof(ID3D11ShaderResourceView),
			reinterpret_cast<void**>(g_job.previewSrv.GetAddressOf())
		);

		return SUCCEEDED(hr) && g_job.previewSrv;
	}

	inline bool CanCancelCurrentJob()
	{
		if (!g_job.active || g_job.ready || g_job.id == 0)
			return false;

		return GetCurrentJobAgeMs() >= MIN_CANCEL_AGE_MS;
	}

	inline int GetUsedLayerCount(const CompositeEmblem& emblem)
	{
		const auto* raw = reinterpret_cast<const std::uint8_t*>(&emblem);

		int highestUsed = -1;

		for (int i = 0; i < EMBLEM_LAYER_CAPACITY; ++i)
		{
			const std::uint8_t* layer = raw + (i * EMBLEM_LAYER_SIZE);

			const std::int16_t materialID =
				*reinterpret_cast<const std::int16_t*>(layer + 0x00);

			const std::int16_t icon =
				*reinterpret_cast<const std::int16_t*>(layer + 0x5C);

			if (materialID != -1 || icon != -1)
				highestUsed = i;
		}

		return highestUsed + 1;
	}

	inline bool HasAnyUsedLayers(const CompositeEmblem& emblem)
	{
		return GetUsedLayerCount(emblem) > 0;
	}

	inline int CountActiveCompositeJobs_NoLock()
	{
		int active = 0;
		CompositeJob* jobs = GetCompositeJobs();

		for (int i = 0; i < COMPOSITE_JOB_COUNT; ++i)
		{
			if (jobs[i].state != COMPOSITE_STATE_IDLE)
				++active;
		}

		return active;
	}

	inline int FindFirstIdleCompositeJobSlot_NoLock()
	{
		CompositeJob* jobs = GetCompositeJobs();
		static int lastAssignedSlot = 0;

		// Start searching from the next slot to distribute load and avoid dead slots
		for (int i = 0; i < COMPOSITE_JOB_COUNT; ++i)
		{
			int index = (lastAssignedSlot + 1 + i) % COMPOSITE_JOB_COUNT;

			// Optional: Skip slot 0 entirely if it's known to trap previews post-match
			// if (index == 0) continue; 

			if (jobs[index].state == COMPOSITE_STATE_IDLE)
			{
				lastAssignedSlot = index;
				return index;
			}
		}

		return -1;
	}

	inline void DumpCompositeJobPool(const char* tag)
	{
		CompositeJob* jobs = GetCompositeJobs();

		int active = 0;

		for (int i = 0; i < COMPOSITE_JOB_COUNT; ++i)
		{
			if (jobs[i].state == COMPOSITE_STATE_IDLE)
				continue;

			++active;

			utils::write_Debug(
				"[CompositePool] %s slot=%d state=%d id=%d type=%d setup=%d result=%p semantic=0x%08X cancel=%d layers=%d",
				tag ? tag : "pool",
				i,
				jobs[i].state,
				jobs[i].id,
				jobs[i].type,
				jobs[i].setupImage ? 1 : 0,
				jobs[i].resultImage,
				jobs[i].imageSemantic,
				jobs[i].cancel ? 1 : 0,
				jobs[i].layerCount
			);
		}

		utils::write_Debug("[CompositePool] %s active=%d firstIdle=%d",
			tag ? tag : "pool",
			active,
			FindFirstIdleCompositeJobSlot_NoLock()
		);
	}

	inline bool IsCompositePoolReadyForPreview(const char* reason)
	{
		const int active = CountActiveCompositeJobs_NoLock();
		const int firstIdle = FindFirstIdleCompositeJobSlot_NoLock();

		if (firstIdle < 0 || active >= COMPOSITE_JOB_COUNT - 2)
		{
			static std::uint64_t lastLogMs = 0;
			const std::uint64_t now = NowMs();

			if (now - lastLogMs >= 1000)
			{
				utils::write_Debug(
					"[Preview] compositor pool too busy reason=%s active=%d firstIdle=%d",
					reason ? reason : "unknown",
					active,
					firstIdle
				);

				DumpCompositeJobPool("too busy for preview");

				lastLogMs = now;
			}

			return false;
		}

		return true;
	}

	inline bool RequestCancelCurrentJob(const char* reason, bool force = false)
	{
		if (!g_job.active || g_job.ready || g_job.id == 0)
			return false;

		if (g_job.cancelRequested)
			return true;

		if (g_job.submitTimeMs == 0)
		{
			g_job.submitTimeMs = NowMs();
			g_job.lastLogMs = g_job.submitTimeMs;

			utils::write_Debug(
				"[Preview] cancel blocked, submitTimeMs was 0 id=%d reason=%s",
				g_job.id,
				reason ? reason : "unknown"
			);

			g_job.releaseRequested = true;
			return false;
		}

		const std::uint64_t age = GetCurrentJobAgeMs();

		if (!force && age < MIN_CANCEL_AGE_MS)
		{
			utils::write_Debug(
				"[Preview] cancel delayed id=%d age=%llu reason=%s",
				g_job.id,
				static_cast<unsigned long long>(age),
				reason ? reason : "unknown"
			);

			g_job.releaseRequested = true;
			return false;
		}

		utils::write_Debug(
			"[Preview] cancel requested id=%d age=%llu reason=%s",
			g_job.id,
			static_cast<unsigned long long>(age),
			reason ? reason : "unknown"
		);

		CL_CompositePushCancel(g_job.id);

		g_job.cancelRequested = true;
		g_job.releaseRequested = true;

		return true;
	}

	inline constexpr int COMPOSITE_EMBLEM = 1;


	inline bool IsPreviewCompositeJob(const CompositeJob& job)
	{
		return job.type == COMPOSITE_EMBLEM &&
			job.setupImage &&
			job.imageSemantic == EMBLEM_PREVIEW_FLAGS;
	}

	inline int ReapCompletedPreviewCompositeJobs(const char* reason)
	{
		CompositeJob* jobs = GetCompositeJobs();
		int released = 0;

		for (int i = 0; i < COMPOSITE_JOB_COUNT; ++i)
		{
			CompositeJob& job = jobs[i];

			if (job.state != COMPOSITE_STATE_COMPLETE)
				continue;

			if (!IsPreviewCompositeJob(job))
				continue;

			if (g_job.active && job.id == g_job.id)
				continue;

			const CompositeJobID id = job.id;
			GfxImage* image = CL_CompositePopImage(id);

			utils::write_Debug(
				"[Preview] reap orphan complete composite slot=%d id=%d image=%p semantic=0x%08X reason=%s",
				i,
				id,
				image,
				job.imageSemantic,
				reason ? reason : "unknown"
			);

			if (image)
				CL_CompositeReleaseImage(image);

			++released;
		}

		return released;
	}

	inline int CancelStalePreCompositeJobs(const char* reason)
	{
		CompositeJob* jobs = GetCompositeJobs();
		int canceled = 0;

		for (int i = 0; i < COMPOSITE_JOB_COUNT; ++i)
		{
			CompositeJob& job = jobs[i];

			if (!IsPreviewCompositeJob(job))
				continue;

			if (job.state != COMPOSITE_STATE_PRE)
				continue;

			if (job.type != COMPOSITE_EMBLEM)
				continue;

			if (g_job.active && job.id == g_job.id)
				continue;

			/*utils::write_Debug(
				"[Preview] forcefully clearing stuck composite slot=%d id=%d reason=%s",
				i,
				job.id,
				reason ? reason : "unknown"
			);*/

			// 1. Tell the engine to cancel (just in case the thread is still slightly alive)
			CL_CompositePushCancel(job.id);

			// 2. Brutally overwrite the job memory to free the slot immediately
			job.state = 0; // COMPOSITE_STATE_IDLE
			job.id = 0;
			job.cancel = false;
			job.setupImage = 0;
			job.resultImage = nullptr;

			++canceled;
		}

		return canceled;
	}


	inline void RequestReleaseCurrentPreview(
		const char* reason,
		bool clearVisiblePreview = true,
		bool lifecycleRelease = false)
	{
		if (lifecycleRelease)
		{
			g_lifecycleReleaseRequested = true;
			g_pendingPreview = {};
		}

		if (clearVisiblePreview)
		{
			g_hasPrivatePreview = false;
			g_privatePreview.reset();
			g_job.previewSrv.Reset();
		}

		if (!g_job.active)
		{
			g_job = {};
			return;
		}

		g_job.releaseRequested = true;

		RequestCancelCurrentJob(reason ? reason : "release requested", false);
	}

	inline bool IsPreviewUnsafeForGame()
	{
		return hooks::is_user_in_game() ||
			hooks::is_in_game();
	}

	inline bool CanUsePreview()
	{
		return g_previewMenuOpen &&
			hooks::is_user_in_lobby() &&
			!IsPreviewUnsafeForGame();
	}

	inline void QueuePreview(
		const CompositeEmblem& emblem,
		SourceKind sourceKind,
		int backendSlot,
		const std::filesystem::path& filePath
	)
	{
		g_pendingPreview.pending = true;
		g_pendingPreview.emblem = emblem;
		g_pendingPreview.sourceKind = sourceKind;
		g_pendingPreview.backendSlot = backendSlot;
		g_pendingPreview.filePath = filePath;

		utils::write_Debug(
			"[Preview] queued source=%d slot=%d",
			static_cast<int>(sourceKind),
			backendSlot
		);
	}

	inline bool SubmitFromComposite(
		const CompositeEmblem& emblem,
		SourceKind sourceKind,
		int backendSlot = -1,
		const std::filesystem::path& filePath = {}
	)
	{
		utils::write_Debug(
			"[Preview] SubmitFromComposite enter source=%d slot=%d active=%d ready=%d pending=%d canUse=%d",
			static_cast<int>(sourceKind),
			backendSlot,
			g_job.active ? 1 : 0,
			g_job.ready ? 1 : 0,
			g_pendingPreview.pending ? 1 : 0,
			CanUsePreview() ? 1 : 0
		);

		if (!CanUsePreview())
		{
			utils::write_Debug("[Preview] Submit blocked because CanUsePreview=false");
			return false;
		}

		if (g_job.active)
		{
			QueuePreview(emblem, sourceKind, backendSlot, filePath);
			return true;
		}

		const std::uint64_t now = NowMs();

		if (now < g_nextPreviewAllowedMs)
		{
			utils::write_Debug(
				"[Preview] submit delayed by cooldown now=%llu allowed=%llu source=%d slot=%d",
				static_cast<unsigned long long>(now),
				static_cast<unsigned long long>(g_nextPreviewAllowedMs),
				static_cast<int>(sourceKind),
				backendSlot
			);

			QueuePreview(emblem, sourceKind, backendSlot, filePath);
			return true;
		}

		const int layerCount = GetUsedLayerCount(emblem);

		if (layerCount <= 0 || layerCount > EMBLEM_LAYER_CAPACITY)
		{
			utils::write_Debug("[Preview] bad layerCount=%d", layerCount);
			return false;
		}

		const int firstIdleBeforePush = FindFirstIdleCompositeJobSlot_NoLock();
		const int activeBeforePush = CountActiveCompositeJobs_NoLock();

		if (firstIdleBeforePush < 0 || activeBeforePush >= COMPOSITE_JOB_COUNT - 2)
		{
			utils::write_Debug(
				"[Preview] submit delayed, compositor pool too busy active=%d firstIdle=%d source=%d slot=%d",
				activeBeforePush,
				firstIdleBeforePush,
				static_cast<int>(sourceKind),
				backendSlot
			);

			DumpCompositeJobPool("submit delayed");

			QueuePreview(emblem, sourceKind, backendSlot, filePath);
			g_nextPreviewAllowedMs = NowMs() + 500;

			return true;
		}

		const CompositeJobID id = CL_CompositePushEmblem(
			&emblem,
			layerCount,
			false,
			nullptr,
			EMBLEM_PREVIEW_FLAGS
		);

		if (id == 0)
		{
			utils::write_Debug("[Preview] PushEmblem returned 0");
			return false;
		}

		const int jobSlot = FindJobSlotById_NoLock(id);

		if (jobSlot < 0)
		{
			utils::write_Debug("[Preview] submitted id=%d but could not find job slot", id);
			return false;
		}

		g_job = {};
		g_job.active = true;
		g_job.ready = false;
		g_job.cancelRequested = false;
		g_job.releaseRequested = false;
		g_job.id = id;
		g_job.jobSlot = jobSlot;
		g_job.image = nullptr;
		g_job.submitTimeMs = now;
		g_job.lastLogMs = now;
		g_job.sourceKind = sourceKind;
		g_job.backendSlot = backendSlot;
		g_job.filePath = filePath;

		utils::write_Debug(
			"[Preview] submitted id=%d slot=%d time=%llu",
			g_job.id,
			g_job.jobSlot,
			static_cast<unsigned long long>(g_job.submitTimeMs)
		);

		return true;
	}

	inline std::uint64_t g_poolBusySinceMs = 0;
	inline bool g_triedStalePoolCleanup = false;

	inline void StartPendingPreviewIfPossible()
	{
		if (g_job.active || !g_pendingPreview.pending)
			return;

		const bool poolReady = IsCompositePoolReadyForPreview("pending");

		if (!poolReady)
		{
			if (g_poolBusySinceMs == 0)
				g_poolBusySinceMs = NowMs();

			if (!g_triedStalePoolCleanup &&
				CanUsePreview() &&
				NowMs() - g_poolBusySinceMs >= 5000)
			{
				const int canceled = CancelStalePreCompositeJobs("pending pool stuck");

				utils::write_Debug(
					"[Preview] stale pool cleanup canceled=%d",
					canceled
				);

				g_triedStalePoolCleanup = true;
				g_nextPreviewAllowedMs = NowMs() + 500;
			}

			return;
		}

		g_poolBusySinceMs = 0;
		g_triedStalePoolCleanup = false;

		if (g_lifecycleReleaseRequested || !CanUsePreview())
		{
			utils::write_Debug(
				"[Preview] dropped pending preview lifecycle=%d canUse=%d",
				g_lifecycleReleaseRequested ? 1 : 0,
				CanUsePreview() ? 1 : 0
			);

			g_pendingPreview = {};
			g_lifecycleReleaseRequested = false;
			return;
		}

		if (NowMs() < g_nextPreviewAllowedMs)
			return;

		if (!IsCompositePoolReadyForPreview("pending"))
			return;

		PendingPreview pending = g_pendingPreview;
		g_pendingPreview = {};

		SubmitFromComposite(
			pending.emblem,
			pending.sourceKind,
			pending.backendSlot,
			pending.filePath
		);
	}

	inline void FinishCurrentJobAndMaybeStartPending(const char* reason)
	{
		const bool mayStartPending =
			g_pendingPreview.pending &&
			!g_lifecycleReleaseRequested &&
			CanUsePreview();

		g_job = {};

		if (mayStartPending)
		{
			StartPendingPreviewIfPossible();
			return;
		}

		g_pendingPreview = {};
		g_lifecycleReleaseRequested = false;
	}

	inline void Update()
	{
		if (!g_job.active)
		{
			StartPendingPreviewIfPossible();
			return;
		}

		if (g_job.ready)
			return;

		CompositeJob* job = GetTrackedJob_NoLock();

		if (!job)
		{
			FinishCurrentJobAndMaybeStartPending("released/canceled idle");
			return;
		}

		const std::uint64_t now = NowMs();
		const std::uint64_t age = GetCurrentJobAgeMs();

		if (g_job.releaseRequested && !g_job.cancelRequested && age >= MIN_CANCEL_AGE_MS)
		{
			RequestCancelCurrentJob("delayed release", false);
			return;
		}

		if (!g_job.cancelRequested && age >= HARD_CANCEL_AGE_MS)
		{
			RequestCancelCurrentJob("hard timeout", false);
			return;
		}

		if (job->state == 4)
		{
			const CompositeJobID finishedId = g_job.id;

			GfxImage* image = CL_CompositePopImage(finishedId);

			if (!image)
			{
				FinishCurrentJobAndMaybeStartPending("complete but PopImage failed");
				return;
			}

			bool copied = false;

			if (!g_job.releaseRequested &&
				!g_job.cancelRequested &&
				!g_lifecycleReleaseRequested &&
				CanUsePreview())
			{
				copied = CopyGfxImageToPrivatePreview(image);
			}

			CL_CompositeReleaseImage(image);
			image = nullptr;

			utils::write_Debug(
				"[Preview] completed id=%d copied=%d privateSrv=%p",
				finishedId,
				copied ? 1 : 0,
				g_privatePreview.srv.Get()
			);

			g_job = {};
			g_nextPreviewAllowedMs = NowMs() + PREVIEW_SUBMIT_COOLDOWN_MS;

			if (copied && CanUsePreview())
			{
				StartPendingPreviewIfPossible();
			}
			else
			{
				g_pendingPreview = {};
				g_lifecycleReleaseRequested = false;
			}

			return;
		}

		if (job->state == 0 && (g_job.cancelRequested || g_job.releaseRequested))
		{
			utils::write_Debug(
				"[Preview] released/canceled job reached idle id=%d cancel=%d release=%d",
				g_job.id,
				g_job.cancelRequested ? 1 : 0,
				g_job.releaseRequested ? 1 : 0
			);

			g_job = {};
			g_pendingPreview = {};
			return;
		}

		if (now - g_job.lastLogMs >= 1000)
		{
			utils::write_Debug(
				"[Preview] waiting id=%d state=%d age=%llu cancel=%d pending=%d",
				g_job.id,
				job->state,
				static_cast<unsigned long long>(age),
				g_job.cancelRequested ? 1 : 0,
				g_pendingPreview.pending ? 1 : 0
			);

			g_job.lastLogMs = now;
		}
	}

	inline void HardResetPreviewSubsystem()
	{
		g_job = {};
		g_pendingPreview = {};
		g_lifecycleReleaseRequested = false;
		// Force your tracker to rescan slot availability from scratch next frame
	}

	inline void ResetPreviewAfterMatchReturn()
	{
		utils::write_Debug("[Preview] reset after match return");

		if (g_job.active)
		{
			RequestReleaseCurrentPreview("reset after match return", true, true);
		}
		else
		{
			g_job = {};
			g_lifecycleReleaseRequested = false;
		}

		g_pendingPreview = {};

		g_hasPrivatePreview = false;
		g_privatePreview.reset();

		g_nextPreviewAllowedMs = NowMs() + 2500;

		g_poolBusySinceMs = 0;
		g_triedStalePoolCleanup = false;

		const int reaped = ReapCompletedPreviewCompositeJobs("post-match reset");
		const int canceled = CancelStalePreCompositeJobs("post-match reset");

		if (reaped || canceled)
		{
			utils::write_Debug(
				"[Preview] post-match pool cleanup reaped=%d canceled=%d",
				reaped,
				canceled
			);
		}

		g_emblemExportBrowser.selectedIndex = -1;
		g_emblemImportBrowser.selectedIndex = -1;

		RefreshEmblemSlotList(CONTROLLER_INDEX_0);
		RefreshEmblemFileList();

		HardResetPreviewSubsystem();
	}

	inline void ClearLifecycleReleaseIfPreviewAllowed(const char* reason)
	{
		if (g_lifecycleReleaseRequested && CanUsePreview() && !g_job.active)
		{
			utils::write_Debug(
				"[Preview] clearing lifecycle release reason=%s",
				reason ? reason : "unknown"
			);

			g_lifecycleReleaseRequested = false;
		}
	}

	inline void UpdateReleaseGuards(bool menuOpen)
	{
		static bool initialized = false;
		static bool lastMenuOpen = false;
		static bool lastUnsafe = false;
		static bool lastMatchLoaded = false;
		static bool lastLobby = false;

		g_previewMenuOpen = menuOpen;

		const bool unsafe = IsPreviewUnsafeForGame();

		const bool matchLoaded = hooks::is_match_loaded();
		const bool lobby = hooks::is_user_in_lobby();

		if (!initialized)
		{
			initialized = true;
			lastMenuOpen = menuOpen;
			lastUnsafe = unsafe;
			lastMatchLoaded = matchLoaded;
			lastLobby = lobby;

			if (!menuOpen || unsafe || matchLoaded)
				RequestReleaseCurrentPreview("initial unsafe state", true, true);

			return;
		}

		if (lastMenuOpen && !menuOpen)
			RequestReleaseCurrentPreview("menu closed", true, true);

		if (!lastUnsafe && unsafe)
			RequestReleaseCurrentPreview("entering game/loading", true, true);

		if (!lastMatchLoaded && matchLoaded)
			RequestReleaseCurrentPreview("match loaded", true, true);

		const bool inGameNow = unsafe || matchLoaded;

		if (inGameNow)
		{
			g_wasInGameForPreview = true;
			g_didPostMatchPreviewReset = false;
			g_lobbyStableSinceMs = 0;
		}

		if (g_wasInGameForPreview && !inGameNow && lobby)
		{
			if (g_lobbyStableSinceMs == 0)
				g_lobbyStableSinceMs = NowMs();

			if (!g_didPostMatchPreviewReset && NowMs() - g_lobbyStableSinceMs >= 2000)
			{
				ResetPreviewAfterMatchReturn();

				g_didPostMatchPreviewReset = true;
				g_wasInGameForPreview = false;
				g_lifecycleReleaseRequested = false;
			}
		}
		else if (!lobby)
		{
			g_lobbyStableSinceMs = 0;
		}

		if (!unsafe && !matchLoaded && lobby && menuOpen && !g_job.active)
		{
			ClearLifecycleReleaseIfPreviewAllowed("stable lobby");
		}

		lastMenuOpen = menuOpen;
		lastUnsafe = unsafe;
		lastMatchLoaded = matchLoaded;
		lastLobby = lobby;
	}

	inline bool PreviewBackendSlot(
		ControllerIndex_t controller,
		int actualSlot
	)
	{
		utils::write_Debug(
			"[Preview] PreviewBackendSlot enter slot=%d menu=%d lobby=%d userInGame=%d localInGame=%d comInGame=%d lifecycle=%d active=%d pending=%d",
			actualSlot,
			g_previewMenuOpen ? 1 : 0,
			hooks::is_user_in_lobby() ? 1 : 0,
			hooks::is_user_in_game() ? 1 : 0,
			hooks::local_client_is_in_game() ? 1 : 0,
			hooks::is_in_game() ? 1 : 0,
			g_lifecycleReleaseRequested ? 1 : 0,
			g_job.active ? 1 : 0,
			g_pendingPreview.pending ? 1 : 0
		);

		if (!CanUsePreview())
		{
			RequestReleaseCurrentPreview("preview requested while unsafe", true);
			utils::write_Debug("[Preview] blocked preview while unsafe/not-lobby");
			return false;
		}

		constexpr int CUSTOMIZATION_TYPE_EMBLEM = 3;
		constexpr int STORAGE_EMBLEMS_FILE_TYPE = 26;

		if (actualSlot < 0 || actualSlot >= 32)
		{
			utils::write_Debug("[Preview] invalid slot=%d", actualSlot);
			return false;
		}

		CompositeEmblem emblem{};
		BG_InitCompositeEmblem(&emblem);

		Live_Emblems_GetEmblemData(
			controller,
			CUSTOMIZATION_TYPE_EMBLEM,
			actualSlot,
			static_cast<StorageFileType>(STORAGE_EMBLEMS_FILE_TYPE),
			&emblem
		);

		const int layerCount = GetUsedLayerCount(emblem);

		utils::write_Debug(
			"[Preview] slot=%d layerCount=%d",
			actualSlot,
			layerCount
		);

		if (layerCount <= 0)
		{
			utils::write_Debug("[Preview] slot=%d has no used layers after Live_Emblems_GetEmblemData", actualSlot);
			return false;
		}

		ClearLifecycleReleaseIfPreviewAllowed("backend preview requested");

		return SubmitFromComposite(
			emblem,
			SourceKind::BackendSlot,
			actualSlot
		);
	}

	inline bool PreviewEmblemFile(const std::filesystem::path& path)
	{
		utils::write_Debug(
			"[Preview] PreviewEmblemFile enter path=%s menu=%d lobby=%d",
			path.string().c_str(),
			g_previewMenuOpen ? 1 : 0,
			hooks::is_user_in_lobby() ? 1 : 0
		);

		if (!CanUsePreview())
		{
			RequestReleaseCurrentPreview("preview requested while unsafe", true);
			utils::write_Debug("[Preview] blocked preview while unsafe/in-game");
			return false;
		}

		ReadEmblemFileResult imported{};

		if (!ReadEmblemFile(path, imported))
			return false;

		if (!HasAnyUsedLayers(imported.emblem))
			return false;

		ClearLifecycleReleaseIfPreviewAllowed("file preview requested");

		return SubmitFromComposite(
			imported.emblem,
			SourceKind::EmblemFile,
			-1,
			path
		);
	}

	inline bool PreviewBackendComposite(
		const CompositeEmblem& emblem,
		int actualSlot
	)
	{
		if (!CanUsePreview())
		{
			RequestReleaseCurrentPreview("backend composite preview while unsafe", true);
			utils::write_Debug("[Preview] blocked backend composite preview while unsafe/in-game");
			return false;
		}

		return SubmitFromComposite(
			emblem,
			SourceKind::BackendSlot,
			actualSlot
		);
	}


	inline void DrawCheckerboardBackground(
		ImDrawList* draw,
		const ImVec2& min,
		const ImVec2& max,
		float cellSize = 16.0f,
		ImU32 colorA = IM_COL32(70, 70, 70, 255),
		ImU32 colorB = IM_COL32(95, 95, 95, 255))
	{
		if (!draw)
			return;

		draw->PushClipRect(min, max, true);

		const float width = max.x - min.x;
		const float height = max.y - min.y;

		const int cols = static_cast<int>(std::ceil(width / cellSize));
		const int rows = static_cast<int>(std::ceil(height / cellSize));

		for (int y = 0; y < rows; ++y)
		{
			for (int x = 0; x < cols; ++x)
			{
				const ImU32 col = ((x + y) & 1) ? colorA : colorB;

				ImVec2 p0(
					min.x + (x * cellSize),
					min.y + (y * cellSize)
				);

				ImVec2 p1(
					(std::min)(p0.x + cellSize, max.x),
					(std::min)(p0.y + cellSize, max.y)
				);

				draw->AddRectFilled(p0, p1, col);
			}
		}

		draw->PopClipRect();
	}

	inline void DrawPreviewImageDirect(const ImVec2& requestedSize)
	{
		ID3D11ShaderResourceView* srv = GetReadyPreviewSRV();

		if (!srv)
		{
			ImGui::TextDisabled("No preview SRV");
			return;
		}

		ImVec2 drawSize = requestedSize;

		const ImVec2 avail = ImGui::GetContentRegionAvail();

		if (avail.x > 0.0f && avail.y > 0.0f)
		{
			const float scaleX = avail.x / requestedSize.x;
			const float scaleY = avail.y / requestedSize.y;
			const float scale = (scaleX < scaleY) ? scaleX : scaleY;


			drawSize.x = requestedSize.x * scale;
			drawSize.y = requestedSize.y * scale;

		}

		const float horizontalOffset = (avail.x - drawSize.x) * 0.5f;

		if (horizontalOffset > 0.0f)
		{
			ImGui::SetCursorPosX(
				ImGui::GetCursorPosX() + horizontalOffset
			);
		}

		const ImVec2 pos = ImGui::GetCursorScreenPos();
		const ImVec2 end(pos.x + drawSize.x, pos.y + drawSize.y);

		ImDrawList* draw = ImGui::GetWindowDrawList();

		DrawCheckerboardBackground(draw, pos, end, 16.0f);

		draw->AddImage(
			reinterpret_cast<ImTextureID>(srv),
			pos,
			end,
			ImVec2(0.0f, 0.0f),
			ImVec2(1.0f, 1.0f),
			IM_COL32(255, 255, 255, 255)
		);

		draw->AddRect(
			pos,
			end,
			IM_COL32(255, 255, 255, 120)
		);

		ImGui::Dummy(drawSize);
	}

	inline void DrawPreviewContents(
		const ImVec2& imageSize = ImVec2(1200.0f, 736.0f)
	)
	{
		ImGui::TextUnformatted("Preview");

		const char* resolutionOptions[] = {
			"Low (300x184)",
			"Medium (600x368)",
			"Good (1200x736)",
			"Best (2400x1472)",
			"Overkill (8175x5014)"
		};

		ImGui::Combo("Emblem Quality", &iSelectedResolutionTier, resolutionOptions, IM_ARRAYSIZE(resolutionOptions));

		ImGui::Separator();
		ImGui::Spacing();

		if (!GetReadyPreviewSRV())
		{
			if (g_job.active)
				ImGui::TextDisabled("Rendering.");
			else
				ImGui::TextDisabled("No preview selected");

			return;
		}

		DrawPreviewImageDirect(imageSize);
		DrawPngExportControls();
	}

	void DrawEmblemImportUI()
	{
		static int targetSlot = 0;
		static bool allowOverwrite = false;

		const auto emblemsDir = GetEmblemsDirectory();

		ImGui::Separator();

		ImGui::SetNextItemWidth(125.0f);

		ImGui::InputInt("Target Slot", &targetSlot);

		if (targetSlot < 0)
			targetSlot = 0;

		if (targetSlot > 31)
			targetSlot = 31;

		ImGui::SameLine();

		ImGui::Checkbox("Allow overwrite used slot", &allowOverwrite);

		ImGui::InputTextWithHint("##CustomImportName", "Import Emblem Name...", &customImportName);
		ImGui::SameLine();
		ImGui::Checkbox("Rename", &customImportRename);

		if (ImGui::Button("Refresh Emblem Files"))
		{
			RefreshEmblemFileList();
		}
		ImGui::Separator();

		if (g_emblemImportBrowser.files.empty())
		{
			ImGui::TextDisabled("No .emblem files found.");
		}
		else
		{
			ImGui::Text("Available .emblem files:");

			if (ImGui::BeginListBox("##EmblemFileList", ImVec2(-FLT_MIN, 150.0f)))
			{
				for (int i = 0; i < static_cast<int>(g_emblemImportBrowser.files.size()); ++i)
				{
					const bool selected = g_emblemImportBrowser.selectedIndex == i;

					if (ImGui::Selectable(g_emblemImportBrowser.files[i].displayName.c_str(), selected)) {
						g_emblemImportBrowser.selectedIndex = i;
						g_pngExportFileName = g_emblemImportBrowser.files[i].path.stem().string();
						PreviewEmblemFile(g_emblemImportBrowser.files[i].path);
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndListBox();
			}
		}

		const bool hasSelection =
			g_emblemImportBrowser.selectedIndex >= 0 &&
			g_emblemImportBrowser.selectedIndex < static_cast<int>(g_emblemImportBrowser.files.size());

		if (hasSelection)
		{
			const auto& selected = g_emblemImportBrowser.files[g_emblemImportBrowser.selectedIndex];

			//ImGui::TextWrapped("Selected: %s", selected.fileName.c_str());

			if (ImGui::Button("Import Selected Emblem"))
			{
				const bool ok = ImportEmblemFileBackend(
					CONTROLLER_INDEX_0,
					targetSlot,
					selected.path,
					allowOverwrite
				);

				ImGui::InsertNotification({
					ok ? ImGuiToastType::Success : ImGuiToastType::Error,
					3000,
					ok ? "Imported emblem" : "Failed to import emblem"
					});
			}
		}
		else
		{
			ImGui::BeginDisabled();
			ImGui::Button("Import Selected Emblem");
			ImGui::EndDisabled();
		}
	}

	void DrawEmblemExportUI()
	{
		constexpr ControllerIndex_t controller = CONTROLLER_INDEX_0;

		const auto emblemsDir = GetEmblemsDirectory();

		bool refreshNeeded = false;

		if (ImGui::Button("Refresh Emblem Slots"))
			refreshNeeded = true;

		ImGui::SameLine();

		if (ImGui::Checkbox("Show empty slots", &g_emblemExportBrowser.showEmptySlots))
			refreshNeeded = true;

		if (refreshNeeded)
			RefreshEmblemSlotList(controller);

		ImGui::Separator();

		if (g_emblemExportBrowser.slots.empty())
		{
			ImGui::TextDisabled("No emblem slots found.");
		}
		else
		{
			ImGui::Text("Emblem slots:");

			if (ImGui::BeginListBox("##EmblemSlotList", ImVec2(-FLT_MIN, 150.0f)))
			{
				for (int i = 0; i < static_cast<int>(g_emblemExportBrowser.slots.size()); ++i)
				{
					const auto& entry = g_emblemExportBrowser.slots[i];
					const bool selected = g_emblemExportBrowser.selectedIndex == i;

					if (ImGui::Selectable(entry.displayName.c_str(), selected)) {
						g_emblemExportBrowser.selectedIndex = i;
						g_pngExportFileName = entry.name;
						PreviewBackendSlot(CONTROLLER_INDEX_0, entry.slot);
					}


					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndListBox();
			}
		}

		const bool hasSelection =
			g_emblemExportBrowser.selectedIndex >= 0 &&
			g_emblemExportBrowser.selectedIndex < static_cast<int>(g_emblemExportBrowser.slots.size());

		if (!hasSelection)
		{
			ImGui::BeginDisabled();
			ImGui::Button("Export Selected Emblem");
			ImGui::EndDisabled();
			return;
		}

		const auto& selected = g_emblemExportBrowser.slots[g_emblemExportBrowser.selectedIndex];

		ImGui::Separator();

		/*ImGui::Text("Selected:");
		ImGui::Text("Slot: %d", selected.slot);
		ImGui::Text("Sort Index: %d", selected.sortIndex);
		ImGui::Text("Name: %s", selected.name.c_str());

		if (!selected.isUsed)
		{
			ImGui::TextDisabled("This slot is empty.");
			ImGui::BeginDisabled();
			ImGui::Button("Export Selected Emblem");
			ImGui::EndDisabled();
			return;
		}*/

		if (ImGui::Button("Export Selected Emblem"))
		{
			const bool ok = ExportEmblemBackend(
				controller,
				selected.slot,
				emblemsDir
			);

			ImGui::InsertNotification({
				ok ? ImGuiToastType::Success : ImGuiToastType::Error,
				3000,
				ok ? "Exported emblem" : "Failed to export emblem"
				});
		}

		ImGui::SetNextItemWidth(125.0f);
		ImGui::InputTextWithHint("##RenameEmblem", "Emblem Name", &EmblemRename);
		ImGui::SameLine();
		if (ImGui::Button("Rename Selected Emblem"))
		{
			const bool ok = RenameEmblem(
				selected.slot,
				EmblemRename
			);

			ImGui::InsertNotification({
				ok ? ImGuiToastType::Success : ImGuiToastType::Error,
				3000,
				ok ? "Renamed emblem" : "Failed to rename emblem"
				});
		}
	}

	inline void DrawImportExportContents()
	{
		ImGui::TextUnformatted("Export In-Game Emblems");
		ImGui::Separator();

		DrawEmblemExportUI();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::TextUnformatted("Import .emblem File");
		ImGui::Separator();

		DrawEmblemImportUI();
	}

	inline void DrawEmblemsTabLayout(
		float requestedLeftWidth = 430.0f
	)
	{
		const ImVec2 avail = ImGui::GetContentRegionAvail();
		const float spacing = ImGui::GetStyle().ItemSpacing.x;
		const float minimumRightWidth = 260.0f;

		float leftWidth = requestedLeftWidth;

		if (leftWidth > avail.x)
			leftWidth = avail.x;

		const float rightWidth = avail.x - leftWidth - spacing;

		if (rightWidth < minimumRightWidth)
		{
			const float toolsHeight = avail.y * 0.60f;

			ImGui::BeginChild(
				"##EmblemToolsChild",
				ImVec2(0.0f, toolsHeight),
				true
			);

			DrawImportExportContents();

			ImGui::EndChild();

			ImGui::BeginChild(
				"##EmblemPreviewChild",
				ImVec2(0.0f, 0.0f),
				true
			);

			DrawPreviewContents();

			ImGui::EndChild();

			return;
		}

		ImGui::BeginChild(
			"##EmblemToolsChild",
			ImVec2(leftWidth, 0.0f),
			true
		);

		DrawImportExportContents();

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild(
			"##EmblemPreviewChild",
			ImVec2(0.0f, 0.0f),
			true
		);

		DrawPreviewContents();

		ImGui::EndChild();
	}
}

using namespace EmblemPreview;

ImColor PulsingColor(
	const ImVec4& colorA,
	const ImVec4& colorB,
	float speed = 1.0f
)
{
	const float pulse =
		(sinf(static_cast<float>(ImGui::GetTime()) * speed) + 1.0f) * 0.5f;

	const ImVec4 color = ImLerp(colorA, colorB, pulse);
	return ImColor(color);
}

inline void DrawEmblemImportExportContents()
{
	ImGui::Text("Export");
	ImGui::Separator();

	DrawEmblemExportUI();

	ImGui::Separator();
	ImGui::Text("Import");

	DrawEmblemImportUI();
}

inline void DrawEmblemsTab()
{
	const ImVec2 avail = ImGui::GetContentRegionAvail();

	const float leftWidth = 430.0f;

	const float spacing = ImGui::GetStyle().ItemSpacing.x;
	const float rightWidth = avail.x - leftWidth - spacing;

	if (rightWidth <= 260.0f)
	{
		if (ImGui::BeginChild("##EmblemToolsChild", ImVec2(0.0f, avail.y * 0.60f), true))
		{
			DrawEmblemImportExportContents();
		}
		ImGui::EndChild();

		if (ImGui::BeginChild("##EmblemPreviewChild", ImVec2(0.0f, 0.0f), true))
		{
			EmblemPreview::DrawPreviewContents();
		}
		ImGui::EndChild();

		return;
	}

	if (ImGui::BeginChild("##EmblemToolsChild", ImVec2(leftWidth, 0.0f), true))
	{
		DrawEmblemImportExportContents();
	}
	ImGui::EndChild();

	ImGui::SameLine();

	if (ImGui::BeginChild("##EmblemPreviewChild", ImVec2(0.0f, 0.0f), true))
	{
		DrawPreviewContents();
	}
	ImGui::EndChild();
}

void draw() {

	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	vec2_t res = { desktop.right, desktop.bottom };
	flHue += flSpeed * ImGui::GetIO().DeltaTime;
	auto& style = ImGui::GetStyle();

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
	style.Colors[ImGuiCol_Button] = ImColor(40, 40, 40, 200);
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
	style.FramePadding = ImVec2(4, 4);
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

	if (open) {

		*(int*)((DWORD64)OFFSET(0x17DF0405)) = 0;

		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		ImDrawList* drawlist = ImGui::GetBackgroundDrawList();
		drawlist->AddRectFilled(ImVec2(0, 0), ImVec2(desktop.right, desktop.bottom), IM_COL32(0, 0, 0, 150));

		static bool wasEmblemsTabVisible = false;

		bool emblemsTabVisible = false;

		ImGui::SetNextWindowSize(ImVec2(850.0f, 500.0f), ImGuiCond_Once);

		std::string title = std::string(" - Scropts QOL for BO3 - ");

		ImGui::Begin(title.c_str(), &open);

		ImGui::BeginTabBar("##main");

		if (ImGui::BeginTabItem("Account", nullptr)) {

			auto currentMode = Com_SessionMode_GetMode();

			int maxXP;
			int maxParagonXP;
			int maxLevel;
			int maxPrestige;
			int minRank;
			int maxRankIcon;

			if (currentMode == MODE_CAMPAIGN) {
				maxLevel = 19;
				maxPrestige = 0;
				maxXP = 581651;
				maxParagonXP = 0;
				maxRankIcon = 0;
				minRank = 0;
			}
			else if (currentMode == MODE_MULTIPLAYER) {
				maxLevel = 54;
				maxPrestige = 11;
				maxXP = 1457200;
				maxParagonXP = 52542000;
				maxRankIcon = 47;
				minRank = 56;
			}
			else if (currentMode == MODE_ZOMBIES) {
				maxLevel = 34;
				maxPrestige = 11;
				maxXP = 1375000;
				maxParagonXP = 52345460;
				maxRankIcon = 55;
				minRank = 36;
			}
			else {
				maxLevel = 0;
				maxPrestige = 0;
				maxXP = 0;
				maxParagonXP = 0;
				maxRankIcon = 0;
				minRank = 0;
			}

			constexpr int kStatMax = MAXINT / 2;

			ImGui::BeginChild("##RANKING", ImGui::GetContentRegionAvail());

			ImGui::Dummy(ImVec2(0, 5));

			ImGui::Checkbox("Freeze Stats", &bProtectStats);

			ImGui::SameLine();

			HelpMarker("Enable to disable the 'To protect your stats you have been kicked to the main menu' error. Use for reverting buggy MP stat edits, No stats are saved to profile while enabled!");

			if (bProtectStats && !bProtectStatsRan) {
				Dvar_SetFromString("tu10_validationFatal", "0", true);
				bProtectStatsRan = true;
			}

			if (!bProtectStats && bProtectStatsRan) {
				Dvar_SetFromString("tu10_validationFatal", "1", true);
				bProtectStatsRan = false;
			}

			ImGui::SameLine();

			ImGui::Checkbox("Arena", &bArena);

			ImGui::SameLine();

			HelpMarker("Enable to switch between MP and Arena stat editor.");

			if (ImGui::Button("Set Rank")) {
				ClampInt(pPrestige, 0, maxPrestige);
				ClampInt(pLevel, 0, maxLevel);
				ClampInt(ParagonRank, minRank, 1000);

				UpdateRankXPFromInputs(currentMode, pLevel, ParagonRank, rankXp, paragonRankXp);

				setAllRanks();
				LiveStorage_UploadStatsForController(0);
			}

			ImGui::SameLine();

			if (ImGui::Button("Set Rank Icon")) {
				ClampInt(icon, 0, maxRankIcon);
				if (RankIconExistsForMode(currentMode, icon)) {
					LiveStats_SetStatByKey(Com_SessionMode_GetMode(), CONTROLLER_INDEX_0, MP_PLAYERSTATSKEY_PARAGONICONID, icon);
					LiveStorage_UploadStatsForController(0);
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Set ALL Stats")) {
				setStats();
				LiveStorage_UploadStatsForController(0);
			}

			// Prestige
			ImGui::Separator();

			ImGui::BulletText("Rank Editor");

			ImGui::SameLine();

			HelpMarker("Input Level and Prestige Master Level. XP is pulled from the game's rank tables automatically.");

			ImGui::Separator();

			if (ImGui::Button("Send##PRESTIGE")) {
				ClampInt(pPrestige, 0, 11);

				setPrestige(pPrestige);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Prestige ##RANK", &pPrestige, 0, 11);

			// Level
			if (ImGui::Button("Send##LEVEL"))
			{
				int visibleLevel = pLevel;
				ClampInt(visibleLevel, 1, maxLevel + 1);

				const int storedRank = visibleLevel - 1;

				rankXp = GetRankXPFromTable(currentMode, storedRank);

				setpLevel(storedRank);
				setpLevelXP(rankXp);

				LiveStorage_UploadStatsForController(0);
			}

			int displayLevel = pLevel + 1;

			ImGui::SameLine();

			if (InputIntClamped("Level ##RANK", &displayLevel, 1, maxLevel + 1))
			{
				pLevel = displayLevel - 1;
			}

			ClampInt(pLevel, 0, maxLevel);

			// Prestige Master Rank
			if (ImGui::Button("Send##PRESTIGEMASTERRANK"))
			{
				ClampInt(ParagonRank, minRank, 1000);

				paragonRankXp = GetParagonXPFromTable(currentMode, ParagonRank);

				setMasterRank(ParagonRank);
				setMasterXP(paragonRankXp);

				LiveStorage_UploadStatsForController(0);
			}

			ImGui::SameLine();
			InputIntClamped("Prestige Master Lvl##RANK", &ParagonRank, minRank, 1000);

			// Rank Icon
			if (ImGui::Button("Send##RANKICON")) {

				ClampInt(icon, 0, maxRankIcon);
				if (RankIconExistsForMode(currentMode, icon)) {
					LiveStats_SetStatByKey(
						Com_SessionMode_GetMode(),
						CONTROLLER_INDEX_0,
						MP_PLAYERSTATSKEY_PARAGONICONID,
						icon
					);
					LiveStorage_UploadStatsForController(0);
				}
			}

			ImGui::SameLine();
			InputIntClamped("Rank Icon##RANK", &icon, 0, maxRankIcon);

			ImGui::Separator();

			ImGui::BulletText("Stat Editor");

			ImGui::Separator();

			if (ImGui::Button("Send##SCORE")) {
				ClampInt(iScore, 0, kStatMax);
				setStatbyName("score", iScore);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Score##RANK", &iScore, 0, kStatMax);

			if (ImGui::Button("Send##KILLS")) {
				ClampInt(iKills, 0, kStatMax);
				setStatbyName("kills", iKills);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Kills##RANK", &iKills, 0, kStatMax);

			if (ImGui::Button("Send##DEATHS")) {
				ClampInt(iDeaths, 0, kStatMax);
				setStatbyName("deaths", iDeaths);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Deaths##RANK", &iDeaths, 0, kStatMax);

			if (ImGui::Button("Send##ASSISTS")) {
				ClampInt(iAssists, 0, kStatMax);
				setStatbyName("assists", iAssists);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Assists##RANK", &iAssists, 0, kStatMax);

			if (ImGui::Button("Send##HEADSHOTS")) {
				ClampInt(iHeadshots, 0, kStatMax);
				setStatbyName("headshots", iHeadshots);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Headshots##RANK", &iHeadshots, 0, kStatMax);

			if (ImGui::Button("Send##TEAMKILLS")) {
				ClampInt(iTeamKills, 0, kStatMax);
				setStatbyName("TEAMKILLS", iTeamKills);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Teamkills##RANK", &iTeamKills, 0, kStatMax);

			if (ImGui::Button("Send##SUICIDES")) {
				ClampInt(iSuicides, 0, kStatMax);
				setStatbyName("SUICIDES", iSuicides);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Suicides##RANK", &iSuicides, 0, kStatMax);

			if (ImGui::Button("Send##TPALLIES")) {
				ClampInt(iTimePlayedAllies, 0, kStatMax);
				setStatbyName("TIME_PLAYED_ALLIES", iTimePlayedAllies);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Time Played (Allies)##RANK", &iTimePlayedAllies, 0, kStatMax);

			if (ImGui::Button("Send##TPOPFOR")) {
				ClampInt(iTimePlayedOpFor, 0, kStatMax);
				setStatbyName("TIME_PLAYED_OPFOR", iTimePlayedOpFor);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Time Played (OpFor)##RANK", &iTimePlayedOpFor, 0, kStatMax);

			if (ImGui::Button("Send##TPOTHER")) {
				ClampInt(iTimePlayedOther, 0, kStatMax);
				setStatbyName("TIME_PLAYED_OTHER", iTimePlayedOther);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Time Played (Other)##RANK", &iTimePlayedOther, 0, kStatMax);

			if (ImGui::Button("Send##TPTOTAL")) {
				ClampInt(iTimePlayedTotal, 0, kStatMax);
				setStatbyName("TIME_PLAYED_TOTAL", iTimePlayedTotal);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Time Played (Total)##RANK", &iTimePlayedTotal, 0, kStatMax);

			if (ImGui::Button("Send##TOTALGAMESPLAYED")) {
				ClampInt(iTotalGamesPlayed, 0, kStatMax);
				setStatbyName("total_games_played", iTotalGamesPlayed);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Total Games Played##RANK", &iTotalGamesPlayed, 0, kStatMax);

			if (ImGui::Button("Send##KDRATIO")) {
				ClampInt(iKD, 0, kStatMax);
				setStatbyName("KDRATIO", iKD);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("KD Ratio##RANK", &iKD, 0, kStatMax);

			if (ImGui::Button("Send##WINS")) {
				ClampInt(iWins, 0, kStatMax);
				setStatbyName("WINS", iWins);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Wins##RANK", &iWins, 0, kStatMax);

			if (ImGui::Button("Send##LOSSES")) {
				ClampInt(iLosses, 0, kStatMax);
				setStatbyName("LOSSES", iLosses);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Losses##RANK", &iLosses, 0, kStatMax);

			if (ImGui::Button("Send##TIES")) {
				ClampInt(iTies, 0, kStatMax);
				setStatbyName("TIES", iTies);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Ties##RANK", &iTies, 0, kStatMax);

			if (ImGui::Button("Send##CUR_WIN_STERAK")) {
				ClampInt(iCurWinStreak, 0, kStatMax);
				setStatbyName("CUR_WIN_STREAK", iCurWinStreak);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Win Streak##RANK", &iCurWinStreak, 0, kStatMax);

			if (ImGui::Button("Send##WLRATIO")) {
				ClampInt(iWL, 0, kStatMax);
				setStatbyName("WLRATIO", iWL);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("WL Ratio##RANK", &iWL, 0, kStatMax);

			if (ImGui::Button("Send##HITS")) {
				ClampInt(iHits, 0, kStatMax);
				setStatbyName("HITS", iHits);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Hits##RANK", &iHits, 0, kStatMax);

			if (ImGui::Button("Send##MISSES")) {
				ClampInt(iMisses, 0, kStatMax);
				setStatbyName("MISSES", iMisses);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Misses##RANK", &iMisses, 0, kStatMax);

			if (ImGui::Button("Send##TOTALSHOTS")) {
				ClampInt(iTotalShots, 0, kStatMax);
				setStatbyName("TOTAL_SHOTS", iTotalShots);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Total Shots##RANK", &iTotalShots, 0, kStatMax);

			if (ImGui::Button("Send##ACCURACY")) {
				ClampInt(iAccuracy, 0, kStatMax);
				setStatbyName("ACCURACY", iAccuracy);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Accuracy##RANK", &iAccuracy, 0, kStatMax);

			if (ImGui::Button("Send##HIGHESTROUND")) {
				ClampInt(iHighestRound, 0, kStatMax);
				setStatbyName("HIGHEST_ROUND_REACHED", iHighestRound);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Highest Round Reached##RANK", &iHighestRound, 0, kStatMax);

			if (ImGui::Button("Send##TOTALROUNDSSURVIVED")) {
				ClampInt(iTotalRounds, 0, kStatMax);
				setStatbyName("TOTAL_ROUNDS_SURVIVED", iTotalRounds);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			InputIntClamped("Total Rounds Survived##RANK", &iTotalRounds, 0, kStatMax);

			ImGui::Separator();


			if (ImGui::Button("Unlock All Class Slots")) {

				unlockClassSlots();
				LiveStorage_UploadStatsForController(0);
			}

			if (ImGui::Button("Max Unlock Tokens")) {

				setMaxTokens();
				LiveStorage_UploadStatsForController(0);
			}

			if (ImGui::Button("Max Weapon Stats")) {

				setMaxWeapons();
				setGroupStats();
				setMaxAttachments();
				LiveStorage_UploadStatsForController(0);
			}

			if (ImGui::Button("Unlock Fresh Start Calling Card")) {

				LiveStats_SetStatByKey(Com_SessionMode_GetMode(), CONTROLLER_INDEX_0, MP_PLAYERSTATSKEY_STATRESETCOUNT, 1);

			}

			if (ImGui::Button("Hard Unlock Calling Cards")) {
				unlockCallingCards();
				LiveStorage_UploadStatsForController(0);
			}

			if (ImGui::Button("Hard Unlock Entitlements")) {

				for (int i = 0; i < 600; ++i) {

					LiveKeyArchive_SetValueForController(CONTROLLER_INDEX_0, KEYARCHIVE_CATEGORY_ENTITLEMENTS, (keyArchiveIndex)i, 0xFFFFFFFFi64, BD_ADD);

				}

				LiveKeyArchive_WriteAllCategories(CONTROLLER_INDEX_0, true);

				ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "Entitlements Unlocked!" });

			}

			if (ImGui::Button("Hard Unlock Contracts")) {
				unlockContracts(1, 9, 1);
				unlockContracts(1000, 1030, 2);
				unlockContracts(3000, 3029, 3);
				LiveStorage_UploadStatsForController(0);
				ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "Contracts Unlocked! Join and leave at least 9 public \nmatches to make contract rewards appear in your inventory\n\nCheck force host, search for MP match, and use Launch game button (MP tab)\nto make this process quicker." });
			}

			if (ImGui::Button("Complete Challenges")) {
				UnlockMilestoneTable("mp_statstable", 0, 256, 9, 2, 3);
				setStatMilestones("mp", "statsmilestones1.csv");
				setStatMilestones("mp", "statsmilestones2.csv");
				setStatMilestones("mp", "statsmilestones3.csv");
				setStatMilestones("mp", "statsmilestones4.csv");
				setStatMilestones("mp", "statsmilestones5.csv");
				setStatMilestones("mp", "statsmilestones6.csv");

				setStatMilestones("zm", "statsmilestones3.csv");

				setStatMilestones("cp", "statsmilestones1.csv");
				setStatMilestones("cp", "statsmilestones3.csv");
				unlockArenaStats();
				setGameTypeStats();
				LiveStorage_UploadStatsForController(0);
			}

			if (ImGui::Button("Complete ZM Daily Challenge")) {
				completeDailyChallenges();
			}

			if (ImGui::Button("Complete Hero Challenges")) {
				unlockSpecialistOutfits();
			}

			if (ImGui::Button("Complete Campaign")) {
				LiveAchievements_GiveAchievement("CP_COMPLETE_PROLOGUE");
				LiveAchievements_GiveAchievement("CP_COMPLETE_NEWWORLD");
				LiveAchievements_GiveAchievement("CP_COMPLETE_BLACKSTATION");
				LiveAchievements_GiveAchievement("CP_COMPLETE_BIODOMES");
				LiveAchievements_GiveAchievement("CP_COMPLETE_SGEN");
				LiveAchievements_GiveAchievement("CP_COMPLETE_VENGEANCE");
				LiveAchievements_GiveAchievement("CP_COMPLETE_RAMSES");
				LiveAchievements_GiveAchievement("CP_COMPLETE_INFECTION");
				LiveAchievements_GiveAchievement("CP_COMPLETE_AQUIFER");
				LiveAchievements_GiveAchievement("CP_COMPLETE_LOTUS");
				LiveAchievements_GiveAchievement("CP_HARD_COMPLETE");
				LiveAchievements_GiveAchievement("CP_REALISTIC_COMPLETE");
				LiveAchievements_GiveAchievement("CP_CAMPAIGN_COMPLETE");
				CompleteAllMissions();
			}

			if (ImGui::Button("Max Collectibles")) {
				LiveAchievements_GiveAchievement("CP_ALL_COLLECTIBLES");
				LiveAchievements_GiveAchievement("CP_MISSION_COLLECTIBLES");
				UnlockAllCollectibles();
			}

			if (ImGui::Button("Max Accolades")) {
				LiveAchievements_GiveAchievement("CP_ALL_COLLECTIBLES");
				UnlockAllAccolades();
			}

			if (ImGui::Button("Max Medals")) {
				LiveAchievements_GiveAchievement("CP_ALL_DECORATIONS");
				UnlockAllMedals();
			}

			if (ImGui::Button("Unlock All Achievements")) {
				UnlockAllAchievements();
			}

			ImGui::TextDisabled("Suggest more unlocks");

			ImGui::EndChild();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Zombies", nullptr)) {

			ImGui::BeginChild("##ZOMBIES", ImGui::GetContentRegionAvail());

			ImGui::Dummy(ImVec2(0, 5));

			ImGui::Text("Host Status: ");
			ImGui::SameLine();
			if (hooks::AreWeInGameAndHosting()) {
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Host");
			}
			else {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Client");
			}

			ImGui::Checkbox("Force Host", &bForceHost);

			if (bForceHost && !bForceHostRan) {
				host::setforcehostdvars(true);
				bForceHostRan = true;
			}

			if (!bForceHost && bForceHostRan) {
				host::setforcehostdvars(false);
				bForceHostRan = false;
			}

			ImGui::SameLine();

			HelpMarker("Enable if you want to always be the host of a public zombies lobby. Enable once before searching for public matches. You should load into lobbies by yourself. Wait for players to join and start the match. Host status should switch to host when loading in.");

			if (bForceHost) {

				ImGui::SameLine();

				ImGui::Dummy(ImVec2(50, 0));

				ImGui::SameLine();

				if (ImGui::Button("Start Game")) {
					Dvar_SetFromString("party_minPlayers", "1", true);
					Cbuf_AddText(0, "LaunchGame");
				}

				ImGui::SameLine();

				HelpMarker("Activate in lobby to start the match.");

			}

			ImGui::Checkbox("Online Rank / Gobblegums in Workshop Maps", &bModTools);

			ImGui::Separator();

			ImGui::Dummy(ImVec2(0, 5));


			if (ImGui::Button("Fast Restart Map")) {
				Cbuf_AddText(0, "fast_restart");
			}

			ImGui::SameLine(0.0f, 3.0f);

			if (ImGui::Button("Full Restart Map")) {
				Cbuf_AddText(0, "map_restart");
			}

			ImGui::SameLine(0.0f, 3.0f);


			ImGui::Checkbox("First Gum Free", &bFirstGumFree);

			if (bFirstGumFree && !bFirstGumFreeRan) {
				Dvar_SetFromString("scr_firstGumFree", "1", false);
				bFirstGumRan = true;
			}

			if (!bFirstGumFree && bFirstGumFreeRan) {
				Dvar_SetFromString("scr_firstGumFree", "0", false);
				bFirstGumRan = false;
			}
			ImGui::SameLine();

			HelpMarker("Relive the OG days where the first gobblegum wasn't free!");

			ImGui::Checkbox("Sv_Cheats", &bsvCheats);

			if (bsvCheats && !bsvCheatsRan) {
				Dvar_SetFromString("sv_cheats", "1", false);
				bsvCheatsRan = true;
			}

			if (!bsvCheats && bsvCheatsRan) {
				Dvar_SetFromString("sv_cheats", "0", false);
				bsvCheatsRan = false;
			}
			ImGui::SameLine();

			HelpMarker("Toggle server cheats to run console commands like noclip or godmode");

			ImGui::SameLine(0.0f, 3.0f);

			ImGui::Checkbox("God Mode", &bGodMode);

			ImGui::SameLine(0.0f, 3.0f);

			ImGui::Checkbox("Inf Ammo", &bInfAmmo);

			if (bInfAmmo && !bInfAmmoRan) {
				Dvar_SetFromString("player_sustainAmmo", "1", false);
				bInfAmmoRan = true;
			}

			if (!bInfAmmo && bInfAmmoRan) {
				Dvar_SetFromString("player_sustainAmmo", "0", false);
				bInfAmmoRan = false;
			}

			if (ImGui::Button("No Teddy Bear")) {
				Dvar_SetFromString("magic_chest_movable", "0", false);
			}

			ImGui::Checkbox("Third Person##TP", &bThirdPerson);

			ImGui::Checkbox("Thorns Mode##DMG", &bThorns);
			ImGui::SameLine(0.0f, 3.0f);
			ImGui::Checkbox("Nukes Mode##DMG", &bNukes);
			//ImGui::SameLine();
			ImGui::Checkbox("##DMG", &bDamageMultiplier);

			ImGui::SameLine();
			ImGui::SliderInt("Damage Multiplier", &iDamageMultiplier, 1, 100000);

			ImGui::Separator();

			ImGui::Dummy(ImVec2(0, 5));
			ImGui::BulletText("Map Stat Editor");

			ImGui::Combo("Map", &map, zmmapnames, IM_ARRAYSIZE(zmmapnames));
			//ImGui::SameLine();
			ImGui::SliderInt("Round##STAT", &setRound, 0, 1000);

			if (ImGui::Button("Set Map Stat##MAP")) {

				setMapStat(map, setRound);

				LiveStorage_UploadStatsForController(0);
			}

			ImGui::SameLine(0.0f, 5.0f);


			if (ImGui::Button("Set all Maps##MAP")) {
				setMaxMapStats(setRound);
			}

			if (ImGui::Button("Set EE##MAP")) {

				setMapEE(map);

				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine(0.0f, 5.0f);

			if (ImGui::Button("Set All EEs Completed##MAP")) {
				setAllMapEE();
			}

			ImGui::SameLine(0.0f, 0.0f);

			HelpMarker("Set round to 0 to clear all EEs.");

			ImGui::Separator();

			ImGui::Dummy(ImVec2(0, 5));

			ImGui::BulletText("Gobblegum Editor");

			static int BgbID;

			const char* ItemName = getItemNameforItemID(BgbID);

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

			ImGui::InputTextWithHint("##INPUTGOBBLEPACKNAME", "GobblePack Name", &sPackName);

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

			static std::string previewBGBValue = "";

			if (ImGui::BeginCombo("Gobblegum##BGB", previewBGBValue.c_str()))
			{
				for (int i = 0; i < 256; i++) {

					std::string itemName = getItemNameforItemID(i);

					if (!itemName.empty()) {
						ImGui::Selectable(itemName.c_str());
						if (ImGui::IsItemClicked()) {
							previewBGBValue = itemName;
							BgbID = i;
						}

					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Set Gobblegum"))
			{

				CACRoot Buffer;

				eGameModes a = Com_SessionMode_GetGameMode();
				eModes a1 = Com_SessionMode_GetMode();
				CACType a3 = LiveStats_Loadouts_GetCACTypeForMode(a1, a);
				auto CACroot = LiveStats_Loadouts_GetCACRoot(&Buffer, 0, a3);
				SetGobblegum(&Buffer, PackIndex, BuffIndex, BgbID);
			}

			ImGui::SameLine(0.0f, 5.0f);

			if (ImGui::Button("Set Pack Name"))
			{
				setGobblePackName(PackIndex, sPackName);
			}

			ImGui::EndChild();
			ImGui::EndTabItem();

		}

		if (ImGui::BeginTabItem("Multiplayer", nullptr)) {

			ImGui::BeginChild("##MULTIPLAYER", ImGui::GetContentRegionAvail());
			ImGui::Dummy(ImVec2(0, 5));

			if (ImGui::Button("Launch MP Game")) {
				Dvar_SetFromString("party_minPlayers", "1", true);
				Cbuf_AddText(0, "LaunchGame");
			}

			ImGui::SameLine();

			ImGui::Checkbox("Make Custom Game Earn XP", &bCustomXP);

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Make sure you enable this AFTER the 10 second countdown!");
			}

			if (ImGui::Button("Modded Lobby##GSPEED")) {
				auto callvote = "callvote map \"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nfast_restart\nset scr_tdm_score_kill 9999999999\nscr_xpscaleMP 99999\"";
				CL_AddReliableCommand(0, callvote);
			}

			ImGui::Separator();

			ImGui::BulletText("Specialist Editor");

			const char* characters[10]{
			"Ruin",
			"Outrider",
			"Prophet",
			"Battery",
			"Seraph",
			"Nomad",
			"Reaper",
			"Spectre",
			"Firebreak",
			"Blackjack"
			};

			static int weaponIndex = 0;
			static int camoIndex = 0;
			static int characterIndex = 0;
			static int headIndex = 0;
			static int BodyIndex = 0;
			static int ColorIndex = 0;

			ImGui::SetNextItemWidth(250);
			ImGui::Combo("Specialist", &characterIndex, characters, IM_ARRAYSIZE(characters));

			if (ImGui::Button("Set##HEADINDEX")) {
				LiveStats_SetSelectedItemIndex(0, MODE_MULTIPLAYER, CUSTOMIZATION_STATS_LOCATION_STATS, characterIndex, CHARACTER_ITEM_TYPE_HELMET, headIndex);
				ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "Successfully set." });
			}
			ImGui::SameLine();
			ImGui::InputInt("Head Index", &headIndex);

			if (ImGui::Button("Set##BODYINDEX")) {
				LiveStats_SetSelectedItemIndex(0, MODE_MULTIPLAYER, CUSTOMIZATION_STATS_LOCATION_STATS, characterIndex, CHARACTER_ITEM_TYPE_BODY, BodyIndex);
				ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "Successfully set." });
			}
			ImGui::SameLine();
			ImGui::InputInt("Body Index", &BodyIndex);

			if (ImGui::Button("Set##CHARCOLOR")) {
				LiveStats_SetSelectedItemColor(0, MODE_MULTIPLAYER, CUSTOMIZATION_STATS_LOCATION_STATS, characterIndex, CHARACTER_ITEM_TYPE_HELMET, headIndex, 0, ColorIndex);
				LiveStats_SetSelectedItemColor(0, MODE_MULTIPLAYER, CUSTOMIZATION_STATS_LOCATION_STATS, characterIndex, CHARACTER_ITEM_TYPE_BODY, BodyIndex, 0, ColorIndex);
				ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "Successfully set." });
			}
			ImGui::SameLine();
			ImGui::InputInt("Color Index", &ColorIndex);

			ImGui::Separator();

			const char* ShowcaseWeaponName = getItemNameforItemID(weaponIndex);
			ImGui::BulletText("Showcase Weapon: %s", ShowcaseWeaponName);

			static std::string previewWeapValue = "";
			static std::string previewCamoValue = "";

			if (ImGui::BeginCombo("Showcase Weapon##SHOWCASE", previewWeapValue.c_str()))
			{
				for (int i = 0; i < 256; i++) {

					std::string itemName = getItemNameforItemID(i);

					if (!itemName.empty()) {
						ImGui::Selectable(itemName.c_str());
						if (ImGui::IsItemClicked()) {
							previewWeapValue = itemName;
							weaponIndex = i;
						}

					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Showcase Weapon Camo##SHOWCASE", previewCamoValue.c_str()))
			{
				for (int i = 0; i < 138; i++) {

					auto itemName = getComboItemName(i, false, CBC_CAMOS);

					if (itemName != nullptr) {
						ImGui::Selectable(itemName);
						if (ImGui::IsItemClicked()) {
							previewCamoValue = itemName;
							camoIndex = i;
						}
					}
				}
				ImGui::EndCombo();
			}
			//ImGui::InputInt("Camo ID##SHOWCASE", &camoIndex);

			if (ImGui::Button("Set Showcase Weapon")) {

				static auto DDLContextForLocation = ((DDLContext * (__cdecl*)(__int64 ControllerIndex, unsigned int sessionMode, int location))(ProcessBase + 0x1E9DFE0))(0, MODE_MULTIPLAYER, CUSTOMIZATION_STATS_LOCATION_STATS);

				Variant setVariant{};
				setVariant.weaponIndex = weaponIndex;
				setVariant.camoIndex = camoIndex;
				setVariant.paintjobIndex = -1;
				setVariant.paintjobSlot = -1;


				LiveStats_SetShowcaseWeapon(MODE_MULTIPLAYER, CUSTOMIZATION_STATS_LOCATION_STATS, DDLContextForLocation, CAC_TYPE_INVALID, characterIndex, setVariant);
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("BlackMarket", nullptr)) {
			ImGui::BeginChild("##BLACKMARKET", ImGui::GetContentRegionAvail());

			ImGui::Dummy(ImVec2(0, 5));
			ImGui::SeparatorText("Cryptokeys");
			ImGui::Dummy(ImVec2(0, 5));

			ImGui::SliderInt("Loot Speed (ms)", &iLootSpeed, 0, 500);

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("How long in ms should we wait to call crypto functions again.");
			}

			ImGui::SliderInt("Crypto Amount ##CRYPTOAMT", &iCryptoAmt, 0, 48);

			ImGui::Checkbox("Loop Cryptokeys", &bCrypto);

			ImGui::SameLine(0.0f, 5.0f);

			ImGui::Checkbox("Spend Cryptokeys", &bCryptoSpend);

			ImGui::Button("Fix Cryptokeys"); {
				resetCrypto();
			}

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Use this if your cryptokey amount always shows - after looping.");
			}

			ImGui::SameLine(0.0f, 5.0f);

			if (ImGui::Button("Burn Duplicates"))
			{
				Loot_BurnDuplicates(0, eModes::MODE_MULTIPLAYER);
			}

			ImGui::Dummy(ImVec2(0, 5));
			ImGui::SeparatorText("Gain Gobblegum");
			ImGui::Dummy(ImVec2(0, 5));

			ImGui::Checkbox("Loop Divinium", &bDivinium);

			ImGui::SameLine(0.0f, 5.0f);

			ImGui::Checkbox("Spend Divinium", &bDiviniumSpend);


			//if (ImGui::Button("Burn Gobblegum"))
			//{
			//	Loot_BurnDuplicates(0, eModes::MODE_ZOMBIES);
			//}

			ImGui::Dummy(ImVec2(0, 5));
			ImGui::SeparatorText("Misc");
			ImGui::Dummy(ImVec2(0, 5));

			if (ImGui::Button("No Dupe Supply Drops")) {
				Dvar_SetFromString("loot_limitedTimeItemPromo_active", "1", true);
				Dvar_SetFromString("loot_limitedTimeItemPromo_promoString", "Scropts", true);
				Dvar_SetFromString("loot_limitedTimeItemPromo_promoIcon", "t7_icon_blackmarket_promo_grandslam_green", true);
				Dvar_SetFromString("loot_noDupeRare_active", "1", true);
				ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "Successfully activated No Dupe Drops." });
			}

			if (ImGui::Button("Triple Play Supply Drops")) {
				Dvar_SetFromString("loot_limitedTimeItemPromo_active", "1", true);
				Dvar_SetFromString("loot_limitedTimeItemPromo_promoString", "Scropts", true);
				Dvar_SetFromString("loot_limitedTimeItemPromo_promoIcon", "t7_icon_blackmarket_promo_grandslam_green", true);
				Dvar_SetFromString("loot_trifectaActive", "1", true);
				Dvar_SetFromString("loot_trifecta_cryptoCost", "500", true);
				Dvar_SetFromString("trifecta_cryptokeys_drop_id", "1", true);
				ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "Successfully activated Triple Plays." });
			}

			/*ImGui::SetNextItemWidth(200.0f);

			ImGui::Combo("Guarantee Bribe", &iBribe, BlackMarketGuaranteeBribes, IM_ARRAYSIZE(BlackMarketGuaranteeBribes));

			ImGui::SameLine();

			if (ImGui::Button("Set##BRIBE")) {
				runDvars(setDvar_e(iBribe));
				ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "Successfully activated bribe: %s.", BlackMarketGuaranteeBribes[iBribe] });
			}*/

			if (ImGui::Button("Grant ZC Reward Gobblegums")) {
				runDvars(ZMHD_GOBBLEGUM_REWARDS);
			}

			ImGui::SameLine();

			HelpMarker("Grants you a free pack of Gobblegums once");


			if (ImGui::Button("Free Gobblegum Challenge")) {
				Dvar_SetFromString("ui_enablePromoMenu", "1", true);
				Dvar_SetFromString("ui_enableZMHDFeaturedCard", "1", true);
				ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "Successfully activated Gobblegum Challenge.\nCheck it out in the contracts section to redeem rewards!" });
			}

			static int chosenBackground;

			static std::string previewValue = "";

			if (ImGui::BeginCombo("Calling Card##BACKGROUND", previewValue.c_str()))
			{
				for (int i = 0; i < 750; i++) {

					std::string cardName = get_column_value_for_row_from_path("gamedata/emblems/backgrounds.csv", i, 4);

					if (!cardName.empty()) {

						auto finalName = UI_SafeTranslateString(cardName.data());

						if (finalName != nullptr) {
							ImGui::Selectable(finalName);
							if (ImGui::IsItemClicked()) {
								previewValue = finalName;
								*(int*)(ProcessBase + 0x11287DD0) = i;
								chosenBackground = i;
							}
						}
					}
				}
				ImGui::EndCombo();
			}

			ImGui::EndChild();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Spoof", nullptr)) {
			ImGui::BeginChild("##SPOOF", ImGui::GetContentRegionAvail());
			ImGui::Dummy(ImVec2(0, 5));

			ImGui::Checkbox("Unlock Full Game (DLC)", &bUnlockFullGame);

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Make sure the host of the match is using this QOL if you are playing a zombies map you unlocked with this!");
			}

			ImGui::Checkbox("Blackmarket Items", &bSpoofBlackMarket);

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Sets the quantity of every black market item and gobblegum to the quantity you choose.\n\nSpecial supply drops 'unlocked' with this will error out and not give any items (Grand Slam, contract bribes etc). Spending cryptokeys and LD you have will still work as normal.");
			}

			if (bSpoofBlackMarket) {
				ImGui::SameLine();
				ImGui::SetNextItemWidth(160);
				ImGui::InputInt("##AMOUNT", &iBlackmarketAmt, 1, 100);
			}


			ImGui::Checkbox("Entitlements", &bSpoofPurchases);

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Spoof unlocks all entitlements, such as pre-order content and special events.");
			}

			ImGui::Checkbox("Extra Slots", &bSpoofSlots);

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Spoof extra slot ownership. Gives you 10 CAC tabs and allows you\nto download / create more emblems, paintjobs, etc.");
			}


			ImGui::Checkbox("Fileshare Downloading", &bFileshareDownloading);

			ImGui::SameLine();

			HelpMarker("Allows you to download items from the media browser (Emblems, Paintshop variants, etc.). Also spoofs ownership of items downloaded through scropts QOL, allowing you to modify your newly downloaded item!\n\nYou can find an item's download button by going to the media browser in the menus, selecting an item, and then going to the social menu [S].");


			if (ImGui::Button("Show Hidden Items")) {
				unlock_secret_items();
			}

			if (bFileshareDownloading && !bFileshareDownloadingRan) {
				runDvars(FILESHARE_ALLOWDOWNLOAD);
				bFileshareDownloadingRan = true;
				//ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "Successfully activated Fileshare Downloading!" });
			}

			if (!bFileshareDownloading && bFileshareDownloadingRan) {
				runDvars(FILESHARE_DISABLEDOWNLOAD);
				bFileshareDownloadingRan = false;
				//ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "Deactivated Fileshare Downloading." });
			}


			if (ImGui::Button("Fix Downloaded Items")) {

				for (int j = 0; j != 32; j++) {
					auto fromState = Storage_GetDDLRootState(26);
					const char* path[8]{};
					char toState[2000]{};
					auto storageContext = Storage_GetDDLContext(0, 26, 0);

					path[0] = "emblemList";
					path[1] = intToConstCharPtr(j);
					path[2] = "readOnly";

					if (DDL_MoveToPath((__int64)fromState, toState, 3, path)) {
						DDL_SetInt((__int64)toState, (__int64)storageContext, 0);
						ZeroMemory(toState, sizeof(toState));
					}
					Storage_Write(0, 26, 0);
				}

			}

			ImGui::SameLine();

			HelpMarker("Allows you to edit old emblems downloaded through other menus.");

			ImGui::EndChild();
			ImGui::EndTabItem();

		}

		if (ImGui::BeginTabItem("Class Editor", nullptr)) {

			ImGui::BeginChild("##CLASSEDITORR", ImGui::GetContentRegionAvail());

			ImGui::Dummy(ImVec2(0, 5));

			if (ImGui::Button("Open New Class Editor")) {
				bClassEditor = true;
			}

			ImGui::Separator();

			ImGui::BulletText("Old Class Editor");

			static int ItemId;

			static int ClassIndex;
			static int BuffIndex;
			static int PackIndex;
			static int class_sel;
			static int slotItem;

			ImGui::Combo("Class Index", &class_sel, customclass, IM_ARRAYSIZE(customclass));

			ImGui::InputInt("Item ID", &ItemId);

			auto ItemName = getItemNameforItemID(ItemId);

			ImGui::Text(ItemName);

			ImGui::Combo("Slot Name", &slotItem, slots, IM_ARRAYSIZE(slots));

			ClassIndex = class_sel;

			if (ImGui::Button("Set Class Item"))
			{

				CACRoot Buffer{};
				eGameModes a = Com_SessionMode_GetGameMode();
				eModes a1 = Com_SessionMode_GetMode();
				CACType a3 = LiveStats_Loadouts_GetCACTypeForMode(a1, a);
				auto CACroot = LiveStats_Loadouts_GetCACRoot(&Buffer, 0, a3);
				__int64 a4 = BG_UnlockablesGetLoadoutSlotFromString(slots[slotItem]);
				BG_UnlockablesSetItemIndex(&Buffer, (unsigned int)ClassIndex, a4, ItemId);

			}

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Make sure you are in the Create-A-Class menu.");
			}

			ImGui::EndChild();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Weapon Stats", nullptr))
		{
			ImGui::BeginChild("##WEPRANKING", ImGui::GetContentRegionAvail());

			ImGui::Dummy(ImVec2(0, 5));

			static int BgbID;

			static std::string previewBGBValue = "";

			if (ImGui::BeginCombo("Weapon##WEPRANK", previewBGBValue.c_str()))
			{
				for (int i = 0; i < 256; i++) {

					std::string itemName = getItemNameforItemID(i);

					if (!itemName.empty()) {
						ImGui::Selectable(itemName.c_str());
						if (ImGui::IsItemClicked()) {
							previewBGBValue = itemName;
							BgbID = i;
						}
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Set Weapon Stats##WEPRANK"))
			{
				SetWeaponStats(BgbID);
			}
			ImGui::SameLine();

			if (ImGui::Button("Set All Weapon Stats##WEPRANK"))
			{
				for (int i = 0; i < 256; i++) {
					SetWeaponStats(i);
				}
			}

			ImGui::Dummy(ImVec2(0, 5));

			ImGui::Separator();
			ImGui::BulletText("Stat Editor");
			ImGui::Separator();

			constexpr int kStatMax = MAXINT / 2;

			InputIntClamped("Purchased##WEP", &g_WeaponStats.purchased, 0, 3);
			InputIntClamped("XP##WEP", &g_WeaponStats.xp, 0, 665535);
			InputIntClamped("Kills##WEP", &g_WeaponStats.kills, 0, kStatMax);
			InputIntClamped("Level##WEP", &g_WeaponStats.plevel, 0, 15);
			InputIntClamped("Hits##WEP", &g_WeaponStats.hits, 0, kStatMax);
			InputIntClamped("Misses##WEP", &g_WeaponStats.misses, 0, kStatMax);
			InputIntClamped("Accuracy##WEP", &g_WeaponStats.accuracy, 0, 100);
			InputIntClamped("Headshots##WEP", &g_WeaponStats.headshots, 0, kStatMax);
			InputIntClamped("Uses##WEP", &g_WeaponStats.used, 0, kStatMax);
			InputIntClamped("Assists##WEP", &g_WeaponStats.assists, 0, kStatMax);
			InputIntClamped("Assist Score##WEP", &g_WeaponStats.assist_score, 0, kStatMax);
			InputIntClamped("Challenges_tu##WEP", &g_WeaponStats.challenges_tu, 0, kStatMax);
			InputIntClamped("Destroyed Aircraft##WEP", &g_WeaponStats.destroyed_aircraft, 0, kStatMax);
			InputIntClamped("Destroyed Aircraft Under 20s##WEP", &g_WeaponStats.destroyed_aircraft_under20s, 0, kStatMax);
			InputIntClamped("Destroy 2 Killstreaks Rapidly##WEP", &g_WeaponStats.destroy_2_killstreaks_rapidly, 0, kStatMax);
			InputIntClamped("Destroy 5 Killstreak Vehicle##WEP", &g_WeaponStats.destroy_5_killstreak_vehicle, 0, kStatMax);
			InputIntClamped("Destroy AiTank or Setinel##WEP", &g_WeaponStats.destroy_aitank_or_setinel, 0, kStatMax);
			InputIntClamped("Destroy Turret##WEP", &g_WeaponStats.destroy_turret, 0, kStatMax);
			InputIntClamped("Direct Hit Kills##WEP", &g_WeaponStats.direct_hit_kills, 0, kStatMax);
			InputIntClamped("Bloodthirsty##WEP", &g_WeaponStats.killstreak_5, 0, kStatMax);
			InputIntClamped("Kills While Active##WEP", &g_WeaponStats.kills_while_active, 0, kStatMax);
			InputIntClamped("One Shots Shotgun##WEP", &g_WeaponStats.kill_enemy_one_bullet_shotgun, 0, kStatMax);
			InputIntClamped("One Shots Snipers##WEP", &g_WeaponStats.kill_enemy_one_bullet_sniper, 0, kStatMax);
			InputIntClamped("Kills While Injured##WEP", &g_WeaponStats.kill_enemy_when_injured, 0, kStatMax);
			InputIntClamped("Kill Enemy W Their Weapon##WEP", &g_WeaponStats.kill_enemy_with_their_weapon, 0, kStatMax);
			InputIntClamped("Loaded Kills##WEP", &g_WeaponStats.loadedKills, 0, kStatMax);
			InputIntClamped("Longshots##WEP", &g_WeaponStats.longshot_kill, 0, kStatMax);
			InputIntClamped("No Attachment Kills##WEP", &g_WeaponStats.noAttKills, 0, kStatMax);
			InputIntClamped("No Perk Kills##WEP", &g_WeaponStats.noPerkKills, 0, kStatMax);
			InputIntClamped("Revenge Kills##WEP", &g_WeaponStats.revenge_kill, 0, kStatMax);
			InputIntClamped("Blackhat Hacks##WEP", &g_WeaponStats.hacks, 0, kStatMax);
			InputIntClamped("EMP Destroys##WEP", &g_WeaponStats.destroyed, 0, kStatMax);
			InputIntClamped("Stun Kills##WEP", &g_WeaponStats.stunkills, 0, kStatMax);
			InputIntClamped("Blind Kills##WEP", &g_WeaponStats.blindkills, 0, kStatMax);
			InputIntClamped("Captures with Smoke##WEP", &g_WeaponStats.capture, 0, kStatMax);
			InputIntClamped("Trophy System Defends##WEP", &g_WeaponStats.projectiles, 0, kStatMax);

			ImGui::EndChild();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Tools", nullptr)) {
			ImGui::BeginChild("##TOOLS", ImGui::GetContentRegionAvail());

			ImGui::Dummy(ImVec2(0, 5));

			static std::string dvar_input = "";

			ImGui::InputTextWithHint("##DVAR", "DVAR", &dvar_input);

			ImGui::SameLine();

			if (ImGui::Button("Send##DVAR"))
			{
				std::vector<std::string> sep = split(dvar_input, ' ');

				if (!sep[0].empty())
					Dvar_SetFromString(sep[0].c_str(), sep[1].c_str(), false);
				else {
					ImGui::InsertNotification({ ImGuiToastType::Error, 5000, "Insert a Valid Dvar." });
				}
			}

			static std::string cbuf_input = "";

			ImGui::InputTextWithHint("##CBUF", "CBUF", &cbuf_input);
			ImGui::SameLine();

			if (ImGui::Button("Send##CBUF"))
			{
				Cbuf_AddText(0, cbuf_input.data());
			}

			static auto clan_input = std::string{ "" };

			ImGui::Dummy(ImVec2(0.0f, 3.0f));
			ImGui::SetNextItemWidth(75);
			ImGui::InputTextWithHint("##CLAN", "CLAN", &clan_input);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(249);
			ImGui::InputTextWithHint("##NAME", "NAME", &nameBuffer);
			ImGui::SameLine();

			if (ImGui::Button("Set##CLAN"))
			{
				if (clan_input.size()) {
					LiveStats_SetClanTagText(0, clan_input.data());
				}

				if (nameBuffer.size()) {
					nameBuffer = utils::decodeEncodedChars(nameBuffer);
					strncpy_s(spoofName, sizeof(spoofName), nameBuffer.data(), -1);
					hooks::renamePlayer(spoofName);
					if (!clan_input.size()) {
						auto clan = LiveStats_GetClanName(0);
						LiveStats_SetClanTagText(0, clan);
					}
				}

				if (!clan_input.size() && !nameBuffer.size()) {
					LiveStats_SetClanTagText(0, "");
				}
			}


			ImGui::Checkbox("Colored UI", &bColoredUI);

			if (bColoredUI) {

				ImGui::SameLine();
				ImGui::ColorEdit4("##UIColorEditor", (float*)&uiColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				ImGui::SameLine();
				ImGui::Checkbox("RGB##MENU", &bUIRgb);

			}

			ImGui::Checkbox("Tracers", &bTracers);
			ImGui::SameLine();
			ImGui::Checkbox("All Tracers", &bAllTracers);

			ImGui::Checkbox("Steam Friends Only", &bFriendsOnly);

			ImGui::SameLine();

			HelpMarker("Enabling this drops all info requests and responses from players who are not on your steam friends list.");

			if (ImGui::Button("Apply Hooks")) {
				hooks::applyPatches();
			}

			if (ImGui::Button("Back to Main Menu")) {
				Cbuf_AddText(0, "disconnect");
			}

			int currentIndex = 0;
			for (int i = 0; i < 17; i++) {
				if (Keybinds[i] == (int)OpenKeybind) {
					currentIndex = i;
					selectedBind = i;
					break;
				}
			}

			if (ImGui::BeginCombo("Menu Keybind", KeybindNames[currentIndex])) {
				for (int i = 0; i < 17; i++) {
					bool isSelected = (currentIndex == i);
					if (ImGui::Selectable(KeybindNames[i], isSelected)) {
						currentIndex = i;
						OpenKeybind = (UINT)Keybinds[i];
					}
				}
				ImGui::EndCombo();
			}
			ImGui::EndChild();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Emblems", nullptr))
		{
			emblemsTabVisible = true;

			DrawEmblemsTab();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Servers", nullptr)) {

			servers::draw();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Friends", nullptr)) {

			players::draw();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Info", nullptr)) {

			ImGui::BeginChild("##INFO", ImGui::GetContentRegionAvail());

			ImGui::Dummy(ImVec2(0, 5));
			ImGui::InputTextWithHint("##TITLE", "Screenshot Title", &custom_title_buf);
			ImGui::InputTextWithHint("##DESC", "Screenshot Description", &custom_desc_buf);

			//ImGui::SeparatorText("Emblem Auto Export");
			ImGui::Dummy(ImVec2(10, 10));
			ImGui::Text("Version: %s", SCROPTS_VERSION);

			ImGui::EndChild();
			ImGui::EndTabItem();

		}

		ImGui::EndTabBar();

		ImGui::End();

		if (bClassEditor) {

			eModes mode = Com_SessionMode_GetMode();

			// if Multiplayer:
			//if (mode == eModes::MODE_MULTIPLAYER) {

			ImGui::SetNextWindowSize(ImVec2(900.0f, 900.0f));

			if (ImGui::Begin("Class Editor", &bClassEditor, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {

				ImGui::Dummy(ImVec2(5, 5));

				//ImGui::BeginChild("##LEFTSIDE", ImVec2(ImGui::GetContentRegionAvail().x / 2, 300));

				static int classSetIndex = 0;
				static int classIndex = 0;
				static std::string className = "";
				static std::string classSetName = "";

				if (classSetName.size() > 16) {
					classSetName.resize(16);
				}

				if (className.size() > 16) {
					className.resize(16);
				}

				ImGui::Text("Class-Set:");

				ImGui::SameLine();

				ImGui::SetNextItemWidth(50.0f);
				ImGui::Combo("##CLASSSETINDEX", &classSetIndex, "1\0002\0003\0004\0005\0006\0007\0008\0009\00010\000");

				//ImGui::InputInt("Class-Set##INDEX", &classSetIndex);

				ImGui::SameLine();

				ImGui::SetNextItemWidth(320.0f);
				ImGui::InputTextWithHint("##INPUTCLASSSETNAME", "Enter Set Name...", &classSetName);

				ImGui::SameLine();

				if (ImGui::Button("Update Set Name##CLASSSETNAME")) {
					eGameModes a = Com_SessionMode_GetGameMode();
					eModes a1 = Com_SessionMode_GetMode();
					auto a3 = LiveStats_ClassSets_GetClassSetTypeForMode(a1, a);
					BG_UnlockablesSetClassSetName(0, a3, classSetIndex, classSetName.c_str());
				}


				//ImGui::SameLine();

				ImGui::Text("Class Index:");

				ImGui::SameLine();

				ImGui::SetNextItemWidth(50.0f);
				ImGui::Combo("##CLASSINDEX", &classIndex, "1\0002\0003\0004\0005\0006\0007\0008\0009\00010\000");

				ImGui::SameLine();

				ImGui::Dummy(ImVec2(5, 0));

				ImGui::SameLine();

				ImGui::SetNextItemWidth(320.0f);
				ImGui::InputTextWithHint("##INPUTCLASSNAME", "Input Class Name...", &className);

				ImGui::SameLine();

				if (ImGui::Button("Update Class Name##CLASSNAME")) {
					setClassName(classSetIndex, classIndex, className);
				}

				//ImGui::SameLine();


				//if (ImGui::Button("Load Class##LOAD")) {

				//	// BG_UnlockablesGetClassSetItem

				//	LoadClassTable(primarySlotsTable, 10, classSetIndex, classIndex);
				//	LoadClassTable(secondarySlotsTable, 10, classSetIndex, classIndex);
				//	LoadClassTable(offhandSlotsTable, 8, classSetIndex, classIndex);
				//	LoadClassTable(wildcardSlotsTable, 9, classSetIndex, classIndex);

				//}


				////ImGui::BeginDisabled();

				////ImGui::SameLine();

				//if (ImGui::Button("Send All##ALL")) {
				//	ApplySlotTable(primarySlotsTable, 10, classSetIndex, classIndex);
				//	ApplySlotTable(secondarySlotsTable, 10, classSetIndex, classIndex);
				//	ApplySlotTable(offhandSlotsTable, 8, classSetIndex, classIndex);
				//	ApplySlotTable(wildcardSlotsTable, 9, classSetIndex, classIndex);
				//}

				//ImGui::EndDisabled();

				//ImGui::SameLine(5,0);

				if (ImGui::Button("500 Class Items##")) {

					setClassSlotByName(255, classIndex, "primarygrenade");
					setClassSlotByName(255, classIndex, "primarygrenadecount");
					setClassSlotByName(255, classIndex, "specialgrenade");
					setClassSlotByName(255, classIndex, "specialgrenadecount");

				}
				if (ImGui::IsItemHovered()) {
					ImGui::SetTooltip("Make sure you are in your selected class in CAC before clicking this button.");
				}

				ImGui::SameLine(150, 0);
				ImGui::SetNextItemWidth(120.0f);
				ImGui::Combo("Combo Box Type", &iComboBoxType, "Weapons\000Attachments\000Camos\000Reticles\000Perks\000WildCards\000KillStreaks\000Extra\000");


				auto limit = getComboItemSize(false);

				if (ImGui::BeginChild("##PRIMARY", ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2))) {

					auto primaryLimit = getComboItemSize(true);

					ImGui::BulletText("Primaries");

					ImGui::Separator();

					if (ImGui::BeginChild("##PRIMARYLIST", ImGui::GetContentRegionAvail())) {

						// Primary Weapon and attachment / camo editor
						DrawSlotTable(primarySlotsTable, 10, classSetIndex, classIndex);
						ImGui::EndChild();
					}

					ImGui::EndChild();
				}

				ImGui::SameLine();

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

				ImGui::SameLine();

				if (ImGui::BeginChild("##SECONDARY", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2))) {

					ImGui::BulletText("Secondaries");

					ImGui::Separator();

					if (ImGui::BeginChild("##SECONDARYLIST", ImGui::GetContentRegionAvail())) {

						// Secondary Weapon and attachment / camo editor
						DrawSlotTable(secondarySlotsTable, 10, classSetIndex, classIndex);

						ImGui::EndChild();
					}
					ImGui::EndChild();
				}

				ImGui::Separator();

				if (ImGui::BeginChild("##OFFHANDS", ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y))) {

					ImGui::BulletText("Offhands");

					ImGui::Separator();

					if (ImGui::BeginChild("##OFFHANDLIST", ImGui::GetContentRegionAvail())) {

						// Wildcard and maybe Streak Editor?
						DrawSlotTable(offhandSlotsTable, 8, classSetIndex, classIndex);

						ImGui::EndChild();
					}
					ImGui::EndChild();
				}

				ImGui::SameLine();

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

				ImGui::SameLine();

				if (ImGui::BeginChild("##WILDCARDS", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y))) {

					ImGui::BulletText("Perks & Wildcards");

					ImGui::Separator();

					if (ImGui::BeginChild("##WILDCARDLIST", ImGui::GetContentRegionAvail())) {

						DrawSlotTable(wildcardSlotsTable, 9, classSetIndex, classIndex);

						// Lethals, Tacticals

						ImGui::EndChild();
					}
					ImGui::EndChild();
				}

				// If we can find a way to edit ZM: 


				ImGui::End();
			}
		}
	}

	else {
		*(int*)((DWORD64)GetModuleHandleA(NULL) + 0x17DF0405) = 1;

	}
}

void MergeIconsWithLatestFont(float font_size, bool FontDataOwnedByAtlas = false)
{
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.FontDataOwnedByAtlas = FontDataOwnedByAtlas;

	ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)fa_solid_900, sizeof(fa_solid_900), font_size, &icons_config, icons_ranges);
}

void CleanupRenderTarget()
{
	if (pContext)
	{
		pContext->OMSetRenderTargets(0, nullptr, nullptr);
	}

	if (mainRenderTargetView)
	{
		mainRenderTargetView->Release();
		mainRenderTargetView = nullptr;
	}

	renderTargetReady = false;
}

bool CreateRenderTarget(IDXGISwapChain* swapChain)
{
	if (!swapChain || !pDevice)
		return false;

	ID3D11Texture2D* pBackBuffer = nullptr;

	HRESULT hr = swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&pBackBuffer)
	);

	if (FAILED(hr) || !pBackBuffer)
		return false;

	hr = pDevice->CreateRenderTargetView(
		pBackBuffer,
		nullptr,
		&mainRenderTargetView
	);

	pBackBuffer->Release();

	renderTargetReady = SUCCEEDED(hr) && mainRenderTargetView;
	return renderTargetReady;
}

HRESULT __stdcall hkResizeBuffers(
	IDXGISwapChain* pSwapChain,
	UINT BufferCount,
	UINT Width,
	UINT Height,
	DXGI_FORMAT NewFormat,
	UINT SwapChainFlags)
{
	CleanupRenderTarget();

	if (ImGui::GetCurrentContext())
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();
	}

	HRESULT hr = oResizeBuffers(
		pSwapChain,
		BufferCount,
		Width,
		Height,
		NewFormat,
		SwapChainFlags
	);

	if (SUCCEEDED(hr))
	{
		DXGI_SWAP_CHAIN_DESC sd{};
		if (SUCCEEDED(pSwapChain->GetDesc(&sd)))
		{
			window = sd.OutputWindow;
		}

		if (ImGui::GetCurrentContext())
		{
			ImGui_ImplDX11_CreateDeviceObjects();
		}

		CreateRenderTarget(pSwapChain);
	}

	return hr;
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&pDevice))))
		{
			return oPresent(pSwapChain, SyncInterval, Flags);
		}

		pDevice->GetImmediateContext(&pContext);

		gSwapChain = pSwapChain;
		gSwapChain->AddRef();

		DXGI_SWAP_CHAIN_DESC sd{};
		pSwapChain->GetDesc(&sd);
		window = sd.OutputWindow;

		CreateRenderTarget(pSwapChain);

		oWndProc = reinterpret_cast<WNDPROC>(
			SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc))
			);

		InitImGui();
		MergeIconsWithLatestFont(16.0f, false);

		init = true;
	}

	if (!mainRenderTargetView)
	{
		CreateRenderTarget(pSwapChain);
	}

	static bool openKeyWasDown = false;
	bool openKeyIsDown = (GetAsyncKeyState(OpenKeybind) & 0x8000) != 0;
	if (openKeyIsDown && !openKeyWasDown)
	{
		open = !open;
	}
	openKeyWasDown = openKeyIsDown;

	EmblemPreview::UpdateReleaseGuards(open);

	hooks::onFrame();

	EmblemPreview::Update();

	if (bDivinium)
	{
		if (clock() - UnlockTMR > iLootSpeed)
		{
			char buf_cmd[255];
			sprintf_s(buf_cmd, "%c %u %u", 120, 3, 250);
			SV_GameSendServerCommand(0, 1, buf_cmd);
			UnlockTMR = clock();
		}
	}

	if (bCrypto)
	{
		if (clock() - UnlockTMR > iLootSpeed)
		{
			GiveLootToSelf(0, 1, iCryptoAmt * 100);
			UnlockTMR = clock();
		}
	}

	if (bCryptoSpend)
	{
		bCrypto = false;
		bSpoofBlackMarket = false;
		Loot_BuyCrate(0, 1, 2);
	}

	if (bDiviniumSpend)
	{
		bDivinium = false;
		bSpoofBlackMarket = false;
		Loot_SpendVials(0, 3);
	}

	if (mainRenderTargetView)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		const bool menuOpenBeforeDraw = open;

		draw();
		drawTracers();

		if (bNotifications)
		{
			ImGui::RenderNotifications();
		}

		ImGui::Render();

		pContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	return oPresent(pSwapChain, SyncInterval, Flags);
}

HookReturn initDirectXPresentPointerSwap()
{
	auto swapChainVtable = **reinterpret_cast<uintptr_t***>(ProcessBase + 0xF4378D8);

	DWORD oldProtect = 0;

	oPresent = reinterpret_cast<Present_t>(swapChainVtable[8]);

	VirtualProtect(&swapChainVtable[8], sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &oldProtect);
	swapChainVtable[8] = reinterpret_cast<uintptr_t>(hkPresent);
	VirtualProtect(&swapChainVtable[8], sizeof(uintptr_t), oldProtect, &oldProtect);

	oResizeBuffers = reinterpret_cast<ResizeBuffers_t>(swapChainVtable[13]);

	VirtualProtect(&swapChainVtable[13], sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &oldProtect);
	swapChainVtable[13] = reinterpret_cast<uintptr_t>(hkResizeBuffers);
	VirtualProtect(&swapChainVtable[13], sizeof(uintptr_t), oldProtect, &oldProtect);

	return Success;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DWORD old;
		dllbase = hMod;

		MH_Initialize();
		initDirectXPresentPointerSwap();
		hooks::initExceptionHandler();
		hooks::initPointerSwaps();

		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

/* TODO
* - Clean up the menu, improve GUI code in general, add more tooltips and notifications, etc. Maybe phantom UI?
* - Add aimbot (smooth, snap), walls, rapid fire, auto fire, infinite ammo, no recoil, etc.
*/