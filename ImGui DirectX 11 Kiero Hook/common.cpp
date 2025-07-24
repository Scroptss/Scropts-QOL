#include "common.h"

HMODULE dllbase;

char spoofName[32];

const char* BlackMarketGuaranteeBribes[19]{
		"GUARANTEE_SPECIALIST_THEME",
		"GUARANTEE_CALLING_CARD",
		"GUARANTEE_FIRST_PLACE_TAUNT",
		"GUARANTEE_SPECIALIST_GESTURE",
		"GUARANTEE_ATTACHMENT_VARIANT",
		"GUARANTEE_CALLING_CARD_2",
		"GUARANTEE_BATTERY_ITEM",
		"GUARANTEE_FIREBREAK_ITEM",
		"GUARANTEE_NOMAD_ITEM",
		"GUARANTEE_OUTRIDER_ITEM",
		"GUARANTEE_PROPHET_ITEM",
		"GUARANTEE_REAPER_ITEM",
		"GUARANTEE_RUIN_ITEM",
		"GUARANTEE_SERAPH_ITEM",
		"GUARANTEE_SPECTRE_ITEM",
		"GUARANTEE_THREE_SPECIALIST_OUTFIT",
		"GUARANTEE_THREE_TAUNTS_AND_GESTURES",
		"GUARANTEE_THREE_CALLING_CARDS",
		"GUARANTEE_THREE_ATTACHMENT_VARIANT",
};

const char* customclass[] = {

	"Custom Class 1",
	"Custom Class 2",
	"Custom Class 3",
	"Custom Class 4",
	"Custom Class 5",
	"Custom Class 6",
	"Custom Class 7",
	"Custom Class 8",
	"Custom Class 9",
	"Custom Class 10"

};

bool bLogE;
bool bFriendsOnly;
bool bUnlockDLC;
bool bUnlockFullGame;
bool bSpoofPurchases;
bool bSpoofBlackMarket;
bool bSpoofCurrency;
bool bSpoofSlots;
bool bFileshareDownloading;
bool bFileshareDownloadingRan;
bool bFirstGumFree = true;
bool bFirstGumFreeRan;
bool bUIRgb;
bool bColoredUI;

int iBlackmarketAmt = 999;
int iBribe;
int iComboBoxType = 0;

float flHue;
float flSpeed = 3.0f;

ImColor mainColor;
ImColor uiColor = { 1.0f,1.0f,1.0f,1.0f };

ImColor mainRgb()
{
	const float hue = fmodf(flHue * 0.1f, 1.0f); // Adjust the speed of color change by modifying the multiplier
	const ImVec4 color = ImColor::HSV(hue, 0.7f, 0.7f);
	return color;
}

std::uintptr_t ProcessBase = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
std::string nameBuffer = "";
std::string custom_title_buf = "";
std::string custom_desc_buf = "";
std::string custom_delete_buf = "";
DWORD_PTR ISteamFriends = *(DWORD_PTR*)(ProcessBase + 0x10BBCBA0);
DWORD_PTR iSteamGameServer = *(DWORD_PTR*)(ProcessBase + 0x1142E158);
DWORD_PTR iSteamApps = *(DWORD_PTR*)(ProcessBase + 0x10BBCBC0);
DWORD_PTR pGetPersonaNameReturn = ProcessBase + 0x1EB0DEC;
std::ofstream myfile;
LiveStats_GetRootDDLStateT LiveStats_GetRootDDLState = (LiveStats_GetRootDDLStateT)(ProcessBase + 0x1EA96D0);
GetSessionStateT GetSessionState = (GetSessionStateT)(ProcessBase + 0x20F6D30);
LiveStats_Core_GetDDLContextT LiveStats_Core_GetDDLContext = (LiveStats_Core_GetDDLContextT)(ProcessBase + 0x1EA9660);
LiveStats_Core_GetStableDDLContextT LiveStats_Core_GetStableDDLContext = (LiveStats_Core_GetStableDDLContextT)(ProcessBase + 0x1EA9730);
DDL_MoveToNameT DDL_MoveToName = (DDL_MoveToNameT)(ProcessBase + 0x2522460);
DDL_MoveToPathT DDL_MoveToPath = (DDL_MoveToPathT)(ProcessBase + 0x2522470);
DDL_Lookup_MoveToNameT DDL_Lookup_MoveToName = (DDL_Lookup_MoveToNameT)(ProcessBase + 0x1427A00);
DDL_SetUIntT DDL_SetUInt = (DDL_SetUIntT)(ProcessBase + 0x2522970);
DDL_GetUIntT DDL_GetUInt = (DDL_GetUIntT)(ProcessBase + 0x2521F30);
DDL_SetIntT DDL_SetInt = (DDL_SetUIntT)(ProcessBase + 0x2522770);
DDL_SetStringT DDL_SetString = (DDL_SetStringT)(ProcessBase + 0x25227F0);
LiveStorage_UploadStatsForControllerT LiveStorage_UploadStatsForController = (LiveStorage_UploadStatsForControllerT)(ProcessBase + 0x1EC0D30);
LiveStats_SetShowcaseWeaponT LiveStats_SetShowcaseWeapon = (LiveStats_SetShowcaseWeaponT)(ProcessBase + 0x1EAD1A0);
LiveStats_SetCharacterHeadIndexT LiveStats_SetCharacterHeadIndex = (LiveStats_SetCharacterHeadIndexT)(ProcessBase + 0x1EACD70);
lergstuffT lergstuff = (lergstuffT)(ProcessBase + 0x1C4D70);
send_p2p_packet_t send_p2p_packet = reinterpret_cast<send_p2p_packet_t>(OFFSET(0x1EB01C0));
uintptr_t spoofAddr = 0;
LootT GiveLootToSelf = (LootT)(ProcessBase + 0x1E82C50);
GameSendServerCommandT SV_GameSendServerCommand = (GameSendServerCommandT)(ProcessBase + 0x224F580);
live_presence_pack_t live_presence_pack = (live_presence_pack_t)(ProcessBase + 0x1E93CE0);



