#pragma once

#include<d2d1.h>
#include<wrl/client.h>
#include "SpriteAnimation.h"
#include<string>
#include<unordered_map>

class EngineContext;
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

	bool InitializeSprite(EngineContext& engine, const std::wstring& filePath, float x, float y, float width, float height);

	void AddAnimation(const std::wstring& name, int frameWidth, int frameHeight, int frameCount, int columns, float framesPerSecond);
	void PlayAnimation(const std::wstring& name);

	void Update		(float deltaTime);
	void Render(D2DManager& d2d) const;

private:
	D2D1_RECT_F GetDestinationRect() const;

private:
	int windowId;
	Transform transform;

	Microsoft::WRL::ComPtr<ID2D1Bitmap> bitmap;

	bool hasAnimation = false;
	SpriteAnimation animation;

	//여러개의 애니메이션 사용하려면
	std::unordered_map<std::wstring, SpriteAnimation> animations;
	SpriteAnimation* currentAnimation = nullptr;
};