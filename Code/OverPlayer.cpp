#include "OverPlayer.h"
#include "OptionSystem.h"
#include "User_Function.h"
#include "MathSupport.h"
#include "gAudio.h"
#include <imgui.h>

OverPlayer::OverPlayer(ID3D11Device* d)
{
    ModelRenderInitializer Ini{};
    Ini.SetALLPath(
        "./resources/Models/Characters/Players/WhitePlayer.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/PhisicsBase_PS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );

    model = std::make_unique<ModelComponent>(d, Ini);
    t.Position = { 30.0f,-1.0f,12.0f };
    t.Scale = { 0.05f,0.05f,0.05f };
    r.Angle = { -0.2f,-4.242,-0.1f };
    posX = -7.0f;

    // イベントフラグを読み込み
    Flags.Load("./resources/Models/Characters/Players/PlayerEventFlag.flg");

    Light = std::make_unique<GamePointLight>(d, 3);
    DirectX::XMFLOAT3 p = t.Position;
    p.y += 6.0f;
    Light->SetPosition(p);
    Light->SetRange(0.0f);
    Light->SetColor({ 0.1f,0.5f,0.8f });
    DirectX::XMFLOAT3 playerColor = { 0.9f,0.2f,0.2f };
    Light->SetColor(playerColor);


    model->f_PlayAnimation(AnimationName::GameOver, true);
    model->Update(0.01f);

    //変形して飛んでいく
    transformAnim = false;
    finishTransform = false;
    performState = 0;
    performTimer = 0.0f;

    seTransform = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/ModeChangeSE.wav");

    efkSonicBoom = std::make_unique<Effect>("./resources/Effects/sonic_boom.efk");

}


void OverPlayer::Update(float elapsedTime)
{
    if (!startTransform)
    {
        //上下運動
        ang += DirectX::XMConvertToRadians(10.0f * elapsedTime);
        if (ang > DirectX::XMConvertToRadians(180))ang -= DirectX::XMConvertToRadians(360);
        t.Position.y = posX + cosf(ang);
    }
    else
    {
        //変形アニメ―ション再生
        if(!transformAnim)
        {
            model->f_PlayAnimation(AnimationName::transform);
            seTransform->Play(false, OptionSystem::Instance().GetSeVolume() * 0.9f);
            transformAnim = true;
        }

        //変形中
        if (!finishTransform)
        {
            float ratio = 25.0f * elapsedTime;
            DirectX::XMFLOAT3 p = { 28.0f,0.0f,11.0f };
            DirectX::XMFLOAT3 ang = { 0.1f,-4.742,-0.1f };
            t.Position = Calcu3D::LerpFloat3(t.Position, p, ratio);
            r.Angle = Calcu3D::LerpFloat3(r.Angle, ang, ratio);
        }

        //変形終了
        if (!finishTransform && Flags.Check("EndChangeToWing", model->GetCurrentAnimationNumber(), model->GetCurrentAnimationFrame()))
        {
            model->f_PlayAnimation(AnimationName::wing, true);
            finishTransform = true;
        }

        //飛んでいく
        if(finishTransform)
        {
            Performance(elapsedTime);
        }
    }


    DirectX::XMFLOAT3 p = t.Position;
    p.y += 6.0f;
    Light->SetPosition(p);

    model->Update(elapsedTime);

#if _DEBUG
    ImGui::Begin("clearPlayer");


    ImGui::DragFloat3("position", &t.Position.x, 0.1f);
    ImGui::DragFloat3("scale", &t.Scale.x, 0.01f);
    ImGui::DragFloat3("angle", &r.Angle.x, 0.1f);

    ImGui::DragFloat("posX", &posX, 0.01f);

    ImGui::DragFloat("ratio", &ratio, 0.01f);


    ImGui::DragFloat("performTimer", &performTimer, 0.01f);
    ImGui::DragFloat("efkScale", &efkScale, 0.01f);


    ImGui::End();
#endif
}

void OverPlayer::Render(ID3D11DeviceContext* dc)
{
    // ライトを更新
    Light->Update(dc);

    model->Render(dc, t, r);
}

void OverPlayer::Finalize()
{
    seTransform->Release();
}


void OverPlayer::Performance(float elapsedTime)
{
    performTimer += elapsedTime;

	switch(performState)
	{
    case 0://後退準備
    {
        //各情報からワールド変換行列を作成する
        DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(t.Scale.x,t.Scale.y,t.Scale.z) };
        DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z) };
        DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(t.Position.x, t.Position.y, t.Position.z) };
        DirectX::XMFLOAT4X4 World = f_SetWorldCoordinate(WorldCoordinate::LHS_YUP, S, R, T);
        DirectX::XMFLOAT3 backVec = { -World._31,-World._32,-World._33 };

        backVec = Calcu3D::Normalize(backVec);
        backVec = backVec * 15.0f;

        targetPos = t.Position + backVec;

        beforePos = t.Position;

        ratio = 0.0f;

        performState++;
        /*FallThrough*/
    }
    case 1://少し後退
    {
        ratio += 5.0f * elapsedTime;

        t.Position = Calcu3D::LerpFloat3(beforePos, targetPos, ratio);

        if (ratio > 0.99f)
        {
            performState++;
            performTimer = 0.0f;
            ratio = 0.0f;
        }

        break;
    }
    case 2://前進準備
    {
        //各情報からワールド変換行列を作成する
        DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(t.Scale.x,t.Scale.y,t.Scale.z) };
        DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z) };
        DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(t.Position.x, t.Position.y, t.Position.z) };
        DirectX::XMFLOAT4X4 World = f_SetWorldCoordinate(WorldCoordinate::LHS_YUP, S, R, T);
        DirectX::XMFLOAT3 backVec = { World._31,World._32,World._33 };

        backVec = Calcu3D::Normalize(backVec);
        backVec = backVec * 50.0f;

        targetPos = t.Position + backVec;
        beforePos = t.Position;

        ratio = 0.0f;

        const DirectX::XMFLOAT3 rad = { Calcu3D::ConvertDegreeToRadian(0.0f,-75.0f,120.0f) };
        hdlSonicBoom = efkSonicBoom->Play(t.Position, rad, efkScale);

        performState++;
        /*FallThrough*/
    }
    case 3://前進
    {
        ratio += 2.5f * elapsedTime;

        std::vector<DirectX::XMFLOAT3> pos;
        pos.emplace_back(beforePos);
        pos.emplace_back(targetPos);
        DirectX::XMFLOAT3 p = { beforePos.x + 15.0f, targetPos.y + 25.0f, targetPos.z - 15.0f };
        pos.emplace_back(p);

        t.Position = Calcu3D::BezierCurve(pos, ratio);

        if (ratio > 0.95f)
        {
            performState++;
            performTimer = 0.0f;
            ratio = 0.0f;
        }

        break;
    }
    case 4:

        finishPerform = true;

        break;
	}
}
