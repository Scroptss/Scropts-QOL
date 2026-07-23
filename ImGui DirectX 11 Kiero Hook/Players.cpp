#include "players.h"

#include <Windows.h>
#include <array>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <mutex>
#include <sstream>
#include <ctime>
#include <cstdint>

namespace players
{
	namespace
	{
		using SteamFriends_fn = void* (*)();

		const static auto Live_SendInvite =
			reinterpret_cast<void(__fastcall*)(int ControllerIndex, std::uint64_t, bool)>(
				OFFSET(0x1E72490)
				);

		const static auto LobbyHost_LobbyJoinBegin =
			reinterpret_cast<bool(*)(int, int, LobbyType, LobbyType)>(
				OFFSET(0x1ECD180)
				);

		const static auto LobbyJoin_Add =
			reinterpret_cast<bool(*)(uint64_t, const char*, const bdSecurityID*, const bdSecurityKey*, const SerializedAdr*, JoinType, uint64_t)>(
				OFFSET(0x1ED8210)
				);

		const static auto LobbyJoinSource_Finalize =
			reinterpret_cast<bool(*)()>(
				OFFSET(0x1ED9AD0)
				);

		const static auto Live_IsUserSignedInToDemonware =
			reinterpret_cast<bool(*)(int controllerIndex)>(
				OFFSET(0x1E013D0)
				);

		const static auto LobbyMsg_SendInfoRequest =
			reinterpret_cast<bool(__fastcall*)(int ControllerIndex_t, const std::uint64_t*, int, const Msg_InfoRequest*)>(
				OFFSET(0x1ED91C0)
				);

		const static auto dwCommonAddrToNetadr =
			reinterpret_cast<bool(*)(netadr_t* const, const void* const, const bdSecurityID*)>(
				OFFSET(0x143C380)
				);

		const static auto dwRegisterSecIDAndKey =
			reinterpret_cast<bool(*)(const bdSecurityID*, const bdSecurityKey*)>(
				OFFSET(0x143E140)
				);

		const static auto Cbuf_AddText =
			reinterpret_cast<std::uintptr_t(__fastcall*)(int, const char*)>(
				OFFSET(0x20DFF50)
				);

		constexpr int kFriendFlagImmediate = 0x4;

		struct friend_entry_t
		{
			std::uint64_t xuid{};
			std::string name;
			std::uint64_t gameID;

			int personaState{};
			bool isOnline{};
			bool isInGame{};

			bool hasNetadr{};
			netadr_t netadr{};

			bool hasHostInfo{};
			HostInfo hostInfo{};

			bool queryPending{};
			DWORD lastInfoRequest{};
			DWORD lastInfoResponse{};
		};

		std::mutex g_friendsMutex;
		std::vector<friend_entry_t> g_friends;
	}

	XSESSION_INFO get_session_info(const InfoResponseLobby& lobby)
	{
		XSESSION_INFO sess_info{};
		sess_info.sessionID = lobby.secId;
		sess_info.keyExchangeKey = lobby.secKey;
		sess_info.hostAddress = lobby.serializedAdr.xnaddr;

		return sess_info;
	}

	void connect_to_session(const HostInfo& info)
	{
		const auto begin = LobbyHost_LobbyJoinBegin(
			0,
			0,
			LOBBY_TYPE_PRIVATE,
			LOBBY_TYPE_GAME
		);

		if (!begin)
			return;

		LobbyJoin_Add(
			info.xuid,
			info.name,
			&info.secId,
			&info.secKey,
			&info.serializedAdr,
			JOIN_TYPE_NORMAL,
			info.xuid
		);

		LobbyJoinSource_Finalize();
	}

