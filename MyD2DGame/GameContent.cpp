#include "GameContent.h"
#include "EngineContext.h"
#include "WindowManager.h"

#include <Windows.h>

void GameContent::OnStart(EngineContext& engine)
{
	player.Initalize(engine);
	enemy.Initalize(engine);

	player.CreatePlayerStartField();
	player.CreatePlayerStartRegion();

	enemy.CreateEnemyStartField();
	enemy.CreateEnemyStartRegion();
}

void GameContent::OnUpdate(EngineContext& engine, float deltaTime)
{
	player.MovePlayerRegion(deltaTime);
	player.ResizeField();
}

void GameContent::OnEnd(EngineContext& engine)
{

}
