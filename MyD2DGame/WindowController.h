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

	// Battle Field Stat -> ��, �÷��̾� region â resize
	void ResizeRegionsForBattle();
	// ���� �� �����ϴ� â
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
	int GetEnemyFieldId() const { return enemyFieldId; }

	// Default Field (Enemy field size up, player size down)
	void DefaultFieldSystem(float deltaTime);
	void ClampRegionsToField();
	void ResetEnemyRegionClamp() { enemyRegionInitialY = -1.0f; }

	void BattleFieldSystem(float deltaTime);
	void PushField(float deltaTime);
	void ResizeRegionsForBattleField(float boundary);

	// �ֿ� �¾������ ü�� �ٰ� �ϱ�
	void ApplyFieldPenalty(float amount);
	void ApplyFieldPenaltyOnly(float amount); // region 위치 변경 없이 field만 조정
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

	
	float fieldBoundary = 0.50f;  // ��輱 Y ���� (0~1), �� �ϳ��� �� �ʵ� ũ�� ����

	float enemyRegionInitialY = -1.0f; // explore 시작 시 enemy region의 초기 Y (픽셀), -1=초기화 안됨

};