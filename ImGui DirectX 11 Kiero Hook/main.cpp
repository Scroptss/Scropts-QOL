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

bool bNotifications = true;
bool bForceHost;
bool bForceHostRan;
bool bFirstGumRan;
bool open = true;
bool bClassEditor;
bool bDivinium;
bool bLoginReward;
bool bDiviniumSpend;
bool bCrypto;
bool bCryptoSpend;
bool bProtectStats;
bool bProtectStatsRan;
bool bCompleteEE;
int UnlockTMR = clock();
std::string version = "2.3.0";

int minRank = 0;
static int icon = 0;
static int rankXp = 0;
static int pLevel = 0;
static int pPrestige = 0;
static int ParagonRank = 36;
static int paragonRankXp = 0;

static int map = 0;
static int setRound = 0;
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

bool is_user_in_lobby()
{
	return (!Live_IsUserInGame(0) && Live_IsUserSignedInToDemonware(CONTROLLER_INDEX_0));
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
				LiveInventory_GetPlayerInventory(CONTROLLER_INDEX_FIRST)->lootItem[i].quantity = 999;
			}
		}

		else if (i > 2818)
		{
			auto item_id = 0;
			if (get_column_value_for_row_from_path("gamedata/loot/zmlootitems.csv", i - 2818, 1, &item_id))
			{
				LiveInventory_GetPlayerInventory(CONTROLLER_INDEX_FIRST)->lootItem[i].itemId = item_id;
				LiveInventory_GetPlayerInventory(CONTROLLER_INDEX_FIRST)->lootItem[i].quantity = 999;
			}
		}

		else
		{
			auto item_id = 0;
			if (get_column_value_for_row_from_path("gamedata/loot/mplootitems.csv", i, 1, &item_id))
			{
				LiveInventory_GetPlayerInventory(CONTROLLER_INDEX_FIRST)->lootItem[i].itemId = item_id;
				LiveInventory_GetPlayerInventory(CONTROLLER_INDEX_FIRST)->lootItem[i].quantity = 999;
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
}

const char* getComboItemName(int index, bool isPrimary) {

	std::string baseName;
	std::string displayName;	

	switch (iComboBoxType) {
	
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

		__int64 RootState = LiveStats_GetRootDDLState(Com_SessionMode_GetMode());
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
		__int64 RootState = LiveStats_GetRootDDLState(Com_SessionMode_GetMode());
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

	__int64 RootState = LiveStats_GetRootDDLState(Com_SessionMode_GetMode());
	const char* path[8]{};
	__int64 DDLContext = GetStatsBuffer(0);
	char toState[2000]{};

	path[0] = "unlocks";
	path[1] = "0";

	if (DDL_MoveToPath(RootState, toState, 2, path)) {
		DDL_SetUInt((__int64)toState, DDLContext, 255);
	}

}


// Campaign 

void UnlockAllAccolades() {

	using namespace std;

	auto tmp = LiveStats_GetRootDDLState(GetSessionState());
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
		}
	}
	LiveStorage_UploadStatsForController(0);

}

void UnlockAllCollectibles() {
	using namespace std;

	auto tmp = LiveStats_GetRootDDLState(GetSessionState());
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

	auto tmp = LiveStats_GetRootDDLState(GetSessionState());
	const char* path[8];
	auto a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsByMap";
	path[2] = "hasBeenCompleted";
	char result[2000];

	for (int i = 0; i < 11; i++) {
		path[1] = cpmapnames[i];
		DDL_MoveToPath(tmp, result, 3, path);
		DDL_SetUInt((__int64)result, a1, 1);
		ZeroMemory(result, size(result));
	}
	LiveStorage_UploadStatsForController(0);

}


// MP

void setGroupStats() {

	const char* path[8]{};
	const char* statPath[8]{};
	auto mode = Com_SessionMode_GetMode();
	__int64 RootState = LiveStats_GetRootDDLState(Com_SessionMode_GetMode());
	__int64 DDLContext = GetStatsBuffer(0);
	char toState[2000]{};

	path[0] = "groupstats";
	path[1] = "weapon_assault";

	path[2] = "stats";
	
	path[3] = "challenges";
	path[4] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 49);
	ZeroMemory(toState, sizeof(toState));
	path[3] = "kills";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 1000);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "weapon_cqb";
	path[3] = "challenges";
	path[4] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 28);
	ZeroMemory(toState, sizeof(toState));
	path[3] = "kills";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 1000);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "weapon_lmg";
	path[3] = "challenges";
	path[4] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 28);
	ZeroMemory(toState, sizeof(toState));
	path[3] = "kills";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 1000);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "weapon_pistol";
	path[3] = "challenges";
	path[4] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 21);
	ZeroMemory(toState, sizeof(toState));
	path[3] = "kills";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 1000);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "weapon_smg";
	path[3] = "challenges";
	path[4] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 42);
	ZeroMemory(toState, sizeof(toState));
	path[3] = "kills";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 1000);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "weapon_sniper";
	path[3] = "challenges";
	path[4] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 28);
	ZeroMemory(toState, sizeof(toState));
	path[3] = "kills";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 1000);
	ZeroMemory(toState, sizeof(toState));


	path[1] = "weapon_knife";
	path[3] = "challenges";
	path[4] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 7);
	ZeroMemory(toState, sizeof(toState));
	path[1] = "weapon_launcher";
	path[3] = "challenges";
	path[4] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 14);
	ZeroMemory(toState, sizeof(toState));

}

