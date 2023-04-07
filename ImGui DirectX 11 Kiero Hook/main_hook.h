#pragma once
#include "includes.h"

namespace hooks {

	extern void Notify(int Type, std::string Message);

	LONG CALLBACK hookHandler(EXCEPTION_POINTERS* ex);

}