uint64_t PlayerXUID;
tGetPersonaName oGetPersonaName;
tLiveSteam_FilterPersonaName oLiveSteam_FilterPersonaName = (tLiveSteam_FilterPersonaName)(ProcessBase + 0x1EB02C0);

std::string primaryPreviews[10];
std::string secondaryPreviews[10];
std::string wildcardPreviews[9];
std::string offhandPreviews[8];

int primarySelectedItems[10];
int secondarySelectedItems[10];
int wildcardSelectedItems[9];
int offhandSelectedItems[8];

int primarySelectedSlot[10];
int secondarySelectedSlot[10];
int wildcardSelectedSlot[9];
int offhandSelectedSlot[8];

const char* primarySlots[] = {
    "primary",
    "primarycamo",
    "primaryattachment1",
    "primaryattachment2",
    "primaryattachment3",
    "primaryattachment4",
    "primaryattachment5",
    "primaryattachment6",
    "primaryattachment7",
    "primaryattachment8"
};

const char* secondarySlots[] = {
    "secondary",
    "secondarycamo",
    "secondaryattachment1",
    "secondaryattachment2",
    "secondaryattachment3",
    "secondaryattachment4",
    "secondaryattachment5",
    "secondaryattachment6",
    "secondaryattachment7",
    "secondaryattachment8"
};

const char* wildcardSlots[] = {
    "specialty1",
    "specialty2",
    "specialty3",
    "specialty4",
    "specialty5",
    "specialty6",
    "bonuscard1",
    "bonuscard2",
    "bonuscard3"
};
// Set primary grenade and grenadecount to 255 for -255 cac slots "Give extra item slots"
const char* offhandSlots[] = {
    "primarygadget",
    "secondarygadget",
    "primarygadgetattachment1",
    "primarygadgetattachment2",
    "primarygadgetattachment3",
    "secondarygadgetattachment1",
    "secondarygadgetattachment2",
    "secondarygadgetattachment3"
};

const char* primaryMenuNames[] = {
    "Primary Weapon",
    "Primary Camo",
    "Attachment 1",
    "Attachment 2",
    "Attachment 3",
    "Attachment 4",
    "Attachment 5",
    "Attachment 6",
    "Attachment 7",
    "Attachment 8",
    "Select"
};

const char* secondaryMenuNames[] = {
    "Secondary Weapon",
    "Secondary Camo",
    "Attachment 1",
    "Attachment 2",
    "Attachment 3",
    "Attachment 4",
    "Attachment 5",
    "Attachment 6",
    "Attachment 7",
    "Attachment 8",
    "Select"
};