	static bool BuildHostInfoFromLobby(
		const InfoResponseLobby& lobby,
		const std::string& fallbackName,
		HostInfo& outHost,
		netadr_t& outAdr
	)
	{
		if (!lobby.isValid)
			return false;

		const auto sessInfo = get_session_info(lobby);

		ZeroMemory(&outHost, sizeof(outHost));
		ZeroMemory(&outAdr, sizeof(outAdr));

		dwRegisterSecIDAndKey(
			&sessInfo.sessionID,
			&sessInfo.keyExchangeKey
		);

		const bool resolved = dwCommonAddrToNetadr(
			&outAdr,
			&sessInfo.hostAddress,
			&sessInfo.sessionID
		);

		if (!resolved || !outAdr.inaddr)
			return false;

		outHost.xuid = lobby.hostXuid;

		if (!fallbackName.empty())
			strncpy_s(outHost.name, fallbackName.c_str(), _TRUNCATE);
		else
			strncpy_s(outHost.name, "Unknown", _TRUNCATE);

		outHost.secId = sessInfo.sessionID;
		outHost.secKey = sessInfo.keyExchangeKey;
		outHost.serializedAdr = lobby.serializedAdr;

		return true;
	}

	void OnInfoResponse(std::uint64_t senderXuid, const Msg_InfoResponse& response)
	{
		const auto& lobby = response.lobby[0];

		if (!lobby.isValid)
		{
			//log::info("players: ignored invalid infoResponse from xuid=%llX", senderXuid);
			return;
		}

		const std::uint64_t hostXuid = lobby.hostXuid ? lobby.hostXuid : senderXuid;

		std::scoped_lock lock(g_friendsMutex);

		for (auto& f : g_friends)
		{
			if (f.xuid != hostXuid && f.xuid != senderXuid)
				continue;

			HostInfo host{};
			netadr_t adr{};

			if (!BuildHostInfoFromLobby(lobby, f.name, host, adr))
			{
				f.queryPending = false;
				//log::info("players: failed to build HostInfo from infoResponse xuid=%llX", hostXuid);
				return;
			}

			f.netadr = adr;
			f.hasNetadr = true;

			f.hostInfo = host;
			f.hasHostInfo = true;

			f.queryPending = false;
			f.lastInfoResponse = GetTickCount();

			/*log::info(
				"players: cached fast-join HostInfo for xuid=%llX",
				f.xuid
			);*/

			return;
		}

		/*log::info(
			"players: got infoResponse for unknown xuid=%llX",
			hostXuid
		);*/
	}

	bool g_autoRefresh = true;
	DWORD g_lastRefresh = 0;
	constexpr DWORD kRefreshMs = 30000;
	constexpr DWORD kInfoRequestTimeoutMs = 7000;
	constexpr DWORD kResolvedInfoStaleMs = 45000;
	constexpr DWORD kAutoPingIntervalMs = 10000;
	constexpr int kAutoPingMaxPerTick = 2;

	DWORD g_lastAutoPing = 0;
	std::size_t g_autoPingCursor = 0;

	char g_searchBuffer[128]{};

	using SteamFriendsAccessor_t = void* (*)();

	void send_info_request(std::uint64_t targetXuid)
	{
		if (!Live_IsUserSignedInToDemonware(0))
			return;

		const Msg_InfoRequest message{ 1 };

		LobbyMsg_SendInfoRequest(
			0,
			&targetXuid,
			1,
			&message
		);
	}

