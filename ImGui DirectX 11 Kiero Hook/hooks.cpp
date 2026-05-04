#include "hooks.h"
#include "crc.h"

namespace hooks {

	DWORD_PTR pUserData = ProcessBase + 0x14F344B0;
	DWORD_PTR pNameBuffer = ProcessBase + 0x15E056C8;

	typedef uint64_t AppId_t;
	typedef bool(__fastcall* ReadP2PPacket_t)(uintptr_t thisptr, void* pub_dest, unsigned int cub_dest, unsigned int* cub_msg_size, unsigned __int64* steam_id_remote, int n_channel);
	ReadP2PPacket_t ReadP2PPacket_o;
	typedef bool(__fastcall* Steam_bIsDlcInstalled_t)(__int64 thisPtr, uint64_t appID);
	Steam_bIsDlcInstalled_t Steam_bIsDlcInstalled_o;
	typedef bool(__fastcall* Steam_bIsAppInstalled_t)(__int64 thisPtr, uint64_t appID);
	Steam_bIsAppInstalled_t Steam_bIsAppInstalled_o;

	std::unordered_map<AppId_t, bool> dlcCache;
	std::unordered_map<AppId_t, bool> appCache;
	std::vector<int> config_strings = { 3514, 3627 };

	uintptr_t jpg_buf_o = NULL;
	char* custom_jpg_buf = NULL;
	uint32_t custom_jpg_buf_size = 0;
	uint32_t custom_thumb_buf_size = 0;
	char* custom_thumb_buf = new char[0x88000];

	const std::vector<uintptr_t> isModLoadedAddrs =
	{
		{ 0x20EB3C4 },
		{ 0x20EB3E4 },
		{ 0x20EB324 },
		{ 0x20EB2F4 },
		{ 0x20EAD97 },
		{ 0x13E6A74 },
		{ 0x15E7EDB },
		{ 0x15E87DB },
		{ 0x15EBAE9 },
		{ 0x15F1F29 },
		{ 0x15F1FD9 },
		{ 0x15F20A0 },
		{ 0x15F7F60 },
		{ 0x1A80D1D },
		{ 0x1A9AD29 },
		{ 0x1E9432B },
		{ 0x1E9C8AE },
		{ 0x1EA5669 },
		{ 0x1EBF6CD },
		{ 0x20EAD5D },
		{ 0x1FC6769 },
	};

	uintptr_t GetCallsiteFromReturn(void* returnAddress)
	{
		uint8_t* ret = (uint8_t*)returnAddress;

		if (ret[-5] == 0xE8)
		{
			return (uintptr_t)(ret - 5);
		}

		for (int i = 1; i <= 16; i++)
		{
			if (ret[-i] == 0xE8)
			{
				return (uintptr_t)(ret - i);
			}
		}

		return (uintptr_t)returnAddress;
	}

	bool is_user_in_game()
	{
		return Live_IsUserInGame(0);
	}

	bool local_client_is_in_game()
	{
		return CL_IsLocalClientInGame(0);
	}

	bool is_in_game()
	{
		return Com_IsInGame();
	}

	bool is_match_loaded()
	{
		return (is_user_in_game() && local_client_is_in_game() && is_in_game());
	}

	bool is_user_in_lobby()
	{
		return (!Live_IsUserInGame(0) && Live_IsUserSignedInToDemonware(CONTROLLER_INDEX_0));
	}

	bool AreWeInGameAndHosting() {

		bool rt = false;

		if (is_match_loaded() && Live_AreWeHost()) {
			return true;
		}
		return rt;

	}

	ImVec2 WorldToScreen(ImVec3 vWorldLocation, int type)
	{

		if (type == 1)
		{
			float world[3];
			world[0] = vWorldLocation.x;
			world[1] = vWorldLocation.y;
			world[2] = vWorldLocation.z;
			float out[3];
			spoof_call(spoof_t, WorldPosToScreenPos, 0, world, out);

			return { out[0], out[1] };

		}
	

		return { 0, 0 };
	}

	ImVec3 CG_DObjGetWorldTagPos(__int64 centity_t, __int64 DObj, int tag, float* whatever)
	{
		ImVec3 pos;
		float WORLD[3];
		auto DOBJ = CG_DObjGetWorldTagPosInternal(centity_t, DObj, tag, whatever, WORLD, 0xFFFF);

		pos.x = WORLD[0];
		pos.y = WORLD[1];
		pos.z = WORLD[2];

		return pos;
	}

	void load_custom_jpg()
	{
		if (custom_jpg_buf)
		{
			*(uint64_t*)(*(uint64_t*)(ProcessBase + 0x1976CA80) + 0x5A8498) = NULL;
			custom_jpg_buf = NULL;
			custom_jpg_buf_size = 0;
		}

		std::ifstream ifs;
		ifs.open("screenshot.jpg", std::ifstream::in | std::ifstream::binary);

		if (!ifs.is_open())
		{
			return;
		}

		ifs.seekg(0, std::ios::end);
		size_t fileSize = ifs.tellg();
		ifs.seekg(0, std::ios::beg);

		custom_jpg_buf_size = (uint32_t)fileSize;
		custom_jpg_buf = new char[(uint32_t)fileSize];
		ifs.read(custom_jpg_buf, (uint32_t)fileSize);

		ifs.close();

		// Thumbnail
		std::ifstream ifst;
		if (custom_thumb_buf)
		{
			custom_thumb_buf = NULL;
			custom_thumb_buf_size = 0;
		}

		ifst.open("thumbnail.jpg", std::ifstream::in | std::ifstream::binary);

		if (!ifst.is_open())
		{
			return;
		}

		ifst.seekg(0, std::ios::end);
		size_t thumbfileSize = ifst.tellg();
		ifst.seekg(0, std::ios::beg);

		custom_thumb_buf_size = (uint32_t)thumbfileSize;
		custom_thumb_buf = new char[(uint32_t)thumbfileSize];
		ifst.read(custom_thumb_buf, (uint32_t)thumbfileSize);

		ifst.close();

	}

	namespace functions {

		// Patches

		char hkMods_IsModsLoaded()
		{
			void* ret = _ReturnAddress();
			uintptr_t base = (uintptr_t)GetModuleHandle(NULL);

			uintptr_t callsite = GetCallsiteFromReturn(ret);
			uintptr_t relativeCall = callsite - base;

			if (bModTools)
			{
				for (auto addr : isModLoadedAddrs)
				{
					if (relativeCall == addr)
					{
						return 0;
					}
				}
			}

			return Mods_IsModsLoaded();
		}

