#pragma once
#include "includes.h"
#define OFFSET(x) ((INT64)GetModuleHandle(NULL) + (INT64)x)
#define QWORD DWORD64
#define Hook(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourAttach((LPVOID*)&original, (LPVOID)hook), DetourTransactionCommit())
#define Unhook(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourDetach((LPVOID*)&original, (LPVOID)hook), DetourTransactionCommit())
//#define ALOG(...) ImGui::InsertNotification({ ImGuiToastType::Info, 6000, ... })

extern HMODULE dllbase;

inline DWORD_PTR HookVMT(DWORD_PTR pVtable, DWORD_PTR hook, int index)
{
    DWORD_PTR* vtable = *(DWORD_PTR**)pVtable;
    DWORD_PTR backup = 0;
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery((LPCVOID)vtable, &mbi, sizeof(mbi));
    VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &mbi.Protect);
    backup = vtable[index];
    vtable[index] = (DWORD_PTR)hook;
    VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &mbi.Protect);
    return backup;
}


extern std::uintptr_t ProcessBase;
extern const void* spoof_t;
extern uint64_t PlayerXUID;
extern char spoofName[32];
extern const char* BlackMarketGuaranteeBribes[19];
extern const char* customclass[10];
extern std::string nameBuffer;
extern std::string custom_title_buf;
extern std::string custom_desc_buf;
extern std::string custom_delete_buf;
extern std::ofstream myfile;

extern bool bLogE;
extern bool bFriendsOnly;
extern bool bUnlockDLC;
extern bool bUnlockFullGame;
extern bool bSpoofPurchases;
extern bool bSpoofBlackMarket;
extern bool bSpoofCurrency;
extern bool bSpoofSlots;
extern bool bFileshareDownloading;
extern bool bFileshareDownloadingRan;
extern bool bFirstGumFree;
extern bool bFirstGumFreeRan;
extern bool bUIRgb;
extern bool bColoredUI;
extern bool bGodMode;
extern bool bThorns;
extern bool bNukes;
extern bool bDamageMultiplier;

extern float flHue;
extern float flSpeed;

extern ImColor mainColor;
extern ImColor uiColor;
extern ImColor mainRgb();

extern int iBlackmarketAmt;
extern int iBribe;
extern int iComboBoxType;
extern int iDamageMultiplier;

typedef bool(__fastcall* send_p2p_packet_t)(unsigned __int64 xuid, char type, const void* data, unsigned int cursize);
typedef __int64(__fastcall* LiveStats_GetRootDDLStateT)(int statThing);
typedef bool(__fastcall* LiveStats_SetShowcaseWeaponT)(unsigned int sessionMode, unsigned int statsLocation, DDLContext* ddlContext, unsigned int cacType, unsigned int characterIndex, Variant showcaseWeapon);
typedef bool(__fastcall* LiveStats_SetCharacterHeadIndexT)(unsigned int sessionMode, unsigned int statsLocation, DDLContext* ddlContext, unsigned int cacType, unsigned int characterHeadIndex);
typedef __int64(__fastcall* lergstuffT)(int, int);
typedef bool(__fastcall* LootT)(int controllerIndex, __int64 index, unsigned int incAmount);
typedef void(__fastcall* GameSendServerCommandT)(int clientNum, int reliable, const char* command);
typedef char* (__fastcall* tGetPersonaName) (DWORD_PTR _this);
typedef __int64(__fastcall* tLiveSteam_FilterPersonaName)(char* buffer, int size, char asciionly);
typedef int(__fastcall* GetSessionStateT)();
typedef __int64(__fastcall* LiveStats_Core_GetDDLContextT)(unsigned int ControllerIndex, int mode);
typedef __int64(__fastcall* LiveStats_Core_GetStableDDLContextT)(unsigned int ControllerIndex, int mode);
typedef __int64(__fastcall* DDL_MoveToNameT)(__int64 fromState, char* toState, const char* name);
typedef __int64(__fastcall* DDL_MoveToPathT)(__int64 fromState, char* toState, int depth, const char** path);
typedef bool(__fastcall* DDL_Lookup_MoveToNameT)(__int64 fromState, __int64 toState, const char* name);
typedef __int64(__fastcall* DDL_SetUIntT)(__int64 result, __int64 a1, int buf);
typedef __int64(__fastcall* DDL_SetIntT)(__int64 result, __int64 a1, int buf);
typedef __int64(__fastcall* DDL_SetStringT)(__int64 result, __int64 a1, const char* string);
typedef __int64(__fastcall* DDL_GetUIntT)(__int64 result, __int64 a1);
typedef __int64(__fastcall* LiveStorage_UploadStatsForControllerT)(int controllerIndex);
typedef void(__fastcall* live_presence_pack_t)(presence_data_s* presence, void* buffer, size_t buffer_size);

