#include "main_hook.h"


namespace hooks {

	//1 is Warning, 2 is Crash, 3 is Success, 4 is Error, 0 for no prefix.
	void Notify(int Type, std::string Message) {

		std::string Prefix = "";

		switch (Type)
		{
		case 1:
			Prefix = "[^3WARNING^7]: ";
			break;
		case 2:
			Prefix = "[^1CRASH^7]: ";
			break;
		case 3:
			Prefix = "[^2SUCCESS^7]: ";
			break;
		case 4:
			Prefix = "[^1ERROR^7]: ";
			break;
		default:
			Prefix = "";
			break;
		}

		std::string msg_to_send = Prefix + Message;

		if (bMatchLoaded) {

			Game_Message(0, msg_to_send.data());

		}

		else {

			CG_BoldGameMessageCenter(0, msg_to_send.data());

		}
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

		std::abort();
	}

	void handle_message(const std::uint64_t sender_id, const char* message, const std::uint32_t message_size)
	{
		msg_t msg{};
		MSG_InitReadOnly(&msg, message, message_size);
		MSG_BeginReading(&msg);

		auto type = 0ui8;
		std::string outputtype = std::to_string(type);

		if (MSG_ReadByte(&msg) == '1')
		{
			type = MSG_ReadByte(&msg);
		}

		auto gamertag = "";
		__int64 vTable = *(__int64*)(ProcessBase + 0x10BBCBA0);
		gamertag = (CHAR*)(*(__int64(__fastcall**)(__int64, __int64))(*(__int64*)vTable + 56i64))(vTable, sender_id);

		if (type == 'h')
		{
			const auto size{ msg.cursize - msg.readcount };
			if (size < 2048u)
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

					}

					if (lobby_msg.msgType == MESSAGE_TYPE_INFO_REQUEST)
					{
						if (sender_id != LiveUser_GetXuid(0)) {
							Notify(1, "Info request from " + std::to_string(sender_id) + " [" + gamertag + "]");
						}
					}

					LobbyMsg_HandleIM(0, sender_id, data, size);

				}
			}
		}


		if (type == 102 && message_size == 2 || type == 102 && message_size == 102) {
			//do notify message here
			if (message_size == 102) {
				Notify(2, "Crash attempt blocked " + std::string("[") + gamertag + "]");
			}
			else {
				Notify(1, "Popup attempt blocked " + std::string("[") + gamertag + "]");
			}
		}

	}


	__int64 hkdwInstantDispatchMessage(__int64 sender_id, unsigned int controllerIndex, __int64 msg, unsigned int messageSize)
	{
		const auto message{ reinterpret_cast<const char*>(msg) };
		const auto message_size{ messageSize };

		handle_message(sender_id, message, message_size);

		std::srand(time(NULL));
		*(__int32*)((INT64)GetModuleHandleA(NULL) + 0x112EFA14) = rand();

		return 0;
	}

	char hkNameLength(unsigned int a1, int* a2, int a3) {

		__int64 v3; // rbx
		int* v6; // rax
		int v9; // [rsp+40h] [rbp+8h] BYREF

		v3 = a3;
		v9 = a3;
		Name_Thing(a2, 0i64, a3);
		v6 = &v9;

		//there was a check here in the original function that limited all text coming through to be 16 characters long.

		Steam_GetName_Thing((UINT8*)a2, *v6, 1);

		/*if (a1) //if controllerIndex: add the controller index at the end of the buffer. commented out bc who would play in splitscreen lul.
		{
			v7 = va(" %d", a1);
			sub_22E9F00(a2, v3, v7);
		}*/
		return 1;
	}

	__int64 hkAuthFunction(__int64 a1, __int64* a2) {
		return 0;
	}

	

	LONG CALLBACK hooks::hookHandler(LPEXCEPTION_POINTERS ex)
	{
		if (ex->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP)
		{
			/*if (ex->ContextRecord->Rip == (uintptr_t)centity_location)
			{
				ex->ContextRecord->Rdi = ex->ContextRecord->Rcx;
				ex->ContextRecord->Rip = (uintptr_t)centity_location + 0x3;
				cg_EntitiesArray = (ex->ContextRecord->Rcx - (0x900 * 500));
			}*/
			if (ex->ContextRecord->Rip == (uintptr_t)(ProcessBase + 0x1EC75D0))
			{
				ex->ContextRecord->Rip = (uintptr_t)hkNameLength;
			}

			else if (ex->ContextRecord->Rip == (uintptr_t)(ProcessBase + 0x143A600)) {
				ex->ContextRecord->Rip = (uintptr_t)hkdwInstantDispatchMessage;
			
			}


			return EXCEPTION_CONTINUE_EXECUTION;

		}

		else {
			catch_exception(ex->ExceptionRecord, ex->ContextRecord);
		}


		return EXCEPTION_CONTINUE_SEARCH;


	}

}