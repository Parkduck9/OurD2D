#pragma once

#include <Windows.h>
#include <memory>
#include <vector>

#include "GameWindow.h"
#include "OverlayWindow.h" //투명창

class InputManager;

class WindowManager
{
public:
	bool Initialize(HINSTANCE hInstance, InputManager& inputManager);
	int CreateGameWindow(const WindowCreateInfo& info);
	GameWindow* GetWindowById(int id);
	const GameWindow* GetWindowById(int id) const;

	//투명창 관련 함수
	bool CreateOverlayWindow();
	OverlayWindow* GetOverlayWindow();
	const OverlayWindow* GetOverlayWindow() const;
	int GetOverlayRenderTargetId() const;

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

	std::unique_ptr<OverlayWindow> overlayWindow;
	int overlayRenderTargetId = 0;
};