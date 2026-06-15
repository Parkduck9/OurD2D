#include "D2DManager.h"

#pragma comment(lib, "d2d1.lib")

using Microsoft::WRL::ComPtr;

bool D2DManager::Initialize()
{
	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		d2dFactory.ReleaseAndGetAddressOf()
	);

	return SUCCEEDED(hr);
}

void D2DManager::Shutdown()
{
	windowRenderTargets.clear();
	d2dFactory.Reset();
}

void D2DManager::GetTransform(int windowId, D2D1_MATRIX_3X2_F& outTransform)
{
	auto iter = windowRenderTargets.find(windowId);
	if (iter == windowRenderTargets.end()) return; 

	iter->second.renderTarget->GetTransform(&outTransform);
}

void D2DManager::SetTransform(int windowId, const D2D1_MATRIX_3X2_F& transform)
{
	auto iter = windowRenderTargets.find(windowId);
	if (iter == windowRenderTargets.end()) return;

	iter->second.renderTarget->SetTransform(transform);
}

//렌더타겟 생성
HRESULT D2DManager::CreateRenderTargetForWindow(int windowId, HWND hwnd)
{
	if (d2dFactory == nullptr || hwnd == nullptr)
	{
		return E_FAIL;
	}

	RECT clientRect{};
	GetClientRect(hwnd, &clientRect);

	D2D1_SIZE_U size = D2D1::SizeU(
		clientRect.right - clientRect.left,
		clientRect.bottom - clientRect.top
	);

	ComPtr<ID2D1HwndRenderTarget> renderTarget;

	HRESULT hr = d2dFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hwnd, size),
		&renderTarget
	);

	if (FAILED(hr))
	{
		return hr;
	}

	WindowRenderData data;
	data.hwnd = hwnd;
	data.hwndRenderTarget = renderTarget;
	data.renderTarget = renderTarget;

	windowRenderTargets[windowId] = data;

	return S_OK;
}

//WIC 이미지 소스를 D2D 비트맵으로 변환
HRESULT D2DManager::CreateBitmapFromWicSource(
	int windowId,
	IWICBitmapSource* source,
	ComPtr<ID2D1Bitmap>& outBitmap
)
{
	outBitmap.Reset();

	auto iter = windowRenderTargets.find(windowId);
	if (iter == windowRenderTargets.end())
	{
		return E_FAIL;
	}

	if (source == nullptr)
	{
		return E_INVALIDARG;
	}

	return iter->second.renderTarget->CreateBitmapFromWicBitmap(
		source,
		nullptr,
		&outBitmap
	);
}

//그리기 함수들
void D2DManager::BeginDraw(int windowId)
{
	auto iter = windowRenderTargets.find(windowId);
	if (iter == windowRenderTargets.end()) return;

	iter->second.renderTarget->BeginDraw();
}

void D2DManager::DrawRectangle(
	int windowId,
	const D2D1_RECT_F& rect
)
{
	auto iter = windowRenderTargets.find(windowId);
	if (iter == windowRenderTargets.end()) return;

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
	HRESULT hr = iter->second.renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &brush);
	if (FAILED(hr)) return;

	iter->second.renderTarget->DrawRectangle(rect, brush.Get(), 3.0f);
}

void D2DManager::Clear(int windowId, const D2D1_COLOR_F& color)
{
	auto iter = windowRenderTargets.find(windowId);
	if (iter == windowRenderTargets.end()) return;

	iter->second.renderTarget->Clear(color);
}
void D2DManager::DrawBitmap(
	int windowId,
	ID2D1Bitmap* bitmap,
	const D2D1_RECT_F& destRect,
	float opacity
)
{
	auto iter = windowRenderTargets.find(windowId);
	if (iter == windowRenderTargets.end()) return;
	if (bitmap == nullptr) return;

	iter->second.renderTarget->DrawBitmap(
		bitmap, &destRect, opacity
	);
}


void D2DManager::DrawBitmapFrame(
	int windowId,
	ID2D1Bitmap* bitmap,
	const D2D1_RECT_F& destRect,
	const D2D1_RECT_F& sourceRect,
	float opacity
)
{
	auto iter = windowRenderTargets.find(windowId);
	if (iter == windowRenderTargets.end()) return;
	if (bitmap == nullptr) return;

	iter->second.renderTarget->DrawBitmap(
		bitmap, &destRect,
		opacity,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		&sourceRect
		);
}

HRESULT D2DManager::EndDraw(int windowId)
{
	auto iter = windowRenderTargets.find(windowId);
	if (iter == windowRenderTargets.end())
	{
		return E_FAIL;
	}

	return iter->second.renderTarget->EndDraw();
}

//삭제, 리사이즈
void D2DManager::RemoveRenderTarget(int windowId)
{
	windowRenderTargets.erase(windowId);
}

void D2DManager::ResizeRenderTarget(int windowId, UINT width, UINT height)
{
	auto iter = windowRenderTargets.find(windowId);
	if (iter == windowRenderTargets.end()) return;

	iter->second.hwndRenderTarget->Resize(D2D1::SizeU(width, height));
}

HRESULT D2DManager::CreateRenderTargetForOverlayDC(int windowId, HDC hdc, int width, int height)
{
	if (d2dFactory == nullptr || hdc == nullptr)
	{
		return E_FAIL;
	}

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_PREMULTIPLIED
			)
		);

	Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> dcRenderTarget;
	HRESULT hr = d2dFactory->CreateDCRenderTarget(&props, &dcRenderTarget);
	if (FAILED(hr)) return hr;

	RECT rect{ 0, 0, width, height };
	hr = dcRenderTarget->BindDC(hdc, &rect);
	if (FAILED(hr)) return hr;

	WindowRenderData data;
	data.hdc = hdc;
	data.dcRenderTarget = dcRenderTarget;
	data.renderTarget = dcRenderTarget;

	windowRenderTargets[windowId] = data;

	return S_OK;
}