void setMaxWeapons() {

	const char* path[8]{};
	const char* statPath[8]{};
	auto mode = Com_SessionMode_GetMode();
	__int64 RootState = LiveStats_GetRootDDLState(Com_SessionMode_GetMode());
	__int64 DDLContext = GetStatsBuffer(0);
	char toState[2000]{};

	for (int i = 0; i < 256; i++) {

		ZeroMemory(path, sizeof(path));
		

		path[0] = "itemstats";
		path[1] = intToConstCharPtr(i);

		path[2] = "purchased";
		DDL_MoveToPath(RootState, toState, 3, path);
		DDL_SetUInt((__int64)toState, DDLContext, 3);
		ZeroMemory(toState, sizeof(toState));

		path[2] = "xp";

		if (DDL_MoveToPath(RootState, toState, 3, path)) {
			DDL_SetUInt((__int64)toState, DDLContext, 665535);
		}

		path[2] = "plevel";

		if (DDL_MoveToPath(RootState, toState, 3, path)) {
			DDL_SetUInt((__int64)toState, DDLContext, 15);
		}

		path[2] = "isproversionunlocked";

		for (int j = 0; j < 3; j++) {

			if (DDL_MoveToPath(RootState, toState, 3, path)) {
				DDL_SetUInt((__int64)toState, DDLContext, 3);
			}

		}

		path[2] = "stats";

		path[3] = "headshots";
		path[4] = "statValue";
		DDL_MoveToPath(RootState, toState, 5, path);
		DDL_SetUInt((__int64)toState, DDLContext, 7195000);
		ZeroMemory(toState, sizeof(toState));

		path[4] = "challengeValue";
		DDL_MoveToPath(RootState, toState, 5, path);
		DDL_SetUInt((__int64)toState, DDLContext, 75000);
		ZeroMemory(toState, sizeof(toState));

		path[3] = "assists";
		path[4] = "statValue";
		DDL_MoveToPath(RootState, toState, 5, path);
		DDL_SetUInt((__int64)toState, DDLContext, 76000);
		ZeroMemory(toState, sizeof(toState));

		path[4] = "challengevalue";
		DDL_MoveToPath(RootState, toState, 5, path);
		DDL_SetUInt((__int64)toState, DDLContext, 75000);
		ZeroMemory(toState, sizeof(toState));

		path[3] = "kills";
		path[4] = "statvalue";
		DDL_MoveToPath(RootState, toState, 5, path);
		DDL_SetUInt((__int64)toState, DDLContext, 76000);
		ZeroMemory(toState, sizeof(toState));

		path[4] = "challengevalue";
		DDL_MoveToPath(RootState, toState, 5, path);
		DDL_SetUInt((__int64)toState, DDLContext, 75000);
		ZeroMemory(toState, sizeof(toState));

		path[3] = "challenges";
		path[4] = "statvalue";
		DDL_MoveToPath(RootState, toState, 5, path);
		DDL_SetUInt((__int64)toState, DDLContext, 6);
		ZeroMemory(toState, sizeof(toState));

		path[4] = "challengevalue";
		DDL_MoveToPath(RootState, toState, 5, path);
		DDL_SetUInt((__int64)toState, DDLContext, 6);
		ZeroMemory(toState, sizeof(toState));

		path[3] = "backstabber_kill";
		path[4] = "statvalue";
		DDL_MoveToPath(RootState, toState, 5, path);
		DDL_SetUInt((__int64)toState, DDLContext, 100);
		ZeroMemory(toState, sizeof(toState));

		path[4] = "challengevalue";
		DDL_MoveToPath(RootState, toState, 5, path);
		DDL_SetUInt((__int64)toState, DDLContext, 10);
		ZeroMemory(toState, sizeof(toState));


		for (int j = 0; j < 8; j++)
		{
			char buf[255];
			sprintf_s(buf, "challenge%d", j);
			path[3] = buf;
			DDL_MoveToPath(RootState, toState, 5, path);
			DDL_SetUInt((__int64)toState, DDLContext, 50000);
			ZeroMemory(toState, sizeof(toState));

			path[4] = "challengeValue";
			DDL_MoveToPath(RootState, toState, 5, path);
			DDL_SetUInt((__int64)toState, DDLContext, 50000);
			ZeroMemory(toState, sizeof(toState));
		}
	}

	path[0] = "playerstatslist";
	path[1] = "weapons_mastery";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 6);
	ZeroMemory(toState, sizeof(toState));

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 6);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "secondary_mastery";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "secondary_mastery_launcher";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "secondary_mastery_pistol";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "tscc_challenge_mastery";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 2);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 2);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "weapons_mastery_assault";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	
	path[1] = "weapons_mastery_cqb";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	
	path[1] = "weapons_mastery_lmg";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	
	path[1] = "weapons_mastery_smg";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	
	path[1] = "weapons_mastery_sniper";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	 // DLC Guns

	path[1] = "ar_famas_for_diamond";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "ar_famas_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "ar_garand_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "ar_garand_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "ar_peacekeeper_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "ar_peacekeeper_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "ar_pulse_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "ar_pulse_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "bowie_knife_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "bowie_knife_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "launcher_multi_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "launcher_multi_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "lmg_infinite_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "lmg_infinite_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_bat_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_bat_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_boneglass_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_boneglass_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_boxing_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_boxing_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_butterfly_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_butterfly_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_chainsaw_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_chainsaw_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_crescent_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_crescent_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_crowbar_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_crowbar_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_dagger_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_dagger_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_fireaxe_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_fireaxe_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_improvise_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_improvise_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_katana_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_katana_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_knuckles_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_knuckles_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_mace_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_mace_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_nunchuks_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_nunchuks_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_prosthetic_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_prosthetic_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_shockbaton_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_shockbaton_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_shovel_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_shovel_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_sword_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_sword_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_wrench_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "melee_wrench_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "pistol_energy_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "pistol_energy_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "pistol_shotgun_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "pistol_shotgun_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "shotgun_energy_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "shotgun_energy_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "smg_mp40_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "smg_mp40_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "smg_nailgun_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "smg_nailgun_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "smg_rechamber_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "smg_rechamber_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "sniper_double_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "sniper_double_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "sniper_quickscope_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "sniper_quickscope_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "special_crossbow_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "special_crossbow_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "special_discgun_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "special_discgun_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_01_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_01_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_02_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_02_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_03_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_03_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_04_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_04_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_05_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_05_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_06_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_06_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_07_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_07_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_08_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_08_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_09_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_09_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_10_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_10_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_11_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_11_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_12_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_12_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_13_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_13_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 5, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_14_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_14_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_15_for_diamond";

	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));

	path[1] = "z_weap_15_gold";
	path[2] = "statValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));
	path[2] = "challengeValue";
	DDL_MoveToPath(RootState, toState, 3, path);
	DDL_SetUInt((__int64)toState, DDLContext, 3);
	ZeroMemory(toState, sizeof(toState));




	LiveStorage_UploadStatsForController(0);

	

}

