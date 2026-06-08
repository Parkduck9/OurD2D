#pragma once
#include "Content.h"

class GameContent : public Content
{
public:
	void OnStart(EngineContext& engine) override;
	void OnUpdate(EngineContext& engine, float deltaTime) override;
	void OnEnd(EngineContext& engine) override;


private:
	int mainWindowId = -1;
	float a = 0.1f, b = 0.1f;//그냥 임시로 창생성 보려고 만든거 나중에 삭제할거임!
};