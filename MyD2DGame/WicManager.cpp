#include "WicManager.h"

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "windowscodecs.lib")

using Microsoft::WRL::ComPtr;

bool WicManager::Initialize()
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	if (SUCCEEDED(hr))
	{
		comInitialized = true;
	}
	else if (hr != RPC_E_CHANGED_MODE)
	{
		return false;
	}

	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&wicFactory)
	);

	return SUCCEEDED(hr);
}

void WicManager::Shutdown()
{
	wicFactory.Reset();

	if (comInitialized)
	{
		CoUninitialize();
		comInitialized = false;
	}
}

HRESULT WicManager::LoadImageSource(
	const std::wstring& filePath,
	ComPtr<IWICBitmapSource>& outSource
)
{
	outSource.Reset();

	if (wicFactory == nullptr) return E_FAIL;

	ComPtr<IWICBitmapDecoder> decoder;
	HRESULT hr = wicFactory->CreateDecoderFromFilename(
		filePath.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&decoder
	);

	if (FAILED(hr)) return hr;

	ComPtr<IWICBitmapFrameDecode> frame;
	hr = decoder->GetFrame(0, &frame);

	if (FAILED(hr)) return hr;

	ComPtr<IWICFormatConverter> converter;
	hr = wicFactory->CreateFormatConverter(&converter);

	if (FAILED(hr)) return hr;

	hr = converter->Initialize(
		frame.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0,
		WICBitmapPaletteTypeMedianCut
	);

	if (FAILED(hr)) return hr;

	hr = converter.As(&outSource);

	return hr;
}