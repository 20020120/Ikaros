#pragma once

#include<DirectXMath.h>
#include<Effekseer.h>

class Effect
{
public:
	Effect(const char* filename);
	~Effect();

	//再生
	Effekseer::Handle Play(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& angle = { 0,0,0 }, const float scale = 1.0f);
	//停止
	void Stop(Effekseer::Handle handle);
	//座標設定
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);
	//スケール設定
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);
	//角度設定
	void SetAngle(Effekseer::Handle handle, const DirectX::XMFLOAT3& angle);

private:
	Effekseer::Effect* effekseerEffect = nullptr;
};