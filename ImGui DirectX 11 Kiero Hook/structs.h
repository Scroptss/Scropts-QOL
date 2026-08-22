#pragma once
#include "includes.h"

using ClientNum_t = std::uint32_t;

enum HookReturn
{
	Success,
	ModuleNotFound,
	D3D11CreateDeviceAndSwapChainFailed
};

enum setDvar_e {
	GUARANTEE_SPECIALIST_THEME,
	GUARANTEE_CALLING_CARD,
	GURANTEE_FIRST_PLACE_TAUNT,
	GUARANTEE_SPECIALIST_GESTURE,
	GUARANTEE_ATTACHMENT_VARIANT,
	GUARANTEE_CALLING_CARD_2,
	GUARANTEE_BATTERY_ITEM,
	GUARANTEE_FIREBREAK_ITEM,
	GUARANTEE_NOMAD_ITEM,
	GUARANTEE_OUTRIDER_ITEM,
	GUARANTEE_PROPHET_ITEM,
	GUARANTEE_REAPER_ITEM,
	GUARANTEE_RUIN_ITEM,
	GUARANTEE_SERAPH_ITEM,
	GUARANTEE_SPECTRE_ITEM,
	GUARANTEE_THREE_SPECIALIST_OUTFIT,
	GUARANTEE_THREE_TAUNTS_AND_GESTURES,
	GUARANTEE_THREE_OF_A_KIND_BACKGROUND,
	GUARANTEE_THREE_ATTACHMENT_VARIANT,
	SUPPLY_DROP_TRIPLE_PLAY,
	SUPPLY_DROP_GRAND_SLAM,
	SUPPLY_DROP_NO_DUPE,
	SUPPLY_DROP_NO_DUPE_20,
	ZM_GOBBLEGUM_CHALLENGE,
	FILESHARE_ALLOWDOWNLOAD,
	FILESHARE_DISABLEDOWNLOAD,
	FORCEHOST_ENABLE,
	FORCEHOST_DISABLE,
	ZMHD_GOBBLEGUM_REWARDS,
	LIMITED_EVENT_M14,
	LIMITED_EVENT_HIVE_CAMO,
	LIMITED_EVENT_PERMAFROST_CAMO,
	LIMITED_EVENT_EMPIRE_CAMO,
	LIMITED_EVENT_CHERRY_FIZZ_CAMO,
	LIMITED_EVENT_INTO_THE_VOID_CAMO,
	LIMITED_EVENT_LUCK_OF_THE_IRISH_CAMO,
};

enum statsLocation
{
	STATS_LOCATION_NORMAL = 0x0,
	STATS_LOCATION_FORCE_NORMAL = 0x1,
	STATS_LOCATION_STABLE = 0x2,
	STATS_LOCATION_OTHERPLAYER = 0x3,
	STATS_LOCATION_COUNT = 0x4,
};

enum ControllerIndex_t
{
	INVALID_CONTROLLER_PORT = 0xFFFFFFFF,
	CONTROLLER_INDEX_FIRST = 0x0,
	CONTROLLER_INDEX_0 = 0x0,
	CONTROLLER_INDEX_1 = 0x1,
	CONTROLLER_INDEX_2 = 0x2,
	CONTROLLER_INDEX_3 = 0x3,
	CONTROLLER_INDEX_COUNT = 0x4,
};

enum LootResultType
{
	LOOT_RESULT_INVALID = 0x0,
	LOOT_RESULT_SUCCESS = 0x1,
	LOOT_RESULT_FAILURE_INSUFFICIENT_FUNDS = 0x2,
	LOOT_RESULT_FAILURE_BAD_RESPONE = 0x3,
	LOOT_RESULT_FAILURE = 0x4,
	LOOT_RESULT_COUNT = 0x5,
};


enum dwKeyArchiveCategory
{
	KEYARCHIVE_CATEGORY_NORMAL = 0,
	KEYARCHIVE_CATEGORY_TEAM = 1,
	KEYARCHIVE_CATEGORY_USER = 2,
	KEYARCHIVE_CATEGORY_LEAGUE = 3,
	KEYARCHIVE_CATEGORY_RESERVED = 4,
	KEYARCHIVE_CATEGORY_ENTITLEMENTS = 5,
	KEYARCHIVE_CATEGORY_COUNT = 6,
};

enum keyArchiveIndex
{
	KEYARCHIVE_INDEX_INVALID = 0xFFFFFFFF,
	KEYARCHIVE_NORMAL_INDEX_BEGIN = 0x0,
	KEYARCHIVE_NORMAL_INDEX_COUNT = 0x1,
	KEYARCHIVE_TEAM_INDEX_BEGIN = 0x64,
	KEYARCHIVE_TEAM_INDEX_COUNT = 0x65,
	KEYARCHIVE_USER_INDEX_BEGIN = 0xC8,
	KEYARCHIVE_USER_INDEX_COUNT = 0xC9,
	KEYARCHIVE_LEAGUE_INDEX_BEGIN = 0x12C,
	KEYARCHIVE_LEAGUE_INDEX_COUNT = 0x12D,
	KEYARCHIVE_RESERVED_INDEX_BEGIN = 0x190,
	KEYARCHIVE_RESERVED_INDEX_COUNT = 0x191,
	KEYARCHIVE_ENTITLEMENTS_INDEX_BEGIN = 0x1F4,
	KEYARCHIVE_ENTITLEMENTS_ZM_DOUBLEXP_INDEX_GRANTED = 0x1F5,
	KEYARCHIVE_ENTITLEMENTS_ZM_DOUBLEXP_INDEX_CONSUMED = 0x1F6,
	KEYARCHIVE_ENTITLEMENTS_ZM_DOUBLEXP_INDEX_DATE = 0x1F7,
	KEYARCHIVE_ENTITLEMENTS_ZM_DOUBLEXP_INDEX_DAILY = 0x1F8,
	KEYARCHIVE_ENTITLEMENTS_ZM_PROMO_CALLING_CARD = 0x1F9,
	KEYARCHIVE_ENTITLEMENTS_NUKETOWN_PACK = 0x1FA,
	KEYARCHIVE_ENTITLEMENTS_AMAZON_CAMO = 0x1FB,
	KEYARCHIVE_ENTITLEMENTS_AMBASSADOR_CALLINGCARD = 0x1FC,
	KEYARCHIVE_ENTITLEMENTS_UNUSED_9 = 0x1FD,
	KEYARCHIVE_ENTITLEMENTS_WALMART_CALLINGCARD = 0x1FE,
	KEYARCHIVE_ENTITLEMENTS_CARLS_CAMO = 0x1FF,
	KEYARCHIVE_ENTITLEMENTS_CARLS_RETICLE = 0x200,
	KEYARCHIVE_ENTITLEMENTS_CARLS_CALLINGCARD = 0x201,
	KEYARCHIVE_ENTITLEMENTS_CARLS_PACK = 0x202,
	KEYARCHIVE_ENTITLEMENTS_COD_ACCOUNT_CALLINGCARD = 0x203,
	KEYARCHIVE_ENTITLEMENTS_CODE_MTX_PACK = 0x204,
	KEYARCHIVE_ENTITLEMENTS_BETA_CALLINGCARD = 0x205,
	KEYARCHIVE_ENTITLEMENTS_LOYALTY_RETICLES = 0x206,
	KEYARCHIVE_ENTITLEMENTS_LOYALTY_CAMO = 0x207,
	KEYARCHIVE_ENTITLEMENTS_LOYALTY_CALLINGCARD = 0x208,
	KEYARCHIVE_ENTITLEMENTS_LOYALTY_PACK = 0x209,
	KEYARCHIVE_ENTITLEMENTS_LOYALTY_LAST_LB_LOOKUP = 0x20A,
	KEYARCHIVE_ENTITLEMENTS_RETAIL_INCENTIVE = 0x20B,
	KEYARCHIVE_ENTITLEMENTS_CODXP_PACK = 0x20C,
	KEYARCHIVE_ENTITLEMENTS_INDEX_COUNT = 0x20D,
};

enum presence_activity_e
{
	presence_activity_menu_first = 0x0,
	presence_activity_online_not_in_title = 0x1,
	presence_activity_menu_inlobby = 0x2,
	presence_activity_menu_looking_for_party = 0x3,
	presence_activity_menu_looking_for_players = 0x4,
	presence_activity_menu_local = 0x5,
	presence_activity_menu_start = 0x6,
	presence_activity_in_title = 0x7,
	presence_activity_not_in_title = 0x8,
	presence_activity_menu_last = 0xF,
	presence_activity_cp_first = 0x10,
	presence_activity_cp_playing_coop_on_map = 0x11,
	presence_activity_cp_playing_solo_on_map = 0x12,
	presence_activity_cp_playing_coop_in_safehouse = 0x13,
	presence_activity_cp_playing_solo_in_safehouse = 0x14,
	presence_activity_cp_in_lobby = 0x15,
	presence_activity_cp_watching_film = 0x16,
	presence_activity_cp_looking_for_party = 0x17,
	presence_activity_cp_looking_for_players = 0x18,
	presence_activity_cp_playing_local = 0x19,
	presence_activity_cp_last = 0x1F,
	presence_activity_mp_first = 0x20,
	presence_activity_mp_playing_gmode_on_map = 0x21,
	presence_activity_mp_in_lobby = 0x22,
	presence_activity_mp_watching_film = 0x23,
	presence_activity_mp_looking_for_party = 0x24,
	presence_activity_mp_looking_for_players = 0x25,
	presence_activity_mp_playing_local = 0x26,
	presence_activity_mp_last = 0x2F,
	presence_activity_zm_first = 0x30,
	presence_activity_zm_playing_map_on_round = 0x31,
	presence_activity_zm_in_lobby = 0x32,
	presence_activity_zm_watching_film = 0x33,
	presence_activity_zm_looking_for_party = 0x34,
	presence_activity_zm_looking_for_players = 0x35,
	presence_activity_zm_playing_local = 0x36,
	presence_activity_zm_last = 0x3F,
	presence_activity_last = 0x40,
};

enum presence_ctx_e
{
	presence_ctx_menu_first = 0x0,
	presence_ctx_unknown = 0x0,
	presence_ctx_menu_mode_select = 0x1,
	presence_ctx_menu_start = 0x2,
	presence_ctx_menu_not_active = 0x3,
	presence_ctx_menu_last = 0xF,
	presence_ctx_cp_first = 0x10,
	presence_ctx_cp_main_menu = 0x11,
	presence_ctx_cp_private_game = 0x12,
	presence_ctx_cp_public_match = 0x13,
	presence_ctx_cp_theater = 0x14,
	presence_ctx_cp_last = 0x1F,
	presence_ctx_mp_first = 0x20,
	presence_ctx_mp_main_menu = 0x21,
	presence_ctx_mp_custom_games = 0x22,
	presence_ctx_mp_public_match = 0x23,
	presence_ctx_mp_theater = 0x24,
	presence_ctx_mp_last = 0x2F,
	presence_ctx_zm_first = 0x30,
	presence_ctx_zm_main_menu = 0x31,
	presence_ctx_zm_private_game = 0x32,
	presence_ctx_zm_public_match = 0x33,
	presence_ctx_zm_theater = 0x34,
	presence_ctx_zm_last = 0x3F,
	presence_ctx_last = 0x40,
};

enum lobby_joinable_e
{
	lobby_joinable_no_not_in_lobby = 0x0,
	lobby_joinable_yes = 0x1,
	lobby_joinable_yes_friends_only = 0x2,
	lobby_joinable_no_party_privacy_invite_only = 0x3,
	lobby_joinable_no_party_privacy_closed = 0x4,
	lobby_joinable_no_party_full = 0x5,
	lobby_joinable_no_game_full = 0x6,
	lobby_joinable_no_playing_local = 0x7,
	lobby_joinable_no_game_paused = 0x8,
	lobby_joinable_no = 0x9,
	lobby_joinable_no_you_need_dlc = 0xA,
	lobby_joinable_count = 0xB,
};
enum presence_primary_e
{
	presence_primary_unknown = 0xFFFFFFFF,
	presence_primary_online = 0x0,
	presence_primary_local = 0x1,
	presence_primary_title = 0x2,
	presence_primary_away = 0x3,
	presence_primary_blocked = 0x4,
};
/*
//=====================================================================================
*/
struct presence_platform_data_s
{
	presence_primary_e primary_presence;
	char title_id[32];
	char title_name[64];
	char title_status[64];
};

struct presence_title_data_party_member_s
{
	char gamertag[17];
};

struct presence_title_data_party_s
{
	int max;
	int total_count;
	int available_count;
	presence_title_data_party_member_s members[18];
};

struct presence_title_data_s
{
	presence_activity_e activity;
	presence_ctx_e ctx;
	lobby_joinable_e joinable;
	int game_type_id;
	int map_id;
	int difficulty;
	int playlist;
	int startup_timestamp;
	presence_title_data_party_s party;
};

struct presence_data_s
{
	int version;
	int flags;
	const unsigned __int64 xuid;
	bool is_dirty;
	bool is_initialzied;
	int id;
	const char* base;
	const char* params;
	const char* data;
	int failure_count;
	int last_update_time;
	int state;
	presence_title_data_s title;
	presence_platform_data_s platform;
};

struct vec2_t {
	int x;
	int y;
};



enum bdArchiveUpdateType
{
	BD_REPLACE = 0x0,
	BD_ADD = 0x1,
	BD_MAX = 0x2,
	BD_MIN = 0x3,
	BD_AND = 0x4,
	BD_OR = 0x5,
	BD_XOR = 0x6,
	BD_SUB_SAFE = 0x7,
};

enum ClassSetType_t
{
	CLASS_SET_TYPE_INVALID = 0xFFFFFFFF,
	CLASS_SET_TYPE_MP_PUBLIC = 0x0,
	CLASS_SET_TYPE_MP_CUSTOM = 0x1,
	CLASS_SET_TYPE_MP_ARENA = 0x2,
	CLASS_SET_TYPE_COUNT = 0x3,
};