void unlockSpecialistOutfits() {
	auto tmp = LiveStats_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	char result[2000];

	for (int l = 0; l < 12; l++)
	{
		std::ostringstream temp;
		temp << l;
		std::string converted(temp.str());

		path[0] = "specialiststats";
		path[1] = converted.c_str();
		path[2] = "stats";
		const char* p3[19] = { "kills","kills_ability", "kills_weapon", "multikill_ability", "multikill_weapon", "kill_one_game_ability", "kill_one_game_weapon", "challenge1", "challenge2", "challenge3", "challenge4", "challenge5", "challenge6", "challenge7", "challenge8", "challenge9", "challenge10", "challenge11", "challenge12" };

		for (int k = 0; k < 19; k++)
		{
			path[3] = p3[k];
			path[4] = "statValue";
			DDL_MoveToPath(tmp, result, 3, path);
			DDL_SetUInt((__int64)result, a1, 1010050000);
			ZeroMemory(result, sizeof(result));
			path[4] = "challengeValue";
			DDL_MoveToPath(tmp, result, 3, path);
			DDL_SetUInt((__int64)result, a1, 1010050000);
			ZeroMemory(result, sizeof(result));
		}
		LiveStorage_UploadStatsForController(0);
	}
}

void setPrestige(int rank) {
	auto tmp = LiveStats_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	char result[2000];

	path[1] = "plevel";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, rank);
	ZeroMemory(result, sizeof(result));
}

void setpLevel(int rank) {
	auto tmp = LiveStats_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	char result[2000];

	path[1] = "rank";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, rank);
	ZeroMemory(result, sizeof(result));
}

void setpLevelXP(int rank) {
	auto tmp = LiveStats_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	char result[2000];

	path[1] = "rankxp";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, rank);
	ZeroMemory(result, sizeof(result));
}

void setMasterRank(int rank) {
	auto tmp = LiveStats_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	char result[2000];

	path[1] = "paragon_rank";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, rank - minRank);
	ZeroMemory(result, sizeof(result));
}

void setMasterXP(int rank) {
	auto tmp = LiveStats_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	char result[2000];

	path[1] = "paragon_rankxp";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, rank);
	ZeroMemory(result, sizeof(result));
}

void setAllRanks()
{
	auto tmp = LiveStats_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
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

	auto tmp = LiveStats_GetRootDDLState(GetSessionState());
	const char* path[8];
	__int64 a1 = GetStatsBuffer(0);
	path[0] = "PlayerStatsList";
	path[2] = "statValue";
	char result[2000];
	int amount = 500000;

	amount = rand() % (10000000 - 8000000 + 1) + 8000000;
	path[2] = "statValue";
	path[1] = "total_games_played";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));

	/*amount = rand() % (500 - 100) + 500;
	path[2] = "statValue";
	path[1] = "highest_round_reached";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));*/

	amount = rand() % (10000000 - 8000000 + 1) + 8000000;
	path[2] = "statValue";
	path[1] = "time_played_total";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));

	amount = rand() % (30 - 10) + 30;
	path[2] = "statValue";
	path[1] = "cur_win_streak";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));

	amount = rand() % (10000000 - 8000000 + 1) + 8000000;

	path[2] = "statValue";
	path[1] = "score";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));

	amount = rand() % (10000000 - 8000000 + 1) + 8000000;

	path[2] = "statValue";
	path[1] = "kills";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));

	amount = rand() % (10000000 - 8000000 + 1) + 8000000;

	path[2] = "statValue";
	path[1] = "deaths";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));

	amount = rand() % (10000000 - 8000000 + 1) + 8000000;

	path[2] = "statValue";
	path[1] = "headshots";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));

	amount = rand() % (10000000 - 8000000 + 1) + 8000000;

	path[2] = "statValue";
	path[1] = "wlratio";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, amount);
	ZeroMemory(result, sizeof(result));

	amount = rand() % (10000000 - 8000000 + 1) + 8000000;

	path[2] = "statValue";
	path[1] = "kdratio";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, 5);
	ZeroMemory(result, sizeof(result));
	path[2] = "challengeValue";
	DDL_MoveToPath(tmp, result, 3, path);
	DDL_SetUInt((__int64)result, a1, 5);
	ZeroMemory(result, sizeof(result));

	path[2] = "statValue";
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

