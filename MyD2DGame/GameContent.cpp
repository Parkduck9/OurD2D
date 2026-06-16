#include "GameContent.h"
#include "EngineContext.h"
#include "WindowManager.h"

#include "InputManager.h"
#include "WicManager.h"
#include "D2DManager.h"
#include <cmath>

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
	d2d.CreateRenderTargetForOverlayDC(overlayRenderTargetId, overlay->GetMemoryDC(), overlay->GetWidth(), overlay->GetHeight());


	//투명창에 적 생성
	auto enemyActor = std::make_unique<Actor>(overlayRenderTargetId);
	enemyActor->SetAnchorWindowId(enemy.GetEnemyRegionId());
	enemyActor->InitializeSprite(engine, L"../Resource/도로롱.png",40.0f, 0.0f, 100.0f, 100.0f);
	//적 애니메이션 생성
	enemyActor->AddAnimation(L"idle", 200, 200, 60, 8, 30.0f);
	enemyActor->PlayAnimation(L"idle");
	enemyActor->AddBoxCollider(0.0f, 0.0f, 100.0f, 100.0f);


	//투명창에 플레이어 생성
	auto playerIdle = std::make_unique<Actor>(overlayRenderTargetId);
	playerIdle->SetAnchorWindowId(player.GetPlayerRegionId());
	playerIdle->InitializeSprite(engine, L"../Resource/구구가가아이들.png", 0.0f, 0.0f, 200.0f, 112.0f);
	playerIdle->AddAnimation(L"idle", 400, 225, 30, 6, 15.0f);
	playerIdle->PlayAnimation(L"idle");
	playerIdle->AddBoxCollider(75.0f, 15.0f, 50.0f, 90.0f);

	// run 액터
	auto playerRun = std::make_unique<Actor>(overlayRenderTargetId);
	playerRun->SetAnchorWindowId(player.GetPlayerRegionId());
	playerRun->InitializeSprite(engine, L"../Resource/GUGARUN.png", 0.0f, 0.0f, 200.0f, 112.0f);
	playerRun->AddAnimation(L"run", 666, 375, 20, 20, 15.0f);
	playerRun->PlayAnimation(L"run");

	this->playerActor = playerIdle.get();
	this->playerActorRun = playerRun.get();
	this->enemyActor = enemyActor.get(); // 추가

	actors.push_back(std::move(playerIdle));
	actors.push_back(std::move(playerRun));
	actors.push_back(std::move(enemyActor));


	//스폰 매니저
	spawnButtonManager.Initialize(
		overlayRenderTargetId,
		player.GetPlayerFieldId(),
		player.GetPlayerRegionId(),
		L"../Resource/5090.png"
	);
	// 각각 Field채우기
	auto* playerFieldWnd = windows.GetWindowById(player.GetPlayerFieldId());
	auto* enemyFieldWnd = windows.GetWindowById(enemy.GetEnemyFieldId());

	d2d.CreateRenderTargetForWindow(player.GetPlayerFieldId(), playerFieldWnd->GetHwnd());
	d2d.CreateRenderTargetForWindow(enemy.GetEnemyFieldId(), enemyFieldWnd->GetHwnd());
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
		MovePlayerActor(engine, deltaTime, 50.0f);

		player.DefaultFieldSystem(deltaTime);
		enemy.DefaultFieldSystem(deltaTime);

		enemyAttackTimer -= deltaTime;
		if (enemyAttackTimer <= 0.0f)
		{
			SpawnEnemyOrange(engine);
			enemyAttackTimer = 0.3f + static_cast<float>(rand()) / RAND_MAX * 0.4f;
		}

		UpdateEnemyOranges(engine, deltaTime);

		//스폰업데이트
		spawnButtonManager.Update(engine, deltaTime);

		if (spawnButtonManager.IsPlayerTouchingButton(*playerActor, engine.GetWindowManager()) && input.IsKeyPressed(player.GetPlayerRegionId(), 'E'))
		{
			spawnButtonManager.Clear();
			oranges.clear();

			battleStartX = playerActor->GetTransform().x;
			battleStartY = playerActor->GetTransform().y;

			enemyBattleStartX = enemyActor->GetTransform().x;
			enemyBattleStartY = enemyActor->GetTransform().y;

			state = BattleState::MoveToBattle;
		}


		// Enter key -> Move to Battle
		if (input.IsKeyPressed(player.GetPlayerRegionId(), VK_RETURN))
		{
			spawnButtonManager.Clear();
			oranges.clear();

			battleStartX = playerActor->GetTransform().x;
			battleStartY = playerActor->GetTransform().y;

			enemyBattleStartX = enemyActor->GetTransform().x;
			enemyBattleStartY = enemyActor->GetTransform().y;

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
		float endHeight = 1.0f;
		float heightRatio = startHeight + (endHeight - startHeight) * battleExpandT;
		player.ResizeBattleField(heightRatio);

		if (battleExpandT >= 1.0f)
		{
			auto& windows = engine.GetWindowManager();

			// 1. Resize 전 actor들의 화면 절대 위치 저장
			std::vector<std::pair<float, float>> absolutePositions;

			for (auto& actor : actors)
			{
				auto* anchorWnd = windows.GetWindowById(actor->GetAnchorWindowId());

				if (anchorWnd == nullptr)
				{
					absolutePositions.push_back({ 0.0f, 0.0f });
					continue;
				}

				absolutePositions.push_back({
					anchorWnd->GetClientX() + actor->GetTransform().x,
					anchorWnd->GetClientY() + actor->GetTransform().y
					});
			}

			// 2. Region battle 크기로 resize
			player.ResizeRegionsForBattle();

			// 3. Resize 후 기존 actor들은 화면 위치 유지되도록 보정
			for (int i = 0; i < (int)actors.size(); i++)
			{
				auto* anchorWnd = windows.GetWindowById(actors[i]->GetAnchorWindowId());
				if (anchorWnd == nullptr) continue;

				actors[i]->SetPosition(
					absolutePositions[i].first - anchorWnd->GetClientX(),
					absolutePositions[i].second - anchorWnd->GetClientY()
				);
			}

			// 4. 플레이어 battle 시작 위치 저장
			battleStartX = playerActor->GetTransform().x;
			battleStartY = playerActor->GetTransform().y;

			playerActorRun->SetPosition(battleStartX, battleStartY);

			// 5. 도로롱은 enemy region 중앙으로 배치
			auto* enemyWnd = windows.GetWindowById(enemy.GetEnemyRegionId());

			if (enemyWnd != nullptr)
			{
				RECT rect{};
				GetWindowRect(enemyWnd->GetHwnd(), &rect);

				float borderX = enemyWnd->GetClientX() - enemyWnd->GetX();
				float borderY = enemyWnd->GetClientY() - enemyWnd->GetY();

				float wndWidth = static_cast<float>(rect.right - rect.left);
				float wndHeight = static_cast<float>(rect.bottom - rect.top);

				enemyActor->SetPosition(
					(wndWidth - 100.0f) * 0.5f - borderX,
					(wndHeight - 100.0f) * 0.5f - borderY
				);

				enemyBattleStartX = enemyActor->GetTransform().x;
				enemyBattleStartY = enemyActor->GetTransform().y;
			}

			prevEnemyClientX = -1.0f;
			prevEnemyClientY = -1.0f;

			state = BattleState::Battle;
		}

		break;
	}
	case BattleState::Battle:

		player.BattleFieldSystem(deltaTime);
		enemy.BattleFieldSystem(deltaTime);

		CenterEnemyActor(engine, deltaTime);
		MovePlayerActor(engine, deltaTime, 200.0f);

		if (input.IsKeyPressed(player.GetPlayerRegionId(), 'Z'))
		{
			player.PushField(deltaTime);
			enemy.PushField(deltaTime);
		}


		if (input.IsKeyPressed(player.GetPlayerRegionId(), 'O'))
		{
			SpawnEnemyOrange(engine);
		}

		spearCooldown -= deltaTime;
		if (input.IsKeyPressed(player.GetPlayerRegionId(), 'X') && spearCooldown <= 0.0f)
		{
			SpawnPlayerSpear(engine);
			spearCooldown = 0.5f;
		}

		UpdateEnemyOranges(engine, deltaTime);
		UpdatePlayerSpears(engine, deltaTime);

		if (input.IsKeyPressed(player.GetPlayerRegionId(), VK_BACK))
		{
			oranges.clear();
			spears.clear();

			returnRegionT = 1.0f;
			returnFieldT = 1.0f;
			regionShrinkFinished = false;
			state = BattleState::ReturnCenter;
		}

		break;
	case BattleState::ReturnCenter:
	{
		CenterEnemyActor(engine, deltaTime);

		float dx = battleStartX - playerActor->GetTransform().x;
		float dy = battleStartY - playerActor->GetTransform().y;

		bool playerArrived = abs(dx) <= 5.0f && abs(dy) <= 5.0f;

		bool enemyArrived = true;

		auto& windows = engine.GetWindowManager();
		auto* enemyWnd = windows.GetWindowById(enemy.GetEnemyRegionId());

		if (enemyWnd != nullptr)
		{
			RECT rect{};
			GetWindowRect(enemyWnd->GetHwnd(), &rect);

			float borderX = enemyWnd->GetClientX() - enemyWnd->GetX();
			float borderY = enemyWnd->GetClientY() - enemyWnd->GetY();

			float wndWidth = static_cast<float>(rect.right - rect.left);
			float wndHeight = static_cast<float>(rect.bottom - rect.top);

			float targetX = (wndWidth - 100.0f) * 0.5f - borderX;
			float targetY = (wndHeight - 100.0f) * 0.5f - borderY;

			float ex = targetX - enemyActor->GetTransform().x;
			float ey = targetY - enemyActor->GetTransform().y;

			enemyArrived = abs(ex) <= 5.0f && abs(ey) <= 5.0f;
		}

		if (playerArrived && enemyArrived)
		{
			playerActor->SetPosition(battleStartX, battleStartY);
			state = BattleState::Return;
		}
		else if (!playerArrived)
		{
			playerActor->Move(dx / 0.5f * deltaTime, dy / 0.5f * deltaTime);
		}

		playerActorRun->SetPosition(
			playerActor->GetTransform().x,
			playerActor->GetTransform().y
		);

		break;
	}

	case BattleState::Return:
	{
		battleExpandT -= battleExpandSpeed * deltaTime;
		if (battleExpandT < 0.0f) battleExpandT = 0.0f;

		auto& windows = engine.GetWindowManager();

		// ------------------------
		// 1단계 : Region 축소
		// ------------------------

		float regionWidth = 0.1f + (0.3f - 0.1f) * battleExpandT;
		float regionHeight = 0.15f + (0.5f - 0.15f) * battleExpandT;

		auto* playerWnd = windows.GetWindowById(player.GetPlayerRegionId());

		if (playerWnd != nullptr)
		{
			float absX = playerWnd->GetClientX() + playerActor->GetTransform().x;
			float absY = playerWnd->GetClientY() + playerActor->GetTransform().y;

			playerWnd->ResizeWindowToMonitorRatio(
				playerWnd->GetHwnd(),
				regionWidth,
				regionHeight,
				0.5f,
				0.2f
			);

			playerActor->SetPosition(
				absX - playerWnd->GetClientX(),
				absY - playerWnd->GetClientY()
			);

			playerActorRun->SetPosition(
				playerActor->GetTransform().x,
				playerActor->GetTransform().y
			);
		}

		auto* enemyWnd = windows.GetWindowById(enemy.GetEnemyRegionId());

		if (enemyWnd != nullptr)
		{
			float absX = enemyWnd->GetClientX() + enemyActor->GetTransform().x;
			float absY = enemyWnd->GetClientY() + enemyActor->GetTransform().y;

			enemyWnd->ResizeWindowToMonitorRatio(
				enemyWnd->GetHwnd(),
				regionWidth,
				regionHeight,
				0.5f,
				0.75f
			);

			enemyActor->SetPosition(
				absX - enemyWnd->GetClientX(),
				absY - enemyWnd->GetClientY()
			);

			CenterEnemyActor(engine, deltaTime);
		}

		// ------------------------
		// Region 다 줄어든 뒤
		// BattleField 축소 시작
		// ------------------------

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

	int battleFieldId = player.GetBattleFieldId();
	if (battleFieldId != -1)
	{
		d2d.BeginDraw(battleFieldId);
		d2d.Clear(battleFieldId, D2D1::ColorF(D2D1::ColorF::White));
		d2d.EndDraw(battleFieldId);
	}

	d2d.BeginDraw(overlayRenderTargetId);
	d2d.Clear(overlayRenderTargetId, D2D1::ColorF(0.0f,0.0f,0.0f,0.0f));
	for (auto& actor : actors)
	{
		if (actor.get() == playerActor || actor.get() == playerActorRun) continue;
		actor->RenderToOverlay(d2d, windows);

		if (showCollider){
			actor->RenderColliderToOverlay(d2d, windows);
		}
	}
	for (auto& orange : oranges)
	{
		auto& d2d = engine.GetD2DManager();
		auto& windows = engine.GetWindowManager();
		orange.actor->RenderToOverlay(d2d, windows);
		if (showCollider)  // 오버레이 좌표 이용해서 따로 생성
		{
			D2D1_RECT_F rect =
				orange.actor->GetBoxCollider().GetWorldRect(*orange.actor);

			d2d.DrawRectangle(overlayRenderTargetId, rect);
		}
	}
	for (auto& spear : spears)
	{
		spear.actor->RenderToOverlay(d2d, windows);
		if (showCollider)
		{
			D2D1_RECT_F rect = spear.actor->GetBoxCollider().GetWorldRect(*spear.actor);
			d2d.DrawRectangle(overlayRenderTargetId, rect);
		}
	}

	// isMoving에 따라 하나만 렌더
	if (isMoving)
		playerActorRun->RenderToOverlay(d2d, windows);
	else
		playerActor->RenderToOverlay(d2d, windows);

	if (showCollider) {
		playerActor->RenderColliderToOverlay(d2d, windows);
		}
	spawnButtonManager.Render(d2d, windows, showCollider);
	d2d.EndDraw(overlayRenderTargetId);

	auto* overlay = windows.GetOverlayWindow();
	if (overlay != nullptr)
	{
		overlay->Present();
	}
	

	d2d.BeginDraw(player.GetPlayerFieldId());
	d2d.Clear(player.GetPlayerFieldId(), D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f)); // blue
	d2d.EndDraw(player.GetPlayerFieldId());

	d2d.BeginDraw(enemy.GetEnemyFieldId());
	d2d.Clear(enemy.GetEnemyFieldId(), D2D1::ColorF(1.0f, 0.5f, 0.0f, 1.0f)); // orange
	d2d.EndDraw(enemy.GetEnemyFieldId());

}