enum playerStatsKeyIndex_t
{
	MP_PLAYERSTATSKEY_RANKXP = 0x0,
	MP_PLAYERSTATSKEY_PLEVEL = 0x1,
	MP_PLAYERSTATSKEY_RANK = 0x2,
	MP_PLAYERSTATSKEY_PARAGONRANKXP = 0x3,
	MP_PLAYERSTATSKEY_PARAGONRANK = 0x4,
	MP_PLAYERSTATSKEY_PARAGONICONID = 0x5,
	MP_PLAYERSTATSKEY_SCORE = 0x6,
	MP_PLAYERSTATSKEY_TIMEPLAYEDTOTAL = 0x7,
	MP_PLAYERSTATSKEY_GAMETYPEBAN = 0x8,
	MP_PLAYERSTATSKEY_TIMEWHENNEXTHOST = 0x9,
	MP_PLAYERSTATSKEY_BADHOSTCOUNT = 0xA,
	MP_PLAYERSTATSKEY_LEADERBOARDFAILURES = 0xB,
	MP_PLAYERSTATSKEY_LASTSTATSBACKUP = 0xC,
	MP_PLAYERSTATSKEY_MAPPACKMASK = 0xD,
	MP_PLAYERSTATSKEY_STATSBACKUPVERSION = 0xE,
	MP_PLAYERSTATSKEY_CODPOINTS = 0xF,
	MP_PLAYERSTATSKEY_CURRENCYSPENT = 0x10,
	MP_PLAYERSTATSKEY_STATS_VERSION = 0x11,
	MP_PLAYERSTATSKEY_KILLS = 0x12,
	MP_PLAYERSTATSKEY_DEATHS = 0x13,
	MP_PLAYERSTATSKEY_DEATHSDURINGUSE = 0x14,
	MP_PLAYERSTATSKEY_HEADSHOTS = 0x15,
	MP_PLAYERSTATSKEY_KDRATIO = 0x16,
	MP_PLAYERSTATSKEY_HIGHEST_KDRATIO = 0x17,
	MP_PLAYERSTATSKEY_ACCURACY = 0x18,
	MP_PLAYERSTATSKEY_HIGHEST_ACCURACY = 0x19,
	MP_PLAYERSTATSKEY_SHOTS = 0x1A,
	MP_PLAYERSTATSKEY_HITS = 0x1B,
	MP_PLAYERSTATSKEY_TIMEUSED = 0x1C,
	MP_PLAYERSTATSKEY_USED = 0x1D,
	MP_PLAYERSTATSKEY_DESTROYED = 0x1E,
	MP_PLAYERSTATSKEY_GAMETYPE = 0x1F,
	MP_PLAYERSTATSKEY_WINS = 0x20,
	MP_PLAYERSTATSKEY_LOSSES = 0x21,
	MP_PLAYERSTATSKEY_TIES = 0x22,
	MP_PLAYERSTATSKEY_LIFETIME_EARNINGS = 0x23,
	MP_PLAYERSTATSKEY_LIFETIME_BUYIN = 0x24,
	MP_PLAYERSTATSKEY_CONTRACTS_PURCHASED = 0x25,
	MP_PLAYERSTATSKEY_CONTRACTS_COMPLETED = 0x26,
	MP_PLAYERSTATSKEY_LASTESCROW = 0x27,
	MP_PLAYERSTATSKEY_STATRESETCOUNT = 0x28,
	MP_PLAYERSTATSKEY_FILMS_SHOUTCASTED = 0x29,
	MP_PLAYERSTATSKEY_HIGHLIGHTS_CREATED = 0x2A,
	MP_PLAYERSTATSKEY_COUNT = 0x2B,
};


enum zombieStatsKeyIndex_t
{
	ZM_ZOMBIESTATSKEY_TIME_PLAYED_TOTAL = 0x0,
	ZM_ZOMBIESTATSKEY_BGB_TOKENS_GAINED = 0x1,
	ZM_ZOMBIESTATSKEY_BGB_TOKENS_USED = 0x2,
	ZM_ZOMBIESTATSKEY_WEIGHTED_DOWNS = 0x3,
	ZM_ZOMBIESTATSKEY_WEIGHTED_ROUNDS_PLAYED = 0x4,
	ZM_ZOMBIESTATSKEY_TIMESTAMPLASTDAY1 = 0x5,
	ZM_ZOMBIESTATSKEY_TIMESTAMPLASTDAY2 = 0x6,
	ZM_ZOMBIESTATSKEY_TIMESTAMPLASTDAY3 = 0x7,
	ZM_ZOMBIESTATSKEY_TIMESTAMPLASTDAY4 = 0x8,
	ZM_ZOMBIESTATSKEY_TIMESTAMPLASTDAY5 = 0x9,
	ZM_ZOMBIESTATSKEY_KILLS = 0xA,
	ZM_ZOMBIESTATSKEY_ROUNDS = 0xB,
	ZM_ZOMBIESTATSKEY_COMBINED_RANK = 0xC,
	ZM_ZOMBIESTATSKEY_WINS = 0xD,
	ZM_ZOMBIESTATSKEY_LOSSES = 0xE,
	ZM_ZOMBIESTATSKEY_COUNT = 0xF,
};

enum entityType_t
{
	ET_GENERAL = 0x0,
	ET_PLAYER = 0x1,
	ET_PLAYER_CORPSE = 0x2,
	ET_ITEM = 0x3,
	ET_MISSILE = 0x4,
	ET_PLAYER_INVISIBLE = 0x5,
	ET_SCRIPTMOVER = 0x6,
	ET_SOUND_BLEND = 0x7,
	ET_FX = 0x8,
	ET_LOOP_FX = 0x9,
	ET_PRIMARY_LIGHT = 0xA,
	ET_LENSFLARE = 0xB,
	ET_REFLECTION_PROBE = 0xC,
	ET_HELICOPTER = 0xD,
	ET_PLANE = 0xE,
	ET_VEHICLE = 0xF,
	ET_VEHICLE_SPAWNER = 0x10,
	ET_VEHICLE_CORPSE = 0x11,
	ET_ACTOR = 0x12,
	ET_ACTOR_SPAWNER = 0x13,
	ET_ACTOR_CORPSE = 0x14,
	ET_STREAMER_HINT = 0x15,
	ET_ZBARRIER = 0x16,
	ET_TRIGGER = 0x17,
	ET_EVENTS = 0x18,
};


enum ComboBoxClass {
	CBC_WEAPONTYPE, // 131 (mp_statstable.csv 2 - 133)
	CBC_ATTACHMENTS, // 45 (attachmenttable.csv 0 - 45)
	CBC_CAMOS, // 138 (attachmenttable.csv 46 - 184)
	CBC_RETICLES, // 40 (attachmenttable.csv 186 - 226)
	CBC_PERK, // 22 (mp_statstable.csv 135 - 157)
	CBC_BONUSCARD, // 9 (mp_statstable.csv 159 - 168)
	CBC_KILLSTREAK, // 28 (mp_statstable.csv 175 - 203)
	CBC_EXTRA, // 51 the rest of 204 - 255

};

#pragma pack(push, 1)
struct item_info_s
{
private:
	byte _0x0[0x18];
public:
	byte requiredLevel;
	byte requiredPrestige;
private:
	byte _0x26[0x1];
public:
	std::int32_t flags;
private:
	byte _0x1F[0xB9];
};
#pragma pack(pop)

struct item_infos_s
{
public:
	item_info_s itemInfo[256];
private:
	byte _0xD800[0x5D70];
};

#pragma pack(push, 8)
struct StringTableCell
{
	const char* string;
	std::int32_t hash;
};
#pragma pack(pop)

struct StringTable
{
	const char* name;
	std::int32_t columnCount;
	std::int32_t rowCount;
	StringTableCell* values;
	std::int16_t* cellIndex;
};

struct inventoryLootItem
{
public:
	std::uint32_t itemId;
	std::uint32_t quantity;
	std::uint32_t modTime;
	std::uint32_t expireTime;
	std::uint16_t collisionField;
private:
	byte _0x12[0x2];
};

struct player_inventory_data_s
{
public:
	inventoryLootItem lootItem[4000];
	std::int32_t itemCount;
private:
	byte _0x13884[0x4];
public:
	std::uint32_t itemFlags;
private:
	byte _0x1388C[0x41C];
};

struct materialName
{
	char prefix;
	char type;

	std::uint8_t width;
	std::uint8_t height;
	std::uint8_t length;

	std::string name;
};

struct DDLMember
{
	const char* name;
	int index;
	void* parent;
	int bitSize;
	int limitSize;
	int offset;
	int type;
	int externalIndex;
	unsigned int rangeLimit;
	unsigned int serverDelta;
	unsigned int clientDelta;
	int arraySize;
	int enumIndex;
	int permission;
};

struct DDLHash
{
	int hash;
	int index;
};


struct DDLHashTable
{
	DDLHash* list;
	int count;
	int max;
};

struct DDLEnum
{
	const char* name;
	int memberCount;
	const char** members;
	DDLHashTable hashTable;
};

struct DDLStruct
{
	const char* name;
	int bitSize;
	int memberCount;
	DDLMember* members;
	DDLHashTable hashTableUpper;
	DDLHashTable hashTableLower;
};

struct DDLDef
{
	char* name;
	uint16_t version;
	unsigned int checksum;
	byte flags;
	int bitSize;
	int byteSize;
	DDLStruct* structList;
	int structCount;
	DDLEnum* enumList;
	int enumCount;
	DDLDef* next;
	int headerBitSize;
	int headerByteSize;
	int reserveSize;
	int userFlagsSize;
	bool paddingUsed;
};

typedef void (*DDLWriteCB)(__int64*, void*);

struct DDLContext
{
	void* buff;
	int32_t len;
	const DDLDef* def;
	DDLWriteCB writeCB;
	void* userData;
};

struct DDLState
{
	bool isValid;
	int32_t offset;
	int32_t arrayIndex;
	DDLMember* member;
	const DDLDef* ddlDef;
};

struct CACRoot
{
	DDLContext* ddlContext;
	DDLState* rootState;
};

enum CustomizationStatsLocation
{
	CUSTOMIZATION_STATS_LOCATION_STATS = 0x0,
	CUSTOMIZATION_STATS_LOCATION_PUBLIC_PROFILE = 0x1,
	CUSTOMIZATION_STATS_LOCATION_COUNT = 0x2,
};

enum eModes {
	MODE_ZOMBIES = 0x0,
	MODE_MULTIPLAYER = 0x1,
	MODE_CAMPAIGN = 0x2,
	MODE_COUNT = 0x3,
	MODE_INVALID = 0xFFFFFFFF,
	MODE_FIRST = 0x0,
};

enum eGameModes
{
	MODE_GAME_MATCHMAKING_PLAYLIST = 0x0,
	MODE_GAME_MATCHMAKING_MANUAL = 0x1,
	MODE_GAME_DEFAULT = 0x2,
	MODE_GAME_LEAGUE = 0x3,
	MODE_GAME_FREERUN = 0x4,
	MODE_GAME_THEATER = 0x5,
	MODE_GAME_COUNT = 0x6,
	MODE_GAME_INVALID = 0x6,
};

enum CACType {

	CAC_TYPE_MP_PUBLIC = 0x0,
	CAC_TYPE_MP_CUSTOM = 0x1,
	CAC_TYPE_MP_ARENA = 0x2,
	CAC_TYPE_MP_OFFLINE = 0x3,
	CAC_TYPE_FR = 0x4,
	CAC_TYPE_FR_OFFLINE = 0x5,
	CAC_TYPE_CP_ONLINE = 0x6,
	CAC_TYPE_CP_OFFLINE = 0x7,
	CAC_TYPE_ZM_ONLINE = 0x8,
	CAC_TYPE_ZM_OFFLINE = 0x9,
	CAC_TYPE_COUNT = 0x0A,
	CAC_TYPE_INVALID = 0xFFFFFFFF,

};

enum IMType
{
	JOIN_REQUEST = 0x0,
	JOIN_REPLY = 0x1,
	INVITE = 0x2,
	UPDATE_INVITE_INFO = 0x3,
	NUM_INSTANT_MESSAGE_TYPES = 0x4,
};

struct JoinSessionMessage
{
	IMType mType;
	char pad[0x60];
};

struct Variant
{
	char variantName[17];
	int attachment[8];
	byte attachmentVariant[8];
	byte camoIndex;
	int paintjobSlot;
	int paintjobIndex;
	int weaponIndex;
	int variantIndex;
	int sortIndex;
};

enum netsrc_t
{
	NS_NULL = 0xFFFFFFFF,
	NS_CLIENT1 = 0x0,
	NS_CLIENT2 = 0x1,
	NS_CLIENT3 = 0x2,
	NS_CLIENT4 = 0x3,
	NS_SERVER = 0x4,
	NS_MAXCLIENTS = 0x4,
	NS_PACKET = 0x5,
};

enum MsgType
{
	MESSAGE_TYPE_NONE = 0xFFFFFFFF,
	MESSAGE_TYPE_INFO_REQUEST = 0x0,
	MESSAGE_TYPE_INFO_RESPONSE = 0x1,
	MESSAGE_TYPE_LOBBY_STATE_PRIVATE = 0x2,
	MESSAGE_TYPE_LOBBY_STATE_GAME = 0x3,
	MESSAGE_TYPE_LOBBY_STATE_GAMEPUBLIC = 0x4,
	MESSAGE_TYPE_LOBBY_STATE_GAMECUSTOM = 0x5,
	MESSAGE_TYPE_LOBBY_STATE_GAMETHEATER = 0x6,
	MESSAGE_TYPE_LOBBY_HOST_HEARTBEAT = 0x7,
	MESSAGE_TYPE_LOBBY_HOST_DISCONNECT = 0x8,
	MESSAGE_TYPE_LOBBY_HOST_DISCONNECT_CLIENT = 0x9,
	MESSAGE_TYPE_LOBBY_HOST_LEAVE_WITH_PARTY = 0xA,
	MESSAGE_TYPE_LOBBY_CLIENT_HEARTBEAT = 0xB,
	MESSAGE_TYPE_LOBBY_CLIENT_DISCONNECT = 0xC,
	MESSAGE_TYPE_LOBBY_CLIENT_RELIABLE_DATA = 0xD,
	MESSAGE_TYPE_LOBBY_CLIENT_CONTENT = 0xE,
	MESSAGE_TYPE_LOBBY_MODIFIED_STATS = 0xF,
	MESSAGE_TYPE_JOIN_LOBBY = 0x10,
	MESSAGE_TYPE_JOIN_RESPONSE = 0x11,
	MESSAGE_TYPE_JOIN_AGREEMENT_REQUEST = 0x12,
	MESSAGE_TYPE_JOIN_AGREEMENT_RESPONSE = 0x13,
	MESSAGE_TYPE_JOIN_COMPLETE = 0x14,
	MESSAGE_TYPE_JOIN_MEMBER_INFO = 0x15,
	MESSAGE_TYPE_SERVERLIST_INFO = 0x16,
	MESSAGE_TYPE_PEER_TO_PEER_CONNECTIVITY_TEST = 0x17,
	MESSAGE_TYPE_PEER_TO_PEER_INFO = 0x18,
	MESSAGE_TYPE_LOBBY_MIGRATE_TEST = 0x19,
	MESSAGE_TYPE_LOBBY_MIGRATE_ANNOUNCE_HOST = 0x1A,
	MESSAGE_TYPE_LOBBY_MIGRATE_START = 0x1B,
	MESSAGE_TYPE_INGAME_MIGRATE_TO = 0x1C,
	MESSAGE_TYPE_INGAME_MIGRATE_NEW_HOST = 0x1D,
	MESSAGE_TYPE_VOICE_PACKET = 0x1E,
	MESSAGE_TYPE_VOICE_RELAY_PACKET = 0x1F,
	MESSAGE_TYPE_DEMO_STATE = 0x20,
	MESSAGE_TYPE_COUNT = 0x21,
};

enum PackageType
{
	PACKAGE_TYPE_NONE = 0x0,
	PACKAGE_TYPE_WRITE = 0x1,
	PACKAGE_TYPE_READ = 0x2,
};

enum LobbyMsgElementType
{
	MESSAGE_ELEMENT_INT32 = 0x0,
	MESSAGE_ELEMENT_UINT32 = 0x1,
	MESSAGE_ELEMENT_INT16 = 0x2,
	MESSAGE_ELEMENT_UINT16 = 0x3,
	MESSAGE_ELEMENT_INT8 = 0x4,
	MESSAGE_ELEMENT_UINT8 = 0x5,
	MESSAGE_ELEMENT_INT64 = 0x6,
	MESSAGE_ELEMENT_UINT64 = 0x7,
	MESSAGE_ELEMENT_FLOAT = 0x8,
	MESSAGE_ELEMENT_XUID = 0x9,
	MESSAGE_ELEMENT_STRING = 0xA,
	MESSAGE_ELEMENT_GLOB = 0xB,
	MESSAGE_ELEMENT_ARRAY_BEGIN = 0xC,
	MESSAGE_ELEMENT_ARRAY_ELEMENT = 0xD,
	MESSAGE_ELEMENT_ARRAY_END = 0xE,
	MESSAGE_ELEMENT_DEBUG_START = 0xF,
	MESSAGE_ELEMENT_DEBUG_END = 0x10,
	MESSAGE_ELEMENT_COUNT = 0x11,
};

