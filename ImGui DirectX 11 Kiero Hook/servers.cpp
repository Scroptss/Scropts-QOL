#include "servers.h"
#include "structs.h"
#include <Windows.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <limits>

namespace servers
{
	namespace
	{
		// ---------------------------------------------------------------------
		// UI/session state
		// ---------------------------------------------------------------------

		struct session_view_t
		{
			MatchMakingInfo session{};
			std::string typeLabel;
			std::string gameTypeLabel;
			std::string latencyLabel;
		};

		struct player_info_t
		{
			int clientNum{};
			std::string name;
		};

		struct server_status_t
		{
			std::string mapName;
			std::string gameType;
			std::string prettyName;
			std::vector<player_info_t> players;
			DWORD lastRequestMs{};
			DWORD lastResponseMs{};
		};

		struct cached_netadr_t
		{
			netadr_t adr{};
			DWORD lastUpdateMs{};
			bool valid{};
		};

		MatchMakingQuery query_info{};
		std::array<MatchMakingInfo, 800> search_results{};

		std::mutex sessions_mutex{};
		std::vector<MatchMakingInfo> sessions{};

		std::vector<session_view_t> g_viewRows{};
		DWORD g_lastViewBuildMs{};
		bool g_viewDirty{ true };

		constexpr DWORD kFetchIntervalMs = 6000;
		constexpr DWORD kViewRebuildIntervalMs = 1000;

		DWORD g_lastFetchMs{};

		bool bAutoFetch = true;
		bool bP2P = false;
		bool bEmpty = false;
		bool bFull = false;
		bool bLessHalf = false;

		std::mutex status_mutex{};
		using status_pair_t = std::pair<std::string, std::string>;

		std::unordered_map<std::string, std::vector<player_info_t>> g_playersByAdr{};
		std::unordered_map<std::string, status_pair_t> g_infoByAdr{};

		std::mutex netadr_cache_mutex{};
		std::unordered_map<std::uint64_t, cached_netadr_t> g_netadrCache{};

		constexpr DWORD kNetadrCacheMs = 60000;

		// ---------------------------------------------------------------------
		// Game funcs
		// ---------------------------------------------------------------------

		const static auto Live_Qos_GetDatacenterById =
			reinterpret_cast<const qosServer_t * (__fastcall*)(std::uint32_t id)>(
				OFFSET(0x1E8A4A0)
				);

		const static auto Live_Qos_GetPingForDatacenter =
			reinterpret_cast<int(__fastcall*)(const qosServer_t * server)>(
				OFFSET(0x1E8A570)
				);

		const static auto TaskManager2_CreateTask =
			reinterpret_cast<TaskRecord * (*)(const void*, const ControllerIndex_t, TaskRecord*, int)>(
				OFFSET(0x22538B0)
				);

		const static auto TaskManager2_StartTask =
			reinterpret_cast<void(*)(TaskRecord*)>(
				OFFSET(0x2254800)
				);

		const static auto TaskManager2_TaskGetInProgress =
			reinterpret_cast<bool(*)(const void*)>(
				OFFSET(0x22548C0)
				);

		const static auto TaskManager2_Tasksmth =
			reinterpret_cast<std::uintptr_t(*)(int)>(
				OFFSET(0x144B2F0)
				);

		const static auto TaskManager2_RevertTask =
			reinterpret_cast<void(*)(TaskRecord*)>(
				OFFSET(0x2254470)
				);

		const static auto TaskManager2_ClearTasks =
			reinterpret_cast<void(*)(const void*)>(
				OFFSET(0x2253750)
				);

		const static auto TaskManager2_searchresultsmth =
			reinterpret_cast<void(*)(void*, unsigned long long, size_t, std::uintptr_t, std::uintptr_t)>(
				OFFSET(0x2BCB1FC)
				);

		const static auto Server_GetServer =
			reinterpret_cast<bdRemoteTask * (*)(const std::uintptr_t, bdRemoteTask**, QueryId, int, size_t, MatchMakingQuery*, MatchMakingInfo*)>(
				OFFSET(0x2891C40)
				);

