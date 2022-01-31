#include "GameOverBoss.h"

GameOverBoss::GameOverBoss(ID3D11Device* d)
{
    ModelRenderInitializer Ini{};
    Ini.SetALLPath(
        "./resources/Models/Characters/Enemies/EnemyRed.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/LambertPS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );

    model = std::make_unique<ModelComponent>(d, Ini);
    t.Scale = { 0.02f,0.02f,0.02f };
    r.Angle = { DirectX::XMConvertToRadians(-90.0f),DirectX::XMConvertToRadians(180.0f),0.0f };
}


void GameOverBoss::Update(float elapsedTime)
{
    
}

void GameOverBoss::Render(ID3D11DeviceContext* dc)
{
    model->Render(dc, t, r);
}

