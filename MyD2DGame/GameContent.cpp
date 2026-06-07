#include "GameContent.h"

#include "EngineContext.h"
#include "WindowManager.h"
#include "InputManager.h"

#include <Windows.h>

void GameContent::OnStart(EngineContext& engine)
{
	auto& windows = engine.GetWindowManager();

	int mainWindow = windows.CreateGameWindow(
		{
			L"Main Window",
			0.5, 0.5,
			0.5, 0.5
		}
	);

	mainWindowId = mainWindow;
	
	
	
}
void GameContent::OnUpdate(EngineContext& engine, float deltaTime)
{
	auto& input = engine.GetInputManager();
	auto& windows = engine.GetWindowManager();

	if (input.IsKeyPressed(mainWindowId, VK_LEFT))
	{
		int mainWindow = windows.CreateGameWindow(
			{
				L"Add Window",
				200, 1000,
				500, 400
			}
		);
	}
	if (input.IsKeyDown(mainWindowId, VK_RIGHT))
	{
		windows.GetWindowById(mainWindowId)->MoveWindow(10, 10);
	}
}

void GameContent::OnEnd(EngineContext& engine)
{

}
