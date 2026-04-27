#pragma once
#include "includes.h"
#define OFFSET(x) ((INT64)GetModuleHandleA(NULL) + (INT64)x)
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

// Typedefs
typedef __int16(__fastcall* CG_DObjGetWorldTagPosInternalT)(__int64 centity_t, __int64 DObj, int tag, float* whatever, float* pos, int something);
typedef __int64(__fastcall* Com_GetClientDObjT)(int a1, int a2);
typedef bool(__fastcall* DDL_GetUIntT)(__int64 result, __int64 a1);
typedef __int64(__fastcall* DDL_MoveToNameT)(__int64 fromState, char* toState, const char* name);
typedef __int64(__fastcall* DDL_MoveToPathT)(__int64 fromState, char* toState, int depth, const char** path);
typedef bool(__fastcall* DDL_Lookup_MoveToNameT)(__int64 fromState, __int64 toState, const char* name);
typedef __int64(__fastcall* DDL_SetIntT)(__int64 result, __int64 a1, int buf);
typedef __int64(__fastcall* DDL_SetStringT)(__int64 result, __int64 a1, const char* string);
typedef __int64(__fastcall* DDL_SetUIntT)(__int64 result, __int64 a1, int buf);
typedef void(__fastcall* GameSendServerCommandT)(int clientNum, int reliable, const char* command);
typedef __int16(__fastcall* GScr_AllocStringT)(const char* s);
typedef int(__fastcall* GetSessionStateT)();
typedef __int64(__fastcall* LiveStats_Core_GetDDLContextT)(unsigned int ControllerIndex, int mode);
typedef __int64(__fastcall* LiveStats_Core_GetStableDDLContextT)(unsigned int ControllerIndex, int mode);
typedef __int64(__fastcall* LiveStats_GetRootDDLStateT)(int statThing);
typedef bool(__fastcall* LiveStats_SetCharacterHeadIndexT)(unsigned int sessionMode, unsigned int statsLocation, DDLContext* ddlContext, unsigned int cacType, unsigned int characterHeadIndex);
typedef bool(__fastcall* LiveStats_SetShowcaseWeaponT)(unsigned int sessionMode, unsigned int statsLocation, DDLContext* ddlContext, unsigned int cacType, unsigned int characterIndex, Variant showcaseWeapon);
typedef __int64(__fastcall* LiveStorage_UploadStatsForControllerT)(int controllerIndex);
typedef bool(__fastcall* LootT)(int controllerIndex, __int64 index, unsigned int incAmount);
typedef __int64(__fastcall* lergstuffT)(int, int);
typedef void(__fastcall* live_presence_pack_t)(presence_data_s* presence, void* buffer, size_t buffer_size);
typedef char* (__fastcall* tGetPersonaName) (DWORD_PTR _this);
typedef __int64(__fastcall* tLiveSteam_FilterPersonaName)(char* buffer, int size, char asciionly);
typedef bool(__fastcall* send_p2p_packet_t)(unsigned __int64 xuid, char type, const void* data, unsigned int cursize);
typedef bool(__fastcall* WorldPosToScreenPosT)(int localClientNum, float* worldPos, float* out);

// Extern variables
extern bool bAllTracers;
extern bool bColoredUI;
extern bool bDamageMultiplier;
extern bool bFileshareDownloading;
extern bool bFileshareDownloadingRan;
extern bool bFirstGumFree;
extern bool bFirstGumFreeRan;
extern bool bFriendsOnly;
extern bool bGodMode;
extern bool bGodModeRan;
extern bool bInfAmmo;
extern bool bInfAmmoRan;
extern bool bLogE;
extern bool bPointersInitialized;
extern bool bSpoofBlackMarket;
extern bool bSpoofCurrency;
extern bool bSpoofPurchases;
extern bool bSpoofSlots;
extern bool bsvCheats;
extern bool bsvCheatsRan;
extern bool bThorns;
extern bool bTracers;
extern bool bUIRgb;
extern bool bUnlockDLC;
extern bool bUnlockFullGame;
extern bool bNukes;

