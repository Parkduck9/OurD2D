#pragma once

#include <Windows.h>
#include <d2d1.h>
#include <wincodec.h>
#include <wrl/client.h>

#include <unordered_map>

class D2DManager
{
public:
	bool Initialize();
	void Shutdown();

	HRESULT CreateRenderTargetForWindow(int windowId, HWND hwnd);
	void RemoveRenderTarget(int windowId);

	HRESULT CreateBitmapFromWicSource(
		int windowId,
		IWICBitmapSource* source,
		Microsoft::WRL::ComPtr<ID2D1Bitmap>& outBitmap
	);

	void BeginDraw(int windowId);
	void Clear(int windowId, const D2D1_COLOR_F& color);
	void DrawBitmap(
		int windowId,
		ID2D1Bitmap* bitmap,
		const D2D1_RECT_F& destRect, //그림을 화면에 그릴 영역
		float opacity = 1.0f//알파값
	);
	void DrawBitmapFrame(
		int windowId,
		ID2D1Bitmap* bitmap,
		const D2D1_RECT_F& destRect, //그림을 화면에 그릴 영역
		const D2D1_RECT_F& sourceRect,//원본 이미지에서 잘라낼 영역
		float opacity = 1.0f
	);
	void DrawRectangle(
		int windowId,
		const D2D1_RECT_F& rectm
	);
	
	HRESULT EndDraw(int windowId);

	void ResizeRenderTarget(int windowId, UINT width, UINT height);

private:
	struct WindowRenderData
	{
		HWND hwnd = nullptr;
		Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> renderTarget;
	};

	
private:
	Microsoft::WRL::ComPtr<ID2D1Factory> d2dFactory;
	std::unordered_map<int, WindowRenderData> windowRenderTargets;
};

