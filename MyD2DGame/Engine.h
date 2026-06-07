#pragma once
#include <Windows.h>
#include <memory>

#include "WindowManager.h"
#include "InputManager.h"


#include "EngineContext.h"
#include "Content.h"


class Engine
{
public:
	bool Initialize (HINSTANCE hInstance);
	void Run	    ();
	void Shutdown	();

private:
	void Update		(float deltaTime);
	void Render		();



private:
	bool	  isRunning  = true;

	WindowManager windowManager;
	InputManager  inputManager;
	std::unique_ptr<EngineContext> context;
	std::unique_ptr<Content> content;
};