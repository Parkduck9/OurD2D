#pragma once
#include "EngineContext.h"

enum class BattleState
{
	Explore, // 탐색
	Battle, // 배틀 중
	Return // 자리로 돌아가는 중
};

class WindowController
{
public:
	void Initalize(EngineContext& engine);

	// 처음 창 위치 저장
	void SaveStartPositions(int enemyRegionId);

	// 플레이어 창 위치 수정
	void CreatePlayerStartField();
	void CreatePlayerStartRegion();

	// 적 창 위치 수정
	void CreateEnemyStartField();
	void CreateEnemyStartRegion();

	void MovePlayerRegion(float deltatime);
	void ResizeField();


	// 적 전투 돌입 시 enemy field 제거 player field 크기 제어
	void BattleRegion(float deltaTime, int enemyRegionId);
	// Player, Enemy Region Id 얻기
	int GetPlayerRegionId() const { return playerRegionId; }
	int GetEnemyRegionId() const { return enemyRegionId; };
	// 적 전투 종료 시 제자리로 복귀
	bool BattleEndRegion(float deltaTime, int enemyRegionId);

private :
	// BattleRegion, BattleEndRegion 시 동시에 사용하는 부분
	void MoveToward(int wndId, float targetX, float targetY, float speed, float deltaTime);
	
	
private:
	EngineContext* context = nullptr;
	int playerFieldId = -1;
	int playerRegionId = -1;
	int enemyFieldId = -1;
	int enemyRegionId = -1;

	float enemyStartX = 0.0f;
	float enemyStartY = 0.0f;
	float playerStartX = 0.0f;
	float playerStartY = 0.0f;
};