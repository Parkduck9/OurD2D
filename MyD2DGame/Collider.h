#pragma once

#include <d2d1.h>

class Actor;

class BoxCollider
{
public:
	BoxCollider() = default;
	BoxCollider(Actor& actor);

	void SetOffset(float x, float y);
	void SetSize(float width, float height);

	//월드좌표(창좌표)기준으로 collider 재배치
	D2D1_RECT_F GetWorldRect(const Actor& actor) const;

	bool Intersects(Actor& myActor, Actor& otherActor);

private:
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	float width = 0.0f;
	float height = 0.0f;

	bool enabled = true;
};