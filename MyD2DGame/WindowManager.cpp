#include "WindowManager.h"

bool WindowManager::Initialize(HINSTANCE hInstance, InputManager& inputManager)
{
	this->hInstance = hInstance;
	this->inputManager = &inputManager;

	WNDCLASSW wc{};
	wc.lpfnWndProc = WindowManager::WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = className;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	
	if (RegisterClassW(&wc) == 0)
	{
		return false;
	}

	return true;
}

int WindowManager::CreateGameWindow(const WindowCreateInfo& info)
{
	const int newId = nextWindowId++;
	auto window = std::make_unique<GameWindow>();

	if (!window->Create(hInstance, className, newId, info, inputManager))
	{
		return -1;
	}

	windows.push_back(std::move(window));

	return newId;
}

GameWindow* WindowManager::GetWindowById(int id)
{
	for (auto& window : windows)
	{
		if (window->GetID() == id)
		{
			return window.get();
		}
	}

	return nullptr;
}
const GameWindow* WindowManager::GetWindowById(int id) const
{
	for (const auto& window : windows)
	{
		if (window->GetID() == id)
		{
			return window.get();
		}
	}

	return nullptr;
}

bool WindowManager::CreateOverlayWindow()
{
	if (overlayWindow != nullptr)
	{
		return true;
	}

	overlayWindow = std::make_unique<OverlayWindow>();
	return overlayWindow->Create(hInstance);
}
OverlayWindow* WindowManager::GetOverlayWindow()
{
	return overlayWindow.get();
}
const OverlayWindow* WindowManager::GetOverlayWindow() const
{
	return overlayWindow.get();
}
int WindowManager::GetOverlayRenderTargetId() const
{
	return overlayRenderTargetId;
}


LRESULT CALLBACK WindowManager::WindowProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
)
{
	GameWindow* window = nullptr;

	if (message == WM_NCCREATE)
	{
		CREATESTRUCTW* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
		window = reinterpret_cast<GameWindow*>(createStruct->lpCreateParams);

		SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(window)
		);
	}
	else
	{
		window = reinterpret_cast<GameWindow*>(
			GetWindowLongPtrW(hwnd, GWLP_USERDATA)
			);
	}

	if (window != nullptr)
	{
		return window->HandleMessage(hwnd, message, wParam, lParam);
	}
		return DefWindowProcW(hwnd, message, wParam, lParam);
	
}