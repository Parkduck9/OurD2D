#include "Actor.h"
#include "D2DManager.h"
#include "EngineContext.h"
#include "WicManager.h"


Actor::Actor(int windowId) : windowId(windowId) {}

void Actor::SetWindowId(int windowId)
{
	this->windowId = windowId;
}

int Actor::GetWindowId() const
{
	return windowId;
}

void Actor::SetPosition(float x, float y)
{
	transform.x = x;
	transform.y = y;
}

void Actor::SetSize(float width, float height)
{
	transform.width = width;
	transform.height = height;

	collider.SetSize(width, height);
}

void Actor::Move(float x, float y)
{
	transform.x += x;
	transform.y += y;
}

Transform Actor::GetTransform()
{
	return transform;
}

BoxCollider& Actor::GetBoxCollider()
{
	return collider;
}
const BoxCollider& Actor::GetBoxCollider() const
{
	return collider;
}

void Actor::AddBoxCollider(float offsetX, float offsetY, float width, float height)
{
	collider.SetOffset(offsetX, offsetY);
	collider.SetSize(width, height);
	hasCollider = true;
}
bool Actor::HasBoxCollider() const
{
	return hasCollider;
}

void Actor::SetBitmap(const Microsoft::WRL::ComPtr<ID2D1Bitmap>& bitmap)
{
	this->bitmap = bitmap;
}

void Actor::ResetBitmap()
{
	bitmap.Reset();
}

bool Actor::InitializeSprite(EngineContext& engine, const std::wstring& filePath, float x, float y, float width, float height)
{
	SetPosition(x, y);
	SetSize(width, height);

	Microsoft::WRL::ComPtr<IWICBitmapSource> source;

	HRESULT hr = engine.GetWicManager().LoadImageSource(filePath, source);
	if (FAILED(hr)) return false;

	hr = engine.GetD2DManager().CreateBitmapFromWicSource(windowId, source.Get(), bitmap);

	return SUCCEEDED(hr);
}

void Actor::AddAnimation(const std::wstring& name, int frameWidth, int frameHeight, int frameCount, int columns, float framesPerSecond)
{
	SpriteAnimation animation;
	animation.Initialize(frameWidth, frameHeight, frameCount, columns, framesPerSecond);

	animations[name] = animation;

	if (currentAnimation == nullptr)
	{
		currentAnimation = &animations[name];
	}

	hasAnimation = true;
}
void Actor::PlayAnimation(const std::wstring & name)
{
	auto iter = animations.find(name);
	if (iter == animations.end())
	{
		return;
	}
	if (currentAnimation == &iter->second)
	{
		return;
	}

	currentAnimation = &iter->second;
}

void Actor::Update(float deltaTime)
{
	if (currentAnimation != nullptr)
	{
		currentAnimation->Update(deltaTime);
	}
}

void Actor::Render(D2DManager& d2d) const
{
	if (windowId == NULL || bitmap == nullptr)
	{
		return;
	}

	D2D1_RECT_F destinationRect = GetDestinationRect();



	if (currentAnimation != nullptr)//애니메이션 있을 때
	{
		d2d.DrawBitmapFrame(windowId, bitmap.Get(), destinationRect,currentAnimation->GetSourceRect());
	}
	else
	{
		d2d.DrawBitmap(windowId, bitmap.Get(), destinationRect);
	}
}

D2D1_RECT_F Actor::GetDestinationRect() const
{
	return D2D1::RectF(
		transform.x,
		transform.y,
		transform.x + transform.width,
		transform.y + transform.height
	);
}