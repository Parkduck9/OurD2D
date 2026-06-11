#pragma once
#include "EngineContext.h"

enum class BattleState
{
	Explore, // Ž��
	Battle, // ��Ʋ ��
	Return // �ڸ��� ���ư��� ��
};

class WindowController
{
public:
	void Initalize(EngineContext& engine);

	void SaveStartPositions(int enemyRegionId);

	void CreatePlayerStartField();
	void CreatePlayerStartRegion();

	void CreateEnemyStartField();
	void CreateEnemyStartRegion();

	void MovePlayerRegion(float deltatime);




	void BattleRegion(float deltaTime, int enemyRegionId);
	// Player, Enemy Region Id ���
	int GetPlayerRegionId() const { return playerRegionId; }
	int GetEnemyRegionId() const { return enemyRegionId; };

	bool BattleEndRegion(float deltaTime, int enemyRegionId);


	void ResizePlayerField(float boundary);
	void ResizeEnemyField(float boundary);

private :

	void MoveToward(int wndId, float targetX, float targetY, float speed, float deltaTime);
	
	
protected:
	EngineContext* context = nullptr;
	int playerFieldId = -1;
	int playerRegionId = -1;
	int enemyFieldId = -1;
	int enemyRegionId = -1;


	float enemyStartX = 0.0f;
	float enemyStartY = 0.0f;
	float playerStartX = 0.0f;
	float playerStartY = 0.0f;

	float fieldWidthRatio = 1.007f;

};