extern char spoofName[32];

extern float flHue;
extern float flSpeed;
extern float flTracerTime;

extern int iBlackmarketAmt;
extern int iBribe;
extern int iComboBoxType;
extern int iDamageMultiplier;

extern __int64 cgsArray;
extern __int64 clientactive_t;
extern __int64 clientinfo;
extern __int64 playerState;

extern uintptr_t cg_EntitiesArray;
extern uintptr_t centity_t;
extern uintptr_t cgArray;

extern uint64_t PlayerXUID;

extern const void* spoof_t;

extern const char* BlackMarketGuaranteeBribes[19];
extern const char* customclass[10];

extern std::string custom_delete_buf;
extern std::string custom_title_buf;
extern std::string custom_desc_buf;
extern std::string statPathBuffer;
extern std::string nameBuffer;

extern std::uintptr_t ProcessBase;

extern std::ofstream myfile;

extern ImColor mainColor;
extern ImColor uiColor;
extern ImColor mainRgb();

// Extern function pointers
extern CG_DObjGetWorldTagPosInternalT CG_DObjGetWorldTagPosInternal;
extern Com_GetClientDObjT Com_GetClientDObj;
extern DDL_GetUIntT DDL_GetUInt;
extern DDL_MoveToNameT DDL_MoveToName;
extern DDL_MoveToPathT DDL_MoveToPath;
extern DDL_Lookup_MoveToNameT DDL_Lookup_MoveToName;
extern DDL_SetIntT DDL_SetInt;
extern DDL_SetStringT DDL_SetString;
extern DDL_SetUIntT DDL_SetUInt;
extern DWORD_PTR ISteamFriends;
extern DWORD_PTR iSteamApps;
extern DWORD_PTR iSteamGameServer;
extern DWORD_PTR pGetPersonaNameReturn;
extern GameSendServerCommandT SV_GameSendServerCommand;
extern GetSessionStateT GetSessionState;
extern GScr_AllocStringT GScr_AllocString;
extern LiveStats_Core_GetDDLContextT LiveStats_Core_GetDDLContext;
extern LiveStats_Core_GetStableDDLContextT LiveStats_Core_GetStableDDLContext;
extern LiveStats_GetRootDDLStateT LiveStats_Core_GetRootDDLState;
extern LiveStats_SetCharacterHeadIndexT LiveStats_SetCharacterHeadIndex;
extern LiveStats_SetShowcaseWeaponT LiveStats_SetShowcaseWeapon;
extern LiveStorage_UploadStatsForControllerT LiveStorage_UploadStatsForController;
extern LootT GiveLootToSelf;
extern lergstuffT lergstuff;
extern live_presence_pack_t live_presence_pack;
extern send_p2p_packet_t send_p2p_packet;
extern tGetPersonaName oGetPersonaName;
extern tLiveSteam_FilterPersonaName oLiveSteam_FilterPersonaName;
extern WorldPosToScreenPosT WorldPosToScreenPos;

// DONT LOSE THIS 0x26AE260

