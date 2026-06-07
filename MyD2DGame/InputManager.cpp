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
	if (windowId < 0 || !IsValidKeyCode(keyCode))
	{
		return false;
	}

	auto iter = keyStateByWindow.find(windowId);
	if (iter == keyStateByWindow.end())
	{
		return false;
	}

	return iter->second[keyCode];

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