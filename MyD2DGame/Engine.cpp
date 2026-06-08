#include "Engine.h"
#include "GameContent.h"
#include <chrono>






bool Engine::Initialize(HINSTANCE hInstance)
{
	if (!windowManager.Initialize(hInstance, inputManager))
	{
		return false;
	}

	context = std::make_unique<EngineContext>(windowManager, inputManager);

	content = std::make_unique<GameContent>();
	content->OnStart(*context);

	return true;
}

void Engine::Run()
{
	MSG msg{};

	auto previousTime = std::chrono::high_resolution_clock::now();

	while (isRunning)
	{
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				isRunning = false;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed = currentTime - previousTime;
		previousTime = currentTime;

		float deltaTime = elapsed.count();

		Update(deltaTime);
		Render();

		inputManager.EndFrame();
	}
}
void Engine::Update(float deltaTime)
{
	if (content != nullptr && context != nullptr)
	{
		content->OnUpdate(*context, deltaTime);
	}
}
void Engine::Shutdown()
{
	if (content != nullptr && context != nullptr)
	{
		content->OnEnd(*context);
	}

	content.reset();
	context.reset();
}



void Engine::Render()
{

}