const char* wildcardMenuNames[] = {
    "Perk 1",
    "Perk 2",
    "Perk 3",
    "Perk 4",
    "Perk 5",
    "Perk 6",
    "Wildcard 1",
    "Wildcard 2",
    "Wildcard 3",
    "Select"
};

const char* offhandMenuNames[] = {
    "Primary Offhand",
    "Secondary Offhand",
    "Primary Offhand Attachment 1",
    "Primary Offhand Attachment 2",
    "Primary Offhand Attachment 3",
    "Secondary Offhand Attachment 1",
    "Secondary Offhand Attachment 2",
    "Secondary Offhand Attachment 3",
    "Select"
};

const char* cpmapnames[] = { "cp_mi_eth_prologue", "cp_mi_zurich_newworld", "cp_mi_sing_blackstation",
"cp_mi_sing_biodomes", "cp_mi_sing_sgen", "cp_mi_sing_vengeance", "cp_mi_cairo_ramses", "cp_mi_cairo_infection",
"cp_mi_cairo_aquifer", "cp_mi_cairo_lotus","cp_mi_zurich_coalescence" };

const char* zmmapnames[] = { "zm_zod", "zm_factory", "zm_castle", "zm_island", "zm_stalingrad", "zm_genesis", "zm_asylum", "zm_cosmodrome", "zm_moon", "zm_prototype",
"zm_sumpf", "zm_theater", "zm_temple", "zm_tomb" };

const char* slots[] = {
    "primary",
    "primaryattachment1",
    "primaryattachment2",
    "primaryattachment3",
    "primaryattachment4",
    "primaryattachment5",
    "primaryattachment6",
    "primaryattachment7",
    "primaryattachment8",
    "primaryattachment1cosmeticvariant",
    "primaryattachment2cosmeticvariant",
    "primaryattachment3cosmeticvariant",
    "primaryattachment4cosmeticvariant",
    "primaryattachment5cosmeticvariant",
    "primaryattachment6cosmeticvariant",
    "primaryattachment7cosmeticvariant",
    "primaryattachment8cosmeticvariant",
    "primarycamo",
    "primaryreticle",
    "primaryreticlecolor",
    "primarylens",
    "primaryemblem",
    "primarytag",
    "primarypaintjobslot",
    "primarypaintjobindex",
    "primarygunsmithvariant",
    "secondary",
    "secondaryattachment1",
    "secondaryattachment2",
    "secondaryattachment3",
    "secondaryattachment4",
    "secondaryattachment5",
    "secondaryattachment6",
    "secondaryattachment7",
    "secondaryattachment8",
    "secondaryattachment1cosmeticvariant",
    "secondaryattachment2cosmeticvariant",
    "secondaryattachment3cosmeticvariant",
    "secondaryattachment4cosmeticvariant",
    "secondaryattachment5cosmeticvariant",
    "secondaryattachment6cosmeticvariant",
    "secondaryattachment7cosmeticvariant",
    "secondaryattachment8cosmeticvariant",
    "secondarycamo",
    "secondaryreticle",
    "secondaryreticlecolor",
    "secondarylens",
    "secondaryemblem",
    "secondarytag",
    "secondarypaintjobslot",
    "secondarypaintjobindex",
    "secondarygunsmithvariant",
    "knifecamo",
    "primarygrenade",
    "primarygrenadecount",
    "primarygrenadestatus1",
    "primarygrenadestatus2",
    "primarygrenadestatus3",
    "specialgrenade",
    "specialgrenadecount",
    "specialgrenadestatus1",
    "specialgrenadestatus2",
    "specialgrenadestatus3",
    "primarygadget",
    "primarygadgetattachment1",
    "primarygadgetattachment2",
    "primarygadgetattachment3",
    "secondarygadget",
    "secondarygadgetattachment1",
    "secondarygadgetattachment2",
    "secondarygadgetattachment3",
    "specialgadget",
    "specialgadgetattachment1",
    "specialgadgetattachment2",
    "specialgadgetattachment3",
    "specialty1",
    "specialty2",
    "specialty3",
    "specialty4",
    "specialty5",
    "specialty6",
    "cybercom_tacrig1",
    "cybercom_tacrig2",
    "heroweapon",
    "herogadget",
    "cybercore",
    "cybercom_ability1",
    "cybercom_ability2",
    "cybercom_ability3",
    "bonuscard1",
    "bonuscard2",
    "bonuscard3",
    "killStreak1",
    "killStreak2",
    "killStreak3",
    "killStreak4"
};