extern live_presence_pack_t live_presence_pack;
extern send_p2p_packet_t send_p2p_packet;
extern LiveStats_GetRootDDLStateT LiveStats_Core_GetRootDDLState;
extern GetSessionStateT GetSessionState;
extern LiveStats_Core_GetDDLContextT LiveStats_Core_GetDDLContext;
extern LiveStats_Core_GetStableDDLContextT LiveStats_Core_GetStableDDLContext;
extern DDL_MoveToNameT DDL_MoveToName;
extern DDL_MoveToPathT DDL_MoveToPath;
extern DDL_Lookup_MoveToNameT DDL_Lookup_MoveToName;
extern DDL_SetUIntT DDL_SetUInt;
extern DDL_SetIntT DDL_SetInt;
extern DDL_SetStringT DDL_SetString;
extern DDL_GetUIntT DDL_GetUInt;
extern LiveStorage_UploadStatsForControllerT LiveStorage_UploadStatsForController;
extern tGetPersonaName oGetPersonaName;
extern tLiveSteam_FilterPersonaName oLiveSteam_FilterPersonaName;
extern LiveStats_SetShowcaseWeaponT LiveStats_SetShowcaseWeapon;
extern LiveStats_SetCharacterHeadIndexT LiveStats_SetCharacterHeadIndex;
extern lergstuffT lergstuff;
extern LootT GiveLootToSelf;
extern GameSendServerCommandT SV_GameSendServerCommand;
extern DWORD_PTR pGetPersonaNameReturn;
extern DWORD_PTR ISteamFriends;
extern DWORD_PTR iSteamGameServer;
extern DWORD_PTR iSteamApps;

// DONT LOSE THIS 0x26AE260

//extern uintptr_t cgArray;

// Weird Crash (Steam auth ticket related)
// 
//const static auto PLMemcpy_Patch = reinterpret_cast<__int64 (__fastcall*)(int* a1, int* a2, unsigned __int64 a3)>(OFFSET(0x2C3D960));

//const static auto BG_UnlockablesIsItemLocked = reinterpret_cast<bool(__fastcall*)(eModes mode, ControllerIndex_t controllerIndex, int itemindex)>(ProcessBase + 0x26A8F60);
//const static auto BG_UnlockablesIsItemValidNotNull = reinterpret_cast<bool(__fastcall*)(eModes mode, int itemindex)>(ProcessBase + 0x26A9920);
//const static auto BG_UnlockablesIsItemDLCAvailable = reinterpret_cast<bool(__fastcall*)(eModes mode, ControllerIndex_t controllerIndex, int itemindex)>(ProcessBase + 0x26A8B80);
const static auto G_Damage = reinterpret_cast<__int64(__fastcall*)(__int64 targ, __int64 inflictor, __int64 attacker, __int64 a4, __int64 a5, int a6, int a7, int a8, __int64 a9, int a10, __int64 a11, int a12, int a13, int a14, __int16 a15, int a16, __int64 a17)>(ProcessBase + 0x1980960);
const static auto CL_ConnectionlessCMD = reinterpret_cast<bool(__fastcall*)(int clientNum, netadr_t *from, msg_t *msg)>(ProcessBase + 0x134CD50);
const static auto Msg_ReadStringLine = reinterpret_cast<char*(__fastcall*)(msg_t* msg, char* string, int maxChars)>(ProcessBase + 0x2157140);
const static auto Sys_GetTLS = reinterpret_cast<__int64(__fastcall*)()>(ProcessBase + 0x21837B0);

