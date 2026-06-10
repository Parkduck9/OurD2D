#pragma once

#include <Windows.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <string>

class WicManager
{
public:
	bool Initialize();
	void Shutdown();

	HRESULT LoadImageSource(
		const std::wstring& filePath,
		Microsoft::WRL::ComPtr<IWICBitmapSource>& outSource
	);

private:
	bool comInitialized = false;
	Microsoft::WRL::ComPtr<IWICImagingFactory> wicFactory;
};