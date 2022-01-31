#pragma once

#include<DirectXMath.h>
#include<Effekseer.h>
#include<EffekseerRendererDX11.h>

class EffectManager
{
private:
	EffectManager(){}
	~EffectManager(){}
public:
	//�B��̃C���X�^���X�擾
	static EffectManager& Instance()
	{
		static EffectManager instance;
		return instance;
	}
	//������
	void Initialize(ID3D11DeviceContext* dc);
	//�I����
	void Finalize();
	//�X�V����
	void Update(float elapsedTime);
	//�`�揈��
	void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);
	//EffekseerManager�̎擾
	Effekseer::Manager* GetEffekseerManager() { return effekseerManager; }
private:
	Effekseer::Manager* effekseerManager = nullptr;
	EffekseerRenderer::Renderer* effekseerRenderer = nullptr;
};