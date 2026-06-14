#include "Collider.h"
#include "Actor.h"

BoxCollider::BoxCollider(Actor& actor)
{
	offsetX = 0.0f;
	offsetY = 0.0f;
	width = actor.GetTransform().width;
	height = actor.GetTransform().height;
}

//액터의 자식같은 개념이여서 액터의 좌상단이 기준임
void BoxCollider::SetOffset(float x, float y)
{
	offsetX = x;
	offsetY = y;
}
void BoxCollider::SetSize(float width, float height)
{
	this->width = width;
	this->height = height;
}

D2D1_RECT_F BoxCollider::GetWorldRect(const Actor& actor) const
{
	return D2D1::RectF(
		actor.GetTransform().x + offsetX,
		actor.GetTransform().y + offsetY,
		actor.GetTransform().x + offsetX + width,
		actor.GetTransform().y + offsetY + height
	);
}

//충돌체크
bool BoxCollider::Intersects(Actor& myActor, Actor& otherActor)
{
	D2D1_RECT_F a = GetWorldRect(myActor);
	D2D1_RECT_F b = otherActor.GetBoxCollider().GetWorldRect(otherActor);//스으으으으읍.....참......쉽지않다....

	bool result = false;
	result = (a.left < b.right) && (a.right > b.left) && (a.top < b.bottom) && (a.bottom > b.top);
	
	return result;
}