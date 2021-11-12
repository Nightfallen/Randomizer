#ifndef SOUCRE_APPLICATION_H
#define SOUCRE_APPLICATION_H

#include "source/precompiled.h"
#include "source/structures.hpp"
#include "source/Freetype.hpp"

namespace Nightfallen 
{
	constexpr auto wndClassName = L"Randomizer App Class";
	constexpr auto wndName = L"Randomizer";
	class Application 
	{
		WNDCLASSEXW wc_									= {};
		HWND hwnd_										= {};
		ID3D11Device* g_pd3dDevice						= NULL;
		ID3D11DeviceContext* g_pd3dDeviceContext		= NULL;
		IDXGISwapChain* g_pSwapChain					= NULL;
		ID3D11RenderTargetView* g_mainRenderTargetView	= NULL;
		ProcUIHandler ui_handler						= NULL;
		FreetypeTest freetype;

		bool CreateDeviceD3D(HWND hWnd);
		void CleanupDeviceD3D();
		void CreateRenderTarget();
		void CleanupRenderTarget();

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK realWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void InitializeWindow();
		void InitializeImgui();

		HICON LoadIcon();

		// A little hack function to hide main winapi window in taskbar
		// and leave there only imgui's docking windows
		void TaskbarTweak();
	public:
		Application();
		~Application();

		void SetUIHandler(ProcUIHandler handler);
		void Run();
	};
}

#endif // !SOUCRE_APPLICATION_H 