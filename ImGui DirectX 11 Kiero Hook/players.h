#pragma once

#include <cstdint>
#include "includes.h"


struct netadr_t;
struct InfoResponseLobby;

namespace players
{
	void OnInfoResponse(std::uint64_t senderXuid, const Msg_InfoResponse& response);
	void draw();
	bool Install();
}