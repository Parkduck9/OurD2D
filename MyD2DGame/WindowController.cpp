
#include "WindowController.h"
#include "WindowManager.h"   
#include "InputManager.h"
#include "D2DManager.h"

//EngineContext save reference
void WindowController::Initialize(EngineContext &engine)
{
	context = &engine;
}

// player/enemy region save start position 
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

// player field 
void WindowController::CreatePlayerStartField() 
{
    auto& windows = context->GetWindowManager();
    playerFieldId = windows.CreateGameWindow({ 
        L"Player Field",
        0.5, 0.75,
        1.007, 0.508, false
        });
}

// enemy field created
void WindowController::CreateEnemyStartField()
{
    auto& windows = context->GetWindowManager();
    enemyFieldId = windows.CreateGameWindow({
        L"Enemy Field",
        0.5, 0.25,
        1.007,  0.508 , false
        });
}

// create player start region
void WindowController::CreatePlayerStartRegion()
{
    auto& windows = context->GetWindowManager();

    playerRegionId = windows.CreateGameWindow( // �÷��̾� ����
        {
            L"Main Window",
            0.5, 0.8,
            0.1, 0.15 , true,
            WindowCloseAction::QuitApp
        }
    );
}

// create enemy start region
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

// BattleState change -> Created Battle field
// if (!= -1) -> just one create
void WindowController::CreateBattleField()
{
    auto& windows = context->GetWindowManager();

    if (battleFieldId != -1) return;

    battleFieldId = windows.CreateGameWindow(
        {
            L"Battle Field",
            0.5f,
            0.5f,
            0.1f,
            0.75f,
            false
        }
    );
    auto* battleWnd = windows.GetWindowById(battleFieldId);
    if (battleWnd == nullptr) return;

    context->GetD2DManager().CreateRenderTargetForWindow(battleFieldId, battleWnd->GetHwnd());

    // 렌더 타겟 등록 후 즉시 작게 축소
    ResizeBattleField(0.01f);
}

// battle field height -> adjustment heightRatio 
// expandBattle / return (state) -> see production
void WindowController::ResizeBattleField(float heightRatio)
{
    auto& windows = context->GetWindowManager();
    auto* battleWnd = windows.GetWindowById(battleFieldId);

    if (battleWnd == nullptr) return;

    battleWnd->ResizeWindowToMonitorRatio(
        battleWnd->GetHwnd(),
        0.3f,
        heightRatio,
        0.5f,
        0.5f
    );
}

//Not implemented
void WindowController::DestroyBattleField()
{
    // WindowManager에 Delete/Destroy 함수가 있으면 그걸 쓰면 됨.
    // 예:
    // context->GetWindowManager().DestroyGameWindow(battleFieldId);
    auto& windows = context->GetWindowManager();
    auto* battleWnd = windows.GetWindowById(battleFieldId);

    battleWnd->DestroyWin();

    battleFieldId = -1;
}


void WindowController::ResizeRegionsForBattle()
{
    auto& windows = context->GetWindowManager();

    // enemy region = BattleField 위 절반
    auto* playerWnd = windows.GetWindowById(playerRegionId);
    if (playerWnd != nullptr)
        playerWnd->ResizeWindowToMonitorRatio(
            playerWnd->GetHwnd(),
            0.3f, 0.5,
            0.5f, 0.25f
        );

    // player region = BattleField 아래 절반
    auto* enemyWnd = windows.GetWindowById(enemyRegionId);
    if (enemyWnd != nullptr)
        enemyWnd->ResizeWindowToMonitorRatio(
            enemyWnd->GetHwnd(),
            0.3f, 0.5f,
            0.5f, 0.75f
        );
}

void WindowController::RestoreRegionsFromBattle()
{
    auto& windows = context->GetWindowManager();

    // player는 enemyStart 위치로 (교환된 상태)
    auto* enemyWnd = windows.GetWindowById(enemyRegionId);
    if (enemyWnd != nullptr)
        enemyWnd->ResizeWindowToMonitorRatio(
            enemyWnd->GetHwnd(),
            0.1f, 0.15f,
            0.5f, 0.8f  // 원래 player 시작 위치
        );

    // enemy는 playerStart 위치로 (교환된 상태)
    auto* playerWnd = windows.GetWindowById(playerRegionId);
    if (playerWnd != nullptr)
        playerWnd->ResizeWindowToMonitorRatio(
            playerWnd->GetHwnd(),
            0.1f, 0.15f,
            0.5f, 0.2f  // 원래 enemy 시작 위치
        );
}


