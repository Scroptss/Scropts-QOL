#pragma once
#include "includes.h"

#define Hook(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourAttach((LPVOID*)&original, (LPVOID)hook), DetourTransactionCommit())
#define Unhook(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourDetach((LPVOID*)&original, (LPVOID)hook), DetourTransactionCommit())

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
extern uint64_t PlayerXUID;
extern char spoofName[32];
extern int slotItem;
extern int class_sel;
extern std::string nameBuffer;
extern HMODULE dllbase;
extern bool bMatchLoaded;
extern bool killspam;
extern std::string killfeed_input;
extern std::string chat_input;

typedef bool(__fastcall* LiveStats_SetShowcaseWeaponT)(unsigned int sessionMode, unsigned int statsLocation, DDLContext* ddlContext, unsigned int cacType, unsigned int characterIndex, Variant showcaseWeapon);
typedef bool(__fastcall* LiveStats_SetCharacterHeadIndexT)(unsigned int sessionMode, unsigned int statsLocation, DDLContext* ddlContext, unsigned int cacType, unsigned int characterHeadIndex);
typedef bool(__fastcall* LootT)(int controllerIndex, __int64 index, unsigned int incAmount);
typedef void(__fastcall* GameSendServerCommandT)(int clientNum, int reliable, const char* command);
typedef char* (__fastcall* tGetPersonaName) (DWORD_PTR _this);
typedef __int64(__fastcall* tLiveSteam_FilterPersonaName)(char* buffer, int size, char asciionly);

extern tGetPersonaName oGetPersonaName;
extern tLiveSteam_FilterPersonaName oLiveSteam_FilterPersonaName;
extern LiveStats_SetShowcaseWeaponT LiveStats_SetShowcaseWeapon;
extern LiveStats_SetCharacterHeadIndexT LiveStats_SetCharacterHeadIndex;
extern LootT GiveLootToSelf;
extern GameSendServerCommandT SV_GameSendServerCommand;
extern DWORD_PTR pGetPersonaNameReturn;
extern DWORD_PTR ISteamFriends;
extern DWORD_PTR pUserData;
extern DWORD_PTR pNameBuffer;
using ControllerIndex_t = std::uint32_t;

