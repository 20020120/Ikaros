#include"SceneManager.h"
#include"Effect.h"
#include"EffectManager.h"
#include <cassert>

Effect::Effect(const char* filename)
{
	//エフェクトを読み込みする前にロックする
	//※マルチスレッドでエフェクトを作成するとデバイスコンテキストを同時アクセスして
	//フリーズする可能性があるので排他制御する
	std::lock_guard<std::mutex> lock(SceneManager::Instance().GetMutex());

	//Effekseerのリソースを読み込む
	//EffekseerはUTF-16のファイルパス以外は対応していないため文字コード変換が必要
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);
	//Effekseer::Managerを取得
	Effekseer::Manager* effekseerManager = EffectManager::Instance().GetEffekseerManager();
	//Effekseerエフェクト読み込み
	effekseerEffect = Effekseer::Effect::Create(effekseerManager, static_cast<EFK_CHAR*>(utf16Filename));
	
	assert(effekseerEffect);
}
Effect::~Effect()
{
	//破棄処理
	if (effekseerEffect != nullptr)
	{
		effekseerEffect->Release();
		effekseerEffect = nullptr;
	}
}

//再生
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& angle, const float scale)
{
	Effekseer::Manager* effekseerManager = EffectManager::Instance().GetEffekseerManager();
	if (effekseerEffect)
	{
		Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, position.x, position.y, position.z);
		effekseerManager->SetScale(handle, scale, scale, scale);
		effekseerManager->SetRotation(handle, angle.x, angle.y, angle.z);
		return  handle;
	}
	return -1;
}
//停止
void Effect::Stop(Effekseer::Handle handle)
{
	Effekseer::Manager* effekseerManager = EffectManager::Instance().GetEffekseerManager();
	effekseerManager->StopEffect(handle);
	effekseerManager->SetAllColor(handle, Effekseer::Color(1, 1, 1, 0));
}
//座標設定
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
	Effekseer::Manager* effekseerManager = EffectManager::Instance().GetEffekseerManager();
	effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}
//スケール設定
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	Effekseer::Manager* effekseerManager = EffectManager::Instance().GetEffekseerManager();
	effekseerManager->SetLocation(handle, scale.x, scale.y, scale.z);
}

//角度
void Effect::SetAngle(Effekseer::Handle handle, const DirectX::XMFLOAT3& angle)
{
	Effekseer::Manager* effekseermanager{ EffectManager::Instance().GetEffekseerManager() };
	effekseermanager->SetRotation(handle, angle.x, angle.y, angle.z);
}