// Input key -> Player region Move
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
    if (input.IsKeyDown(playerRegionId, VK_ESCAPE))
    {
        windows.GetWindowById(playerRegionId)->DestroyWin();
    }
   
}

// player,enemy region wnd -> change(player, enemy)
void WindowController::BattleRegion(float deltaTime, int enemyRegionId)
{
    auto& windows = context->GetWindowManager();
    auto* playerWnd = windows.GetWindowById(playerRegionId);
    auto* enemyWnd = windows.GetWindowById(enemyRegionId);
    if (playerWnd == nullptr || enemyWnd == nullptr) return;

    MoveToward(playerRegionId, enemyStartX, enemyStartY, 3.0f, deltaTime);
    MoveToward(enemyRegionId, playerStartX, playerStartY, 3.0f, deltaTime);
}

// return to start position(player, enemy)
bool WindowController::BattleEndRegion(float deltaTime, int enemyRegionId)
{
    auto& windows = context->GetWindowManager();
    auto* playerWnd = windows.GetWindowById(playerRegionId);
    auto* enemyWnd = windows.GetWindowById(enemyRegionId);
    if (playerWnd == nullptr || enemyWnd == nullptr) return false;

    MoveToward(playerRegionId, playerStartX, playerStartY, 3.0f, deltaTime);
    MoveToward(enemyRegionId, enemyStartX, enemyStartY, 3.0f, deltaTime);

    // if inside 5px -> Arrived(true) or false
    bool playerArrived = abs(playerStartX - playerWnd->GetX()) 
        <= 5.0f && abs(playerStartY - playerWnd->GetY()) <= 5.0f;

    bool enemyArrived = abs(enemyStartX - enemyWnd->GetX()) 
        <= 5.0f && abs(enemyStartY - enemyWnd->GetY()) <= 5.0f;

    return playerArrived && enemyArrived;  // -> true change state
}

// windowId (targetX,Y) pixel -> move Speed(speeD) 
// if inside 5px -> Dont move
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



void WindowController::BattleFieldSystem(float deltaTime)
{
    fieldBoundary += 0.01f * deltaTime;
    if (fieldBoundary > 1.0f) fieldBoundary = 1.0f;
    ResizePlayerField(fieldBoundary);
    ResizeEnemyField(fieldBoundary);
    ResizeRegionsForBattleField(fieldBoundary);

}

void WindowController::PushField(float deltaTime)
{
    fieldBoundary -= 0.5f * deltaTime;
    if (fieldBoundary < 0.0f) fieldBoundary = 0.0f;
    ResizePlayerField(fieldBoundary);
    ResizeEnemyField(fieldBoundary);
    ResizeRegionsForBattleField(fieldBoundary);
}

void WindowController::ResizeRegionsForBattleField(float boundary)
{
    auto& windows = context->GetWindowManager();
    auto* playerWnd = windows.GetWindowById(playerRegionId);
    auto* enemyWnd = windows.GetWindowById(enemyRegionId);

    float playerHeight = boundary;           // enemyField 크기
    float playerY = boundary / 2.0f;

    float enemyHeight = 1.0f - boundary;    // playerField 크기
    float enemyY = (1.0f - boundary) / 2.0f + boundary; // playerField 중심

    if (playerWnd != nullptr)
        playerWnd->ResizeWindowToMonitorRatio(
            playerWnd->GetHwnd(),
            0.3f, playerHeight, 0.5f, playerY
        );

    if (enemyWnd != nullptr)
        enemyWnd->ResizeWindowToMonitorRatio(
            enemyWnd->GetHwnd(),
            0.3f, enemyHeight, 0.5f, enemyY
        );
}

void WindowController::ApplyFieldPenalty(float amount)
{
    fieldBoundary += amount;

    if (fieldBoundary < 0.0f)
    {
        fieldBoundary = 0.0f;
    }

    ResizePlayerField(fieldBoundary);
    ResizeEnemyField(fieldBoundary);
    ResizeRegionsForBattleField(fieldBoundary);
}

