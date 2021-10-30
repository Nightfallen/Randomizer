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

typedef struct {
	HWND hwnd;
	ImVec2 nWndRnd;
	ImVec2 nWndSets;
	int timer;		// in seconds
	bool bTimer;
	int theme;
	bool bSettings; // settings open flag

} AppSettings;

#endif // !SOURCE_STRUCTURES_HPP 