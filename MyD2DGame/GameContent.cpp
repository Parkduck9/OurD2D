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
	player.Initalize(engine);
	enemy.Initalize(engine);
	auto& windows = engine.GetWindowManager();
	auto& d2d = engine.GetD2DManager();
	// 플레이어와 적 객체 생성


	int mainWindowId = windows.CreateGameWindow(
		{
			L"Main Window",
			0.5, 0.5,
			0.5, 0.5,
			true
		}
	);

	this->mainWindowId = mainWindowId;


	GameWindow* mainWindow = windows.GetWindowById(mainWindowId);
	if (mainWindow == nullptr) return;
	//렌더타겟 생성
	HRESULT hr = d2d.CreateRenderTargetForWindow(
		mainWindowId,
		mainWindow->GetHwnd()
	);
	player.CreatePlayerStartField();
	player.CreatePlayerStartRegion();
	// 플레이어 지역, 필드 창 생성

	if (FAILED(hr))
	{
		return;
	}

	Microsoft::WRL::ComPtr<IWICBitmapSource> source;


	auto player = std::make_unique<Actor>(mainWindowId);
	player->SetPosition(100.f, 100.0f);
	player->SetSize(128.0f, 128.0f);

	//이미지, 애니메이션 등록(애니메이션 추가 등록은 추후 예정)
	player->InitializeSprite(engine, L"../Resource/도로롱.png", 100.0f, 100.0f, 128.0f, 128.0f);
	//player->AddAnimation(L"idle",200, 200, 60, 8, 30.0f);
	//player->InitializeSprite(engine, L"../Resource/도로롱_걷기.png", 100.0f, 100.0f, 128.0f, 128.0f);
	//player->AddAnimation(L"walk", 176, 176, 8, 8, 30.0f);

	playerActor = player.get();
	actors.push_back(std::move(player));


	enemy.CreateEnemyStartField();
	enemy.CreateEnemyStartRegion();
	// 적 지역, 필드 창 생성

	player.SaveStartPositions(enemy.GetEnemyRegionId()); 
	// 시작 위치 저장

}

void GameContent::OnUpdate(EngineContext& engine, float deltaTime)
{
	auto& input = engine.GetInputManager();
	auto& windows = engine.GetWindowManager();

	if (input.IsKeyPressed(mainWindowId, VK_SPACE))
	{
		int mainWindow = windows.CreateGameWindow(
			{
				L"Add Window",
				a, b,
				0.2, 0.2,
				false
			}
		);
		
			a += 0.01; b += 0.01;
			HMONITOR hMonitor = MonitorFromPoint({ 0,0 }, MONITOR_DEFAULTTONEAREST);

			MONITORINFO mi = {};
			mi.cbSize = sizeof(MONITORINFO);

			if (!GetMonitorInfo(hMonitor, &mi)) return;


			RECT work = mi.rcWork; //툴바 작업 표시줄 등등 제외한 영역

			if (work.bottom < b)
			{
				a = 0.3f;
				b = 0.3f;
			}
	}
	if (input.IsKeyPressed(mainWindowId, VK_TAB))
	{
		int subWindow = windows.CreateGameWindow(
			{
				L"Add Window",
				a+0.3f, b,
				0.2, 0.2,
				true
			}
		);

		a += 0.01; b += 0.01;
		HMONITOR hMonitor = MonitorFromPoint({ 0,0 }, MONITOR_DEFAULTTONEAREST);

		MONITORINFO mi = {};
		mi.cbSize = sizeof(MONITORINFO);

		if (!GetMonitorInfo(hMonitor, &mi)) return;


		RECT work = mi.rcWork; //툴바 작업 표시줄 등등 제외한 영역

		if (work.bottom < b)
		{
			a = 0.3f;
			b = 0.3f;
		}
	}
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


	
	player.MovePlayerRegion(deltaTime);
	player.ResizeField();
	auto& input = engine.GetInputManager();

	//테스트용 


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