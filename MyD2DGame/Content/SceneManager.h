#pragma once
#include "../Actor.h"
#include "../EngineContext.h"
#include "../D2DManager.h"
#include "../WindowManager.h"
#include <memory>

enum class ScenePhase
{
    Title,
    Intro,
    Done,           // 게임 진행 중
    EndingWait,     // 1초 대기
    EndingFirst,    // 도로윈 or GuGuWin_1
    EndingSecond,   // GuGuWin_2 (PlayerWin only, Z키)
    EndingBehind,   // GuGuWin_Behind (PlayerWin only, Z키)
};

class SceneManager
{
public:
    void Initialize(EngineContext& engine, int overlayRtId, int inputWindowId);
    void Update(EngineContext& engine, float deltaTime);
    void Render(D2DManager& d2d, WindowManager& windows);

    void StartEnding(EngineContext& engine, bool playerWin, int inputWndId);

    bool IsFinished()            const { return phase == ScenePhase::Done; }
    bool IsRestartRequested()    const { return restartRequested; }
    bool IsEndingImageShowing()  const { return phase == ScenePhase::EndingFirst
                                             || phase == ScenePhase::EndingSecond
                                             || phase == ScenePhase::EndingBehind; }
    bool IsEnding()              const { return phase == ScenePhase::EndingWait
                                             || IsEndingImageShowing(); }

private:
    ScenePhase phase = ScenePhase::Title;

    int overlayRtId = 0;
    int inputWndId  = 0;

    // 인트로
    std::unique_ptr<Actor> titleActor;
    std::unique_ptr<Actor> startActor;
    std::unique_ptr<Actor> introActor;
    int introIndex = 0;
    static constexpr int introCount = 5;
    void AdvanceIntro(EngineContext& engine);

    // 엔딩
    std::unique_ptr<Actor> endingActor;
    bool  playerWin      = false;
    float endingWaitTimer = 0.0f;
    bool  restartRequested = false;
    bool  f5WasDown        = false;

    void ShowEndingFirst(EngineContext& engine);
    void ShowEndingSecond(EngineContext& engine);
    void ShowEndingBehind(EngineContext& engine);
    D2D1_RECT_F GetMonitorRect(WindowManager& windows);
};
