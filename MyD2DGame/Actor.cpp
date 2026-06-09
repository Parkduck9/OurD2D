#include "Actor.h"
#include "D2DManager.h"

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
}

void Actor::Move(float x, float y)
{
	transform.x += x;
	transform.y += y;
}

void Actor::SetBitmap(const Microsoft::WRL::ComPtr<ID2D1Bitmap>& bitmap)
{
	this->bitmap = bitmap;
}

void Actor::ResetBitmap()
{
	bitmap.Reset();
}

void Actor::Update(float deltaTime)
{
	//나중에 애니메이션같은거 추가할때 코드 추가할것같음!
}

void Actor::Render(D2DManager& d2d) const
{
	if (windowId == NULL || bitmap == nullptr)
	{
		return;
	}

	D2D1_RECT_F destinationRect = GetDestinationRect();

	d2d.DrawBitmap(windowId, bitmap.Get(), destinationRect);
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