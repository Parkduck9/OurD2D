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
			0.3, 0.3
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

	hr = wic.LoadImageSource(L"../Resource/알아.png", source);

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
		windows.GetWindowById(mainWindowId)->MoveWindow(0.2, 0,3,deltaTime);
		
		PlayerHitSound();
	}
	if (input.IsKeyDown(mainWindowId, VK_LEFT))
	{
		windows.GetWindowById(mainWindowId)->MoveWindow(-0.2, 0, 3, deltaTime);
	}
	if (input.IsKeyDown(mainWindowId, VK_UP))
	{
		windows.GetWindowById(mainWindowId)->MoveWindow(0, -0.2, 3, deltaTime);
	}
	if (input.IsKeyDown(mainWindowId, VK_DOWN))
	{
		windows.GetWindowById(mainWindowId)->MoveWindow(0, 0.2, 3, deltaTime);
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

	d2d.DrawBitmap(mainWindowId, testBitmap.Get(), D2D1::RectF(0.0f, 0.0f, 800.0f, 500.0f));

	HRESULT hr = d2d.EndDraw(mainWindowId);

	if (hr == D2DERR_RECREATE_TARGET)
	{
		testBitmap.Reset();
	}
}

void GameContent::OnEnd(EngineContext& engine)
{
	testBitmap.Reset();
}
