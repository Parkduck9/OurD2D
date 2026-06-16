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

	// 도로롱 걷기 액터 (Explore 전용)
	auto enemyActorWalkPtr = std::make_unique<Actor>(overlayRenderTargetId);
	enemyActorWalkPtr->SetAnchorWindowId(enemy.GetEnemyRegionId());
	enemyActorWalkPtr->InitializeSprite(engine, L"../Resource/도로롱_걷기.png", 40.0f, 0.0f, 130.0f, 130.0f);
	enemyActorWalkPtr->AddAnimation(L"도로롱_걷기", 177, 206, 8, 8, 15.0f);
	enemyActorWalkPtr->PlayAnimation(L"도로롱_걷기");

	this->playerActor = playerIdle.get();
	this->playerActorRun = playerRun.get();
	this->enemyActor = enemyActor.get();
	this->enemyActorWalk = enemyActorWalkPtr.get();

	// 초기 이동 방향 랜덤 대각선 (4대각선 중 하나)
	float speedX = 150.0f, speedY = 300.0f;
	enemyRegionVelX = (rand() % 2 == 0) ?  speedX : -speedX;
	enemyRegionVelY = (rand() % 2 == 0) ?  speedY : -speedY;
	if (enemyActorWalk) enemyActorWalk->SetFlipx(enemyRegionVelX > 0.0f);

	actors.push_back(std::move(playerIdle));
	actors.push_back(std::move(playerRun));
	actors.push_back(std::move(enemyActor));
	actors.push_back(std::move(enemyActorWalkPtr));


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

	if (!audiomanger.PlayBGM(L"../Resource/평시.wav"))
	{
		MessageBoxW(nullptr, L"BGM 재생 실패", L"Audio Error", MB_OK);
	}
}

