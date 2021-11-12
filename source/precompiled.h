#ifndef SOUCRE_PRECOMPILED_H
#define SOUCRE_PRECOMPILED_H

// Winapi headers
#define WIN32_LEAN_AND_MEAN		// to remove unnecessary things from 'Windows.h'
#include <Windows.h>
#include <d3d11.h>
#include <tchar.h>
#pragma comment(lib, "d3d11.lib")

// imgui headers
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_freetype.h"

// C++ headers
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <regex>
#include <string_view>
#include <string>
#include <vector>

// Vcpkg headers


#endif // !SOUCRE_PRECOMPILED_H
