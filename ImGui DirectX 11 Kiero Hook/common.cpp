#include "common.h"

char spoofName[32];
int slotItem;
int class_sel;
HMODULE dllbase;
bool bMatchLoaded;
bool killspam;
std::uintptr_t ProcessBase = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
std::string nameBuffer = "";
std::string killfeed_input = ""s;
std::string chat_input = ""s;
DWORD_PTR ISteamFriends = *(DWORD_PTR*)(ProcessBase + 0x10BBCBA0);
DWORD_PTR pGetPersonaNameReturn = ProcessBase + 0x1EB0DEC;
DWORD_PTR pUserData = ProcessBase + 0x14FB3420;
DWORD_PTR pNameBuffer = ProcessBase + 0x15E84638;

LiveStats_SetShowcaseWeaponT LiveStats_SetShowcaseWeapon = (LiveStats_SetShowcaseWeaponT)(ProcessBase + 0x1EAD1A0);
LiveStats_SetCharacterHeadIndexT LiveStats_SetCharacterHeadIndex = (LiveStats_SetCharacterHeadIndexT)(ProcessBase + 0x1EACD70);
LootT GiveLootToSelf = (LootT)(ProcessBase + 0x1E82C50);
GameSendServerCommandT SV_GameSendServerCommand = (GameSendServerCommandT)(ProcessBase + 0x224F580);
uint64_t PlayerXUID;
tGetPersonaName oGetPersonaName;
tLiveSteam_FilterPersonaName oLiveSteam_FilterPersonaName = (tLiveSteam_FilterPersonaName)(ProcessBase + 0x1EB02C0);

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
    "primarygrenadestatus1"
    "primarygrenadestatus2"
    "primarygrenadestatus3"
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
    "herogadget",
    "specialty1",
    "specialty2",
    "specialty3",
    "specialty4",
    "specialty5",
    "specialty6",
    "cybercom_tacrig1",
    "cybercom_tacrig2"
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