enum LobbyNetworkMode
{
	LOBBY_NETWORKMODE_INVALID = 0xFFFFFFFF,
	LOBBY_NETWORKMODE_UNKNOWN = 0x0,
	LOBBY_NETWORKMODE_LAN = 0x1,
	LOBBY_NETWORKMODE_LIVE = 0x2,
	LOBBY_NETWORKMODE_COUNT = 0x3,
	LOBBY_NETWORKMODE_LOCAL = 0x1,
};

enum LobbyMainMode
{
	LOBBY_MAINMODE_INVALID = 0xFFFFFFFF,
	LOBBY_MAINMODE_CP = 0x0,
	LOBBY_MAINMODE_MP = 0x1,
	LOBBY_MAINMODE_ZM = 0x2,
	LOBBY_MAINMODE_COUNT = 0x3,
};

struct msg_t
{
	int overflowed;
	int readOnly;
	char* data;
	char* splitData;
	int maxsize;
	int cursize;
	int splitSize;
	int readcount;
	int bit;
	int lastEntityRef;
	int flush;
	netsrc_t targetLocalNetID;
	MsgType type;
	char encodeFlags;
	PackageType packageType;

	//template<size_t buf_size>
	/*void init_lobby(char(&buf)[buf_size], const MsgType msg_type)
	{
		this->init(buf, buf_size);

		this->packageType = PACKAGE_TYPE_WRITE;
		this->type = msg_type;
		this->encodeFlags = 0;

		this->write<uint8_t>(MESSAGE_ELEMENT_UINT8);
		this->write<uint8_t>(msg_type);
		this->write<uint8_t>(MESSAGE_ELEMENT_STRING);
		const auto msg_name = LobbyTypes_GetMsgTypeName(static_cast<MsgType>(-1));
		this->write_data(msg_name);
	}*/

	void init(char* buffer, const size_t buf_size)
	{
		*this = {};

		data = buffer;
		maxsize = buf_size;
	}

	void write_data(const std::string& buffer)
	{
		return write_data(buffer.data(), buffer.size() + 1);
	}

	void write_data(const char* buffer, const size_t length)
	{
		const auto final_size = cursize + length;

		if (final_size > maxsize)
		{
			overflowed = 1;
		}
		else
		{
			std::memcpy(&data[cursize], buffer, length);
			cursize = final_size;
		}
	}

	void write_bits(int value, int bits)
	{
		if (static_cast<uint32_t>(bits) > 0x20)
			return;

		if (maxsize - cursize < 4)
		{
			overflowed = 1;
		}
		else
		{
			while (bits)
			{
				--bits;

				const auto b = bit & 7;

				if (!b)
				{
					bit = sizeof(uint64_t) * cursize;
					data[++cursize] = 0;
				}

				if ((value & 1) != 0)
					data[bit >> 3] |= 1 << b;

				++bit;
				value >>= 1;
			}
		}
	}

	template<typename T> void write(T value)
	{
		const auto final_size = cursize + sizeof(value);

		if (final_size > maxsize)
		{
			overflowed = 1;
		}
		else
		{
			*reinterpret_cast<T*>(&data[cursize]) = value;
			cursize = final_size;
		}
	}

	template<typename T> T read()
	{
		auto result = static_cast<T>(-1);

		if (readcount >= splitSize + cursize)
		{
			overflowed = 1;
		}
		else
		{
			if (readcount < cursize)
			{
				result = *reinterpret_cast<T*>(&data[readcount]);
			}
			else
			{
				if (!splitData)
					return result;

				result = *reinterpret_cast<T*>(&splitData[readcount - cursize]);
			}

			readcount += sizeof(T);
		}

		return result;
	}
};


struct LobbyMsg
{
	msg_t msg;
	MsgType msgType;
	PackageType packageType;
	char encodeFlags;
};

struct bdSecurityID
{
	std::uint64_t id;
};

#pragma pack(push, 1)
struct XNADDR
{
	char pad[0x1E];
	union
	{
		std::uint8_t ip[4];
		std::uint32_t inaddr;
	};

	std::uint16_t port;
	char pad2[0x1];
};
#pragma pack(pop)

struct SerializedAdr
{
	bool valid;
	XNADDR xnaddr;
};

struct LobbyParams
{
	LobbyNetworkMode networkMode;
	LobbyMainMode mainMode;
};

struct bdSecurityKey
{
	char ab[16];
};

struct InfoResponseLobby
{
	bool isValid;
	std::uint64_t hostXuid;
	char hostName[32];
	bdSecurityID secId;
	bdSecurityKey secKey;
	SerializedAdr serializedAdr;
	LobbyParams lobbyParams;
	char pad[0x28];
};

struct Msg_InfoResponse
{
	std::uint32_t nonce;
	int uiScreen;
	char natType;
	InfoResponseLobby lobby[2];
};

enum CharacterItemType
{
	CHARACTER_ITEM_TYPE_BODY = 0x0,
	CHARACTER_ITEM_TYPE_HELMET = 0x1,
	CHARACTER_ITEM_TYPE_COUNT = 0x2,
};

enum statsResetReason_t
{
	STATSRESET_FILE_NOT_FOUND = 0x0,
	STATSRESET_CFG_VERSION_MISMATCH = 0x1,
	STATSRESET_DDL_CONVERSION_FAILED = 0x2,
	STATSRESET_EXEC_RESET_COMMAND = 0x3,
	STATSRESET_CHEATER_DETECTED = 0x4,
	STATSRESET_NEW_OFFLINE_USER = 0x5,
	STATSRESET_PRESTIGE_RESETALL = 0x6,
};

enum netadrtype_t
{
	NA_BOT = 0x0,
	NA_BAD = 0x1,
	NA_LOOPBACK = 0x2,
	NA_RAWIP = 0x3,
	NA_IP = 0x4,
};

struct netadr_t
{
	union
	{
		std::uint8_t ip[4];
		std::uint32_t inaddr;
	};

	std::uint16_t port;
	netadrtype_t type;
	netsrc_t localNetID;
};

struct ReliableCommands
{
	int sequence;
	int acknowledge;
	char* commands[128];
	int commandBufferNext;
	char commandBuffer[16384];
};

struct clientConnection_t
{
	int qport;
	ClientNum_t clientNum;
	int lastPacketSentTime;
	int lastPacketTime;
	netadr_t serverAddress;
	int connectTime;
	int connectPacketCount;
	char serverMessage[256];
	int challenge;
	int checksumFeed;
	ReliableCommands reliableCommands;
	int serverMessageSequence;
	int serverCommandSequence;
	int lastExecutedServerCommand;
	char serverCommands[128][1024];
	bool isServerRestarting;
	bool areTexturesLoaded;
	bool waitForMovie;
	bool hostCompromised;
};

struct FileshareMetaInfo
{
	uint64_t originID;
	int createTime;
	char createTimeString[64];
	uint64_t authorXuid;
	char authorName[33];
	char name[64];
	bool isModifiedName;
	char description[64];
	bool isModifiedDescription;
	bool isPooled;
	int duration;
};

struct bullet_tracer
{
	float opacity;

	ImVec3 start;
	ImVec3 end;

	ImVec2 start_screen;
	ImVec2 end_screen;

	int radius;

	std::chrono::time_point<std::chrono::high_resolution_clock> time{};
};



enum loadoutClass_t : __int32
{
	CLASS_INVALID = 0xFFFFFFFF,
	CUSTOM_CLASS_1 = 0x0,
	CUSTOM_CLASS_FIRST = 0x0,
	CUSTOM_CLASS_2 = 0x1,
	CUSTOM_CLASS_3 = 0x2,
	CUSTOM_CLASS_4 = 0x3,
	CUSTOM_CLASS_5 = 0x4,
	BASIC_CUSTOM_CLASS_COUNT = 0x5,
	CUSTOM_CLASS_6 = 0x5,
	FIELD_OPS_CLASS_FIRST = 0x5,
	CUSTOM_CLASS_7 = 0x6,
	CUSTOM_CLASS_8 = 0x7,
	CUSTOM_CLASS_9 = 0x8,
	FIELD_OPS_CLASS_LAST = 0x8,
	CUSTOM_CLASS_10 = 0x9,
	CUSTOM_CLASS_COUNT = 0xA,
	DEFAULT_CLASS_FIRST = 0xA,
	DEFAULT_CLASS_SMG = 0xA,
	DEFAULT_CLASS_CQB = 0xB,
	DEFAULT_CLASS_ASSAULT = 0xC,
	DEFAULT_CLASS_LMG = 0xD,
	DEFAULT_CLASS_SNIPER = 0xE,
	DEFAULT_CLASS_LAST = 0xE,
	TOTAL_CLASS_COUNT = 0xF,
};


enum PresenceActivity : __int32
{
	PRESENCE_ACTIVITY_MENU_FIRST = 0x0,
	PRESENCE_ACTIVITY_UNKNOWN = 0x0,
	PRESENCE_ACTIVITY_OFFLINE = 0x0,
	PRESENCE_ACTIVITY_ONLINE_NOT_IN_TITLE = 0x1,
	PRESENCE_ACTIVITY_MENU_INLOBBY = 0x2,
	PRESENCE_ACTIVITY_MENU_LOOKING_FOR_PARTY = 0x3,
	PRESENCE_ACTIVITY_MENU_LOOKING_FOR_PLAYERS = 0x4,
	PRESENCE_ACTIVITY_MENU_LOCAL = 0x5,
	PRESENCE_ACTIVITY_MENU_START = 0x6,
	PRESENCE_ACTIVITY_IN_TITLE = 0x7,
	PRESENCE_ACTIVITY_NOT_IN_TITLE = 0x8,
	PRESENCE_ACTIVITY_MENU_LAST = 0xF,
	PRESENCE_ACTIVITY_CP_FIRST = 0x10,
	PRESENCE_ACTIVITY_CP_PLAYING_COOP_ON_MAP = 0x11,
	PRESENCE_ACTIVITY_CP_PLAYING_SOLO_ON_MAP = 0x12,
	PRESENCE_ACTIVITY_CP_PLAYING_COOP_IN_SAFEHOUSE = 0x13,
	PRESENCE_ACTIVITY_CP_PLAYING_SOLO_IN_SAFEHOUSE = 0x14,
	PRESENCE_ACTIVITY_CP_IN_LOBBY = 0x15,
	PRESENCE_ACTIVITY_CP_WATCHING_FILM = 0x16,
	PRESENCE_ACTIVITY_CP_LOOKING_FOR_PARTY = 0x17,
	PRESENCE_ACTIVITY_CP_LOOKING_FOR_PLAYERS = 0x18,
	PRESENCE_ACTIVITY_CP_PLAYING_LOCAL = 0x19,
	PRESENCE_ACTIVITY_CP_LAST = 0x1F,
	PRESENCE_ACTIVITY_MP_FIRST = 0x20,
	PRESENCE_ACTIVITY_MP_PLAYING_GMODE_ON_MAP = 0x21,
	PRESENCE_ACTIVITY_MP_IN_LOBBY = 0x22,
	PRESENCE_ACTIVITY_MP_WATCHING_FILM = 0x23,
	PRESENCE_ACTIVITY_MP_LOOKING_FOR_PARTY = 0x24,
	PRESENCE_ACTIVITY_MP_LOOKING_FOR_PLAYERS = 0x25,
	PRESENCE_ACTIVITY_MP_PLAYING_LOCAL = 0x26,
	PRESENCE_ACTIVITY_MP_LAST = 0x2F,
	PRESENCE_ACTIVITY_ZM_FIRST = 0x30,
	PRESENCE_ACTIVITY_ZM_PLAYING_MAP_ON_ROUND = 0x31,
	PRESENCE_ACTIVITY_ZM_IN_LOBBY = 0x32,
	PRESENCE_ACTIVITY_ZM_WATCHING_FILM = 0x33,
	PRESENCE_ACTIVITY_ZM_LOOKING_FOR_PARTY = 0x34,
	PRESENCE_ACTIVITY_ZM_LOOKING_FOR_PLAYERS = 0x35,
	PRESENCE_ACTIVITY_ZM_PLAYING_LOCAL = 0x36,
	PRESENCE_ACTIVITY_ZM_LAST = 0x3F,
	PRESENCE_ACTIVITY_LAST = 0x40,
};

enum PresenceCtx : __int32
{
	PRESENCE_CTX_MENU_FIRST = 0x0,
	PRESENCE_CTX_UNKNOWN = 0x0,
	PRESENCE_CTX_MENU_MODE_SELECT = 0x1,
	PRESENCE_CTX_MENU_START = 0x2,
	PRESENCE_CTX_MENU_NOT_ACTIVE = 0x3,
	PRESENCE_CTX_MENU_LAST = 0xF,
	PRESENCE_CTX_CP_FIRST = 0x10,
	PRESENCE_CTX_CP_MAIN_MENU = 0x11,
	PRESENCE_CTX_CP_PRIVATE_GAME = 0x12,
	PRESENCE_CTX_CP_PUBLIC_MATCH = 0x13,
	PRESENCE_CTX_CP_THEATER = 0x14,
	PRESENCE_CTX_CP_LAST = 0x1F,
	PRESENCE_CTX_MP_FIRST = 0x20,
	PRESENCE_CTX_MP_MAIN_MENU = 0x21,
	PRESENCE_CTX_MP_CUSTOM_GAMES = 0x22,
	PRESENCE_CTX_MP_PUBLIC_MATCH = 0x23,
	PRESENCE_CTX_MP_THEATER = 0x24,
	PRESENCE_CTX_MP_LAST = 0x2F,
	PRESENCE_CTX_ZM_FIRST = 0x30,
	PRESENCE_CTX_ZM_MAIN_MENU = 0x31,
	PRESENCE_CTX_ZM_PRIVATE_GAME = 0x32,
	PRESENCE_CTX_ZM_PUBLIC_MATCH = 0x33,
	PRESENCE_CTX_ZM_THEATER = 0x34,
	PRESENCE_CTX_ZM_LAST = 0x3F,
	PRESENCE_CTX_LAST = 0x40,
};