void WindowController::ApplyFieldPenaltyOnly(float amount)
{
    fieldBoundary += amount;

    if (fieldBoundary < 0.0f)
    {
        fieldBoundary = 0.0f;
    }

    ResizePlayerField(fieldBoundary);
    ResizeEnemyField(fieldBoundary);
}




void WindowController::DefaultFieldSystem(float deltaTime)
{
    fieldBoundary += 0.0025f * deltaTime; // 속도 조절
    if (fieldBoundary > 1.0f) fieldBoundary = 1.0f;
    ResizePlayerField(fieldBoundary);
    ResizeEnemyField(fieldBoundary);
    ClampRegionsToField();

}

void WindowController::ClampRegionsToField()
{
    auto& windows = context->GetWindowManager();
    auto* playerFieldWnd = windows.GetWindowById(playerFieldId);
    auto* playerRegionWnd = windows.GetWindowById(playerRegionId);

    if (playerFieldWnd != nullptr && playerRegionWnd != nullptr)
    {
        RECT fieldRect{};
        GetWindowRect(playerFieldWnd->GetHwnd(), &fieldRect);
        float fieldTop = static_cast<float>(fieldRect.top);
        float fieldBottom = static_cast<float>(fieldRect.bottom);
        float fieldLeft = static_cast<float>(fieldRect.left);
        float fieldRight = static_cast<float>(fieldRect.right);

        float regionX = playerRegionWnd->GetX();
        float regionY = playerRegionWnd->GetY();
        float regionWidth = playerRegionWnd->GetWidth();
        float regionHeight = playerRegionWnd->GetHeight();

        float clampedX = regionX;
        float clampedY = regionY;

        if (regionX < fieldLeft)   clampedX = fieldLeft;
        if (regionX + regionWidth > fieldRight)  clampedX = fieldRight - regionWidth;
        if (regionY < fieldTop)    clampedY = fieldTop;
        if (regionY + regionHeight > fieldBottom) clampedY = fieldBottom - regionHeight;

        if (clampedX != regionX || clampedY != regionY)
        {
            HMONITOR hMonitor = MonitorFromWindow(playerRegionWnd->GetHwnd(), MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = {};
            mi.cbSize = sizeof(MONITORINFO);
            if (!GetMonitorInfo(hMonitor, &mi)) return;

            int workWidth = mi.rcWork.right - mi.rcWork.left;
            int workHeight = mi.rcWork.bottom - mi.rcWork.top;

            float xRatio = (clampedX + regionWidth / 2.0f) / workWidth;
            float yRatio = (clampedY + regionHeight / 2.0f) / workHeight;

            playerRegionWnd->ResizeWindowToMonitorRatio(
                playerRegionWnd->GetHwnd(),
                0.1f, 0.15f,
                xRatio, yRatio
            );
        }
    }

}


// based on boundary -> player field resize
// boundary size up -> player field size down

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
// based on boundary -> boundary size up = enemy field size up
void WindowController::ResizeEnemyField(float boundary)
{
    auto& windows = context->GetWindowManager();
    auto* fieldWnd = windows.GetWindowById(enemyFieldId);
    if (fieldWnd == nullptr) return;

    float heightRatio = boundary;
    float yRatio = boundary / 2.0f; // 0이 위다! 그러니까 반대로 슈슉
    

    fieldWnd->ResizeWindowToMonitorRatio(
        fieldWnd->GetHwnd(),
        fieldWidthRatio,
        heightRatio,
        0.5,
        yRatio
    );
}

// player, enemy region position arrived? < check
// inside 5px -> arrived(true)
bool WindowController::IsBattleRegionArrived(int enemyRegionId)
{
    auto& windows = context->GetWindowManager();

    auto* playerWnd = windows.GetWindowById(playerRegionId);
    auto* enemyWnd = windows.GetWindowById(enemyRegionId);

    if (playerWnd == nullptr || enemyWnd == nullptr) return false;

    bool playerArrived =
        abs(enemyStartX - playerWnd->GetX()) <= 5.0f &&
        abs(enemyStartY - playerWnd->GetY()) <= 5.0f;

    bool enemyArrived =
        abs(playerStartX - enemyWnd->GetX()) <= 5.0f &&
        abs(playerStartY - enemyWnd->GetY()) <= 5.0f;

    return playerArrived && enemyArrived; // true -> change battlestate
}
