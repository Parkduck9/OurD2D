#pragma once


class WindowManager;
class InputManager;

class EngineContext
{
public:
	EngineContext(WindowManager& windowManager, InputManager& inputManager);

	WindowManager& GetWindowManager();
	InputManager& GetInputManager();

private:
	WindowManager& windowManager;
	InputManager& inputManager;
};