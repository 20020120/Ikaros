#include"SceneManager.h"
#include"EffectManager.h"

void EffectManager::Initialize(ID3D11DeviceContext* dc)
{
	SceneManager& sceneManager = SceneManager::Instance();

	//EffekseerRenderer生成
	effekseerRenderer = EffekseerRendererDX11::Renderer::Create(sceneManager.GetDevice(), dc, 5000);
	//EffekseerManager生成
	effekseerManager = Effekseer::Manager::Create(5000);

	//EffekseerRendererの各種設定（特別なカスタマイズをしない場合は定型的に以下の設定でOK）
	effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
	effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
	effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
	effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());
	//Effekseer内でのローダーの設定（特別なカスタマイズをしない場合は以下の設定でOK）
	effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
	effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());
	//Effekseerを左手座標系で計算する
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
	//エフェクト更新処理
	effekseerManager->Update(elapsedTime * 60.0f);
}

void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	//ビュー＆プロジェクション行列をEffekseerRendererに設定
	effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	//Effekseer描画開始
	effekseerRenderer->BeginRendering();
	//Manager単位で描画するので描画順を制御する場合はManagerを複数個作成し、
	//Draw()関数を実行する順序で制御出来そう
	effekseerManager->Draw();
	//Effekseer描画終了
	effekseerRenderer->EndRendering();

}