	void AutoPingResolvableFriends()
	{
		const DWORD now = GetTickCount();

		if (now - g_lastAutoPing < kAutoPingIntervalMs)
			return;

		g_lastAutoPing = now;

		std::vector<std::uint64_t> toPing;

		{
			std::scoped_lock lock(g_friendsMutex);

			if (g_friends.empty())
				return;

			std::size_t checked = 0;

			while (checked < g_friends.size() && toPing.size() < kAutoPingMaxPerTick)
			{
				if (g_autoPingCursor >= g_friends.size())
					g_autoPingCursor = 0;

				auto& entry = g_friends[g_autoPingCursor++];

				++checked;

				if (entry.gameID != 311210)
					continue;

				if (entry.hasHostInfo)
					continue;

				if (entry.queryPending)
					continue;

				entry.queryPending = true;
				entry.lastInfoRequest = now;
				entry.hasNetadr = false;
				entry.hasHostInfo = false;
				entry.lastInfoResponse = 0;

				ZeroMemory(&entry.netadr, sizeof(entry.netadr));
				ZeroMemory(&entry.hostInfo, sizeof(entry.hostInfo));

				toPing.emplace_back(entry.xuid);
			}
		}

		for (const auto xuid : toPing)
		{
			send_info_request(xuid);
			//log::info("players: auto sent info request to %llX", xuid);
		}
	}

	const char* PersonaStateToString(int state)
	{
		switch (state)
		{
		case 0: return "Offline";
		case 1: return "Online";
		case 2: return "Busy";
		case 3: return "Away";
		case 4: return "Snooze";
		case 5: return "Looking to Trade";
		case 6: return "Looking to Play";
		case 7: return "Invisible";
		default: return "Unknown";
		}
	}

	void UpdateFriendSessionTimeouts()
	{
		const DWORD now = GetTickCount();

		std::scoped_lock lock(g_friendsMutex);

		for (auto& entry : g_friends)
		{
			if (entry.queryPending && now - entry.lastInfoRequest > kInfoRequestTimeoutMs)
			{
				entry.queryPending = false;
			}

			if (entry.hasHostInfo && entry.lastInfoResponse != 0 &&
				now - entry.lastInfoResponse > kResolvedInfoStaleMs)
			{
				entry.hasHostInfo = false;
				entry.hasNetadr = false;
				entry.lastInfoResponse = 0;

				ZeroMemory(&entry.hostInfo, sizeof(entry.hostInfo));
				ZeroMemory(&entry.netadr, sizeof(entry.netadr));
			}
		}
	}

	void RequestFriendSessionByXuid(std::uint64_t xuid)
	{
		bool shouldSend = false;

		{
			std::scoped_lock lock(g_friendsMutex);

			for (auto& entry : g_friends)
			{
				if (entry.xuid != xuid)
					continue;

				entry.queryPending = true;
				entry.lastInfoRequest = GetTickCount();

				entry.hasNetadr = false;
				entry.hasHostInfo = false;
				entry.lastInfoResponse = 0;

				ZeroMemory(&entry.netadr, sizeof(entry.netadr));
				ZeroMemory(&entry.hostInfo, sizeof(entry.hostInfo));

				shouldSend = true;
				break;
			}
		}

		if (!shouldSend)
		{
			//log::info("players: tried to request unknown friend xuid=%llX", xuid);
			return;
		}

		send_info_request(xuid);

		//log::info("players: sent info request to %llX", xuid);
	}

	void* AcquireSteamFriends()
	{
		__try
		{
			void* friends = *reinterpret_cast<void**>(OFFSET(0x10B3DC20));

			if (!friends)
			{
				//log::info("players: STEAMAPI_FRIENDS is null.");
				return nullptr;
			}

			auto vtable = *reinterpret_cast<void***>(friends);

			if (!vtable)
			{
				//log::info("players: STEAMAPI_FRIENDS vtable is null.");
				return nullptr;
			}

			for (int i = 0; i < 8; ++i)
			{
				if (!vtable[i])
				{
					//log::info("players: STEAMAPI_FRIENDS vtable[%d] is null.", i);
					return nullptr;
				}
			}

			return friends;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			//log::info("players: exception while reading STEAMAPI_FRIENDS.");
			return nullptr;
		}
	}

	std::string XuidToHex(std::uint64_t xuid)
	{
		char buffer[32]{};
		sprintf_s(buffer, "%llX", xuid);
		return buffer;
	}

