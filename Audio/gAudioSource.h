#pragma once

#include <memory>
#include <xaudio2.h>
#include "gAudioResource.h"

// オーディオソース
class AudioSource
{
public:
	AudioSource(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource);
	~AudioSource();

	// 再生
	void Play(bool loop, const float volume = 1.0f);

	// 停止
	void Stop();

	//音量設定
	void SetVolume(float volume);

	//解放処理
	void Release();

private:
	IXAudio2SourceVoice*			sourceVoice = nullptr;
	std::shared_ptr<AudioResource>	resource;
};