		char hkMods_IsModsLoaded_1() {

			if (bModTools) {
				return 0;
			}

			return Mods_IsModsLoaded_1();
		}

		char hkCom_Error(const char* a1, int a2, unsigned int a3, const char* fmt, ...)
		{
			void* ret = _ReturnAddress();
			uintptr_t baseAddr = (uintptr_t)GetModuleHandle(NULL);
			uintptr_t relativeAddr = (uintptr_t)ret - baseAddr;
			va_list args;
			va_start(args, fmt);

			static char buffer[1024]{};

			if (ret == (void*)OFFSET(0x134CAE7)) // Server Disconnected, player has been kicked 
			{

				vsnprintf(buffer, sizeof(buffer), fmt, args);
				va_end(args);

				if (strstr(buffer, "PLATFORM_STEAM_AUTH_DENIED_NO_LICENSE_FOR_CONTENT"))
				{
					snprintf(buffer, sizeof(buffer),
						"^1DLC SPOOF ERROR: ^7%s\n\n^7You were removed from the match for not owning the content\n\n^8To prevent this error, have the ^2HOST ^8use Scropts QOL", "PLATFORM_STEAM_AUTH_DENIED_NO_LICENSE_FOR_CONTENT");
					return Com_Error(a1, a2, a3, "%s", buffer);
				}

				return Com_Error(a1, a2, a3, "%s", buffer);

			}

			if (ret == (void*)OFFSET(0x20F0227))
			{

				const char* mapName = va_arg(args, const char*);
				const char* error = va_arg(args, const char*);

				va_end(args);

				snprintf(buffer, sizeof(buffer),
					"^1DLC SPOOF ERROR: ^7%s\n\n^7Cannot load DLC files, Have you downloaded ^2%s's ^7files and placed them in your Zone Folder?\n\n ^8C:\\Program Files (x86)\\Steam\\steamapps\\common\\Call of Duty Black Ops III\\zone",
					error ? error : "null",				
				mapName ? mapName : "null");

				return Com_Error(a1, a2, a3, "%s", buffer);

			}

			if (ret == (void*)OFFSET(0x1EB0E44))
			{
				const char* error = va_arg(args, const char*);
				int errorCode = va_arg(args, int);
				va_end(args);
				
				snprintf(buffer, sizeof(buffer),
					"^1MULTIPLAYER STAT PROTECTION:^7 [%d]\n\n^7To go backwards in rank, do a ^2Fresh Restart\n\n^8Menu -> Barracks -> Multiplayer -> Prestige Options -> Fresh Restart\n\n^7Stuck on a bugged rank? You can ^2Freeze ^7your stats, set Prestige 1,\ngo to Fresh Restart, ^1Unfreeze^7 stats, then Fresh Restart!", errorCode);				

				return Com_Error(a1, a2, a3, "%s", buffer);

			}

			vsnprintf(buffer, sizeof(buffer), fmt, args);
			utils::write_log("[Com_Error] Relative Return Address: 0x%llX\nError Message: %s", relativeAddr, buffer);
			va_end(args);

			return Com_Error(a1, a2, a3, "%s", buffer);
			
		}
					
		const char* __fastcall hkCL_GetConfigString(std::int32_t configStringIndex)
		{

			constexpr auto mspreload_command = "mspreload";

			if (utils::is_in_number_array(configStringIndex, config_strings))
			{
				if (auto config_string{ CL_GetConfigString(configStringIndex) }; utils::is_equal(config_string, mspreload_command, std::strlen(mspreload_command), false))
				{
					CL_StoreConfigString(configStringIndex, "");
					ImGui::InsertNotification({ ImGuiToastType::Info, 5000, "Loadside Crash attempt caught. \n%s ", config_string });

				}
			}
			return CL_GetConfigString(configStringIndex);
		}