void setChallenges() {
	auto tmp = LiveStats_GetRootDDLState(GetSessionState());
	auto a1 = GetStatsBuffer(0);
	const char* path[8];
	const char* mp[95] = { "kill_with_pickup","kill_while_wallrunning", "kill_while_in_air", "kill_while_sliding","kill_while_mantling","kill_enemy_thats_wallrunning","kill_enemy_that_in_air","kill_while_underwater","kill_after_doublejump_out_of_water","kill_while_sliding_from_doublejump","kill_while_wallrunning_2_walls","destroy_equipment_with_bullet", "darkops_zod_ee", "darkops_factory_ee", "darkops_castle_ee", "darkops_island_ee", "darkops_stalingrad_ee", "darkops_genesis_ee", "darkops_zod_super_ee", "darkops_factory_super_ee", "darkops_castle_super_ee", "darkops_island_super_ee", "darkops_stalingrad_super_ee", "darkops_genesis_super_ee", "bouncingbetty_planted","bouncingbetty_pickedup", "bouncingbetty_devil_planted", "bouncingbetty_devil_pickedup","bouncingbetty_holly_planted","bouncingbetty_holly_pickedup","ballistic_knives_pickedup","wallbuy_weapons_purchased","zdogs_killed","zraps_killed", "zwasp_killed", "zspiders_killed", "zthrashers_killed", "zsentinel_killed", "zraz_killed", "zdog_rounds_finished", "specialty_armorvest_drank","specialty_quickrevive_drank", "specialty_fastreload_drank", "specialty_additionalprimaryweapon_drank","specialty_staminup_drank","specialty_doubletap2_drank","specialty_widowswine_drank","specialty_deadshot_drank","specialty_electriccherry_drank","specialty_electriccherry_drank", "zombie_hunter_kill_headshot","zombie_hunter_kill_melee", "zombie_hunter_kill_crawler", "zombie_hunter_kill_packapunch","zombie_hunter_kill_trap","zombie_hunter_kill_explosives","zombie_hunter_explosion_multikill","zombie_hunter_blast_furnace","zombie_hunter_dead_wire","zombie_hunter_fire_works","zombie_hunter_thunder_wall","zombie_hunter_turned","zombie_hunter_mastery", "survivalist_buy_magic_box", "survivalist_buy_perk", "survivalist_buy_wallbuy", "survivalist_buy_door", "survivalist_revive", "survivalist_survive_rounds", "survivalist_craftable", "survivalist_board", "survivalist_powerup", "survivalist_mastery", "darkops_zod_ee", "darkops_factory_ee", "darkops_castle_ee", "darkops_island_ee", "darkops_stalingrad_ee", "darkops_genesis_ee", "darkops_zod_super_ee", "darkops_factory_super_ee", "darkops_castle_super_ee", "darkops_island_super_ee", "darkops_stalingrad_super_ee", "darkops_genesis_super_ee", "gum_gobbler_consume", "gum_gobbler_powerups", "gum_gobbler_alchemical_antithesis", "gum_gobbler_anywhere_but_here", "gum_gobbler_burned_out", "gum_gobbler_ephemeral_enhancement", "gum_gobbler_phoenix_up", "gum_gobbler_sword_flay", "gum_gobbler_wall_power", "gum_gobbler_mastery" };
	char result[2000];
	for (int i = 0; i < 95; i++) {
		path[0] = "playerstatslist";
		path[1] = mp[i];
		path[2] = "statValue";
		DDL_MoveToPath(tmp, result, 3, path);
		DDL_SetUInt((__int64)result, a1, 50000);
		path[2] = "challengeValue";
		DDL_MoveToPath(tmp, result, 3, path);
		DDL_SetUInt((__int64)result, a1, 50000);
	}
	LiveStorage_UploadStatsForController(0);
}

void unlockContracts(int index, int max, int type) {

	StringTable* table = nullptr;
	StringTable_GetAsset("gamedata/tables/mp/mp_contracttable.csv", &table);

	while (index < max) {

		__int64 RootState = LiveStats_GetRootDDLState(Com_SessionMode_GetMode());
		const char* path[8]{};
		const char* statPath[8]{};
		__int64 DDLContext = GetStatsBuffer(0);
		char toState[2000]{};

		auto value = intToConstCharPtr(index);
		auto ContractID = StringTable_Lookup(table, 0i64, value, 0);
		auto ChallengeValue = StringTable_Lookup(table, 0i64, value, 2);
		auto ChallengeName = StringTable_Lookup(table, 0i64, value, 7);
		auto ChallengeAlias = StringTable_Lookup(table, 0i64, value, 8);
		auto ExtraBytes = StringTable_Lookup(table, 0i64, value, 9);

		path[0] = "contracts";
		path[1] = intToConstCharPtr(type);

		path[2] = "active";

		if (DDL_MoveToPath(RootState, toState, 3, path)) {
			DDL_SetUInt((__int64)toState, DDLContext, 3);
			ZeroMemory(toState, sizeof(toState));
		}

		path[2] = "index";

		if (DDL_MoveToPath(RootState, toState, 3, path)) {
			DDL_SetUInt((__int64)toState, DDLContext, index);
			ZeroMemory(toState, sizeof(toState));
		}

		path[2] = "progress";

		if (DDL_MoveToPath(RootState, toState, 3, path)) {
			DDL_SetUInt((__int64)toState, DDLContext, std::stoi(ChallengeValue));
			ZeroMemory(toState, sizeof(toState));
		}

		path[2] = "award_given";

		if (DDL_MoveToPath(RootState, toState, 3, path)) {
			DDL_SetUInt((__int64)toState, DDLContext, 3);
			ZeroMemory(toState, sizeof(toState));
		}

		if (ChallengeName != nullptr && ChallengeName[0] != '\0') {

			statPath[0] = "playerStatsList";
			statPath[1] = ChallengeName;
			statPath[2] = "statValue";

			if (DDL_MoveToPath(RootState, toState, 3, statPath)) {
				DDL_SetUInt((__int64)toState, DDLContext, 3);
				ZeroMemory(toState, sizeof(toState));
			}

		}

		if (ChallengeAlias != nullptr && ChallengeAlias[0] != '\0') {

			statPath[0] = "playerStatsList";
			statPath[1] = ChallengeAlias;
			statPath[2] = "statValue";

			if (DDL_MoveToPath(RootState, toState, 3, statPath)) {
				DDL_SetUInt((__int64)toState, DDLContext, 3);
				ZeroMemory(toState, sizeof(toState));
			}

		}

		if (ExtraBytes != nullptr && ExtraBytes[0] != '\0') {

			const char* sepPath[8]{};

			// Split the string into sep parts
			auto sep = utils::splitString(ExtraBytes);

			for (int i = 0; i < sep.size(); i++) {
				sepPath[i] = sep[i].data();
			}

			if (DDL_MoveToPath(RootState, toState, 2, sepPath)) {
				DDL_SetUInt((__int64)toState, DDLContext, 3);
				ZeroMemory(toState, sizeof(toState));
			}

		}

		index++;
	}

}


