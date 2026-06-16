#include "AudioManager.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

AudioManager::AudioManager() :m_isBGMPlaying(false)
{

}

AudioManager::~AudioManager()
{
	StopBGM();
}

bool AudioManager::PlayBGM(const std::wstring& filePath)
{
	StopBGM();

	std::wstring openCommand = L"open \"" + filePath + L"\" type waveaudio alias bgm";

	MCIERROR openResult = mciSendString(
		openCommand.c_str(),
		nullptr,
		0,
		nullptr
	);

	if (openResult != 0)
	{
		m_isBGMPlaying = false;
		return false;
	}

	MCIERROR playResult = mciSendString(
		L"play bgm repeat",
		nullptr,
		0,
		nullptr
	);

	if (playResult != 0)
	{
		mciSendString(L"close bgm", nullptr, 0, nullptr);
		m_isBGMPlaying = false;
		return false;
	}

	m_isBGMPlaying = true;
	return true;
}

void AudioManager::StopBGM()
{
	if (m_isBGMPlaying)
	{
		mciSendString(L"stop bgm", nullptr, 0, nullptr);
		mciSendString(L"close bgm", nullptr, 0, nullptr);

		m_isBGMPlaying = false;
	}
}

void AudioManager::PlaySFX(const std::wstring& filePath)
{
	PlaySound(
		filePath.c_str(),
		nullptr,
		SND_FILENAME | SND_ASYNC
	);
}