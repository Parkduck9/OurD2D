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

	// Render Target -> after Region Wnd Create 
	auto* playerWnd = windows.GetWindowById(player.GetPlayerRegionId());
	auto* enemyWnd = windows.GetWindowById(enemy.GetEnemyRegionId());

	d2d.CreateRenderTargetForWindow(player.GetPlayerRegionId(), playerWnd->GetHwnd());
	d2d.CreateRenderTargetForWindow(enemy.GetEnemyRegionId(), enemyWnd->GetHwnd());

	auto enemyActor = std::make_unique<Actor>(enemy.GetEnemyRegionId());
	enemyActor->InitializeSprite(engine, L"../Resource/도로롱.png",
		40.0f, 0.0f, 100.0f, 100.0f);

	enemyActor->AddAnimation(L"idle", 200, 200, 60, 8, 8.0f);
	enemyActor->PlayAnimation(L"idle");

	enemyActor->AddAnimation(L"walk", 176, 176, 8, 8, 8.0f);

	// 플레이어 액터 생성
	auto playerActor = std::make_unique<Actor>(player.GetPlayerRegionId());
	playerActor->InitializeSprite(engine, L"../Resource/알아.png",
		45.0f, 0.0f, 100.0f, 100.0f);

	// actors에 저장
	this->playerActor = playerActor.get();
	actors.push_back(std::move(playerActor));
	actors.push_back(std::move(enemyActor));

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
		if (!battleFieldCreated)
		{
			player.CreateBattleField();
			battleFieldCreated = true;
		}

		battleExpandT += battleExpandSpeed * deltaTime;
		if (battleExpandT > 1.0f) battleExpandT = 1.0f;

		// 1/3 넘는 순간 windowId 전환 + bitmap 재생성
		if (battleExpandT >= 0.9f && actors[0]->GetWindowId() != player.GetBattleFieldId())
		{
			int battleId = player.GetBattleFieldId();
			for (auto& actor : actors)
			{
				actor->SetWindowId(battleId);
				actor->ResetBitmap();
			}
			actors[0]->InitializeSprite(engine, L"../Resource/알아.png", 45.0f, 50.0f, 100.0f, 100.0f);
			actors[1]->InitializeSprite(engine, L"../Resource/도로롱.png", 40.0f, 650.0f, 100.0f, 100.0f);
		}

		float startHeight = 0.01f;
		float endHeight = 0.75f;
		float heightRatio = startHeight + (endHeight - startHeight) * battleExpandT;
		player.ResizeBattleField(heightRatio);

		if (battleExpandT >= 1.0f)
			state = BattleState::Battle;

		break;
	}

	case BattleState::Battle:
		if (input.IsKeyPressed(player.GetPlayerRegionId(), VK_BACK))
		{
			battleExpandT = 1.0f;
			state = BattleState::Return;
		}
		break;

	case BattleState::Return:
	{
		battleExpandT -= battleExpandSpeed * deltaTime;
		if (battleExpandT < 0.0f) battleExpandT = 0.0f;

		// 0.1f 이하로 내려가는 순간 region으로 복구
		if (battleExpandT <= 0.1f && actors[0]->GetWindowId() != player.GetPlayerRegionId())
		{
			actors[0]->SetWindowId(player.GetPlayerRegionId());
			actors[1]->SetWindowId(enemy.GetEnemyRegionId());
			actors[0]->ResetBitmap();
			actors[1]->ResetBitmap();
			actors[0]->InitializeSprite(engine, L"../Resource/알아.png", 45.0f, 0.0f, 100.0f, 100.0f);
			actors[1]->InitializeSprite(engine, L"../Resource/도로롱.png", 40.0f, 0.0f, 100.0f, 100.0f);
		}
		float startHeight = 0.01f;
		float endHeight = 0.75f;
		float heightRatio = startHeight + (endHeight - startHeight) * battleExpandT;
		player.ResizeBattleField(heightRatio);

		if (battleExpandT <= 0.0f)
		{
			player.DestroyBattleField();
			battleFieldCreated = false;
			state = BattleState::ReturnExplore;
		}
		break;
	}


	case BattleState::ReturnExplore:

		if (player.BattleEndRegion(deltaTime, enemy.GetEnemyRegionId()))
		{
			battleExpandT = 0.0f;
			state = BattleState::Explore;
		}
		break;
	}
	// actor update ( maybe delete ? )
	for (auto& actor : actors)
	{
		actor->Update(deltaTime);
	}
	
}

void GameContent::OnRender(EngineContext& engine)
{
	auto& d2d = engine.GetD2DManager();

	int playerRegionId = player.GetPlayerRegionId();
	int enemyRegionId = enemy.GetEnemyRegionId();

	bool drawRegion = (state == BattleState::Explore ||
		state == BattleState::MoveToBattle ||
		state == BattleState::ReturnExplore ||
		(state == BattleState::ExpandBattle && battleExpandT < 0.9f) ||
		(state == BattleState::Return && battleExpandT <= 0.1f));

	bool drawBattle = (state == BattleState::Battle) ||
		(state == BattleState::ExpandBattle && battleExpandT >= 0.9f) ||
		(state == BattleState::Return && battleExpandT > 0.1f);

	if (drawRegion)
	{
		d2d.BeginDraw(playerRegionId);
		d2d.Clear(playerRegionId, D2D1::ColorF(D2D1::ColorF::White));
		for (auto& actor : actors)
			if (actor->GetWindowId() == playerRegionId)
				actor->Render(d2d);
		d2d.EndDraw(playerRegionId);

		d2d.BeginDraw(enemyRegionId);
		d2d.Clear(enemyRegionId, D2D1::ColorF(D2D1::ColorF::White));
		for (auto& actor : actors)
			if (actor->GetWindowId() == enemyRegionId)
				actor->Render(d2d);
		d2d.EndDraw(enemyRegionId);
	}

	if (drawBattle)
	{
		int battleId = player.GetBattleFieldId();
		if (battleId == -1) return;

		d2d.BeginDraw(battleId);
		d2d.Clear(battleId, D2D1::ColorF(D2D1::ColorF::White));
		for (auto& actor : actors)
			if (actor->GetWindowId() == battleId)
				actor->Render(d2d);
		d2d.EndDraw(battleId);
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