void GameContent::OnEnd(EngineContext& engine)
{
	oranges.clear();
	spears.clear();
	spawnButtonManager.Clear();

	actors.clear();
	playerActor = nullptr;
	playerActorRun = nullptr;
	enemyActor = nullptr;

	auto& d2d = engine.GetD2DManager();
	d2d.RemoveRenderTarget(overlayRenderTargetId);
	d2d.RemoveRenderTarget(player.GetPlayerFieldId());
	d2d.RemoveRenderTarget(enemy.GetEnemyFieldId());
}

void GameContent::UpdateEnemyOranges(EngineContext& engine, float deltaTime)
{
	for (auto& orange : oranges)
	{
		if (orange.falling)
		{
			orange.velY += 980.0f * deltaTime; // 중력
			orange.actor->Move(orange.velX * deltaTime, orange.velY * deltaTime);
			continue;
		}

		orange.elapsed += deltaTime;

		float t = orange.elapsed / orange.duration;

		if (t >= 1.0f)
		{
			orange.falling = true;
			// 포물선 끝 시점 속도를 이어받음
			orange.velX = (orange.targetX - orange.startX) / orange.duration;
			orange.velY = (orange.targetY - orange.startY) / orange.duration
				+ orange.arcHeight * 3.141592f / orange.duration;
			orange.actor->Move(orange.velX * deltaTime, orange.velY * deltaTime);
			continue;
		}

		float x = orange.startX + (orange.targetX - orange.startX) * t;
		float y = orange.startY + (orange.targetY - orange.startY) * t
			- orange.arcHeight * sinf(t * 3.141592f);

		orange.actor->SetPosition(x, y);
		D2D1_RECT_F orangeRect =
			orange.actor->GetBoxCollider().GetWorldRect(*orange.actor);

		D2D1_RECT_F playerRect =
			playerActor->GetColliderOverlayRect(engine.GetWindowManager());

		bool isHit =
			orangeRect.left < playerRect.right &&
			orangeRect.right > playerRect.left &&
			orangeRect.top < playerRect.bottom &&
			orangeRect.bottom > playerRect.top;

		if (isHit && !orange.hasHitPlayer)
		{
			orange.hasHitPlayer = true;

			if (state == BattleState::Explore)
			{
				player.ApplyFieldPenaltyOnly(0.01f);
				enemy.ApplyFieldPenaltyOnly(0.01f);
			}
			else
			{
				player.ApplyFieldPenalty(0.01f);
				enemy.ApplyFieldPenalty(0.01f);
			}
		}
	}

	auto* overlay = engine.GetWindowManager().GetOverlayWindow();
	float overlayHeight = overlay ? static_cast<float>(overlay->GetHeight()) : 9999.0f;

	oranges.erase(
		std::remove_if(oranges.begin(), oranges.end(), [overlayHeight](const EnemyOrange& o) {
			return o.actor->GetTransform().y > overlayHeight;
		}),
		oranges.end()
	);
}

