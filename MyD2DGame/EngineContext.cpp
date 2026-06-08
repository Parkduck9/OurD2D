#include "EngineContext.h"
#include "WindowManager.h"
//이준이가 쓸 엔진 기능 모아둔 파일
//지금은 윈도우 매니저 하나만 있는데
//나중에는 인풋이나 비트맵같은거 넣어서 기능 쓰게 할 예정


EngineContext::EngineContext(WindowManager& windowManager, InputManager& inputManager) : windowManager(windowManager), inputManager(inputManager)
{

}

WindowManager& EngineContext::GetWindowManager()
{
	return windowManager;
}

InputManager& EngineContext::GetInputManager()
{
	return inputManager;
}