	bool IsResolvedOnline(const friend_entry_t& entry)
	{
		return entry.isOnline && entry.hasHostInfo && entry.gameID == 311210;
	}

	int FriendSortRank(const friend_entry_t& entry)
	{
		if (IsResolvedOnline(entry))
			return 0;

		if (entry.isOnline)
			return 1;

		return 2;
	}

	bool IsAwayState(int state)
	{
		return state >= 3;
	}

	bool IsResolvedBo3(const friend_entry_t& entry)
	{
		return entry.isOnline && entry.hasHostInfo && entry.gameID == 311210;
	}

	ImVec4 FriendStatusColor(const friend_entry_t& entry)
	{
		if (IsResolvedBo3(entry))
			return ImVec4(1.0f, 0.55f, 0.12f, 1.0f);

		if (entry.isOnline && entry.isInGame)
			return ImVec4(0.45f, 0.85f, 0.25f, 1.0f);

		if (entry.personaState == 1)
			return ImVec4(0.45f, 0.75f, 1.0f, 1.0f);

		if (IsAwayState(entry.personaState))
			return ImVec4(0.25f, 0.45f, 0.65f, 1.0f);

		return ImVec4(0.55f, 0.55f, 0.55f, 1.0f);
	}

	const char* FriendStatusText(const friend_entry_t& entry)
	{
		if (IsResolvedBo3(entry) || entry.gameID == 311210)
			return "In BO3";

		if (entry.isOnline && entry.isInGame)
			return "In Game";

		return PersonaStateToString(entry.personaState);
	}

	struct SteamFriendGameInfo_t
	{
		std::uint64_t gameID{};
		std::uint32_t gameIP{};
		std::uint16_t gamePort{};
		std::uint16_t queryPort{};
		std::uint64_t lobbySteamID{};
	};

	using SteamAPI_ISteamFriends_GetFriendGamePlayed_t =
		bool(__cdecl*)(void* self, std::uint64_t steamIDFriend, SteamFriendGameInfo_t* gameInfo);

	using SteamAPI_ISteamFriends_GetFriendCount_t =
		int(__cdecl*)(void* self, int friendFlags);

	using SteamAPI_ISteamFriends_GetFriendByIndex_t =
		std::uint64_t(__cdecl*)(void* self, int index, int friendFlags);

	using GetFriendPersonaState_fn =
		int(__fastcall*)(void* self, std::uint64_t steamID);

	using GetFriendPersonaName_fn =
		const char* (__fastcall*)(void* self, std::uint64_t steamID);

	using SteamAPI_ISteamFriends_InviteUserToGame_t =
		bool(__cdecl*)(void* self, std::uint64_t steamIDFriend, const char* connectString);

	using SteamAPI_ISteamUser_GetSteamID_t =
		std::uint64_t(__cdecl*)(void* self);

	using SteamAPI_SteamUser_vXXX_t =
		void* (__cdecl*)();


