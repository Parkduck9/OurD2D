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

	//d2d.CreateRenderTargetForWindow(player.GetPlayerRegionId(), playerWnd->GetHwnd());
	//d2d.CreateRenderTargetForWindow(enemy.GetEnemyRegionId(), enemyWnd->GetHwnd());
	//투명창 생성
	windows.CreateOverlayWindow();

	auto* overlay = windows.GetOverlayWindow();
	overlayRenderTargetId = windows.GetOverlayRenderTargetId();
	//투명창에 렌더타겟 생성
	d2d.CreateRenderTargetForWindow(overlayRenderTargetId, overlay->GetHwnd());


	//투명창에 적 생성
	auto enemyActor = std::make_unique<Actor>(overlayRenderTargetId);
	enemyActor->SetAnchorWindowId(enemy.GetEnemyRegionId());
	enemyActor->InitializeSprite(engine, L"../Resource/도로롱.png",40.0f, 0.0f, 100.0f, 100.0f);
	//적 애니메이션 생성
	enemyActor->AddAnimation(L"idle", 200, 200, 60, 8, 30.0f);
	enemyActor->PlayAnimation(L"idle");
	enemyActor->AddBoxCollider(0.0f, 0.0f, 100.0f, 100.0f);


	//투명창에 플레이어 생성
	auto playerActor = std::make_unique<Actor>(overlayRenderTargetId);
	playerActor->SetAnchorWindowId(player.GetPlayerRegionId());

	playerActor->InitializeSprite(engine, L"../Resource/구구가가idle2 (1)-export-export.png", 00.0f, 0.0f, 200.0f, 112.0f);
	playerActor->AddAnimation(L"idle", 400, 225, 30, 6, 15.0f);
	playerActor->PlayAnimation(L"idle");

	//playerActor->InitializeSprite(engine, L"../Resource/알아.png", 40.0f, 0.0f, 100.0f, 100.0f);
	playerActor->AddBoxCollider(75.0f, 15.0f, 50.0f, 90.0f);


	// actors에 저장
	this->playerActor = playerActor.get();
	actors.push_back(std::move(playerActor));
	actors.push_back(std::move(enemyActor));

}