void GameContent::OnUpdate(EngineContext& engine, float deltaTime)
{
	auto& input = engine.GetInputManager();

	//콜라이더 온오프
	if (input.IsKeyPressed(player.GetPlayerRegionId(), VK_F1))
	{
		showCollider = !showCollider;
		//audiomanger.PlayBGM(L"../Resource/평시.wav");
	}

	switch (state) {
	case BattleState::Explore:
		// playerMove for key
		player.MovePlayerRegion(deltaTime);
		MovePlayerActor(engine, deltaTime, 50.0f);

		player.DefaultFieldSystem(deltaTime);
		enemy.DefaultFieldSystem(deltaTime);
		UpdateEnemyExplore(engine, deltaTime);

		// 플레이어 이동 속도 추정
		if (playerActor != nullptr)
		{
			auto& windows = engine.GetWindowManager();
			auto* anchor = windows.GetWindowById(player.GetPlayerRegionId());
			if (anchor != nullptr)
			{
				float curX = anchor->GetClientX() + playerActor->GetTransform().x;
				float curY = anchor->GetClientY() + playerActor->GetTransform().y;
				if (prevPlayerOverlayX >= 0.0f)
				{
					playerVelX = (curX - prevPlayerOverlayX) / deltaTime;
					playerVelY = (curY - prevPlayerOverlayY) / deltaTime;
				}
				prevPlayerOverlayX = curX;
				prevPlayerOverlayY = curY;
			}
		}

		enemyAttackTimer -= deltaTime;
		if (enemyAttackTimer <= 0.0f)
		{
			SpawnEnemyOrange(engine);
			enemyAttackTimer = 0.15f + static_cast<float>(rand()) / RAND_MAX * 0.2f;
		}

		UpdateEnemyOranges(engine, deltaTime);

		//audiomanger.PlayBGM(L"../Resource/평시.wav");
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

		// EnemyWin 조건: player field가 player region보다 같거나 작아질 때
		if (player.IsPlayerFieldSmallerThanRegion())
		{
			spawnButtonManager.Clear();
			oranges.clear();
			player.DestroyPlayerFieldAndRegion(engine);
			enemy.ExpandEnemyWindowsFull();
			state = BattleState::EnemyWin;
		}

		break;

	case BattleState::MoveToBattle:

		audiomanger.PlayBGM(L"../Resource/줄다리기.wav");
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

		CenterEnemyActor(engine, deltaTime);

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

			// 3. Resize 후 기존 actor들은 화면 위치 유지되도록 보정 (enemyActor 제외 - CenterEnemyActor가 처리)
			for (int i = 0; i < (int)actors.size(); i++)
			{
				if (actors[i].get() == enemyActor) continue;

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


			prevEnemyClientX = -1.0f;
			prevEnemyClientY = -1.0f;

			// 도로운전 초기화
			ClearDrivingObjects();
			battleElapsed    = 0.0f;
			driveSpawnIndex  = 0;
			battleOrangeTimer = battleOrangeInterval; // 바로 안 던지게 첫 간격만큼 대기

			// Battle 진입 시 player region top Y 저장 (경고 위치 기준)
			{
				auto& wins = engine.GetWindowManager();
				auto* overlay   = wins.GetOverlayWindow();
				auto* playerWnd = wins.GetWindowById(player.GetPlayerRegionId());
				if (overlay && playerWnd)
					battlePlayerRegionY = playerWnd->GetY() - static_cast<float>(overlay->GetY());
			}

			battleTimer = 20.0f;
			state = BattleState::Battle;
		}

		break;
	}
	case BattleState::Battle:
	{
		auto& windows = engine.GetWindowManager();
		auto* enemyWnd = windows.GetWindowById(enemy.GetEnemyRegionId());

		// 리사이즈 전 도로롱 절대 위치 저장
		float enemyAbsX = 0.0f, enemyAbsY = 0.0f;
		if (enemyWnd != nullptr)
		{
			enemyAbsX = enemyWnd->GetClientX() + enemyActor->GetTransform().x;
			enemyAbsY = enemyWnd->GetClientY() + enemyActor->GetTransform().y;
		}

		player.BattleFieldSystem(deltaTime);
		enemy.BattleFieldSystem(deltaTime);

		// 리사이즈 후 절대 위치 복원
		if (enemyWnd != nullptr)
		{
			enemyActor->SetPosition(
				enemyAbsX - enemyWnd->GetClientX(),
				enemyAbsY - enemyWnd->GetClientY()
			);
		}

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
			spearCooldown = 10.0f;
		}

		UpdateEnemyOranges(engine, deltaTime);
		UpdatePlayerSpears(engine, deltaTime);

		battleElapsed += deltaTime;

		// Battle 귤 (간격 길고 데미지 큼)
		battleOrangeTimer -= deltaTime;
		if (battleOrangeTimer <= 0.0f)
		{
			SpawnEnemyOrange(engine);
			battleOrangeTimer = battleOrangeInterval;
		}
		UpdateEnemyOranges(engine, deltaTime);

		// 경고/차 스폰 타이밍: 차=2,5,8,11,12,14초 기준 0.3초 전 경고
		static constexpr float warningTimes[6] = { 1.7f, 4.7f, 7.7f, 10.7f, 11.7f, 13.7f };
		if (driveSpawnIndex < 6 && !drivingWarning.active && !drivingCar.active)
		{
			if (battleElapsed >= warningTimes[driveSpawnIndex])
				SpawnDrivingWarning(engine);
		}
		if (drivingWarning.active)
		{
			drivingWarning.timer -= deltaTime;
			if (drivingWarning.timer <= 0.0f)
			{
				drivingWarning.active = false;
				SpawnDrivingCar(engine);
				driveSpawnIndex++;
			}
		}
		if (drivingCar.active)
			UpdateDrivingCar(engine, deltaTime);

		battleTimer -= deltaTime;

		// 승패 조건 체크 (Return 애니메이션 후 창 처리)
		if (player.IsPlayerFieldSmallerThanRegion())
		{
			playerFieldLost = true;
			oranges.clear(); spears.clear(); ClearDrivingObjects();
			returnRegionT = 1.0f; returnFieldT = 1.0f; regionShrinkFinished = false;
			state = BattleState::ReturnCenter;
			break;
		}
		if (enemy.IsEnemyFieldSmallerThanRegion())
		{
			enemyFieldLost = true;
			oranges.clear(); spears.clear(); ClearDrivingObjects();
			returnRegionT = 1.0f; returnFieldT = 1.0f; regionShrinkFinished = false;
			state = BattleState::ReturnCenter;
			break;
		}

		if (battleTimer <= 0.0f || input.IsKeyPressed(player.GetPlayerRegionId(), VK_BACK))
		{
			oranges.clear(); spears.clear(); ClearDrivingObjects();
			returnRegionT = 1.0f; returnFieldT = 1.0f; regionShrinkFinished = false;
			state = BattleState::ReturnCenter;
		}

		break;
	}
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
		audiomanger.PlayBGM(L"../Resource/평시.wav");
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
			enemy.ResetEnemyRegionClamp();

			if (playerFieldLost)
			{
				playerFieldLost = false;
				player.DestroyPlayerFieldAndRegion(engine);
				enemy.ExpandEnemyWindowsFull();
				state = BattleState::EnemyWin;
			}
			else if (enemyFieldLost)
			{
				enemyFieldLost = false;
				enemy.DestroyEnemyFieldAndRegion(engine);
				player.ExpandPlayerWindowsFull();
				state = BattleState::PlayerWin;
			}
			else
			{
				state = BattleState::Explore;
			}
		}
		break;

	case BattleState::EnemyWin:
		// enemy region/field만 남아서 Explore처럼 튕겨다님
		UpdateEnemyExplore(engine, deltaTime);
		break;

	case BattleState::PlayerWin:
		// player region/field만 남아서 플레이어 계속 조작 가능
		// DefaultFieldSystem 호출 안 함 → field 크기 고정 (더 안 줄어듦)
		player.MovePlayerRegion(deltaTime);
		MovePlayerActor(engine, deltaTime, 50.0f);
		break;
	}
	// actor update
	for (auto& actor : actors)
	{
		// EnemyWin: player 창이 없으므로 player 액터 업데이트 스킵
		if (state == BattleState::EnemyWin &&
			(actor.get() == playerActor || actor.get() == playerActorRun)) continue;
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
	bool isExploreLike = (state == BattleState::Explore || state == BattleState::EnemyWin);
	bool enemyWindowsAlive = (state != BattleState::PlayerWin);
	bool playerWindowsAlive = (state != BattleState::EnemyWin);

	for (auto& actor : actors)
	{
		if (actor.get() == playerActor || actor.get() == playerActorRun) continue;

		bool isEnemyActor = (actor.get() == enemyActor || actor.get() == enemyActorWalk);
		if (isEnemyActor && !enemyWindowsAlive) continue;

		if (isExploreLike && actor.get() == enemyActor) continue;
		if (!isExploreLike && actor.get() == enemyActorWalk) continue;

		actor->RenderToOverlay(d2d, windows);

		if (showCollider && playerWindowsAlive && enemyWindowsAlive)
			actor->RenderColliderToOverlay(d2d, windows);
	}

	if (playerWindowsAlive)
	{
		for (auto& orange : oranges)
		{
			orange.actor->RenderToOverlay(d2d, windows);
			if (showCollider)
			{
				D2D1_RECT_F rect = orange.actor->GetBoxCollider().GetWorldRect(*orange.actor);
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

		// player actor 렌더
		if (isMoving)
			playerActorRun->RenderToOverlay(d2d, windows);
		else
			playerActor->RenderToOverlay(d2d, windows);

		if (showCollider)
			playerActor->RenderColliderToOverlay(d2d, windows);

		// 도로롱 경고 렌더
		if (drivingWarning.active && drivingWarning.actor)
		{
			drivingWarning.actor->RenderToOverlay(d2d, windows);
			d2d.DrawRectangle(overlayRenderTargetId, drivingWarning.warnRect);
		}

		// 도로롱 차 렌더
		if (drivingCar.active && drivingCar.actor)
		{
			drivingCar.actor->RenderToOverlay(d2d, windows);
			if (showCollider)
			{
				D2D1_RECT_F r = drivingCar.actor->GetBoxCollider().GetWorldRect(*drivingCar.actor);
				d2d.DrawRectangle(overlayRenderTargetId, r);
			}
		}

		spawnButtonManager.Render(d2d, windows, showCollider);
	}
	d2d.EndDraw(overlayRenderTargetId);

	auto* overlay = windows.GetOverlayWindow();
	if (overlay != nullptr)
	{
		overlay->Present();
	}
	

	if (player.GetPlayerFieldId() != -1)
	{
		d2d.BeginDraw(player.GetPlayerFieldId());
		d2d.Clear(player.GetPlayerFieldId(), D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f)); // blue
		d2d.EndDraw(player.GetPlayerFieldId());
	}

	if (enemy.GetEnemyFieldId() != -1)
	{
		d2d.BeginDraw(enemy.GetEnemyFieldId());
		d2d.Clear(enemy.GetEnemyFieldId(), D2D1::ColorF(1.0f, 0.5f, 0.0f, 1.0f)); // orange
		d2d.EndDraw(enemy.GetEnemyFieldId());
	}

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
	enemyActorWalk = nullptr;

	auto& d2d = engine.GetD2DManager();
	d2d.RemoveRenderTarget(overlayRenderTargetId);
	if (player.GetPlayerFieldId() != -1)
		d2d.RemoveRenderTarget(player.GetPlayerFieldId());
	if (enemy.GetEnemyFieldId() != -1)
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
				player.ApplyFieldPenalty(battleOrangeDamage);
				enemy.ApplyFieldPenalty(battleOrangeDamage);
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
		5.0f,
		5.0f,
		orangeWidth - 10.0f,
		orangeHeight - 10.0f
	);

	orange.startX = startX;
	orange.startY = startY;

	// 30% 확률로 플레이어 이동 방향 예측
	if (rand() % 10 < 3)
	{
		float predictTime = 0.6f; // 몇 초 앞을 예측할지
		targetX += playerVelX * predictTime;
		targetY += playerVelY * predictTime;
	}
	else
	{
		targetX += static_cast<float>((rand() % 121) - 60);
		targetY += static_cast<float>((rand() % 81) - 40);
	}
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

void GameContent::UpdateEnemyExplore(EngineContext& engine, float deltaTime)
{
    auto& windows = engine.GetWindowManager();

    auto* fieldWnd  = windows.GetWindowById(enemy.GetEnemyFieldId());
    auto* regionWnd = windows.GetWindowById(enemy.GetEnemyRegionId());
    if (fieldWnd == nullptr || regionWnd == nullptr) return;

    // 픽셀 단위 위치/크기
    RECT fieldRect{};
    GetWindowRect(fieldWnd->GetHwnd(), &fieldRect);

    float regionX = regionWnd->GetX();
    float regionY = regionWnd->GetY();
    float regionW = regionWnd->GetWidth();
    float regionH = regionWnd->GetHeight();

    // 이동
    float nextX = regionX + enemyRegionVelX * deltaTime;
    float nextY = regionY + enemyRegionVelY * deltaTime;

    // 좌우 경계 반사
    if (nextX < fieldRect.left)
    {
        nextX = static_cast<float>(fieldRect.left);
        enemyRegionVelX = fabsf(enemyRegionVelX); // 오른쪽으로
        if (enemyActorWalk) enemyActorWalk->SetFlipx(true);
    }
    else if (nextX + regionW > fieldRect.right)
    {
        nextX = static_cast<float>(fieldRect.right) - regionW;
        enemyRegionVelX = -fabsf(enemyRegionVelX); // 왼쪽으로
        if (enemyActorWalk) enemyActorWalk->SetFlipx(false);
    }

    // 상하 경계 반사
    if (nextY < fieldRect.top)
    {
        nextY = static_cast<float>(fieldRect.top);
        enemyRegionVelY = fabsf(enemyRegionVelY);
    }
    else if (nextY + regionH > fieldRect.bottom)
    {
        nextY = static_cast<float>(fieldRect.bottom) - regionH;
        enemyRegionVelY = -fabsf(enemyRegionVelY);
    }

    // 모니터 비율로 변환 후 이동
    HMONITOR hMon = MonitorFromWindow(regionWnd->GetHwnd(), MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(MONITORINFO);
    if (!GetMonitorInfo(hMon, &mi)) return;

    int workW = mi.rcWork.right  - mi.rcWork.left;
    int workH = mi.rcWork.bottom - mi.rcWork.top;

    float xRatio = (nextX + regionW / 2.0f) / workW;
    float yRatio = (nextY + regionH / 2.0f) / workH;

    regionWnd->ResizeWindowToMonitorRatio(regionWnd->GetHwnd(), 0.1f, 0.15f, xRatio, yRatio);

    // walk 액터 위치 동기화
    if (enemyActorWalk)
        enemyActorWalk->SetPosition(enemyActorWalk->GetTransform().x, enemyActorWalk->GetTransform().y);
}

void GameContent::SpawnDrivingWarning(EngineContext& engine)
{
    auto& windows = engine.GetWindowManager();
    auto* overlay      = windows.GetOverlayWindow();
    auto* battleWnd    = windows.GetWindowById(player.GetBattleFieldId());
    if (!overlay || !battleWnd) return;

    float ovX = static_cast<float>(overlay->GetX());
    float ovY = static_cast<float>(overlay->GetY());

    float bfLeft   = battleWnd->GetClientX() - ovX;
    float bfRight  = bfLeft + battleWnd->GetWidth();
    float bfBottom = battleWnd->GetClientY() - ovY + battleWnd->GetHeight();

    // 플레이어 overlay X 기준 +-200 범위 내 랜덤, battlefield 안으로 클램핑
    D2D1_RECT_F playerRect = playerActor->GetColliderOverlayRect(windows);
    float playerCenterX = (playerRect.left + playerRect.right) * 0.5f - carW * 0.5f;
    float offset = static_cast<float>((rand() % 161) - 80); // -80 ~ +80
    float spawnX = playerCenterX + offset;
    spawnX = max(bfLeft, min(spawnX, bfRight - carW));
    drivingWarning.carSpawnX = spawnX;

    // 경고 이미지: player region top Y (battle 진입 시 저장)
    float warnY = battlePlayerRegionY;

    // 경고 사각형: battlefield bottom 조금 위 ~ 경고 이미지 조금 위
    drivingWarning.warnRect = D2D1::RectF(
        spawnX,
        warnY - 5.0f,
        spawnX + carW,
        bfBottom - 5.0f
    );

    // 경고 스프라이트 생성
    drivingWarning.actor = std::make_unique<Actor>(overlayRenderTargetId);
    drivingWarning.actor->InitializeSprite(engine,
        L"../Resource/도로롱위험위험해.png",
        spawnX, warnY, carW, warnH);

    drivingWarning.timer  = 0.3f;
    drivingWarning.active = true;
}

void GameContent::SpawnDrivingCar(EngineContext& engine)
{
    auto& windows  = engine.GetWindowManager();
    auto* overlay  = windows.GetOverlayWindow();
    auto* battleWnd = windows.GetWindowById(player.GetBattleFieldId());
    if (!overlay || !battleWnd) return;

    float ovY    = static_cast<float>(overlay->GetY());
    float bfBottom = battleWnd->GetClientY() - ovY + battleWnd->GetHeight();

    drivingCar.actor = std::make_unique<Actor>(overlayRenderTargetId);
    drivingCar.actor->InitializeSprite(engine,
        L"../Resource/도로롱도로운전.png",
        drivingWarning.carSpawnX, bfBottom, carW, carH);
    drivingCar.actor->AddBoxCollider(0.0f, 0.0f, carW, carH);
    drivingCar.active = true;
}

void GameContent::UpdateDrivingCar(EngineContext& engine, float deltaTime)
{
    if (!drivingCar.actor) return;

    auto& windows  = engine.GetWindowManager();
    auto* overlay  = windows.GetOverlayWindow();

    // 위쪽으로 이동
    drivingCar.actor->Move(0.0f, -drivingCar.speed * deltaTime);

    // 플레이어 충돌 체크
    D2D1_RECT_F carRect    = drivingCar.actor->GetBoxCollider().GetWorldRect(*drivingCar.actor);
    D2D1_RECT_F playerRect = playerActor->GetColliderOverlayRect(windows);

    bool hit = carRect.left   < playerRect.right  &&
               carRect.right  > playerRect.left   &&
               carRect.top    < playerRect.bottom &&
               carRect.bottom > playerRect.top;

    if (hit)
    {
        player.ApplyFieldPenalty(0.07f);
        enemy.ApplyFieldPenalty(0.07f);
        ClearDrivingObjects();
        return;
    }

    // enemy field 상단 위로 벗어나면 소멸
    auto* enemyFieldWnd = windows.GetWindowById(enemy.GetEnemyFieldId());
    if (overlay && enemyFieldWnd)
    {
        float enemyFieldTop = enemyFieldWnd->GetClientY() - static_cast<float>(overlay->GetY());
        if (carRect.bottom < enemyFieldTop)
        {
            drivingCar.active  = false;
            drivingCar.actor   = nullptr;
        }
    }
}
