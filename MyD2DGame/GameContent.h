#pragma once
#include "Content.h" // Content Interface
#include "WindowController.h" // WindowController/transform/Size ex.. Controller

#include "Actor.h" // Actor(Character, Object)
#include "BattleState.h" // Battle State Enum
#include "SpriteAnimation.h" // Sprite Animation 
#include "SpawnButtonManager.h"
#include "AudioManager.h"
#include "Content/SceneManager.h"

#include <d2d1.h> //D2D
#include<wrl/client.h> // Comptr
#include<vector>
#include<memory> // unique_ptr


class GameContent : public Content
{
public:
	void OnStart(EngineContext& engine) override;
	void OnUpdate(EngineContext& engine, float deltaTime) override;
	void OnRender(EngineContext& engine) override;
	void OnEnd(EngineContext& engine) override;


private:
	struct EnemyOrange // 도로롱 귤 구조체
	{
		std::unique_ptr<Actor> actor;

		float startX = 0.0f; // 귤 시작 위치
		float startY = 0.0f;
		float targetX = 0.0f; // 귤 떨어질 위치
		float targetY = 0.0f;

		float elapsed = 0.0f; //날아간 시간
		float duration = 1.2f; // 총 비행 시간

		float arcHeight = 180.0f; // 포물선 높이
		bool falling = false;
		float fallSpeed = 400.0f;
		float velX = 0.0f; // 포물선 종료 시점 속도
		float velY = 0.0f;
		bool hasHitPlayer = false; // 맞았느지 체크용
	};
	std::vector<EnemyOrange> oranges;
	void UpdateEnemyOranges(EngineContext& engine, float deltaTime);

	float enemyAttackTimer = 0.0f;
	float enemyAttackInterval = 1.5f;

	float battleOrangeTimer = 0.0f;
	static constexpr float battleOrangeInterval = 2.5f; // Battle 귤 간격
	static constexpr float battleOrangeDamage   = 0.06f; // Battle 귤 데미지
	void SpawnEnemyOrange(EngineContext& engine);

	struct PlayerSpear // 플레이어 창 구조체
	{
		std::unique_ptr<Actor> actor;
		float speed = 300.0f;
		bool hasHitEnemyField = false;
	};
	std::vector<PlayerSpear> spears;
	void UpdatePlayerSpears(EngineContext& engine, float deltaTime);
	void SpawnPlayerSpear(EngineContext& engine);
	float spearCooldown = 0.0f;
	// 

	int mainWindowId = -1; // Main Render (Now Don't Use)
	WindowController player; // 플레이어 컨트롤러(창) 객체
	WindowController enemy; // 적 컨트롤러(창) 객체


	BattleState state = BattleState::Start; // 배틀 상태

	void RestartToStart(EngineContext& engine);


	float battleExpandT = 0.0f; // Battle field expansion timer (0~1)
	float battleTimer = 0.0f;  // 배틀 제한 시간 (초)
	// Battle Field height interpolation use
	float battleExpandSpeed = 1.0f; // battle field expansion speed
	//1.0f second -> (0~1) 
	bool battleFieldCreated = false; // battle field created flag(true,false)

	//오디오, 비트맵 임시 변수(삭제예정)
	float a = 0.1f, b = 0.1f;

	//투명윈도우ID
	int overlayRenderTargetId = 0;

	// 플레이어 리소스 움직이기
	void MovePlayerActor(EngineContext& engine, float deltaTime, float moveSpeed);

	// Battle 종료 시 돌아올 위치 (마우가) 
	float battleStartX = 0.0f;
	float battleStartY = 0.0f;

	void PlayerHitSound();
	
	//콜라이더 표시변수
	bool showCollider = false;
	bool isMoving = false;

	//버튼 스폰 매니저
	SpawnButtonManager spawnButtonManager;

	//사운드 매니저
	AudioManager audiomanger;

	//액터 임시 변수(삭제 예정)
	std::vector<std::unique_ptr<Actor>> actors;
	Actor* playerActor = nullptr;
	Actor* playerActorRun = nullptr;
	void MoveEnemyActor(EngineContext& engine, float deltaTime);
	float enemyBattleStartX = 0.0f;
	float enemyBattleStartY = 0.0f;
	Actor* enemyActor = nullptr;
	Actor* enemyActorWalk = nullptr;
	void CenterEnemyActor(EngineContext& engine, float deltaTime);
	void UpdateEnemyExplore(EngineContext& engine, float deltaTime);
	float prevEnemyClientY = -1.0f;
	float prevEnemyClientX = -1.0f;

	float enemyRegionVelX = 0.0f; // 적 region 이동 속도 X (픽셀/초)
	float enemyRegionVelY = 0.0f; // 적 region 이동 속도 Y (픽셀/초)

	float prevPlayerOverlayX = -1.0f; // 이전 프레임 플레이어 오버레이 X
	float prevPlayerOverlayY = -1.0f; // 이전 프레임 플레이어 오버레이 Y
	float playerVelX = 0.0f; // 플레이어 이동 속도 추정 X
	float playerVelY = 0.0f; // 플레이어 이동 속도 추정 Y

	SceneManager sceneManager;
	bool endingStarted = false;


	// 멤버 변수
	float returnRegionT = 1.0f;
	float returnFieldT = 1.0f;
	bool regionShrinkFinished = false;

	// Battle 승패 플래그 (Return 애니메이션 후 처리)
	bool playerFieldLost = false;
	bool enemyFieldLost = false;

	// 도로롱 도로 운전 (battle 전용)
	struct DrivingCar
	{
		std::unique_ptr<Actor> actor;
		bool active = false;
		float speed = 1600.0f;
	};
	struct DrivingWarning
	{
		std::unique_ptr<Actor> actor; // 도로롱위험위험해 스프라이트
		bool  active = false;
		float timer = 0.0f;          // 차 등장까지 남은 시간
		float carSpawnX = 0.0f;      // 차가 나올 overlay X
		D2D1_RECT_F warnRect = {};   // 경고 사각형 (overlay 좌표)
	};

	DrivingCar    drivingCar;
	DrivingWarning drivingWarning;
	float battleElapsed   = 0.0f;
	int   driveSpawnIndex = 0;
	float battlePlayerRegionY = 0.0f; // Battle 진입 시 player region top Y (overlay)

	static constexpr float carW = 130.0f;
	static constexpr float carH = 100.0f;
	static constexpr float warnH = 80.0f;

	void SpawnDrivingWarning(EngineContext& engine);
	void SpawnDrivingCar(EngineContext& engine);
	void UpdateDrivingCar(EngineContext& engine, float deltaTime);
	void ClearDrivingObjects()
	{
		drivingCar.active = false;    drivingCar.actor    = nullptr;
		drivingWarning.active = false; drivingWarning.actor = nullptr;
	}



};