enum bdLobbyErrorCode
{
	BD_NO_ERROR = 0x0,
	BD_TOO_MANY_TASKS = 0x1,
	BD_NOT_CONNECTED = 0x2,
	BD_SEND_FAILED = 0x3,
	BD_HANDLE_TASK_FAILED = 0x4,
	BD_START_TASK_FAILED = 0x5,
	BD_RESULT_EXCEEDS_BUFFER_SIZE = 0x64,
	BD_ACCESS_DENIED = 0x65,
	BD_EXCEPTION_IN_DB = 0x66,
	BD_MALFORMED_TASK_HEADER = 0x67,
	BD_INVALID_ROW = 0x68,
	BD_EMPTY_ARG_LIST = 0x69,
	BD_PARAM_PARSE_ERROR = 0x6A,
	BD_PARAM_MISMATCHED_TYPE = 0x6B,
	BD_SERVICE_NOT_AVAILABLE = 0x6C,
	BD_CONNECTION_RESET = 0x6D,
	BD_INVALID_USER_ID = 0x6E,
	BD_LOBBY_PROTOCOL_VERSION_FAILURE = 0x6F,
	BD_LOBBY_INTERNAL_FAILURE = 0x70,
	BD_LOBBY_PROTOCOL_ERROR = 0x71,
	BD_LOBBY_FAILED_TO_DECODE_UTF8 = 0x72,
	BD_LOBBY_ASCII_EXPECTED = 0x73,
	BD_INVALID_CONTEXT = 0x74,
	BD_LOBBY_METHOD_DISABLED = 0x75,
	BD_ASYNCHRONOUS_ERROR = 0xC8,
	BD_STREAMING_COMPLETE = 0xC9,
	BD_MEMBER_NO_PROPOSAL = 0x12C,
	BD_TEAMNAME_ALREADY_EXISTS = 0x12D,
	BD_MAX_TEAM_MEMBERSHIPS_LIMITED = 0x12E,
	BD_MAX_TEAM_OWNERSHIPS_LIMITED = 0x12F,
	BD_NOT_A_TEAM_MEMBER = 0x130,
	BD_INVALID_TEAM_ID = 0x131,
	BD_INVALID_TEAM_NAME = 0x132,
	BD_NOT_A_TEAM_OWNER = 0x133,
	BD_NOT_AN_ADMIN_OR_OWNER = 0x134,
	BD_MEMBER_PROPOSAL_EXISTS = 0x135,
	BD_MEMBER_EXISTS = 0x136,
	BD_TEAM_FULL = 0x137,
	BD_VULGAR_TEAM_NAME = 0x138,
	BD_TEAM_USERID_BANNED = 0x139,
	BD_TEAM_EMPTY = 0x13A,
	BD_INVALID_TEAM_PROFILE_QUERY_ID = 0x13B,
	BD_TEAMNAME_TOO_SHORT = 0x13C,
	BD_UNIQUE_PROFILE_DATA_EXISTS_ALREADY = 0x13D,
	BD_APPLICATION_EXISTS = 0x13E,
	BD_APPLICATIONS_MAX_EXCEEDED = 0x13F,
	BD_PAGE_SIZE_LIMIT_EXCEEDED = 0x140,
	BD_APPLICATION_REJECTED_EXISTS = 0x141,
	BD_APPLICATION_WITHDRAWN_EXISTS = 0x142,
	BD_APPLICATION_DOES_NOT_EXIST = 0x143,
	BD_APPLICATION_INVALID_STATE = 0x144,
	BD_MEMBER_BAN_EXISTS = 0x145,
	BD_MEMBER_BAN_DOES_NOT_EXIST = 0x146,
	BD_OWNER_BAN_FORBIDDEN = 0x147,
	BD_INVALID_ACCOUNT_TYPE = 0x148,
	BD_CONFIGURED_ACCOUNT_TYPE_NOT_FOUND = 0x149,
	BD_OWNER_OF_NON_EMPTY_TEAM = 0x14A,
	BD_CANNOT_APPLY_TO_PIRIVATE_TEAM = 0x14B,
	BD_MEMBER_IS_OWNER = 0x15E,
	BD_AUTO_JOINING_DISABLED = 0x15F,
	BD_TEAM_SHOWCASE_DISABLED = 0x160,
	BD_INVALID_FILTER = 0x162,
	BD_INVALID_TEAM_LEADERBOARD = 0x163,
	BD_TEAM_LEADERBOARD_LOADING = 0x164,
	BD_TEAM_SHOWCASE_COUNT_EXCEEDED = 0x165,
	BD_USER_ALREADY_TEAM_OWNER = 0x16A,
	BD_INVALID_BASE_CONTEXT = 0x16C,
	BD_INVALID_LEADERBOARD_ID = 0x190,
	BD_INVALID_STATS_SET = 0x191,
	BD_EMPTY_STATS_SET_IGNORED = 0x193,
	BD_NO_DIRECT_ACCESS_TO_ARBITRATED_LBS = 0x194,
	BD_STATS_WRITE_PERMISSION_DENIED = 0x195,
	BD_STATS_WRITE_TYPE_DATA_TYPE_MISMATCH = 0x196,
	BD_NO_STATS_FOR_USER = 0x197,
	BD_INVALID_ACCESS_TO_UNRANKED_LB = 0x198,
	BD_INVALID_EXTERNAL_TITLE_ID = 0x199,
	BD_DIFFERENT_LEADERBOARD_SCHEMAS = 0x19A,
	BD_TOO_MANY_LEADERBOARDS_REQUESTED = 0x19B,
	BD_ENTITLEMENTS_ERROR = 0x19C,
	BD_ENTITLEMENTS_INVALID_TITLEID = 0x19D,
	BD_ENTITLEMENTS_INVALID_LEADERBOARDID = 0x19E,
	BD_ENTITLEMENTS_INVALID_GET_MODE_FOR_TITLE = 0x19F,
	BD_ENTITLEMENTS_URL_CONNECTION_ERROR = 0x1A0,
	BD_ENTITLEMENTS_CONFIG_ERROR = 0x1A1,
	BD_ENTITLEMENTS_NAMED_PARENT_ERROR = 0x1A2,
	BD_ENTITLEMENTS_NAMED_KEY_ERROR = 0x1A3,
	BD_TOO_MANY_ENTITY_IDS_REQUESTED = 0x1A4,
	BD_STATS_READ_FAILED = 0x1A5,
	BD_STATS_INVALID_WRITE_TO_VIRTUAL_LEADERBOARD = 0x1A6,
	BD_STATS_INVALID_WRITE_TYPE_TO_MULTIRANK_LB = 0x1A7,
	BD_STATS_INVALID_EXCLUDED_COLUMN = 0x1A8,
	BD_STATS_INVALID_INCLUDED_COLUMN = 0x1A9,
	BD_STATS_WRITE_NO_SCRIPT_MODULE = 0x1AA,
	BD_STATS_WRITE_SCRIPT_MODULE_ERROR = 0x1AB,
	BD_NON_READ_ON_READ_ONLY_LEADERBOARD = 0x1AC,
	BD_INVALID_TITLE_ID = 0x1F4,
	BD_DOWN_FOR_MAINTENANCE = 0x1F5,
	BD_MESSAGING_INVALID_MAIL_ID = 0x258,
	BD_SELF_BLOCK_NOT_ALLOWED = 0x259,
	BD_GLOBAL_MESSAGE_ACCESS_DENIED = 0x25A,
	BD_GLOBAL_MESSAGES_USER_LIMIT_EXCEEDED = 0x25B,
	BD_MESSAGING_SENDER_DOES_NOT_EXIST = 0x25C,
	BD_AUTH_NO_ERROR = 0x2BC,
	BD_AUTH_BAD_REQUEST = 0x2BD,
	BD_AUTH_SERVER_CONFIG_ERROR = 0x2BE,
	BD_AUTH_BAD_TITLE_ID = 0x2BF,
	BD_AUTH_BAD_ACCOUNT = 0x2C0,
	BD_AUTH_ILLEGAL_OPERATION = 0x2C1,
	BD_AUTH_INCORRECT_LICENSE_CODE = 0x2C2,
	BD_AUTH_CREATE_USERNAME_EXISTS = 0x2C3,
	BD_AUTH_CREATE_USERNAME_ILLEGAL = 0x2C4,
	BD_AUTH_CREATE_USERNAME_VULGAR = 0x2C5,
	BD_AUTH_CREATE_MAX_ACC_EXCEEDED = 0x2C6,
	BD_AUTH_MIGRATE_NOT_SUPPORTED = 0x2C7,
	BD_AUTH_TITLE_DISABLED = 0x2C8,
	BD_AUTH_ACCOUNT_EXPIRED = 0x2C9,
	BD_AUTH_ACCOUNT_LOCKED = 0x2CA,
	BD_AUTH_UNKNOWN_ERROR = 0x2CB,
	BD_AUTH_INCORRECT_PASSWORD = 0x2CC,
	BD_AUTH_IP_NOT_IN_ALLOWED_RANGE = 0x2CD,
	BD_AUTH_WII_TOKEN_VERIFICATION_FAILED = 0x2CE,
	BD_AUTH_WII_AUTHENTICATION_FAILED = 0x2CF,
	BD_AUTH_IP_KEY_LIMIT_REACHED = 0x2D0,
	BD_AUTH_INVALID_GSPID = 0x2D1,
	BD_AUTH_INVALID_IP_RANGE_ID = 0x2D2,
	BD_AUTH_3DS_TOKEN_VERIFICATION_FAILED = 0x2D1,
	BD_AUTH_3DS_AUTHENTICATION_FAILED = 0x2D2,
	BD_AUTH_STEAM_APP_ID_MISMATCH = 0x2D3,
	BD_AUTH_ABACCOUNTS_APP_ID_MISMATCH = 0x2D4,
	BD_AUTH_CODO_USERNAME_NOT_SET = 0x2D5,
	BD_AUTH_WIIU_TOKEN_VERIFICATION_FAILED = 0x2D6,
	BD_AUTH_WIIU_AUTHENTICATION_FAILED = 0x2D7,
	BD_AUTH_CODO_USERNAME_NOT_BASE64 = 0x2D8,
	BD_AUTH_CODO_USERNAME_NOT_UTF8 = 0x2D9,
	BD_AUTH_TENCENT_TICKET_EXPIRED = 0x2DA,
	BD_AUTH_PS3_SERVICE_ID_MISMATCH = 0x2DB,
	BD_AUTH_CODOID_NOT_WHITELISTED = 0x2DC,
	BD_AUTH_PLATFORM_TOKEN_RETRIEVAL_ERROR = 0x2DD,
	BD_AUTH_JSON_FORMAT_ERROR = 0x2DE,
	BD_AUTH_REPLY_CONTENT_ERROR = 0x2DF,
	BD_AUTH_PLATFORM_TOKEN_EXPIRED = 0x2E0,
	BD_AUTH_CONTINUING = 0x2E1,
	BD_AUTH_PLATFORM_TOKEN_DECRYPTION_ERROR = 0x2E2,
	BD_AUTH_PLATFORM_TOKEN_SIGNATURE_ERROR = 0x2E3,
	BD_AUTH_DNS_RESOLUTION_ERROR = 0x2E4,
	BD_AUTH_SSL_CERTIFICATE_ERROR = 0x2E5,
	BD_AUTH_SERVER_UNAVAILABLE_ERROR = 0x2E6,
	BD_AUTH_ENVIRONMENT_ERROR = 0x2E7,
	BD_AUTH_PLATFORM_DEVICE_ID_ERROR = 0x2E8,
	BD_AUTH_UNO_APP_ID_MISMATCH = 0x2E9,
	BD_AUTH_UNICODE_DECODE_ERROR = 0x2EA,
	BD_AUTH_STEAM_PUBLISHER_BAN = 0x2EB,
	BD_AUTH_TICKET_DECRYPTION_ERROR = 0x2EC,
	BD_AUTH_SIGNATURE_ERROR = 0x2ED,
	BD_AUTH_REQUEST_TIMEOUT_ERROR = 0x2EE,
	BD_AUTH_REQUEST_ABORTED_ERROR = 0x2EF,
	BD_AUTH_SINGLE_IDENTITY_FLOW_DISABLED_ERROR = 0x2F0,
	BD_AUTH_SINGLE_IDENTITY_TOKEN_MISSING_ERROR = 0x2F1,
	BD_AUTH_SINGLE_IDENTITY_TOKEN_EXPIRED_ERROR = 0x2F2,
	BD_AUTH_SINGLE_IDENTITY_TOKEN_INVALID_ERROR = 0x2F3,
	BD_NO_PROFILE_INFO_EXISTS = 0x320,
	BD_FRIENDSHIP_NOT_REQUSTED = 0x384,
	BD_NOT_A_FRIEND = 0x385,
	BD_SELF_FRIENDSHIP_NOT_ALLOWED = 0x387,
	BD_FRIENDSHIP_EXISTS = 0x388,
	BD_PENDING_FRIENDSHIP_EXISTS = 0x389,
	BD_USERID_BANNED = 0x38A,
	BD_FRIENDS_FULL = 0x38C,
	BD_FRIENDS_NO_RICH_PRESENCE = 0x38D,
	BD_RICH_PRESENCE_TOO_LARGE = 0x38E,
	BD_NO_FILE = 0x3E8,
	BD_PERMISSION_DENIED = 0x3E9,
	BD_FILESIZE_LIMIT_EXCEEDED = 0x3EA,
	BD_FILENAME_MAX_LENGTH_EXCEEDED = 0x3EB,
	BD_EXTERNAL_STORAGE_SERVICE_ERROR = 0x3EC,
	BD_VALIDATION_ERROR = 0x3ED,
	BD_VALIDATION_TOKEN_ERROR = 0x3EE,
	BD_VALIDATION_TOKEN_GENERATION_ERROR = 0x3EF,
	BD_VALIDATION_TOKEN_VERIFICATION_ERROR = 0x3F0,
	BD_STORAGE_SERVER_UNREACHABLE = 0x3F1,
	BD_CHANNEL_DOES_NOT_EXIST = 0x44D,
	BD_CHANNEL_ALREADY_SUBSCRIBED = 0x44E,
	BD_CHANNEL_NOT_SUBSCRIBED = 0x44F,
	BD_CHANNEL_FULL = 0x450,
	BD_CHANNEL_SUBSCRIPTIONS_FULL = 0x451,
	BD_CHANNEL_NO_SELF_WHISPERING = 0x452,
	BD_CHANNEL_ADMIN_REQUIRED = 0x453,
	BD_CHANNEL_TARGET_NOT_SUBSCRIBED = 0x454,
	BD_CHANNEL_REQUIRES_PASSWORD = 0x455,
	BD_CHANNEL_TARGET_IS_SELF = 0x456,
	BD_CHANNEL_PUBLIC_BAN_NOT_ALLOWED = 0x457,
	BD_CHANNEL_USER_BANNED = 0x458,
	BD_CHANNEL_PUBLIC_PASSWORD_NOT_ALLOWED = 0x459,
	BD_CHANNEL_PUBLIC_KICK_NOT_ALLOWED = 0x45A,
	BD_CHANNEL_MUTED = 0x45B,
	BD_CONTENT_UNLOCK_UNKNOWN_ERROR = 0x514,
	BD_UNLOCK_KEY_INVALID = 0x515,
	BD_UNLOCK_KEY_ALREADY_USED_UP = 0x516,
	BD_CONTENT_UNLOCK_LIMIT_REACHED = 0x517,
	BD_DIFFERENT_HARDWARE_ID = 0x518,
	BD_INVALID_CONTENT_OWNER = 0x519,
	BD_CONTENT_UNLOCK_INVALID_USER = 0x51A,
	BD_CONTENT_UNLOCK_INVALID_CATEGORY = 0x51B,
	BD_KEY_ARCHIVE_INVALID_WRITE_TYPE = 0x5DC,
	BD_KEY_ARCHIVE_EXCEEDED_MAX_IDS_PER_REQUEST = 0x5DD,
	BD_BANDWIDTH_TEST_TRY_AGAIN = 0x712,
	BD_BANDWIDTH_TEST_STILL_IN_PROGRESS = 0x713,
	BD_BANDWIDTH_TEST_NOT_PROGRESS = 0x714,
	BD_BANDWIDTH_TEST_SOCKET_ERROR = 0x715,
	BD_INVALID_SESSION_NONCE = 0x76D,
	BD_ARBITRATION_FAILURE = 0x76F,
	BD_ARBITRATION_USER_NOT_REGISTERED = 0x771,
	BD_ARBITRATION_NOT_CONFIGURED = 0x772,
	BD_CONTENTSTREAMING_FILE_NOT_AVAILABLE = 0x7D0,
	BD_CONTENTSTREAMING_STORAGE_SPACE_EXCEEDED = 0x7D1,
	BD_CONTENTSTREAMING_NUM_FILES_EXCEEDED = 0x7D2,
	BD_CONTENTSTREAMING_UPLOAD_BANDWIDTH_EXCEEDED = 0x7D3,
	BD_CONTENTSTREAMING_FILENAME_MAX_LENGTH_EXCEEDED = 0x7D4,
	BD_CONTENTSTREAMING_MAX_THUMB_DATA_SIZE_EXCEEDED = 0x7D5,
	BD_CONTENTSTREAMING_DOWNLOAD_BANDWIDTH_EXCEEDED = 0x7D6,
	BD_CONTENTSTREAMING_NOT_ENOUGH_DOWNLOAD_BUFFER_SPACE = 0x7D7,
	BD_CONTENTSTREAMING_SERVER_NOT_CONFIGURED = 0x7D8,
	BD_CONTENTSTREAMING_INVALID_APPLE_RECEIPT = 0x7DA,
	BD_CONTENTSTREAMING_APPLE_STORE_NOT_AVAILABLE = 0x7DB,
	BD_CONTENTSTREAMING_APPLE_RECEIPT_FILENAME_MISMATCH = 0x7DC,
	BD_CONTENTSTREAMING_BATCH_DOWNLOAD_PARTIAL_FAILURE = 0x7DD,
	BD_CONTENTSTREAMING_HTTP_ERROR = 0x7E4,
	BD_CONTENTSTREAMING_FAILED_TO_START_HTTP = 0x7E5,
	BD_CONTENTSTREAMING_LOCALE_INVALID = 0x7E6,
	BD_CONTENTSTREAMING_LOCALE_MISSING = 0x7E7,
	BD_VOTERANK_ERROR_EMPTY_RATING_SUBMISSION = 0x7EE,
	BD_VOTERANK_ERROR_MAX_VOTES_EXCEEDED = 0x7EF,
	BD_VOTERANK_ERROR_INVALID_RATING = 0x7F0,
	BD_MAX_NUM_TAGS_EXCEEDED = 0x82A,
	BD_TAGGED_COLLECTION_DOES_NOT_EXIST = 0x82B,
	BD_EMPTY_TAG_ARRAY = 0x82C,
	BD_SEARCH_SERVER_UNREACHABLE = 0x82F,
	BD_ENTITY_ID_NOT_SPECIFIED = 0x830,
	BD_INVALID_QUERY_ID = 0x834,
	BD_NO_ENTRY_TO_UPDATE = 0x835,
	BD_SESSION_INVITE_EXISTS = 0x836,
	BD_INVALID_SESSION_ID = 0x837,
	BD_ATTACHMENT_TOO_LARGE = 0x838,
	BD_INVALID_FIELD_VALUE = 0x839,
	BD_INVALID_GROUP_ID = 0xAF0,
	BD_MAIL_INVALID_MAIL_ID_ERROR = 0xB55,
	BD_MAIL_PERMISSION_DENIED_ERROR = 0xB56,
	BD_TWITCH_SERVICE_ERROR = 0xC1D,
	BD_TWITCH_ACCOUNT_ALREADY_LINKED = 0xC1E,
	BD_TWITCH_NO_LINKED_ACCOUNT = 0xC1F,
	BD_TWITTER_AUTH_ATTEMPT_FAILED = 0xDAD,
	BD_TWITTER_AUTH_TOKEN_INVALID = 0xDAE,
	BD_TWITTER_UPDATE_LIMIT_REACHED = 0xDAF,
	BD_TWITTER_UNAVAILABLE = 0xDB0,
	BD_TWITTER_ERROR = 0xDB1,
	BD_TWITTER_TIMED_OUT = 0xDB2,
	BD_TWITTER_ACCOUNT_AMBIGUOUS = 0xDB4,
	BD_TWITTER_MAXIMUM_ACCOUNTS_REACHED = 0xDB5,
	BD_TWITTER_ACCOUNT_NOT_REGISTERED = 0xDB6,
	BD_TWITTER_DUPLICATE_STATUS = 0xDB7,
	BD_TWITTER_ACCOUNT_ALREADY_REGISTERED = 0xE1C,
	BD_FACEBOOK_AUTH_ATTEMPT_FAILED = 0xE11,
	BD_FACEBOOK_AUTH_TOKEN_INVALID = 0xE12,
	BD_FACEBOOK_PHOTO_DOES_NOT_EXIST = 0xE13,
	BD_FACEBOOK_PHOTO_INVALID = 0xE14,
	BD_FACEBOOK_PHOTO_ALBUM_FULL = 0xE15,
	BD_FACEBOOK_UNAVAILABLE = 0xE16,
	BD_FACEBOOK_ERROR = 0xE17,
	BD_FACEBOOK_TIMED_OUT = 0xE18,
	BD_FACEBOOK_DISABLED_FOR_USER = 0xE19,
	BD_FACEBOOK_ACCOUNT_AMBIGUOUS = 0xE1A,
	BD_FACEBOOK_MAXIMUM_ACCOUNTS_REACHED = 0xE1B,
	BD_FACEBOOK_INVALID_NUM_PICTURES_REQUESTED = 0xE1C,
	BD_FACEBOOK_VIDEO_DOES_NOT_EXIST = 0xE1D,
	BD_FACEBOOK_ACCOUNT_ALREADY_REGISTERED = 0xE1E,
	BD_FACEBOOK_TARGET_OBJECT_ID_INVALID = 0xE1F,
	BD_FACEBOOK_NO_SUCH_ACCOUNT = 0xE20,
	BD_APNS_INVALID_PAYLOAD = 0xE74,
	BD_APNS_INVALID_TOKEN_LENGTH_ERROR = 0xE76,
	BD_MAX_CONSOLEID_LENGTH_EXCEEDED = 0xEE1,
	BD_MAX_WHITELIST_LENGTH_EXCEEDED = 0xEE2,
	BD_TOTP_CHALLENGE_FAILED = 0xEE3,
	BD_NO_TOTP_ACCOUNT = 0xEE4,
	BD_EXTENDED_AUTH_INFO_ERROR = 0xEE5,
	BD_EXTENDED_AUTH_INFO_LENGTH_EXCEEDED = 0xEE6,
	BD_EXTENDED_AUTH_INFO_EXPIRED = 0xEE7,
	BD_WHITELIST_TRAILING_WHITE_SPACE = 0xEE8,
	BD_USERGROUP_NAME_ALREADY_EXISTS = 0x1770,
	BD_INVALID_USERGROUP_ID = 0x1771,
	BD_USER_ALREADY_IN_USERGROUP = 0x1772,
	BD_USER_NOT_IN_USERGROUP = 0x1773,
	BD_INVALID_USERGROUP_MEMBER_TYPE = 0x1774,
	BD_TOO_MANY_MEMBERS_REQUESTED = 0x1775,
	BD_USERGROUP_NAME_TOO_SHORT = 0x1776,
	BD_RICH_PRESENCE_DATA_TOO_LARGE = 0x1A90,
	BD_RICH_PRESENCE_TOO_MANY_USERS = 0x1A91,
	BD_PRESENCE_DATA_TOO_LARGE = 0x283C,
	BD_PRESENCE_TOO_MANY_USERS = 0x283D,
	BD_USER_LOGGED_IN_OTHER_TITLE = 0x283E,
	BD_USER_NOT_LOGGED_IN = 0x283F,
	BD_PRESENCE_INVALID_PLATFORM = 0x2840,
	BD_SUBSCRIPTION_TOO_MANY_USERS = 0x1B58,
	BD_SUBSCRIPTION_TICKET_PARSE_ERROR = 0x1B59,
	BD_CODO_ID_INVALID_DATA = 0x1BBC,
	BD_INVALID_MESSAGE_FORMAT = 0x1BBD,
	BD_TLOG_TOO_MANY_MESSAGES = 0x1BBE,
	BD_CODO_ID_NOT_IN_WHITELIST = 0x1BBF,
	BD_TLOG_MESSAGE_TRANSFORMATION_ERROR = 0x1BC0,
	BD_REWARDS_NOT_ENABLED = 0x1BC1,
	BD_REWARDS_INVALID_RULE = 0x1BC2,
	BD_REDEEM_NAME_CHANGE_INTERNAL_ERROR = 0x1BC3,
	BD_REDEEM_NAME_CHANGE_INVALID_ITEM = 0x1BC4,
	BD_REDEEM_NAME_CHANGE_UNAVAILABLE_ITEM = 0x1BC5,
	BD_REDEEM_NAME_CHANGE_IN_PROGRESS = 0x1BC6,
	BD_REDEEM_NAME_CHANGE_INACTIVE_REQUEST = 0x1BC7,
	BD_REDEEM_NAME_CHANGE_INVALID_NAME = 0x1BC8,
	BD_REDEEM_NAME_CHANGE_SAME_NAME = 0x1BC9,
	BD_REDEEM_NAME_CHANGE_DUPLICATE_NAME = 0x1BCA,
	BD_TENCENT_NO_APPLICABLE_REWARDS = 0x1BCB,
	BD_TLOG_MESSAGE_DECOMPRESSION_ERROR = 0x1BCC,
	BD_REDEEM_TEAM_NAME_CHANGE_INTERNAL_ERROR = 0x1BCD,
	BD_REDEEM_TEAM_NAME_CHANGE_INVALID_ITEM = 0x1BCE,
	BD_REDEEM_TEAM_NAME_CHANGE_UNAVAILABLE_ITEM = 0x1BCF,
	BD_MARKETPLACE_ERROR = 0x1F40,
	BD_MARKETPLACE_RESOURCE_NOT_FOUND = 0x1F41,
	BD_MARKETPLACE_INVALID_CURRENCY = 0x1F42,
	BD_MARKETPLACE_INVALID_PARAMETER = 0x1F43,
	BD_MARKETPLACE_RESOURCE_CONFLICT = 0x1F44,
	BD_MARKETPLACE_STORAGE_ERROR = 0x1F45,
	BD_MARKETPLACE_INTEGRITY_ERROR = 0x1F46,
	BD_MARKETPLACE_INSUFFICIENT_FUNDS_ERROR = 0x1F47,
	BD_MARKETPLACE_MMP_SERVICE_ERROR = 0x1F48,
	BD_MARKETPLACE_PRECONDITION_REQUIRED = 0x1F49,
	BD_MARKETPLACE_ITEM_MULTIPLE_PURCHASE_ERROR = 0x1F4A,
	BD_MARKETPLACE_MISSING_REQUIRED_ENTITLEMENT = 0x1F4B,
	BD_MARKETPLACE_VALIDATION_ERROR = 0x1F4C,
	BD_MARKETPLACE_TENCENT_PAYMENT_ERROR = 0x1F4D,
	BD_MARKETPLACE_SKU_NOT_COUPON_ENABLED_ERROR = 0x1F4E,
	BD_MARKETPLACE_TRANSACTION_ERROR = 0x1F4F,
	BD_MARKETPLACE_RECEIPT_USED = 0x1F50,
	BD_MARKETPLACE_INVALID_RECEIPT = 0x1F51,
	BD_MARKETPLACE_STEAM_REQUEST_FAILED = 0x1F52,
	BD_MARKETPLACE_STEAM_NOT_APPROVED = 0x1F53,
	BD_MARKETPLACE_PRODUCT_NOT_FOUND_IN_FP = 0x1F54,
	BD_MARKETPLACE_STEAM_ALREADY_FINALIZED = 0x1F54,
	BD_MARKETPLACE_STEAM_BAD_ORDER_ID = 0x1F55,
	BD_MARKETPLACE_STEAM_CURRENCY_ERROR = 0x1F56,
	BD_MARKETPLACE_PSN_INVALID_NP_TITLE_ID = 0x1F65,
	BD_MARKETPLACE_PSN_INVALID_NP_AUTH_CODE = 0x1F57,
	BD_MARKETPLACE_PSN_INVALID_ENTITLEMENT_ID = 0x1F58,
	BD_MARKETPLACE_PSN_ENTITLEMENT_NOT_PRESENT = 0x1F59,
	BD_MARKETPLACE_PSN_INTERNAL_ERROR = 0x1F5A,
	BD_MARKETPLACE_XB1_UNAUTHORISED_ERROR = 0x1F5B,
	BD_MARKETPLACE_XB1_REQUEST_REJECTED_ERROR = 0x1F5C,
	BD_MARKETPLACE_XB1_INSUFFICIENT_QUANTITY_ERROR = 0x1F5D,
	BD_MARKETPLACE_XB1_BAD_DELEGATION_TOKEN_ERROR = 0x1F5E,
	BD_MARKETPLACE_XB1_REQUEST_FAILED_ERROR = 0x1F5F,
	BD_MARKETPLACE_X360_REQUEST_FAILED = 0x1F60,
	BD_MARKETPLACE_X360_ITEM_NOT_PRESENT = 0x1F61,
	BD_MARKETPLACE_X360_CONNECTION_ERROR = 0x1F62,
	BD_MARKETPLACE_USAGE_TIME_ERROR = 0x1F63,
	BD_MARKETPLACE_DURABLE_ALREADY_GRANTED = 0x1F64,
	BD_MARKETPLACE_FIRST_PARTY_DURABLE_EXISTS = 0x1F66,
	BD_MARKETPLACE_ITEM_LIMIT_REACHED = 0x1F67,
	BD_MARKETPLACE_OVER_ITEM_MAX_QUANTITY_ERROR = 0x1F69,
	BD_MARKETPLACE_INSUFFICIENT_ITEM_QUANTITY = 0x1F6A,
	BD_MARKETPLACE_ENTITY_NOT_ENABLED = 0x1F6B,
	BD_MARKETPLACE_MISCONFIGURED = 0x1F6C,
	BD_MARKETPLACE_COUPON_NOT_CLAIMED_OR_FOUND = 0x1F6D,
	BD_MARKETPLACE_INVALID_DISCOUNT = 0x1F6E,
	BD_MARKETPLACE_INVALID_STORE_VERSION = 0x1F6F,
	BD_MARKETPLACE_SKU_SOLD_OUT = 0x1F72,
	BD_MARKETPLACE_WIIU_AUTH_FAILED = 0x1F73,
	BD_MARKETPLACE_WIIU_INVALID_PARAMETER = 0x1F74,
	BD_MARKETPLACE_WIIU_TAX_LOCATION_NOT_SPECIFIED = 0x1F75,
	BD_MARKETPLACE_WIIU_ACCOUNT_ERROR = 0x1F76,
	BD_MARKETPLACE_WIIU_PURCHASING_ERROR = 0x1F77,
	BD_MARKETPLACE_WIIU_BALANCE_ERROR = 0x1F78,
	BD_MARKETPLACE_WIIU_SERVER_ERROR = 0x1F79,
	BD_MARKETPLACE_WIIU_REQUEST_FAILED = 0x1F7A,
	BD_MARKETPLACE_WIIU_SERVER_MAINTENANCE = 0x1F7B,
	BD_MARKETPLACE_WIIU_SERVICE_TERMINATED = 0x1F7C,
	BD_MARKETPLACE_WIIU_ITEM_NOT_PRESENT = 0x1F7D,
	BD_MARKETPLACE_WIIU_TAX_LOCATION_INVALID = 0x1F7E,
	BD_MARKETPLACE_XB1_DURABLE_NOT_PRESENT = 0x1F7F,
	BD_MARKETPLACE_EXPECTED_PRICE_MISMATCH = 0x1F80,
	BD_MARKETPLACE_ITEM_NOT_CONSUMABLE = 0x1F81,
	BD_MARKETPLACE_IDEMPOTENT_REQUEST_COLLISION = 0x1F82,
	BD_MARKETPLACE_COUPON_NOT_STARTED = 0x1F83,
	BD_MARKETPLACE_MULTIPLE_OPERATIONS_ERROR = 0x1F84,
	BD_MARKETPLACE_MISSING_PAYMENT_PROVIDER_CURRENCY_ERROR = 0x1F85,
	BD_MARKETPLACE_WIIU_LANGUAGE_NOT_SUPPORTED = 0x1F86,
	BD_MARKETPLACE_PAWN_CHOICE_EXPECTED = 0x1F87,
	BD_MARKETPLACE_PAWN_CHOICE_UNSUPPORTED = 0x1F88,
	BD_MARKETPLACE_INVALID_REWARD_ERROR = 0x1F89,
	BD_COMMS_SERVICE_NOT_AVAILABLE_ERROR = 0x28A0,
	BD_COMMS_CLIENT_ERROR = 0x28A1,
	BD_LEAGUE_INVALID_TEAM_SIZE = 0x1FA4,
	BD_LEAGUE_INVALID_TEAM = 0x1FA5,
	BD_LEAGUE_INVALID_SUBDIVISION = 0x1FA6,
	BD_LEAGUE_INVALID_LEAGUE = 0x1FA7,
	BD_LEAGUE_TOO_MANY_RESULTS_REQUESTED = 0x1FA8,
	BD_LEAGUE_METADATA_TOO_LARGE = 0x1FA9,
	BD_LEAGUE_TEAM_ICON_TOO_LARGE = 0x1FAA,
	BD_LEAGUE_TEAM_NAME_TOO_LONG = 0x1FAB,
	BD_LEAGUE_ARRAY_SIZE_MISMATCH = 0x1FAC,
	BD_LEAGUE_SUBDIVISION_MISMATCH = 0x2008,
	BD_LEAGUE_INVALID_WRITE_TYPE = 0x2009,
	BD_LEAGUE_INVALID_STATS_DATA = 0x200A,
	BD_LEAGUE_SUBDIVISION_UNRANKED = 0x200B,
	BD_LEAGUE_CROSS_TEAM_STATS_WRITE_PREVENTED = 0x200C,
	BD_LEAGUE_INVALID_STATS_SEASON = 0x200D,
	BD_COMMERCE_ERROR = 0x206C,
	BD_COMMERCE_RESOURCE_NOT_FOUND = 0x206D,
	BD_COMMERCE_STORAGE_INVALID_PARAMETER = 0x206E,
	BD_COMMERCE_APPLICATION_INVALID_PARAMETER = 0x206F,
	BD_COMMERCE_RESOURCE_CONFLICT = 0x2070,
	BD_COMMERCE_STORAGE_ERROR = 0x2071,
	BD_COMMERCE_INTEGRITY_ERROR = 0x2072,
	BD_COMMERCE_MMP_SERVICE_ERROR = 0x2073,
	BD_COMMERCE_PERMISSION_DENIED = 0x2074,
	BD_COMMERCE_INSUFFICIENT_FUNDS_ERROR = 0x2075,
	BD_COMMERCE_UNKNOWN_CURRENCY = 0x2076,
	BD_COMMERCE_INVALID_RECEIPT = 0x2077,
	BD_COMMERCE_RECEIPT_USED = 0x2078,
	BD_COMMERCE_TRANSACTION_ALREADY_APPLIED = 0x2079,
	BD_COMMERCE_INVALID_CURRENCY_TYPE = 0x207A,
	BD_COMMERCE_UNKNOWN_PLATFORM = 0x207B,
	BD_COMMERCE_WINDOWS_STORE_CERT_INVALID = 0x207C,
	BD_CONNECTION_COUNTER_ERROR = 0x20D0,
	BD_LINKED_ACCOUNTS_INVALID_CONTEXT = 0x2198,
	BD_LINKED_ACCOUNTS_INVALID_PLATFORM = 0x2199,
	BD_LINKED_ACCOUNTS_LINKED_ACCOUNTS_FETCH_ERROR = 0x219A,
	BD_LINKED_ACCOUNTS_INVALID_ACCOUNT = 0x219B,
	BD_LINKED_ACCOUNTS_INVALID_TOKEN = 0x219C,
	BD_QUEUED_MATCHING_ERROR = 0x2260,
	BD_QUEUED_MATCHING_NOT_FOUND = 0x2261,
	BD_QUEUED_MATCHING_WRONG_QUEUE_TYPE = 0x2262,
	BD_QUEUED_MATCHING_INVALID_QUEUE_ID = 0x2263,
	BD_LOGIN_QUEUE_NO_ERROR = 0x22C4,
	BD_LOGIN_QUEUE_ENVIRONMENT_ERROR = 0x22C5,
	BD_LOGIN_QUEUE_UNKNOWN_ERROR = 0x22C6,
	BD_LOGIN_QUEUE_BAD_REQUEST = 0x22C7,
	BD_LOGIN_QUEUE_SERVER_UNAVAILABLE_ERROR = 0x22C8,
	BD_LOGIN_QUEUE_SSL_CERTIFICATE_ERROR = 0x22C9,
	BD_LOGIN_QUEUE_DNS_RESOLUTION_ERROR = 0x22CA,
	BD_LOGIN_QUEUE_JSON_FORMAT_ERROR = 0x22CB,
	BD_LOGIN_QUEUE_TICKET_PARSE_ERROR = 0x22CC,
	BD_LOGIN_QUEUE_INVALID_TITLE_ID = 0x22CD,
	BD_VOTING_MAX_VALUE_EXCEEDED = 0x2328,
	BD_VOTING_INVALID_GROUP_NAME = 0x2329,
	BD_VOTING_IN_PROGRESS = 0x232A,
	BD_VOTING_NON_EXISTENT_GROUP = 0x232B,
	BD_USER_GENERATED_STATS_ERROR = 0x238C,
	BD_INVALID_ACCESS_TO_USER_GENERATED_LB_ERROR = 0x238D,
	BD_INVALID_ACCESS_TO_NON_USER_GENERATED_LB_ERROR = 0x238E,
	BD_SUB_LEADERBOARD_ID_MISMATCH_ERROR = 0x238D,
	BD_PUBLISHER_VARIABLES_SERVICE_ERROR = 0x251C,
	BD_PUBLISHER_VARIABLES_NOT_FOUND = 0x251D,
	BD_PUBLISHER_VARIABLES_INVALID_NAMESPACE = 0x251E,
	BD_PUBLISHER_VARIABLES_INVALID_GROUP_ID = 0x251F,
	BD_GMSG_INVALID_CATEGORY_ID = 0x27D8,
	BD_GMSG_CATEGORY_MEMBERSHIPS_LIMIT = 0x27D9,
	BD_GMSG_NONMEMBER_POST_DISALLOWED = 0x27DA,
	BD_GMSG_CATEGORY_DISALLOWS_CLIENT_TYPE = 0x27DB,
	BD_GMSG_PAYLOAD_TOO_BIG = 0x27DC,
	BD_GMSG_MEMBER_POST_DISALLOWED = 0x27DD,
	BD_GMSG_OVERLOADED = 0x27DE,
	BD_GMSG_USER_PERCATEGORY_POST_RATE_EXCEEDED = 0x27DF,
	BD_GMSG_USER_GLOBAL_POST_RATE_EXCEEDED = 0x27E0,
	BD_GMSG_GROUP_POST_RATE_EXCEEDED = 0x27E1,
	BD_GMSG_GROUP_LIMIT_REACHED = 0x27E2,
	BD_GMSG_GROUP_MEMBERSHIP_LIMIT_REACHED = 0x27E3,
	BD_SERVER_INVENTORY_SERVER_ERROR = 0x2904,
	BD_SERVER_INVENTORY_SERVER_ALLOCATED = 0x2905,
	BD_SERVER_INVENTORY_ENTRY_DOES_NOT_EXIST = 0x2906,
	BD_SERVER_INVENTORY_DATA_LAYER_ERROR = 0x2907,
	BD_SERVER_INVENTORY_NOT_ENABLED = 0x2908,
	BD_SERVER_INVENTORY_NO_SERVER_ALLOCATED = 0x2909,
	BD_SERVER_INVENTORY_MAX_REGISTRATION_LENGTH_EXCEEDED = 0x290A,
	BD_CODO_CLAN_NO_TEMP_RESERVATION = 0x2AF8,
	BD_CODO_CLAN_TOO_MANY_MEMBERSHIPS = 0x2AF9,
	BD_CODO_CLAN_UNEXPECTED_FOUNDATION_STATE = 0x2AFA,
	BD_CODO_CLAN_UNABLE_TO_FIND_NAME = 0x2AFB,
	BD_CODO_CLAN_INTERNAL_MARKETPLACE_ERROR = 0x2AFC,
	BD_CODO_CLAN_INSUFFICIENT_FUNDS = 0x2AFD,
	BD_CODO_CLAN_UNSATISFIED_PRECONDITION = 0x2AFE,
	BD_CODO_CLAN_NO_CLAN = 0x2AFF,
	BD_CODO_CLAN_NO_CHANNEL = 0x2B00,
	BD_CODO_CLAN_NOT_A_CLAN_MEMBER = 0x2B01,
	BD_CODO_CLAN_DISBAND_TOO_MANY_MEMBERS = 0x2B04,
	BD_CODO_CLAN_DISBAND_TOO_SOON = 0x2B05,
	BD_NO_SUCH_ROLE = 0x2EE0,
	BD_NO_SUCH_PERMISSION = 0x2EE1,
	BD_BAD_ROLE_SPECIFICATION = 0x2EE2,
	BD_BAD_PERMISSION_SPECIFICATION = 0x2EE3,
	BD_REDEEMABLE_CODE_REGISTRY_SERVICE_GENERIC_ERROR = 0x300D,
	BD_REDEEMABLE_CODE_MARKETPLACE_SERVICE_GENERIC_ERROR = 0x300E,
	BAD_PAYLOAD_ERROR = 0x300F,
	INVALID_PAYLOAD_SIGNATURE_ERROR = 0x3010,
	GENERATE_PAYLOAD_SIGNATURE_ERROR = 0x3011,
	BD_REDEEMABLE_CODE_EXPIRED_CLIENT_TRANSACTION_ID = 0x3012,
	BD_REDEEMABLE_CODE_NOT_FOUND = 0x3071,
	BD_REDEEMABLE_CODE_USE_LIMIT_EXCEEDED = 0x3072,
	BD_REDEEMABLE_CODE_ALREADY_USED_BY_USER = 0x3073,
	BD_REDEEMABLE_CODE_EXPIRED = 0x3074,
	INVALID_CLIENT_TRANSACTION_ID = 0x3075,
	BD_REWARD_NO_REWARD_TOKEN_FOUND = 0x364C,
	BD_REWARD_INVALID_ACHIEVEMENT_IDS_SPECIFIED = 0x364D,
	BD_REWARD_CLIENT_ACHIEVEMENTS_NOT_ENABLED = 0x364E,
	BD_REWARD_EVENTS_ERROR = 0x364F,
	BD_REWARD_EVENTS_NOT_ENABLED = 0x3650,
	BD_REWARD_EVENTS_RULES_ERROR = 0x3651,
	BD_REWARD_EVENTS_DATA_ERROR = 0x3652,
	BD_REWARD_EVENTS_TRANSACTION_ERROR = 0x3653,
	BD_REWARD_CONFIGURATION_ERROR = 0x3654,
	BD_REWARD_TOO_MANY_ACTIVE_CHALLENGES = 0x3655,
	BD_REWARD_CODO_TOO_MANY_REROLLS = 0x367E,
	BD_REWARD_CODO_BAD_REROLL_PARAMS = 0x367F,
	BD_UMBRELLA_PROVIDER_SERVER_ERROR = 0x36B0,
	BD_UMBRELLA_PROVIDER_UNSUPPORTED_OPERATION = 0x36B1,
	BD_UMBRELLA_ERROR = 0x36B2,
	BD_UMBRELLA_INVALID_TITLE_ID = 0x36B3,
	BD_UMBRELLA_INVALID_QUERY_STRING = 0x36B4,
	BD_UMBRELLA_INVALID_DATA = 0x36B5,
	BD_UMBRELLA_INVALID_CLIENT = 0x36B6,
	BD_UMBRELLA_PROVIDER_NOT_SUPPORTED = 0x36B7,
	BD_UMBRELLA_UNAUTHORIZED_ACCESS = 0x36B8,
	BD_UMBRELLA_INVALID_TOKEN = 0x36B8,
	BD_UMBRELLA_EXPIRED_PROVIDER_REFRESH_TOKEN = 0x36B9,
	BD_UMBRELLA_NO_VALID_PROVIDER_TOKENS = 0x36BA,
	BD_UMBRELLA_INVALID_LSG_TICKET = 0x36BB,
	BD_UMBRELLA_TOKEN_NOT_FOUND = 0x36BC,
	BD_UMBRELLA_USER_NOT_FOUND = 0x36BD,
	BD_UMBRELLA_ACCOUNT_NOT_FOUND = 0x36BE,
	BD_UMBRELLA_MERGE_CONFLICT = 0x36BF,
	BD_UMBRELLA_PROVIDER_INACCESSIBLE = 0x36C0,
	BD_UMBRELLA_MISSING_FIELD = 0x36C1,
	BD_UNO_ERROR = 0x3714,
	BD_UNO_INVALID_DATA = 0x3715,
	BD_UNO_INVALID_DATE_OF_BIRTH = 0x3716,
	BD_UNO_UNAUTHORIZED_ACCESS = 0x3717,
	BD_UNO_INVALID_TOKEN = 0x3718,
	BD_UNO_EXPIRED_TOKEN = 0x3719,
	BD_UNO_EMAIL_NOT_VERIFIED = 0x371A,
	BD_UNO_TOS_VERSION_NOT_FOUND = 0x371B,
	BD_UNO_TOS_CONTENT_NOT_FOUND = 0x371C,
	BD_UNO_TOS_ALREADY_ACCEPTED = 0x371D,
	BD_UNO_MISSING_FIELD = 0x371E,
	BD_UNO_DNS_RESOLUTION_FAILED = 0x371F,
	BD_SI_ERROR = 0x3777,
	BD_AMM_NOT_LOBBY_HOST = 0x38A4,
	BD_AMM_NOT_PARTY_HOST = 0x38A5,
	BD_AMM_NOT_LOBBY_MEMBER = 0x38A6,
	BD_AMM_INVALID_MATCHMAKING_ID = 0x38A7,
	BD_AMM_INVALID_LOBBY_ID = 0x38A8,
	BD_AMM_SEARCH_IN_PROGRESS = 0x38A9,
	BD_AMM_USER_ALREADY_MATCHMAKING = 0x38AA,
	BD_AMM_INVALID_TOKEN = 0x38AB,
	BD_AMM_INVALID_DOCUMENT_FORMAT = 0x38AC,
	BD_AMM_PLAYER_INFO_UNAVAILABLE = 0x38AD,
	BD_AMM_REQUEST_DESERIALIZATION_FAILED = 0x38AE,
	BD_AMM_INVALID_QOS_TRANSACTION_ID = 0x38AF,
	BD_AMM_INVALID_USER_ID_IN_GUEST_SLOTS = 0x38B0,
	BD_AMM_NO_BUILD_NAME_SET = 0x38B1,
	BD_AMM_LOBBY_MERGED = 0x38B2,
	BD_AMM_BACKOFF_REQUESTED = 0x38B3,
	BD_MATCH_SCHEDULER_ERROR = 0x3A98,
	BD_MATCH_SCHEDULER_SCHEDULED_MATCH_DOES_NOT_EXIST = 0x3A99,
	BD_MATCH_SCHEDULER_MAXIMUM_EVENTS_PER_MATCH_REACHED = 0x3A9A,
	BD_MLG_ERROR = 0x3AFC,
	BD_MLG_REMOTE_ERROR = 0x3AFD,
	BD_CODO_AUTO_JOIN_LEVEL_NOT_MET = 0x4A9D,
	BD_CODO_NOT_PERMITTED_TO_UPDATE_PROFILE_FIELD = 0x4A9E,
	BD_CODO_INVALID_PROFILE_VALUE = 0x4A9F,
	BD_CODO_PROFILE_COLUMN_DOES_NOT_EXIST = 0x4AA0,
	BD_CODO_LOUDSPEAKER_INVALID_TOKEN = 0x4AA1,
	BD_CODO_LOUDSPEAKER_INVALID_CATEGORY = 0x4AA2,
	BD_CODO_RESETTABLE_STATS_NOT_CONFIGURED = 0x4AA3,
	BD_MAX_ERROR_CODE = 0x4AA4,
};