// Const static function pointers
const static auto BG_GetAttachmentName = reinterpret_cast<const char* (__fastcall*)(int itemIndex)>(ProcessBase + 0x266FAF0);
const static auto BG_UnlockablesGetClassSetItem = reinterpret_cast<__int64(__fastcall*)(ControllerIndex_t controllerIndex, ClassSetType_t classSetType, int classSetIndex, int loadoutSlot, const char* slotName)>(ProcessBase + 0x262CAC0);
const static auto BG_UnlockablesGetItemAttachmentDisplayName = reinterpret_cast<const char* (__fastcall*)(eModes eMode, int itemIndex, int attachmentNum)>(ProcessBase + 0x262D960);
const static auto BG_UnlockablesGetItemName = reinterpret_cast<const char* (__fastcall*)(eModes eMode, int itemIndex)>(ProcessBase + 0x262E740);
const static auto BG_UnlockablesGetLoadoutSlotFromString = reinterpret_cast<std::uintptr_t(__fastcall*)(const char*)>(ProcessBase + 0x262EC60);
const static auto BG_UnlockablesGetNumItemAttachments = reinterpret_cast<int(__fastcall*)(eModes eMode, int itemIndex)>(ProcessBase + 0x262EE20);
const static auto BG_UnlockablesSetBubbleGumPackName= reinterpret_cast<std::uintptr_t(__fastcall*)(CACRoot* cacRoot, int packIndex, const char* name)>(ProcessBase + 0x2634A20);
const static auto BG_UnlockablesSetClassSetItem = reinterpret_cast<std::uintptr_t(__fastcall*)(ControllerIndex_t controllerIndex, ClassSetType_t classSetType, int classSetIndex, int loadoutSlot, const char* slotName, int itemIndex)>(ProcessBase + 0x2634BF0);
const static auto BG_UnlockablesSetItemIndex = reinterpret_cast<std::uintptr_t(__fastcall*)(CACRoot* CacRoot, __int64 a2, __int64 a3, __int64 itemIndex)>(ProcessBase + 0x16C8A0);
const static auto Cbuf_AddText = reinterpret_cast<std::uintptr_t(__fastcall*)(int, const char*)>(ProcessBase + 0x20DFF50);
const static auto CG_BoldGameMessageCenter = reinterpret_cast<std::uintptr_t(__fastcall*)(int clientNum, const char* msg)>(ProcessBase + 0x8C4C80);
const static auto CG_BulletHitEvent_Internal = reinterpret_cast<__int64(__fastcall*)(int localClientNum, int sourceEntityNum, int targetEntityNum, __int64 weapon, ImVec3 startPos, ImVec3 position, ImVec3 normal, ImVec3 seeThruDecalNormal, int surfType, int* _event, __int64 eventParam, __int16 a12, unsigned __int16 a13, int a14)>(ProcessBase + 0x1189E10);
const static auto CG_Draw2D = reinterpret_cast<void(__fastcall*)(int a1)>(ProcessBase + 0x60F920);
const static auto CL_AddReliableCommand = reinterpret_cast<std::uintptr_t(__fastcall*)(int, const char*)>(ProcessBase + 0x135D4E0);
const static auto CL_ConnectionlessCMD = reinterpret_cast<bool(__fastcall*)(int clientNum, netadr_t *from, msg_t *msg)>(ProcessBase + 0x134CD70);
const static auto CL_GetConfigString = reinterpret_cast<const char* (__fastcall*)(int configStringIndex)>(ProcessBase + 0x1321130);
const static auto CL_IsLocalClientInGame = reinterpret_cast<std::uintptr_t(__fastcall*)(int localClientNum)>(ProcessBase + 0x1359900);
const static auto CL_StoreConfigString = reinterpret_cast<const char* (*)(int configStringIndex, const char* string)>(ProcessBase + 0x13667E0);
const static auto Com_Error = reinterpret_cast<bool(*)(const char* a1, int a2, unsigned int a3, const char * a4, ...)>(ProcessBase + 0x20EC0B0);
const static auto Com_IsInGame = reinterpret_cast<bool(*)()>(ProcessBase + 0x20EFF20);
const static auto Com_LoadLevelFastFiles = reinterpret_cast<void(__fastcall*)(const char* mapName)>(ProcessBase + 0x20F0140);
const static auto Com_SessionMode_GetGameMode = reinterpret_cast<eGameModes(__cdecl*)()>(ProcessBase + 0x20EA7F0);
const static auto Com_SessionMode_GetMode = reinterpret_cast<eModes(__cdecl*)()>(ProcessBase + 0x20EAC70);
const static auto DB_CheckXFile = reinterpret_cast<bool(__fastcall*)(const char* a1, __int64 a2, __int64 a3, __int64 a4)>(ProcessBase + 0x141F9D0);
const static auto Demo_SaveScreenshotToContentServer = reinterpret_cast<void(__fastcall*)(int localClientNum, int fileSlot)>(ProcessBase + 0x2591050);
const static auto Demo_SetMetaData = reinterpret_cast<bool(__fastcall*)(unsigned int a1, __int64 a2, unsigned int a3, __int64 metaDataSize, int a5, char duration)>(ProcessBase + 0x2591590);
const static auto Dvar_SetFromString = reinterpret_cast<std::uintptr_t(__fastcall*)(const char* dvarname, const char* value, bool createifmissing)>(ProcessBase + 0x226B0A0);
const static auto Fileshare_CanDownloadFile = reinterpret_cast<bool(__fastcall*)(unsigned int a1, __int64 ownerXuid, int a3, __int64 fileId, bool isCommunityFile)>(ProcessBase + 0x1E7B060);
const static auto Fileshare_CreateMetaData = reinterpret_cast<bool(__fastcall*)(__int64 a1, FileshareMetaInfo* metaInfo, int* a3, int a4)>(ProcessBase + 0x1DE65C0);
const static auto Fileshare_GetSummaryFileAuthorXUID = reinterpret_cast<uint64_t(__fastcall*)(ControllerIndex_t controllerIndex, uint64_t fileID)>(ProcessBase + 0x1DE9E10);
const static auto flsomeWeirdCharacterIndex = reinterpret_cast<float(__fastcall*)(__int64 a1, int a2, int a3)>(ProcessBase + 0x22C9650);
const static auto G_Damage = reinterpret_cast<__int64(__fastcall*)(__int64 targ, __int64 inflictor, __int64 attacker, __int64 a4, __int64 a5, int a6, int a7, int a8, __int64 a9, int a10, __int64 a11, int a12, int a13, int a14, __int16 a15, int a16, __int64 a17)>(ProcessBase + 0x1980980);
const static auto Game_Message = reinterpret_cast<std::uintptr_t(__fastcall*)(int clientNum, const char* msg)>(ProcessBase + 0x8CB400);
const static auto get_item_infos(eModes mode) { return &reinterpret_cast<item_infos_s*>(ProcessBase + 0x19BABF00)[mode]; }
const static auto I_Strcpy = reinterpret_cast<void(__fastcall*)(BYTE* a1, __int64 a2, const char* a3)>(ProcessBase + 0x227CA00);
const static auto Live_AreWeHost = reinterpret_cast<bool(*)()>(ProcessBase + 0x1E199F0);
const static auto Live_Base_UserGetName = reinterpret_cast<UINT8(__fastcall*)(UINT8 * a1, int a2, char a3)>(ProcessBase + 0x1EA4960);
const static auto Live_IsUserInGame = reinterpret_cast<bool(*)(const int controllerIndex)>(ProcessBase + 0x1E01380);
const static auto Live_IsUserSignedInToDemonware = reinterpret_cast<bool(*)(const ControllerIndex_t)>(ProcessBase + 0x1E013D0);
const static auto Live_UserGetName = reinterpret_cast<bool(__fastcall*)(ControllerIndex_t controllerIndex, char* buf, int bufSize)>(ProcessBase + 0x1EBB200);
const static auto LiveAchievements_GiveAchievement = reinterpret_cast<void(__fastcall*)(const char* achievementName)>(ProcessBase + 0x1EA5650);
const static auto LiveFriends_IsFriendByXUID = reinterpret_cast<bool(__fastcall*)(int controllerIndex, __int64 XUID)>(ProcessBase + 0x1DECFD0);
const static auto LiveEntitlements_IsEntitlementActiveForController = reinterpret_cast<bool(__fastcall*)(ControllerIndex_t controllerIndex, int incentiveId)>(ProcessBase + 0x1E06110);
const static auto LiveInventory_AreExtraSlotsPurchased = reinterpret_cast<bool(__fastcall*)(ControllerIndex_t controllerIndex)>(ProcessBase + 0x1DFC580);
const static auto LiveInventory_IsValid = reinterpret_cast<bool(__fastcall*)(ControllerIndex_t controllerIndex)>(ProcessBase + 0x1DFDFE0);
const static auto LiveInventory_GetItemQuantity = reinterpret_cast<__int64(__fastcall*)(ControllerIndex_t controllerIndex, int itemId)>(ProcessBase + 0x1DFCC60);
const static auto LiveInventory_GetPlayerInventory = reinterpret_cast<player_inventory_data_s * (__fastcall*)(ControllerIndex_t controllerIndex)>(ProcessBase + 0x1DFCDD0);
const static auto LiveKeyArchive_SetValueForController = reinterpret_cast<void(__fastcall*)(ControllerIndex_t controllerIndex, dwKeyArchiveCategory category, keyArchiveIndex key, __int64 value, bdArchiveUpdateType updateType)>(ProcessBase + 0x1EB0620);
const static auto LiveKeyArchive_WriteAllCategories = reinterpret_cast<__int64(__fastcall*)(ControllerIndex_t controllerIndex, bool immediate)>(ProcessBase + 0x1EB0760);
const static auto LivePresence_Serialize = reinterpret_cast<__int64(__fastcall*)(__int64 a1, __int64 a2)>(ProcessBase + 0x1E85450);
const static auto LiveStats_ClassSets_GetClassSetDDLState = reinterpret_cast<bool*(__fastcall*)(DDLState* a1, int a2)>(ProcessBase + 0x1E9CDE0);
const static auto LiveStats_ClassSets_GetClassSetTypeForMode = reinterpret_cast<ClassSetType_t(__fastcall*)( eModes gameMode, eGameModes sessionMode )>(ProcessBase + 0x1E9CE90);
const static auto LiveStats_GetClanName = reinterpret_cast<const char* (__fastcall*)(int controllerIndex)>(ProcessBase + 0x1E90A50);
const static auto LiveStats_GetSelectedItemIndex = reinterpret_cast<unsigned int(__fastcall*)(__int64 controllerIndex, unsigned int mode, unsigned int location, int characterIndex, int ItemType)>(ProcessBase + 0x1E9F050);
const static auto LiveStats_Loadouts_GetCACRoot = reinterpret_cast<CACRoot*(__fastcall*)(CACRoot* CacRoot, int ControllerIndex, int CacType)>(ProcessBase + 0x1EA2CC0);
const static auto LiveStats_Loadouts_GetCACTypeForMode = reinterpret_cast<CACType(__fastcall*)(__int64 sessionMode, __int64 gameMode)>(ProcessBase + 0x1EA2CE0);
const static auto LiveStats_SetClanTagText = reinterpret_cast<std::uintptr_t(__fastcall*)(int, const char*)>(ProcessBase + 0x1E990B0);
const static auto LiveStats_SetClassSetname = reinterpret_cast<bool(__fastcall*)(int controllerIndex, int classSetType, int classSetIndex, const char* classSetName)>(ProcessBase + 0x2634CF0);
const static auto LiveStats_SetIntZombieStatByKey = reinterpret_cast<bool(__fastcall*)(ControllerIndex_t controllerIndex, zombieStatsKeyIndex_t keyIndex, int value)>(ProcessBase + 0x1E99770);
const static auto LiveStats_SetItemColor = reinterpret_cast<bool(__fastcall*)(__int64 a1, __int64 a2, QWORD * a3, int a4, int a5, int a6, int a7, int a8)>(ProcessBase + 0x1EA0280);
const static auto LiveStats_SetSelectedItemColor = reinterpret_cast<bool(__fastcall*)(__int64 controllerIndex, unsigned int mode, unsigned int location, int characterIndex, int ItemType, int itemIndex, int colorSlot, int index)>(ProcessBase + 0x1EA0230);
const static auto LiveStats_SetSelectedItemIndex = reinterpret_cast<bool(__fastcall*)(__int64 controllerIndex, unsigned int mode, unsigned int location, int characterIndex, int ItemType, int index)>(ProcessBase + 0x1EA0A80);
const static auto LiveStats_SetStatByKey = reinterpret_cast<bool(__fastcall*)(eModes mode, ControllerIndex_t controllerIndex, playerStatsKeyIndex_t keyIndex, int value)>(ProcessBase + 0x1E99B80);
const static auto LiveUser_GetXUID = reinterpret_cast<__int64(__fastcall*)(int)>(ProcessBase + 0x1EBAF40);
const static auto LiveUser_GetXuidString = reinterpret_cast<const char* (__fastcall*)(int)>(ProcessBase + 0x1EBAAC0);
const static auto LobbyMsg_HandleIM = reinterpret_cast<std::uintptr_t(__fastcall*)(unsigned int targetController, __int64 senderXuid, void* buff, int len)>(ProcessBase + 0x1EEA130);
const static auto LobbyMsgRW_PackageElement = reinterpret_cast<bool(__fastcall*)(LobbyMsg* lobbyMsg, bool addElement)>(ProcessBase + 0x1EEA320);
const static auto LobbyMsgRW_PrepReadData = reinterpret_cast<bool(__fastcall*)(LobbyMsg*, char*, int)>(ProcessBase + 0x1EEA4F0);
const static auto LobbyMsgRW_PrepReadMsg = reinterpret_cast<bool(__fastcall*)(LobbyMsg*, msg_t*)>(ProcessBase + 0x1EEA520);
const static auto LobbyMsgRW_PrepWriteMsg = reinterpret_cast<bool(__fastcall*)(LobbyMsg*, char*, int, MsgType)>(ProcessBase + 0x1EEA560);
const static auto LobbyMsgRW_ReadArrayBegin = reinterpret_cast<void(__fastcall*)(LobbyMsg*, const char*)>(ProcessBase + 0x1EEA260);
const static auto LobbyMsgRW_ReadString = reinterpret_cast<bool(__fastcall*)(LobbyMsg*, const char* expectedKey, char* a3)>(ProcessBase + 0x227CF30);
const static auto LobbyTypes_GetMainMode = reinterpret_cast<LobbyMainMode(__cdecl*)()>(ProcessBase + 0x1EDFA00);
const static auto Loot_BuyCrate = reinterpret_cast<bool(*)(int ControllerIndex, int CrateType, unsigned int CurrencyType)>(ProcessBase + 0x1E760B0);
const static auto Loot_SpendVials = reinterpret_cast<bool(*)(unsigned int ControllerIndex, int VialNum)>(ProcessBase + 0x1E77490);
const static auto Memset = reinterpret_cast<int(__fastcall*)(char* a1, __int64 a2, unsigned __int64 a3)>(ProcessBase + 0x2BC53B0);
const static auto MSG_BeginReading = reinterpret_cast<void(*)(msg_t*)>(ProcessBase + 0x20FC900);
const static auto MSG_InfoResponse = reinterpret_cast<bool(*)(void*, LobbyMsg*)>(ProcessBase + 0x1ED5B40);
const static auto MSG_InitReadOnly = reinterpret_cast<void(*)(msg_t*, const char*, int)>(ProcessBase + 0x20FCC10);
const static auto MSG_ReadByte = reinterpret_cast<std::uint8_t(*)(msg_t*)>(ProcessBase + 0x20FD050);
const static auto MSG_ReadData = reinterpret_cast<void(*)(msg_t*, void*, int)>(ProcessBase + 0x20FD0B0);
const static auto Msg_ReadStringLine = reinterpret_cast<char*(__fastcall*)(msg_t* msg, char* string, int maxChars)>(ProcessBase + 0x20FED40);
const static auto R_ConvertColorToBytes = reinterpret_cast<void(__fastcall*)(ImVec4 * color, byte * bytes)>(ProcessBase + 0x1D10840);
const static auto SEH_SafeTranslateString = reinterpret_cast<const char* (__fastcall*)(const char* String)>(ProcessBase + 0x221D0B0);
const static auto SetCharacterIndex = reinterpret_cast<bool(__fastcall*)(__int64 ControllerIndex, __int64 eModes, unsigned int CharacterIndex)>(ProcessBase + 0x19E1F0);
const static auto SetGobblegum = reinterpret_cast<std::uintptr_t(__fastcall*)(CACRoot* CacRoot, __int64 packindex, __int64 buffIndex, __int64 itemIndex)>(ProcessBase + 0x2634990);
const static auto Storage_GetDDLContext = reinterpret_cast<DDLContext*(__fastcall*)(unsigned int a1, int a2, int a3)>(ProcessBase + 0x221A640);
const static auto Storage_GetDDLRootState = reinterpret_cast<DDLState*(__fastcall*)(int fileType)>(ProcessBase + 0x221A710);
const static auto Storage_Write = reinterpret_cast<bool(__fastcall*)(int a1, int a2, __int64 a3)>(ProcessBase + 0x221B4F0);
const static auto StringTable_GetAsset = reinterpret_cast<void(__fastcall*)(const char* filename, StringTable * *tablePtr)>(ProcessBase + 0x2251F70);
const static auto StringTable_GetColumnValueForRow = reinterpret_cast<const char* (__fastcall*)(StringTable * table, __int32 row, __int32 column)>(ProcessBase + 0x2252200);
const static auto StringTable_Lookup = reinterpret_cast<const char* (__fastcall*)(StringTable * table, int comparisonColumn, const char* value, int valueColumn)>(ProcessBase + 0x2252390);
const static auto Sys_GetTLS = reinterpret_cast<__int64(__fastcall*)()>(ProcessBase + 0x212B3D0);
const static auto UI_Interface_DrawText = reinterpret_cast<void(__fastcall*)(unsigned int localClientNum, __int64* luiElement, float xPos, float yPos, unsigned int R, unsigned int G,  unsigned int B, unsigned int A, char flags, char* text, __int64 font, float fontHeight, float wrapWidth, float alignment, char luaVM, QWORD * element)>(ProcessBase + 0x1F28860);
const static auto UI_IsRenderingImmediately = reinterpret_cast<bool(__cdecl*)()>(ProcessBase + 0x268CC60);
const static auto UI_SafeTranslateString = reinterpret_cast<const char* (__fastcall*)(const char* String)>(ProcessBase + 0x22328F0);
const static auto UserHasLicenseForApp = reinterpret_cast<char(__fastcall*)(__int64 mapInfo, __int64* userObj)>(ProcessBase + 0x1EAAD60);
const static auto dwInstantDispatchMessage = reinterpret_cast<__int64(__fastcall*)(__int64 SenderId, unsigned int controllerIndex, __int64 msg, unsigned int messageSize)>(ProcessBase + 0x143A620);

// Extern array and vector declarations
extern const char* cpmapnames[11];
extern const char* KeybindNames[17];
extern const char* offhandMenuNames[9];
extern const char* offhandSlots[8];
extern const char* primaryMenuNames[11];
extern const char* primarySlots[10];
extern const char* secondaryMenuNames[11];
extern const char* secondarySlots[10];
extern const char* slots[96];
extern const char* wildcardMenuNames[10];
extern const char* wildcardSlots[9];
extern const char* zmmapnames[14];

extern int Keybinds[17];
extern int offhandSelectedItems[8];
extern int offhandSelectedSlot[8];
extern int primarySelectedItems[10];
extern int primarySelectedSlot[10];
extern int secondarySelectedItems[10];
extern int secondarySelectedSlot[10];
extern int wildcardSelectedItems[9];
extern int wildcardSelectedSlot[9];

extern std::string offhandPreviews[8];
extern std::string primaryPreviews[10];
extern std::string secondaryPreviews[10];
extern std::string wildcardPreviews[9];

extern std::vector<std::string> legit_packets;
extern std::vector<bullet_tracer> tracers;
