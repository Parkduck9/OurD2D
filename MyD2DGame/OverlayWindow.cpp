#include "OverlayWindow.h"


OverlayWindow::~OverlayWindow()
{
	ReleaseAlphaBuffer();
}
bool OverlayWindow::Create(HINSTANCE hInstance)
{
	const wchar_t* className = L"OverlayWindowClass";

	WNDCLASSW wc{};
	wc.lpfnWndProc = OverlayWindow::WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = className;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	if (RegisterClassW(&wc) == 0 && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
	{
		return false;
	}

	//그냥 시스템상 주모니터에서 가져옴
	x = 0;
	y = 0;
	width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	//투명 창 서술
	DWORD exStyle =
		WS_EX_LAYERED     | //레이어드 윈도우, 투명같은 요소를 다루기 위해 사용됨
		WS_EX_TRANSPARENT | //아래쪽 형제창들을 먼저 그리게 하는 스타일
		WS_EX_TOPMOST     | //창을 항상 맨 위에 두는 스타일
		WS_EX_NOACTIVATE  | //창을 클릭하거나 표시해도 활성화되지 않게 하는 스타일
		WS_EX_TOOLWINDOW  ; //작업표시줄 등 사용자가 UI에서 발견하지 못 하도록 하는 스타일

	hwnd = CreateWindowExW(
		exStyle,
		className,
		L"OverlayWindow",
		WS_POPUP,
		x, y,
		width, height,
		nullptr, nullptr,
		hInstance, nullptr
	);

	if (hwnd == nullptr)
	{
		return false;
	}

	if (!CreateAlphaBuffer())
	{
		DestroyWindow(hwnd);
		hwnd = nullptr;
		return false;
	}
	Present();

	//SetLayeredWindowAttributes(hwnd, TransparentColor, 0, LWA_COLORKEY);
	ShowWindow(hwnd, SW_SHOWNOACTIVATE);
	UpdateWindow(hwnd);

	return true;


}


//Get함수들
HWND OverlayWindow::GetHwnd  () const { return hwnd  ; }
int  OverlayWindow::GetX     () const { return x     ; }
int  OverlayWindow::GetY     () const { return y     ; }
int  OverlayWindow::GetWidth () const { return width ; }
int  OverlayWindow::GetHeight() const { return height; }

LRESULT CALLBACK OverlayWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCHITTEST)
	{
		return HTTRANSPARENT;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}



HDC OverlayWindow::GetMemoryDC() const
{
	return memoryDc;
}

bool OverlayWindow::CreateAlphaBuffer()
{
	ReleaseAlphaBuffer();

	BITMAPINFO bmi{};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	HDC screenDc = GetDC(nullptr);
	if (screenDc == nullptr) return false;

	memoryDc = CreateCompatibleDC(screenDc);
	bitmap = CreateDIBSection(screenDc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);

	ReleaseDC(nullptr, screenDc);

	if (memoryDc == nullptr || bitmap == nullptr || bits == nullptr)
	{
		ReleaseAlphaBuffer();
		return false;
	}

	oldBitmap = static_cast<HBITMAP>(SelectObject(memoryDc, bitmap));
	ZeroMemory(bits, width * height * 4);

	return true;
}

void OverlayWindow::ReleaseAlphaBuffer()
{
	if (memoryDc != nullptr && oldBitmap != nullptr)
	{
		SelectObject(memoryDc, oldBitmap);
		oldBitmap = nullptr;
	}

	if (bitmap != nullptr)
	{
		DeleteObject(bitmap);
		bitmap = nullptr;
	}

	if (memoryDc != nullptr)
	{
		DeleteDC(memoryDc);
		memoryDc = nullptr;
	}

	bits = nullptr;
}

void OverlayWindow::Present()
{
	if (hwnd == nullptr || memoryDc == nullptr) return;

	HDC screenDc = GetDC(nullptr);
	if (screenDc == nullptr) return;

	POINT srcPoint{ 0, 0 };
	POINT dstPoint{ x, y };
	SIZE size{ width, height };

	BLENDFUNCTION blend{};
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;
	blend.AlphaFormat = AC_SRC_ALPHA;

	UpdateLayeredWindow(
		hwnd,
		screenDc,
		&dstPoint,
		&size,
		memoryDc,
		&srcPoint,
		0,
		&blend,
		ULW_ALPHA
	);

	ReleaseDC(nullptr, screenDc);
}