const static auto BG_UnlockablesSetBubbleGumPackName= reinterpret_cast<std::uintptr_t(__fastcall*)(CACRoot* cacRoot, int packIndex, const char* name)>(ProcessBase + 0x26AD4D0);
const static auto BG_UnlockablesGetItemName = reinterpret_cast<const char* (__fastcall*)(eModes eMode, int itemIndex)>(ProcessBase + 0x26A71F0);
const static auto BG_UnlockablesGetItemAttachmentDisplayName = reinterpret_cast<const char* (__fastcall*)(eModes eMode, int itemIndex, int attachmentNum)>(ProcessBase + 0x26A6410);
const static auto BG_UnlockablesClearWeaponOptionNew = reinterpret_cast<const char* (__fastcall*)(eModes eMode, ControllerIndex_t, int itemIndex, int xp)>(ProcessBase + 0x26A2E30);
const static auto BG_UnlockablesGetNumItemAttachments = reinterpret_cast<int(__fastcall*)(eModes eMode, int itemIndex)>(ProcessBase + 0x26A78D0);
const static auto UI_SafeTranslateString = reinterpret_cast<const char* (__fastcall*)(const char* String)>(ProcessBase + 0x228ED50);
const static auto SEH_SafeTranslateString = reinterpret_cast<const char* (__fastcall*)(const char* String)>(ProcessBase + 0x2279510);
const static auto CG_BoldGameMessageCenter = reinterpret_cast<std::uintptr_t(__fastcall*)(int clientNum, const char* msg)>(ProcessBase + 0x8C4C80);
const static auto Com_IsInGame = reinterpret_cast<bool(*)()>(ProcessBase + 0x21482C0);
const static auto Com_SessionMode_GetMode = reinterpret_cast<eModes(__cdecl*)()>(ProcessBase + 0x20F6D30);
const static auto Com_SessionMode_GetGameMode = reinterpret_cast<eGameModes(__cdecl*)()>(ProcessBase + 0x20F68B0);
const static auto LiveStats_ResetStats = reinterpret_cast<__int64(__fastcall*)(eModes mode, int controllerIndex, bool versionChanged, statsResetReason_t reason)>(ProcessBase + 0x1EA4E80);
const static auto LiveStats_Loadouts_GetCACTypeForMode = reinterpret_cast<CACType(__fastcall*)(__int64 sessionMode, __int64 gameMode)>(ProcessBase + 0x1EAF0B0);
const static auto LiveStats_ClassSets_GetClassSetTypeForMode = reinterpret_cast<ClassSetType_t(__fastcall*)( eModes gameMode, eGameModes sessionMode )>(ProcessBase + 0x1EA9260);
const static auto LiveStats_Loadouts_GetCACRoot = reinterpret_cast<CACRoot*(__fastcall*)(CACRoot* CacRoot, int ControllerIndex, int CacType)>(ProcessBase + 0x1EAF090);
const static auto LiveStats_ClassSets_GetClassSetDDLState = reinterpret_cast<bool*(__fastcall*)(DDLState* a1, int a2)>(ProcessBase + 0x1EA91B0);
const static auto BG_UnlockablesGetLoadoutSlotFromString = reinterpret_cast<std::uintptr_t(__fastcall*)(const char*)>(ProcessBase + 0x26A7710);
const static auto BG_UnlockablesSetItemIndex = reinterpret_cast<std::uintptr_t(__fastcall*)(CACRoot* CacRoot, __int64 a2, __int64 a3, __int64 itemIndex)>(ProcessBase + 0x16C8A0);
const static auto BG_UnlockablesSetClassSetItem = reinterpret_cast<std::uintptr_t(__fastcall*)(ControllerIndex_t controllerIndex, ClassSetType_t classSetType, int classSetIndex, int loadoutSlot, const char* slotName, int itemIndex)>(ProcessBase + 0x26AD6A0);
const static auto SetGobblegum = reinterpret_cast<std::uintptr_t(__fastcall*)(CACRoot* CacRoot, __int64 packindex, __int64 buffIndex, __int64 itemIndex)>(ProcessBase + 0x26AD440);
const static auto Live_IsUserInGame = reinterpret_cast<bool(*)(const int controllerIndex)>(ProcessBase + 0x1E0D750);
const static auto CL_IsLocalClientInGame = reinterpret_cast<std::uintptr_t(__fastcall*)(int localClientNum)>(ProcessBase + 0x13598E0);
const static auto CL_GetConfigString = reinterpret_cast<const char* (__fastcall*)(int configStringIndex)>(ProcessBase + 0x1321110);
const static auto CL_StoreConfigString = reinterpret_cast<const char* (*)(int configStringIndex, const char* string)>(ProcessBase + 0x13667C0);
const static auto Game_Message = reinterpret_cast<std::uintptr_t(__fastcall*)(int clientNum, const char* msg)>(ProcessBase + 0x8CB400);
const static auto Cbuf_AddText = reinterpret_cast<std::uintptr_t(__fastcall*)(int, const char*)>(ProcessBase + 0x20EC010);
const static auto Live_AreWeHost = reinterpret_cast<bool(*)()>(ProcessBase + 0x1E25DC0);
const static auto LiveFriends_IsFriendByXUID = reinterpret_cast<bool(__fastcall*)(int controllerIndex, __int64 XUID)>(ProcessBase + 0x1DF93A0);
const static auto Loot_BuyCrate = reinterpret_cast<bool(*)(int ControllerIndex, int CrateType, unsigned int CurrencyType)>(ProcessBase + 0x1E82480);
const static auto Loot_SpendVials = reinterpret_cast<bool(*)(unsigned int ControllerIndex, int VialNum)>(ProcessBase + 0x1E83860);
const static auto LiveStats_GetClanName = reinterpret_cast<const char* (__fastcall*)(int controllerIndex)>(ProcessBase + 0x1E9CE20);
const static auto LiveInventory_UpdateItemQuantity = reinterpret_cast<bool(__fastcall*)(int controllerIndex, int itemId, int newAmount, int modDateTime, int collisionField)>(ProcessBase + 0x1E0AA30);
const static auto ComScore_RedeemLootEvent = reinterpret_cast<void(__fastcall*)(int controllerIndex, int lootId, const char* lootRarity, const char* lootCategory)>(ProcessBase + 0x13E71B0);
const static auto LiveStats_SetClanTagText = reinterpret_cast<std::uintptr_t(__fastcall*)(int, const char*)>(ProcessBase + 0x1EA5480);
const static auto LiveStats_SetClassSetname = reinterpret_cast<bool(__fastcall*)(int controllerIndex, int classSetType, int classSetIndex, const char* classSetName)>(ProcessBase + 0x26AD7A0);
const static auto LiveStats_SetItemColor = reinterpret_cast<bool(__fastcall*)(__int64 a1, __int64 a2, QWORD * a3, int a4, int a5, int a6, int a7, int a8)>(ProcessBase + 0x1EAC650);
const static auto LiveStats_SetStatByKey = reinterpret_cast<bool(__fastcall*)(eModes mode, ControllerIndex_t controllerIndex, playerStatsKeyIndex_t keyIndex, int value)>(ProcessBase + 0x1EA5F50);
const static auto LiveStats_SetIntZombieStatByKey = reinterpret_cast<bool(__fastcall*)(ControllerIndex_t controllerIndex, zombieStatsKeyIndex_t keyIndex, int value)>(ProcessBase + 0x1EA5B40);
const static auto dwInstantDispatchMessage = reinterpret_cast<__int64(__fastcall*)(__int64 SenderId, unsigned int controllerIndex, __int64 msg, unsigned int messageSize)>(ProcessBase + 0x143A600);
const static auto LobbyMsg_HandleIM = reinterpret_cast<std::uintptr_t(__fastcall*)(unsigned int targetController, __int64 senderXuid, void* buff, int len)>(ProcessBase + 0x1EF63E0);
const static auto Dvar_SetFromString = reinterpret_cast<std::uintptr_t(__fastcall*)(const char* dvarname, const char* value, bool createifmissing)>(ProcessBase + 0x22C7500);
const static auto SetCharacterIndex = reinterpret_cast<bool(__fastcall*)(__int64 ControllerIndex, __int64 eModes, unsigned int CharacterIndex)>(ProcessBase + 0x19E1F0);
const static auto LiveStats_GetSelectedItemIndex = reinterpret_cast<unsigned int(__fastcall*)(__int64 controllerIndex, unsigned int mode, unsigned int location, int characterIndex, int ItemType)>(ProcessBase + 0x1EAB420);
const static auto LiveStats_SetSelectedItemIndex = reinterpret_cast<bool(__fastcall*)(__int64 controllerIndex, unsigned int mode, unsigned int location, int characterIndex, int ItemType, int index)>(ProcessBase + 0x1EACE50);
const static auto LiveStats_GetSelectedItemColor = reinterpret_cast<bool(__fastcall*)(__int64 controllerIndex, unsigned int mode, unsigned int location, int characterIndex, int ItemType, int itemIndex, int colorSlot)>(ProcessBase + 0x1EAACB0);
const static auto LiveStats_SetSelectedItemColor = reinterpret_cast<bool(__fastcall*)(__int64 controllerIndex, unsigned int mode, unsigned int location, int characterIndex, int ItemType, int itemIndex, int colorSlot, int index)>(ProcessBase + 0x1EAC600);
const static auto LiveStats_GetSelectedCharacterIndex = reinterpret_cast<unsigned int(__fastcall*)(__int64 controllerIndex, unsigned int mode, unsigned int location)>(ProcessBase + 0x1EAAE20);
const static auto LiveStats_SetSelectedCharacterIndex = reinterpret_cast<bool(__fastcall*)(__int64 controllerIndex, unsigned int mode, unsigned int location, int index)>(ProcessBase + 0x1EAAE20);
const static auto MSG_InitReadOnly = reinterpret_cast<void(*)(msg_t*, const char*, int)>(ProcessBase + 0x2155010); 
const static auto MSG_BeginReading = reinterpret_cast<void(*)(msg_t*)>(ProcessBase + 0x2154D00);
const static auto MSG_ReadByte = reinterpret_cast<std::uint8_t(*)(msg_t*)>(ProcessBase + 0x2155450);
const static auto MSG_ReadData = reinterpret_cast<void(*)(msg_t*, void*, int)>(ProcessBase + 0x21554B0);
const static auto MSG_InfoResponse = reinterpret_cast<bool(*)(void*, LobbyMsg*)>(ProcessBase + 0x1EE1E60);
const static auto Live_IsUserSignedInToDemonware = reinterpret_cast<bool(*)(const ControllerIndex_t)>(ProcessBase + 0x1E0D7A0);
const static auto LiveUser_GetXUID = reinterpret_cast<__int64(__fastcall*)(int)>(ProcessBase + 0x1EC7310);
const static auto LobbyTypes_GetMainMode = reinterpret_cast<LobbyMainMode(__cdecl*)()>(ProcessBase + 0x1EEBCB0);
const static auto LobbyMsgRW_PrepReadData = reinterpret_cast<bool(__fastcall*)(LobbyMsg*, char*, int)>(ProcessBase + 0x1EF69C0);
const static auto LobbyMsgRW_PrepWriteMsg = reinterpret_cast<bool(__fastcall*)(LobbyMsg*, char*, int, MsgType)>(ProcessBase + 0x1EF6A30);
const static auto LobbyMsgRW_PrepReadMsg = reinterpret_cast<bool(__fastcall*)(LobbyMsg*, msg_t*)>(ProcessBase + 0x1EF69F0);
const static auto LobbyMsgRW_ReadArrayBegin = reinterpret_cast<void(__fastcall*)(LobbyMsg*, const char*)>(ProcessBase + 0x1EF6510);
const static auto LobbyMsgRW_ReadString = reinterpret_cast<bool(__fastcall*)(LobbyMsg*, const char* expectedKey, char* a3)>(ProcessBase + 0x22E9940);
const static auto LobbyMsgRW_PackageArrayStart = reinterpret_cast<bool(__fastcall*)(void* lobbyMsg, const char* key)>(ProcessBase + 0x1EF6510);
const static auto LobbyMsgRW_PackageFloat = reinterpret_cast<bool(__fastcall*)(void* lobbyMsg, const char* key, float* val)>(ProcessBase + 0x1EF6630);
const static auto LobbyMsgRW_PackageUInt = reinterpret_cast<bool(__fastcall*)(void* lobbyMsg, const char* key, unsigned __int32* val)>(ProcessBase + 0x1EF68A0);
const static auto LobbyMsgRW_PackageInt = reinterpret_cast<bool(__fastcall*)(void* lobbyMsg, const char* key, int32_t * val)>(ProcessBase + 0x1EF66D0);
const static auto LobbyMsgRW_PackageUChar = reinterpret_cast<bool(__fastcall*)(void* lobbyMsg, const char* key, uint8_t * val)>(ProcessBase + 0x1EF6800);
const static auto LobbyMsgRW_PackageGlob = reinterpret_cast<bool(__fastcall*)(void* lobbyMsg, const char* key, char* val, int maxLength)>(ProcessBase + 0x1EF66B0);
const static auto LobbyMsgRW_PackageShort = reinterpret_cast<bool(__fastcall*)(void* lobbyMsg, const char* key, __int16* val)>(ProcessBase + 0x1EF6750);
const static auto LobbyMsgRW_PackageXuid = reinterpret_cast<bool(__fastcall*)(void* lobbyMsg, const char* key, uint64_t * val)>(ProcessBase + 0x1EF6940);
const static auto LobbyMsgRW_PackageString = reinterpret_cast<bool(__fastcall*)(void* lobbyMsg, const char* key, char* val, int maxLength)>(ProcessBase + 0x1EF6770);
const static auto LobbyMsgRW_PackageUInt64 = reinterpret_cast<bool(__fastcall*)(void* lobbyMsg, const char* key, unsigned __int64* val)>(ProcessBase + 0x1EF6820);
const static auto LobbyMsgRW_PackageBool = reinterpret_cast<bool(__fastcall*)(void* lobbyMsg, const char* key, char* val)>(ProcessBase + 0x1EF6580);
const static auto LobbyMsgRW_PackageUShort = reinterpret_cast<bool(__fastcall*)(void* lobbyMsg, const char* key, unsigned __int16* val)>(ProcessBase + 0x1EF6920);
const static auto LobbyMsgRW_PackageElement = reinterpret_cast<bool(__fastcall*)(LobbyMsg* lobbyMsg, bool addElement)>(ProcessBase + 0x1EF65C0);
const static auto LobbyMsgRW_PrintKeyTypeAndValue = reinterpret_cast<__int64(__fastcall*)(LobbyMsg * lobbyMsg, LobbyMsgElementType elementType)>(ProcessBase + 0x1EF6D20);
const static auto LobbyMsgRW_PrintDebugMessage = reinterpret_cast<__int64(__fastcall*)(LobbyMsg * lobbyMsg)>(ProcessBase + 0x1EF6B80);
const static auto LobbyMsgRW_PrintMessage = reinterpret_cast<void(__fastcall*)(LobbyMsg * lobbyMsg, bool forcePrint)>(ProcessBase + 0x1EF6E40);
const static auto UserHasLicenseForApp = reinterpret_cast<char(__fastcall*)(__int64 mapInfo, __int64* userObj)>(ProcessBase + 0x1EB7130);
const static auto LiveKeyArchive_SetValueForController = reinterpret_cast<void(__fastcall*)(ControllerIndex_t controllerIndex, dwKeyArchiveCategory category, keyArchiveIndex key, __int64 value, bdArchiveUpdateType updateType)>(ProcessBase + 0x1EBC9F0);
const static auto LiveKeyArchive_WriteAllCategories = reinterpret_cast<__int64(__fastcall*)(ControllerIndex_t controllerIndex, bool immediate)>(ProcessBase + 0x1EBCB30);
const static auto R_ConvertColorToBytes = reinterpret_cast<void(__fastcall*)(ImVec4 * color, byte * bytes)>(ProcessBase + 0x1D1CC10);
const static auto UI_IsRenderingImmediately = reinterpret_cast<bool(__cdecl*)()>(ProcessBase + 0x2705710);
const static auto UI_Model_GetDataType = reinterpret_cast<__int64(__fastcall*)(unsigned __int16 model)>(ProcessBase + 0x2019440);
const static auto UI_Interface_DrawText = reinterpret_cast<void(__fastcall*)(unsigned int localClientNum, __int64* luiElement, float xPos, float yPos, unsigned int R, unsigned int G,  unsigned int B, unsigned int A, char flags, char* text, __int64 font, float fontHeight, float wrapWidth, float alignment, char luaVM, QWORD * element)>(ProcessBase + 0x1F34920);
const static auto Memset = reinterpret_cast<int(__fastcall*)(char* a1, __int64 a2, unsigned __int64 a3)>(ProcessBase + 0x2C3DE60);
const static auto Live_Base_UserGetName = reinterpret_cast<UINT8(__fastcall*)(UINT8 * a1, int a2, char a3)>(ProcessBase + 0x1EB0D30);
const static auto LiveEntitlements_IsEntitlementActiveForController = reinterpret_cast<bool(__fastcall*)(ControllerIndex_t controllerIndex, int incentiveId)>(ProcessBase + 0x1E124E0);
const static auto LiveInventory_AreExtraSlotsPurchased = reinterpret_cast<bool(__fastcall*)(ControllerIndex_t controllerIndex)>(ProcessBase + 0x1E08950);
const static auto LiveInventory_GetItemQuantity = reinterpret_cast<__int64(__fastcall*)(ControllerIndex_t controllerIndex, int itemId)>(ProcessBase + 0x1E09030);
const static auto LiveInventory_GetPlayerInventory = reinterpret_cast<player_inventory_data_s * (__fastcall*)(ControllerIndex_t controllerIndex)>(ProcessBase + 0x1E091A0);
const static auto Fileshare_GetSummaryFileAuthorXUID = reinterpret_cast<uint64_t(__fastcall*)(ControllerIndex_t controllerIndex, uint64_t fileID)>(ProcessBase + 0x1DF61E0);
const static auto Fileshare_DownloadFile = reinterpret_cast<bool(__fastcall*)(const ControllerIndex_t controllerIndex, uint64_t fileId, const char* fileName, bool isCommunityFile)>(ProcessBase + 0x1E874F0);
const static auto Fileshare_CanDownloadFile = reinterpret_cast<bool(__fastcall*)(unsigned int a1, __int64 ownerXuid, int a3, __int64 fileId, bool isCommunityFile)>(ProcessBase + 0x1E87430);
const static auto Live_UserGetName = reinterpret_cast<bool(__fastcall*)(ControllerIndex_t controllerIndex, char* buf, int bufSize)>(ProcessBase + 0x1EC75D0);
const static auto StringTable_GetAsset = reinterpret_cast<void(__fastcall*)(const char* filename, StringTable * *tablePtr)>(ProcessBase + 0x22AE3D0);
const static auto StringTable_GetColumnValueForRow = reinterpret_cast<const char* (__fastcall*)(StringTable * table, __int32 row, __int32 column)>(ProcessBase + 0x22AE660);
const static auto StringTable_Lookup = reinterpret_cast<const char* (__fastcall*)(StringTable * table, int comparisonColumn, const char* value, int valueColumn)>(ProcessBase + 0x22AE7F0);
const static auto Storage_GetDDLRootState = reinterpret_cast<DDLState*(__fastcall*)(int fileType)>(ProcessBase + 0x2276B80);
const static auto Storage_GetDDLContext = reinterpret_cast<DDLContext*(__fastcall*)(unsigned int a1, int a2, int a3)>(ProcessBase + 0x2276AB0);
const static auto Storage_Write = reinterpret_cast<bool(__fastcall*)(int a1, int a2, __int64 a3)>(ProcessBase + 0x2277960);
const static auto SV_AddTestClient = reinterpret_cast<__int64(__fastcall*)()>(ProcessBase + 0x2248F40);
const static auto LiveUser_GetXuidString = reinterpret_cast<const char* (__fastcall*)(int)>(ProcessBase + 0x1EC6E90);
const static auto Demo_SaveScreenshotToContentServer = reinterpret_cast<void(__fastcall*)(int localClientNum, int fileSlot)>(ProcessBase + 0x2609B00);
const static auto Demo_SetMetaData = reinterpret_cast<bool(__fastcall*)(unsigned int a1, __int64 a2, unsigned int a3, __int64 metaDataSize, int a5, char duration)>(ProcessBase + 0x260A040);
const static auto Fileshare_CreateMetaData = reinterpret_cast<bool(__fastcall*)(__int64 a1, FileshareMetaInfo* metaInfo, int* a3, int a4)>(ProcessBase + 0x1DF2990);
const static auto I_Strcpy = reinterpret_cast<void(__fastcall*)(BYTE* a1, __int64 a2, const char* a3)>(ProcessBase + 0x22E9410);
const static auto Fileshare_FileRemove = reinterpret_cast<bool(__fastcall*)(unsigned int controllerIndex, __int64 fileID)>(ProcessBase + 0x1DEF2E0);
//const static auto DDL_Lookup_MoveToName = reinterpret_cast<bool(__fastcall*)(const DDLState* fromState, DDLState* toState, const char* name)>(ProcessBase + 0x1427A00);

