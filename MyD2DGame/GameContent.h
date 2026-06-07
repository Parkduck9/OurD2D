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
};