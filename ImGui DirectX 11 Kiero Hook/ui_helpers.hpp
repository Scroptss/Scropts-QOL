#pragma once
#include "includes.h"

namespace ui
{
	// Draws a wrapped tooltip bound to the previously submitted ImGui item.
	inline void TooltipOnHover(const char* desc)
	{
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
}
