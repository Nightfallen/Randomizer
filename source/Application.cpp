﻿#include "Application.h"
#include "source/Fonts.hpp"
#include "fonts/icon_app.hpp"

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

// Forwarding declaration of function 'ImGui_ImplWin32_WndProcHandler'
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Nightfallen
{
	bool Application::CreateDeviceD3D(HWND hWnd)
	{
		// Setup swap chain
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
		//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
			return false;

		CreateRenderTarget();
		return true;
	}

	void Application::CleanupDeviceD3D()
	{
		CleanupRenderTarget();
		if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
		if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
		if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	}

	void Application::CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
		pBackBuffer->Release();
	}

	void Application::CleanupRenderTarget()
	{
		if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
	}

	LRESULT CALLBACK Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		Application* cur_app = reinterpret_cast<Application*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
		if (cur_app)	return cur_app->realWndProc(hWnd, msg, wParam, lParam);
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK Application::realWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		switch (msg)
		{
		case WM_SIZE:
			if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				CleanupRenderTarget();
				g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				CreateRenderTarget();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		case WM_DPICHANGED:
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
			{
				//const int dpi = HIWORD(wParam);
				//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
				const RECT* suggested_rect = (RECT*)lParam;
				::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
			break;
		}
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	void Application::InitializeWindow()
	{
		wc_ = { sizeof(WNDCLASSEXW), CS_CLASSDC, this->WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, wndClassName, NULL };
		wc_.hIcon = this->LoadIcon();
		if (!::RegisterClassExW(&wc_))
		{
			DWORD dwError = ::GetLastError();
			assert(dwError);
		}

		// Size of window is made '{ wnd_wh, wnd_wh }' because imgui's docking feature creates a new window
		int wnd_wh = 50;
		hwnd_ = CreateWindowEx(WS_EX_TOPMOST, wc_.lpszClassName, wndName, WS_POPUP | WS_SYSMENU, 100, 100, wnd_wh, wnd_wh, NULL, NULL, wc_.hInstance, NULL);

		// Make main window's area fully transparent
		SetWindowLong(hwnd_, GWL_EXSTYLE, (int)GetWindowLong(hwnd_, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
		SetLayeredWindowAttributes(hwnd_, RGB(0, 0, 0), 0, LWA_ALPHA);

		// Initialize Direct3D
		if (!CreateDeviceD3D(hwnd_))
		{
			CleanupDeviceD3D();
			::UnregisterClass(wc_.lpszClassName, wc_.hInstance);
			DWORD dwError = ::GetLastError();
			assert(dwError);
		}

		// For 'realWndProc' purposes
		SetWindowLongPtrW(hwnd_, GWLP_USERDATA, (LONG_PTR)this);
		Impl_Win32_EnableGlassEffect(hwnd_);
		this->TaskbarTweak();
		ShowWindow(hwnd_, SW_SHOWDEFAULT);
		::UpdateWindow(hwnd_);
	}

	HICON Application::LoadIcon()
	{
		// Somehow it works: https://stackoverflow.com/a/51806326
		WORD icon_count = 0;
		icon_count = ((WORD*)icon_app_data)[2];
		int offset = 3 * sizeof(WORD) + 16 * icon_count;

		icon_app_size;
		icon_app_data;


		int sz_icon = 128;
		HICON hIcon = NULL;
		if (offset != 0) 
		{
			hIcon = CreateIconFromResourceEx((PBYTE)icon_app_data + offset, icon_app_size - offset, TRUE, 0x30000, sz_icon, sz_icon, LR_DEFAULTCOLOR);
			if (hIcon != NULL) 
				return hIcon;
		}
		return NULL;
	}

	void Application::TaskbarTweak()
	{
		long style = GetWindowLong(hwnd_, GWL_EXSTYLE);
		//style &= ~(WS_VISIBLE);
		style |= WS_EX_TOOLWINDOW;   
		//style &= ~(WS_EX_APPWINDOW);

		//ShowWindow(hwnd_, SW_HIDE);
		SetWindowLong(hwnd_, GWL_EXSTYLE, style);
		ShowWindow(hwnd_, SW_SHOW); // show the window for the new style to come into effect
	}

	void Application::InitializeImgui()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		io.ConfigViewportsNoTaskBarIcon = false;
		io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Min size of window
		style.WindowMinSize = {200, 100};

		// Application handles windows sizing on its own
		ImGui::GetIO().IniFilename = NULL;

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(hwnd_);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

		BuildDefaultFont(io);
	}

	Application::Application()
	{
		InitializeWindow();
		InitializeImgui();
	}

	Application::~Application()
	{
		// Cleanup
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanupDeviceD3D();
		::DestroyWindow(hwnd_);
		::UnregisterClass(wc_.lpszClassName, wc_.hInstance);
	}

	void Application::SetUIHandler(ProcUIHandler handler)
	{
		this->ui_handler = handler;
	}

	void Application::Run()
	{
		if (this->ui_handler == nullptr)
		{
			assert("Ui_Handler is set to 'nullptr'");
		}

		bool done = false;
		ImGuiIO& io = ImGui::GetIO();
		while (!done)
		{
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					done = true;
			}
			if (done)
				break;

			// Start the Dear ImGui frame
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// Our ImGui content
			this->ui_handler(hwnd_);

			// Rendering
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);
			ImGui::Render();
			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
			g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			// Update and Render additional Platform Windows
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
			g_pSwapChain->Present(1, 0); // Present with vsync
		}
	}
}