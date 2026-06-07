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
			100, 100,
			500, 400
		}
	);

	mainWindowId = mainWindow;
	
	
	
}
void GameContent::OnUpdate(EngineContext& engine, float deltaTime)
{
	auto& input = engine.GetInputManager();
	auto& windows = engine.GetWindowManager();

	if (input.IsKeyDown(mainWindowId, VK_LEFT))
	{
		int mainWindow = windows.CreateGameWindow(
			{
				L"Add Window",
				200, 1000,
				500, 400
			}
		);
	}
}

void GameContent::OnEnd(EngineContext& engine)
{

}