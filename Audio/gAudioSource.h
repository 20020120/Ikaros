#pragma once

#include <memory>
#include <xaudio2.h>
#include "gAudioResource.h"

// �I�[�f�B�I�\�[�X
class AudioSource
{
public:
	AudioSource(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource);
	~AudioSource();

	// �Đ�
	void Play(bool loop, const float volume = 1.0f);

	// ��~
	void Stop();

	//���ʐݒ�
	void SetVolume(float volume);

	//�������
	void Release();

private:
	IXAudio2SourceVoice*			sourceVoice = nullptr;
	std::shared_ptr<AudioResource>	resource;
};
