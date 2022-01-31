

#include "HealthPoint.h"
#include "SpriteBatch.h"

#include "User_Function.h"
#include <imgui.h>

void HealthPoint::Initialize(ID3D11Device* Device, int* Default_HealthPoint)
{
    // 初期体力のの設定
    health_point = Default_HealthPoint;

    MAXHP = *Default_HealthPoint;

    hpRatio = 1.0f;

    // 表示するスプライトの生成

    spr_heart.Initialize(Device, L"./resources/Sprite/UIs/HPMask.png", L"./resources/Sprite/Shaders/dissolve3.png", L"./resources/Sprite/UIs/HPGauge.png");
    spr_frame.Initialize(Device, L"./resources/Sprite/UIs/HPFrame.png");

    spr_heart.SetDissolvePower(1.0f);
    spr_heart.Position = { 10,10 };
    spr_frame.Position = { 10,10 };
    spr_heart.TexSize = { 120,120 };
    spr_frame.TexSize = { 120,120 };
    spr_heart.Color = { 0,1,1,1 };
    spr_frame.Color = { 0,1,1,1 };

    font = std::make_unique<Font>(Device, "./resources/Sprite/Font/Font.fnt", 2048);
    fontColor = { 0,1,1,1 };


    //アラート
    spr_alert.Initialize(Device, L"./resources/Sprite/UIs/Alert.png");
    spr_alert.Position = { 640,360 };
    spr_alert.Scale = { 1.1f,1.1f };
    spr_alert.Pivot = { 640,360 };
    spr_alert.TexSize = { 1280,720 };
    spr_alert.TexPos = { 0,0 };
    spr_alert.Color.w = 0.0f;
    alertState = 0;
    alertTimer = 0.0f;
}


void HealthPoint::Update(float ElapsedTime)
{
    lerpRatio = 10.0f * ElapsedTime;

    AlertUpdate(ElapsedTime);

#ifdef _DEBUG
    ImGui::Begin("HealthPoint");

    //ImGui::DragFloat2("CheckBoxPosition", &spr_heart.Position.x);
    //spr_frame.Position = spr_heart.Position;
    //ImGui::DragFloat2("percentagePos", &percentagePos.x);
    //ImGui::DragFloat2("percentageScale", &percentageScale.x,0.01f);
    //ImGui::DragFloat2("percentPos", &percentPos.x);
    //ImGui::DragFloat2("percentScale", &percentScale.x, 0.01f);

    ImGui::DragFloat("spr_alert.scale", &spr_alert.Scale.x, 0.01f);
    spr_alert.Scale.y = spr_alert.Scale.x;

    ImGui::DragFloat("spr_alert.Color.w", &spr_alert.Color.w, 0.01f);




    ImGui::End();


#endif


}


void HealthPoint::SpriteRender(ID3D11DeviceContext* Immediate_Context)
{
    // 体力のスプライト表示

    if (*health_point <= 0)
    {
        const float targetRatio = 0.0f;
        hpRatio = Calcu3D::LerpFloat(hpRatio, targetRatio, lerpRatio * 5.0f);
        return;
    }
    else
    {

        const float targetRatio = (*health_point == MAXHP) ? 1.0f : static_cast<float>(*health_point) / static_cast<float>(MAXHP);
        hpRatio = Calcu3D::LerpFloat(hpRatio, targetRatio, lerpRatio * 2.0f);
        if (hpRatio > 0.997f)hpRatio = 1.0f;
        hpRatio = roundf(hpRatio * 100.0f) * 0.01f;
    }

    spr_heart.SetDissolvePower(hpRatio);

    //---color変更---//
    DirectX::XMFLOAT4 color = { 1,1,1,1 };
    if (hpRatio > 0.5f)//青
    {
        color = { 0,1,1,1 };
    }
    else if(hpRatio > 0.2f)//オレンジ
    {
        color = { 1.0f,0.75f,0.15f,1.0f };
    }
    else//赤
    {
        color = { 1,0,0,1 };
    }
    spr_heart.Color = Calcu3D::LerpFloat4(spr_heart.Color, color, lerpRatio);
    spr_frame.Color = Calcu3D::LerpFloat4(spr_frame.Color, color, lerpRatio);
    fontColor = Calcu3D::LerpFloat4(fontColor, color, lerpRatio);

    //描画　
    spr_heart.Render(Immediate_Context);
    spr_frame.Render(Immediate_Context);

    //font
    wchar_t percentage[10];
    _itow_s(static_cast<int>(hpRatio * 100.0f), percentage, 10);
    wchar_t* p = percentage;

    percentageScale.x = ((*health_point) == MAXHP) ? 0.8f : 1.0f;

    font->Begin(Immediate_Context);
    font->Draw(percentagePos, percentageScale, fontColor, p, wcslen(p), Font::TEXT_ALIGN::UPPER_MIDDLE);
    font->Draw(percentPos, percentScale, fontColor, L"%", wcslen(L"%"), Font::TEXT_ALIGN::UPPER_MIDDLE);
    font->End(Immediate_Context);

    spr_alert.Render(Immediate_Context);
}


