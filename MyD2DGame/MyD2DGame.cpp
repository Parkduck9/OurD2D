#include <Windows.h>
#include "GameWindow.h"
//김성영의 원맨쇼
int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE,
	PWSTR,
	int nCmdShow
)
{
	
	GameWindow window1;
	GameWindow window2;
	GameWindow window3;
	GameWindow window4;

	if (!window1.Create(hInstance, L"Player Field", 0, 540, 1920, 540))
	{
		return 0;
	}
	if (!window2.Create(hInstance, L"Enemy Field", 0, 0, 1920, 540))
	{
		return 0;
	}
	if (!window3.Create(hInstance, L"Player Local", 780, 600, 400, 400))
	{
		return 0;
	}
	if (!window4.Create(hInstance, L"Enemy Local", 780, 50, 400, 400))
	{
		return 0;
	}

	window1.Show(nCmdShow);
	window2.Show(nCmdShow);
	window3.Show(nCmdShow);
	window4.Show(nCmdShow);
	
	window1.ResizeWindowToMonitorRatio(window1.GetHwnd(), 1, 0.5, 0, 0.5, true);
	window2.ResizeWindowToMonitorRatio(window2.GetHwnd(), 1, 0.5, 0, 0, true);
	window3.ResizeWindowToMonitorRatio(window3.GetHwnd(), 0.2, 0.2, 0.5, 0.75, false);
	window4.ResizeWindowToMonitorRatio(window4.GetHwnd(), 0.2, 0.2, 0.5, 0.25, false);

	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}