#pragma once
#include <Windows.h>
#include <string>

class InputManager;

//윈도우창 설계정보(이거 기반으로 생성할 수 있게)
struct WindowCreateInfo
{
	std::wstring title = L"Default Window";

	int x	   = 100; //x좌표
	int y	   = 100; //y좌표
	int width  = 800; //높이
	int height = 600; //너비
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

	LRESULT HandleMessage(HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam);

	//void ResizeWindowToMonitorRatio(HWND hwnd, double widthRatio, double heightRaio, double XRatio, double YRatio, bool flag);


private:
	HWND m_hwnd=nullptr;
	HINSTANCE m_hInstance;
	int id = -1;
	InputManager* inputManager = nullptr;

//
//private:
//	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM IParam);
//		   LRESULT			WndProc		 (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//

	
};