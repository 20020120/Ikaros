#include "ClearBack.h"

#include <imgui.h>

ClearShip::ClearShip(ID3D11Device* d)
{
    ModelRenderInitializer Ini{};
    Ini.SetALLPath(
        "./resources/Models/Stages/Ship.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/LambertPS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );

    model = std::make_unique<ModelComponent>(d, Ini);
    t.Scale = { 0.03f,0.03f,0.03f };
    r.Angle = { DirectX::XMConvertToRadians(0.0f),DirectX::XMConvertToRadians(-180.0f),0.0f };
    t.Position = { 6.0f,-24.0f,23.0f };

    model->Update(0.01f);
}

void ClearShip::Update(float elapsedTime)
{
	model->Update(elapsedTime);

#if _DEBUG
    ImGui::Begin("clearShip");


    ImGui::DragFloat3("position", &t.Position.x, 0.1f);
    DirectX::XMFLOAT3 deg = {
        DirectX::XMConvertToDegrees(r.Angle.x),
        DirectX::XMConvertToDegrees(r.Angle.y),
      DirectX::XMConvertToDegrees(r.Angle.z)
    };
    ImGui::DragFloat3("Angle", &deg.x);

    r.Angle = {
        DirectX::XMConvertToRadians(deg.x),
        DirectX::XMConvertToRadians(deg.y),
        DirectX::XMConvertToRadians(deg.z)
    };


    ImGui::End();
#endif
}

void ClearShip::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, t, r);
}

void ClearShip::RankingInitialize()
{
    t.Position = { 6.0f,-24.0f,23.0f };
    r.Angle = { 0.0f,DirectX::XMConvertToRadians(-180.0f),DirectX::XMConvertToRadians(0.0f) };
}




//-----“V‹…-----//
ClearSkyDome::ClearSkyDome(ID3D11Device* d)
{
    ModelRenderInitializer Ini{};
    Ini.SetALLPath(
        "./resources/Models/Stages/SkyDome.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/LambertPS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );

    t.Scale = { 0.3f,0.3f,0.3f };
    model = std::make_unique<ModelComponent>(d, Ini);
}

void ClearSkyDome::Update(float elapsedTime)
{
    model->Update(elapsedTime);
}

void ClearSkyDome::Render(ID3D11DeviceContext* dc)
{
    model->Render(dc, t, r);
}