	void RefreshFriends()
	{
		void* friends = AcquireSteamFriends();
		if (!friends)
		{
			//log::info("players: failed to acquire Steam friends interface.");
			return;
		}

		HMODULE steamapi = GetModuleHandleA("steam_api64.dll");
		if (!steamapi)
		{
			//log::info("players: steam_api64.dll not loaded.");
			return;
		}

		auto GetFriendCount = reinterpret_cast<SteamAPI_ISteamFriends_GetFriendCount_t>(
			GetProcAddress(steamapi, "SteamAPI_ISteamFriends_GetFriendCount")
			);

		auto GetFriendByIndex = reinterpret_cast<SteamAPI_ISteamFriends_GetFriendByIndex_t>(
			GetProcAddress(steamapi, "SteamAPI_ISteamFriends_GetFriendByIndex")
			);

		if (!GetFriendCount || !GetFriendByIndex)
		{
			/*log::info(
				"players: SteamFriends wrapper exports missing. GetFriendCount=%p GetFriendByIndex=%p",
				GetFriendCount,
				GetFriendByIndex
			);*/
			return;
		}

		auto vt = *reinterpret_cast<std::uintptr_t**>(friends);

		if (!vt)
		{
			//log::info("players: SteamFriends vtable was null.");
			return;
		}

		constexpr int VT_GetFriendPersonaState = 6;
		constexpr int VT_GetFriendPersonaName = 7;

		auto GetFriendPersonaState = reinterpret_cast<GetFriendPersonaState_fn>(
			vt[VT_GetFriendPersonaState]
			);

		auto GetFriendPersonaName = reinterpret_cast<GetFriendPersonaName_fn>(
			vt[VT_GetFriendPersonaName]
			);

		if (!GetFriendPersonaState || !GetFriendPersonaName)
		{
			//log::info("players: SteamFriends persona vfuncs were null.");
			return;
		}

		auto GetFriendGamePlayed = reinterpret_cast<SteamAPI_ISteamFriends_GetFriendGamePlayed_t>(
			GetProcAddress(steamapi, "SteamAPI_ISteamFriends_GetFriendGamePlayed")
			);

		if (!GetFriendGamePlayed)
		{
			//log::info("players: GetFriendGamePlayed export missing; in-game status unavailable.");
		}

		int count = 0;

		count = GetFriendCount(friends, kFriendFlagImmediate);

		//log::info("players: friend count = %d.", count);

		if (count < 0 || count > 10000)
		{
			//log::info("players: implausible friend count %d.", count);
			return;
		}

		std::vector<friend_entry_t> next;
		next.reserve(static_cast<std::size_t>(count));

		for (int i = 0; i < count; ++i)
		{
			std::uint64_t xuid = 0;

			xuid = GetFriendByIndex(
				friends,
				i,
				kFriendFlagImmediate
			);

			if (!xuid)
				continue;

			friend_entry_t entry{};
			entry.xuid = xuid;

			int personaState = 0;

			personaState = GetFriendPersonaState(friends, xuid);

			entry.personaState = personaState;
			entry.isOnline = personaState != 0;
			entry.isInGame = false;

			if (entry.isOnline && GetFriendGamePlayed)
			{
				SteamFriendGameInfo_t gameInfo{};
				entry.isInGame = GetFriendGamePlayed(
					friends,
					xuid,
					&gameInfo
				);

				entry.gameID = gameInfo.gameID;
			}

			const char* name = nullptr;


			name = GetFriendPersonaName(friends, xuid);


			entry.name = name && *name ? name : "Unknown";

			{
				std::scoped_lock lock(g_friendsMutex);

				const auto old = std::find_if(
					g_friends.begin(),
					g_friends.end(),
					[xuid](const friend_entry_t& f)
					{
						return f.xuid == xuid;
					}
				);

				if (old != g_friends.end())
				{
					entry.hasNetadr = old->hasNetadr;
					entry.netadr = old->netadr;

					entry.hasHostInfo = old->hasHostInfo;
					entry.hostInfo = old->hostInfo;

					entry.queryPending = old->queryPending;
					entry.lastInfoRequest = old->lastInfoRequest;
					entry.lastInfoResponse = old->lastInfoResponse;
				}
			}

			next.emplace_back(std::move(entry));
		}

		{
			std::scoped_lock lock(g_friendsMutex);
			g_friends.swap(next);
		}

		g_lastRefresh = GetTickCount();

		//log::info("players: cached %zu Steam friends.", next.size());
	}

	bool MatchesSearch(const friend_entry_t& entry)
	{
		if (g_searchBuffer[0] == '\0')
			return true;

		std::string query = g_searchBuffer;
		std::string name = entry.name;
		std::string xuidHex = XuidToHex(entry.xuid);
		std::string xuidDec = std::to_string(entry.xuid);

		std::transform(query.begin(), query.end(), query.begin(), ::tolower);
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		std::transform(xuidHex.begin(), xuidHex.end(), xuidHex.begin(), ::tolower);

		return name.find(query) != std::string::npos ||
			xuidHex.find(query) != std::string::npos ||
			xuidDec.find(query) != std::string::npos;
	}

