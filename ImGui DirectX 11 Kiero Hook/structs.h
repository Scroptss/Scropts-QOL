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

struct loot_item_s
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
	loot_item_s lootItem[4000];
	std::int32_t itemCount;
private:
	byte _0x13884[0x4];
public:
	std::uint32_t itemFlags;
private:
	byte _0x1388C[0x41C];
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
	bool overflowed;
	bool readOnly;
	char* data;
	char* splitData;
	int maxsize;
	int cursize;
	int splitSize;
	int readcount;
	int bit;
	int lastEntityRef;
	bool flush;
	netsrc_t targetLocalNetID;
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


enum eWeaponSlot {
	PRIMARY,
	PRIMARY_ATTACHMENT1,
	PRIMARY_ATTACHMENT2,
	PRIMARY_ATTACHMENT3,
	PRIMARY_ATTACHMENT4,
	PRIMARY_ATTACHMENT5,
	PRIMARY_ATTACHMENT6,
	PRIMARY_ATTACHMENT7,
	PRIMARY_ATTACHMENT8,
	PRIMARY_ATTACHMENT1_COSMETIC_VARIANT,
	PRIMARY_ATTACHMENT2_COSMETIC_VARIANT,
	PRIMARY_ATTACHMENT3_COSMETIC_VARIANT,
	PRIMARY_ATTACHMENT4_COSMETIC_VARIANT,
	PRIMARY_ATTACHMENT5_COSMETIC_VARIANT,
	PRIMARY_ATTACHMENT6_COSMETIC_VARIANT,
	PRIMARY_ATTACHMENT7_COSMETIC_VARIANT,
	PRIMARY_ATTACHMENT8_COSMETIC_VARIANT,
	PRIMARY_CAMO,
	PRIMARY_RETICLE,
	PRIMARY_RETICLE_COLOR,
	PRIMARY_LENS,
	PRIMARY_EMBLEM,
	PRIMARY_TAG,
	PRIMARY_PAINTJOB_SLOT,
	PRIMARY_PAINTJOB_INDEX,
	PRIMARY_GUNSMITH_VARIANT,
	SECONDARY,
	SECONDARY_ATTACHMENT1,
	SECONDARY_ATTACHMENT2,
	SECONDARY_ATTACHMENT3,
	SECONDARY_ATTACHMENT4,
	SECONDARY_ATTACHMENT5,
	SECONDARY_ATTACHMENT6,
	SECONDARY_ATTACHMENT7,
	SECONDARY_ATTACHMENT8,
	SECONDARY_ATTACHMENT1_COSMETIC_VARIANT,
	SECONDARY_ATTACHMENT2_COSMETIC_VARIANT,
	SECONDARY_ATTACHMENT3_COSMETIC_VARIANT,
	SECONDARY_ATTACHMENT4_COSMETIC_VARIANT,
	SECONDARY_ATTACHMENT5_COSMETIC_VARIANT,
	SECONDARY_ATTACHMENT6_COSMETIC_VARIANT,
	SECONDARY_ATTACHMENT7_COSMETIC_VARIANT,
	SECONDARY_ATTACHMENT8_COSMETIC_VARIANT,
	SECONDARY_CAMO,
	SECONDARY_RETICLE,
	SECONDARY_RETICLE_COLOR,
	SECONDARY_LENS,
	SECONDARY_EMBLEM,
	SECONDARY_TAG,
	SECONDARY_PAINTJOB_SLOT,
	SECONDARY_PAINTJOB_INDEX,
	SECONDARY_GUNSMITH_VARIANT,
	KNIFE_CAMO,
	PRIMARY_GRENADE,
	PRIMARY_GRENADE_COUNT,
	PRIMARY_GRENADE_STATUS1,
	PRIMARY_GRENADE_STATUS2,
	PRIMARY_GRENADE_STATUS3,
	SPECIAL_GRENADE,
	SPECIAL_GRENADE_COUNT,
	SPECIAL_GRENADE_STATUS1,
	SPECIAL_GRENADE_STATUS2,
	SPECIAL_GRENADE_STATUS3,
	PRIMARY_GADGET,
	PRIMARY_GADGET_ATTACHMENT1,
	PRIMARY_GADGET_ATTACHMENT2,
	PRIMARY_GADGET_ATTACHMENT3,
	SECONDARY_GADGET,
	SECONDARY_GADGET_ATTACHMENT1,
	SECONDARY_GADGET_ATTACHMENT2,
	SECONDARY_GADGET_ATTACHMENT3,
	SPECIAL_GADGET,
	SPECIAL_GADGET_ATTACHMENT1,
	SPECIAL_GADGET_ATTACHMENT2,
	SPECIAL_GADGET_ATTACHMENT3,
	SPECIALTY1,
	SPECIALTY2,
	SPECIALTY3,
	SPECIALTY4,
	SPECIALTY5,
	SPECIALTY6,
	CYBERCOM_TACRIG1,
	CYBERCOM_TACRIG2,
	HERO_WEAPON,
	HERO_GADGET,
	CYBERCORE,
	CYBERCOM_ABILITY1,
	CYBERCOM_ABILITY2,
	CYBERCOM_ABILITY3,
	BONUSCARD1,
	BONUSCARD2,
	BONUSCARD3,
	KILLSTREAK1,
	KILLSTREAK2,
	KILLSTREAK3,
	KILLSTREAK4,
	COUNT // This will hold the count of the enum items
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


enum WeaponType {
	WEAPON_NULL = 0,
	WEAPON_PISTOL_STANDARD = 1,
	WEAPON_PISTOL_BURST = 2,
	WEAPON_PISTOL_FULLAUTO = 3,
	WEAPON_PISTOL_STANDARD_DW = 4,
	WEAPON_PISTOL_BURST_DW = 5,
	WEAPON_PISTOL_FULLAUTO_DW = 6,
	WEAPON_AR_PEACEKEEPER = 7,
	WEAPON_PISTOL_SHOTGUN = 8,
	WEAPON_PISTOL_SHOTGUN_DW = 9,
	WEAPON_SMG_STANDARD = 10,
	WEAPON_SMG_BURST = 11,
	WEAPON_SMG_FASTFIRE = 12,
	WEAPON_SMG_LONGRANGE = 13,
	WEAPON_SMG_VERSATILE = 14,
	WEAPON_SMG_CAPACITY = 15,
	WEAPON_SMG_AK74U = 16,
	WEAPON_SMG_MP40 = 17,
	WEAPON_SMG_RECHAMBER = 18,
	WEAPON_SMG_NAILGUN = 19,
	WEAPON_AR_STANDARD = 20,
	WEAPON_AR_ACCURATE = 21,
	WEAPON_AR_CQB = 22,
	WEAPON_AR_DAMAGE = 23,
	WEAPON_AR_FASTBURST = 24,
	WEAPON_AR_LONGBURST = 25,
	WEAPON_AR_MARKSMAN = 26,
	WEAPON_AR_AN94 = 27,
	WEAPON_AR_GARAND = 28,
	WEAPON_AR_FAMAS = 29,
	WEAPON_LMG_CQB = 30,
	WEAPON_LMG_HEAVY = 31,
	WEAPON_LMG_LIGHT = 32,
	WEAPON_LMG_SLOWFIRE = 33,
	WEAPON_SMG_MSMC = 34,
	WEAPON_LMG_INFINITE = 35,
	WEAPON_AR_PULSE = 36,
	WEAPON_AR_M16 = 37,
	WEAPON_SMG_PPSH = 38,
	WEAPON_LAUNCHER_EX41 = 39,
	WEAPON_SNIPER_FASTBOLT = 40,
	WEAPON_SNIPER_FASTSEMI = 41,
	WEAPON_SNIPER_POWERBOLT = 42,
	WEAPON_SNIPER_CHARGESHOT = 43,
	WEAPON_SHOTGUN_OLYMPIA = 44,
	WEAPON_SNIPER_QUICKSCOPE = 45,
	WEAPON_SNIPER_DOUBLE = 46,
	WEAPON_PISTOL_ENERGY = 47,
	WEAPON_PISTOL_M1911 = 48,
	WEAPON_PISTOL_M1911_DW = 49,
	WEAPON_SHOTGUN_FULLAUTO = 50,
	WEAPON_SHOTGUN_PRECISION = 51,
	WEAPON_SHOTGUN_PUMP = 52,
	WEAPON_SHOTGUN_SEMIAUTO = 53,
	WEAPON_SPECIAL_CROSSBOW = 54,
	WEAPON_SPECIAL_CROSSBOW_DW = 55,
	WEAPON_SHOTGUN_ENERGY = 56,
	WEAPON_LAUNCHER_MULTI = 57,
	WEAPON_LAUNCHER_STANDARD = 58,
	WEAPON_LAUNCHER_LOCKONLY = 59,
	WEAPON_FRAGGRENADE = 61,
	WEAPON_HATCHET = 62,
	WEAPON_STICKY_GRENADE = 63,
	WEAPON_SATCHEL_CHARGE = 64,
	WEAPON_BOUNCINGBETTY = 65,
	WEAPON_INCENDIARY_GRENADE = 66,
	WEAPON_MELEE_SWORD = 67,
	WEAPON_MELEE_MACE = 68,
	WEAPON_WILLY_PETE = 70,
	WEAPON_STUN_GRENADE = 71,
	WEAPON_EMPGRENADE = 72,
	WEAPON_FLASHBANG = 74,
	WEAPON_PROXIMITY_GRENADE = 75,
	WEAPON_PDA_HACK = 76,
	WEAPON_MELEE_BAT = 77,
	WEAPON_TROPHY_SYSTEM = 78,
	WEAPON_BARE_HANDS = 79,
	WEAPON_GADGET_CAMO = 80,
	WEAPON_GADGET_VISION_PULSE = 81,
	WEAPON_GADGET_ARMOR = 82,
	WEAPON_MELEE_BOWIE = 83,
	WEAPON_MELEE_DAGGER = 84,
	WEAPON_MELEE_CROWBAR = 85,
	WEAPON_GADGET_SPEED_BURST = 86,
	WEAPON_GADGET_COMBAT_EFFICIENCY = 87,
	WEAPON_GADGET_FLASHBACK = 88,
	WEAPON_GADGET_CLONE = 89,
	WEAPON_GADGET_RESURRECT = 90,
	WEAPON_GADGET_HEAT_WAVE = 91,
	WEAPON_GADGET_ROULETTE = 92,
	WEAPON_GADGET_THIEF = 93,
	WEAPON_MELEE_FIREAXE = 94,
	WEAPON_MELEE_KNUCKLES = 95,
	WEAPON_MELEE_BUTTERFLY = 96,
	WEAPON_MELEE_WRENCH = 97,
	WEAPON_SNIPER_XPR50 = 98,
	WEAPON_KNIFE_LOADOUT = 99,
	WEAPON_BOWIE = 100,
	WEAPON_MINIGUN = 101,
	WEAPON_KNIFE = 102,
	WEAPON_MELEE_BONEGLASS = 103,
	WEAPON_BALL = 104,
	WEAPON_MELEE_IMPROVISE = 105,
	WEAPON_BLACKJACK_COIN = 106,
	WEAPON_BLACKJACK_COIN_DUP = 107,
	WEAPON_MELEE_SHOCKBATON = 108,
	WEAPON_MELEE_NUNCHUKS = 109,
	WEAPON_SMG_STEN = 110,
	WEAPON_HERO_MINIGUN = 111,
	WEAPON_HERO_LIGHTNINGGUN = 112,
	WEAPON_HERO_GRAVITYSPIKES = 113,
	WEAPON_HERO_ARMBLADE = 114,
	WEAPON_HERO_ANNIHILATOR = 115,
	WEAPON_HERO_PINEAPPLEGUN = 116,
	WEAPON_HERO_BOWLAUNCHER = 117,
	WEAPON_HERO_CHEMICALGELGUN = 118,
	WEAPON_HERO_FLAMETHROWER = 119,
	WEAPON_MELEE_BOXING = 120,
	WEAPON_MELEE_KATANA = 121,
	WEAPON_MELEE_SHOVEL = 122,
	WEAPON_SPECIAL_DISCGUN = 123,
	WEAPON_MELEE_PROSTHETIC = 124,
	WEAPON_MELEE_CHAINSAW = 125,
	WEAPON_AR_GALIL = 126,
	WEAPON_SPECIAL_KNIFE_BALLISTIC = 127,
	WEAPON_LMG_RPK = 128,
	WEAPON_AR_M14 = 129,
	WEAPON_SNIPER_MOSIN = 130,
	WEAPON_MELEE_CRESCENT = 147,
	WEAPON_DESTRUCTIBLE_CAR = 193
};

enum Perk {
	PERKS_FLAK_JACKET = 148,
	PERKS_BLIND_EYE = 149,
	PERKS_HARDLINE = 150,
	PERKS_GHOST = 151,
	PERKS_JETQUIET = 152,
	PERKS_LIGHTWEIGHT = 153,
	PERKS_JETCHARGER = 154,
	PERKS_OVERCHARGE = 155,
	PERKS_HARD_WIRED = 156,
	PERKS_SCAVENGER = 157,
	PERKS_COLD_BLOODED = 158,
	PERKS_FAST_HANDS = 159,
	PERKS_TOUGHNESS = 160,
	PERKS_TRACKER = 161,
	PERKS_ANTEUP = 162,
	PERKS_DEXTERITY = 163,
	PERKS_HACKER = 164,
	PERKS_NINJA = 165,
	PERKS_TACTICAL_MASK = 166,
	PERKS_AWARENESS = 167,
	PERKS_SIXTH_SENSE = 168,
	PERKS_EXTREME_CONDITIONING = 169,
	PERKS_GUNG_HO = 170,
	PERKS_NONE = 194
};

enum BonusCard {
	BONUSCARD_PRIMARY_GUNFIGHTER_1 = 178,
	BONUSCARD_PRIMARY_GUNFIGHTER_2 = 179,
	BONUSCARD_PRIMARY_GUNFIGHTER_3 = 180,
	BONUSCARD_SECONDARY_GUNFIGHTER = 181,
	BONUSCARD_OVERKILL = 182,
	BONUSCARD_PERK_1_GREED = 183,
	BONUSCARD_PERK_2_GREED = 184,
	BONUSCARD_PERK_3_GREED = 185,
	BONUSCARD_DANGER_CLOSE = 186,
	BONUSCARD_TWO_TACTICALS = 187
};

enum Killstreak {
	KILLSTREAK_NULL = 197,
	KILLSTREAK_RCBOMB = 198,
	KILLSTREAK_RECON = 199,
	KILLSTREAK_MISSILE_DRONE = 200,
	KILLSTREAK_COUNTER_UAV = 201,
	KILLSTREAK_SUPPLY_DROP = 202,
	KILLSTREAK_MICROWAVE_TURRET = 203,
	KILLSTREAK_REMOTE_MISSILE = 204,
	KILLSTREAK_PLANEMORTAR = 205,
	KILLSTREAK_AUTO_TURRET = 207,
	KILLSTREAK_MINIGUN_DROP = 208,
	KILLSTREAK_QRDRONE = 210,
	KILLSTREAK_AI_TANK_DROP = 211,
	KILLSTREAK_HELICOPTER_COMLINK = 212,
	KILLSTREAK_SATELLITE = 213,
	KILLSTREAK_HELICOPTER_GUARD = 214,
	KILLSTREAK_STRAFERUN = 215,
	KILLSTREAK_EMP = 216,
	KILLSTREAK_REMOTE_MORTAR = 217,
	KILLSTREAK_HELICOPTER_GUNNER = 218,
	KILLSTREAK_DOGS = 219,
	KILLSTREAK_MISSILE_SWARM = 220,
	KILLSTREAK_HELICOPTER_PLAYER = 221,
	KILLSTREAK_RAPS = 222,
	KILLSTREAK_DRONE_STRIKE = 223,
	KILLSTREAK_DART = 224,
	KILLSTREAK_SENTINEL = 225,
	KILLSTREAK_COMBAT_ROBOT = 226,
	KILLSTREAK_NULL_DUP = 227 // Assuming it’s a duplicate of 197
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
	STORAGE_GUNSMITH = 0x1A, // 24
	STORAGE_PAINTJOBS = 0x1B, // 25
	STORAGE_EMBLEMS = 0x1C, // 26
	STORAGE_DEFAULT_EMBLEMS = 0x1D,
	STORAGE_EMBLEMS_LOOT = 0x1E,
	STORAGE_CUSTOM_GAMES = 0x1F,
	STORAGE_OFFICIAL_CUSTOM_GAMES = 0x20,
	STORAGE_EXTERNAL_DATA = 0x21,
	STORAGE_FILE_COUNT = 0x22,
	STORAGE_FILE_FIRST = 0x0,
	STORAGE_FILE_INVALID = 0xFFFFFFFF,
};

enum Feature {
	FEATURE_KILLSTREAKS = 228,
	FEATURE_EMBLEM_TYPE_A_Z = 229,
	FEATURE_EMBLEM_NUMBERS = 230,
	FEATURE_CLOSEASSAULT = 233,
	FEATURE_SNIPER = 234,
	FEATURE_CREATE_A_CLASS = 235,
	FEATURE_COMBAT_RECORD = 237,
	FEATURE_CLAN_TAG = 238,
	FEATURE_PRESTIGE_LB = 239,
	FEATURE_CHALLENGES = 240,
	FEATURE_CHALLENGES_SPECIALS = 241,
	FEATURE_PRESTIGE_CLASS_1 = 242,
	FEATURE_PRESTIGE_CLASS_2 = 243,
	FEATURE_PRESTIGE_CLASS_3 = 244,
	FEATURE_PRESTIGE_CLASS_4 = 245,
	FEATURE_PRESTIGE_CLASS_5 = 246,
	FEATURE_CHALLENGES_KILLSTREAKS = 248,
	FEATURE_CHALLENGES_ELITE = 249,
	FEATURE_CHALLENGES_FINAL = 250,
	FEATURE_CHALLENGES_GAMEMODE = 251,
	FEATURE_CHALLENGES_MEDALS = 252,
	FEATURE_ALLOCATION_SLOT_1 = 253,
	FEATURE_ALLOCATION_SLOT_2 = 254,
	FEATURE_ALLOCATION_SLOT_3 = 255
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