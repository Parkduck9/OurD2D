#include "GameContent.h"
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
	player.Initialize(engine);
	enemy.Initialize(engine);
	// 플레이어와 적 객체 생성 (refrence)

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
	// 시작 위치 저장 (return to end battle)

}

void GameContent::OnUpdate(EngineContext& engine, float deltaTime)
{
	auto& input = engine.GetInputManager();


	switch (state) {
	case BattleState::Explore:
		// playerMove for key
		player.MovePlayerRegion(deltaTime);


		fixedFieldTime += deltaTime;

		//0.1second -> fieldBoundary update -> Enemy field resize up, Player field resize down
		if (fixedFieldTime >= 0.1f)
		{
			fieldBoundary += 0.001f;
			fieldBoundary = max(0.0f, min(1.0f, fieldBoundary));
			player.ResizePlayerField(fieldBoundary);
			enemy.ResizeEnemyField(fieldBoundary);
			fixedFieldTime = 0.0f;
		}

		// Enter key -> Move to Battle
		if (input.IsKeyPressed(player.GetPlayerRegionId(), VK_RETURN))
		{
			state = BattleState::MoveToBattle;
		}

		break;

	case BattleState::MoveToBattle:
		// player, enemy move to battle region
		player.BattleRegion(deltaTime, enemy.GetEnemyRegionId());
		// battle region arrived -> Change Expand Battle (BattleState)
		if (player.IsBattleRegionArrived(enemy.GetEnemyRegionId()))
		{
			battleExpandT = 0.0f;
			battleFieldCreated = false;
			state = BattleState::ExpandBattle;
		}
		break;

	case BattleState::ExpandBattle:
	{
		// just one create battle field
		if (!battleFieldCreated)
		{
			player.CreateBattleField();
			battleFieldCreated = true;
		}
		// battleExpandT update 0->1 (battle field height ratio resize)
		battleExpandT += battleExpandSpeed * deltaTime;

		if (battleExpandT > 1.0f)
		{
			battleExpandT = 1.0f;
		}

		float startHeight = 0.01f;
		float endHeight = 0.75f;
		float heightRatio =
			startHeight + (endHeight - startHeight) * battleExpandT;

		player.ResizeBattleField(heightRatio);

		// if battleExpandT arrived 1.0f -> Change BattleState to Battle
		if (battleExpandT >= 1.0f)
		{
			state = BattleState::Battle;
		}

		break;
	}
	case BattleState::Battle:
		// BackSpace key use -> Return BattleExpandT (rewind) (BattleState)
		if (input.IsKeyPressed(player.GetPlayerRegionId(), VK_BACK))
		{
			battleExpandT = 1.0f;
			state = BattleState::Return;
		}
		break;

	case BattleState::Return:
	{
		battleExpandT -= battleExpandSpeed * deltaTime;
		// battleExpandT 1-> 0 ( battle field  height resize for small)
		if (battleExpandT < 0.0f)
		{
			battleExpandT = 0.0f;
		}

		float startHeight = 0.01f;
		float endHeight = 0.75;

		float heightRatio =
			startHeight + (endHeight - startHeight) * battleExpandT;

		player.ResizeBattleField(heightRatio);
		// resize clear -> delete battle field -> State change Return Explore
		if (battleExpandT <= 0.0f)
		{
			player.DestroyBattleField();
			battleFieldCreated = false;
			state = BattleState::ReturnExplore;
		}

		break;
	

	}

	case BattleState::ReturnExplore:
		//player,enemy region return start position
		if (player.BattleEndRegion(deltaTime, enemy.GetEnemyRegionId()))
		{
			battleExpandT = 0.0f;
			state = BattleState::Explore; // change Explore(State)
		}
		break;

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



	}
	// actor update ( maybe delete ? )
	for (auto& actor : actors)
	{
		actor->Update(deltaTime);
	}
}

void GameContent::OnRender(EngineContext& engine)
{
	// not mainWindowId -> skip
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