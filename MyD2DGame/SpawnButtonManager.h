#pragma once
#include "Actor.h"

#include <memory>
#include <random>
#include <string>
#include <vector>

class EngineContext;
class D2DManager;
class WindowManger;

class SpawnButtonManager
{
public:
	void Initialize(
		int overlayRenderTargetId,
		int playerFieldId,
		int playerRegionId,
		const std::wstring& spritePath
	);

	void Update(EngineContext& engine, float deltaTime);
	void Render(D2DManager& d2d, const WindowManager& windows, bool showCollider);
	void Clear();


	bool IsPlayerTouchingButton(
		const Actor& playerActor,
		const WindowManager& windows
	)const;


private:
	void SpawnButton(EngineContext& engine);
	void UpdateButtonVisibility(const WindowManager& windows);

	float DistancePointToRect(float x, float y, const D2D1_RECT_F& rect) const;
	float Clamp01(float value) const;

private:
	int overlayRenderTargetId = -1;
	int playerFieldId = -1;
	int playerRegionId = -1;

	std::wstring spritePath;

	float spawnTimer = 0.0f;
	float spawnInterval = 5.0f;
	float revealDistance = 250.0f;

	float buttonWidth = 80.0f;
	float buttonHeight = 80.0f;

	int maxButtonCount = 5;

	std::vector<std::unique_ptr<Actor>> buttons;
	std::mt19937 randomEngine;
};