#include "includes.h"
#include <string>
#include <sstream>
#include <vector>
#include "host.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LPVOID oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;
UINT OpenKeybind = VK_F5;
int selectedBind = 4;

typedef unsigned __int64 ull;
typedef ull             uint64;
#define _QWORD uint64

bool bDisclaimerShown = false;
bool bNotifications = true;
bool bForceHost;
bool bForceHostRan;
bool bFirstGumRan;
bool open = true;
bool bClassEditor;
bool bDivinium;
bool bLoginReward = true;
bool bDiviniumSpend;
bool bCrypto;
bool bCryptoSpend;
bool bProtectStats;
bool bProtectStatsRan;
bool bCompleteEE;
bool bArena;
int UnlockTMR = clock();
std::string version = "3.0.3";
std::string sPackName;

int minRank = 0;
static int iCryptoAmt = 1500;
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
	case WM_SETCURSOR:
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
		break; }
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

	Cbuf_AddText(0, "achievement CP_COMPLETE_PROLOGUE");
	Cbuf_AddText(0, "achievement CP_COMPLETE_NEWWORLD");
	Cbuf_AddText(0, "achievement CP_COMPLETE_BLACKSTATION");
	Cbuf_AddText(0, "achievement CP_COMPLETE_BIODOMES");
	Cbuf_AddText(0, "achievement CP_COMPLETE_SGEN");
	Cbuf_AddText(0, "achievement CP_COMPLETE_VENGEANCE");
	Cbuf_AddText(0, "achievement CP_COMPLETE_RAMSES");
	Cbuf_AddText(0, "achievement CP_COMPLETE_INFECTION");
	Cbuf_AddText(0, "achievement CP_COMPLETE_AQUIFER");
	Cbuf_AddText(0, "achievement CP_COMPLETE_LOTUS");
	Cbuf_AddText(0, "achievement CP_HARD_COMPLETE");
	Cbuf_AddText(0, "achievement CP_REALISTIC_COMPLETE");
	Cbuf_AddText(0, "achievement CP_CAMPAIGN_COMPLETE");
	Cbuf_AddText(0, "achievement CP_FIREFLIES_KILL");
	Cbuf_AddText(0, "achievement CP_UNSTOPPABLE_KILL");
	Cbuf_AddText(0, "achievement CP_FLYING_WASP_KILL");
	Cbuf_AddText(0, "achievement CP_TIMED_KILL");
	Cbuf_AddText(0, "achievement CP_ALL_COLLECTIBLES");
	Cbuf_AddText(0, "achievement CP_DIFFERENT_GUN_KILL");
	Cbuf_AddText(0, "achievement CP_ALL_DECORATIONS");
	Cbuf_AddText(0, "achievement CP_ALL_WEAPON_CAMOS");
	Cbuf_AddText(0, "achievement CP_CONTROL_QUAD");
	Cbuf_AddText(0, "achievement CP_MISSION_COLLECTIBLES");
	Cbuf_AddText(0, "achievement CP_DISTANCE_KILL");
	Cbuf_AddText(0, "achievement CP_OBSTRUCTED_KILL");
	Cbuf_AddText(0, "achievement CP_MELEE_COMBO_KILL");
	Cbuf_AddText(0, "achievement CP_COMPLETE_WALL_RUN");
	Cbuf_AddText(0, "achievement CP_TRAINING_GOLD");
	Cbuf_AddText(0, "achievement CP_COMBAT_ROBOT_KILL");
	Cbuf_AddText(0, "achievement CP_KILL_WASPS");
	Cbuf_AddText(0, "achievement CP_CYBERCORE_UPGRADE");
	Cbuf_AddText(0, "achievement CP_ALL_WEAPON_ATTACHMENTS");
	Cbuf_AddText(0, "achievement CP_TIMED_STUNNED_KILL");
	Cbuf_AddText(0, "achievement CP_UNLOCK_DOA");

	// MP Achievements

	Cbuf_AddText(0, "achievement MP_REACH_SERGEANT");
	Cbuf_AddText(0, "achievement MP_REACH_ARENA");
	Cbuf_AddText(0, "achievement MP_SPECIALIST_MEDALS");
	Cbuf_AddText(0, "achievement MP_MULTI_KILL_MEDALS");

	// ZM Achievements

	Cbuf_AddText(0, "achievement ZM_COMPLETE_RITUALS");
	Cbuf_AddText(0, "achievement ZM_SPOT_SHADOWMAN");
	Cbuf_AddText(0, "achievement ZM_GOBBLE_GUM");
	Cbuf_AddText(0, "achievement ZM_STORE_KILL");
	Cbuf_AddText(0, "achievement ZM_ROCKET_SHIELD_KILL");
	Cbuf_AddText(0, "achievement ZM_CIVIL_PROTECTOR");
	Cbuf_AddText(0, "achievement ZM_WINE_GRENADE_KILL");
	Cbuf_AddText(0, "achievement ZM_MARGWA_KILL");
	Cbuf_AddText(0, "achievement ZM_PARASITE_KILL");
	Cbuf_AddText(0, "achievement ZM_CASTLE_EE");
	Cbuf_AddText(0, "achievement ZM_CASTLE_ALL_BOWS");
	Cbuf_AddText(0, "achievement ZM_CASTLE_MINIGUN_MURDER");
	Cbuf_AddText(0, "achievement ZM_CASTLE_UPGRADED_BOW");
	Cbuf_AddText(0, "achievement ZM_CASTLE_MECH_TRAPPER");
	Cbuf_AddText(0, "achievement ZM_CASTLE_SPIKE_REVIVE");
	Cbuf_AddText(0, "achievement ZM_CASTLE_WALL_RUNNER");
	Cbuf_AddText(0, "achievement ZM_CASTLE_ELECTROCUTIONER");
	Cbuf_AddText(0, "achievement ZM_CASTLE_WUNDER_TOURIST");
	Cbuf_AddText(0, "achievement ZM_CASTLE_WUNDER_SNIPER");
	Cbuf_AddText(0, "achievement ZM_ISLAND_COMPLETE_EE");
	Cbuf_AddText(0, "achievement ZM_ISLAND_DRINK_WINE");
	Cbuf_AddText(0, "achievement ZM_ISLAND_CLONE_REVIVE");
	Cbuf_AddText(0, "achievement ZM_ISLAND_OBTAIN_SKULL");
	Cbuf_AddText(0, "achievement ZM_ISLAND_WONDER_KILL");
	Cbuf_AddText(0, "achievement ZM_ISLAND_STAY_UNDERWATER");
	Cbuf_AddText(0, "achievement ZM_ISLAND_THRASHER_RESCUE");
	Cbuf_AddText(0, "achievement ZM_ISLAND_ELECTRIC_SHIELD");
	Cbuf_AddText(0, "achievement ZM_ISLAND_DESTROY_WEBS");
	Cbuf_AddText(0, "achievement ZM_ISLAND_EAT_FRUIT");
	Cbuf_AddText(0, "achievement ZM_STALINGRAD_NIKOLAI");
	Cbuf_AddText(0, "achievement ZM_STALINGRAD_WIELD_DRAGON");
	Cbuf_AddText(0, "achievement ZM_STALINGRAD_TWENTY_ROUNDS");
	Cbuf_AddText(0, "achievement ZM_STALINGRAD_RIDE_DRAGON");
	Cbuf_AddText(0, "achievement ZM_STALINGRAD_LOCKDOWN");
	Cbuf_AddText(0, "achievement ZM_STALINGRAD_BEAM_KILL");
	Cbuf_AddText(0, "achievement ZM_STALINGRAD_STRIKE_DRAGON");
	Cbuf_AddText(0, "achievement ZM_STALINGRAD_FAFNIR_KILL");
	Cbuf_AddText(0, "achievement ZM_STALINGRAD_AIR_ZOMBIES");
	Cbuf_AddText(0, "achievement ZM_GENESIS_EE");
	Cbuf_AddText(0, "achievement ZM_GENESIS_SUPER_EE");
	Cbuf_AddText(0, "achievement ZM_GENESIS_PACKECTOMY");
	Cbuf_AddText(0, "achievement ZM_GENESIS_KEEPER_ASSIST");
	Cbuf_AddText(0, "achievement ZM_GENESIS_DEATH_RAY");
	Cbuf_AddText(0, "achievement ZM_GENESIS_GRAND_TOUR");
	Cbuf_AddText(0, "achievement ZM_GENESIS_WARDROBE_CHANGE");
	Cbuf_AddText(0, "achievement ZM_GENESIS_WONDERFUL");
	Cbuf_AddText(0, "achievement ZM_GENESIS_CONTROLLED_CHAOS");
	Cbuf_AddText(0, "achievement DLC2_ZOMBIE_ALL_TRAPS");
	Cbuf_AddText(0, "achievement DLC2_ZOM_LUNARLANDERS");
	Cbuf_AddText(0, "achievement DLC2_ZOM_FIREMONKEY");
	Cbuf_AddText(0, "achievement DLC4_ZOM_TEMPLE_SIDEQUEST");
	Cbuf_AddText(0, "achievement DLC4_ZOM_SMALL_CONSOLATION");
	Cbuf_AddText(0, "achievement DLC5_ZOM_CRYOGENIC_PARTY");
	Cbuf_AddText(0, "achievement DLC5_ZOM_GROUND_CONTROL");
	Cbuf_AddText(0, "achievement ZM_DLC4_TOMB_SIDEQUEST");
	Cbuf_AddText(0, "achievement ZM_DLC4_OVERACHIEVER");
	Cbuf_AddText(0, "achievement ZM_PROTOTYPE_I_SAID_WERE_CLOSED");
	Cbuf_AddText(0, "achievement ZM_ASYLUM_ACTED_ALONE");
	Cbuf_AddText(0, "achievement ZM_THEATER_IVE_SEEN_SOME_THINGS");
}


