#pragma once
#include <array>
#include<unordered_map>

class InputManager
{
public:
	void SetKeyDown(int windowId, int keyCode, bool isDown);
	bool IsKeyDown(int windowId, int keyCode) const;

	void SetFocusedWindowId(int windowId);
	int GetFocusedWindowId() const;

private:
	bool IsValidKeyCode(int keyCode) const;

	int focusedWindowId = -1;
	std::unordered_map<int, std::array<bool, 256>> keyStateByWindow;
};