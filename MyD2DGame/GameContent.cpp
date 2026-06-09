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
	auto& windows = engine.GetWindowManager();
	auto& wic = engine.GetWicManager();
	auto& d2d = engine.GetD2DManager();


	int mainWindowId = windows.CreateGameWindow(
		{
			L"Main Window",
			0.5, 0.5,
			0.5, 0.5
		}
	);

	this->mainWindowId = mainWindowId;


	GameWindow* mainWindow = windows.GetWindowById(mainWindowId);
	if (mainWindow == nullptr) return;
	
	HRESULT hr = d2d.CreateRenderTargetForWindow(
		mainWindowId,
		mainWindow->GetHwnd()
	);

	if (FAILED(hr))
	{
		return;
	}

	Microsoft::WRL::ComPtr<IWICBitmapSource> source;

	hr = wic.LoadImageSource(L"../Resource/도로롱.png", source);

	if (FAILED(hr)) return;

	hr = d2d.CreateBitmapFromWicSource(
		mainWindowId,
		source.Get(),
		testBitmap
	);
	if (FAILED(hr))
	{
		testBitmap.Reset();
		return;
	}
	

	playerAnimation.Initialize(200, 200, 60, 8,30.0f);

	auto player = std::make_unique<Actor>(mainWindowId);
	player->SetBitmap(testBitmap);
	player->SetPosition(100.f, 100.0f);
	player->SetSize(128.0f, 128.0f);
	
	playerActor = player.get();
	actors.push_back(std::move(player));


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
				0.2, 0.2
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
	}
	if (input.IsKeyDown(mainWindowId, VK_LEFT))
	{
		playerActor->Move(-5, 0);
		//windows.GetWindowById(mainWindowId)->MoveWindow(-0.2, 0, 3, deltaTime);
	}
	if (input.IsKeyDown(mainWindowId, VK_UP))
	{
		playerActor->Move(0, -5);
		//windows.GetWindowById(mainWindowId)->MoveWindow(0, -0.2, 3, deltaTime);
	}
	if (input.IsKeyDown(mainWindowId, VK_DOWN))
	{
		playerActor->Move(0, 5);
		//windows.GetWindowById(mainWindowId)->MoveWindow(0, 0.2, 3, deltaTime);
	}

	playerAnimation.Update(deltaTime);

	for (auto& actor : actors)
	{
		actor->Update(deltaTime);
	}
}

void GameContent::OnRender(EngineContext& engine)
{
	if (mainWindowId == -1 || testBitmap == nullptr)
	{
		return;
	}

	auto& d2d = engine.GetD2DManager();

	d2d.BeginDraw(mainWindowId);

	d2d.Clear(mainWindowId, D2D1::ColorF(D2D1::ColorF::White));

	//d2d.DrawBitmap(mainWindowId, testBitmap.Get(), D2D1::RectF(0.0f, 0.0f, 800.0f, 500.0f));

	D2D1_RECT_F sourceRect = playerAnimation.GetSourceRect();

	D2D1_RECT_F destRect = D2D1::RectF(
		playerX,
		playerY,
		playerX + 128.0f,
		playerY + 128.0f
	);

	d2d.DrawBitmapFrame(
		mainWindowId,
		testBitmap.Get(),
		destRect,
		sourceRect
	);

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
		testBitmap.Reset();

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

	testBitmap.Reset();
}

void GameContent::PlayerHitSound()
{
	PlaySound(L"../Resource/Shock The World.wav", nullptr, SND_FILENAME | SND_ASYNC);
}