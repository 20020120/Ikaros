#include "misc.h"
#include "gAudioSource.h"

// コンストラクタ
AudioSource::AudioSource(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource)
	: resource(resource)
{
	HRESULT hr;

	// ソースボイスを生成
	hr = xaudio->CreateSourceVoice(&sourceVoice, &resource->GetWaveFormat());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

// デストラクタ
AudioSource::~AudioSource()
{
	if (sourceVoice != nullptr)
	{
		sourceVoice->DestroyVoice();
		sourceVoice = nullptr;
	}
}

// 再生
void AudioSource::Play(bool loop, const float volume)
{

	// ソースボイスにデータを送信
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = resource->GetAudioBytes();
	buffer.pAudioData = resource->GetAudioData();
	buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	
	sourceVoice->SubmitSourceBuffer(&buffer);

	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	sourceVoice->SetVolume(volume * 0.5f);
}

// 停止
void AudioSource::Stop()
{
	sourceVoice->Stop();
	sourceVoice->FlushSourceBuffers();
}

void AudioSource::SetVolume(float volume)
{
	sourceVoice->SetVolume(volume * 0.5f);
}

void AudioSource::Release()
{
	if (sourceVoice != nullptr)
	{
		sourceVoice->DestroyVoice();
		sourceVoice = nullptr;
	}
}