void GameContent::SpawnEnemyOrange(EngineContext& engine)
{
	if (enemyActor == nullptr || playerActor == nullptr)
	{
		return;
	}

	auto& windows = engine.GetWindowManager();

	D2D1_RECT_F enemyRect = enemyActor->GetColliderOverlayRect(windows);
	D2D1_RECT_F playerRect = playerActor->GetColliderOverlayRect(windows);

	float orangeWidth = 50.0f;
	float orangeHeight = 50.0f;

	float startX = (enemyRect.left + enemyRect.right) * 0.5f - orangeWidth * 0.5f;
	float startY = (enemyRect.top + enemyRect.bottom) * 0.5f - orangeHeight * 0.5f;

	float targetX = (playerRect.left + playerRect.right) * 0.5f - orangeWidth * 0.5f;
	float targetY = (playerRect.top + playerRect.bottom) * 0.5f - orangeHeight * 0.5f;

	EnemyOrange orange;
	orange.actor = std::make_unique<Actor>(overlayRenderTargetId);

	orange.actor->InitializeSprite(
		engine,
		L"../Resource/귤.png",
		startX,
		startY,
		orangeWidth,
		orangeHeight
	);

	orange.actor->AddBoxCollider(
		10.0f,
		10.0f,
		orangeWidth - 20.0f,
		orangeHeight - 20.0f
	);

	orange.startX = startX;
	orange.startY = startY;

	targetX += static_cast<float>((rand() % 121) - 60);
	targetY += static_cast<float>((rand() % 81) - 40);
	orange.targetX = targetX;
	orange.targetY = targetY;
	orange.arcHeight = static_cast<float>((rand() % 151) + 120);

	oranges.push_back(std::move(orange));
}

