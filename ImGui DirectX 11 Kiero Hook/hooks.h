#pragma once
#include "includes.h"

namespace hooks {

    extern DWORD_PTR pUserData;
    extern DWORD_PTR pNameBuffer;

    void applyPatches();
	void initExceptionHandler();
    void onFrame();
    void initPointerSwaps();

    bool is_user_in_game();
    bool is_user_in_lobby();
    bool AreWeInGameAndHosting();
    bool is_in_game();
    bool local_client_is_in_game();

    inline void renamePlayer(std::string name)
    {
        if (name.size() > 31)
            name.resize(31);

        strncpy_s((char*)(pUserData + 0x8), 32, name.c_str(), name.size());
        strncpy_s((char*)(pNameBuffer), 32, name.c_str(), name.size());
    }

}