void HealthPoint::Finalize()
{

}


//****************************************************************
// 
//  Set関数
// 
//****************************************************************


void HealthPoint::AddHP(const int& add)
{
    if (add < 0) return;

    if (*health_point + add > MAXHP) return;

    *health_point += add;

}


void HealthPoint::SubtractHP(const int& subtract)
{
    if (subtract > 0) return;

    if (*health_point + subtract < ZEROHP) return;

    *health_point += subtract;
}

void HealthPoint::AlertUpdate(float elapsedTime)
{
    const float r = static_cast<float>(*health_point) / static_cast<float>(MAXHP);
    if (r < 0.2f)
    {
        const float maxAlpha = 1.0f;
        const float minAlpha = 0.0f;

        const float frac = 5.0f;

        switch (alertState)
        {
        case 0:
            alertTimer += elapsedTime * frac;
            if (alertTimer > 1.0f)
            {
                alertTimer = 1.0f;
                alertState++;
            }
            break;

        case 1:
            alertTimer -= elapsedTime * frac;
            if (alertTimer < 0.0f)
            {
                alertTimer = 0.0f;
                alertState++;
            }
            break;
        case 2:
            alertTimer += elapsedTime * frac;
            if (alertTimer > 1.0f)
            {
                alertTimer = 1.0f;
                alertState++;
            }

            break;

        case 3:
            alertTimer -= elapsedTime * frac;
            if (alertTimer < 0.0f)
            {
                alertTimer = 0.0f;
                alertState++;
            }

            break;

        case 4:
            alertTimer -= elapsedTime;
            if(alertTimer < -0.5f)
            {
                alertTimer = 0.0f;
                alertState = 0;
            }
            break;
        }
        spr_alert.Color.w = Calcu3D::LerpFloat(minAlpha, maxAlpha, alertTimer);
    }
    else if (r < 0.4f)
    {
        const float maxAlpha = 1.0f;
        const float minAlpha = 0.0f;

        const float frac = 2.5f;
        switch (alertState)
        {
        case 0:
            alertTimer += elapsedTime * frac;
            if (alertTimer > 1.0f)
            {
                alertTimer = 1.0f;
                alertState++;
            }
            break;

        case 1:
            alertTimer -= elapsedTime * frac;
            if (alertTimer < 0.0f)
            {
                alertTimer = 0.0f;
                alertState++;
            }
            break;
        case 2:
            alertTimer += elapsedTime * frac;
            if (alertTimer > 1.0f)
            {
                alertTimer = 1.0f;
                alertState++;
            }

            break;

        case 3:
            alertTimer -= elapsedTime * frac;
            if (alertTimer < 0.0f)
            {
                alertTimer = 0.0f;
                alertState++;
            }

            break;

        case 4:
            alertTimer -= elapsedTime;
            if (alertTimer < -1.0f)
            {
                alertTimer = 0.0f;
                alertState = 0;
            }
            break;
        }
        spr_alert.Color.w = Calcu3D::LerpFloat(minAlpha, maxAlpha, alertTimer);
    }

    else
    {
        alertTimer = 0.0f;
        alertState = 0;

        spr_alert.Color.w = Calcu3D::LerpFloat(spr_alert.Color.w, 0.0f, 10.0f * elapsedTime);

    }
}