void GameContent::MovePlayerActor(EngineContext& engine, float deltaTime, float moveSpeed)
{
	auto& input = engine.GetInputManager();
	bool moving = false;

	if (input.IsKeyDown(player.GetPlayerRegionId(), VK_UP))
	{
		playerActor->Move(0, -moveSpeed * deltaTime); moving = true;
	}
	if (input.IsKeyDown(player.GetPlayerRegionId(), VK_DOWN))
	{
		playerActor->Move(0, moveSpeed * deltaTime); moving = true;
	}
	if (input.IsKeyDown(player.GetPlayerRegionId(), VK_LEFT))
	{
		playerActorRun->SetFlipx(false);
		playerActor->Move(-moveSpeed * deltaTime, 0); moving = true;
	}

	if (input.IsKeyDown(player.GetPlayerRegionId(), VK_RIGHT))
	{
		playerActorRun->SetFlipx(true);
		playerActor->Move(moveSpeed * deltaTime, 0); moving = true;
	}
	isMoving = moving;
	playerActorRun->SetPosition(playerActor->GetTransform().x, playerActor->GetTransform().y);

	// 클램핑
	auto& windows = engine.GetWindowManager();
	auto* playerWnd = windows.GetWindowById(player.GetPlayerRegionId());
	if (playerWnd == nullptr) return;

	float borderX = playerWnd->GetClientX() - playerWnd->GetX();
	float borderY = playerWnd->GetClientY() - playerWnd->GetY();

	RECT rect{};
	GetWindowRect(playerWnd->GetHwnd(), &rect);
	float wndWidth = static_cast<float>(rect.right - rect.left);
	float wndHeight = static_cast<float>(rect.bottom - rect.top);

	float maxX = wndWidth - 133.0f - borderX * 2;
	float maxY = wndHeight - 115.0f - borderY - borderX;

	float clampedX = max(-65.0f, min(playerActor->GetTransform().x, maxX));
	float clampedY = max(0.0f, min(playerActor->GetTransform().y, maxY));

	playerActor->SetPosition(clampedX, clampedY);
}

