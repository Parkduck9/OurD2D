#pragma once

#include <d2d1.h>

class SpriteAnimation
{
public:
	void Initialize(int frameWidth, int frameHeight, int frameCount, int columns, float framesPerSecond)
	{
		this->frameWidth = frameWidth;
		this->frameHeight = frameHeight;
		this->frameCount = frameCount;
		this->columns = columns;
		this->frameDuration = 1.0f / framesPerSecond;

		currentFrame = 0;
		elapsedTime = 0.0f;
	}

	void Update(float deltaTime)
	{
		if (frameCount <= 0 || frameDuration <= 0.0f || columns <=0) return;

		elapsedTime += deltaTime;

		while (elapsedTime >= frameDuration)
		{
			elapsedTime -= frameDuration;
			currentFrame++;

			if (currentFrame >= frameCount) currentFrame = 0;
		}
	}

	D2D1_RECT_F GetSourceRect() const
	{
		int column = currentFrame % columns;
		int row = currentFrame / columns;

		float left = static_cast<float>(column * frameWidth);
		float top = static_cast<float>(row * frameHeight);
		float right = left + static_cast<float>(frameWidth);
		float bottom = top + static_cast<float>(frameHeight);

		return D2D1::RectF(left, top, right, bottom);
	}

	int GetCurrentFrame() const
	{
		return currentFrame;
	}



private:
	int frameWidth   = 0;
	int frameHeight  = 0;
	int frameCount   = 0;
	int columns		 = 1;
	int currentFrame = 0;

	float frameDuration = 0.0f;
	float elapsedTime   = 0.0f;
};