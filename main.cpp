#include "source/precompiled.h"
#include "source/Application.h"
#include "source/Interface.hpp"


// Main code

#ifndef _DEBUG
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int main(int, char**)
#endif
{
	using namespace Nightfallen;
	Application* app = new Application();
	app->SetUIHandler(UI_HANDLER);
	app->Run();
	return 0;
}