void GameContent::PlayerHitSound()
{
	PlaySound(L"../Resource/Shock The World.wav", nullptr, SND_FILENAME | SND_ASYNC);
}

void GameContent::MoveEnemyActor(EngineContext& engine, float deltaTime)
{
	auto& windows = engine.GetWindowManager();
	auto* enemyWnd = windows.GetWindowById(enemy.GetEnemyRegionId());
	if (enemyWnd == nullptr) return;

	float currClientX = enemyWnd->GetClientX();
	float currClientY = enemyWnd->GetClientY();

	if (prevEnemyClientX >= 0.0f)
	{
		float diffX = currClientX - prevEnemyClientX;
		float diffY = currClientY - prevEnemyClientY;

		//enemyActor->Move(-diffX, -diffY);
	}

	prevEnemyClientX = currClientX;
	prevEnemyClientY = currClientY;

	// 클램핑
	RECT rect{};
	GetWindowRect(enemyWnd->GetHwnd(), &rect);
	float wndWidth = static_cast<float>(rect.right - rect.left);
	float wndHeight = static_cast<float>(rect.bottom - rect.top);

	float borderX = enemyWnd->GetClientX() - enemyWnd->GetX();
	float borderY = enemyWnd->GetClientY() - enemyWnd->GetY();

	float maxX = wndWidth - 100.0f - borderX * 2;
	float maxY = wndHeight - 100.0f - borderY - borderX;
	float clampedX = max(0.0f, min(enemyActor->GetTransform().x, maxX));
	float clampedY = max(0.0f, min(enemyActor->GetTransform().y, maxY));

	enemyActor->SetPosition(clampedX, clampedY);
}

