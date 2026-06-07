#pragma once
//실제로 콘텐츠 개발 때 사용할 인터페이스 클래스, 교수님 거랑 비슷한 개념이라고 보면 돼
class EngineContext; //엔진기능 뽑아쓰는 클래스

class Content
{
public:
	virtual ~Content() = default;

	virtual void OnStart(EngineContext& engine) {}
	virtual void OnUpdate(EngineContext& engine, float deltaTime) {}
	virtual void OnEnd(EngineContext& engine) {}
};