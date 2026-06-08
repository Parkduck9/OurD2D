#include "InputManager.h"

void InputManager::SetKeyDown(int windowId, int keyCode, bool isDown)
{
	if (windowId < 0 || !IsValidKeyCode(keyCode))
	{
		return;
	}

	keyStateByWindow[windowId][keyCode] = isDown;
}

bool InputManager::IsKeyDown(int windowId, int keyCode) const
{
	return GetKeyState(keyStateByWindow, windowId, keyCode);
}

bool InputManager::IsKeyPressed(int windowId, int keyCode) const
{
	return GetKeyState(keyStateByWindow, windowId, keyCode) &&
		!GetKeyState(previousKeyStateByWindow, windowId, keyCode);
}

void InputManager::EndFrame()
{
	previousKeyStateByWindow = keyStateByWindow;
}

void InputManager::SetFocusedWindowId(int windowId)
{
	focusedWindowId = windowId;
}

int InputManager::GetFocusedWindowId() const
{
	return focusedWindowId;
}

bool InputManager::IsValidKeyCode(int keyCode) const
{
	return keyCode >= 0 && keyCode < 256;
}

bool InputManager::GetKeyState(
	const std::unordered_map<int, std::array<bool, 256>>& states,
	int windowId,
	int keyCode
) const
{
	if (windowId < 0 || !IsValidKeyCode(keyCode))
	{
		return false;
	}

	auto iter = states.find(windowId);
	if (iter == states.end())
	{
		return false;
	}

	return iter->second[keyCode];
}