const static auto Cg_Obituary = reinterpret_cast<__int64(__fastcall*)(int a1, unsigned char* killer, char a3, __int64 killed, char a5, __int64 a6, float a7, float a8, bool a9)>(ProcessBase + 0x1337360);
const static auto Steam_GetServerTable = reinterpret_cast<__int64(__fastcall*)()>(ProcessBase + 0x1EB82D0);
const static auto RegisterMaterial = reinterpret_cast<__int64(__fastcall*)(int a1, const char* a2, __int64 a3)>(ProcessBase + 0xAA060);
const static auto UI_Model_GetString = reinterpret_cast<const char*(__fastcall*)(unsigned __int16 a1)>(ProcessBase + 0x20194A0);
const static auto LiveUser_GetXuid = reinterpret_cast<int64_t(__fastcall*)(unsigned int ControllerIndex)>(ProcessBase + 0x1EC7310);
const static auto CG_BoldGameMessageCenter = reinterpret_cast<std::uintptr_t(__fastcall*)(int clientNum, const char* msg)>(ProcessBase + 0x8C4C80);
const static auto Com_IsInGame = reinterpret_cast<bool(*)()>(ProcessBase + 0x21482C0);
const static auto Live_IsUserInGame = reinterpret_cast<bool(*)(const int controllerIndex)>(ProcessBase + 0x1E0D750);
const static auto CL_IsLocalClientInGame = reinterpret_cast<std::uintptr_t(__fastcall*)(int localClientNum)>(ProcessBase + 0x13598E0);
const static auto Game_Message = reinterpret_cast<std::uintptr_t(__fastcall*)(int clientNum, const char* msg)>(ProcessBase + 0x8CB400);
const static auto Cbuf_AddText = reinterpret_cast<std::uintptr_t(__fastcall*)(int, const char*)>(ProcessBase + 0x20EC010);
const static auto Live_AreWeHost = reinterpret_cast<bool(*)()>(ProcessBase + 0x1E25DC0);
const static auto Loot_BuyCrate = reinterpret_cast<bool(*)(int ControllerIndex, int CrateType, unsigned int CurrencyType)>(ProcessBase + 0x1E82480);
const static auto Loot_SpendVials = reinterpret_cast<bool(*)(unsigned int ControllerIndex, int VialNum)>(ProcessBase + 0x1E83860);
const static auto LiveStats_SetClanTagText = reinterpret_cast<std::uintptr_t(__fastcall*)(int, const char*)>(ProcessBase + 0x1EA5480);
const static auto dwInstantDispatchMessage = reinterpret_cast<__int64(__fastcall*)(__int64 SenderId, unsigned int controllerIndex, __int64 msg, unsigned int messageSize)>(ProcessBase + 0x143A600);
const static auto LobbyMsg_HandleIM = reinterpret_cast<std::uintptr_t(__fastcall*)(unsigned int targetController, __int64 senderXuid, void* buff, int len)>(ProcessBase + 0x1EF63E0);
const static auto Dvar_SetFromString = reinterpret_cast<std::uintptr_t(__fastcall*)(const char* dvarname, const char* value, bool createifmissing)>(ProcessBase + 0x22C7500);
const static auto SetCharacterIndex = reinterpret_cast<bool(__fastcall*)(__int64 ControllerIndex, __int64 eModes, unsigned int CharacterIndex)>(ProcessBase + 0x19E1F0); 
const static auto Name_Thing = reinterpret_cast<int(__fastcall*)(int* a1, __int64 a2, unsigned __int64 a3)>(ProcessBase + 0x2C3DE60);
const static auto Steam_GetName_Thing = reinterpret_cast<UINT8(__fastcall*)(UINT8 * a1, int a2, char a3)>(ProcessBase + 0x1EB0D30);
const static auto MSG_InitReadOnly = reinterpret_cast<void(*)(msg_t*, const char*, int)>(ProcessBase + 0x2155010);
const static auto MSG_BeginReading = reinterpret_cast<void(*)(msg_t*)>(ProcessBase + 0x2154D00);
const static auto MSG_ReadByte = reinterpret_cast<std::uint8_t(*)(msg_t*)>(ProcessBase + 0x2155450);
const static auto MSG_ReadData = reinterpret_cast<void(*)(msg_t*, void*, int)>(ProcessBase + 0x21554B0);
const static auto MSG_InfoResponse = reinterpret_cast<bool(*)(void*, LobbyMsg*)>(ProcessBase + 0x1EE1E60);
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
const static auto LobbyMsgRW_PackageElement2 = reinterpret_cast<bool(__fastcall*)(void* lobbyMsg, bool addElement)>(ProcessBase + 0x1EF65C2);
const static auto LobbyMsgRW_PrintKeyTypeAndValue = reinterpret_cast<__int64(__fastcall*)(LobbyMsg * lobbyMsg, LobbyMsgElementType elementType)>(ProcessBase + 0x1EF6D20);
const static auto LobbyMsgRW_PrintDebugMessage = reinterpret_cast<__int64(__fastcall*)(LobbyMsg * lobbyMsg)>(ProcessBase + 0x1EF6B80);
const static auto LobbyMsgRW_PrintMessage = reinterpret_cast<void(__fastcall*)(LobbyMsg * lobbyMsg, bool forcePrint)>(ProcessBase + 0x1EF6E40);
const static auto Live_IsUserSignedInToDemonware = reinterpret_cast<bool(*)(const ControllerIndex_t)>(ProcessBase + 0x1E0D7A0);
const static auto Steam_GetGamertagByXuid = reinterpret_cast<const char* (__fastcall*)(__int64 Xuid)>(ProcessBase + 0x1EB5A20);
const static auto dwInstantSendMessage = reinterpret_cast<void* (*)(const ControllerIndex_t, const std::uint64_t*, std::uint32_t, std::uint8_t, const void*, unsigned int)>(ProcessBase + 0x143A810);

//CLASS EDITOR:
const static auto _Storage_GetDDLRootState_ = (ProcessBase + 0x2276B80);
const static auto _Storage_GetDDLContext_ = (ProcessBase + 0x2276AB0);
const static auto _DDL_MoveToPath_ = (ProcessBase + 0x2522470);
const static auto _DDL_SetUInt_ = (ProcessBase + 0x2522970);
const static auto Com_SessionMode_GetGameMode = reinterpret_cast<eGameModes(__cdecl*)()>(ProcessBase + 0x20F68B0);
const static auto Com_SessionMode_GetMode = reinterpret_cast<eModes(__cdecl*)()>(ProcessBase + 0x20F6D30);
const static auto LiveStats_Loadouts_GetCACTypeForMode = reinterpret_cast<CACType(__fastcall*)(__int64 sessionMode, __int64 gameMode)>(ProcessBase + 0x1EAF0B0);
const static auto LiveStats_Loadouts_GetCACRoot = reinterpret_cast<CACRoot*(__fastcall*)(CACRoot* CacRoot, int ControllerIndex, int CacType)>(ProcessBase + 0x1EAF090);
const static auto BG_UnlockablesGetLoadoutSlotFromString = reinterpret_cast<std::uintptr_t(__fastcall*)(const char*)>(ProcessBase + 0x26A7710);
const static auto SetClass = reinterpret_cast<std::uintptr_t(__fastcall*)(CACRoot* CacRoot, __int64 a2, __int64 a3, __int64 itemIndex)>(ProcessBase + 0x16C8A0);
const static auto SetGobblegum = reinterpret_cast<std::uintptr_t(__fastcall*)(CACRoot* CacRoot, __int64 packindex, __int64 buffIndex, __int64 itemIndex)>(ProcessBase + 0x26AD440);
const static auto LiveStats_Loadouts_Save = reinterpret_cast<bool(__fastcall*)(unsigned int ControllerIndex, CACType a2)>(ProcessBase + 0x1EAF500);
extern const char* slots[93];
extern const char* customclass[10];