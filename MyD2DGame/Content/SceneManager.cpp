#include "SceneManager.h"
#include "../InputManager.h"
#include <Windows.h>

// 주 모니터 기준 overlay 상대 좌표 + 크기 반환 (x, y, w, h → rect 형식으로 재사용)
static void GetPrimaryMonitorLayout(WindowManager& windows,
    float& outX, float& outY, float& outW, float& outH)
{
    auto* overlay = windows.GetOverlayWindow();

    POINT pt{ 0, 0 };
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(hMon, &mi);

    outW = static_cast<float>(mi.rcMonitor.right  - mi.rcMonitor.left);
    outH = static_cast<float>(mi.rcMonitor.bottom - mi.rcMonitor.top);
    outX = overlay ? static_cast<float>(mi.rcMonitor.left - overlay->GetX()) : 0.0f;
    outY = overlay ? static_cast<float>(mi.rcMonitor.top  - overlay->GetY()) : 0.0f;
}

void SceneManager::Initialize(EngineContext& engine, int overlayRtId, int inputWindowId)
{
    this->overlayRtId = overlayRtId;
    this->inputWndId  = inputWindowId;
    phase             = ScenePhase::Title;
    introIndex        = 0;
    restartRequested  = false;

    auto& windows = engine.GetWindowManager();
    float monX, monY, monW, monH;
    GetPrimaryMonitorLayout(windows, monX, monY, monW, monH);

    float titleH = monH;
    float titleW = monH * (3000.0f / 1687.0f);
    float titleX = monX + (monW - titleW) * 0.5f;
    float titleY = monY;

    titleActor = std::make_unique<Actor>(overlayRtId);
    titleActor->InitializeSprite(engine, L"../Resource/Title.png", titleX, titleY, titleW, titleH);

    startActor = std::make_unique<Actor>(overlayRtId);
    startActor->InitializeSprite(engine, L"../Resource/StartText.png", titleX, titleY + titleH, 677, 369);
}

void SceneManager::Update(EngineContext& engine, float deltaTime)
{
    auto& input = engine.GetInputManager();
    restartRequested = false;

    // F5는 창 포커스 무관하게 감지
    bool f5Down    = (GetAsyncKeyState(VK_F5) & 0x8000) != 0;
    bool f5Pressed = f5Down && !f5WasDown;
    f5WasDown      = f5Down;

    switch (phase)
    {
    case ScenePhase::Title:
        if (input.IsKeyPressed(inputWndId, VK_SPACE))
        {
            titleActor.reset();
            startActor.reset();
            introIndex = 0;
            AdvanceIntro(engine);
            phase = ScenePhase::Intro;
        }
        if (input.IsKeyPressed(inputWndId, VK_ESCAPE))
        {
            PostQuitMessage(0);
        }
        break;

    case ScenePhase::Intro:
        if (input.IsKeyPressed(inputWndId, 'Z'))
        {
            introIndex++;
            if (introIndex >= introCount)
            {
                introActor.reset();
                phase = ScenePhase::Done;
            }
            else
            {
                AdvanceIntro(engine);
            }
        }
        break;

    case ScenePhase::EndingWait:
        endingWaitTimer -= deltaTime;
        if (endingWaitTimer <= 0.0f)
        {
            ShowEndingFirst(engine);
            phase = ScenePhase::EndingFirst;
        }
        break;

    case ScenePhase::EndingFirst:
        if (input.IsKeyPressed(inputWndId, 'Z'))
        {
            if (playerWin)
            {
                ShowEndingSecond(engine);
                phase = ScenePhase::EndingSecond;
            }
        }
        if (f5Pressed)
        {
            endingActor.reset();
            restartRequested = true;
        }
        break;

    case ScenePhase::EndingSecond:
        if (input.IsKeyPressed(inputWndId, 'Z'))
        {
            ShowEndingBehind(engine);
            phase = ScenePhase::EndingBehind;
        }
        if (f5Pressed)
        {
            endingActor.reset();
            restartRequested = true;
        }
        break;

    case ScenePhase::EndingBehind:
        if (f5Pressed)
        {
            endingActor.reset();
            restartRequested = true;
        }
        break;

    default:
        break;
    }

    if (titleActor)  titleActor->Update(deltaTime);
    if (startActor)  startActor->Update(deltaTime);
    if (introActor)  introActor->Update(deltaTime);
    if (endingActor) endingActor->Update(deltaTime);
}

