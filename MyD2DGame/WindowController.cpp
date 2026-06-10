
#include "WindowController.h"
#include "WindowManager.h"   
#include "InputManager.h"


void WindowController::Initalize(EngineContext &engine)
{
	context = &engine;
}

void WindowController::SaveStartPositions(int enemyRegionId)
{
    auto& windows = context->GetWindowManager();
    auto* playerWnd = windows.GetWindowById(playerRegionId);
    auto* enemyWnd = windows.GetWindowById(enemyRegionId);

    playerStartX = playerWnd->GetX();
    playerStartY = playerWnd->GetY();
    enemyStartX = enemyWnd->GetX();
    enemyStartY = enemyWnd->GetY();
    // 처음 위치 픽셀 값 받기
} 

void WindowController::CreatePlayerStartField() // 플레이어 필드
{
    auto& windows = context->GetWindowManager();
    playerFieldId = windows.CreateGameWindow({ 
        L"Player Field",
        0.5, 0.75,
        1.007, 0.508, false
        });
}

void WindowController::CreateEnemyStartField() // 적 필드
{
    auto& windows = context->GetWindowManager();
    enemyFieldId = windows.CreateGameWindow({
        L"Enemy Field",
        0.5, 0.25,
        1.007,  0.508 , false
        });
}

void WindowController::CreatePlayerStartRegion()
{
    auto& windows = context->GetWindowManager();

    playerRegionId = windows.CreateGameWindow( // 플레이어 지역
        {
            L"Main Window",
            0.5, 0.8,
            0.1, 0.15 , true
        }
    );
}


void WindowController::CreateEnemyStartRegion()
{
    auto& windows = context->GetWindowManager();
    enemyRegionId = windows.CreateGameWindow( // 적 지역
        {
            L"Main Window",
            0.5, 0.2,
            0.1, 0.15 , false
        }
    );
}


void WindowController::MovePlayerRegion(float deltaTime)
{
    auto& input = context->GetInputManager();
    auto& windows = context->GetWindowManager();
   /* if (input.IsKeyPressed(playerRegionId, VK_LEFT))
    {
        int playerFieldId = windows.CreateGameWindow(
            {
                L"Add Window",
                x, y,
                0.2, 0.2
            }
        );
        x  += 0.01; y+= 0.01;
    }*/
    
    if (input.IsKeyDown(playerRegionId, VK_UP))
    {
        windows.GetWindowById(playerRegionId)->MoveWindow(0.0, -0.15, 1.3, deltaTime);
    }
    if (input.IsKeyDown(playerRegionId, VK_DOWN))
    {
        windows.GetWindowById(playerRegionId)->MoveWindow(0.0, 0.15, 1.3, deltaTime);
    }
    if (input.IsKeyDown(playerRegionId, VK_LEFT))
    {
        windows.GetWindowById(playerRegionId)->MoveWindow(-0.15, 0, 1.3, deltaTime);
    }
    if (input.IsKeyDown(playerRegionId, VK_RIGHT))
    {
        windows.GetWindowById(playerRegionId)->MoveWindow(0.15, 0.0, 1.3, deltaTime);
    }
    if (input.IsKeyPressed(playerRegionId, VK_SPACE))
    {
        windows.GetWindowById(playerRegionId)->MoveWindow(0, -40.0, 1, deltaTime);
    }
}


void WindowController::BattleRegion(float deltaTime, int enemyRegionId)
{
    auto& windows = context->GetWindowManager();
    auto* playerWnd = windows.GetWindowById(playerRegionId);
    auto* enemyWnd = windows.GetWindowById(enemyRegionId);
    if (playerWnd == nullptr || enemyWnd == nullptr) return;

    MoveToward(playerRegionId, enemyStartX, enemyStartY, 3.0f, deltaTime);
    MoveToward(enemyRegionId, playerStartX, playerStartY, 3.0f, deltaTime);
}


bool WindowController::BattleEndRegion(float deltaTime, int enemyRegionId)
{
    auto& windows = context->GetWindowManager();
    auto* playerWnd = windows.GetWindowById(playerRegionId);
    auto* enemyWnd = windows.GetWindowById(enemyRegionId);
    if (playerWnd == nullptr || enemyWnd == nullptr) return false;

    MoveToward(playerRegionId, playerStartX, playerStartY, 3.0f, deltaTime);
    MoveToward(enemyRegionId, enemyStartX, enemyStartY, 3.0f, deltaTime);

    // 둘 다 도착했으면 true 반환
    bool playerArrived = abs(playerStartX - playerWnd->GetX()) 
        <= 5.0f && abs(playerStartY - playerWnd->GetY()) <= 5.0f;

    bool enemyArrived = abs(enemyStartX - enemyWnd->GetX()) 
        <= 5.0f && abs(enemyStartY - enemyWnd->GetY()) <= 5.0f;

    return playerArrived && enemyArrived; // 둘다 True여야 return 1 되도록 설정
}


void WindowController::MoveToward(int windowId, float targetX, float targetY, float speed, float deltaTime)
{
    auto& windows = context->GetWindowManager();
    auto* wnd = windows.GetWindowById(windowId);
    if (wnd == nullptr) return;

    float dirX = targetX - wnd->GetX();  // windowId -> wnd
    float dirY = targetY - wnd->GetY();  // windowId -> wnd

    if (abs(dirX) <= 5.0f && abs(dirY) <= 5.0f) return;

    HMONITOR hMonitor = MonitorFromWindow(wnd->GetHwnd(), MONITOR_DEFAULTTONEAREST);  // windowId -> wnd
    MONITORINFO mi = {};
    mi.cbSize = sizeof(MONITORINFO);
    if (!GetMonitorInfo(hMonitor, &mi)) return;

    int workWidth = mi.rcWork.right - mi.rcWork.left;
    int workHeight = mi.rcWork.bottom - mi.rcWork.top;

    wnd->MoveWindow(dirX / workWidth, dirY / workHeight, speed, deltaTime);
}

void WindowController::DefaultFieldSystem(float deltaTime)
{
    fixedFieldTime += deltaTime;

    if (fixedFieldTime >= 1.0f)
    {
        if (playerFieldId != -1) PlayerResizeField(deltaTime);
        if (enemyFieldId != -1) EnemyResizeField(deltaTime);
        fixedFieldTime = 0.0f;
    }
}

void WindowController::PlayerResizeField(float deltaTime)
{
    auto& windows = context->GetWindowManager();
    auto* fieldWnd = windows.GetWindowById(playerFieldId);
    if (fieldWnd == nullptr) return;

    fieldHeightRatio -= 0.001f;
    fieldWnd->ReSizeWindow(fieldWidthRatio, fieldHeightRatio);
    fieldWnd->MoveWindow(0.000f, 0.0005f, 1.0f, 1.0f); // 아래로
}

void WindowController::EnemyResizeField(float deltaTime)
{
    auto& windows = context->GetWindowManager();
    auto* fieldWnd = windows.GetWindowById(enemyFieldId);
    if (fieldWnd == nullptr) return;

    fieldHeightRatio += 0.001f;
    fieldWnd->ReSizeWindow(fieldWidthRatio, fieldHeightRatio);
    fieldWnd->MoveWindow(0.0000f, 0.0005f, 1.0f, 1.0f); // 위로
}
