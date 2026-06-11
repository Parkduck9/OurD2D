
#include "WindowController.h"
#include "WindowManager.h"   
#include "InputManager.h"


void WindowController::Initialize(EngineContext &engine)
{
	context = &engine;
}

void WindowController::SaveStartPositions(int enemyRegionId)
{
    auto& windows = context->GetWindowManager();
    auto* playerWnd = windows.GetWindowById(playerRegionId);
    auto* enemyWnd = windows.GetWindowById(enemyRegionId);

    if (playerWnd == nullptr || enemyWnd == nullptr) return;

    playerStartX = playerWnd->GetX();
    playerStartY = playerWnd->GetY();
    enemyStartX = enemyWnd->GetX();
    enemyStartY = enemyWnd->GetY();
} 

void WindowController::CreatePlayerStartField() 
{
    auto& windows = context->GetWindowManager();
    playerFieldId = windows.CreateGameWindow({ 
        L"Player Field",
        0.5, 0.75,
        1.007, 0.508, false
        });
}

void WindowController::CreateEnemyStartField()
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

    playerRegionId = windows.CreateGameWindow( // �÷��̾� ����
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
    enemyRegionId = windows.CreateGameWindow( // �� ����
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

    // �� �� ���������� true ��ȯ
    bool playerArrived = abs(playerStartX - playerWnd->GetX()) 
        <= 5.0f && abs(playerStartY - playerWnd->GetY()) <= 5.0f;

    bool enemyArrived = abs(enemyStartX - enemyWnd->GetX()) 
        <= 5.0f && abs(enemyStartY - enemyWnd->GetY()) <= 5.0f;

    return playerArrived && enemyArrived; // �Ѵ� True���� return 1 �ǵ��� ����
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

void WindowController::ResizePlayerField(float boundary)
{
    auto& windows = context->GetWindowManager();
    auto* fieldWnd = windows.GetWindowById(playerFieldId);
    if (fieldWnd == nullptr) return;

    float heightRatio = 1.0f - boundary;
    float yRatio = boundary + heightRatio / 2.0f;

    fieldWnd->ResizeWindowToMonitorRatio(
        fieldWnd->GetHwnd(),
        fieldWidthRatio,
        heightRatio,
        0.5,
        yRatio
    );
}

void WindowController::ResizeEnemyField(float boundary)
{
    auto& windows = context->GetWindowManager();
    auto* fieldWnd = windows.GetWindowById(enemyFieldId);
    if (fieldWnd == nullptr) return;

    float heightRatio = boundary;
    float yRatio = boundary / 2.0f;
    

    fieldWnd->ResizeWindowToMonitorRatio(
        fieldWnd->GetHwnd(),
        fieldWidthRatio,
        heightRatio,
        0.5,
        yRatio
    );
}