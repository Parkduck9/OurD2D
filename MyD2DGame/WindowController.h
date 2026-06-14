#pragma once
#include "EngineContext.h"

class WindowController
{
public:

	// Initialize the WindowController with the engine context
	void Initialize(EngineContext& engine);

	// Start player and enemy positions save
	void SaveStartPositions(int enemyRegionId);

	// Create the player, enemy fields and regions
	void CreatePlayerStartField();
	void CreatePlayerStartRegion();

	void CreateEnemyStartField();
	void CreateEnemyStartRegion();

	// Battle Field functions
	void CreateBattleField(); // create
	void ResizeBattleField(float heightRatio); // resize
	void DestroyBattleField(); // destroy (Not Maked)

	// Battle Field Stat -> 적, 플레이어 region 창 resize
	void ResizeRegionsForBattle();
	// 전투 후 복구하는 창
	void RestoreRegionsFromBattle();


	// Create MovePlayerRegion function
	void MovePlayerRegion(float deltatime);

	// player, enemy battle region move function
	void BattleRegion(float deltaTime, int enemyRegionId);
	int GetPlayerFieldId() const { return playerFieldId; }
	int GetPlayerRegionId() const { return playerRegionId; }
	int GetEnemyRegionId() const { return enemyRegionId; };

	// player,enemy battle region return function
	bool BattleEndRegion(float deltaTime, int enemyRegionId);

	//fieldBoundary resize function
	void ResizePlayerField(float boundary); // Resize player field (down down gogo~)
	void ResizeEnemyField(float boundary); // Resize enemy field (up up gogo~)
	bool IsBattleRegionArrived(int enemyRegionId); // Arrived battle region check function

	// (get Field) 
	int GetBattleFieldId() const { return battleFieldId; }
	
	// Default Field (Enemy field size up, player size down)
	void DefaultFieldSystem(float deltaTime);
	void ClampRegionsToField();

	void BattleFieldSystem(float deltaTime);
	void PushField(float deltaTime);
	void ResizeRegionsForBattleField(float boundary);

private :
	// windowId -> targetX,targetY -> speed Move function (windowId -> wnd -> move)
	void MoveToward(int wndId, float targetX, float targetY, float speed, float deltaTime);


	
protected:

	EngineContext* context = nullptr;
	// engine manager (refernce) pointer

	// id == -1 (not created), id != -1 (created)
	int battleFieldId = -1;
	int playerFieldId = -1;
	int playerRegionId = -1;
	int enemyFieldId = -1;
	int enemyRegionId = -1;


	// region wnd start position(pixel) 
	float enemyStartX = 0.0f;
	float enemyStartY = 0.0f;
	float playerStartX = 0.0f;
	float playerStartY = 0.0f;

	float fieldWidthRatio = 1.007f;
	// field width ratio(0~1) 


	float fieldBoundary = 0.50f;  // 경계선 Y 비율 (0~1), 이 하나로 두 필드 크기 결정
	float fixedFieldTime = 0.0f; // Field size renewal timer

};