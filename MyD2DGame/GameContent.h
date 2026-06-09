#pragma once
#include "Content.h"
#include "SpriteAnimation.h"
#include <d2d1.h>
#include<wrl/client.h>


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

	//임시파일(삭제예정)
	float a = 0.1f, b = 0.1f;
	void PlayerHitSound()
	{
		PlaySound(L"../Resource/Shock The World.wav", nullptr, SND_FILENAME | SND_ASYNC);
	}
	SpriteAnimation playerAnimation;

	float playerX = 100.0f;
	float playerY = 100.0f;

};