void GameContent::OnUpdate(EngineContext& engine, float deltaTime)
{
	auto& input = engine.GetInputManager();

	//콜라이더 온오프
	if (input.IsKeyPressed(player.GetPlayerRegionId(), VK_F1))
	{
		showCollider = !showCollider;
	}

	switch (state) {
	case BattleState::Explore:
		// playerMove for key
		player.MovePlayerRegion(deltaTime);
		player.DefaultFieldSystem(deltaTime);
		enemy.DefaultFieldSystem(deltaTime);

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

		float startHeight = 0.01f;
		float endHeight = 0.75f;
		float heightRatio = startHeight + (endHeight - startHeight) * battleExpandT;
		player.ResizeBattleField(heightRatio);

		if (battleExpandT >= 1.0f) {
			/*player.ResizeRegionsForBattle();
			enemy.ResizeRegionsForBattle();
			state = BattleState::Battle;*/
			auto& windows = engine.GetWindowManager();

			// 1. Resize 전 절대 위치 저장
			std::vector<std::pair<float, float>> absolutePositions;
			for (auto& actor : actors)
			{
				auto* anchorWnd = windows.GetWindowById(actor->GetAnchorWindowId());
				if (anchorWnd == nullptr) { absolutePositions.push_back({ 0,0 }); continue; }
				absolutePositions.push_back({
					anchorWnd->GetClientX() + actor->GetTransform().x,
					anchorWnd->GetClientY() + actor->GetTransform().y
					});
			}

			// 2. Resize
			player.ResizeRegionsForBattle();
			enemy.ResizeRegionsForBattle();

			// 3. Resize 후 transform 보정
			for (int i = 0; i < actors.size(); i++)
			{
				auto* anchorWnd = windows.GetWindowById(actors[i]->GetAnchorWindowId());
				if (anchorWnd == nullptr) continue;
				actors[i]->SetPosition(
					absolutePositions[i].first - anchorWnd->GetClientX(),
					absolutePositions[i].second - anchorWnd->GetClientY()
				);

				// playerActor 배틀 시작 위치 저장
				if (actors[i].get() == playerActor)
				{
					battleStartX = playerActor->GetTransform().x;
					battleStartY = playerActor->GetTransform().y;
				}
			}
			state = BattleState::Battle;
		}

		break;
	}

	case BattleState::Battle:
		player.BattleFieldSystem(deltaTime);
		enemy.BattleFieldSystem(deltaTime);
		MovePlayerActor(engine, deltaTime);


		if (input.IsKeyPressed(player.GetPlayerRegionId(), 'Z'))
		{
			player.PushField(deltaTime);
			enemy.PushField(deltaTime);
		}

		if (input.IsKeyPressed(player.GetPlayerRegionId(), VK_BACK))
		{
			state = BattleState::ReturnCenter;
		}
		break;
	
	case BattleState::ReturnCenter:
	{
		float dx = battleStartX - playerActor->GetTransform().x;
		float dy = battleStartY - playerActor->GetTransform().y;

		if (abs(dx) <= 5.0f && abs(dy) <= 5.0f)
		{
			playerActor->SetPosition(battleStartX, battleStartY);

			// 도착 -> Restore 후 Return
			auto& windows = engine.GetWindowManager();
			std::vector<std::pair<float, float>> absolutePositions;
			for (auto& actor : actors)
			{
				auto* anchorWnd = windows.GetWindowById(actor->GetAnchorWindowId());
				if (anchorWnd == nullptr) { absolutePositions.push_back({ 0,0 }); continue; }
				absolutePositions.push_back({
					anchorWnd->GetClientX() + actor->GetTransform().x,
					anchorWnd->GetClientY() + actor->GetTransform().y
					});
			}

			player.RestoreRegionsFromBattle();
			enemy.RestoreRegionsFromBattle();

			for (int i = 0; i < (int)actors.size(); i++)
			{
				auto* anchorWnd = windows.GetWindowById(actors[i]->GetAnchorWindowId());
				if (anchorWnd == nullptr) continue;
				actors[i]->SetPosition(
					absolutePositions[i].first - anchorWnd->GetClientX(),
					absolutePositions[i].second - anchorWnd->GetClientY()
				);
			}

			battleExpandT = 1.0f;
			state = BattleState::Return;
		}
		else
		{
			playerActor->Move(dx / 0.5f * deltaTime, dy / 0.5f * deltaTime);
		}
		break;
	}

	case BattleState::Return:
	{
		battleExpandT -= battleExpandSpeed * deltaTime;

		if (battleExpandT < 0.0f) battleExpandT = 0.0f;

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
	auto& windows = engine.GetWindowManager();

	// Battle Field 렌더링
	int battleFieldId = player.GetBattleFieldId();
	if (battleFieldId != -1)
	{
		d2d.BeginDraw(battleFieldId);
		d2d.Clear(battleFieldId, D2D1::ColorF(D2D1::ColorF::White)); // 원하는 색
		d2d.EndDraw(battleFieldId);
	}

	// 기존 overlay 렌더링
	d2d.BeginDraw(overlayRenderTargetId);
	d2d.Clear(overlayRenderTargetId, D2D1::ColorF(1.0f, 0.0f, 1.0f));
	for (auto& actor : actors)
	{
		actor->RenderToOverlay(d2d, windows);
		
		if (showCollider)
		{
			actor->RenderColliderToOverlay(d2d, windows);
		}
	}
	d2d.EndDraw(overlayRenderTargetId);
}

void GameContent::OnEnd(EngineContext& engine)
{
	actors.clear();
	playerActor = nullptr;
}

void GameContent::MovePlayerActor(EngineContext& engine, float deltaTime)
{
	auto& input = engine.GetInputManager();
	if (input.IsKeyDown(player.GetPlayerRegionId(), VK_UP))
		playerActor->Move(0, -200.0f * deltaTime);
	if (input.IsKeyDown(player.GetPlayerRegionId(), VK_DOWN))
		playerActor->Move(0, 200.0f * deltaTime);
	if (input.IsKeyDown(player.GetPlayerRegionId(), VK_LEFT))
		playerActor->Move(-200.0f * deltaTime, 0);
	if (input.IsKeyDown(player.GetPlayerRegionId(), VK_RIGHT))
		playerActor->Move(200.0f * deltaTime, 0);
	// 클램핑
	auto& windows = engine.GetWindowManager();
	auto* playerWnd = windows.GetWindowById(player.GetPlayerRegionId());
	if (playerWnd == nullptr) return;

	float borderX = playerWnd->GetClientX() - playerWnd->GetX();
	float borderY = playerWnd->GetClientY() - playerWnd->GetY();

	float maxX = playerWnd->GetWidth() - playerActor->GetTransform().width - borderX * 2;
	float maxY = playerWnd->GetHeight() - playerActor->GetTransform().height - borderY - borderX;

	float clampedX = max(0.0f, min(playerActor->GetTransform().x, maxX));
	float clampedY = max(0.0f, min(playerActor->GetTransform().y, maxY));

	playerActor->SetPosition(clampedX, clampedY);
}

void GameContent::PlayerHitSound()
{
	PlaySound(L"../Resource/Shock The World.wav", nullptr, SND_FILENAME | SND_ASYNC);
}