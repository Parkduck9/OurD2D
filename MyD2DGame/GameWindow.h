#pragma once
#include <Windows.h>
//김성영의 원맨쇼
class GameWindow
{
public:
	GameWindow();
	~GameWindow();

	bool Create(
		HINSTANCE hInstance,  // 창의 핸들
		const wchar_t* title, // 창의 이름
		int x    , int y    , // 창의 좌표
		int width, int height // 창의 크기
	);

	void Show(int nCmdShow);
	HWND GetHwnd() const;

	void ResizeWindowToMonitorRatio(HWND hwnd, double widthRatio, double heightRaio, double XRatio, double YRatio, bool flag);
private:
	HWND m_hwnd;
	HINSTANCE m_hInstance;

private:
	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM IParam);
		   LRESULT			WndProc		 (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


	
};