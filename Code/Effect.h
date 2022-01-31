#pragma once

#include<DirectXMath.h>
#include<Effekseer.h>

class Effect
{
public:
	Effect(const char* filename);
	~Effect();

	//�Đ�
	Effekseer::Handle Play(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& angle = { 0,0,0 }, const float scale = 1.0f);
	//��~
	void Stop(Effekseer::Handle handle);
	//���W�ݒ�
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);
	//�X�P�[���ݒ�
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);
	//�p�x�ݒ�
	void SetAngle(Effekseer::Handle handle, const DirectX::XMFLOAT3& angle);

private:
	Effekseer::Effect* effekseerEffect = nullptr;
};