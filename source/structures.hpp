#ifndef SOURCE_STRUCTURES_HPP
#define SOURCE_STRUCTURES_HPP

#include "source/precompiled.h"

typedef void (*ProcUIHandler)(HWND hwnd);

enum THEMES {
	DARK1,
	DARK2,
	HALFLIFE,
	WHITE1
};

struct HotKey
{
	std::vector<int> keys;
	std::vector<int> default_keys;
	size_t nMaxKeys;
	HotKey() = default;
	HotKey(const std::vector<int> in_keys) :
		keys(in_keys), default_keys(in_keys), nMaxKeys(2) {}
	auto operator<=>(const HotKey&) const = default;
};

typedef struct {
	HWND hwnd;
	ImVec2 nWndRnd;
	ImVec2 nWndSets;
	ImVec2 posWndRnd;
	ImVec2 posWndSets;
	int timer;		// in seconds
	bool bTimer;
	int theme;
	bool bSettings; // settings open flag
	HotKey hotkeyRnd;
} AppSettings;

#endif // !SOURCE_STRUCTURES_HPP 