enum LobbyJoinable : __int32
{
	LOBBY_JOINABLE_NO_NOT_IN_LOBBY = 0x0,
	LOBBY_JOINABLE_YES = 0x1,
	LOBBY_JOINABLE_YES_FRIENDS_ONLY = 0x2,
	LOBBY_JOINABLE_NO_PARTY_PRIVACY_INVITE_ONLY = 0x3,
	LOBBY_JOINABLE_NO_PARTY_PRIVACY_CLOSED = 0x4,
	LOBBY_JOINABLE_NO_PARTY_FULL = 0x5,
	LOBBY_JOINABLE_NO_GAME_FULL = 0x6,
	LOBBY_JOINABLE_NO_PLAYING_LOCAL = 0x7,
	LOBBY_JOINABLE_NO_GAME_PAUSED = 0x8,
	LOBBY_JOINABLE_NO = 0x9,
	LOBBY_JOINABLE_NO_YOU_NEED_DLC = 0xA,
	LOBBY_JOINABLE_COUNT = 0xB,
};

enum PresencePrimary : __int32
{
	PRESENCE_PRIMARY_UNKNOWN = 0xFFFFFFFF,
	PRESENCE_PRIMARY_OFFLINE = 0x0,
	PRESENCE_PRIMARY_ONLINE = 0x1,
	PRESENCE_PRIMARY_LOCAL = 0x2,
	PRESENCE_PRIMARY_TITLE = 0x3,
	PRESENCE_PRIMARY_AWAY = 0x4,
	PRESENCE_PRIMARY_BLOCKED = 0x5,
};


