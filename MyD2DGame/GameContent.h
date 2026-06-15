#pragma once
#include "Content.h" // Content Interface
#include "WindowController.h" // WindowController/transform/Size ex.. Controller

#include "Actor.h" // Actor(Character, Object)
#include "BattleState.h" // Battle State Enum
#include "SpriteAnimation.h" // Sprite Animation 
#include "SpawnButtonManager.h"

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

	int mainWindowId = -1; // Main Render (Now Don't Use)
	WindowController player; // 플레이어 컨트롤러(창) 객체
	WindowController enemy; // 적 컨트롤러(창) 객체


	BattleState state = BattleState::Explore; // 배틀 상태


	
	float battleRegionHeight = 0.15f; //Battle region height ratio (0~1)
	float battleExpandT = 0.0f; // Battle field expansion timer (0~1)
	// Battle Field height interpolation use
	float battleExpandSpeed = 1.0f; // battle field expansion speed
	//1.0f second -> (0~1) 
	bool battleFieldCreated = false; // battle field created flag(true,false)

	//오디오, 비트맵 임시 변수(삭제예정)
	float a = 0.1f, b = 0.1f;

	//투명윈도우ID
	int overlayRenderTargetId = 0;

	// 플레이어 리소스 움직이기
	void MovePlayerActor(EngineContext& engine, float deltaTime);

	// Battle 종료 시 돌아올 위치 (마우가) 
	float battleStartX = 0.0f;
	float battleStartY = 0.0f;

	void PlayerHitSound();
	
	//콜라이더 표시변수
	bool showCollider = false;

	//버튼 스폰 매니저
	SpawnButtonManager spawnButtonManager;

	//액터 임시 변수(삭제 예정)
	std::vector<std::unique_ptr<Actor>> actors;
	Actor* playerActor = nullptr;


};