	void SendGameInviteToFriend(std::uint64_t xuid, bool recentPlayerInvite = false)
	{
		if (!xuid)
			return;

		Live_SendInvite(0, xuid, recentPlayerInvite);
	}

	void draw()
	{

		UpdateFriendSessionTimeouts();

		if (g_autoRefresh)
		{
			const DWORD now = GetTickCount();

			if (g_lastRefresh == 0 || now - g_lastRefresh > kRefreshMs)
				RefreshFriends();

			AutoPingResolvableFriends();
		}

		ImGui::BeginChild("##PlayersFriendsList", ImGui::GetContentRegionAvail(), true);

		ImGui::TextUnformatted("Friends List");
		ImGui::Separator();

		if (ImGui::Button("Refresh##FriendsList"))
			RefreshFriends();

		ImGui::SameLine();
		ImGui::Checkbox("Auto refresh", &g_autoRefresh);

		ImGui::InputTextWithHint(
			"##FriendSearch",
			"Search name / XUID...",
			g_searchBuffer,
			sizeof(g_searchBuffer)
		);

		ImGui::Spacing();

		std::vector<friend_entry_t> snapshot;
		{
			std::scoped_lock lock(g_friendsMutex);
			snapshot = g_friends;
		}

		auto FriendSortRank = [](const friend_entry_t& entry)
			{
				if (entry.isOnline && entry.hasHostInfo)
					return 0;

				if (entry.isOnline && entry.isInGame)
					return 1;

				if (entry.personaState == 1)
					return 2;

				if (IsAwayState(entry.personaState))
					return 3;

				return 4;
			};

		std::sort(
			snapshot.begin(),
			snapshot.end(),
			[&](const friend_entry_t& a, const friend_entry_t& b)
			{
				const int rankA = FriendSortRank(a);
				const int rankB = FriendSortRank(b);

				if (rankA != rankB)
					return rankA < rankB;

				return a.name < b.name;
			}
		);

		ImGui::Text("Cached friends: %zu", snapshot.size());

		if (ImGui::BeginTable(
			"##FriendsTable",
			3,
			ImGuiTableFlags_Borders |
			ImGuiTableFlags_RowBg |
			ImGuiTableFlags_Resizable |
			ImGuiTableFlags_ScrollY,
			ImVec2(0.0f, 0.0f)
		))
		{
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 90.0f);
			ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 130.0f);
			ImGui::TableHeadersRow();

			for (const auto& entry : snapshot)
			{
				if (!MatchesSearch(entry))
					continue;

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(entry.name.c_str());

				ImGui::TableSetColumnIndex(1);

				ImGui::TextColored(
					FriendStatusColor(entry),
					"%s",
					FriendStatusText(entry)
				);

				ImGui::TableSetColumnIndex(2);

				if (entry.gameID == 311210)
				{
					if (ImGui::SmallButton(("Join##" + std::to_string(entry.xuid)).c_str()))
					{
						const auto command = "join " + std::to_string(entry.xuid);
						Cbuf_AddText(0, command.data());
					}

					ImGui::SameLine();

					if (ImGui::SmallButton(("Invite##" + std::to_string(entry.xuid)).c_str()))
					{
						SendGameInviteToFriend(entry.xuid);
					}
				}
				else if (entry.isOnline)
				{
					if (ImGui::SmallButton(("Invite##" + std::to_string(entry.xuid)).c_str()))
					{
						SendGameInviteToFriend(entry.xuid);
					}
				}

			}

			ImGui::EndTable();
		}

		ImGui::EndChild();
	}

	bool Install()
	{
		//RefreshFriends();
		return true;
	}
}