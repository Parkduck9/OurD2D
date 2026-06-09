#pragma once

#include<d2d1.h>
#include<wrl/client.h>

class D2DManager;

struct Transform//액터의 기본 위치와 크기
{
	float x = 0.0f;
	float y = 0.0f;
	float width = 0.0f;
	float height = 0.0f;
};

class Actor
{
public:
	Actor(int windowId);//생성할때 창에 귀속되게

	void SetWindowId(int windowId);
	int  GetWindowId() const;

	void SetPosition(float x, float y);
	void SetSize	(float width, float height);
	void Move		(float x, float y);

	void SetBitmap  (const Microsoft::WRL::ComPtr<ID2D1Bitmap>& bitmap);
	void ResetBitmap();

	void Update		(float deltaTime);
	void Render(D2DManager& d2d) const;

private:
	D2D1_RECT_F GetDestinationRect() const;

private:
	int windowId;
	Transform transform;

	Microsoft::WRL::ComPtr<ID2D1Bitmap> bitmap;
};