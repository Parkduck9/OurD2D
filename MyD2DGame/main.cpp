#include "Engine.h"
#include <Windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	Engine BaseEngine;

	if (!BaseEngine.Initialize(hInstance))
	{
		MessageBoxW(nullptr, L"엔진 시동 실패!", L"에러!", MB_OK);
		return -1;
	}

	BaseEngine.Run();
	BaseEngine.Shutdown();

	return 0;
}