// ZM

std::vector<std::string> tokenize(const std::string& text, char delimiter)
{
	std::vector<std::string> buf;
	std::stringstream ss(text);
	std::string item;

	while (std::getline(ss, item, delimiter))
	{
		buf.push_back(item);
	}

	return buf;
}

const char** vector_to_pointer_array(const std::vector<std::string>& strings)
{
	std::vector<const char*> buf;
	buf.clear();

	for (auto& str : strings)
	{
		buf.push_back(str.c_str());
	}

	return buf.data();
}

void ddl_set_for_path(__int64 state, __int64 context, std::string path, std::uint32_t value)
{
	auto ddl_path = tokenize(path, '.');
	char result[2000];

	DDL_MoveToPath(state, result, ddl_path.size(), vector_to_pointer_array(ddl_path));
	DDL_SetUInt(state, context, value);
}

void set_stat_from_path(eModes mode, std::string path, std::uint32_t value)
{
	auto ddl_state = LiveStats_GetRootDDLState(mode);
	auto ddl_context = GetStatsBuffer(0);

	ddl_set_for_path(ddl_state, ddl_context, path, value);
}

void setMapEE(int map, bool all) {

	if (all) {
		set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_zod_ee", 1);
		set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_zod_super_ee", 1);
		set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_factory_ee", 1);
		set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_factory_super_ee", 1);
		set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_castle_ee", 1);
		set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_castle_super_ee", 1);
		set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_island_ee", 1);
		set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_island_super_ee", 1);
		set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_stalingrad_ee", 1);
		set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_stalingrad_super_ee", 1);
		set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_genesis_ee", 1);
		set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.DARKOPS_GENESIS_SUPER_EE", 1);

		switch (map) {
		case 0:
			set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_zod_ee", 1);
			set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_zod_super_ee", 1);
			break;
		case 1:
			set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_factory_ee", 1);
			set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_factory_super_ee", 1);
			break;
		case 2:
			set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_castle_ee", 1);
			set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_castle_super_ee", 1);
			break;
		case 3:
			set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_island_ee", 1);
			set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_island_super_ee", 1);
			break;
		case 4:
			set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_stalingrad_ee", 1);
			set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_stalingrad_super_ee", 1);
			break;
		case 5:
			set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.darkops_genesis_ee", 1);
			set_stat_from_path(Com_SessionMode_GetMode(), "playerstatslist.DARKOPS_GENESIS_SUPER_EE", 1);
			break;
		default:
			ImGui::InsertNotification({ ImGuiToastType::Info, 5000, "Selected map does not track EE completion." });
			break;
		}
	}
}