void SceneManager::Render(D2DManager& d2d, WindowManager& windows)
{
    switch (phase)
    {
    case ScenePhase::Title:
        if (titleActor) titleActor->RenderToOverlay(d2d, windows);
        if (startActor) startActor->RenderToOverlay(d2d, windows);
        break;

    case ScenePhase::Intro:
        if (introActor) introActor->RenderToOverlay(d2d, windows);
        break;

    case ScenePhase::EndingFirst:
    case ScenePhase::EndingSecond:
    case ScenePhase::EndingBehind:
        if (endingActor) endingActor->RenderToOverlay(d2d, windows);
        break;

    default:
        break;
    }
}

void SceneManager::StartEnding(EngineContext& engine, bool isPlayerWin, int inputWndId)
{
    playerWin        = isPlayerWin;
    this->inputWndId = inputWndId;
    endingWaitTimer  = 1.0f;
    endingActor.reset();
    phase = ScenePhase::EndingWait;
}

void SceneManager::ShowEndingFirst(EngineContext& engine)
{
    auto& windows = engine.GetWindowManager();
    float monX, monY, monW, monH;
    GetPrimaryMonitorLayout(windows, monX, monY, monW, monH);

    float h = monH;
    float w = monH * (16.0f / 9.0f);
    float x = monX + (monW - w) * 0.5f;
    float y = monY;

    const wchar_t* path = playerWin
        ? L"../Resource/GuGuWin_1.png"
        : L"../Resource/DororongWin.png";

    endingActor = std::make_unique<Actor>(overlayRtId);
    endingActor->InitializeSprite(engine, path, x, y, w, h);
}

void SceneManager::ShowEndingSecond(EngineContext& engine)
{
    auto& windows = engine.GetWindowManager();
    float monX, monY, monW, monH;
    GetPrimaryMonitorLayout(windows, monX, monY, monW, monH);

    float h = monH;
    float w = monH * (16.0f / 9.0f);
    float x = monX + (monW - w) * 0.5f;
    float y = monY;

    endingActor = std::make_unique<Actor>(overlayRtId);
    endingActor->InitializeSprite(engine, L"../Resource/GuGuWin_2.png", x, y, w, h);
}

void SceneManager::ShowEndingBehind(EngineContext& engine)
{
    auto& windows = engine.GetWindowManager();
    float monX, monY, monW, monH;
    GetPrimaryMonitorLayout(windows, monX, monY, monW, monH);

    float h = monH;
    float w = monH * (16.0f / 9.0f);
    float x = monX + (monW - w) * 0.5f;
    float y = monY;

    endingActor = std::make_unique<Actor>(overlayRtId);
    endingActor->InitializeSprite(engine, L"../Resource/GuGuWin_Behind.png", x, y, w, h);
}

void SceneManager::AdvanceIntro(EngineContext& engine)
{
    auto& windows = engine.GetWindowManager();
    float monX, monY, monW, monH;
    GetPrimaryMonitorLayout(windows, monX, monY, monW, monH);

    const wchar_t* paths[introCount] = {
        L"../Resource/0.png",
        L"../Resource/1.png",
        L"../Resource/2.png",
        L"../Resource/3.png",
        L"../Resource/4.png",
        L"../Resource/구가도로게임진짜완성.png"
    };

    float h = monH;
    float w = monH * (3000.0f / 1686.0f);
    float x = monX + (monW - w) * 0.5f;
    float y = monY;

    introActor = std::make_unique<Actor>(overlayRtId);
    introActor->InitializeSprite(engine, paths[introIndex], x, y, w, h);
}
