#pragma once
#include "EngineContext.h"

class WindowController
{
public:
	void Initalize(EngineContext& engine);
	// 플레이어 창 위치 수정
	void CreatePlayerStartField();
	void CreatePlayerStartRegion();

	// 적 창 위치 수정
	void CreateEnemyStartField();
	void CreateEnemyStartRegion();

	void MovePlayerRegion(float deltatime);
	void ResizeField();

private:
	EngineContext* context = nullptr;
	int playerFieldId = -1;
	int playerRegionId = -1;
	int enemyFieldId = -1;
	int enemyRegionId = -1;


	float x = 0;
	float y = 0;
};