struct PresencePlatformData
{
	PresencePrimary primaryPresence;
	char titleId[32];
	char titleName[64];
	char titleStatus[64];
};






struct PresenceTitleData
{

	struct Party
	{
		struct Member
		{
			char gamertag[17];
		};

		int max;
		int totalCount;
		int availableCount;
		PresenceTitleData::Party::Member members[18];
	};

	PresenceActivity activity;
	PresenceCtx ctx;
	LobbyJoinable joinable;
	int gametypeID;
	int mapID;
	int difficulty;
	int playlist;
	int startupTimestamp;
	PresenceTitleData::Party party;
};


struct PresenceData
{
	int version;
	int flags;
	__int64 xuid;
	bool isDirty;
	bool isInitialzied;
	int id;
	const char* base;
	const char* params;
	const char* data;
	int failureCount;
	int lastUpdateTime;
	int state;
	PresenceTitleData title;
	PresencePlatformData platform;
};

struct CACSlot
{
	const char* label;
	const char* slotName;

	bool isPrimary;
	int defaultComboType;

	std::string* previews;
	int* selectedItems;
	int* selectedSlots;
};

struct refdef_s
{
public:
	std::int32_t x;
	std::int32_t y;
	std::int32_t width;
	std::int32_t height;
private:
	byte _0x10[0x68];
public:
	std::float_t fovX;
	std::float_t fovY;
private:
	byte _0x80[0x8];
public:
	ImVec3 viewOrigin;
private:
	byte _0x94[0x10];
public:
	ImVec3 viewAxis[3];
private:
	byte _0xC8[0x1A6B28];
};

