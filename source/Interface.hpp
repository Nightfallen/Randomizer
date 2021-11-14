#ifndef SOURCE_INTERFACE_HPP
#define SOURCE_INTERFACE_HPP

#include "source/precompiled.h"
#include "fonts/DefinesFontAwesome.hpp"
#include "source/helpers.hpp"
#include "source/Settings.hpp"
#include "source/structures.hpp"
#include "source/Styles.hpp"
#include "source/ui.hpp"
#include "source/Keyboard.hpp"

#define DefineSettingsBinding(x) auto& [hwnd, nWndRnd, nWndSets, posWndRnd, posWndSets, timer, bTimer, theme, bSettings, hotkey_random] = settings

void SettingsWindow(bool* is_open, AppSettings& settings)
{
	auto& style = ImGui::GetStyle();
	DefineSettingsBinding(settings);

	static bool just_once = true;
	if (just_once)
	{
		ImGui::SetNextWindowPos(settings.posWndSets);
		ImGui::SetNextWindowSize(settings.nWndSets);
		just_once = false;
	}


	ImGui::Begin("Settings", is_open, ImGuiWindowFlags_NoBackground);

	ImVec2 szItemWidth = ImGui::GetWindowSize() - style.WindowPadding * 2;
	szItemWidth.y = 0;
	ImGui::PushItemWidth(szItemWidth.x);

	ImGui::Checkbox("Enable Auto-Random", &bTimer);

	if (!bTimer) ImGui::BeginDisabled();
	ImGui::DragInt("##Auto-Random timer", &timer, 1.0f, 1, 60, "%ds", ImGuiSliderFlags_AlwaysClamp);
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted("Auto-Random timer");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
	if (!bTimer) ImGui::EndDisabled();



	const char* themes_items[] = { "Dark 1", "Dark 2", "Half-Life", "White" };
	ImGui::Combo("##Theme", &theme, themes_items, IM_ARRAYSIZE(themes_items), 4);
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted("Theme");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	HotKeyEx("##Random Number Hotkey", hotkey_random);
	if (auto active = ImGui::IsItemActive(); ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		if (active)
			ImGui::TextUnformatted("Press 'Esc' to remove hotkey\nPress 'Enter' to finish editing hotkey");
		else
			ImGui::TextUnformatted("Hotkey Random Number");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	ImGui::PopItemWidth();
	if (!ImGui::IsWindowCollapsed())
	{
		nWndSets = ImGui::GetWindowSize();
		posWndSets = ImGui::GetWindowPos();
	}
	ImGui::End();
}

void RandomizerWindow(bool* is_open, AppSettings& settings)
{
	auto& style = ImGui::GetStyle();
	DefineSettingsBinding(settings);
	static std::string text_rnd_number = "";
	std::string_view fmt_rnd_number = "{}";
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
	ImVec2 btnSize = ImGui::CalcTextSize(ICON_FA_WRENCH, NULL, true, 2.f);
	ImGui::PopFont();

	static bool just_once = true;
	if (just_once)
	{
		ImGui::SetNextWindowPos(settings.posWndRnd);
		ImGui::SetNextWindowSize(settings.nWndRnd);
		just_once = false;
	}

	ImGui::Begin("Randomizer", is_open, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
	static auto rand_int = GetRandomInt(0, 100);
	static auto start = std::chrono::steady_clock::now();
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<float> elapsed_seconds = end - start;

	if (bTimer && elapsed_seconds.count() >= timer || IsHotkeyPressed(hotkey_random))
	{
		rand_int = GetRandomInt(0, 100);
		start = end;
	}

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	auto szText = ImGui::CalcTextSize(std::format(fmt_rnd_number, rand_int).data());
	auto posCursor = ImGui::GetCursorPos();
	ImVec2 nextPosCursor = (ImGui::GetWindowSize() - szText) * 0.5f;
	nextPosCursor.y = posCursor.y - style.WindowPadding.y - style.FramePadding.y;
	nextPosCursor.x -= btnSize.x / 2;
	ImGui::SetCursorPos(nextPosCursor);
	ImGui::Text(std::format(fmt_rnd_number, rand_int).data());
	ImGui::PopFont();

	auto wndSize = ImGui::GetWindowSize();
	auto curStartPos = ImGui::GetCursorStartPos();

	ImVec2 resCurPos = curStartPos;
	resCurPos.x = wndSize.x - curStartPos.x;
	resCurPos = wndSize;
	resCurPos.y = curStartPos.y;
	resCurPos.x -= style.WindowPadding.x * 2;

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
	resCurPos.x -= btnSize.x;
	ImGui::SetCursorPos(resCurPos);
	if (ImGui::Button(ICON_FA_WRENCH, btnSize * 1.5f))
	{
		bSettings = !bSettings;
	}
	ImGui::PopFont();

	if (!ImGui::IsWindowCollapsed())
	{
		nWndRnd		= ImGui::GetWindowSize();
		posWndRnd	= ImGui::GetWindowPos();
	}
	
	ImGui::End();
}

void ChangeTheme(int themeCode)
{
	switch (themeCode)
	{
	case DARK1:	DarkStyle1();
		break;
	case DARK2: DarkStyle2();
		break;
	case HALFLIFE: HalfLifeStyle();
		break;
	case WHITE1: WhiteStyle1();
		break;
	default:
		break;
	}
}


void UI_HANDLER(HWND hwnd)
{
	RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
	// Our state
	static AppSettings settings = { ReadSettings() };
	settings.hwnd = hwnd;
	static bool show_randomizer = true;
	static auto prevTheme = -1;

	// Close 
	if (!show_randomizer)
	{
		SaveSettings(settings);
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	}

	if (show_randomizer)
		RandomizerWindow(&show_randomizer, settings);

	if (settings.bSettings)
		SettingsWindow(&settings.bSettings, settings);

	if (prevTheme != settings.theme)
	{
		ChangeTheme(settings.theme);
		prevTheme = settings.theme;
	}
}

#endif // !SOURCE_INTERFACE_HPP
