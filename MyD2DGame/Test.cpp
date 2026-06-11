#include "Test.h"
#include "EngineContext.h"
#include "WindowManager.h"

#include "InputManager.h"
#include "WicManager.h"
#include "D2DManager.h"

#include <Windows.h>

//임시 파일//
#include<mmsystem.h>
#pragma comment(lib, "winmm.lib")
///////////


void GameContent::OnStart(EngineContext& engine)
{
	player.Initalize(engine);
	enemy.Initalize(engine);

	// 플레이어와 적 객체 생성

	auto& windows = engine.GetWindowManager();
	auto& d2d = engine.GetD2DManager();



	Microsoft::WRL::ComPtr<IWICBitmapSource> source;


	enemy.CreateEnemyStartField();
	player.CreatePlayerStartField();
	// 적, 플레이어 필드 우선 생성 (아니면 창 아래로 들어가서 문제 생김)

	enemy.CreateEnemyStartRegion();
	player.CreatePlayerStartRegion();
	// 플레이어, 적 지역 우선 생성 ( 제일 위로 올리기 위해서 )

	player.SaveStartPositions(enemy.GetEnemyRegionId());
	// 시작 위치 저장
	mainWindowId = player.GetPlayerRegionId();

	auto* window = engine.GetWindowManager().GetWindowById(mainWindowId);
	engine.GetD2DManager().CreateRenderTargetForWindow(
		mainWindowId,
		window->GetHwnd()
	);


	auto actorA = std::make_unique<Actor>(mainWindowId);
	auto actorB = std::make_unique<Actor>(mainWindowId);

	actorA->InitializeSprite(engine, L"../Resource/알아.png", 0.0f, 0.0f, 100.0f, 100.0f);
	actorB->InitializeSprite(engine, L"../Resource/디바.png", 50.0f, 50.0f, 100.0f, 100.0f);

	playerActor = actorA.get();
	Actor* enemyActor = actorB.get();

	actors.push_back(std::move(actorA));
	actors.push_back(std::move(actorB));




}

void GameContent::OnUpdate(EngineContext& engine, float deltaTime)
{
	auto& input = engine.GetInputManager();

	//테스트용 
	Actor* actorA = actors[0].get();
	Actor* actorB = actors[1].get();

	switch (state) {
	case BattleState::Explore:
		player.MovePlayerRegion(deltaTime);

		if (input.IsKeyPressed(player.GetPlayerRegionId(), VK_RETURN))
		{
			state = BattleState::Battle;
		}

		break;

	case BattleState::Battle:
		player.BattleRegion(deltaTime, enemy.GetEnemyRegionId());
		if (input.IsKeyPressed(player.GetPlayerRegionId(), VK_BACK))
		{
			state = BattleState::Return;
		}
		break;

	case BattleState::Return:
		if (player.BattleEndRegion(deltaTime, enemy.GetEnemyRegionId()))
		{
			state = BattleState::Explore;
		}
		break;

		//player.ResizeField();
	}

	/*
	if (input.IsKeyDown(mainWindowId, VK_RIGHT))
	{
		//windows.GetWindowById(mainWindowId)->MoveWindow(0.2, 0,3,deltaTime);
		playerActor->Move(5, 0);
		//PlayerHitSound();
		playerActor->PlayAnimation(L"walk");
	}
	if (input.IsKeyDown(mainWindowId, VK_LEFT))
	{
		playerActor->Move(-5, 0);
		//windows.GetWindowById(mainWindowId)->MoveWindow(-0.2, 0, 3, deltaTime);
		playerActor->PlayAnimation(L"walk");
	}
	if (input.IsKeyDown(mainWindowId, VK_UP))
	{
		playerActor->Move(0, -5);
		//windows.GetWindowById(mainWindowId)->MoveWindow(0, -0.2, 3, deltaTime);
		playerActor->PlayAnimation(L"idle");
	}
	if (input.IsKeyDown(mainWindowId, VK_DOWN))
	{
		playerActor->Move(0, 5);
		//windows.GetWindowById(mainWindowId)->MoveWindow(0, 0.2, 3, deltaTime);
		playerActor->PlayAnimation(L"idle");
	}
	*/
	bool isCollision = actorA->GetBoxCollider().Intersects(*actorA, *actorB);
	if (isCollision)
	{
		OutputDebugStringW(L"Collision!\n");
	}
	else
	{
		OutputDebugStringW(L"NoCollision~~~\n");
	}

	for (auto& actor : actors)
	{
		actor->Update(deltaTime);
	}
}

void GameContent::OnRender(EngineContext& engine)
{
	if (mainWindowId == -1)
	{
		return;
	}

	auto& d2d = engine.GetD2DManager();

	d2d.BeginDraw(mainWindowId);

	d2d.Clear(mainWindowId, D2D1::ColorF(D2D1::ColorF::White));

	//d2d.DrawBitmap(mainWindowId, testBitmap.Get(), D2D1::RectF(0.0f, 0.0f, 800.0f, 500.0f));

	//D2D1_RECT_F sourceRect = playerAnimation.GetSourceRect();

	//D2D1_RECT_F destRect = D2D1::RectF(
	//	playerX,
	//	playerY,
	//	playerX + 128.0f,
	//	playerY + 128.0f
	//);

	//d2d.DrawBitmapFrame(
	//	mainWindowId,
	//	testBitmap.Get(),
	//	destRect,
	//	sourceRect
	//);

	//액터 렌더는 여기서
	for (auto& actor : actors)
	{
		if (actor->GetWindowId() == mainWindowId)
		{
			actor->Render(d2d);
		}
	}

	HRESULT hr = d2d.EndDraw(mainWindowId);

	if (hr == D2DERR_RECREATE_TARGET)
	{
		//액터 리셋은 여기서
		for (auto& actor : actors)
		{
			if (actor->GetWindowId() == mainWindowId)
			{
				actor->ResetBitmap();
			}
		}
	}



}

void GameContent::OnEnd(EngineContext& engine)
{
	actors.clear();
	playerActor = nullptr;
}

void GameContent::PlayerHitSound()
{
	PlaySound(L"../Resource/Shock The World.wav", nullptr, SND_FILENAME | SND_ASYNC);
}