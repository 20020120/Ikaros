#include"SceneManager.h"
#include"Effect.h"
#include"EffectManager.h"
#include <cassert>

Effect::Effect(const char* filename)
{
	//�G�t�F�N�g��ǂݍ��݂���O�Ƀ��b�N����
	//���}���`�X���b�h�ŃG�t�F�N�g���쐬����ƃf�o�C�X�R���e�L�X�g�𓯎��A�N�Z�X����
	//�t���[�Y����\��������̂Ŕr�����䂷��
	std::lock_guard<std::mutex> lock(SceneManager::Instance().GetMutex());

	//Effekseer�̃��\�[�X��ǂݍ���
	//Effekseer��UTF-16�̃t�@�C���p�X�ȊO�͑Ή����Ă��Ȃ����ߕ����R�[�h�ϊ����K�v
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);
	//Effekseer::Manager���擾
	Effekseer::Manager* effekseerManager = EffectManager::Instance().GetEffekseerManager();
	//Effekseer�G�t�F�N�g�ǂݍ���
	effekseerEffect = Effekseer::Effect::Create(effekseerManager, static_cast<EFK_CHAR*>(utf16Filename));
	
	assert(effekseerEffect);
}
Effect::~Effect()
{
	//�j������
	if (effekseerEffect != nullptr)
	{
		effekseerEffect->Release();
		effekseerEffect = nullptr;
	}
}

//�Đ�
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
//��~
void Effect::Stop(Effekseer::Handle handle)
{
	Effekseer::Manager* effekseerManager = EffectManager::Instance().GetEffekseerManager();
	effekseerManager->StopEffect(handle);
	effekseerManager->SetAllColor(handle, Effekseer::Color(1, 1, 1, 0));
}
//���W�ݒ�
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
	Effekseer::Manager* effekseerManager = EffectManager::Instance().GetEffekseerManager();
	effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}
//�X�P�[���ݒ�
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	Effekseer::Manager* effekseerManager = EffectManager::Instance().GetEffekseerManager();
	effekseerManager->SetLocation(handle, scale.x, scale.y, scale.z);
}

//�p�x
void Effect::SetAngle(Effekseer::Handle handle, const DirectX::XMFLOAT3& angle)
{
	Effekseer::Manager* effekseermanager{ EffectManager::Instance().GetEffekseerManager() };
	effekseermanager->SetRotation(handle, angle.x, angle.y, angle.z);
}

