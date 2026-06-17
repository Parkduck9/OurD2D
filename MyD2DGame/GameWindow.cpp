#pragma once
#include "GameWindow.h"
#include "InputManager.h"
#include <cmath>

bool GameWindow::Create(HINSTANCE hInstance,
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
	this->closeAction = info.closeAction;
	m_hwnd = CreateWindowEx(
							0,  title,info.title.c_str(),
							WS_POPUP | WS_CAPTION | WS_DLGFRAME,
							info.x, info.y,
							info.width, info.height,
							nullptr, nullptr,
							hInstance, this
							);
	
	if (m_hwnd == nullptr) return false;
	
	if (info.focusOnCreate == true)
	{
		ShowWindow(m_hwnd, SW_SHOW);
		SetForegroundWindow(m_hwnd);
		SetFocus(m_hwnd);
	}
	else
	{
		ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);//생성된 창에 포커싱 안 뺏기게
	}

	
	UpdateWindow(m_hwnd);
	RefreshWindowCacheFromHwnd();

	return true;
}

void GameWindow::DestroyWin()
{
	DestroyWindow(m_hwnd);
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

float GameWindow::GetX() const
{
	return x;
}

float GameWindow::GetY() const
{
	return y;
}

float GameWindow::GetWidth() const
{
	return width;
}

float GameWindow::GetHeight() const
{
	return height;
}

float GameWindow::GetClientX() const
{
	return clientX;
}

float GameWindow::GetClientY() const
{
	return clientY;
}

void GameWindow::RefreshWindowCacheFromHwnd()
{
	RECT rect{};
	if (!GetWindowRect(m_hwnd, &rect))
	{
		return;
	}

	POINT clientOrigin{ 0,0 };
	ClientToScreen(m_hwnd, &clientOrigin);

	x = static_cast<float>(rect.left);
	y= static_cast<float>(rect.top);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	clientOffsetX = static_cast<float>(clientOrigin.x - rect.left);
	clientOffsetY = static_cast<float>(clientOrigin.y - rect.top);

	clientX = static_cast<float>(clientOrigin.x);
	clientY = static_cast<float>(clientOrigin.y);
}

void GameWindow::UpdateClientCacheFromStoreRect() {
	clientX = x + clientOffsetX;
	clientY = y + clientOffsetY;
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

	case WM_MOVE://플레이어가 직접 움직일때에만 좌표갱신
		if (!isMovingByCode)
		{
			//UpdateRect();
			RefreshWindowCacheFromHwnd();
		}
		
		return 0;

	case WM_DESTROY:
		if (closeAction == WindowCloseAction::QuitApp)
		{
			PostQuitMessage(0);
		}
		return 0;

	case WM_CLOSE:
		switch (closeAction)
		{
		case WindowCloseAction::None:
			return 0;
		case WindowCloseAction::Hide:
			ShowWindow(m_hwnd, SW_HIDE);
			return 0;
		case WindowCloseAction::Destory:
		case WindowCloseAction::QuitApp:
			DestroyWindow(m_hwnd);
			return 0;
		}
		return 0;
	default:
		return DefWindowProcW(hwnd, message, wParam, lParam);
	}
}

void GameWindow::ReSizeWindow(float WidthRatio, float HeightRatio)
{
	HMONITOR hMonitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);

	MONITORINFO mi = {};
	mi.cbSize = sizeof(MONITORINFO);

	if (!GetMonitorInfo(hMonitor, &mi)) return;


	RECT work = mi.rcWork; //툴바 작업 표시줄 등등 제외한 영역

	//모니터 (작업공간의 너비 높이) 구하기
	double workWidth = work.right - work.left;
	double workHeight = work.bottom - work.top;

	//모니터 작업공간의 (창의 너비, 높이) 구하기
	double targetWidth = workWidth * WidthRatio;
	double targetHeight = workHeight * HeightRatio;

	x = x + width / 2.0;
	y = y + height / 2.0;

	x = x - targetWidth / 2.0;
	y = y - targetHeight / 2.0;


	SetWindowPos(m_hwnd, nullptr, static_cast<int>(std::round(x)), static_cast<int>(std::round(y)), static_cast<int>(std::round(targetWidth)), static_cast<int>(std::round(targetHeight)), SWP_NOZORDER | SWP_NOACTIVATE);

}

void GameWindow::MoveWindow(float XRatio, float YRatio, float Speed, float deltaTime)
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
	//x += locationWidth * Speed*deltaTime;
	//y += locationHeight* Speed * deltaTime;

	//isMovingByCode = true;
	//SetWindowPos(m_hwnd, nullptr, static_cast<int>(x), static_cast<int>(y), 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
	//isMovingByCode = false;
	float nextX = x + locationWidth * Speed * deltaTime;
	float nextY = y + locationHeight * Speed * deltaTime;

	isMovingByCode = true;

	if (SetWindowPos(
		m_hwnd,
		nullptr,
		static_cast<int>(nextX),
		static_cast<int>(nextY),
		0,
		0,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE))
	{
		x = nextX;
		y = nextY;
		UpdateClientCacheFromStoreRect();

	}

	isMovingByCode = false;

}

void GameWindow::MoveWindowByPixels(float offsetX, float offsetY)
{
	float nextX = x + offsetX;
	float nextY = y + offsetY;

	isMovingByCode = true;

	if (SetWindowPos(
		m_hwnd,
		nullptr,
		static_cast<int>(std::round(nextX)),
		static_cast<int>(std::round(nextY)),
		0,
		0,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE))
	{
		x = nextX;
		y = nextY;
		UpdateClientCacheFromStoreRect();
	}

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
	RefreshWindowCacheFromHwnd();
}

void GameWindow::UpdateRect()
{
	RECT rect = {};

	if (GetWindowRect(m_hwnd, &rect))
	{
		x = rect.left;
		y = rect.top;

		width  = rect.right  -  rect.left;
		height = rect.bottom -  rect.top;
	}
}