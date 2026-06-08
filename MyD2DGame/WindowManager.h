#pragma once

#include <Windows.h>
#include <memory>
#include <vector>

#include "GameWindow.h"

class InputManager;

class WindowManager
{
public:
	bool Initialize(HINSTANCE hInstance, InputManager& inputManager);
	int CreateGameWindow(const WindowCreateInfo& info);
	GameWindow* GetWindowById(int id);

private:
	static LRESULT CALLBACK WindowProc(
		HWND hwnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
	);

	HINSTANCE hInstance = nullptr;
	const wchar_t* className = L"WindowClass";

	InputManager* inputManager = nullptr;

	int nextWindowId = 1;
	std::vector<std::unique_ptr<GameWindow>> windows;
};