#pragma once
#include "Content.h"

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
	Microsoft::WRL::ComPtr<ID2D1Bitmap> testBitmap;

	//오디오, 비트맵 임시 변수(삭제예정)
	float a = 0.1f, b = 0.1f;

	SpriteAnimation playerAnimation;

	float playerX = 100.0f;
	float playerY = 100.0f;
	
	void PlayerHitSound();

	//액터 임시 변수(삭제 예정)
	std::vector<std::unique_ptr<Actor>> actors;
	Actor* playerActor = nullptr;

};