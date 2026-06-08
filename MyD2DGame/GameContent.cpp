#include "GameContent.h"

#include "EngineContext.h"
#include "WindowManager.h"
#include "InputManager.h"

#include <Windows.h>

void GameContent::OnStart(EngineContext& engine)
{
	auto& windows = engine.GetWindowManager();

	int plyaerField = windows.CreateGameWindow( // 플레이어 필드
		{
			L"Main Window",
			0.5, 0.75, // 위치 좌표 
			1.0, 0.5  // 스케일
		}
	);
	int playerregion = windows.CreateGameWindow( // 플레이어 지역
		{
			L"Main Window",
			0.5, 0.8,
			0.1, 0.15
		}
	);

	int EnemyField = windows.CreateGameWindow( // 적 필드
		{
			L"Main Window",
			0.5, 0.25,
			1.0, 0.5
		}
	);

	int Enemyregion = windows.CreateGameWindow( // 적 지역
		{
			L"Main Window",
		}
	);

	mainWindowId = plyaerField;
	mainWindowId = EnemyField;
	mainWindowId = Enemyregion;
	mainWindowId = playerregion;
	
	
	
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
				a, b,
				0.2, 0.2
			}
		);
		a += 0.01; b += 0.01;
	}
	if (input.IsKeyDown(mainWindowId, VK_RIGHT))
	{
		windows.GetWindowById(mainWindowId)->MoveWindow(0.2, 0,1.3,deltaTime);
	}
	if (input.IsKeyPressed(mainWindowId, VK_SPACE))
	{
		windows.GetWindowById(mainWindowId)->MoveWindow(0, -40.0, 1 , deltaTime);
	}
	if (input.IsKeyPressed(mainWindowId, VK_SHIFT))
	{
		windows.GetWindowById(mainWindowId)->ReSizeWindow(0.1, 0.1);
	}
	if (input.IsKeyPressed(mainWindowId, VK_TAB))
	{
		windows.GetWindowById(mainWindowId)->ReSizeWindow(0.8, 0.8);
	}
}

void GameContent::OnEnd(EngineContext& engine)
{

}
