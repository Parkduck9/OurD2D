#pragma once
#include "Content.h"
#include "WindowController.h"

#include "Actor.h"

#include "SpriteAnimation.h"
#include <d2d1.h>
#include<wrl/client.h>
#include<vector>
#include<memory>


class GameContent : public Content
{
public:
	void OnStart(EngineContext& engine) override;
	void OnUpdate(EngineContext& engine, float deltaTime) override;
	void OnRender(EngineContext& engine) override;
	void OnEnd(EngineContext& engine) override;


private:
	int mainWindowId = -1;

	WindowController player; // 플레이어 컨트롤러(창) 객체
	WindowController enemy; // 적 컨트롤러(창) 객체
	BattleState state = BattleState::Explore; // 배틀 상태



	//오디오, 비트맵 임시 변수(삭제예정)
	float a = 0.1f, b = 0.1f;

	
	void PlayerHitSound();
	
	//액터 임시 변수(삭제 예정)
	std::vector<std::unique_ptr<Actor>> actors;
	Actor* playerActor = nullptr;

	D2D1_COLOR_F Winbk = D2D1::ColorF(D2D1::ColorF::White);
};