const static auto flsomeWeirdCharacterIndex = reinterpret_cast<float(__fastcall*)(__int64 a1, int a2, int a3)>(ProcessBase + 0x2342100);
const static auto get_item_infos(eModes mode) { return &reinterpret_cast<item_infos_s*>(ProcessBase + 0x19C2AE80)[mode]; }

extern const char* slots[96];

extern const char* primaryMenuNames[11];
extern const char* primarySlots[10];
extern std::string primaryPreviews[10];
extern int primarySelectedItems[10];
extern int primarySelectedSlot[10];

extern const char* cpmapnames[11];
extern const char* zmmapnames[14];
extern const char* secondaryMenuNames[11];
extern const char* secondarySlots[10];
extern std::string secondaryPreviews[10];
extern int secondarySelectedItems[10];
extern int secondarySelectedSlot[10];

extern const char* wildcardMenuNames[10];
extern const char* wildcardSlots[9];
extern std::string wildcardPreviews[9];
extern int wildcardSelectedItems[9];
extern int wildcardSelectedSlot[9];

extern const char* offhandMenuNames[9];
extern const char* offhandSlots[8];
extern const char* KeybindNames[17];
extern int Keybinds[17];
extern std::string offhandPreviews[8];
extern int offhandSelectedItems[8];
extern int offhandSelectedSlot[8];

extern std::vector<std::string> legit_packets;
