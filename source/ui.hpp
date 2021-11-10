#ifndef SOURCE_UI_HPP
#define SOURCE_UI_HPP

// In this file located UI's element's functions

#include "source/precompiled.h"
#include "imgui/imgui_internal.h"
#include "source/constants.hpp"
#include "source/structures.hpp"
#include "source/Keyboard.hpp"

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

// Helper function are located in anonymous namespace
namespace {
	std::string GenerateStringFromKeys(const std::vector<int>& keys)
	{
		if (keys.empty())
			return "None";

		std::string result;
		result.reserve(128);

		const char* separator = "";
		for (auto&& key : keys)
		{
			result.append(separator).append(KeyNames[key]);
			separator = " + ";
		}

		return result;
	}
}

bool HotKeyEx(const char* label, HotKey& hotkey, const ImVec2& size_arg = ImVec2(0, 0))
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext* context = ImGui::GetCurrentContext();
	auto& io = ImGui::GetIO();
	const ImGuiStyle* style = &ImGui::GetStyle();

	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	ImVec2 size = ImGui::CalcItemSize(size_arg, ImGui::CalcItemWidth(), label_size.y + style->FramePadding.y * 2.f);
	const ImRect frame_bb(window->DC.CursorPos + ImVec2(label_size.x , 0.0f), window->DC.CursorPos + size);
	const ImRect total_bb(window->DC.CursorPos, frame_bb.Max);

	ImGui::ItemSize(total_bb, style->FramePadding.y);
	if (!ImGui::ItemAdd(total_bb, id, 0, ImGuiItemFlags_Inputable))
		return false;

	//const bool focus_requested = (ImGui::GetItemStatusFlags() & ImGuiItemStatusFlags_Focused) != 0;
	const bool focus_requested_by_tab = (ImGui::GetItemStatusFlags() & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
	//const bool focus_requested_by_code = (ImGui::GetItemStatusFlags() & ImGuiItemStatusFlags_FocusedByCode) != 0;
	static bool hovered = false, held = false;

	hovered = ImGui::ItemHoverable(frame_bb, id);
	held = hovered && io.MouseDown[0];
	const bool user_clicked = hovered && io.MouseClicked[0];

	if (hovered) {
		ImGui::SetHoveredID(id);
		context->MouseCursor = ImGuiMouseCursor_TextInput;
	}

	auto& [keys, default_keys, nMaxKeys] = hotkey;
	if (user_clicked) {
		if (context->ActiveId != id) {
			// Start edition
			memset(io.MouseDown, 0, sizeof(io.MouseDown));
			memset(io.KeysDown, 0, sizeof(io.KeysDown));

			keys = {};
		}
		ImGui::SetActiveID(id, window);
		ImGui::FocusWindow(window);
	}
	else if (io.MouseClicked[0]) {
		// Release focus when we click outside
		if (context->ActiveId == id)
			ImGui::ClearActiveID();
		if (keys.empty())
			keys = default_keys;
	}
	if (focus_requested_by_tab)
	{
		// Release focus when it was requested by tab
		if (context->ActiveId == id)
			ImGui::ClearActiveID();
	}

	bool value_changed = false;
	int key = 0;
	if (ImGui::IsItemDeactivated() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
	{
		keys = default_keys = {};
		value_changed = true;
		ImGui::ClearActiveID();
	}

	if (context->ActiveId == id) 
	{
		if (ImGui::IsKeyPressed(ImGuiKey_Enter))
		{
			value_changed = true;
			ImGui::ClearActiveID();
		}

		for (auto i = 2; i < 5; i++) 
		{
			if (io.MouseDown[i]) 
			{
				switch (i) 
				{
				case 2:
					key = VK_MBUTTON;
					break;
				case 3:
					key = VK_XBUTTON1;
					break;
				case 4:
					key = VK_XBUTTON2;
					break;
				}
				value_changed = true;

				auto nSize = keys.size();
				if (nSize < nMaxKeys && value_changed)
				{
					if (std::find(keys.begin(), keys.end(), key) == keys.end())
						keys.push_back(key);
				}
				if (nMaxKeys == nSize)
					ImGui::ClearActiveID();
			}
		}

		if (!value_changed) 
		{
			for (auto i = VK_BACK; i <= VK_RMENU; i++) 
			{
				if (io.KeysDown[i]) 
				{
					key = i;
					value_changed = true;

					auto nSize = keys.size();
					if (nSize < nMaxKeys && value_changed)
					{
						if (std::find(keys.begin(), keys.end(), key) == keys.end())
							keys.push_back(key);
					}
					if (nMaxKeys == nSize)
						ImGui::ClearActiveID();
				}
			}
		}
	}

	if (!keys.empty())
		default_keys = keys;

	ImVec4 color_frame = hovered ? style->Colors[ImGuiCol_FrameBgHovered] : style->Colors[ImGuiCol_FrameBg];
	if (user_clicked || held)
		color_frame = style->Colors[ImGuiCol_FrameBgActive];
	if (context->ActiveId == id)
		color_frame = style->Colors[ImGuiCol_FrameBgActive];
	ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(color_frame), true, style->FrameRounding);
	

	std::string buf_display = GenerateStringFromKeys(keys);
	int index_blink = (int)(ImGui::GetTime() / 0.33f) & 2;
	if (index_blink && context->ActiveId == id)
		buf_display = "";
	if (context->ActiveId == id && keys.empty())
		buf_display = "<Press a key>";

	const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y); // Not using frame_bb.Max because we have adjusted size
	ImVec2 render_pos = frame_bb.Min + style->FramePadding;
	ImGui::RenderTextClipped(frame_bb.Min + style->FramePadding, frame_bb.Max - style->FramePadding, buf_display.data(), NULL, NULL, style->ButtonTextAlign, &clip_rect);

	if (label_size.x > 0)
		ImGui::RenderText(ImVec2(total_bb.Min.x, frame_bb.Min.y + style->FramePadding.y), label);

	return value_changed;
}


#endif // !SOURCE_UI_HPP 