// Campaign 

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
	using namespace std;

	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	auto a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsByMap";
	
	char result[2000];

	for (int i = 0; i < 11; i++) {
		path[1] = cpmapnames[i];
		path[2] = "hasBeenCompleted";
		DDL_MoveToPath(tmp, result, 3, path);
		DDL_SetUInt((__int64)result, a1, 1);
		ZeroMemory(result, size(result));
		path[2] = "score";
		DDL_MoveToPath(tmp, result, 3, path);
		DDL_SetUInt((__int64)result, a1, 99999999);
		ZeroMemory(result, size(result));
	}
	LiveStorage_UploadStatsForController(0);

}

// Arena 

void setArenaPoints(int points) {
	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	auto a1 = GetStatsBuffer(0);
	path[0] = "arenaStats";
	path[1] = "points";
	char result[2000];
	DDL_MoveToPath(tmp, result, 2, path);
	DDL_SetUInt((__int64)result, a1, points);
	ZeroMemory(result, sizeof(result));
}


// MP

void setStatbyName(const char* statName, int value) {
	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	char result[2000];
	path[1] = statName;
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, value);
	ZeroMemory(result, sizeof(result));

}

void setGroupStats() {
	auto mode = Com_SessionMode_GetMode();
	__int64 RootState = LiveStats_Core_GetRootDDLState(mode);
	__int64 DDLContext = GetStatsBuffer(0);
	char toState[2000]{};

	// The categories/groups you provided
	const char* groups[] = {
		"weapon_grenade", "weapon_pistol", "weapon_smg", "weapon_assault",
		"weapon_lmg", "weapon_cqb", "weapon_sniper", "weapon_launcher",
		"weapon_knife", "killstreak"
	};

	// The specific challenge keys you provided
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

			// 1. Set the Stat Value (The actual number achieved)
			path[4] = "statValue";
			if (DDL_MoveToPath(RootState, toState, 5, path)) {
				DDL_SetUInt((__int64)toState, DDLContext, 999999);
			}
			ZeroMemory(toState, sizeof(toState));

			// 2. Set the Challenge Value (Progress toward completion)
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

// 1. Pass by reference (&) to avoid making a copy of the vector
void GetValue(const std::vector<std::string>& path) {
	auto root = LiveStats_Core_GetRootDDLState(Com_SessionMode_GetMode());
	auto ctx = GetStatsBuffer(0);
	char toState[2000]{};
	const char* arr[8];
	int depth = 0;

	// 2. Iterate by reference (const auto& p) 
	// This ensures p.c_str() points to the strings living in the 'parts' vector 
	// inside your Button function, which stays alive until GetValue returns.
	for (const auto& p : path) {
		if (depth < 8) {
			arr[depth++] = p.c_str();
		}
	}

	// Now arr[0], arr[1], etc., point to valid memory
	ImGui::InsertNotification({ ImGuiToastType::None, 2000, "%s %s %s", arr[0], arr[1], arr[2] });

	if (DDL_MoveToPath(root, toState, depth, arr)) {
		lastIntResult = DDL_GetUInt((__int64)toState, ctx);
		// Note: ZeroMemory isn't strictly necessary if toState is local, but doesn't hurt.
		ZeroMemory(toState, 2000);
	}
}

void setGameTypeStats() {

	auto root = LiveStats_Core_GetRootDDLState(Com_SessionMode_GetMode());
	auto ctx = GetStatsBuffer(0);
	char toState[2000]{};

	// List of gamemodes provided
	const char* gameModes[] = {
		"ball", "conf", "ctf", "dem", "dm", "dom", "escort", "gun",
		"hcconf", "hcctf", "hcdem", "hcdm", "hcdom", "hchq", "hckoth",
		"hcsd", "hctdm", "hq", "koth", "sas", "sd", "tdm"
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

	// Core stats
	SetValue(root, ctx, toState, { base[0], base[1], "purchased" }, g_WeaponStats.purchased);
	SetValue(root, ctx, toState, { base[0], base[1], "xp" }, g_WeaponStats.xp);
	SetValue(root, ctx, toState, { base[0], base[1], "plevel" }, g_WeaponStats.plevel);

	for (int i = 0; i < 3; i++)
		SetValue(root, ctx, toState, { base[0], base[1], "isproversionunlocked" }, g_WeaponStats.purchased);

	// Core tracked stats
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


	// Challenge progression
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
		const char* p3[23] = { "kills","kills_ability", "kills_weapon", "multikill_ability", "multikill_weapon", "kill_one_game_ability", "kill_one_game_weapon", "challenge1", "challenge2", "challenge3", "challenge4", "challenge5", "challenge6", "challenge7", "challenge8", "challenge9", "challenge10", "challenge11", "challenge12", "transmission1", "transmission2", "transmission3", "transmission4"};

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

void setMasterRank(int rank) {
	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	char result[2000];

	path[1] = "paragon_rank";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, rank - minRank);
	ZeroMemory(result, sizeof(result));
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

void setAllRanks()
{
	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	if (bArena) {
		path[2] = "arenaValue";
	}
	char result[2000];

	path[1] = "plevel";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, pPrestige);
	ZeroMemory(result, sizeof(result));

	path[1] = "rank";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, pLevel);
	ZeroMemory(result, sizeof(result));

	path[1] = "rankxp";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, rankXp);
	ZeroMemory(result, sizeof(result));

	path[1] = "paragon_rankxp";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, paragonRankXp);
	ZeroMemory(result, sizeof(result));

	path[1] = "paragon_rank";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, ParagonRank - minRank);
	ZeroMemory(result, sizeof(result));
	
}

