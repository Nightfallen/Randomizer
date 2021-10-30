#ifndef SOURCE_INTERFACE_HPP
#define SOURCE_INTERFACE_HPP

#include "source/precompiled.h"
#include "fonts/DefinesFontAwesome.hpp"
#include "source/helpers.hpp"
#include "source/Settings.hpp"
#include "source/structures.hpp"
#include "source/Styles.hpp"
#include "source/ui.hpp"


void SettingsWindow(bool* is_open, AppSettings& settings)
{
	auto& [hwnd, nWndRnd, nWndSets, timer, bTimer, theme, bSettings] = settings;

	ImGui::SetNextWindowSize(settings.nWndSets);
	ImGui::Begin("Settings", is_open);

	ImGui::Text("This is settings window :)");
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

	if (!ImGui::IsWindowCollapsed())
		nWndSets = ImGui::GetWindowSize();
	ImGui::End();
}

void RandomizerWindow(bool* is_open, AppSettings& settings)
{
	auto& [hwnd, nWndRnd, nWndSets, timer, bTimer, theme, bSettings] = settings;
	static std::string text_rnd_number = "";
	std::string_view fmt_rnd_number = "{}";

	RECT lpRect{};
	GetWindowRect(hwnd, &lpRect);
	int width = lpRect.right - lpRect.left;
	int height = lpRect.bottom - lpRect.top;

	ImGui::SetNextWindowSize(settings.nWndRnd);
	ImGui::Begin("randomizer", is_open);

	// Move main system window with current window
	// It's transparent but still need to be moved synchronously
	{
		static auto wndPos = ImGui::GetWindowPos();

		auto wndPos2 = ImGui::GetWindowPos();
		if (!AreEqual(wndPos2, wndPos))
		{
			wndPos = wndPos2;
			SetWindowPos(hwnd, HWND_TOPMOST, wndPos.x, wndPos.y, width, height, SWP_SHOWWINDOW);
		}
	}

	static auto rand_int = GetRandomInt(0, 100);
	static auto start = std::chrono::steady_clock::now();
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<float> elapsed_seconds = end - start;

	if (bTimer && elapsed_seconds.count() >= timer)
	{
		rand_int = GetRandomInt(0, 100);
		start = end;
	}


	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	auto szText = ImGui::CalcTextSize(std::format(fmt_rnd_number, rand_int).data());
	auto posCursor = ImGui::GetCursorPos();
	ImVec2 nextPosCursor = (ImGui::GetWindowSize() - szText) * 0.5f;
	nextPosCursor.y = posCursor.y;
	ImGui::SetCursorPos(nextPosCursor);
	ImGui::Text(std::format(fmt_rnd_number, rand_int).data());
	ImGui::PopFont();

	ImVec2 btnSize = { 90, 0 };
	if (ImGui::Button("Random!", btnSize))
	{
		rand_int = GetRandomInt(0, 100);
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_COG " Settings", btnSize))
	{
		bSettings = !bSettings;
	}

	if (!ImGui::IsWindowCollapsed())
		nWndRnd = ImGui::GetWindowSize();

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
	// Our state
	static AppSettings settings = { ReadSettings() };
	settings.hwnd = hwnd;
	static bool show_randomizer = true;
	auto prevTheme = -1;

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
