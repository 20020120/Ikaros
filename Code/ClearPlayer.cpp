#include "ClearPlayer.h"
#include <imgui.h>


ClearPlayer::ClearPlayer(ID3D11Device* d)
{
    ModelRenderInitializer Ini{};
    Ini.SetALLPath(
        "./resources/Models/Characters/Players/WhitePlayer.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/PhisicsBase_PS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );

    model = std::make_unique<ModelComponent>(d, Ini);
    t.Position = { 7.0f,-1.0f,-1.5f };
	t.Scale = { 0.03f,0.03f,0.03f };
    r.Angle = { -0.8f,3.1f,0.0f };

    model->f_PlayAnimation(AnimationName::GameClearBegin, false);
    model->Update(0.01f);

    // ƒ‰ƒCƒg‚ğ‰Šú‰»
    mPointLight = std::make_unique<GamePointLight>(d, 3);
    mPointLight->SetColor({ 0.1f,0.5f,0.8f });

}


void ClearPlayer::Update(float elapsedTime)
{

    if(!model->GetIsPlayAnimation())
    {
        model->f_PlayAnimation(AnimationName::GameClearLoop, true);
    }

    //ã‰º‰^“®
    ang += DirectX::XMConvertToRadians(50.0f * elapsedTime);
    if (ang > DirectX::XMConvertToRadians(180))ang -= DirectX::XMConvertToRadians(360);
    t.Position.y = -3.0f + cosf(ang);


    model->Update(elapsedTime);

#if _DEBUG
    ImGui::Begin("clearPlayer");


    ImGui::DragFloat3("position", &t.Position.x, 0.1f);
    ImGui::DragFloat3("scale", &t.Scale.x, 0.1f);
    ImGui::DragFloat3("angle", &r.Angle.x, 0.1f);



    ImGui::End();
#endif
}

void ClearPlayer::Render(ID3D11DeviceContext* dc)
{
    mPointLight->Update(dc);
    model->Render(dc, t, r);
}