		const static auto Server_GetServerInfos =
			reinterpret_cast<std::uintptr_t(*)(MatchMakingQuery*, int, bool)>(
				OFFSET(0x144D300)
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

		const static auto Com_GameInfo_GetGameTypeNameForID =
			reinterpret_cast<const char* (__fastcall*)(int id)>(
				OFFSET(0x20E7340)
				);

		const static auto Com_GameInfo_GetGameTypeRef =
			reinterpret_cast<const char* (__fastcall*)(const char* name)>(
				OFFSET(0x20E74F0)
				);

		const static auto UI_SafeTranslateString =
			reinterpret_cast<const char* (__fastcall*)(const char* String)>(
				OFFSET(0x22328F0)
				);

		const static auto Com_GameInfo_GetGameTypeOnMapName =
			reinterpret_cast<std::uint32_t(*)(const char*, const char*, char*, unsigned int)>(
				OFFSET(0x20E7420)
				);

		const static auto NET_OutOfBandPrint =
			reinterpret_cast<bool(__fastcall*)(netsrc_t, netadr_t, const char*)>(
				OFFSET(0x211B310)
				);

		const static auto dwCommonAddrToNetadr =
			reinterpret_cast<bool(*)(netadr_t* const, const void* const, const bdSecurityID*)>(
				OFFSET(0x143C380)
				);

		const static auto dwRegisterSecIDAndKey =
			reinterpret_cast<bool(*)(const bdSecurityID*, const bdSecurityKey*)>(
				OFFSET(0x143E140)
				);

		const static auto MSG_ReadStringLine =
			reinterpret_cast<const char* (__fastcall*)(msg_t*, char*, int)>(
				OFFSET(0x20FED40)
				);

		// ---------------------------------------------------------------------
		// OOB / connectionless hooks
		// ---------------------------------------------------------------------

		using CL_DispatchConnectionlessPacket_t = bool(__fastcall*)(int localClientNum, netadr_t from, msg_t* msg);
		using Sys_SendPacket_t = bool(__fastcall*)(netsrc_t sock, int length, const std::uint8_t* data, netadr_t to);

		CL_DispatchConnectionlessPacket_t o_CL_DispatchConnectionlessPacket{};
		Sys_SendPacket_t o_Sys_SendPacket{};

		bool g_oobHooksInstalled{};
		bool g_logOobCommands{ true };

		constexpr std::uintptr_t CL_DispatchConnectionlessPacket_Offset = 0x134CD50;
		constexpr std::uintptr_t Sys_SendPacket_Offset = 0x2332F70;

		// ---------------------------------------------------------------------
		// Helpers
		// ---------------------------------------------------------------------

		std::string netadr_to_key(const netadr_t& adr)
		{
			char buffer[64]{};

			sprintf_s(
				buffer,
				"%u:%u:%d:%d",
				static_cast<unsigned>(adr.inaddr),
				static_cast<unsigned>(adr.port),
				static_cast<int>(adr.type),
				static_cast<int>(adr.localNetID)
			);

			return buffer;
		}

		std::uint32_t random_u32()
		{
			static thread_local std::random_device rd;
			static thread_local std::mt19937 rng(rd());
			static thread_local std::uniform_int_distribution<std::uint32_t> dist(
				0,
				std::numeric_limits<std::uint32_t>::max()
			);

			return dist(rng);
		}

		bool net_send(const netadr_t& netadr, const std::string& data)
		{
			using GetNetworkId_t = int(__fastcall*)(int);

			using SysSendPacket_t = bool(__fastcall*)(
				int networkId,
				int length,
				const void* data,
				const netadr_t* to
				);

			using SendLoopbackPacket_t = bool(__fastcall*)(
				void* loopbackChannel,
				int unused,
				int networkId,
				const netadr_t* to,
				int length,
				const void* data
				);

			const auto GetNetworkId =
				reinterpret_cast<GetNetworkId_t>(OFFSET(0x20E3890));

			const auto SysSendPacket =
				reinterpret_cast<SysSendPacket_t>(OFFSET(0x22B9900));

			const auto SendLoopbackPacket =
				reinterpret_cast<SendLoopbackPacket_t>(OFFSET(0x211F7C0));

			const int networkId = GetNetworkId(0);
			const int length = static_cast<int>(data.size());

			if (netadr.type != NA_LOOPBACK)
			{
				return SysSendPacket(
					networkId,
					length,
					data.data(),
					&netadr
				);
			}

			const auto loopbackChannel =
				reinterpret_cast<void*>(OFFSET(0x16DEACA0) + 104i64 * netadr.port);

			return SendLoopbackPacket(
				loopbackChannel,
				0,
				networkId,
				&netadr,
				length,
				data.data()
			);
		}

		bool netchan_send(
			const NetChanMsgType type,
			std::string data,
			const netadr_t& netadr,
			std::uint64_t destXuid = 0,
			std::uint64_t srcXuid = 0,
			std::uint16_t nonce = 0)
		{
			if (!destXuid)
				destXuid = 0xDEADFA11;

			if (!srcXuid)
				srcXuid = 0x1111111100000000ULL + random_u32();

			bool result = false;

			constexpr auto maxFragmentSize = 1222;

			if (data.size() > maxFragmentSize * 128)
				return false;

			while (data.size() % maxFragmentSize)
				data.resize(data.size() + 1);

			const auto dataLength = data.size();
			const auto numFragments = std::max<std::size_t>(1, dataLength / maxFragmentSize);

			auto fragments = std::make_unique<NetChanFragment_s[]>(numFragments);

			NetChanMessage_s msg{};

			msg.sequence = 1;

			if (nonce)
			{
				const auto chan = reinterpret_cast<NetChanMessageList_s*>(OFFSET(0x16DEAEB0));
				const auto oldMsg = chan->out[type];

				if (oldMsg)
					msg.sequence = oldMsg->sequence + 1;
			}

			msg.numFragments = static_cast<std::uint8_t>(numFragments);
			msg.complete = false;
			msg.messageLen = static_cast<std::uint32_t>(dataLength);
			msg.destXUID = destXuid;
			msg.sourceXUID = srcXuid;
			msg.sendCount = 0;
			msg.destAddress = netadr;
			msg.msgConfig = nullptr;
			msg.next = nullptr;
			msg.fragments = fragments.get();
			ZeroMemory(msg.acked, sizeof(msg.acked));
			msg.lastAckMS = 0;
			msg.lastTouchedMS = 0;
			msg.lastKeepAliveMs = 0;
			msg.timeoutMS = 0;
			msg.dropped = false;
			msg.nonce = std::numeric_limits<std::uint16_t>::max();

			for (std::size_t i = 0; i < numFragments; ++i)
			{
				auto& f = fragments[i];

				f.sendCount = 0;
				f.sequence = static_cast<std::uint8_t>(i);
				f.size = maxFragmentSize;

				std::memcpy(
					f.msgBuf,
					&data[i * maxFragmentSize],
					f.size
				);

				char buffer[1256]{};
				msg_t m{};

				m.init(buffer, sizeof(buffer));
				m.write<std::uint8_t>(0xE0);
				m.write<std::uint64_t>(msg.destXUID);
				m.write<std::uint64_t>(msg.sourceXUID);
				m.write<std::uint8_t>(static_cast<std::uint8_t>(type));
				m.write<std::uint16_t>(msg.nonce);
				m.write<std::uint32_t>(msg.sequence);
				m.write<std::uint8_t>(f.sequence);
				m.write<std::uint8_t>(msg.numFragments);
				m.write<std::uint16_t>(static_cast<std::uint16_t>(f.size));
				m.write_data(f.msgBuf, f.size);

				result = net_send(
					msg.destAddress,
					std::string(m.data, static_cast<std::size_t>(m.cursize))
				);
			}

			return result;
		}

		bool netchan_send_oob(const netadr_t& netadr, const std::string& data)
		{
			const std::string finalData = std::string("\xFF\xFF\xFF\xFF", 4) + data;
			return netchan_send(NETCHAN_CLIENTMSG, finalData, netadr);
		}

		void prepare_connless_read(msg_t& msg);
		bool parse_status_response(const netadr_t& from, msg_t& msg);

		bool parse_connectionless_command(msg_t& msg, std::string& out)
		{
			prepare_connless_read(msg);

			char buffer[1024]{};
			const char* line = MSG_ReadStringLine(&msg, buffer, sizeof(buffer));

			if (!line || !*line)
				return false;

			std::string value = line;
			const auto space = value.find_first_of(" \t\r\n");

			if (space != std::string::npos)
				value.resize(space);

			if (value.empty())
				return false;

			out = value;
			return true;
		}

		bool __fastcall hk_Sys_SendPacket(netsrc_t sock, int length, const std::uint8_t* data, netadr_t to)
		{
			return o_Sys_SendPacket
				? o_Sys_SendPacket(sock, length, data, to)
				: false;
		}


		bool Live_Qos_Finished()
		{
			return *reinterpret_cast<int*>(OFFSET(0x11392044)) >= 4;
		}

		int get_qos_ping_for_location(std::uint32_t serverLocation)
		{
			if (!Live_Qos_Finished())
				return -1;

			const auto* qos = Live_Qos_GetDatacenterById(serverLocation);

			if (!qos || !qos->numPacketsRecvd)
				return -1;

			const int ping = Live_Qos_GetPingForDatacenter(qos);

			return ping == 0x7FFFFFFF ? -1 : ping;
		}

		std::string netadr_to_status_key(const netadr_t& adr)
		{
			char buffer[64]{};

			sprintf_s(
				buffer,
				"%u:%u",
				static_cast<unsigned>(adr.inaddr),
				static_cast<unsigned>(adr.port)
			);

			return buffer;
		}

		std::string info_get_value(const char* info, const char* key)
		{
			if (!info || !key || !*key)
				return {};

			const std::string wanted = key;
			const char* p = info;

			while (*p)
			{
				if (*p == '\\')
					++p;

				const char* keyStart = p;

				while (*p && *p != '\\')
					++p;

				if (!*p)
					break;

				const std::string currentKey(keyStart, p - keyStart);

				++p;

				const char* valueStart = p;

				while (*p && *p != '\\')
					++p;

				const std::string currentValue(valueStart, p - valueStart);

				if (currentKey == wanted)
					return currentValue;
			}

			return {};
		}

		std::string parse_status_player_name(const char* line)
		{
			if (!line || !*line)
				return {};

			const char* firstQuote = std::strchr(line, '"');

			if (!firstQuote)
				return {};

			++firstQuote;

			const char* secondQuote = std::strchr(firstQuote, '"');

			if (!secondQuote)
				return firstQuote;

			return std::string(firstQuote, secondQuote - firstQuote);
		}

		bool resolve_session_netadr(const MatchMakingInfo& session, netadr_t& out)
		{
			const auto hostInfo = session.get_host_info();

			dwRegisterSecIDAndKey(&hostInfo.secId, &hostInfo.secKey);

			if (!dwCommonAddrToNetadr(
				&out,
				&hostInfo.serializedAdr.xnaddr,
				&hostInfo.secId))
			{
				return false;
			}

			return true;
		}

		void prepare_connless_read(msg_t& msg)
		{
			if (!msg.data || msg.cursize < 4)
				return;

			const auto* marker = reinterpret_cast<const std::int32_t*>(msg.data);

			if (msg.readcount == 0 && *marker == -1)
			{
				msg.readcount = 4;
			}
		}

		bool make_netadr_from_host_cached(const MatchMakingInfo& session, const HostInfo& hostInfo, netadr_t& out)
		{
			const auto key = static_cast<std::uint64_t>(session.xuid);

			if (!key)
				return false;

			const DWORD now = GetTickCount();

			{
				std::scoped_lock lock(netadr_cache_mutex);

				const auto it = g_netadrCache.find(key);
				if (it != g_netadrCache.end() &&
					it->second.valid &&
					now - it->second.lastUpdateMs < kNetadrCacheMs)
				{
					out = it->second.adr;
					return true;
				}
			}

			netadr_t resolved{};

			if (!dwRegisterSecIDAndKey(&hostInfo.secId, &hostInfo.secKey))
				return false;

			if (!dwCommonAddrToNetadr(&resolved, &hostInfo.serializedAdr.xnaddr, &hostInfo.secId))
				return false;

			if (!resolved.inaddr)
				return false;

			{
				std::scoped_lock lock(netadr_cache_mutex);
				g_netadrCache[key] = cached_netadr_t{ resolved, now, true };
			}

			out = resolved;
			return true;
		}

		bool request_status_for_session(const MatchMakingInfo& session)
		{
			netadr_t netadr{};

			if (!resolve_session_netadr(session, netadr))
			{
				ImGui::InsertNotification({
					ImGuiToastType::Warning,
					3000,
					"getstatus failed: dwCommonAddrToNetadr"
					});

				return false;
			}

			netchan_send_oob(netadr, "getstatus\n");

			return true;
		}

		/*bool get_status_copy(std::uint64_t xuid, server_status_t& out)
		{
			std::scoped_lock lock(status_mutex);

			const auto it = g_statusByXuid.find(xuid);

			if (it == g_statusByXuid.end())
				return false;

			out = it->second;
			return true;
		}*/

		bool parse_status_response(const netadr_t& from, msg_t& msg)
		{
			char buffer[1024]{};

			const char* infoLine = MSG_ReadStringLine(&msg, buffer, sizeof(buffer));

			if (!infoLine || !*infoLine)
				return false;

			const auto adrKey = netadr_to_key(from);

			{
				std::scoped_lock lock(status_mutex);

				g_infoByAdr[adrKey] =
				{
					info_get_value(infoLine, "mapname"),
					info_get_value(infoLine, "g_gametype")
				};
			}

			std::vector<player_info_t> playersInfo;
			playersInfo.reserve(18);

			for (std::size_t i = 0; i < 18; ++i)
			{
				ZeroMemory(buffer, sizeof(buffer));

				const char* line = MSG_ReadStringLine(&msg, buffer, sizeof(buffer));

				if (!line || !*line)
					break;

				const auto name = parse_status_player_name(line);

				if (!name.empty())
				{
					playersInfo.emplace_back(player_info_t{
						static_cast<int>(i),
						name
						});
				}
			}

			{
				std::scoped_lock lock(status_mutex);
				g_playersByAdr[adrKey] = std::move(playersInfo);
			}

			g_viewDirty = true;

			return true;
		}

		std::string server_type_to_string(std::uint32_t serverType)
		{
			switch (serverType)
			{
			case 1000: return "P2P MP";
			case 1001: return "P2P ZM";
			case 1002: return "P2P CP";
			case 1003: return "P2P CPZM";
			case 1004: return "P2P DOA";
			case 2000: return "DEDI MP";
			default:   return "Unknown";
			}
		}

		bool is_zombies_server(std::uint32_t serverType)
		{
			return serverType == 1001 || // P2P ZM
				serverType == 1003 ||   // P2P CPZM
				serverType == 1004;     // P2P DOA
		}

		bool is_campaign_server(std::uint32_t serverType)
		{
			return serverType == 1002 || // P2P CP
				serverType == 1003;     // P2P CPZM
		}

		bool is_multiplayer_server(std::uint32_t serverType)
		{
			return serverType == 1000 || // P2P MP
				serverType == 2000;     // Dedicated MP
		}

		std::string game_type_to_string(const MatchMakingInfo& session)
		{
			const auto serverType = static_cast<std::uint32_t>(session.serverType);
			const auto gameType = static_cast<std::uint32_t>(session.info.gameType);

			if (is_zombies_server(serverType))
			{
				switch (serverType)
				{
				case 1001:
					return "Zombies";
				case 1003:
					return "Campaign/Zombies";
				case 1004:
					return "Dead Ops Arcade";
				default:
					return "Zombies";
				}
			}

			if (is_campaign_server(serverType))
			{
				return "Campaign";
			}

			if (is_multiplayer_server(serverType))
			{
				const char* name = Com_GameInfo_GetGameTypeNameForID(
					static_cast<int>(gameType)
				);

				if (name && *name) {
					const char* translated = UI_SafeTranslateString(Com_GameInfo_GetGameTypeRef(name));
					return translated;
				}

				return "MP GameType " + std::to_string(gameType);
			}

			return "GameType " + std::to_string(gameType);
		}

		bool SessionChanged(const MatchMakingInfo& a, const MatchMakingInfo& b)
		{
			return
				a.info.numPlayers != b.info.numPlayers ||
				a.info.maxPlayers != b.info.maxPlayers ||
				a.info.gameType != b.info.gameType ||
				a.serverType != b.serverType ||
				a.isEmpty != b.isEmpty;
		}

		bool SessionPassesFilters(const MatchMakingInfo& session)
		{
			if (!bEmpty && session.isEmpty == 1)
				return false;

			if (!bFull && session.info.maxPlayers != 0 &&
				session.info.numPlayers == session.info.maxPlayers)
			{
				return false;
			}

			if (session.info.numPlayers > 12)
			{
				return false;
			}

			if (!bLessHalf && session.info.maxPlayers != 0 &&
				session.info.numPlayers < session.info.maxPlayers / 2)
			{
				return false;
			}

			if (!bP2P && session.serverType != 2000)
				return false;

			return true;
		}

		void register_session(const MatchMakingInfo& session)
		{
			std::scoped_lock lock(sessions_mutex);

			const auto entry = std::find_if(
				sessions.begin(),
				sessions.end(),
				[&](const MatchMakingInfo& existing)
				{
					return existing.xuid == session.xuid;
				}
			);

			if (entry == sessions.end())
			{
				sessions.emplace_back(session);
				g_viewDirty = true;
				return;
			}

			if (SessionChanged(*entry, session))
			{
				*entry = session;
				g_viewDirty = true;
			}
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

		const std::vector<session_view_t>& get_session_view_rows()
		{
			const DWORD now = GetTickCount();

			if (!g_viewDirty && now - g_lastViewBuildMs < kViewRebuildIntervalMs)
				return g_viewRows;

			std::vector<MatchMakingInfo> copy;

			{
				std::scoped_lock lock(sessions_mutex);
				copy = sessions;
			}

			std::sort(
				copy.begin(),
				copy.end(),
				[](const MatchMakingInfo& a, const MatchMakingInfo& b)
				{
					if (a.info.numPlayers != b.info.numPlayers)
						return a.info.numPlayers > b.info.numPlayers;

					return a.info.maxPlayers > b.info.maxPlayers;
				}
			);

			g_viewRows.clear();
			g_viewRows.reserve(copy.size());

			for (const auto& session : copy)
			{
				if (!SessionPassesFilters(session))
					continue;

				session_view_t row{};
				row.session = session;
				row.typeLabel = server_type_to_string(session.serverType);
				row.gameTypeLabel = game_type_to_string(session);

				g_viewRows.emplace_back(std::move(row));
			}

			g_viewDirty = false;
			g_lastViewBuildMs = now;

			return g_viewRows;
		}

		void ClearServerList()
		{
			{
				std::scoped_lock lock(sessions_mutex);
				sessions.clear();
			}

			{
				std::scoped_lock lock(status_mutex);
				g_playersByAdr.clear();
				g_infoByAdr.clear();
			}

			{
				std::scoped_lock lock(netadr_cache_mutex);
				g_netadrCache.clear();
			}

			g_viewRows.clear();
			g_viewDirty = true;

		}

		// ---------------------------------------------------------------------
		// Task callbacks
		// ---------------------------------------------------------------------

		const auto lobby_search_success_callback = [](TaskRecord* task)
			{
				if (!task || !task->remoteTask)
					return;

				const auto remote_task = task->remoteTask;

				const size_t resultCount = std::min<std::size_t>(
					remote_task->numResults,
					search_results.size()
				);

				for (size_t i = 0; i < resultCount; ++i)
				{
					const auto& result = search_results[i];

					if (result.info.hostAddrSize != sizeof(XNADDR))
						continue;

					if (!result.xuid)
						continue;

					register_session(result);
				}
			};

		const auto lobby_search_failure_callback = [](TaskRecord*)
			{
				//log::info("servers: session search task failed.");
			};

		const TaskDefinition task_lobby_search
		{
			2,
			"t71",
			0x10,
			lobby_search_success_callback,
			lobby_search_failure_callback
		};

		void fetch_sessions(bool force = false)
		{
			if (force)
			{
				TaskManager2_ClearTasks(&task_lobby_search);
			}
			else if (TaskManager2_TaskGetInProgress(&task_lobby_search))
			{
				return;
			}

			TaskManager2_ClearTasks(&task_lobby_search);

			const auto task = TaskManager2_CreateTask(
				&task_lobby_search,
				CONTROLLER_INDEX_0,
				nullptr,
				10000
			);

			if (!task)
				return;

			const auto matchmaking = TaskManager2_Tasksmth(0);

			if (!matchmaking)
			{
				TaskManager2_RevertTask(task);
				return;
			}

			query_info.queryId = ENUM_SEARCH_SESSIONS;

			bdRemoteTask* remote_task{};

			Server_GetServer(
				matchmaking,
				&remote_task,
				query_info.queryId,
				0,
				search_results.size(),
				&query_info,
				search_results.data()
			);

			if (!remote_task)
			{
				TaskManager2_RevertTask(task);
				return;
			}

			task->remoteTask = remote_task;
			TaskManager2_StartTask(task);
		}
	}

	// -------------------------------------------------------------------------
	// Draw
	// -------------------------------------------------------------------------

	void draw()
	{
		if (ImGui::BeginChild("Servers", ImGui::GetContentRegionAvail())) {

			ImGui::Dummy(ImVec2(0, 5));

			if (ImGui::Checkbox("P2P", &bP2P))
				g_viewDirty = true;

			ImGui::SameLine();
			if (ImGui::Checkbox("Empty", &bEmpty))
				g_viewDirty = true;

			ImGui::SameLine();
			if (ImGui::Checkbox("Full", &bFull))
				g_viewDirty = true;

			ImGui::SameLine();
			if (ImGui::Checkbox("< Half", &bLessHalf))
				g_viewDirty = true;

			if (ImGui::Button("Fetch"))
				fetch_sessions(true);

			ImGui::SameLine();
			if (ImGui::Button("Clear List"))
				ClearServerList();

			if (bAutoFetch)
			{
				const DWORD now = GetTickCount();

				if (now - g_lastFetchMs > kFetchIntervalMs)
				{
					fetch_sessions(false);
					g_lastFetchMs = now;
				}
			}

			const auto& rows = get_session_view_rows();

			ImGui::Dummy(ImVec2(0, 2));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 2));

			ImGui::Text("Total Servers : %zu", rows.size());

			static ImGuiTextFilter filter;
			filter.Draw("##Servers", ImGui::GetContentRegionAvail().x);

			ImGui::Dummy(ImVec2(0, 2));

			if (ImGui::BeginChild("##ServerListChild", ImGui::GetContentRegionAvail(), true))
			{
				const ImVec2 tableSize = ImGui::GetContentRegionAvail();

				if (ImGui::BeginTable(
					"##ServersTable",
					4,
					ImGuiTableFlags_Borders |
					ImGuiTableFlags_RowBg |
					ImGuiTableFlags_Resizable |
					ImGuiTableFlags_ScrollY |
					ImGuiTableFlags_SizingStretchProp,
					tableSize
				))
				{
					ImGui::TableSetupScrollFreeze(0, 1);
					ImGui::TableSetupColumn("Server", ImGuiTableColumnFlags_WidthFixed, 95.0f);
					ImGui::TableSetupColumn("Game Type", ImGuiTableColumnFlags_WidthFixed, 112.0f);
					ImGui::TableSetupColumn("Ping", ImGuiTableColumnFlags_WidthFixed, 45.0f);
					ImGui::TableSetupColumn("Clients", ImGuiTableColumnFlags_WidthFixed, 60.0f);
					ImGui::TableHeadersRow();

					for (size_t i = 0; i < rows.size(); ++i)
					{
						const auto& row = rows[i];
						const auto& session = row.session;

						const auto row_id = std::to_string(i + 1);
						const auto label = row.typeLabel + " Server##server" + row_id;
						const auto popup = "server_popup##" + row_id;

						if (!filter.PassFilter(label.c_str()) &&
							!filter.PassFilter(row.gameTypeLabel.c_str()))
						{
							continue;
						}

						ImGui::PushID(static_cast<int>(i));

						const auto hostInfo = session.get_host_info();

						std::string mapName;
						std::string gameType;
						std::string prettyName;
						std::vector<player_info_t> players;


						ImGui::TableNextRow();

						if (ImGui::BeginPopup(popup.c_str(), ImGuiWindowFlags_NoMove))
						{
							if (ImGui::MenuItem("Join Server"))
							{
								connect_to_session(hostInfo);
							}

							if (ImGui::MenuItem("Copy XUID"))
							{
								ImGui::SetClipboardText(
									std::to_string(static_cast<std::uint64_t>(session.xuid)).c_str()
								);
							}

							ImGui::Separator();

							ImGui::MenuItem(row.typeLabel.c_str(), nullptr, false, false);
							ImGui::MenuItem(row.gameTypeLabel.c_str(), nullptr, false, false);

							ImGui::EndPopup();
						}

						ImGui::TableSetColumnIndex(0);

						if (ImGui::Selectable(
							label.c_str(),
							false,
							ImGuiSelectableFlags_SpanAllColumns
						))
						{
							ImGui::OpenPopup(popup.c_str());
						}

						ImGui::TableSetColumnIndex(1);

						if (!prettyName.empty())
							ImGui::TextUnformatted(prettyName.c_str());
						else
							ImGui::TextUnformatted(row.gameTypeLabel.c_str());

						ImGui::TableSetColumnIndex(2);

						const int ping = get_qos_ping_for_location(session.serverLocation);

						if (ping >= 0)
							ImGui::Text("%d ms", ping);
						else
							ImGui::TextUnformatted("-");

						ImGui::TableSetColumnIndex(3);
						ImGui::Text("%u/%u", session.info.numPlayers, session.info.maxPlayers);

						ImGui::PopID();
					}

					ImGui::EndTable();
				}

				ImGui::EndChild();
			}

			ImGui::EndChild();
		}
	}

	// -------------------------------------------------------------------------
	// Status response hook entry point
	// -------------------------------------------------------------------------

	bool HandleStatusResponse(const netadr_t& from, msg_t& msg)
	{
		char buffer[1024]{};

		msg_t copy = msg;

		const char* firstLine = MSG_ReadStringLine(&copy, buffer, sizeof(buffer));

		if (firstLine && !_stricmp(firstLine, "statusResponse"))
		{
			msg = copy;
		}

		return parse_status_response(from, msg);
	}

	// -------------------------------------------------------------------------
	// Install / Initialize
	// -------------------------------------------------------------------------

	bool Install()
	{
		Server_GetServerInfos(&query_info, 0, true);

		TaskManager2_searchresultsmth(
			&search_results,
			sizeof(MatchMakingInfo),
			search_results.size(),
			OFFSET(0x144CA20),
			OFFSET(0x2D3A5B0)
		);

		return true;
	}
}
