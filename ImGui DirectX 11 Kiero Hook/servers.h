#pragma once

#include "includes.h"

#include <string>

namespace servers
{
    bool Install();
    void draw();

    bool HandleStatusResponse(const netadr_t& from, msg_t& msg);
}