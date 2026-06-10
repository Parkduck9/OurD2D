#pragma once

class WicManager;
class D2DManager;
class WindowManager;
class InputManager;

class EngineContext
{
public:
	EngineContext(WindowManager& windowManager, InputManager& inputManager,
				  WicManager& wicManager, D2DManager& d2dManager);

	WicManager& GetWicManager();
	D2DManager& GetD2DManager();
	WindowManager& GetWindowManager();
	InputManager& GetInputManager();

private:
	WindowManager& windowManager;
	InputManager& inputManager;
	WicManager& wicManager;
	D2DManager& d2dManager;

};