void setMapStat(int map, int round) {
	auto tmp = LiveStats_GetRootDDLState(GetSessionState());
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

void setMaxMapStats() {
	auto tmp = LiveStats_GetRootDDLState(GetSessionState());
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
		DDL_SetUInt((__int64)result, a1, 999);
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

void setClassName(int classSetIndex, int classIndex, std::string className) { 
	
	eGameModes a = Com_SessionMode_GetGameMode();
	eModes a1 = Com_SessionMode_GetMode();
	ClassSetType_t a3 = LiveStats_ClassSets_GetClassSetTypeForMode(a1, a);

	std::string cmd = "";
	
	switch (a3) {
	case CLASS_SET_TYPE_MP_PUBLIC:
		cmd = "storagewriteddl class_sets_mp publicmatchclasssets " + std::to_string(classSetIndex) + " customclassname " + std::to_string(classIndex) + " \"" + className + "\"";
		ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "%s", cmd.c_str()});
		break;
	case CLASS_SET_TYPE_MP_CUSTOM:
		cmd = "storagewriteddl class_sets_mp custommatchclasssets " + std::to_string(classSetIndex) + " customclassname " + std::to_string(classIndex) + " \"" + className + "\"";
		ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "%s", cmd.c_str()});
		break;
	case CLASS_SET_TYPE_MP_ARENA:
		cmd = "storagewriteddl class_sets_mp leaguematchclasssets " + std::to_string(classSetIndex) + " customclassname " + std::to_string(classIndex) + " \"" + className + "\"";
		ImGui::InsertNotification({ ImGuiToastType::Success, 5000, "%s", cmd.c_str()});
		break;
	default:
		cmd = "";
		break;
	}

	Cbuf_AddText(0, cmd.c_str());
	

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

		*(int*)((DWORD64)GetModuleHandleA(NULL) + 0x17E6F335) = 0;

		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		ImDrawList* drawlist = ImGui::GetBackgroundDrawList();
		drawlist->AddRectFilled(ImVec2(0, 0), ImVec2(desktop.right, desktop.bottom), IM_COL32(0, 0, 0, 150));

		ImGui::SetNextWindowSize(ImVec2(650.0f, 350.0f));

		std::string title = std::string(" - Scropts QOL for BO3 (") + KeybindNames[selectedBind] + ")";

		ImGui::Begin(title.c_str(), &open);

		ImGui::BeginTabBar("##main");

		if (ImGui::BeginTabItem("Account")) {

			ImGui::Dummy(ImVec2(0, 5));

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

			if (Live_IsUserSignedInToDemonware(CONTROLLER_INDEX_0)) ImGui::BeginDisabled();

			ImGui::Checkbox("Perma-unlock Watermelon Camo", &bLoginReward);
			if (Live_IsUserSignedInToDemonware(CONTROLLER_INDEX_0)) ImGui::EndDisabled();

			ImGui::SameLine();
			HelpMarker("Enable before start screen to permanently unlock the watermelon camo!");

			// Prestige
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

			if (ImGui::Button("Send All")) {

				setAllRanks();
				//LiveStats_SetStatByKey(Com_SessionMode_GetMode(), CONTROLLER_INDEX_0, MP_PLAYERSTATSKEY_PARAGONICONID, icon);

				LiveStorage_UploadStatsForController(0);
			}

			ImGui::SameLine();

			if (ImGui::Button("Max Stats")) {
				setStats();
				setChallenges();
				unlockSpecialistOutfits();
				setMaxTokens();
				LiveStorage_UploadStatsForController(0);
			}

			ImGui::Checkbox("Freeze Stats", &bProtectStats);

			ImGui::SameLine();

			HelpMarker("Enable this to freeze your stats and disable the 'To protect your stats you have been kicked to the main menu' error. Useful for reverting buggy MP stat edits. Disable before fresh restarts.");

			if (bProtectStats && !bProtectStatsRan) {
				Dvar_SetFromString("tu10_validationFatal", "0", true);
				bProtectStatsRan = true;
			}

			if (!bProtectStats && bProtectStatsRan) {
				Dvar_SetFromString("tu10_validationFatal", "1", true);
				bProtectStatsRan = false;
			}

			if (ImGui::Button("Unlock All Class Slots")) {

				unlockClassSlots();
				LiveStorage_UploadStatsForController(0);
			}

			if (ImGui::Button("Max Unlock Tokens")) {

				setMaxTokens();
				LiveStorage_UploadStatsForController(0);
			}

			if (ImGui::Button("Max Weapons")) {

				setMaxWeapons();
				setGroupStats();
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

				for (int i = 500; i < 600; ++i) {

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

			if (ImGui::Button("Complete Campaign")) {
				CompleteAllMissions();
			}

			if (ImGui::Button("Max Collectibles")) {
				UnlockAllCollectibles();
			}

			if (ImGui::Button("Max Accolades")) {
				UnlockAllCollectibles();
			}

			ImGui::TextDisabled("Suggest more unlocks");

			ImGui::EndChild();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Zombies")) {

			ImGui::Dummy(ImVec2(0, 5));

			ImGui::Text("Host Status: ");
			ImGui::SameLine();
			if (AreWeInGameAndHosting()) {
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

			if (ImGui::Button("Fast Restart Map")) {
				Cbuf_AddText(0, "fast_restart");
			}

			if (ImGui::Button("Full Restart Map")) {
				Cbuf_AddText(0, "map_restart");
			}

			if (ImGui::Button("No Teddy Bear")) {
				Dvar_SetFromString("sv_cheats", "1", false);
				Dvar_SetFromString("magic_chest_movable", "0", false);
				Dvar_SetFromString("sv_cheats", "0", false);
			}

			ImGui::Checkbox("God Mode##DMG", &bGodMode);
			//ImGui::SameLine();
			ImGui::Checkbox("Thorns Mode##DMG", &bThorns);
			//ImGui::SameLine();
			ImGui::Checkbox("Nukes Mode##DMG", &bNukes);
			//ImGui::SameLine();
			ImGui::Checkbox("##DMG", &bDamageMultiplier);

			ImGui::SameLine();
			ImGui::SliderInt("DMG Multiplier", &iDamageMultiplier, 1, 100);

			ImGui::Separator();

			ImGui::Dummy(ImVec2(0, 5));
			ImGui::BulletText("Map Stat Editor");

			ImGui::Combo("Map", &map, zmmapnames, IM_ARRAYSIZE(zmmapnames));
			ImGui::SameLine();
			ImGui::Checkbox("EE##Easteregg", &bCompleteEE);

			if (ImGui::Button("Set##MAP")) {

				setMapStat(map, setRound);
				if (bCompleteEE) {
					setMapEE(map, false);
				}
			}

			ImGui::SameLine();

			ImGui::SliderInt("Round##STAT", &setRound, 0, 1000);

			if (ImGui::Button("Max all Maps##MAP")) {
				setMaxMapStats();
			}

			if (ImGui::Button("Complete All EEs##MAP")) {
				setMapEE(0, true);
			}

			ImGui::Separator();

			ImGui::Dummy(ImVec2(0, 5));

			ImGui::BulletText("Gobblegum Editor");

			static int ItemID;

			const char* ItemName = getItemNameforItemID(ItemID);

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

			ImGui::InputInt("Item ID##GOBBLE", &ItemID);

			ImGui::Text(ItemName);

			if (ImGui::Button("Set Gobblegum"))
			{

				CACRoot Buffer;

				eGameModes a = Com_SessionMode_GetGameMode();
				eModes a1 = Com_SessionMode_GetMode();
				CACType a3 = LiveStats_Loadouts_GetCACTypeForMode(a1, a);
				auto CACroot = LiveStats_Loadouts_GetCACRoot(&Buffer, 0, a3);
				SetGobblegum(&Buffer, PackIndex, BuffIndex, ItemID);
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

				if (AreWeInGameAndHosting()) {

					__int64 sSessionModeState = ProcessBase + 0x168ED7F4;
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
			ImGui::InputInt("Weapon ID##SHOWCASE", &weaponIndex);
			ImGui::InputInt("Camo ID##SHOWCASE", &camoIndex);

			if (ImGui::Button("Set Showcase Weapon")) {

				static auto DDLContextForLocation = ((DDLContext * (__cdecl*)(__int64 ControllerIndex, unsigned int sessionMode, int location))(ProcessBase + 0x1EAA3B0))(0, MODE_MULTIPLAYER, CUSTOMIZATION_STATS_LOCATION_STATS);

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

			ImGui::Checkbox("Loop Cryptokeys", &bCrypto);

			ImGui::Checkbox("Spend Cryptokeys", &bCryptoSpend);

			ImGui::Checkbox("Loop Divinium", &bDivinium);

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
				for (size_t i = 0; i < 750; i++) {

					std::string cardName = get_column_value_for_row_from_path("gamedata/emblems/backgrounds.csv", i, 4);

					if (!cardName.empty()) {

						auto finalName = UI_SafeTranslateString(cardName.data());

						if (finalName != nullptr) {
							ImGui::Selectable(finalName);
							if (ImGui::IsItemClicked()) {
								previewValue = finalName;
								*(int*)(ProcessBase + 0x11306D40) = i;
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

			ImGui::Checkbox("Entitlements", &bSpoofPurchases);

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Spoof unlocks all entitlements, such as pre-order content and special events.");
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

			ImGui::Checkbox("Extra Slots", &bSpoofSlots);

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Spoof extra slot ownership. Gives you 10 CAC tabs and allows you\nto download / create more emblems, paintjobs, etc.");
			}

			// Make a checkbox.
			ImGui::Checkbox("Fileshare Downloading", &bFileshareDownloading);

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

			ImGui::SameLine();

			HelpMarker("Allows you to download items from the media browser (Emblems, Paintshop variants, etc.). Also spoofs ownership of items downloaded through scropts QOL, allowing you to modify your newly downloaded item!\n\nYou can find an item's download button by going to the media browser in the menus, selecting an item, and then going to the social menu [S].");

			

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
			ImGui::Text("Version: %s", version);

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

					ImGui::InputTextWithHint("##INPUTCLASSSETNAME", "Class-Set Name", &classSetName);

					ImGui::SameLine();				

					if (ImGui::Button("Set##CLASSSETNAME")) {
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

					ImGui::SetNextItemWidth(220.0f);
					ImGui::InputTextWithHint("##INPUTCLASSNAME", "Class Name", &className);

					ImGui::SameLine();

					if (ImGui::Button("Set##CLASSNAME")) {
						setClassName(classSetIndex, classIndex, className);
					}

					ImGui::SameLine();

					/*ImGui::BeginDisabled();

					if (ImGui::Button("Load Class##LOAD")) {

					}

					

					ImGui::SameLine();

					if (ImGui::Button("Send All##ALL")) {

					}

					ImGui::EndDisabled();

					ImGui::SameLine();*/

					ImGui::SetNextItemWidth(120.0f);
					ImGui::Combo("Combo Box Type", &iComboBoxType, "Weapons\000Attachments\000Camos\000Reticles\000Perks\000WildCards\000KillStreaks\000Extra\000");

					ImGui::SameLine();

					if (ImGui::Button("500 Class Items##")) {

						setClassSlotByName(255, classIndex, "primarygrenade");
						setClassSlotByName(255, classIndex, "primarygrenadecount");
						setClassSlotByName(255, classIndex, "specialgrenade");
						setClassSlotByName(255, classIndex, "specialgrenadecount");

					}

					auto limit = getComboItemSize(false);

					if (ImGui::BeginChild("##PRIMARY", ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2))) {

						auto primaryLimit = getComboItemSize(true);

						ImGui::BulletText("Primaries");

						ImGui::Separator();

						ImGui::BeginChild("##PRIMARYLIST", ImGui::GetContentRegionAvail());

						for (int j = 0; j < 10; ++j)
						{
							ImGui::Text(primaryMenuNames[j]);

							if (primaryPreviews[j].empty()) {
								primaryPreviews[j] = primaryMenuNames[10];
							}

							std::string primaryLabel = "##primary_class_options_" + std::to_string(j);
							std::string primaryButtonLabel = "Send Data##primary_class_button_" + std::to_string(j);

							if (ImGui::BeginCombo(primaryLabel.c_str(), primaryPreviews[j].c_str())) {

								for (size_t i = 0; i < primaryLimit; i++) {

									auto selectableLabel = getComboItemName(i, true);

									if (selectableLabel != nullptr) {

										bool isSelected = (!strcmp(selectableLabel, primaryPreviews[j].c_str()) && (i == primarySelectedItems[j]));
										std::string finalLabel = std::string(selectableLabel) + "##" + std::to_string(i);

										ImGui::Selectable(finalLabel.c_str(), isSelected);
										if (ImGui::IsItemClicked()) {
											primaryPreviews[j] = getComboItemName(i, true);
											primarySelectedItems[j] = i;
											primarySelectedSlot[j] = iComboBoxType;
										}
									}


								}
								ImGui::EndCombo();
							}



							ImGui::SameLine();
							if (ImGui::Button(primaryButtonLabel.c_str())) {
								setClassSetItem(primarySelectedItems[j], primarySelectedSlot[j], classSetIndex, classIndex, primarySlots[j]);
							}

						}

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

						for (int j = 0; j < 10; ++j)
						{
							ImGui::Text(secondaryMenuNames[j]);

							if (secondaryPreviews[j].empty()) {
								secondaryPreviews[j] = secondaryMenuNames[10];
							}

							std::string secondaryLabel = "##secondary_class_options_" + std::to_string(j);
							std::string secondaryButtonLabel = "Send Data##secondary_class_button_" + std::to_string(j);

							if (ImGui::BeginCombo(secondaryLabel.c_str(), secondaryPreviews[j].c_str())) {

								for (size_t i = 0; i < limit; i++) {

									auto selectableLabel = getComboItemName(i, false);

									if (selectableLabel != nullptr) {

										bool isSelected = (!strcmp(selectableLabel, secondaryPreviews[j].c_str()) && (i == secondarySelectedItems[j]));
										std::string finalLabel = std::string(selectableLabel) + "##" + std::to_string(i);

										ImGui::Selectable(finalLabel.c_str(), isSelected);
										if (ImGui::IsItemClicked()) {
											secondaryPreviews[j] = getComboItemName(i, false);
											secondarySelectedItems[j] = i;
											secondarySelectedSlot[j] = iComboBoxType;
										}
									}


								}
								ImGui::EndCombo();
							}



							ImGui::SameLine();
							if (ImGui::Button(secondaryButtonLabel.c_str())) {
								setClassSetItem(secondarySelectedItems[j], secondarySelectedSlot[j], classSetIndex, classIndex, secondarySlots[j]);
							}

						}

						// Secondary Weapon and attachment / camo editor

						ImGui::EndChild();
						ImGui::EndChild();
					}

					ImGui::Separator();

					if (ImGui::BeginChild("##OFFHANDS", ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y))) {

						ImGui::BulletText("Offhands");

						ImGui::Separator();

						ImGui::BeginChild("##OFFHANDLIST", ImGui::GetContentRegionAvail());

						for (int j = 0; j < 8; ++j)
						{
							ImGui::Text(offhandMenuNames[j]);

							if (offhandPreviews[j].empty()) {
								offhandPreviews[j] = offhandMenuNames[8];
							}

							std::string secondaryLabel = "##offhand_class_options_" + std::to_string(j);
							std::string secondaryButtonLabel = "Send Data##offhand_class_button_" + std::to_string(j);

							if (ImGui::BeginCombo(secondaryLabel.c_str(), offhandPreviews[j].c_str())) {

								for (size_t i = 0; i < limit; i++) {

									auto selectableLabel = getComboItemName(i, false);

									if (selectableLabel != nullptr) {

										bool isSelected = (!strcmp(selectableLabel, offhandPreviews[j].c_str()) && (i == offhandSelectedItems[j]));
										std::string finalLabel = std::string(selectableLabel) + "##" + std::to_string(i);

										ImGui::Selectable(finalLabel.c_str(), isSelected);
										if (ImGui::IsItemClicked()) {
											offhandPreviews[j] = getComboItemName(i, false);
											offhandSelectedItems[j] = i;
											offhandSelectedSlot[j] = iComboBoxType;
										}
									}


								}
								ImGui::EndCombo();
							}



							ImGui::SameLine();
							if (ImGui::Button(secondaryButtonLabel.c_str())) {
								setClassSetItem(offhandSelectedItems[j], offhandSelectedSlot[j], classSetIndex, classIndex, offhandSlots[j]);
							}

						}

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

						for (int j = 0; j < 9; ++j)
						{
							ImGui::Text(wildcardMenuNames[j]);

							if (wildcardPreviews[j].empty()) {
								wildcardPreviews[j] = wildcardMenuNames[9];
							}

							std::string secondaryLabel = "##wildcard_class_options_" + std::to_string(j);
							std::string secondaryButtonLabel = "Send Data##wildcard_class_button_" + std::to_string(j);

							if (ImGui::BeginCombo(secondaryLabel.c_str(), wildcardPreviews[j].c_str())) {

								for (size_t i = 0; i < limit; i++) {

									auto selectableLabel = getComboItemName(i, false);

									if (selectableLabel != nullptr) {

										bool isSelected = (!strcmp(selectableLabel, wildcardPreviews[j].c_str()) && (i == wildcardSelectedItems[j]));
										std::string finalLabel = std::string(selectableLabel) + "##" + std::to_string(i);

										ImGui::Selectable(finalLabel.c_str(), isSelected);
										if (ImGui::IsItemClicked()) {
											wildcardPreviews[j] = getComboItemName(i, false);
											wildcardSelectedItems[j] = i;
											wildcardSelectedSlot[j] = iComboBoxType;
										}
									}


								}
								ImGui::EndCombo();
							}



							ImGui::SameLine();
							if (ImGui::Button(secondaryButtonLabel.c_str())) {

								setClassSetItem(wildcardSelectedItems[j], wildcardSelectedSlot[j], classSetIndex, classIndex, wildcardSlots[j]);

							}

						}

						// Lethals, Tacticals

						ImGui::EndChild();
						ImGui::EndChild();
					}

					// If we can find a way to edit ZM: 


					ImGui::End();
				}
			//}
		}

	}

	else {
		*(int*)((DWORD64)GetModuleHandleA(NULL) + 0x17E6F335) = 1;
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

	if (bLoginReward) Dvar_SetFromString("loot_loginReward_active", "1", 1);

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

	ImGui::GetIO().MouseDrawCursor = open;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	draw();

	if (bNotifications) {
		ImGui::RenderNotifications();
	}

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return decltype(&hkPresent)(oPresent)(pSwapChain, SyncInterval, Flags);
}

HookReturn initialize() {
	__int64 DirectXVtableThing = **(__int64**)(ProcessBase + 0xF4B6858);
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
		dllbase = hMod;
		hooks::initialize();
		MH_Initialize();
		initialize();
		break;
	case DLL_PROCESS_DETACH:
		
		break;
	}
	return TRUE;
}