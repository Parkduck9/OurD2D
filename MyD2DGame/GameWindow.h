#pragma once
#include <Windows.h>
#include <string>

class InputManager;

//윈도우창 설계정보(이거 기반으로 생성할 수 있게)
struct WindowCreateInfo
{

	WindowCreateInfo(std::wstring title, float XRatio, float YRatio, float WidthRatio, float HeightRatio)
	{
		this->title = title;
		RatioToPoint(XRatio, YRatio, WidthRatio, HeightRatio);
	}


	std::wstring title = L"Default Window";

	float x	   = 100; //x좌표
	float y	   = 100; //y좌표
	int width  = 800; //높이
	int height = 600; //너비


	void RatioToPoint(float XRatio, float YRatio, float WidthRatio, float HeightRatio)
	{
		HMONITOR hMonitor = MonitorFromPoint({ 0,0 }, MONITOR_DEFAULTTONEAREST);//주모니터

		MONITORINFO mi = {};
		mi.cbSize = sizeof(MONITORINFO);

		if (!GetMonitorInfo(hMonitor, &mi)) return;

		RECT work = mi.rcWork; //툴바 작업 표시줄 등등 제외한 영역

		//모니터 (작업공간의 너비 높이) 구하기
		int workWidth = work.right - work.left;
		int workHeight = work.bottom - work.top;

		//모니터 작업공간의 (창의 너비, 높이) 구하기
		width = static_cast<int>(workWidth * WidthRatio);
		height = static_cast<int>(workHeight * HeightRatio);


		x = static_cast<int>(workWidth * XRatio)- width / 2;
		y = static_cast<int>(workHeight * YRatio)- height / 2;

	}
};



class GameWindow
{
public:
	GameWindow() = default;

	bool Create(
		HINSTANCE hInstance,		 // 창의 핸들
		const wchar_t* title,		 // 창의 이름
		int id,						 // 창의 아이디
		const WindowCreateInfo& info, // 창 정보 구조체
		InputManager* inputManager
	);
	HWND GetHwnd() const;
	int  GetID()   const;
	void UpdateRect();

	LRESULT HandleMessage(HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam);

	void ReSizeWindow(float WidthRatio , float HeightRatio );
	void MoveWindow	(float XRatio, float YRatio, float Speed, float deltaTime);


	void ResizeWindowToMonitorRatio(HWND hwnd, double widthRatio, double heightRaio, double XRatio, double YRatio);


private:
	HWND	  m_hwnd=nullptr;
	HINSTANCE m_hInstance;


	int id = -1;
	float x = 0;//현재창의 X좌표
	float y = 0;//현재창의 Y좌표
	int width = 0;
	int height = 0;

	bool isMovingByCode = false;
	InputManager* inputManager = nullptr;

//
//private:
//	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM IParam);
//		   LRESULT			WndProc		 (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//

	
};