struct trajectory_s
{
public:
	std::int32_t trType;
	std::int32_t trTime;
	std::int32_t trDuration;
	ImVec3 trBase;
	ImVec3 trDelta;
};
#pragma pack(push, 8)

struct lerp_entity_state_s
{
public:
	std::int32_t eFlags[2];
	trajectory_s pos;
	trajectory_s apos;
private:
	byte _0x50[0x78];
};
#pragma pack(pop)

#pragma pack(push, 4)
struct cpose_s
{
private:
	byte _0x0[0x40];
public:
	ImVec3 origin;
	ImVec3 angles;
	ImVec3 absmin;
	ImVec3 absmax;
private:
	byte _0x70[0x2C0];
};
#pragma pack(pop)

#pragma pack(push, 4)
struct entity_state_s
{
public:
	std::int32_t number;
	lerp_entity_state_s lerp;
private:
	byte _0xCC[0xCC];
public:
	std::uint64_t eventParm;
	entityType_t eType;
private:
	byte _0x1A4[0xA];
public:
	std::int16_t otherEntityNum;
	std::int16_t attackerEntityNum;
private:
	byte _0x1B2[0x6];
public:
	std::int64_t weapon;
private:
	byte _0x1C0[0x33C];
};
#pragma pack(pop)

struct centity_s
{
public:
	cpose_s pose;
	lerp_entity_state_s prevState;
	entity_state_s nextState;
	std::int32_t alive;
private:
	byte _0x8F8[0x8];
};

struct player_held_weapon_s
{
public:
	std::int64_t weapon;
private:
	byte _0x8[0x28];
};

struct ammoCount {
	int count;
};

struct ammoPool {
	int count;
};

struct player_state_s
{
public:
	std::int32_t clientNum;
	std::int32_t commandTime;
private:
	byte _0x8[0x18];
public:
	std::int32_t otherFlags;
private:
	byte _0x24[0x10];
public:
	ImVec3 origin;
	ImVec3 velocity;
private:
	byte _0x4C[0x10];
public:
	std::uint32_t weaponState;
private:
	byte _0x60[0x18];
public:
	std::uint32_t weaponStateLeft;
private:
	byte _0x7C[0x40];
public:
	bool bThirdPerson;
private:
	byte _0xBD[0x13];
public:
	ImVec3 deltaAngles;
private:
	byte _0xDC[0x220];
public:
	std::float_t weaponPosFrac;
private:
	byte _0x300[0x18];
public:
	ImVec3 viewAngles;
private:
	byte _0x324[0x30];
public:
	std::int32_t damageEvent;
	std::int32_t damageYaw;
	std::int32_t damagePitch;
	std::int32_t damageProjYaw;
	std::int32_t damageCount;
	std::int32_t stats[4];
	player_held_weapon_s heldWeapons[15];
	ammoPool ammoNotInClip[15];
	ammoCount ammoInClip[15];
private:
	byte _0x648[0x6C];
public:
	std::int32_t satalliteTypeEnabled;
private:
	byte _0x730[0xB4];
public:
	std::int32_t mantleFlags;
private:
	byte _0x7E8[0x390];
public:
	std::float_t spreadMultiplier;
private:
	byte _0xB7C[0x4];
};

struct cgs_s
{
private:
	byte _0x0[0x1BC];
public:
	std::int32_t voteTime;
private:
	byte _0x1C0[0x8];
public:
	char voteString[256];
};

struct client_info_s
{
public:
	std::int32_t infoValid;
	std::int32_t nextValid;
	std::int32_t clientNum;
	char clientName[32];
	std::int32_t team;
private:
	byte _0x30[0x64];
public:
	std::int32_t score;
private:
	byte _0x98[0x14];
public:
	std::int32_t deaths;
private:
	byte _0xB0[0x28];
public:
	std::int32_t kills;
private:
	byte _0xDC[0x18];
public:
	std::int32_t health;
private:
	byte _0xF8[0x48C];
public:
	ImVec3 angles;
private:
	byte _0x590[0x940];
};

struct cg_s
{
public:
	std::int32_t clientNum;
private:
	byte _0x4[0x7C];
public:
	ImVec3 origin;
	ImVec3 velocity;
private:
	byte _0x98[0x11A7EC];
public:
	std::float_t frameInterpolation;
	std::int32_t frameTime;
	std::int32_t time;
	std::int32_t oldTime;
private:
	byte _0x11A894[0x14];
public:
	bool bThirdPerson;
	bool renderingThirdPerson;
	bool renderingThirdPersonDeath;
	bool renderingThirdPersonLastFrame;
private:
	byte _0x11A8AC[0x4];
public:
	player_state_s predictedPlayerState;
private:
	byte _0x11B430[0xA9F8];
public:
	centity_s predictedPlayerEntity;
private:
	byte _0x126728[0xB5C8];
public:
	refdef_s refDef;
	ImVec3 refDefViewAngles;
private:
	byte _0x2D88EC[0xEBC8];
public:
	std::int32_t v_dmg_time;
	std::float_t v_dmg_pitch;
	std::float_t v_dmg_roll;
	std::float_t v_dmg_flash;
	std::float_t bobCycle;
	std::float_t xySpeed;
	ImVec3 kickAngleVelocity;
	ImVec3 kickAngles;
	ImVec3 offsetAngles;
	std::float_t gunPitch;
	std::float_t gunYaw;
private:
	byte _0x2E74F8[0x548];
public:
	client_info_s clientInfo[18];
private:
	byte _0x2F84E0[0x38C960];
public:
	cgs_s cgs;
private:
	byte _0x685108[0x2259D8];
public:
	centity_s centity[1024];
};

enum LobbyType
{
	LOBBY_TYPE_INVALID = 0xFFFFFFFF,
	LOBBY_TYPE_PRIVATE = 0x0,
	LOBBY_TYPE_GAME = 0x1,
	LOBBY_TYPE_TRANSITION = 0x2,
	LOBBY_TYPE_COUNT = 0x3,
	LOBBY_TYPE_FIRST = 0x0,
	LOBBY_TYPE_LAST = 0x2,
	LOBBY_TYPE_AUTO = 0x3,
};

struct server_t {

	unsigned __int64 xuid;
	std::string ip;
	std::string port;
	netadr_t netadr;
	unsigned int numPlayers;
	unsigned int maxPlayers;
	unsigned int serverType;
	unsigned int gameType;

	int geo1;
	int geo2;
	int geo3;
	int geo4;

	bdSecurityID LobbyID;

};

typedef uint64_t XUID;

#pragma pack(push, 1)
struct HostInfo
{
	XUID xuid;                    // 0x00
	char name[32];                // 0x08
	netadr_t netAdr;              // 0x28
	SerializedAdr serializedAdr;  // 0x38, xnaddr at 0x39
	bdSecurityID secId;           // 0x5E
	bdSecurityKey secKey;         // 0x66
	std::uint32_t serverLocation; // 0x76
};
#pragma pack(pop)


struct bdMatchMakingInfo
{
	bdSecurityID sessionID;
	uint8_t hostAddr[255];
	uint32_t hostAddrSize;
	uint32_t gameType;
	uint32_t maxPlayers;
	uint32_t numPlayers;
};

struct MatchMakingInfo
{
	char pad[0x20];
	bdMatchMakingInfo info;
	uint64_t sessionID;
	char keyExchangeKey[17];
	int serverType;
	uint64_t xuid;
	uint32_t serverLocation;
	uint32_t latencyBand;
	int showInMatchmaking;
	int netcodeVersion;
	int mapPacks;
	int playlistVersion;
	int playlistNumber;
	int isEmpty;
	int teamMax;
	float skill;
	int geo1;
	int geo2;
	int geo3;
	int geo4;
	int dirty;
	int active;
	int timeSinceLastUpdate;
	int recreateSession;
	int timeSinceUpdate;
	char pad2[0x14];

	HostInfo get_host_info() const
	{
		HostInfo host_info{};
		host_info.xuid = xuid;
		host_info.secId.id = sessionID;
		host_info.secKey = *reinterpret_cast<const bdSecurityKey*>(keyExchangeKey);
		host_info.serializedAdr.xnaddr = *reinterpret_cast<const XNADDR*>(info.hostAddr);
		return host_info;
	}

	bool operator<(const MatchMakingInfo& other) const
	{
		return info.numPlayers > other.info.numPlayers;
	}
};

enum JoinClientState
{
	JOIN_CLIENT_STATE_IDLE = 0x0,
	JOIN_CLIENT_STATE_ASSOCIATING = 0x1,
	JOIN_CLIENT_STATE_COUNT = 0x2,
};

struct JoinHost
{
	HostInfo info;
	LobbyType lobbyType;
	LobbyParams lobbyParams;
	uint64_t reservationKey;
	int retryTime;
	int retryCount;
};

struct JoinClient
{
	JoinClientState state;
	JoinHost potentialHost;
	LobbyType sourceLobbyType;
	int controllerIndex;
};


enum TaskState
{
	TASK_STATE_INVALID = 0x0,
	TASK_STATE_INIT = 0x1,
	TASK_STATE_INPROGRESS = 0x2,
	TASK_STATE_INCALLBACK = 0x3,
	TASK_STATE_CHILDCOMPLETE = 0x4,
	TASK_STATE_CHILDFAILED = 0x5,
	TASK_STATE_COMPLETED = 0x6,
	TASK_STATE_FAILED = 0x7,
	TASK_STATE_CANCELLED = 0x8,
	TASK_STATE_COUNT = 0x9,
};

struct bdRemoteTask
{
	char pad[0x38];
	uint32_t numResults;
	uint32_t maxNumResults;
	uint32_t totalNumResults;
	uint64_t transactionID;
	bdLobbyErrorCode errorCode;
	void* taskResultProcessor;
	uint8_t serviceId;
	uint8_t taskId;
};

struct TaskRecord;
using task_callback = void(*)(TaskRecord*);

struct TaskDefinition
{
	const uint64_t category;
	const char* const name;
	const int payloadSize;
	const task_callback completed_callback;
	const task_callback failure_callback;
};

struct TaskRecord
{
	TaskRecord* next;
	const TaskDefinition* definition;
	TaskState state;
	unsigned int controllerIndex;
	unsigned int lastPoll;
	unsigned int lastPollMS;
	unsigned int id;
	int startMS;
	int timeOut;
	int timeIn;
	char pad[0x8];
	bdRemoteTask* remoteTask;
	TaskRecord* nestedTask;
	void* payload;
};


struct XSESSION_INFO
{
	bdSecurityID sessionID;
	XNADDR hostAddress;
	bdSecurityKey keyExchangeKey;
};

struct NetChanFragment_s
{
	NetChanFragment_s* next;
	int size;
	uint8_t sequence;
	char msgBuf[1256];
	uint16_t sendCount;
};