void GameContent::SpawnPlayerSpear(EngineContext& engine)
{
	auto& windows = engine.GetWindowManager();
	auto* playerFieldWnd = windows.GetWindowById(player.GetPlayerFieldId());
	if (playerFieldWnd == nullptr) return;

	auto* overlay = windows.GetOverlayWindow();
	if (overlay == nullptr) return;

	float spearWidth = 100.0f;
	float spearHeight = 100.0f;

	auto* battleFieldWnd = windows.GetWindowById(player.GetBattleFieldId());
	if (battleFieldWnd == nullptr) return;

	float overlayX = static_cast<float>(overlay->GetX());
	float overlayY = static_cast<float>(overlay->GetY());

	float playerFieldLeft  = playerFieldWnd->GetClientX() - overlayX;
	float playerFieldRight = playerFieldLeft + playerFieldWnd->GetWidth();
	float fieldTop         = playerFieldWnd->GetClientY() - overlayY;
	float fieldBottom      = fieldTop + playerFieldWnd->GetHeight();

	float battleFieldLeft  = battleFieldWnd->GetClientX() - overlayX;
	float battleFieldRight = battleFieldLeft + battleFieldWnd->GetWidth();

	// 왼쪽: battlefield 좌측끝 ~ playerfield 좌측끝 중간
	float leftX  = (battleFieldLeft + playerFieldLeft) * 0.5f - spearWidth * 0.5f;
	// 오른쪽: playerfield 우측끝 ~ battlefield 우측끝 중간
	float rightX = (playerFieldRight + battleFieldRight) * 0.5f - spearWidth * 0.5f;
	float startY = fieldBottom - spearHeight;

	PlayerSpear leftSpear;
	leftSpear.actor = std::make_unique<Actor>(overlayRenderTargetId);
	leftSpear.actor->InitializeSprite(engine, L"../Resource/구구가가파도.png", leftX, startY, spearWidth, spearHeight);
	leftSpear.actor->AddBoxCollider(0.0f, 0.0f, spearWidth, spearHeight);
	spears.push_back(std::move(leftSpear));

	PlayerSpear rightSpear;
	rightSpear.actor = std::make_unique<Actor>(overlayRenderTargetId);
	rightSpear.actor->InitializeSprite(engine, L"../Resource/구구가가파도.png", rightX, startY, spearWidth, spearHeight);
	rightSpear.actor->AddBoxCollider(0.0f, 0.0f, spearWidth, spearHeight);
	spears.push_back(std::move(rightSpear));
}

