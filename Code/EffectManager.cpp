#include"SceneManager.h"
#include"EffectManager.h"

void EffectManager::Initialize(ID3D11DeviceContext* dc)
{
	SceneManager& sceneManager = SceneManager::Instance();

	//EffekseerRenderer����
	effekseerRenderer = EffekseerRendererDX11::Renderer::Create(sceneManager.GetDevice(), dc, 5000);
	//EffekseerManager����
	effekseerManager = Effekseer::Manager::Create(5000);

	//EffekseerRenderer�̊e��ݒ�i���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͒�^�I�Ɉȉ��̐ݒ��OK�j
	effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
	effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
	effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
	effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());
	//Effekseer���ł̃��[�_�[�̐ݒ�i���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͈ȉ��̐ݒ��OK�j
	effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
	effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());
	//Effekseer��������W�n�Ōv�Z����
	effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

void EffectManager::Finalize()
{
	if (effekseerManager != nullptr)
	{
		effekseerManager->Destroy();
		effekseerManager = nullptr;
	}
	if (effekseerRenderer != nullptr)
	{
		effekseerRenderer->Destroy();
		effekseerRenderer = nullptr;
	}
}

void EffectManager::Update(float elapsedTime)
{
	//�G�t�F�N�g�X�V����
	effekseerManager->Update(elapsedTime * 60.0f);
}

void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	//�r���[���v���W�F�N�V�����s���EffekseerRenderer�ɐݒ�
	effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	//Effekseer�`��J�n
	effekseerRenderer->BeginRendering();
	//Manager�P�ʂŕ`�悷��̂ŕ`�揇�𐧌䂷��ꍇ��Manager�𕡐��쐬���A
	//Draw()�֐������s���鏇���Ő���o������
	effekseerManager->Draw();
	//Effekseer�`��I��
	effekseerRenderer->EndRendering();

}
