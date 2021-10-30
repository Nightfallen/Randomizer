#ifndef SOURCE_FONTS_HPP
#define SOURCE_FONTS_HPP

#include "source/precompiled.h"
#include "fonts/BinaryFontAwesome.hpp"
#include "fonts/BinaryFontRoboto.hpp"
#include "fonts/DefinesFontAwesome.hpp"

void BuildDefaultFont(ImGuiIO& io)
{
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };	// Will not be copied by AddFont* so keep in scope.
	static const ImWchar ranges[] =											// Will not be copied by AddFont* so keep in scope.
	{
		0x0020, 0x00FF,	// Latin ranges
		0x0400, 0x044F,	// Cyrillic ranges
		0,
	};
	static ImFontConfig font_config{};
	static ImFontConfig icons_config{};
	font_config.PixelSnapH = true;
	font_config.OversampleH = 2;
	font_config.OversampleV = 1;
	icons_config = font_config;
	icons_config.MergeMode = true;	// Merging icons into the first font

	ImFont* font1 = io.Fonts->AddFontFromMemoryCompressedTTF(binRobotoFont_compressed_data, binRobotoFont_compressed_size, 18.f, &font_config, ranges);
	ImFont* font2 = io.Fonts->AddFontFromMemoryCompressedTTF(binIconsFont_compressed_data, binIconsFont_compressed_size, 14.f, &icons_config, icons_ranges);
	io.Fonts->Build();

	ImFont* font3 = io.Fonts->AddFontFromMemoryCompressedTTF(binRobotoFont_compressed_data, binRobotoFont_compressed_size, 100.f, &font_config, ranges);
}


#endif // !SOURCE_FONTS_HPP
