#pragma once
#include <Windows.h>
#include <string>

class InputManager;


//창 상태 관리
enum class WindowCloseAction
{
	None,
	Hide,
	Destory,
	QuitApp
};



//윈도우창 설계정보(이거 기반으로 생성할 수 있게)
struct WindowCreateInfo
{

	WindowCreateInfo(std::wstring title, float XRatio, float YRatio, float WidthRatio, float HeightRatio, bool focusOnCreate, WindowCloseAction closeAction = WindowCloseAction::Destory)
	{
		this->title = title;
		RatioToPoint(XRatio, YRatio, WidthRatio, HeightRatio);
		this->focusOnCreate = focusOnCreate;
		this->closeAction = closeAction;
	}

	bool focusOnCreate = false;//창생성시 포커싱여부

	std::wstring title = L"Default Window";

	float x	   = 100; //x좌표
	float y	   = 100; //y좌표
	int width  = 800; //높이
	int height = 600; //너비

	WindowCloseAction closeAction = WindowCloseAction::Destory;

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


		x = work.left + static_cast<int>(workWidth * XRatio)- width / 2;
		y = work.top + static_cast<int>(workHeight * YRatio)- height / 2;

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
	void DestroyWin();

	HWND  GetHwnd()    const;
	int   GetID()      const;
	float GetX()	   const;
	float GetY()	   const;
	float GetWidth()   const;
	float GetHeight()  const;
	float GetClientX() const;//이미지 밀리는 문제 때문에 추가
	float GetClientY() const;

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

	float clientX = 0.0f;
	float clientY = 0.0f;
	float clientOffsetX = 0.0f;
	float clientOffsetY = 0.0f;

	void RefreshWindowCacheFromHwnd();
	void UpdateClientCacheFromStoreRect();

	WindowCloseAction closeAction = WindowCloseAction::Destory;

	bool isMovingByCode = false;
	InputManager* inputManager = nullptr;

//
//private:
//	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM IParam);
//		   LRESULT			WndProc		 (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//

	
};