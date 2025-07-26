#include "hooks.h"

namespace hooks {

	DWORD_PTR pUserData = ProcessBase + 0x14FB3420;
	DWORD_PTR pNameBuffer = ProcessBase + 0x15E84638;

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

	void load_custom_jpg()
	{
		if (custom_jpg_buf)
		{
			*(uint64_t*)(*(uint64_t*)(ProcessBase + 0x197EBA00) + 0x5A8498) = NULL;
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

	void nlog(const wchar_t* str, ...)
	{
		va_list ap;
		HWND notepad, edit;
		wchar_t buf[256];

		va_start(ap, str);
		vswprintf(buf, str, ap);
		va_end(ap);
		wcscat(buf, L"\r\n");
		notepad = FindWindowW(NULL, L"Untitled - Notepad");
		if (!notepad)
		{
			notepad = FindWindowW(NULL, L"*Untitled - Notepad");
		}
		edit = FindWindowExW(notepad, NULL, L"EDIT", NULL);
		SendMessage(edit, EM_REPLACESEL, TRUE, (LPARAM)buf);
	}

	namespace functions {

		// Patches 

		// Loadside Crash
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

					if (game_im == 'e')
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
		/***********************************************************************************/

		bool hkUI_IsRenderingImmediately() {

			return true;

		}

		void hkR_ConvertColorToBytes(ImVec4* color, byte* bytes) {

			auto desiredColor = bUIRgb ? (ImVec4)mainRgb() : (ImVec4)uiColor;

			void* returnAddress = _ReturnAddress();

			if (bColoredUI && returnAddress != nullptr && color->w > 0.1f) {

				if (returnAddress == (void*)(ProcessBase + 0x1F34A23) || returnAddress == (void*)(ProcessBase + 0x1CD998F) || returnAddress == (void*)(ProcessBase + 0x2814DC7)
					|| returnAddress == (void*)(ProcessBase + 0x1CD92A7) || returnAddress == (void*)(ProcessBase + 0x1CD9FB3) || returnAddress == (void*)(ProcessBase + 0x228192A)) {
					return R_ConvertColorToBytes(&desiredColor, bytes);
				}

			}

			return R_ConvertColorToBytes(color, bytes);

		}

		unsigned int hkSteam_UserHasLicenseForApp(__int64 steamID, unsigned __int64 appID) {

			ImGui::InsertNotification({ ImGuiToastType::Info, 5000, "UserHasLicenseForApp called for %llu, returning as valid...", steamID });

			return 0;
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

			__int64 xuid = *userObj;
			//ImGui::InsertNotification({ ImGuiToastType::Info, 5000, "UserHasLicenseForApp called for %llu, returning as valid...", xuid });

			//*(DWORD*)(mapInfo + 0xC18) = 0; // Would jump to successful ownership but caused XP issues.

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

			auto g_entity = *(__int64*)(ProcessBase + 0x1770D4E8);
			auto g_entityP2 = (__int64)(g_entity + 0x4F8);
			auto g_entityP3 = (__int64)(g_entity + 0x9F0);
			auto g_entityP4 = (__int64)(g_entity + 0xEE8);
			auto newDamage = (int)(damage * iDamageMultiplier);

			if (LobbyTypes_GetMainMode() == LOBBY_MAINMODE_ZM) {

				if (targ == g_entity || targ == g_entityP2 || targ == g_entityP3 || targ == g_entityP4) {

					if (bThorns) {

						if (bNukes) {
							for (int i = 4; i < 2047; ++i) {
								__int64 target = static_cast<__int64>(g_entity) + (0x4F8 * i);
								spoof_call(spoof_t, G_Damage, target, targ, targ, a4, a5, (int)9999999, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);
							}
						}

						spoof_call(spoof_t, G_Damage, attacker, targ, targ, a4, a5, newDamage, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);
					}

					if (bGodMode) {
						return 0;
					}
				}
				else {
					if (bDamageMultiplier) {
						return spoof_call(spoof_t, G_Damage, targ, inflictor, attacker, a4, a5, newDamage, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);
					}				

					if (bNukes) {
						for (int i = 4; i < 2047; ++i) {
							__int64 target = static_cast<__int64>(g_entity) + (0x4F8 * i);
							spoof_call(spoof_t, G_Damage, target, inflictor, attacker, a4, a5, (int)9999999, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);
						}
					}
				}
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
				strncpy((char*)((ProcessBase + 0x197EBA00) + 0x6706EA), titleBuf.c_str(), 32);

			}

			if (!custom_desc_buf.empty()) {

				ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Desc..." });

				static std::string descBuf = utils::decodeEncodedChars(custom_desc_buf);

				strncpy((char*)metaDataSize + 0x287, descBuf.c_str(), 32);
				*(bool*)(metaDataSize + 0x307) = true;
				strncpy((char*)((ProcessBase + 0x197EBA00) + 0x67072F), descBuf.c_str(), 32);

			}

			return Demo_SetMetaData(controllerIndex, metaData, outBufferSize, metaDataSize, info, duration);
		}

		bool hkFileshare_CreateMetaData(__int64 a1, FileshareMetaInfo* metaInfo, int* a3, int a4) {

			if ((DWORD_PTR)_ReturnAddress() == (ProcessBase + 0x260A141)) {

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
					jpg_buf_o = *(uint64_t*)(*(uint64_t*)(ProcessBase + 0x197EBA00) + 0x5A8490);
				}
				ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Screenshot..." });
				*(uint64_t*)(*(uint64_t*)(ProcessBase + 0x197EBA00) + 0x5A8490) = (uint64_t)custom_jpg_buf;
				*(uint32_t*)(*(uint64_t*)(ProcessBase + 0x197EBA00) + 0x5A8498) = custom_jpg_buf_size;
			}

			if (custom_thumb_buf) {
				if (custom_thumb_buf_size <= 0x88000) {
					ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Thumbnail..." });
					DWORD oldProtect;

					uintptr_t imageBufferAddress = (*(uint64_t*)(ProcessBase + 0x197EBA00)) + 0x5A84A0;
					char* targetBuffer = reinterpret_cast<char*>(imageBufferAddress);
					memcpy((LPVOID)targetBuffer, custom_thumb_buf, custom_thumb_buf_size);
					*(uint32_t*)(*(uint64_t*)(ProcessBase + 0x197EBA00) + 0x5A849C) = custom_thumb_buf_size;

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



	}

	void initPointerSwaps() {
		DWORD oldprotect;

		// Save the original functions
		VirtualProtect((LPVOID)(*(__int64*)iSteamApps + 48i64), 16, PAGE_EXECUTE_READWRITE, &oldprotect);
		
		Steam_bIsDlcInstalled_o = (decltype(&functions::hkSteam_bIsDlcInstalled))(*(__int64*)(*(__int64*)iSteamApps + 48i64));
		Steam_bIsAppInstalled_o = (decltype(&functions::hkSteam_bIsAppInstalled))(*(__int64*)(*(__int64*)iSteamApps + 56i64));
		//Steam_bIsAppInstalled_o = (decltype(&functions::hkSteam_UserHasLicenseForApp))(*(__int64*)(*(__int64*)iSteamApps + 256i64));

		// Replace the vtable pointers
		*(__int64*)(*(__int64*)iSteamApps + 48i64) = (__int64)functions::hkSteam_bIsDlcInstalled;
		*(__int64*)(*(__int64*)iSteamApps + 56i64) = (__int64)functions::hkSteam_bIsAppInstalled;
		
		VirtualProtect((LPVOID)(*(__int64*)iSteamApps + 48i64), 16, oldprotect, &oldprotect);


		// Steam Peer to Peer
		__int64 ptr = *(__int64*)(ProcessBase + 0x10BBCBD0);
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

		auto ptr = *(uintptr_t*)(ProcessBase + 0x9BB4878);
		auto vmt = **(uintptr_t***)(ptr + 1384);

		auto vtable_buf = new uintptr_t[50];
		for (auto count = 0; count < 50; ++count) {
			vtable_buf[count] = vmt[count];
		}

		**(uintptr_t**)(ptr + 1384) = (uintptr_t)vtable_buf;
		*(uintptr_t*)(**(uintptr_t**)(ptr + 1384) + 192) = (uintptr_t)functions::Global_InstantMessage;

	}

	void onFrame() {

		static bool dispatch_enabled = false;

		if (!dispatch_enabled && Live_IsUserSignedInToDemonware(CONTROLLER_INDEX_0))
		{
			initialize_dispatch();
			dispatch_enabled = true;
			ImGui::InsertNotification({ ImGuiToastType::Success, 3000, "Patches Enabled" });
		}
	}

	void applyPatches() {

		 //Patches 

		MH_CreateHook((LPVOID)(ProcessBase + 0x1321110), functions::hkCL_GetConfigString, (LPVOID*)&CL_GetConfigString);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1321110), functions::hkLobbyMsgRW_PackageElement, (LPVOID*)&LobbyMsgRW_PackageElement);
		//MH_CreateHook((LPVOID)(ProcessBase + 0x143A600), functions::hkdwInstantDispatchMessage, (LPVOID*)&dwInstantDispatchMessage);
		//MH_CreateHook((LPVOID)(ProcessBase + 0x1F34920), functions::hkUI_Interface_DrawText, (LPVOID*)&UI_Interface_DrawText);

		MH_CreateHook((LPVOID)(ProcessBase + 0x1980960), functions::hkG_Damage, (LPVOID*)&G_Damage);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1D1CC10), functions::hkR_ConvertColorToBytes, (LPVOID*)&R_ConvertColorToBytes);
		MH_CreateHook((LPVOID)(ProcessBase + 0x2705710), functions::hkUI_IsRenderingImmediately, (LPVOID*)&UI_IsRenderingImmediately);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1DF61E0), functions::hkFileshare_GetSummaryFileAuthorXUID, (LPVOID*)&Fileshare_GetSummaryFileAuthorXUID);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1E87430), functions::hkFileshare_CanDownloadFile, (LPVOID*)&Fileshare_CanDownloadFile);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1EB7130), functions::hkUserHasLicenseForApp, (LPVOID*)&UserHasLicenseForApp);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1E124E0), functions::hkLiveEntitlements_IsEntitlementActiveForController, (LPVOID*)&LiveEntitlements_IsEntitlementActiveForController);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1E08950), functions::hkLiveInventory_AreExtraSlotsPurchased, (LPVOID*)&LiveInventory_AreExtraSlotsPurchased);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1E09030), functions::hkLiveInventory_GetItemQuantity, (LPVOID*)&LiveInventory_GetItemQuantity);
		MH_CreateHook((LPVOID)(ProcessBase + 0x1EC75D0), functions::hkLive_UserGetName, (LPVOID*)&Live_UserGetName);
		MH_CreateHook((LPVOID)(ProcessBase + 0x2342100), functions::hkflsomeWeirdCharacterIndex, (LPVOID*)&flsomeWeirdCharacterIndex);
		MH_CreateHook((LPVOID)(ProcessBase + 0x2609B00), functions::hkDemo_SaveScreenshotToContentServer, (LPVOID*)&Demo_SaveScreenshotToContentServer);
		MH_CreateHook((LPVOID)(ProcessBase + 0x260A040), functions::hkDemo_SetMetaData, (LPVOID*)&Demo_SetMetaData);
		MH_EnableHook(MH_ALL_HOOKS);
	}

	void restorePatches() {

		// Patches

		MH_RemoveHook((LPVOID)(ProcessBase + 0x1321110));		//hkCL_GetConfigString
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1EF65C0));		//hkLobbyMsgRW_PackageElement

		MH_RemoveHook((LPVOID)(ProcessBase + 0x1980960));		//G_Damage
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1D1CC10));		//hkR_ConvertColorToBytes
		MH_RemoveHook((LPVOID)(ProcessBase + 0x2705710));		//hkUI_IsRenderingImmediately
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1DF61E0));		//hkFileshare_GetSummaryFileAuthorXUID
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1E87430));		//hkFileshare_CanDownloadFile
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1EB7130));		//hkUserHasLicenseForApp
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1E124E0));		//hkLiveEntitlements_IsEntitlementActiveForController
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1E08950));		//hkLiveInventory_AreExtraSlotsPurchased
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1E09030));		//hkLiveInventory_GetItemQuantity
		MH_RemoveHook((LPVOID)(ProcessBase + 0x1EC75D0));		//hkLive_UserGetName
		MH_RemoveHook((LPVOID)(ProcessBase + 0x2342100));		//flsomeWeirdCharacterIndex
		MH_RemoveHook((LPVOID)(ProcessBase + 0x2609B00));		//Demo_SaveScreenshotToContentServer
		MH_RemoveHook((LPVOID)(ProcessBase + 0x260A040));		//Demo_SetMetaData
		MH_DisableHook(MH_ALL_HOOKS);
	}

	void catch_exception(LPEXCEPTION_RECORD ExceptionRecord, LPCONTEXT ContextRecord)
	{
		MEMORY_BASIC_INFORMATION mbi;
		VirtualQuery(ExceptionRecord->ExceptionAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

		char modulename[MAX_PATH];
		bool module_is_filled = false;
		char file_name[260];
		DWORD module_name = 0;
		if (RtlPcToFileHeader(ExceptionRecord->ExceptionAddress, &mbi.AllocationBase)) {
			module_name = GetModuleFileName((HMODULE)reinterpret_cast<std::uintptr_t>(mbi.AllocationBase), modulename, sizeof(modulename));
			char* last_slash = strrchr(modulename, '\\');
			strcpy_s(file_name, last_slash + 1);
			module_is_filled = true;
		}

		auto module_offset = reinterpret_cast<std::uintptr_t>(ExceptionRecord->ExceptionAddress) - reinterpret_cast<std::uintptr_t>(mbi.AllocationBase);

		utils::begin_exception();
		utils::write_log("Image base: 0x%llX", ProcessBase);
		utils::write_log("Cheat base: 0x%llX", dllbase);

		utils::write_log("Exception at: [ 0x%llX ] ", ContextRecord->Rip);
		utils::write_log("Exception address: 0x%llX (0x%llX)", ExceptionRecord->ExceptionAddress, module_offset);
		if (module_is_filled) {
			utils::write_log("Module: %s ", file_name);
		}
		utils::write_log("Exception code: 0x%llX", ExceptionRecord->ExceptionCode);

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

		for (auto i{ 0 }; i < 50; i++)
		{
			utils::write_log("Rsp+%i: 0x%llX", i * sizeof(std::uintptr_t), reinterpret_cast<std::uintptr_t*>(ContextRecord->Rsp)[i]);
		}

		utils::end_exception();

	}

	LONG CALLBACK hookHandler(LPEXCEPTION_POINTERS ex)
	{

		auto record = ex->ExceptionRecord;
		auto ctx = ex->ContextRecord;

		if (ex->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
		{

			if (ctx->Dr1 == OFFSET(0x1F9CCE26))
			{
				applyPatches();
				ctx->Dr1 = OFFSET(0x3000);

				return EXCEPTION_CONTINUE_EXECUTION;
			}
			else if (ctx->Dr1 == OFFSET(0x3000))
			{
				restorePatches();
				ctx->Dr1 = OFFSET(0x1F9CCE26);

				return EXCEPTION_CONTINUE_EXECUTION;
			}
			return EXCEPTION_CONTINUE_EXECUTION;
		}

		if (ex->ExceptionRecord->ExceptionCode == 0xC0000005) {

			if (ctx->Rip == OFFSET(0x11D256B))
			{
				ctx->Rax = FALSE;
				ctx->Rip = OFFSET(0x11D2572);
				ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "CG_GetEntityImpactType crash prevented" });

				return EXCEPTION_CONTINUE_EXECUTION;
			}
			else if (ctx->Rip == OFFSET(0x233D3E5))
			{
				ctx->Rax = FALSE;
				ctx->Rip = OFFSET(0x233D4AE);
				ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "DObjGetBoneIndex crash prevented" });

				return EXCEPTION_CONTINUE_EXECUTION;
			}

			else if (utils::is_address_within_range(ctx->Rip, OFFSET(0x1E7E870), OFFSET(0x1E7EB0F)))
			{
				ctx->Rip = OFFSET(0x1E7EAF6);
				ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "LiveInvites_SendJoinInfo prevented" });

				return EXCEPTION_CONTINUE_EXECUTION;
			}

			else {				
				catch_exception(ex->ExceptionRecord, ex->ContextRecord);
				return EXCEPTION_CONTINUE_EXECUTION;
			}
		}
	
		return EXCEPTION_CONTINUE_SEARCH;
	}

	void initialize() {

		AddVectoredExceptionHandler(1, hookHandler);

		THREADENTRY32 te32;
		HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

		if (hThreadSnap)
		{
			te32.dwSize = sizeof(THREADENTRY32);

			if (!Thread32First(hThreadSnap, &te32))
			{
				CloseHandle(hThreadSnap);
			}
			else {
				do
				{
					if (te32.th32OwnerProcessID == GetCurrentProcessId() && te32.th32ThreadID != GetCurrentThreadId())
					{
						HANDLE hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, 0, te32.th32ThreadID);
						if (hThread)
						{
							CONTEXT context;
							context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
							SuspendThread(hThread);

							if (GetThreadContext(hThread, &context))
							{
								context.Dr0 = 0;
								context.Dr1 = OFFSET(0x3000);
								context.Dr2 = 0;
								context.Dr3 = 0;

								context.Dr7 = (1 << 0) | (1 << 20) | (1 << 21) | (1 << 2) | (1 << 4) | (1 << 6);

								SetThreadContext(hThread, &context);
							}

							ResumeThread(hThread);
							CloseHandle(hThread);
						}
					}
				} while (Thread32Next(hThreadSnap, &te32));
				CloseHandle(hThreadSnap);
			}
		}

		applyPatches();
		initPointerSwaps();

	}
}