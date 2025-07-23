#pragma once
#include "includes.h"

namespace hooks {

    void nlog(const wchar_t* str, ...);

    namespace functions {

        unsigned int hkSteam_UserHasLicenseForApp(__int64 steamID, unsigned __int64 appID);

    }

    extern DWORD_PTR pUserData;
    extern DWORD_PTR pNameBuffer;

    void applyPatches();
	void initialize();
    void onFrame();

    inline void renamePlayer(std::string name)
    {
        if (name.size() > 31)
            name.resize(31);

        strncpy_s((char*)(pUserData + 0x8), 32, name.c_str(), name.size());
        strncpy_s((char*)(pNameBuffer), 32, name.c_str(), name.size());
    }

}
