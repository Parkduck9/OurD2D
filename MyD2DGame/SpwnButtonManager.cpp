#include "SpawnButtonManager.h"

#include "D2DManager.h"
#include "EngineContext.h"
#include "GameWindow.h"
#include "OverlayWindow.h"
#include "WindowManager.h"

#include <Windows.h>
#include <cmath>

void SpawnButtonManager::Initialize(
	int overlayRenderTargetId,
	int playerFieldId,
	int playerRegionId,
	const std::wstring& spritePath
)
{
	this->overlayRenderTargetId = overlayRenderTargetId;
	this->playerFieldId = playerFieldId;
	this->playerRegionId = playerRegionId;
	this->spritePath = spritePath;

	std::random_device rd;
	randomEngine.seed(rd());
}

void SpawnButtonManager::Update(EngineContext& engine, float deltaTime)
{
	spawnTimer += deltaTime;

	if (spawnTimer >= spawnInterval)
	{
		SpawnButton(engine);
		spawnTimer = 0.0f;
	}

	UpdateButtonVisibility(engine.GetWindowManager());

	for (auto& button : buttons)
	{
		button->Update(deltaTime);
	}
}

void SpawnButtonManager::Render(D2DManager& d2d, const WindowManager& windows, bool showCollider)
{
	for (auto& button : buttons)
	{
		if (button->GetAlpha() <= 0.01f)
		{
			continue;
		}
		button->RenderToOverlay(d2d, windows);

		if (showCollider && button->HasBoxCollider())
		{
			D2D1_RECT_F rect = button->GetBoxCollider().GetWorldRect(*button);
			d2d.DrawRectangle(overlayRenderTargetId, rect);
		}
	}
}

void SpawnButtonManager::Clear()
{
	buttons.clear();
	spawnTimer = 0.0f;
}

void SpawnButtonManager::SpawnButton(EngineContext& engine)
{
	if ((int)buttons.size() >= maxButtonCount)
	{
		return;
	}

	auto& windows = engine.GetWindowManager();

	const GameWindow* fieldWindow = windows.GetWindowById(playerFieldId);
	const OverlayWindow* overlay = windows.GetOverlayWindow();

	if (fieldWindow == nullptr || overlay == nullptr)
	{
		return;
	}

	RECT clientRect{};
	if (!GetClientRect(fieldWindow->GetHwnd(), &clientRect))
	{
		return;
	}

	float fieldLeft = fieldWindow->GetClientX() - static_cast<float>(overlay->GetX());
	float fieldTop = fieldWindow->GetClientY() - static_cast<float>(overlay->GetY());

	float fieldWidth = static_cast<float>(clientRect.right - clientRect.left);
	float fieldHeight = static_cast<float>(clientRect.bottom - clientRect.top);

	if (fieldWidth <= buttonWidth || fieldHeight <= buttonHeight)
	{
		return;
	}

	std::uniform_real_distribution<float> randomX(
		fieldLeft,
		fieldLeft + fieldWidth - buttonWidth
	);
	std::uniform_real_distribution<float> randomY(
		fieldTop,
		fieldTop + fieldHeight - buttonHeight
	);

	float x = randomX(randomEngine);
	float y = randomY(randomEngine);

	auto button = std::make_unique<Actor>(overlayRenderTargetId);
	button->InitializeSprite(engine, spritePath, x, y, buttonWidth, buttonHeight);
	button->AddBoxCollider(0.0f, 0.0f, buttonWidth, buttonHeight);
	button->SetAlpha(0.0f);

	

	buttons.push_back(std::move(button));
}

void SpawnButtonManager::UpdateButtonVisibility(const WindowManager& windows)
{
	const GameWindow* playerWindow = windows.GetWindowById(playerRegionId);
	const OverlayWindow* overlay = windows.GetOverlayWindow();

	if (playerWindow == nullptr || overlay == nullptr)
	{
		return;
	}

	RECT playerClientRect{};
	if (!GetClientRect(playerWindow->GetHwnd(), &playerClientRect))
	{
		return;
	}

	float playerLeft = playerWindow->GetClientX() - static_cast<float>(overlay->GetX());
	float playerTop = playerWindow->GetClientY() - static_cast<float>(overlay->GetY());
	float playerRight = playerLeft + static_cast<float>(playerClientRect.right - playerClientRect.left);
	float playerBottom = playerTop + static_cast<float>(playerClientRect.bottom - playerClientRect.top);

	D2D1_RECT_F playerRect = D2D1::RectF(
		playerLeft,
		playerTop,
		playerRight,
		playerBottom
	);

	for (auto& button : buttons)
	{
		Transform transform = button->GetTransform();

		float buttonCenterX = transform.x + transform.width * 0.5f;
		float buttonCenterY = transform.y + transform.height * 0.5f;

		float distance = DistancePointToRect(buttonCenterX, buttonCenterY, playerRect);
		float alpha = 1.0f - distance / revealDistance;

		button->SetAlpha(Clamp01(alpha));
	}
}

float SpawnButtonManager::DistancePointToRect(float x, float y, const D2D1_RECT_F& rect) const
{
	float dx = 0.0f;
	float dy = 0.0f;

	if (x < rect.left) dx = rect.left - x;
	else if (x > rect.right) dx = x - rect.right;

	if (y < rect.top) dy = rect.top - y;
	else if (y > rect.bottom) dy = y - rect.bottom;

	return std::sqrt(dx * dx + dy * dy);
}

float SpawnButtonManager::Clamp01(float value) const
{
	if (value < 0.0f) return 0.0f;
	if (value > 1.0f) return 1.0f;
	return value;
}

bool IsRectIntersect(const D2D1_RECT_F& a, const D2D1_RECT_F& b)
{
	bool result = a.left < b.right && a.right > b.left && a.top < b.bottom && a.bottom > b.top;
	return result;
}

bool SpawnButtonManager::IsPlayerTouchingButton(
	const Actor& playerActor,
	const WindowManager& windows
) const
{
	if (!playerActor.HasBoxCollider())
	{
		return false;
	}

	D2D1_RECT_F playerRect = playerActor.GetColliderOverlayRect(windows);

	for (const auto& button : buttons)
	{
		if (!button->HasBoxCollider())
		{
			continue;
		}

		if (button->GetAlpha() < 0.0f)
		{
			continue;
		}

		D2D1_RECT_F buttonRect = button->GetColliderOverlayRect(windows);

		if (IsRectIntersect(playerRect, buttonRect))
		{
			return true;
		}
	}

	return false;
}