#include "GameWindow.h"
#include "InputManager.h"


bool GameWindow::Create(
						HINSTANCE hInstance,
						const wchar_t* title,
						int	id,
						const WindowCreateInfo& info,
						InputManager* inputManager
)
{
	this->m_hInstance = hInstance;
	this->id = id;
	this->inputManager = inputManager;
	this->x = info.x;
	this->y = info.y;
	this->width = info.width;
	this->height = info.height;

	m_hwnd = CreateWindowEx(
							0,  title,info.title.c_str(),
							WS_POPUP | WS_BORDER | WS_DLGFRAME,
							info.x, info.y,
							info.width, info.height,
							nullptr, nullptr,
							hInstance, this
							);
	
	if (m_hwnd == nullptr) return false;
	
	ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);//생성된 창에 포커싱 안 뺏기게
	UpdateWindow(m_hwnd);

	return true;
}

//void GameWindow::Show(int nCmdShow)
//{
//	ShowWindow(m_hwnd, nCmdShow);
//	UpdateWindow(m_hwnd);
//}

HWND GameWindow::GetHwnd() const
{
	return m_hwnd;
}

int GameWindow::GetID() const
{
	return id;
}

LRESULT GameWindow::HandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_hwnd == nullptr)
	{
		m_hwnd = hwnd;
	}

	switch (message)
	{
	case WM_NCCREATE:
		return TRUE;

	case WM_SETFOCUS:
		if (inputManager != nullptr)
		{
			inputManager->SetFocusedWindowId(id);
		}
		return 0;

	case WM_KEYDOWN:
		if (inputManager != nullptr)
		{
			inputManager->SetKeyDown(id, static_cast<int>(wParam), true);
		}
		return 0;

	case WM_KEYUP:
		if (inputManager != nullptr)
		{
			inputManager->SetKeyDown(id, static_cast<int>(wParam), false);
		}
		return 0;

	case WM_MOVE:
		if (!isMovingByCode)
		{
			UpdateRect();
		}
		
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProcW(hwnd, message, wParam, lParam);
	}
}

void GameWindow::SetPosition(float x, float y)
{

}
void GameWindow::MoveWindow(float XRatio, float YRatio, float deltaTime)
{

	HMONITOR hMonitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);

	MONITORINFO mi = {};
	mi.cbSize = sizeof(MONITORINFO);

	if (!GetMonitorInfo(hMonitor, &mi)) return;


	RECT work = mi.rcWork; //툴바 작업 표시줄 등등 제외한 영역

	//모니터 (작업공간의 너비 높이) 구하기
	int workWidth = work.right - work.left;
	int workHeight = work.bottom - work.top;




	float locationWidth =(workWidth * XRatio);// - width / 2;
	float locationHeight =(workHeight * YRatio);// - height / 2;
	x += locationWidth * deltaTime;
	y += locationHeight* deltaTime;

	isMovingByCode = true;
	SetWindowPos(m_hwnd, nullptr, static_cast<int>(x), static_cast<int>(y), 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
	isMovingByCode = false;
}
void GameWindow::ResizeWindowToMonitorRatio(HWND hwnd, double widthRatio, double heightRatio, double XRatio, double YRatio)
{
	HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

	MONITORINFO mi = {};
	mi.cbSize = sizeof(MONITORINFO);

	if (!GetMonitorInfo(hMonitor, &mi)) return;


	RECT work = mi.rcWork; //툴바 작업 표시줄 등등 제외한 영역

	//모니터 (작업공간의 너비 높이) 구하기
	int workWidth  = work.right  - work.left;
	int workHeight = work.bottom - work.top ;

	//모니터 작업공간의 (창의 너비, 높이) 구하기
	int targetWidth = static_cast<int>(workWidth * widthRatio);
	int targetHeight = static_cast<int>(workHeight * heightRatio);


	int locationWidth = static_cast<int>(workWidth * XRatio) - targetWidth / 2;
	int locationHeigh = static_cast<int>(workHeight * YRatio) - targetHeight / 2;
	

	SetWindowPos(hwnd, nullptr, locationWidth, locationHeigh, targetWidth, targetHeight, SWP_NOZORDER | SWP_NOACTIVATE);
}

void GameWindow::UpdateRect()
{
	RECT rect = {};

	if (GetWindowRect(m_hwnd, &rect))
	{
		x = rect.left;
		y = rect.top;
	}
}