#pragma once

#include <windows.h>
#include <string>

class AudioManager
{
	
public://생성자, 소멸자
	AudioManager();
	~AudioManager();

public:
	//재생 및 중지함수
	bool PlayBGM(const std::wstring& filePath);
	void StopBGM();

	void PlaySFX(const std::wstring& filePath);

private:
	bool m_isBGMPlaying;
};