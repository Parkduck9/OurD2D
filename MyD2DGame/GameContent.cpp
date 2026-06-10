#include "GameContent.h"
#include "EngineContext.h"
#include "WindowManager.h"
#include "InputManager.h"  

#include <Windows.h>

void GameContent::OnStart(EngineContext& engine)
{
	player.Initalize(engine);
	enemy.Initalize(engine);
	// 플레이어와 적 객체 생성

	player.CreatePlayerStartField();
	player.CreatePlayerStartRegion();
	// 플레이어 지역, 필드 창 생성

	enemy.CreateEnemyStartField();
	enemy.CreateEnemyStartRegion();
	// 적 지역, 필드 창 생성

	player.SaveStartPositions(enemy.GetEnemyRegionId()); 
	// 시작 위치 저장

}

void GameContent::OnUpdate(EngineContext& engine, float deltaTime)
{
	auto& input = engine.GetInputManager();

	//테스트용 


	switch (state) {
	case BattleState::Explore:
		player.MovePlayerRegion(deltaTime);

		if (input.IsKeyPressed(player.GetPlayerRegionId(), VK_RETURN))
		{
			state = BattleState::Battle;
		}

		break;

	case BattleState::Battle:
		player.BattleRegion(deltaTime, enemy.GetEnemyRegionId());
		if (input.IsKeyPressed(player.GetPlayerRegionId(), VK_BACK))
		{
			state = BattleState::Return;
		}
		break;

	case BattleState::Return:
		if (player.BattleEndRegion(deltaTime, enemy.GetEnemyRegionId()))
		{
			state = BattleState::Explore;
		}
		break;

		//player.ResizeField();
	}
}

void GameContent::OnEnd(EngineContext& engine)
{

}
