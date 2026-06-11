#pragma once
#include "EngineContext.h"

class WindowController
{
public:

	// Initialize the WindowController with the engine context
	void Initialize(EngineContext& engine);

	void SaveStartPositions(int enemyRegionId);

	// Create the player, enemy fields and regions
	void CreatePlayerStartField();
	void CreatePlayerStartRegion();

	void CreateEnemyStartField();
	void CreateEnemyStartRegion();

	// Create MovePlayerRegion function
	void MovePlayerRegion(float deltatime);




	void BattleRegion(float deltaTime, int enemyRegionId);

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