void GameContent::UpdatePlayerSpears(EngineContext& engine, float deltaTime)
{
	auto& windows = engine.GetWindowManager();
	auto* playerFieldWnd = windows.GetWindowById(player.GetPlayerFieldId());

	for (auto& spear : spears)
	{
		if (spear.hasHitEnemyField) continue;

		spear.actor->Move(0.0f, -spear.speed * deltaTime);

		auto* overlay = windows.GetOverlayWindow();
		if (playerFieldWnd != nullptr && overlay != nullptr)
		{
			D2D1_RECT_F spearRect = spear.actor->GetBoxCollider().GetWorldRect(*spear.actor);
			float playerFieldTop = playerFieldWnd->GetClientY() - overlay->GetY();

			if (spearRect.top <= playerFieldTop)
			{
				spear.hasHitEnemyField = true;
				player.PushField(deltaTime);
				enemy.PushField(deltaTime);
			}
		}
	}

	spears.erase(
		std::remove_if(spears.begin(), spears.end(), [](const PlayerSpear& s) {
			return s.hasHitEnemyField;
		}),
		spears.end()
	);
}

void GameContent::CenterEnemyActor(EngineContext& engine, float deltaTime)
{
    auto& windows = engine.GetWindowManager();
    auto* enemyWnd = windows.GetWindowById(enemy.GetEnemyRegionId());
    if (enemyWnd == nullptr) return;

    RECT rect{};
    GetWindowRect(enemyWnd->GetHwnd(), &rect);

    float borderX = enemyWnd->GetClientX() - enemyWnd->GetX();
    float borderY = enemyWnd->GetClientY() - enemyWnd->GetY();

    float wndWidth = static_cast<float>(rect.right - rect.left);
    float wndHeight = static_cast<float>(rect.bottom - rect.top);

    float targetX = (wndWidth - 100.0f) * 0.5f - borderX;
    float targetY = (wndHeight - 100.0f) * 0.5f - borderY;

    float x = enemyActor->GetTransform().x;
    float y = enemyActor->GetTransform().y;

    float dx = targetX - x;
    float dy = targetY - y;

    float dist = sqrtf(dx * dx + dy * dy);
    if (dist <= 1.0f)
    {
        enemyActor->SetPosition(targetX, targetY);
        return;
    }

    float speed = 300.0f;
    float move = speed * deltaTime;

    if (move >= dist)
    {
        enemyActor->SetPosition(targetX, targetY);
    }
    else
    {
        enemyActor->Move(dx / dist * move, dy / dist * move);
    }
}