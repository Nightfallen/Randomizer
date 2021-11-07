#ifndef SOURCE_KEYBOARD_HPP
#define SOURCE_KEYBOARD_HPP

#include "precompiled.h"
#include "structures.hpp"

bool IsKeyPressed(int vk_code)
{
	return GetKeyState(vk_code) & 0x8000;
}

bool IsHotkeyPressed(const HotKey& hotkey)
{
	auto& keys = hotkey.keys;
	bool result = false;
	if (keys.empty())
		return false;

	auto& io = ImGui::GetIO();

	static bool just_pressed = true;

	bool is_hotkey_pressed = true;
	for (auto&& key : keys)
		is_hotkey_pressed &= IsKeyPressed(key);

	if (is_hotkey_pressed)
	{
		if (just_pressed)
		{
			just_pressed = false;
			result = true;
		}

	}
	else
	{
		just_pressed = true;
	}

	return result;
}

#endif // !SOURCE_KEYBOARD_HPP 