		bool hkCL_ConnectionlessCMD(int clientNum, netadr_t* from, msg_t* msg) {

			auto v7 = *(int**)(Sys_GetTLS() + 24);
			auto v8 = *v7;
			auto message = **(CHAR***)&v7[2 * v8 + 34];			
		
			if (utils::is_in_array(message, legit_packets)) {
				return CL_ConnectionlessCMD(clientNum, from, msg);
			}
			ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "Invalid OOB recieved: %s", message});
			return 1;
		}

		bool hkLobbyMsgRW_PackageElement(LobbyMsg* lobbyMsg, bool addElement) {

			auto should_ignore = lobbyMsg->msg.overflowed;

			if (should_ignore) {
				return 0;
			}

			return LobbyMsgRW_PackageElement(lobbyMsg, addElement);

		}

		float hkflsomeWeirdCharacterIndex(__int64 a1, int a2, int a3) {

			__int64 v3; // r9

			v3 = *(QWORD*)(a1 + 24i64 * a2 + 56);
			if (v3) {
				return flsomeWeirdCharacterIndex(a1, a2, a3);
			}

			return -1.0f;
			
		}

		bool __fastcall ReadP2PPacket(uintptr_t thisptr, void* pub_dest, unsigned int cub_dest, unsigned int* cub_msg_size, unsigned __int64* steam_id_remote, int n_channel) {

			bool result = ReadP2PPacket_o(thisptr, pub_dest, cub_dest, cub_msg_size, steam_id_remote, n_channel);

			if (result) {


				char* data = reinterpret_cast<char*>(pub_dest);

				auto game_im = data[5];

				if (game_im == 'f' || game_im == 'e' || game_im == 'h')
				{

					if (game_im == 'h') {

						if (bFriendsOnly && !LiveFriends_IsFriendByXUID(0, *steam_id_remote)) {
							ImGui::InsertNotification({ ImGuiToastType::Info, 3000, "[Instant Message] Dropping msg recieved from (%llu) with size [%u] (Non-Friend)", game_im, *steam_id_remote, *cub_msg_size });
							return false;
						}
					}


					if (game_im == 'f')
					{
						ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "[Instant Message] popup/crash attempt caught from (%llu) with size [%u]", *steam_id_remote, *cub_msg_size });

						return false;
					}

					if (game_im == 101 || game_im == 109) // cbuf
					{
						ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "[Instant Message] Remote cbuf attempt caught from (%llu) with size [%u]", *steam_id_remote, *cub_msg_size });

						return false;
					}

					


				}
			}

			return result;
		}

		bool HandleMessage(const unsigned __int64 sender_id, const char* sender_name, const char* message, const unsigned int message_size) {

			msg_t msg{};
			MSG_BeginReading(&msg);
			MSG_InitReadOnly(&msg, message, message_size);

			auto type{ 0ui8 };

			if (MSG_ReadByte(&msg) == '2')
			{
				type = MSG_ReadByte(&msg);
			}

			LobbyMsg lobby_msg{};
			LobbyMsgRW_PrepReadMsg(&lobby_msg, &msg);

			if (type == 'f')
			{
				auto output = message_size == 102 ? "Crash" : "Popup";

				ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "[Instant Message] %s attempt from %s (%llu)", output, sender_name, sender_id });
				
			}

			if (type == 'e')
			{
				ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "[Instant Message] Remote cbuf attempt from %s (%llu)", sender_name, sender_id });
				
			}
			
			return false;
		}

		bool __fastcall Global_InstantMessage(uintptr_t thisptr, unsigned __int64 sender_id, const char* sender_name, uintptr_t msg, unsigned int size) {

			const auto message{ reinterpret_cast<const char*>(msg) };
			const auto message_size{ size };

			return HandleMessage(sender_id, sender_name, message, size);
		}

		void handleInstantMsg(const std::uint64_t sender_id, const char* message, const std::uint32_t message_size)
		{

			msg_t msg{};
			MSG_InitReadOnly(&msg, message, message_size);
			MSG_BeginReading(&msg);

			auto type = 0ui8;


			if (MSG_ReadByte(&msg) == '1')
			{
				type = MSG_ReadByte(&msg);
			}

			//ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "[Instant Message] '%s' ('%c') received from (%llu) with a size of %u", message, type, sender_id, message_size });

			if (type == 'h')
			{
				if (const auto size{ msg.cursize - msg.readcount }; size < 2048u)
				{
					char data[2048] = { 0 };
					MSG_ReadData(&msg, data, size);

					if (!msg.overflowed)
					{
						LobbyMsg lobby_msg{};
						if (!LobbyMsgRW_PrepReadData(&lobby_msg, data, size))
							return;

						if (lobby_msg.msgType == MESSAGE_TYPE_INFO_RESPONSE)
						{
							Msg_InfoResponse response{};

							if (!MSG_InfoResponse(&response, &lobby_msg))
								return;

						};

						if (lobby_msg.msgType == MESSAGE_TYPE_INFO_REQUEST)
						{
							

							if (sender_id != LiveUser_GetXUID(0))
							{

								if (bFriendsOnly)
								{
									if (!LiveFriends_IsFriendByXUID(0, sender_id))
									{
										return;
									}
								}
							}
						}

						LobbyMsg_HandleIM(0, sender_id, data, size);
					}
				}
			}
		}

		__int64 hkdwInstantDispatchMessage(__int64 sender_id, unsigned int controllerIndex, __int64 msg, unsigned int messageSize)
		{
			const auto message{ reinterpret_cast<const char*>(msg) };
			const auto message_size{ messageSize };

			handleInstantMsg(sender_id, message, message_size);

			return 0;
		}

		void __fastcall live_presence_pack_h(presence_data_s* presence, void* buffer, size_t buffer_size) {

			presence->platform.primary_presence = presence_primary_online;

			presence->title.activity = presence_activity_mp_watching_film;
			presence->title.ctx = presence_ctx_mp_public_match;
			presence->title.joinable = lobby_joinable_no_you_need_dlc;
			presence->title.difficulty = 2;
			presence->title.map_id = 0;
			presence->title.party.total_count = 1;
			presence->title.party.max = 18;
			presence->title.party.available_count = 17;

			const char* gamertags[18]{ // Can't exceed 16 chars 
			"S",
			"c",
			"r",
			"o",
			"p",
			"t",
			"s",
			"^7",
			"Q",
			"O",
			"L",
			"^7",
			"^7",
			"^7",
			"^7",
			"^7",
			"^7",
			"^BXENONBUTTONA^"
			};

			for (auto i{ 0 }; i < 18; i++)
			{
				strncpy_s(presence->title.party.members[i].gamertag, gamertags[i], sizeof(gamertags[i]));
			}

			return live_presence_pack(presence, buffer, buffer_size);
		}

		bool hkUI_IsRenderingImmediately() {

			return true;

		}

		void hkR_ConvertColorToBytes(ImVec4* color, byte* bytes) {

			auto desiredColor = bUIRgb ? (ImVec4)mainRgb() : (ImVec4)uiColor;

			desiredColor.w = color->w;

			void* returnAddress = _ReturnAddress();

			//return R_ConvertColorToBytes(&desiredColor, bytes);

			if (bColoredUI && returnAddress != nullptr) {

				if (returnAddress == (void*)(ProcessBase + 0x1F28963) || returnAddress == (void*)(ProcessBase + 0x1CCDD8C) || returnAddress == (void*)(ProcessBase + 0x279C317)
					|| returnAddress == (void*)(ProcessBase + 0x1CCCED7) || returnAddress == LPVOID(ProcessBase + 0x134DA0F) ) {

					float brightness = 0.299f * color->x + 0.587f * color->y + 0.114f * color->z;

					if (brightness > 0.7f) {
						return R_ConvertColorToBytes(&desiredColor, bytes);
					}

					auto darkDesired = ImVec4(
						desiredColor.x * 0.1f,
						desiredColor.y * 0.1f,
						desiredColor.z * 0.1f,
						desiredColor.w
					);

					return R_ConvertColorToBytes(&darkDesired, bytes);
				}

			}

			return R_ConvertColorToBytes(color, bytes);

		}

		bool hkSteam_bIsDlcInstalled(__int64 vTableThisPtr, uint64_t appID) {

			if (bUnlockFullGame) return true;

			auto it = dlcCache.find(appID);
			if (it != dlcCache.end()) {
				return it->second;
			}

			bool result = Steam_bIsDlcInstalled_o(vTableThisPtr, appID);

			dlcCache[appID] = result;

			return result;


		}

		bool hkSteam_bIsAppInstalled(__int64 vTableThisPtr, uint64_t appID) {

			if (bUnlockFullGame) return true;

			auto it = appCache.find(appID);
			if (it != appCache.end()) {
				return it->second;
			}

			bool result = Steam_bIsAppInstalled_o(vTableThisPtr, appID);

			appCache[appID] = result;

			return result;
		}

		char hkUserHasLicenseForApp(__int64 mapInfo, __int64* userObj) {

			*((BYTE*)userObj + 13) = 1;
			*((BYTE*)userObj + 12) |= 0;
			*((DWORD*)userObj + 4) |= 8u;

			return 1;
		}

		__int64 hkLiveInventory_GetItemQuantity(ControllerIndex_t controllerIndex, int itemId) {

			if (itemId == 99003 && bSpoofSlots) {
				return 1;
			}

			if (bSpoofBlackMarket) return iBlackmarketAmt;

			return LiveInventory_GetItemQuantity(controllerIndex, itemId);

		}

		bool hkLiveInventory_AreExtraSlotsPurchased(ControllerIndex_t controllerIndex) {

			if (bSpoofSlots) {
				return true;
			}
			return LiveInventory_AreExtraSlotsPurchased(controllerIndex);
		}

		bool hkLiveInventory_IsValid(ControllerIndex_t controllerIndex) {



			return true;
		}

		bool hkLiveEntitlements_IsEntitlementActiveForController(ControllerIndex_t controllerIndex, int incentiveId) {

			if (bSpoofPurchases) return true;

			return LiveEntitlements_IsEntitlementActiveForController(controllerIndex, incentiveId);

		}

		bool hkLive_UserGetName(ControllerIndex_t controllerIndex, char* buf, int bufSize) {

			__int64 v3; // rbx
			int* v6; // rax
			int v9; // [rsp+40h] [rbp+8h] BYREF

			v3 = bufSize;
			v9 = bufSize;
			Memset(buf, 0i64, bufSize);
			v6 = &v9;

			Live_Base_UserGetName((UINT8*)buf, *v6, 1);

			return 1;
		}

		__int64 hkG_Damage(__int64 targ, __int64 inflictor, __int64 attacker, __int64 a4, __int64 a5, int damage, int a7, int a8, __int64 a9, int a10, __int64 a11, int a12, int a13, int a14, __int16 a15, int a16, __int64 a17) {

			auto g_entity = *(__int64*)(ProcessBase + 0x1768E578);
			auto g_entityP2 = (__int64)(g_entity + 0x4F8);
			auto g_entityP3 = (__int64)(g_entity + 0x9F0);
			auto g_entityP4 = (__int64)(g_entity + 0xEE8);
			auto newDamage = (int)(damage * iDamageMultiplier);
			auto isPlayer = targ == g_entity || targ == g_entityP2 || targ == g_entityP3 || targ == g_entityP4;

			if (LobbyTypes_GetMainMode() == LOBBY_MAINMODE_ZM && inflictor != NULL && attacker != NULL) {

				if (isPlayer) {

					if (bThorns) {
						return spoof_call(spoof_t, G_Damage, attacker, targ, targ, a4, a5, newDamage, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);
					}
				}

				if (bNukes && !isPlayer) {
					for (int i = 4; i < 2047; ++i) {
						__int64 target = static_cast<__int64>(g_entity) + (0x4F8 * i);
						spoof_call(spoof_t, G_Damage, target, inflictor, attacker, a4, a5, (int)9999999, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);
					}
				}

				if (bDamageMultiplier && !isPlayer) {
					return spoof_call(spoof_t, G_Damage, targ, inflictor, attacker, a4, a5, newDamage, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);
				}
							
			}

			if (bGodMode && isPlayer) {
				return 0;
			}

			return spoof_call(spoof_t, G_Damage, targ, inflictor, attacker, a4, a5, damage, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);

		}

		void hkI_Strcpy(BYTE* a1, __int64 a2, const char* a3) {

			if ((DWORD_PTR)_ReturnAddress() == (ProcessBase + 0x264A547)) {

				if (!custom_title_buf.empty()) {

					static std::string titleBuf = utils::decodeEncodedChars(custom_title_buf);

					return I_Strcpy(a1, a2, titleBuf.c_str());
				}
			}

			if ((DWORD_PTR)_ReturnAddress() == (ProcessBase + 0x264A580)) {

				if (!custom_desc_buf.empty()) {

					static std::string descBuf = utils::decodeEncodedChars(custom_desc_buf);

					return I_Strcpy(a1, a2, descBuf.c_str());
				}
			}

			return I_Strcpy(a1, a2, a3);

		}

		bool hkDemo_SetMetaData(unsigned int controllerIndex, __int64 metaData, unsigned int outBufferSize, __int64 metaDataSize, int info, char duration) {


			if (!custom_title_buf.empty()) {

				ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Title..." });

				static std::string titleBuf = utils::decodeEncodedChars(custom_title_buf);

				strncpy((char*)metaDataSize + 0x242, titleBuf.c_str(), 32);
				*(bool*)(metaDataSize + 0x286) = true;
				strncpy((char*)((ProcessBase + 0x1976CA80) + 0x6706EA), titleBuf.c_str(), 32);

			}

			if (!custom_desc_buf.empty()) {

				ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Desc..." });

				static std::string descBuf = utils::decodeEncodedChars(custom_desc_buf);

				strncpy((char*)metaDataSize + 0x287, descBuf.c_str(), 32);
				*(bool*)(metaDataSize + 0x307) = true;
				strncpy((char*)((ProcessBase + 0x1976CA80) + 0x67072F), descBuf.c_str(), 32);

			}

			return Demo_SetMetaData(controllerIndex, metaData, outBufferSize, metaDataSize, info, duration);
		}

		bool hkFileshare_CreateMetaData(__int64 a1, FileshareMetaInfo* metaInfo, int* a3, int a4) {

			if ((DWORD_PTR)_ReturnAddress() == (ProcessBase + 0x2591691)) {

				//DumpMemoryToFile((uintptr_t)metaInfo, 256, "metaData.bin");

				if (!custom_title_buf.empty()) { // 32 chars max

					ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Title..." });

					static std::string titleBuf = utils::decodeEncodedChars(custom_title_buf);

					//metaInfo.name = titleBuf.c_str();

					strcpy_s(metaInfo->name, 64, titleBuf.c_str());
					*(unsigned __int8*)(metaInfo + 0xB9) = 1;
					//metaInfo->isModifiedName = 1;

				}

				if (!custom_desc_buf.empty()) { // 32 chars max

					ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Desc..." });

					static std::string descBuf = utils::decodeEncodedChars(custom_desc_buf);

					strcpy_s(metaInfo->description, 64, descBuf.c_str());
					*(unsigned __int8*)(metaInfo + 0xFA) = 1;

				}
			}

			/*
			* qword_197EBA00 + 0x6704A8,
			* qword_197EBA00 + 0x687640,
			* 0x168EBF9C
			* 0x168EBFAD
			*/

			return Fileshare_CreateMetaData(a1, metaInfo, a3, a4);
			/* 
			* PS4 PDB:
			I_strcpy(metaInfo.name, 64uLL, (const char *)(metaDataSize + 0x24A));
			metaInfo.isModifiedName = *(_BYTE *)(metaDataSize + 0x28E);
			I_strcpy(metaInfo.description, 64uLL, (const char *)(metaDataSize + 0x28F));
			metaInfo.isModifiedDescription = *(_BYTE *)(metaDataSize + 0x30F);
			metaInfo.createTime = *(_DWORD *)(metaDataSize + 0x3A0);
			metaInfo.originID = *(_QWORD *)(metaDataSize + 0x310);
			*
			* PC DUMP:
			I_strcpy(v16, 64i64, metaDataSize + 0x242); name
			v16[64] = *(_BYTE *)(metaDataSize + 0x286); isModifiedName
			I_strcpy(v17, 64i64, metaDataSize + 0x287); desc
			v17[64] = *(_BYTE *)(metaDataSize + 0x307); isModifiedDesc
			v13[0] = *(_DWORD *)(metaDataSize + 0x398); createTime
			v12 = *(_QWORD *)(metaDataSize + 0x308);	originID
			*/

		}

		void hkDemo_SaveScreenshotToContentServer(int a1, int a2) {

			load_custom_jpg();

			if (custom_jpg_buf)
			{
				if (!jpg_buf_o) {
					jpg_buf_o = *(uint64_t*)(*(uint64_t*)(ProcessBase + 0x1976CA80) + 0x5A8490);
				}
				ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Screenshot..." });
				*(uint64_t*)(*(uint64_t*)(ProcessBase + 0x1976CA80) + 0x5A8490) = (uint64_t)custom_jpg_buf;
				*(uint32_t*)(*(uint64_t*)(ProcessBase + 0x1976CA80) + 0x5A8498) = custom_jpg_buf_size;
			}

			if (custom_thumb_buf) {
				if (custom_thumb_buf_size <= 0x88000) {
					ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Thumbnail..." });
					//DWORD oldProtect;

					uintptr_t imageBufferAddress = (*(uint64_t*)(ProcessBase + 0x1976CA80)) + 0x5A84A0;
					char* targetBuffer = reinterpret_cast<char*>(imageBufferAddress);
					memcpy((LPVOID)targetBuffer, custom_thumb_buf, custom_thumb_buf_size);
					*(uint32_t*)(*(uint64_t*)(ProcessBase + 0x1976CA80) + 0x5A849C) = custom_thumb_buf_size;

				}
			}

			Demo_SaveScreenshotToContentServer(a1, a2);
			
		}

		__int64 hkFileshare_GetSummaryFileAuthorXUID(ControllerIndex_t controllerIndex, uint64_t fileID) {

			return LiveUser_GetXUID((int)controllerIndex);
		}

		bool hkFileshare_CanDownloadFile(unsigned int a1, __int64 ownerXuid, int a3, __int64 fileId, bool isCommunityFile) {

			if (bFileshareDownloading) return true;

			return Fileshare_CanDownloadFile(a1, ownerXuid, a3, fileId, isCommunityFile);
		}

		__int64 hkLiveSteam_FilterPersonaName(char* buffer, int size, char asciionly)
		{
			//ImGui::InsertNotification({ ImGuiToastType::Info, 4000, "hLiveSteam_FilterPersonaName called" });
			Unhook(oLiveSteam_FilterPersonaName, hkLiveSteam_FilterPersonaName);
			return 0;
		}

		char* hkGetPersonaName(DWORD_PTR _this)
		{

			//ImGui::InsertNotification({ ImGuiToastType::Info, 4000, "hGetPersonaName called" });

			if (spoofName[0] != 0)
			{
				if ((DWORD_PTR)_ReturnAddress() == pGetPersonaNameReturn)
				{
					Hook(oLiveSteam_FilterPersonaName, hkLiveSteam_FilterPersonaName);
				}

				return spoofName;
			}

			return oGetPersonaName(_this);
		}

		__int64 hkLivePresence_Serialize(__int64 a1, __int64 a2)
		{
			if (!a1 || !a2)
				return 0;

			int* packedPtr = *reinterpret_cast<int**>(a1 + 16);

			if (!packedPtr)
				return 0;

			int packed = *packedPtr;
			int count = (packed >> 2) & 0x1F;

			constexpr int maxPlayers = 18;

			if (count > maxPlayers)
			{
				int sanitized = packed;

				sanitized &= ~(0x1F << 2);
				sanitized |= (maxPlayers & 0x1F) << 2;

				*packedPtr = sanitized;

				/*ImGui::InsertNotification({
					ImGuiToastType::Warning,
					3000,
					"Crash Attempt: Presence player count clamped: %d -> %d",
					count,
					maxPlayers
					});*/
			}

			return LivePresence_Serialize(a1, a2);
		}

		void hkUI_Interface_DrawText(unsigned int localClientNum, __int64* luiElement, float xPos, float yPos, unsigned int R, unsigned int G, unsigned int B, unsigned int A, char flags, char* text, __int64 font, float fontHeight, float wrapWidth, float alignment, char luaVM, QWORD* element) {
			
			std::string curText(text ? text : "");
			if (text != NULL) {

				if (auto pos{ utils::findInvalidMaterials(text) }; pos != std::string::npos)
				{
					curText.replace(pos, curText.size(), "^1<InvalidMaterial>");
				}

				if (auto pos{ utils::findInvalidModels(text) }; pos != std::string::npos)
				{
					curText.replace(pos, curText.size(), "^1<InvalidModel>");
				}
			}

			return UI_Interface_DrawText(localClientNum, luiElement, xPos, yPos, R, G, B, A, flags, (char*)curText.c_str(), font, fontHeight, wrapWidth, alignment, luaVM, element);
		}

		void hkDraw2D(int a1) {

			for (auto& tracer : tracers)
			{
				if (tracer.opacity > 0)
				{
					tracer.start_screen = WorldToScreen(tracer.start, 1);
					tracer.end_screen = WorldToScreen(tracer.end, 1);
					tracer.radius = 6;
				}
			}
			spoof_call(spoof_t, CG_Draw2D, a1);
		}

		__int64 hkCG_BulletHitEvent_Internal(int localClientNum, int sourceEntityNum, int targetEntityNum, __int64 weapon, ImVec3 startPos, ImVec3 position, ImVec3 normal, ImVec3 seeThruDecalNormal, int surfType, int* _event, __int64 eventParam, __int16 a12, unsigned __int16 a13, int a14) {
			
			if (!bAllTracers) {
				if (targetEntityNum > 18 || sourceEntityNum != *(DWORD*)cgArray) return spoof_call(spoof_t, CG_BulletHitEvent_Internal, localClientNum, sourceEntityNum, targetEntityNum, weapon, startPos, position, normal, seeThruDecalNormal, surfType, _event, eventParam, a12, a13, a14);
			}

			bullet_tracer tracer{};
			uintptr_t centity_t = cg_EntitiesArray + (0x900 * sourceEntityNum);
			uintptr_t DObj = Com_GetClientDObj(*(DWORD*)(centity_t + 0x3F8), 0);
			if (!centity_t || !DObj || sourceEntityNum != *(DWORD*)cgArray) return spoof_call(spoof_t, CG_BulletHitEvent_Internal, localClientNum, sourceEntityNum, targetEntityNum, weapon, startPos, position, normal, seeThruDecalNormal, surfType, _event, eventParam, a12, a13, a14);

			static float whatever1[255];

			tracer.start = CG_DObjGetWorldTagPos(centity_t, DObj, GScr_AllocString("tag_flash"), whatever1);
			tracer.end = position;
			tracer.opacity = 255.0f;
			tracer.time = std::chrono::high_resolution_clock::now();

			if (bTracers)
				tracers.push_back(tracer);

			return spoof_call(spoof_t, CG_BulletHitEvent_Internal, localClientNum, sourceEntityNum, targetEntityNum, weapon, startPos, position, normal, seeThruDecalNormal, surfType, _event, eventParam, a12, a13, a14);

		}

	}

	__int64 GetCGArray(int a1, int a2) {
		return spoof_call(spoof_t, lergstuff, a1, a2);
	}

	void initPointers() {

		cgArray = reinterpret_cast<std::uintptr_t>((__int64*)(GetCGArray(0, 1793) - 3294544));

		if (cgArray > 0) {
			cg_EntitiesArray = cgArray + 0x8AAAE0;
			playerState = cgArray + 0x11A8B0;
			cgsArray = cgArray + 0x684E40;
			clientactive_t = (__int64)(cgArray + 0x15CE0B0);
			clientinfo = (__int64)(cgArray + 0x2E7A40);
			bPointersInitialized = true;
		}
	}

	void initPointerSwaps() {
		DWORD oldprotect;

		// Save the original functions
		VirtualProtect((LPVOID)(*(__int64*)iSteamApps + 48i64), 16, PAGE_EXECUTE_READWRITE, &oldprotect);
		
		Steam_bIsDlcInstalled_o = (decltype(&functions::hkSteam_bIsDlcInstalled))(*(__int64*)(*(__int64*)iSteamApps + 48i64));
		Steam_bIsAppInstalled_o = (decltype(&functions::hkSteam_bIsAppInstalled))(*(__int64*)(*(__int64*)iSteamApps + 56i64));

		// Replace the vtable pointers
		*(__int64*)(*(__int64*)iSteamApps + 48i64) = (__int64)functions::hkSteam_bIsDlcInstalled;
		*(__int64*)(*(__int64*)iSteamApps + 56i64) = (__int64)functions::hkSteam_bIsAppInstalled;
		
		VirtualProtect((LPVOID)(*(__int64*)iSteamApps + 48i64), 16, oldprotect, &oldprotect);


		// Steam Peer to Peer
		__int64 ptr = *(__int64*)(ProcessBase + 0x10B3DC50);
		__int64 vtable = *(__int64*)(ptr);

		ReadP2PPacket_o = (decltype(&functions::ReadP2PPacket))((*(__int64*)(vtable + 0x10)));
		DWORD old_protect;
		VirtualProtect((LPVOID)(vtable + 0x10), 8, PAGE_EXECUTE_READWRITE, &old_protect);
		*(__int64*)(vtable + 0x10) = (__int64)functions::ReadP2PPacket;
		VirtualProtect((LPVOID)(vtable + 0x10), 8, old_protect, &old_protect);

		oGetPersonaName = (tGetPersonaName)HookVMT(ISteamFriends, (DWORD_PTR)&functions::hkGetPersonaName, 0);

	}

	void initialize_dispatch()
	{

		auto ptr = *(uintptr_t*)(ProcessBase + 0x9B35878);
		auto vmt = **(uintptr_t***)(ptr + 1384);

		auto vtable_buf = new uintptr_t[50];
		for (auto count = 0; count < 50; ++count) {
			vtable_buf[count] = vmt[count];
		}

		**(uintptr_t**)(ptr + 1384) = (uintptr_t)vtable_buf;
		*(uintptr_t*)(**(uintptr_t**)(ptr + 1384) + 192) = (uintptr_t)functions::Global_InstantMessage;

	}

	inline void PA(uint8_t* address, const uint8_t* patch, size_t patchSize)
	{
		DWORD oldProtect;

		VirtualProtect(address, patchSize, PAGE_EXECUTE_READWRITE, &oldProtect);

		for (size_t i = 0; i < patchSize; i++)
			address[i] = patch[i];

		FlushInstructionCache(GetCurrentProcess(), address, patchSize);

		DWORD dummy;
		VirtualProtect(address, patchSize, oldProtect, &dummy);
	}

	template <size_t N>
	struct OP {
		std::array<uint8_t, N> d;
		static constexpr uint8_t k = 0x69;

		constexpr OP(const uint8_t(&in)[N]) : d{} {
			for (size_t i = 0; i < N; ++i) {
				d[i] = in[i] ^ k;
			}
		}
		void A(uint8_t* a, void (*p)(uint8_t*, const uint8_t*, size_t)) const {
			uint8_t dc[N];
			for (size_t i = 0; i < N; ++i) {
				dc[i] = d[i] ^ k;
			}
			p(a, dc, N);

			memset(dc, 0, N);
		}
	};

	static constexpr OP pc1({ 0x48, 0x31, 0xC9, 0x90, 0x90, 0x90 });
	static constexpr OP pc2({ 0x48, 0x31, 0xC9, 0x90, 0x90, 0x90, 0x90, 0x90 });
	static constexpr OP pc3({ 0x48, 0x31, 0xC0, 0x48, 0x31, 0xD2 });


	void PatchPrecomputed()
	{

		for (auto rva : crc1)
			pc1.A((uint8_t*)(ProcessBase + rva), PA);

		for (auto rva : crc2)
			pc2.A((uint8_t*)(ProcessBase + rva), PA);

		for (auto rva : crc3)
			pc3.A((uint8_t*)(ProcessBase + rva), PA);
	}


	void applyPatches() {

		MH_CreateHook((LPVOID)(ProcessBase + 0x20C8F60), functions::hkMods_IsModsLoaded, (LPVOID*)&Mods_IsModsLoaded);
		MH_CreateHook((LPVOID)(ProcessBase + 0x20C9AE0), functions::hkMods_IsModsLoaded_1, (LPVOID*)&Mods_IsModsLoaded_1);
		MH_CreateHook((LPVOID)(ProcessBase + 0x22C9650), functions::hkflsomeWeirdCharacterIndex, (LPVOID*)&flsomeWeirdCharacterIndex);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1EAAD60), functions::hkUserHasLicenseForApp, (LPVOID*)&UserHasLicenseForApp);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1321130), functions::hkCL_GetConfigString, (LPVOID*)&CL_GetConfigString);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1980980), functions::hkG_Damage, (LPVOID*)&G_Damage);
		MH_CreateHook((LPVOID)(ProcessBase + 0x60F920), functions::hkDraw2D, (LPVOID*)&CG_Draw2D);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1189E10), functions::hkCG_BulletHitEvent_Internal, (LPVOID*)&CG_BulletHitEvent_Internal); 
		MH_CreateHook((LPVOID)(ProcessBase + 0x1D10840), functions::hkR_ConvertColorToBytes, (LPVOID*)&R_ConvertColorToBytes);
		MH_CreateHook((LPVOID)(ProcessBase + 0x268CC60), functions::hkUI_IsRenderingImmediately, (LPVOID*)&UI_IsRenderingImmediately);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1DFCC60), functions::hkLiveInventory_GetItemQuantity, (LPVOID*)&LiveInventory_GetItemQuantity);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1E06110), functions::hkLiveEntitlements_IsEntitlementActiveForController, (LPVOID*)&LiveEntitlements_IsEntitlementActiveForController);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1DFC580), functions::hkLiveInventory_AreExtraSlotsPurchased, (LPVOID*)&LiveInventory_AreExtraSlotsPurchased);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1EBB200), functions::hkLive_UserGetName, (LPVOID*)&Live_UserGetName);
		MH_CreateHook((LPVOID)(ProcessBase + 0x2591590), functions::hkDemo_SetMetaData, (LPVOID*)&Demo_SetMetaData);
		MH_CreateHook((LPVOID)(ProcessBase + 0x2591050), functions::hkDemo_SaveScreenshotToContentServer, (LPVOID*)&Demo_SaveScreenshotToContentServer);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1DE9E10), functions::hkFileshare_GetSummaryFileAuthorXUID, (LPVOID*)&Fileshare_GetSummaryFileAuthorXUID);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1E7B060), functions::hkFileshare_CanDownloadFile, (LPVOID*)&Fileshare_CanDownloadFile);
		MH_CreateHook((LPVOID)(ProcessBase + 0x134CD70), functions::hkCL_ConnectionlessCMD, (LPVOID*)&CL_ConnectionlessCMD);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1F28860), functions::hkUI_Interface_DrawText, (LPVOID*)&UI_Interface_DrawText);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1E85450), functions::hkLivePresence_Serialize, (LPVOID*)&LivePresence_Serialize);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1EEA320), functions::hkLobbyMsgRW_PackageElement, (LPVOID*)&LobbyMsgRW_PackageElement);
		MH_CreateHook((LPVOID)(ProcessBase + 0x143A620), functions::hkdwInstantDispatchMessage, (LPVOID*)&dwInstantDispatchMessage);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1DFDFE0), functions::hkLiveInventory_IsValid, (LPVOID*)&LiveInventory_IsValid);
		MH_CreateHook((LPVOID)(ProcessBase + 0x20EC0B0), functions::hkCom_Error, (LPVOID*)&Com_Error);

		MH_EnableHook(MH_ALL_HOOKS);

	}



	void restorePatches() {

		MH_RemoveHook((LPVOID)(ProcessBase + 0x20C8F60));		//Mods_IsModsLoaded
		MH_RemoveHook((LPVOID)(ProcessBase + 0x20C9AE0));		//Mods_IsModsLoaded_1
		MH_RemoveHook((LPVOID)(ProcessBase + 0x22C9650));		//flsomeWeirdCharacterIndex
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1EAAD60));		//hkUserHasLicenseForApp
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1321130));		//hkCL_GetConfigString
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1980980));		//G_Damage
		MH_RemoveHook((LPVOID)(ProcessBase + 0x60F920));		//CG_Draw2D
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1189E10));		//CG_BulletHitEvent_Internal
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1D10840));		//hkR_ConvertColorToBytes
		MH_RemoveHook((LPVOID)(ProcessBase + 0x268CC60));		//hkUI_IsRenderingImmediately
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1DFCC60));		//hkLiveInventory_GetItemQuantity
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1E06110));		//hkLiveEntitlements_IsEntitlementActiveForController
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1DFC580));		//hkLiveInventory_AreExtraSlotsPurchased
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1EBB200));		//hkLive_UserGetName
		MH_RemoveHook((LPVOID)(ProcessBase + 0x2591590));		//Demo_SetMetaData	
		MH_RemoveHook((LPVOID)(ProcessBase + 0x2591050));		//Demo_SaveScreenshotToContentServer
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1DE9E10));		//hkFileshare_GetSummaryFileAuthorXUID
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1E7B060));		//hkFileshare_CanDownloadFile
		MH_RemoveHook((LPVOID)(ProcessBase + 0x134CD70));		//hkCL_ConnectionlessCMD
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1F28860));		//UI_Interface_DrawText
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1E85450));		//LivePresence_Serialize
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1EEA320));		//hkLobbyMsgRW_PackageElement
		MH_RemoveHook((LPVOID)(ProcessBase + 0x143A620));		//dwInstantDispatchMessage
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1DFDFE0));		//LiveInventory_IsValid
		MH_RemoveHook((LPVOID)(ProcessBase + 0x20EC0B0));		//Com_Error	
		MH_DisableHook(MH_ALL_HOOKS);

	}


	void onFrame() {

		static bool inGameBoolean = false; // Tracks whether hooks are currently enabled
		static bool dispatch_enabled = false;
		static bool crc_enabled = false;

		if (!crc_enabled && *(__int64*)(ProcessBase + 0x1686E948)) {
			PatchPrecomputed();
			crc_enabled = true;
			ImGui::InsertNotification({ ImGuiToastType::Success, 3000, "CRC Disabled" });
		}

		if (is_match_loaded()) {
			if (!inGameBoolean) {
				// Enable hooks
				//tracers.clear();
				inGameBoolean = true;
			}
		}
		else {
			if (inGameBoolean) {
				// Disable hooks
				tracers.clear();
				inGameBoolean = false;
			}
		}

		if (!bPointersInitialized) {
			initPointers();
		}

		if (!Live_IsUserInGame(0) && Live_IsUserSignedInToDemonware(CONTROLLER_INDEX_0)) {


			Dvar_SetFromString("groupCountsVisible", "1", true);


			Dvar_SetFromString("groupZeroCountsVisible", "1", true);


			Dvar_SetFromString("groupDownloadInterval", "1", true);


			Dvar_SetFromString("groupUploadInterval", "1", true);


		}


		if (!dispatch_enabled && Live_IsUserSignedInToDemonware(CONTROLLER_INDEX_0))
		{
			initialize_dispatch();
			
			applyPatches();
			dispatch_enabled = true;
			ImGui::InsertNotification({ ImGuiToastType::Success, 3000, "Patches Enabled" });
		}
	}

	void catch_exception(LPEXCEPTION_RECORD ExceptionRecord, LPCONTEXT ContextRecord)
	{
		MEMORY_BASIC_INFORMATION mbi{};
		char modulename[MAX_PATH]{};
		char file_name[MAX_PATH]{};
		bool module_is_filled = false;

		// Query memory information for the exception address
		if (VirtualQuery(ExceptionRecord->ExceptionAddress, &mbi, sizeof(mbi)) == 0) {
			utils::write_log("VirtualQuery failed for address: 0x%llX", reinterpret_cast<std::uintptr_t>(ExceptionRecord->ExceptionAddress));
		} else {
			// Try to get the module base and name
			if (RtlPcToFileHeader(ExceptionRecord->ExceptionAddress, &mbi.AllocationBase)) {
				if (GetModuleFileNameA(reinterpret_cast<HMODULE>(mbi.AllocationBase), modulename, sizeof(modulename))) {
					if (const char* last_slash = strrchr(modulename, '\\')) {
						strncpy_s(file_name, last_slash + 1, _TRUNCATE);
					} else {
						strncpy_s(file_name, modulename, _TRUNCATE);
					}
					module_is_filled = true;
				}
			}
		}

		// Calculate offsets
		auto exception_addr = reinterpret_cast<std::uintptr_t>(ExceptionRecord->ExceptionAddress);
		auto process_offset = exception_addr - ProcessBase;
		auto module_offset = mbi.AllocationBase ? (exception_addr - reinterpret_cast<std::uintptr_t>(mbi.AllocationBase)) : 0;

		utils::begin_exception();
		utils::write_log("Image base: 0x%llX", ProcessBase);
		utils::write_log("Cheat base: 0x%llX", dllbase);

		utils::write_log("Exception at: [0x%llX]", ContextRecord->Rip);
		utils::write_log("Exception address: 0x%llX (process offset: 0x%llX, module offset: 0x%llX)", exception_addr, process_offset, module_offset);
		if (module_is_filled) {
			utils::write_log("Module: %s", file_name);
		}
		utils::write_log("Exception code: 0x%llX", ExceptionRecord->ExceptionCode);

		// Dump registers
		utils::write_log("Rax: 0x%llX", ContextRecord->Rax);
		utils::write_log("Rcx: 0x%llX", ContextRecord->Rcx);
		utils::write_log("Rdx: 0x%llX", ContextRecord->Rdx);
		utils::write_log("Rbx: 0x%llX", ContextRecord->Rbx);
		utils::write_log("Rsp: 0x%llX", ContextRecord->Rsp);
		utils::write_log("Rbp: 0x%llX", ContextRecord->Rbp);
		utils::write_log("Rsi: 0x%llX", ContextRecord->Rsi);
		utils::write_log("Rdi: 0x%llX", ContextRecord->Rdi);
		utils::write_log("R8: 0x%llX", ContextRecord->R8);
		utils::write_log("R9: 0x%llX", ContextRecord->R9);
		utils::write_log("R10: 0x%llX", ContextRecord->R10);
		utils::write_log("R11: 0x%llX", ContextRecord->R11);
		utils::write_log("R12: 0x%llX", ContextRecord->R12);
		utils::write_log("R13: 0x%llX", ContextRecord->R13);
		utils::write_log("R14: 0x%llX", ContextRecord->R14);
		utils::write_log("R15: 0x%llX", ContextRecord->R15);

		// Dump stack
		for (int i = 0; i < 50; ++i) {
			utils::write_log("Rsp+%i: 0x%llX", i * sizeof(std::uintptr_t), reinterpret_cast<std::uintptr_t*>(ContextRecord->Rsp)[i]);
		}

		utils::end_exception();

	}
	
	LONG CALLBACK hookHandler(LPEXCEPTION_POINTERS ex)
	{
		auto record = ex->ExceptionRecord;
		auto ctx = ex->ContextRecord;

		if (ex->ExceptionRecord->ExceptionCode == 0xC0000005) {

			if (ctx->Rip == OFFSET(0x11D2580)) // 0x11D2580 66 83 BE ? ? ? ? ? 0F 95 C0 8B 84 81 ? ? ? ?
			{
				ctx->Rax = FALSE;
				ctx->Rip = OFFSET(0x11D2592);
				ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "CG_GetEntityImpactType crash prevented" });

				return EXCEPTION_CONTINUE_EXECUTION;
			}
			else if (ctx->Rip == OFFSET(0x22C4935)) // 0x22C4935 44 0F B6 B1 ?? ?? ?? ?? 33 FF 4C 89 7C 24 ?? 4C 8B 79 18
			{
				ctx->Rax = FALSE;
				ctx->Rip = OFFSET(0x22C49FE);
				ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "DObjGetBoneIndex crash prevented" });

				return EXCEPTION_CONTINUE_EXECUTION;
			}

			//else if (utils::is_address_within_range(ctx->Rip, OFFSET(0x1E724A0), OFFSET(0x1E7273F))) // 0x1E724A0 - 0x1E7273F 8B 43 04 89 44 24 74 C7 44 24 ?? ?? ?? ?? ?? C6 44 24 ?? ?? B3 01
			//{
			//	ctx->Rip = OFFSET(0x1E72726);
			//	ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "LiveInvites_SendJoinInfo prevented" });

			//	return EXCEPTION_CONTINUE_EXECUTION;
			//}

			else {				
				catch_exception(ex->ExceptionRecord, ex->ContextRecord);
				return EXCEPTION_CONTINUE_EXECUTION;
			}
		}
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	void initExceptionHandler() {		

		AddVectoredExceptionHandler(1, hookHandler);

	}


}