struct NetChanMessage_s
{
	uint8_t numFragments;
	bool complete;
	uint32_t messageLen;
	uint32_t sequence;
	uint64_t destXUID;
	uint64_t sourceXUID;
	uint16_t sendCount;
	netadr_t destAddress;
	void* msgConfig;
	NetChanMessage_s* next;
	NetChanFragment_s* fragments;
	uint32_t acked[4];
	int lastAckMS;
	int lastTouchedMS;
	int lastKeepAliveMs;
	int timeoutMS;
	bool dropped;
	uint16_t nonce;
	int rttMS;
	int nextSendMS;
};

struct NetChanMessageList_s
{
	NetChanMessage_s* in[18];
	NetChanMessage_s* out[18];
	void* ackCache;
};

struct qosEndpoint_t
{
	std::uint32_t ip;      // inet_addr result
	std::uint16_t port;
	std::uint16_t pad;
};

struct qosServer_t
{
	std::uint32_t serverId;
	std::uint16_t portOrId;
	char name[32];

	char pad_0026[0x02];

	qosEndpoint_t endpoints[8];

	std::uint8_t endpointCount;
	char pad_0069[0x07];

	std::uint16_t activeEndpoint;
	std::uint16_t challenge;
	std::uint16_t bestPing;
	std::uint16_t pad_0076;

	std::uint32_t state;
	std::uint8_t numPacketsRecvd;

	char pad_007D[0x03];

	std::uint32_t replyBits;

	char pad_0084[0x04];

	std::uint16_t pings[10];

	std::uint16_t pingTotal;

	char pad_009E[0x02];
};

struct Msg_InfoRequest
{
	std::uint32_t nonce;
};

enum QueryId
{
	ENUM_SEARCH_SESSIONS_INVALID = 0x0,
	ENUM_SEARCH_SESSIONS_BY_PLAYLIST_VERSION = 0x1,
	ENUM_SEARCH_SESSIONS = 0x2,
	ENUM_SEARCH_SESSION_BY_GAME_SECURITY_ID = 0x3,
	ENUM_SEARCH_SESSION_DEDICATED = 0x4,
	ENUM_SEARCH_SESSIONS_DEDICATED_PARKED = 0x5,
	ENUM_SEARCH_SESSIONS_BY_SERVER_TYPE = 0x6,
};

enum JoinType
{
	JOIN_TYPE_NORMAL = 0x0,
	JOIN_TYPE_PLAYLIST = 0x1,
	JOIN_TYPE_FRIEND = 0x2,
	JOIN_TYPE_INVITE = 0x3,
	JOIN_TYPE_PARTY = 0x4,
	JOIN_TYPE_GROUPS = 0x5,
	JOIN_TYPE_COUNT = 0x6,
};

struct MatchMakingQuery
{
	char pad[0x10];
	QueryId queryId;
	uint64_t securityID;
	int showInMatchmaking;
	int netcodeVersion;
	int mapPacks;
	int playlistVersion;
	int playlistNum;
	int serverType;
	int isEmpty;
	int teamSize;
	char pad2[0x78];
};

enum StorageFileType
{
	STORAGE_COMMON_SETTINGS = 0x0,
	STORAGE_PROFILE_SHOUTCASTER = 0x1,
	STORAGE_CP_SAVEGAME_ONLINE = 0x2,
	STORAGE_CP_SAVEGAME_OFFLINE = 0x3,
	STORAGE_CPNIGHTMARE_SAVEGAME_ONLINE = 0x4,
	STORAGE_CPNIGHTMARE_SAVEGAME_OFFLINE = 0x5,
	STORAGE_CP_STATS_ONLINE = 0x6,
	STORAGE_CP_STATS_OFFLINE = 0x7,
	STORAGE_CP_STATS_NIGHTMARE_ONLINE = 0x8,
	STORAGE_CP_STATS_NIGHTMARE_OFFLINE = 0x9,
	STORAGE_CP_LOADOUTS = 0xA,
	STORAGE_CP_LOADOUTS_OFFLINE = 0xB,
	STORAGE_MP_STATS_ONLINE = 0xC,
	STORAGE_MP_STATS_OFFLINE = 0xD,
	STORAGE_MP_LOADOUTS = 0xE,
	STORAGE_MP_LOADOUTS_CUSTOM = 0xF,
	STORAGE_MP_LOADOUTS_ARENA = 0x10,
	STORAGE_MP_LOADOUTS_OFFLINE = 0x11,
	STORAGE_MP_CLASS_SETS = 0x12,
	STORAGE_ZM_STATS_ONLINE = 0x13,
	STORAGE_ZM_STATS_OFFLINE = 0x14,
	STORAGE_ZM_LOADOUTS = 0x15,
	STORAGE_ZM_LOADOUTS_OFFLINE = 0x16,
	STORAGE_FR_STATS_ONLINE = 0x17,
	STORAGE_FR_STATS_OFFLINE = 0x18,
	STORAGE_PAINTSHOP_DATA = 0x19,
	STORAGE_GUNSMITH = 0x1A,
	STORAGE_PAINTJOBS = 0x1B,
	STORAGE_EMBLEMS = 0x1C,
	STORAGE_DEFAULT_EMBLEMS = 0x1D,
	STORAGE_EMBLEMS_LOOT = 0x1E,
	STORAGE_CUSTOM_GAMES = 0x1F,
	STORAGE_OFFICIAL_CUSTOM_GAMES = 0x20,
	STORAGE_EXTERNAL_DATA = 0x21,
	STORAGE_FILE_COUNT = 0x22,
	STORAGE_FILE_FIRST = 0x0,
	STORAGE_FILE_INVALID = 0xFFFFFFFF,
};

enum NetChanMsgType
{
	NETCHAN_INVALID_CHANNEL = 0xFFFFFFFF,
	NETCHAN_SNAPSHOT,
	NETCHAN_CLIENTMSG,
	NETCHAN_VOICE,
	NETCHAN_LOBBY_VOICE,
	NETCHAN_LOBBYPRIVATE_STATE,
	NETCHAN_LOBBYPRIVATE_HEARTBEAT,
	NETCHAN_LOBBYPRIVATE_RELIABLE,
	NETCHAN_LOBBYPRIVATE_UNRELIABLE,
	NETCHAN_LOBBYPRIVATE_MIGRATE,
	NETCHAN_LOBBYGAME_STATE,
	NETCHAN_LOBBYGAME_HEARTBEAT,
	NETCHAN_LOBBYGAME_RELIABLE,
	NETCHAN_LOBBYGAME_UNRELIABLE,
	NETCHAN_LOBBYGAME_MIGRATE,
	NETCHAN_LOBBY_JOIN,
	NETCHAN_PTP,
	NETCHAN_CLIENT_CONTENT,
	NETCHAN_TEST,
	NETCHAN_MAX_CHANNELS,
};

struct GfxImage;

struct CompositeEmblem
{
	uint8_t bytes[0x1C50];
};

static_assert(sizeof(CompositeEmblem) == 0x1C50);

struct CmdArgs
{
	int nesting;
	int localClientNum[8];
	ControllerIndex_t controllerIndex[8];
	int argshift[8];
	int argc[8];
	const char** argv[8];
	char textPool[8192];
	const char* argvPool[512];
	int usedTextPool[8];
	int totalUsedArgvPool;
	int totalUsedTextPool;
};

enum class ResolutionTier {
	Low = 0,    // 300x184
	Medium,     // 600x368
	Good,       // 1200x736
	Best,        // 2400x1472
	Overkill        // 8175x5014
};

using CompositeJobID = std::int32_t;

enum CompositeState : std::int32_t
{
	COMPOSITE_STATE_IDLE = 0,
	COMPOSITE_STATE_PRE = 1,
	COMPOSITE_STATE_RENDERING = 2,
	COMPOSITE_STATE_WAITING_FOR_GPU = 3,
	COMPOSITE_STATE_COMPLETE = 4,
	COMPOSITE_STATE_COUNT = 5,
};

#pragma pack(push, 1)

struct CompositeJob
{
	std::int32_t state;                     // 0x0000
	std::int32_t pad_0004;                  // 0x0004
	std::uint64_t lastStateChangeFrame;     // 0x0008
	std::int32_t type;                      // 0x0010
	std::int32_t id;                        // 0x0014
	std::int32_t layerCount;                // 0x0018

	std::uint8_t emblem[0x1C44]; // 0x001C - 0x1C5F

	bool setupImage;                        // 0x1C60
	std::uint8_t pad_1C61[7];               // 0x1C61
	GfxImage* resultImage;                  // 0x1C68
	std::uint32_t imageSemantic;            // 0x1C70
	bool cancel;                            // 0x1C74
	std::uint8_t pad_1C75[3];               // 0x1C75
	std::int32_t renderDelay;               // 0x1C78
	std::int32_t pad_1C7C;                  // 0x1C7C
};

#pragma pack(pop)

static_assert(sizeof(CompositeJob) == 0x1C80);
static_assert(offsetof(CompositeJob, id) == 0x14);
static_assert(offsetof(CompositeJob, resultImage) == 0x1C68);
static_assert(offsetof(CompositeJob, cancel) == 0x1C74);



/*
#pragma pack(push, 1)

struct vec2_t_pc
{
	float x;
	float y;
};

struct vec4_t_pc
{
	float r;
	float g;
	float b;
	float a;
};

static_assert(sizeof(vec2_t_pc) == 0x08);
static_assert(sizeof(vec4_t_pc) == 0x10);

// Shared 0x5C-byte base used by both layers and groups.
struct CompositeEmblemTransform_PC
{
	std::int16_t materialID;              // 0x00, DDL materialID - 1
	std::uint8_t pad_02[2];              // 0x02

	vec4_t_pc color;                     // 0x04, red/green/blue/alpha
	vec4_t_pc color1;                    // 0x14, red1/green1/blue1/alpha1

	std::uint32_t gradientType;          // 0x24
	float gradientAngle;                 // 0x28
	float gradientFill;                  // 0x2C

	vec2_t_pc pos;                       // 0x30, posx/posy
	vec2_t_pc scale;                     // 0x38, scalex/scaley
	float angle;                         // 0x40

	vec2_t_pc materialScale;             // 0x44, materialscalex/materialscaley
	vec2_t_pc materialPos;               // 0x4C, material pos/offset
	float materialAngle;                 // 0x54

	std::uint8_t outline;                // 0x58
	std::uint8_t flip;                   // 0x59
	std::uint8_t blend;                  // 0x5A
	std::uint8_t coupledMaterialScale;   // 0x5B
};

static_assert(sizeof(CompositeEmblemTransform_PC) == 0x5C);

struct CompositeEmblemLayer_PC : CompositeEmblemTransform_PC
{
	std::int16_t icon;                   // 0x5C, DDL icon - 1
	std::int8_t groupIndex;              // 0x5E, DDL groupIndex - 1
	std::uint8_t pad_5F;                 // 0x5F
};

static_assert(sizeof(CompositeEmblemLayer_PC) == 0x60);
static_assert(offsetof(CompositeEmblemLayer_PC, materialID) == 0x00);
static_assert(offsetof(CompositeEmblemLayer_PC, color) == 0x04);
static_assert(offsetof(CompositeEmblemLayer_PC, color1) == 0x14);
static_assert(offsetof(CompositeEmblemLayer_PC, gradientType) == 0x24);
static_assert(offsetof(CompositeEmblemLayer_PC, gradientAngle) == 0x28);
static_assert(offsetof(CompositeEmblemLayer_PC, gradientFill) == 0x2C);
static_assert(offsetof(CompositeEmblemLayer_PC, pos) == 0x30);
static_assert(offsetof(CompositeEmblemLayer_PC, scale) == 0x38);
static_assert(offsetof(CompositeEmblemLayer_PC, angle) == 0x40);
static_assert(offsetof(CompositeEmblemLayer_PC, materialScale) == 0x44);
static_assert(offsetof(CompositeEmblemLayer_PC, materialPos) == 0x4C);
static_assert(offsetof(CompositeEmblemLayer_PC, materialAngle) == 0x54);
static_assert(offsetof(CompositeEmblemLayer_PC, outline) == 0x58);
static_assert(offsetof(CompositeEmblemLayer_PC, flip) == 0x59);
static_assert(offsetof(CompositeEmblemLayer_PC, blend) == 0x5A);
static_assert(offsetof(CompositeEmblemLayer_PC, coupledMaterialScale) == 0x5B);
static_assert(offsetof(CompositeEmblemLayer_PC, icon) == 0x5C);
static_assert(offsetof(CompositeEmblemLayer_PC, groupIndex) == 0x5E);

struct CompositeEmblemGroup_PC : CompositeEmblemTransform_PC
{
	std::uint8_t isGrouped;              // 0x5C
	std::uint8_t groupedMaterial;        // 0x5D, 
	std::uint8_t pad_5E[2];              // 0x5E
};

static_assert(sizeof(CompositeEmblemGroup_PC) == 0x60);
static_assert(offsetof(CompositeEmblemGroup_PC, isGrouped) == 0x5C);
static_assert(offsetof(CompositeEmblemGroup_PC, groupedMaterial) == 0x5D);

struct CompositeEmblem_PC
{
	CompositeEmblemLayer_PC layers[64];  // 0x0000 - 0x17FF
	CompositeEmblemGroup_PC groups[11];  // 0x1800 - 0x1C1F

	std::int32_t weaponIndex;            // 0x1C20, paintshop uses this, emblems usually 0
	std::int32_t side;                   // 0x1C24, paintshop side, emblems usually INVALID
	std::int32_t gridSize;               // 0x1C28

	vec4_t_pc highlightColor;            // 0x1C2C

	std::int8_t highlightLayer;          // 0x1C3C, -1 when none
	std::int8_t highlightGroup;          // 0x1C3D, -1 / 255 when none
	std::uint8_t highlightBorder;        // 0x1C3E
	std::uint8_t pad_1C3F[5];            // 0x1C3F - 0x1C43
};

#pragma pack(pop)

static_assert(sizeof(CompositeEmblem_PC) == 0x1C44);
static_assert(offsetof(CompositeEmblem_PC, layers) == 0x0000);
static_assert(offsetof(CompositeEmblem_PC, groups) == 0x1800);
static_assert(offsetof(CompositeEmblem_PC, weaponIndex) == 0x1C20);
static_assert(offsetof(CompositeEmblem_PC, side) == 0x1C24);
static_assert(offsetof(CompositeEmblem_PC, gridSize) == 0x1C28);
static_assert(offsetof(CompositeEmblem_PC, highlightColor) == 0x1C2C);
static_assert(offsetof(CompositeEmblem_PC, highlightLayer) == 0x1C3C);
static_assert(offsetof(CompositeEmblem_PC, highlightGroup) == 0x1C3D);
static_assert(offsetof(CompositeEmblem_PC, highlightBorder) == 0x1C3E);

*/

// static s_rankData @ 0x576A980
struct rankData_t
{
	int minxp[96];               // 0x0
	int maxxp[96];               // 0x180
	int rankId[96];              // 0x300
	unsigned __int8 unlocks[96][27]; // 0x480
	int topRow;                  // 0xEA0
	int paragonMinXp[1000];      // 0xEA4
	int paragonMaxXp[1000];      // 0x1E44
	int paragonRankId[1000];     // 0x2DE4
	int paragonTopRow;           // 0x3D84
	int prestigeCap;             // 0x3D88
	int xpCap;                   // 0x3D8C
	int starterPackMaxXp;        // 0x3D90
	int paragonXpCap;            // 0x3D94
	bool isValid;                // 0x3D98
}; // Size 0x3D9C

static_assert(sizeof(rankData_t) == 0x3D9C);