void setStats() {

	auto tmp = LiveStats_Core_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	char result[2000];

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

void setChallenges(const char* mode, const char* csv)
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

		int value = 50000; // fallback default

		if (targetStr && targetStr[0])
			value = std::stoi(targetStr);

		if (!value) value = 50000;

		// -----------------------------
		// playerstatslist.<name>.statValue
		// -----------------------------
		path[0] = "playerStatsList";
		path[1] = challengeName;
		path[2] = "statValue";

		if (DDL_MoveToPath(root, stateBuf, 3, path))
			DDL_SetUInt((__int64)stateBuf, ctx, value);

		// -----------------------------
		// playerstatslist.<name>.challengeValue
		// -----------------------------
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

void unlockContracts(int startID, int endID, int type)
{
	StringTable* table = nullptr;
	StringTable_GetAsset("gamedata/tables/mp/mp_contracttable.csv", &table);

	auto RootState = LiveStats_Core_GetRootDDLState(Com_SessionMode_GetMode());
	auto DDLContext = GetStatsBuffer(0);

	char stateBuf[2000]{};

	for (int contractID = startID; contractID <= endID; contractID++)
	{
		const char* path[3]{};
		const char* statPath[3]{};

		std::string idStr = std::to_string(contractID);

		// 🔥 Lookup by ContractID (NOT row index)
		const char* ContractIDStr = StringTable_Lookup(table, 0, idStr.c_str(), 0);
		if (!ContractIDStr || !ContractIDStr[0])
			continue;

		const char* ChallengeValue = StringTable_Lookup(table, 0, idStr.c_str(), 2);
		const char* ChallengeName = StringTable_Lookup(table, 0, idStr.c_str(), 7);
		const char* ChallengeAlias = StringTable_Lookup(table, 0, idStr.c_str(), 8);
		const char* ExtraBytes = StringTable_Lookup(table, 0, idStr.c_str(), 9);

		int progress = (ChallengeValue && ChallengeValue[0]) ? std::stoi(ChallengeValue) : 0;

		// -----------------------------
		// contracts.<type>.active = 1
		// -----------------------------
		path[0] = "contracts";
		path[1] = intToConstCharPtr(type);
		path[2] = "active";

		if (DDL_MoveToPath(RootState, stateBuf, 3, path))
			DDL_SetUInt((__int64)stateBuf, DDLContext, 1);

		// -----------------------------
		// contracts.<type>.index = ID
		// -----------------------------
		path[2] = "index";

		if (DDL_MoveToPath(RootState, stateBuf, 3, path))
			DDL_SetUInt((__int64)stateBuf, DDLContext, contractID);

		// -----------------------------
		// contracts.<type>.progress = value
		// -----------------------------
		path[2] = "progress";

		if (DDL_MoveToPath(RootState, stateBuf, 3, path))
			DDL_SetUInt((__int64)stateBuf, DDLContext, progress);

		// -----------------------------
		// contracts.<type>.award_given = 1
		// -----------------------------
		path[2] = "award_given";

		if (DDL_MoveToPath(RootState, stateBuf, 3, path))
			DDL_SetUInt((__int64)stateBuf, DDLContext, 1);

		// =========================
		// OPTIONAL WRITES
		// =========================

		if (ChallengeName && ChallengeName[0])
		{
			statPath[0] = "playerStatsList";
			statPath[1] = ChallengeName;
			statPath[2] = "statValue";

			if (DDL_MoveToPath(RootState, stateBuf, 3, statPath))
				DDL_SetUInt((__int64)stateBuf, DDLContext, 1);
		}

		if (ChallengeAlias && ChallengeAlias[0])
		{
			statPath[0] = "playerStatsList";
			statPath[1] = ChallengeAlias;
			statPath[2] = "statValue";

			if (DDL_MoveToPath(RootState, stateBuf, 3, statPath))
				DDL_SetUInt((__int64)stateBuf, DDLContext, 1);
		}

		if (ExtraBytes && ExtraBytes[0])
		{
			std::vector<std::string> parts = split(ExtraBytes, ' ');

			std::vector<const char*> dynPath;
			dynPath.reserve(parts.size());

			for (auto& s : parts)
				dynPath.push_back(s.c_str());

			if (DDL_MoveToPath(RootState, stateBuf, (int)dynPath.size(), dynPath.data()))
				DDL_SetUInt((__int64)stateBuf, DDLContext, 1);
		}
	}
}

//void unlockContracts(int index, int max, int type) {
//
//	StringTable* table = nullptr;
//	StringTable_GetAsset("gamedata/tables/mp/mp_contracttable.csv", &table);
//	__int64 RootState = LiveStats_Core_GetRootDDLState(Com_SessionMode_GetMode());
//	__int64 DDLContext = GetStatsBuffer(0);
//
//	while (index < max) {
//		
//		const char* path[8]{};
//		const char* statPath[8]{};
//		
//		char toState[2000]{};
//
//		auto value = intToConstCharPtr(index);
//		auto ContractID = StringTable_Lookup(table, 0i64, value, 0);
//		auto ChallengeValue = StringTable_Lookup(table, 0i64, value, 2);
//		auto ChallengeName = StringTable_Lookup(table, 0i64, value, 7);
//		auto ChallengeAlias = StringTable_Lookup(table, 0i64, value, 8);
//		auto ExtraBytes = StringTable_Lookup(table, 0i64, value, 9);
//
//		path[0] = "contracts";
//		path[1] = intToConstCharPtr(type);
//
//		path[2] = "active";
//
//		if (DDL_MoveToPath(RootState, toState, 3, path)) {
//			DDL_SetUInt((__int64)toState, DDLContext, 1);
//			ZeroMemory(toState, sizeof(toState));
//		}
//
//		path[2] = "index";
//
//		if (DDL_MoveToPath(RootState, toState, 3, path)) {
//			auto iContractID = std::stoi(ContractID);
//			DDL_SetUInt((__int64)toState, DDLContext, iContractID);
//			ZeroMemory(toState, sizeof(toState));
//		}
//
//		path[2] = "progress";
//
//		if (DDL_MoveToPath(RootState, toState, 3, path)) {
//			auto iChallengeValue = std::stoi(ChallengeValue);
//			DDL_SetUInt((__int64)toState, DDLContext, iChallengeValue);
//			ZeroMemory(toState, sizeof(toState));
//		}
//
//		path[2] = "award_given";
//
//		if (DDL_MoveToPath(RootState, toState, 3, path)) {
//			DDL_SetUInt((__int64)toState, DDLContext, 1);
//			ZeroMemory(toState, sizeof(toState));
//		}
//
//		if (ChallengeName != nullptr && ChallengeName[0] != '\0') {
//
//			statPath[0] = "playerStatsList";
//			statPath[1] = ChallengeName;
//			statPath[2] = "statValue";
//
//			if (DDL_MoveToPath(RootState, toState, 3, statPath)) {
//				DDL_SetUInt((__int64)toState, DDLContext, 1);
//				ZeroMemory(toState, sizeof(toState));
//			}
//
//		}
//
//		if (ChallengeAlias != nullptr && ChallengeAlias[0] != '\0') {
//
//			statPath[0] = "playerStatsList";
//			statPath[1] = ChallengeAlias;
//			statPath[2] = "statValue";
//
//			if (DDL_MoveToPath(RootState, toState, 3, statPath)) {
//				DDL_SetUInt((__int64)toState, DDLContext, 1);
//				ZeroMemory(toState, sizeof(toState));
//			}
//
//		}
//
//		if (ExtraBytes != nullptr && ExtraBytes[0] != '\0') {
//
//			const char* sepPath[8]{};
//
//			// Split the string into sep parts
//			//auto sep = utils::splitString(ExtraBytes);
//			std::vector<std::string> sep = split(ExtraBytes, ' ');
//
//			for (int i = 0; i < sep.size(); i++) {
//				sepPath[i] = sep[i].data();
//			}
//
//			int depth = sep.size();
//
//			if (DDL_MoveToPath(RootState, toState, depth, sepPath)) {
//				DDL_SetUInt((__int64)toState, DDLContext, 1);
//				ZeroMemory(toState, sizeof(toState));
//			}
//
//		}
//
//		index++;
//	}
//
//}

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
		std::vector<const char*> base = { "Attachments", BG_GetAttachmentName(i)};

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

	BG_UnlockablesSetClassSetItem(CONTROLLER_INDEX_0, classSetType, classSetIndex, classIndex , slotName, item);

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
	ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "setClassItem: Successfully set item %i in %s (Class index %i)", ItemId, slots[slotItem], classIndex});

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

	if (!bDisclaimerShown)
	{
		*(int*)((DWORD64)OFFSET(0x17DF0405)) = 0;

		// 1. Setup Background Dimming
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		ImDrawList* drawlist = ImGui::GetBackgroundDrawList();
		drawlist->AddRectFilled(
			ImVec2(0, 0),
			ImVec2((float)desktop.right, (float)desktop.bottom),
			IM_COL32(0, 0, 0, 150)
		);

		// 2. Set Window Position (Center of Screen) and Size
		// Increased size slightly to accommodate larger text
		ImVec2 window_size = ImVec2(600.0f, 250.0f);
		ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(
			ImVec2(desktop.right * 0.5f, desktop.bottom * 0.5f),
			ImGuiCond_Always,
			ImVec2(0.5f, 0.5f)
		);

		// 3. Render Disclaimer Window
		if (ImGui::Begin(" - Disclaimer - ", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			// --- INCREASE TEXT SIZE ---
			ImGui::SetWindowFontScale(1.3f); // Scales text to 120% size

			ImGui::Spacing();
			ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);
			ImGui::TextWrapped(

				"Scropts QOL is FREE. If you paid for this DLL, you should seek a refund. "
				"We do not use password-protected files or third-party distributors. "
				"Download only via GitHub to avoid malware and unofficial builds. "

			);

			ImGui::PopTextWrapPos();
			ImGui::Dummy(ImVec2(0, 25)); // Vertical padding

			// 4. Action Buttons
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Dummy(ImVec2(0, 25)); // Vertical padding

			// --- CENTER BUTTONS ---
			float button_width = 130.0f; // Slightly wider for larger text
			float Gaps = ImGui::GetStyle().ItemSpacing.x;
			int num_buttons = 2;

			// Calculate total width of all buttons + the space between them
			float total_buttons_width = (num_buttons * button_width) + Gaps;

			// Move cursor to center
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - total_buttons_width) * 0.5f);

			if (ImGui::Button("Okay##DISCLAIMER", ImVec2(button_width, 40))) {
				bDisclaimerShown = true;
			}

			ImGui::SameLine();

			if (ImGui::Button("GitHub##DISCLAIMER", ImVec2(button_width, 40))) {
				ShellExecuteA(0, "open", "https://github.com/Scroptss/Scropts-QOL/releases", 0, 0, SW_SHOW);
			}

			// Reset font scale so other windows aren't affected
			ImGui::SetWindowFontScale(1.0f);
			ImGui::End();

		}
	}


	if (open && bDisclaimerShown) {

		*(int*)((DWORD64)OFFSET(0x17DF0405)) = 0;

		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		ImDrawList* drawlist = ImGui::GetBackgroundDrawList();
		drawlist->AddRectFilled(ImVec2(0, 0), ImVec2(desktop.right, desktop.bottom), IM_COL32(0, 0, 0, 150));

		ImGui::SetNextWindowSize(ImVec2(650.0f, 350.0f));

		std::string title = std::string(" - Scropts QOL for BO3 - ");

		ImGui::Begin(title.c_str(), &open);

		ImGui::BeginTabBar("##main");

		if (ImGui::BeginTabItem("Account")) {

			auto currentMode = Com_SessionMode_GetMode();
			int maxXP = 1457200;
			int maxParagonXP = 52542000;

			if(currentMode == MODE_CAMPAIGN) {
				maxXP = 581651;
				maxParagonXP = 0;
				minRank = 0;
			}
			else if (currentMode == MODE_MULTIPLAYER) {
				maxXP = 1457200;
				maxParagonXP = 52542000;
				minRank = 56;
			}
			else if (currentMode == MODE_ZOMBIES) {
				maxXP = 1375000;
				maxParagonXP = 52345460;
				minRank = 36;
			}			

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

				setAllRanks();
				//LiveStats_SetStatByKey(Com_SessionMode_GetMode(), CONTROLLER_INDEX_0, MP_PLAYERSTATSKEY_PARAGONICONID, icon);

				LiveStorage_UploadStatsForController(0);
			}

			ImGui::SameLine();

			if (ImGui::Button("Set Stats")) {
				setStats();
				//setChallenges();
				//unlockSpecialistOutfits();
				//setMaxTokens();
				LiveStorage_UploadStatsForController(0);
			}



			// Prestige
			ImGui::Separator();

			ImGui::BulletText("Rank Editor");

			ImGui::SameLine();

			HelpMarker("CTRL + Click to input custom values");

			ImGui::Separator();

			if (ImGui::Button("Send##PRESTIGE")) {
				setPrestige(pPrestige);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Prestige ##RANK", &pPrestige, 0, 11);

			// Level
			if (ImGui::Button("Send##LEVEL")) {
				setpLevel(pLevel);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Level ##RANK", &pLevel, 0, 54);

			// XP max: CP - 581651, MP - 1457200, ZM - 1375000
			if (ImGui::Button("Send##XP")) {
				setpLevelXP(rankXp);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Base XP##RANK", &rankXp, 0, maxXP);

			// Paragon Rank Max: 
			if (ImGui::Button("Send##PRESTIGEMASTERRANK")) {
				setMasterRank(ParagonRank);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Prestige Master Lvl##RANK", &ParagonRank, minRank, 1000);

			// Paragon XP Max: CP - 0, MP - 52542000, ZM - 52345460
			if (ImGui::Button("Send##PRESTIGEMASTERXP")) {
				setMasterXP(paragonRankXp);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Prestige Master XP##RANK", &paragonRankXp, 0, maxParagonXP);

			ImGui::Separator();

			ImGui::BulletText("Stat Editor");

			ImGui::Separator();

			if (ImGui::Button("Send##SCORE")) {
				setStatbyName("score", iScore);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Score##RANK", &iScore, 0, MAXINT / 2);
			

			if (ImGui::Button("Send##KILLS")) {
				setStatbyName("kills", iKills);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Kills##RANK", &iKills, 0, MAXINT / 2);
			
			if (ImGui::Button("Send##DEATHS")) {
				setStatbyName("deaths", iDeaths);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Deaths##RANK", &iDeaths, 0, MAXINT / 2);
			
			if (ImGui::Button("Send##ASSISTS")) {
				setStatbyName("assists", iAssists);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Assists##RANK", &iAssists, 0, MAXINT / 2);

			if (ImGui::Button("Send##HEADSHOTS")) {
				setStatbyName("headshots", iHeadshots);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Headshots##RANK", &iHeadshots, 0, MAXINT / 2);

			if (ImGui::Button("Send##TEAMKILLS")) {
				setStatbyName("TEAMKILLS", iTeamKills);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Teamkills##RANK", &iTeamKills, 0, MAXINT / 2);

			if (ImGui::Button("Send##SUICIDES")) {
				setStatbyName("SUICIDES", iSuicides);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Suicides##RANK", &iSuicides, 0, MAXINT / 2);

			if (ImGui::Button("Send##TPALLIES")) {
				setStatbyName("TIME_PLAYED_ALLIES", iTimePlayedAllies);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Time Played (Allies)##RANK", &iTimePlayedAllies, 0, MAXINT / 2);

			if (ImGui::Button("Send##TPOPFOR")) {
				setStatbyName("TIME_PLAYED_OPFOR", iTimePlayedOpFor);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Time Played (OpFor)##RANK", &iTimePlayedOpFor, 0, MAXINT / 2);

			if (ImGui::Button("Send##TPOTHER")) {
				setStatbyName("TIME_PLAYED_OTHER", iTimePlayedOther);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Time Played (Other)##RANK", &iTimePlayedOther, 0, MAXINT / 2);

			if (ImGui::Button("Send##TPTOTAL")) {
				setStatbyName("TIME_PLAYED_TOTAL", iTimePlayedTotal);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Time Played (Total)##RANK", &iTimePlayedTotal, 0, MAXINT / 2);

			if (ImGui::Button("Send##TOTALGAMESPLAYED")) {
				setStatbyName("total_games_played", iTotalGamesPlayed);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Total Games Played##RANK", &iTotalGamesPlayed, 0, MAXINT / 2);

			if (ImGui::Button("Send##KDRATIO")) {
				setStatbyName("KDRATIO", iKD);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("KD Ratio##RANK", &iKD, 0, MAXINT / 2);

			if (ImGui::Button("Send##WINS")) {
				setStatbyName("WINS", iWins);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Wins##RANK", &iWins, 0, MAXINT / 2);

			if (ImGui::Button("Send##LOSSES")) {
				setStatbyName("LOSSES", iLosses);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Losses##RANK", &iLosses, 0, MAXINT / 2);

			if (ImGui::Button("Send##TIES")) {
				setStatbyName("TIES", iTies);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Ties##RANK", &iTies, 0, MAXINT / 2);

			if (ImGui::Button("Send##CUR_WIN_STERAK")) {
				setStatbyName("CUR_WIN_STREAK", iCurWinStreak);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Win Streak##RANK", &iCurWinStreak, 0, MAXINT / 2);

			if (ImGui::Button("Send##WLRATIO")) {
				setStatbyName("WLRATIO", iWL);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("WL Ratio##RANK", &iWL, 0, MAXINT / 2);

			if (ImGui::Button("Send##HITS")) {
				setStatbyName("HITS", iHits);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Hits##RANK", &iHits, 0, MAXINT / 2);

			if (ImGui::Button("Send##MISSES")) {
				setStatbyName("MISSES", iMisses);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Misses##RANK", &iMisses, 0, MAXINT / 2);

			if (ImGui::Button("Send##TOTALSHOTS")) {
				setStatbyName("TOTAL_SHOTS", iTotalShots);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Total Shots##RANK", &iTotalShots, 0, MAXINT / 2);

			if (ImGui::Button("Send##ACCURACY")) {
				setStatbyName("ACCURACY", iAccuracy);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Accuracy##RANK", &iAccuracy, 0, MAXINT / 2);

			if (ImGui::Button("Send##HIGHESTROUND")) {
				setStatbyName("HIGHEST_ROUND_REACHED", iHighestRound);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Highest Round Reached##RANK", &iHighestRound, 0, MAXINT / 2);

			if (ImGui::Button("Send##TOTALROUNDSSURVIVED")) {
				setStatbyName("TOTAL_ROUNDS_SURVIVED", iTotalRounds);
				LiveStorage_UploadStatsForController(0);
			}
			ImGui::SameLine();
			ImGui::SliderInt("Total Rounds Survived##RANK", &iTotalRounds, 0, MAXINT / 2);

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
				ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "Contracts Unlocked! Join and leave at least 9 public \nmatches to make contract rewards appear in your inventory" });
			}

			if (ImGui::Button("Complete Challenges")) {
				setChallenges("mp","statsmilestones1.csv");
				setChallenges("mp","statsmilestones2.csv");
				setChallenges("mp","statsmilestones3.csv");
				setChallenges("mp","statsmilestones4.csv");
				setChallenges("mp","statsmilestones5.csv");
				setChallenges("mp","statsmilestones6.csv");

				setChallenges("zm","statsmilestones3.csv");

				setChallenges("cp","statsmilestones1.csv");
				setChallenges("cp","statsmilestones3.csv");

				setGameTypeStats();
				LiveStorage_UploadStatsForController(0);
			}

			if (ImGui::Button("Complete Hero Challenges")) {
				unlockSpecialistOutfits();
			}

			if (ImGui::Button("Complete Campaign")) {
				Cbuf_AddText(0, "achievement CP_COMPLETE_PROLOGUE");
				Cbuf_AddText(0, "achievement CP_COMPLETE_NEWWORLD");
				Cbuf_AddText(0, "achievement CP_COMPLETE_BLACKSTATION");
				Cbuf_AddText(0, "achievement CP_COMPLETE_BIODOMES");
				Cbuf_AddText(0, "achievement CP_COMPLETE_SGEN");
				Cbuf_AddText(0, "achievement CP_COMPLETE_VENGEANCE");
				Cbuf_AddText(0, "achievement CP_COMPLETE_RAMSES");
				Cbuf_AddText(0, "achievement CP_COMPLETE_INFECTION");
				Cbuf_AddText(0, "achievement CP_COMPLETE_AQUIFER");
				Cbuf_AddText(0, "achievement CP_COMPLETE_LOTUS");
				Cbuf_AddText(0, "achievement CP_HARD_COMPLETE");
				Cbuf_AddText(0, "achievement CP_REALISTIC_COMPLETE");
				Cbuf_AddText(0, "achievement CP_CAMPAIGN_COMPLETE");
				CompleteAllMissions();
			}

			if (ImGui::Button("Max Collectibles")) {
				Cbuf_AddText(0, "achievement CP_ALL_COLLECTIBLES");
				Cbuf_AddText(0, "achievement CP_MISSION_COLLECTIBLES");
				UnlockAllCollectibles();
			}

			if (ImGui::Button("Max Accolades")) {
				Cbuf_AddText(0, "achievement CP_ALL_COLLECTIBLES");
				UnlockAllAccolades();
			}

			if (ImGui::Button("Max Medals")) {
				Cbuf_AddText(0, "achievement CP_ALL_DECORATIONS");
				UnlockAllMedals();
			}

			if (ImGui::Button("Unlock All Achievements")) {
				UnlockAllAchievements();
			}



			ImGui::TextDisabled("Suggest more unlocks");

			ImGui::EndChild();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Zombies")) {

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

			ImGui::Separator();

			ImGui::Dummy(ImVec2(0, 5));

			
			if (ImGui::Button("Fast Restart Map")) {
				Cbuf_AddText(0, "fast_restart");
			}

			ImGui::SameLine(0.0f,3.0f);

			if (ImGui::Button("Full Restart Map")) {
				Cbuf_AddText(0, "map_restart");
			}

			ImGui::SameLine(0.0f,3.0f);
		

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

			ImGui::SameLine(0.0f,3.0f);

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

			ImGui::Checkbox("Thorns Mode##DMG", &bThorns);
			ImGui::SameLine(0.0f, 3.0f);
			ImGui::Checkbox("Nukes Mode##DMG", &bNukes);
			//ImGui::SameLine();
			ImGui::Checkbox("##DMG", &bDamageMultiplier);

			ImGui::SameLine();
			ImGui::SliderInt("DMG Multiplier", &iDamageMultiplier, 1, 10000);

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


			ImGui::EndTabItem();

		}

		if (ImGui::BeginTabItem("Multiplayer")) {

			ImGui::Dummy(ImVec2(0, 5));

			if (ImGui::Button("Launch MP Game")) {
				Dvar_SetFromString("party_minPlayers", "1", true);
				Cbuf_AddText(0, "LaunchGame");
			}

			ImGui::SameLine();

			if (ImGui::Button("Make Custom Game Earn XP")) {

				if (hooks::AreWeInGameAndHosting()) {

					__int64 sSessionModeState = ProcessBase + 0x1686E874;
					*(__int32*)sSessionModeState = 129;
					ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "XP Earned in this match will stick." });
				}
				else {
					ImGui::InsertNotification({ ImGuiToastType::Error, 5000, "You must be hosting a custom game!" });
				}

			}

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Make sure you enable this AFTER the 10 second countdown!");
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
			ImGui::BulletText("Showcase Weapon: %s",ShowcaseWeaponName);

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

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("BlackMarket")) {

			ImGui::Dummy(ImVec2(0, 5));

			ImGui::SliderInt("Loot Speed (ms)", &iLootSpeed, 5, 500);
			ImGui::SliderInt("Crypto Amount ##CRYPTOAMT", &iCryptoAmt, 0, 4950);

			ImGui::Checkbox("Loop Cryptokeys", &bCrypto);

			ImGui::SameLine(0.0f, 5.0f);

			ImGui::Checkbox("Spend Cryptokeys", &bCryptoSpend);

			ImGui::SameLine(0.0f, 5.0f);

			ImGui::Button("Fix Cryptokeys"); {
				resetCrypto();
			}

			ImGui::Checkbox("Loop Divinium", &bDivinium);

			ImGui::SameLine(0.0f, 5.0f);

			ImGui::Checkbox("Spend Divinium", &bDiviniumSpend);

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


			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Spoof")) {

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


			ImGui::EndTabItem();

		}

		if (ImGui::BeginTabItem("Class Editor")) {

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


			ImGui::EndTabItem();
		}
		
		if (ImGui::BeginTabItem("Weapon Stats"))
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

			ImGui::SliderInt("Purchased##WEP", &g_WeaponStats.purchased, 0, 3);
			ImGui::SliderInt("XP##WEP", &g_WeaponStats.xp, 0, 665535);
			ImGui::SliderInt("Kills##WEP", &g_WeaponStats.kills, 0, 10000000);
			ImGui::SliderInt("Level##WEP", &g_WeaponStats.plevel, 0, 15);
			ImGui::SliderInt("Hits##WEP", &g_WeaponStats.hits, 0, 10000000);
			ImGui::SliderInt("Misses##WEP", &g_WeaponStats.misses, 0, 10000000);
			ImGui::SliderInt("Accuracy##WEP", &g_WeaponStats.accuracy, 0, 100);
			ImGui::SliderInt("Headshots##WEP", &g_WeaponStats.headshots, 0, 10000000);
			ImGui::SliderInt("Uses##WEP", &g_WeaponStats.used, 0, 10000000);
			ImGui::SliderInt("Assists##WEP", &g_WeaponStats.assists, 0, 100000);
			ImGui::SliderInt("Assist Score##WEP", &g_WeaponStats.assist_score, 0, 100000);
			ImGui::SliderInt("Challenges_tu##WEP", &g_WeaponStats.challenges_tu, 0, 100000);
			ImGui::SliderInt("Destroyed Aircraft##WEP", &g_WeaponStats.destroyed_aircraft, 0, 100000);
			ImGui::SliderInt("Destroyed Aircraft Under 20s##WEP", &g_WeaponStats.destroyed_aircraft_under20s, 0, 100000);
			ImGui::SliderInt("Destroy 2 Killstreaks Rapidly", &g_WeaponStats.destroy_2_killstreaks_rapidly, 0, 100000);
			ImGui::SliderInt("Destroy 5 Killstreak Vehicle", &g_WeaponStats.destroy_5_killstreak_vehicle, 0, 100000);
			ImGui::SliderInt("Destroy AiTank or Setinel", &g_WeaponStats.destroy_aitank_or_setinel, 0, 100000);
			ImGui::SliderInt("Destroy Turret", &g_WeaponStats.destroy_turret, 0, 100000);
			ImGui::SliderInt("Direct Hit Kills", &g_WeaponStats.direct_hit_kills, 0, 100000);
			ImGui::SliderInt("Bloodthirsty", &g_WeaponStats.killstreak_5, 0, 100000);
			ImGui::SliderInt("Kills While Active", &g_WeaponStats.kills_while_active, 0, 100000);
			ImGui::SliderInt("One Shots Shotgun", &g_WeaponStats.kill_enemy_one_bullet_shotgun, 0, 100000);
			ImGui::SliderInt("One Shots Snipers", &g_WeaponStats.kill_enemy_one_bullet_sniper, 0, 100000);
			ImGui::SliderInt("Kills While Injured", &g_WeaponStats.kill_enemy_when_injured, 0, 100000);
			ImGui::SliderInt("Kill Enemy W Their Weapon", &g_WeaponStats.kill_enemy_with_their_weapon, 0, 100000);
			ImGui::SliderInt("Loaded Kills", &g_WeaponStats.loadedKills, 0, 100000);
			ImGui::SliderInt("Longshots##WEP", &g_WeaponStats.longshot_kill, 0, 100000);
			ImGui::SliderInt("No Attachment Kills", &g_WeaponStats.noAttKills, 0, 100000);
			ImGui::SliderInt("No Perk Kills", &g_WeaponStats.noPerkKills, 0, 100000);
			ImGui::SliderInt("Revenge Kills##WEP", &g_WeaponStats.revenge_kill, 0, 100000);
			ImGui::SliderInt("Blackhat Hacks##WEP", &g_WeaponStats.hacks, 0, 100000);
			ImGui::SliderInt("EMP Destroys##WEP", &g_WeaponStats.destroyed, 0, 100000);
			ImGui::SliderInt("Stun Kills##WEP", &g_WeaponStats.stunkills, 0, 100000);
			ImGui::SliderInt("Blind Kills##WEP", &g_WeaponStats.blindkills, 0, 100000);
			ImGui::SliderInt("Captures with Smoke##WEP", &g_WeaponStats.capture, 0, 100000);
			ImGui::SliderInt("Trophy System Defends##WEP", &g_WeaponStats.projectiles, 0, 100000);

			ImGui::EndChild();

			ImGui::EndTabItem();

		}

		if (ImGui::BeginTabItem("Tools")) {

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


			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Info")) {
					
			ImGui::Dummy(ImVec2(0, 5));
			ImGui::InputTextWithHint("##TITLE", "Screenshot Title", &custom_title_buf);
			ImGui::InputTextWithHint("##DESC", "Screenshot Description", &custom_desc_buf);
			
			ImGui::Dummy(ImVec2(10, 10));
			ImGui::Text("Version: %s", SCROPTS_VERSION);

			/*auto mode = Com_SessionMode_GetMode();
			auto root = LiveStats_Core_GetRootDDLState(mode);
			auto ctx = GetStatsBuffer(0);


			ImGui::InputTextWithHint("##SPB", "StatPat int", &statPathBuffer);

			if (ImGui::Button("Get DDL Int")) {
				std::vector<std::string> parts = split(statPathBuffer, '/');

				GetValue(parts);
			}*/

			//ImGui::SameLine();
			//ImGui::Text("Int Result: %d", lastIntResult);

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
					LiveStats_SetClassSetname(0, a3, classSetIndex, classSetName.c_str());
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

				ImGui::SameLine(150,0);
				ImGui::SetNextItemWidth(120.0f);
				ImGui::Combo("Combo Box Type", &iComboBoxType, "Weapons\000Attachments\000Camos\000Reticles\000Perks\000WildCards\000KillStreaks\000Extra\000");


				auto limit = getComboItemSize(false);

				if (ImGui::BeginChild("##PRIMARY", ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2))) {

					auto primaryLimit = getComboItemSize(true);

					ImGui::BulletText("Primaries");

					ImGui::Separator();

					ImGui::BeginChild("##PRIMARYLIST", ImGui::GetContentRegionAvail());

					DrawSlotTable(primarySlotsTable, 10, classSetIndex, classIndex);

					// Primary Weapon and attachment / camo editor

					ImGui::EndChild();
					ImGui::EndChild();
				}

				ImGui::SameLine();

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

				ImGui::SameLine();

				if (ImGui::BeginChild("##SECONDARY", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2))) {

					ImGui::BulletText("Secondaries");

					ImGui::Separator();

					ImGui::BeginChild("##SECONDARYLIST", ImGui::GetContentRegionAvail());

					DrawSlotTable(secondarySlotsTable, 10, classSetIndex, classIndex);
					// Secondary Weapon and attachment / camo editor

					ImGui::EndChild();
					ImGui::EndChild();
				}

				ImGui::Separator();

				if (ImGui::BeginChild("##OFFHANDS", ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y))) {

					ImGui::BulletText("Offhands");

					ImGui::Separator();

					ImGui::BeginChild("##OFFHANDLIST", ImGui::GetContentRegionAvail());

					DrawSlotTable(offhandSlotsTable, 8, classSetIndex, classIndex);
					// Wildcard and maybe Streak Editor?

					ImGui::EndChild();
					ImGui::EndChild();
				}

				ImGui::SameLine();

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

				ImGui::SameLine();

				if (ImGui::BeginChild("##WILDCARDS", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y))) {

					ImGui::BulletText("Perks & Wildcards");

					ImGui::Separator();

					ImGui::BeginChild("##WILDCARDLIST", ImGui::GetContentRegionAvail());

					DrawSlotTable(wildcardSlotsTable, 9, classSetIndex, classIndex);

					// Lethals, Tacticals

					ImGui::EndChild();
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

bool init = false;

void MergeIconsWithLatestFont(float font_size, bool FontDataOwnedByAtlas = false)
{
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.FontDataOwnedByAtlas = FontDataOwnedByAtlas;

	ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)fa_solid_900, sizeof(fa_solid_900), font_size, &icons_config, icons_ranges);
}

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
			MergeIconsWithLatestFont(16.0f, false);
			init = true;
		}

		else
			return decltype(&hkPresent)(oPresent)(pSwapChain, SyncInterval, Flags);
	}

	if (GetAsyncKeyState(OpenKeybind) & 1)
	{
		open = !open;
	}

	hooks::onFrame();

	if (bDivinium) {
		if (clock() - UnlockTMR > 5) {
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
		if (bDivinium) {
			bDivinium = false;
		}

		Loot_SpendVials(0, 3);

	}

	Dvar_SetFromString("loot_loginReward_active", "1", 1);

	ImGui::GetIO().MouseDrawCursor = open;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	draw();
	drawTracers();

	if (bNotifications) {
		ImGui::RenderNotifications();
	}

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return decltype(&hkPresent)(oPresent)(pSwapChain, SyncInterval, Flags);
}

HookReturn initDirectXPresentPointerSwap() {
	__int64 DirectXVtableThing = **(__int64**)(ProcessBase + 0xF4378D8); // 48 89 84 24 ? ? ? ? 48 8B 0D ? ? ? ? 0x1c40000
	oPresent = (void*)(*(__int64*)(DirectXVtableThing + 0x40)); // E0 0E 41 89 FF 7F 00 00
	DWORD old;
	VirtualProtect((LPVOID)(DirectXVtableThing + 0x40), 8, PAGE_EXECUTE_READWRITE, &old);
	*(__int64*)(DirectXVtableThing + 0x40) = (__int64)hkPresent;
	VirtualProtect((LPVOID)(DirectXVtableThing + 0x40), 8, old, &old);

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
* - 
* - Clean up the menu, improve GUI code in general, add more tooltips and notifications, etc. Maybe phantom UI?
* - Check for t7Patch.dll, if so dont init protections / arxan since its all taken care of already.
* - Disclaimer
*/