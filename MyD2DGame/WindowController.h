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

	// ó�� â ��ġ ����
	void SaveStartPositions(int enemyRegionId);

	// �÷��̾� â ��ġ ����
	void CreatePlayerStartField();
	void CreatePlayerStartRegion();

	// �� â ��ġ ����
	void CreateEnemyStartField();
	void CreateEnemyStartRegion();

	void MovePlayerRegion(float deltatime);




	// �� ���� ���� �� enemy field ���� player field ũ�� ����
	void BattleRegion(float deltaTime, int enemyRegionId);
	// Player, Enemy Region Id ���
	int GetPlayerRegionId() const { return playerRegionId; }
	int GetEnemyRegionId() const { return enemyRegionId; };
	// �� ���� ���� �� ���ڸ��� ����
	bool BattleEndRegion(float deltaTime, int enemyRegionId);

	// �⺻ field �ý���
	void ResizePlayerField(float boundary);
	void ResizeEnemyField(float boundary);

private :
	// BattleRegion, BattleEndRegion �� ���ÿ� ����ϴ� �κ�
	void MoveToward(int wndId, float targetX, float targetY, float speed, float deltaTime);
	
	
protected:
	EngineContext* context = nullptr;
	int playerFieldId = -1;
	int playerRegionId = -1;
	int enemyFieldId = -1;
	int enemyRegionId = -1;

	// ���� ���� �� ù region ��ġ�� ����ϱ� ���� �Լ�
	float enemyStartX = 0.0f;
	float enemyStartY = 0.0f;
	float playerStartX = 0.0f;
	float playerStartY = 0.0f;

	float fieldWidthRatio = 1.007f;

};