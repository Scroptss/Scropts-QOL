#pragma once
#include "includes.h"


struct vec2_t {
	int x;
	int y;
};

enum HookReturn
{
	Success,
	ModuleNotFound,
	D3D11CreateDeviceAndSwapChainFailed
};

//enum ControllerIndex_t
//{
//	INVALID_CONTROLLER_PORT = 0xFFFFFFFF,
//	CONTROLLER_INDEX_0 = 0x0,
//	CONTROLLER_INDEX_FIRST = 0x0,
//	CONTROLLER_INDEX_1 = 0x1,
//	CONTROLLER_INDEX_2 = 0x2,
//	CONTROLLER_INDEX_3 = 0x3,
//	CONTROLLER_INDEX_COUNT = 0x4,
//};

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

//template <typename Ret, typename... Args>
//static inline auto spoof_call(
//	const void* trampoline,
//	Ret(*fn)(Args...),
//	Args... args
//) -> Ret
//{
//	struct shell_params
//	{
//		const void* trampoline;
//		void* function;
//		void* rbx;
//	};
//
//	shell_params p{ trampoline, reinterpret_cast<void*>(fn) };
//	using mapper = detail::argument_remapper<sizeof...(Args), void>;
//	return mapper::template do_call<Ret, Args...>((const void*)&detail::_spoofer_stub, &p, args...);
//}