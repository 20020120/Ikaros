

#include "EnergyGauge.h"
#include "SpriteBatch.h"
#include "User_Function.h"
#include <algorithm>
#include <imgui.h>

#include "Renderer.h"
#include "gAudio.h"
#include "OptionSystem.h"

void EnergyGauge::Initialize(ID3D11Device* Device, const int* Energy_)
{
    // 表示するスプライトの生成
    spr_gauges[0].Initialize(Device, L"./resources/Sprite/UIs/EnergyGauge.png");
    spr_gauges[1].Initialize(Device, L"./resources/Sprite/UIs/EnergyGauge.png");
    spr_gauge_ends[0].Initialize(Device, L"./resources/Sprite/UIs/LeftStart.png");
    spr_gauge_ends[1].Initialize(Device, L"./resources/Sprite/UIs/LeftEnd.png");
    spr_gauge_ends[2].Initialize(Device, L"./resources/Sprite/UIs/RightStart.png");
    spr_gauge_ends[3].Initialize(Device, L"./resources/Sprite/UIs/RightEnd.png");

    //TexPos初期化
    spr_gauges[0].TexSize = { spr_gauges[0].GetTextureWidth(),spr_gauges[0].GetTextureHeight() };
    spr_gauges[1].TexSize = { spr_gauges[1].GetTextureWidth(),spr_gauges[1].GetTextureHeight() };
    for (int i = 0; i < 4; i++)
    {
        spr_gauge_ends[i].TexSize = { spr_gauge_ends[i].GetTextureWidth(),spr_gauge_ends[i].GetTextureHeight() };
    }

    //位置初期化
    gaugeRatio = 0.0f;
    constexpr float PosX = 235.0f;
    constexpr float PosY = 680.0f;
    spr_gauge_ends[0].Position = { PosX - spr_gauge_ends[0].TexSize.x, PosY };
    spr_gauge_ends[1].Position = { PosX + spr_gauges[0].TexSize.x * gaugeRatio, PosY };
    spr_gauge_ends[2].Position = { PosX + spr_gauges[0].TexSize.x * gaugeRatio + spr_gauge_ends[0].TexSize.x + 10.0f + 0, PosY };
    spr_gauge_ends[3].Position = { PosX + spr_gauges[0].TexSize.x + spr_gauge_ends[0].TexSize.x * 2.0f + 10.0f, PosY };

    colors[0][0] = { 0,0,1,1 };
    colors[0][1] = { 0.7f,0,0,1 };

    colors[1][0] = { 1,0,0,1 };
    colors[1][1] = { 0,0,0.7f,1 };

    Reset();
    Energy = Energy_;


    ifRatio[0] = 0.005f;
    ifRatio[1] = 0.02f;
    ifRatio[2] = 0.995f;
    ifRatio[3] = 0.95f;

    //-----ダイヤ-----//
    spr_diamond.Initialize(Device, L"./resources/Sprite/UIs/DiamondAnim.png");
    spr_diamond.Position = { 640.0f, 660.0f };
    spr_diamond.Scale = { 1.2f,1.2f };
    spr_diamond.TexSize = { 23,45 };//1コマ当たりの大きさ
    spr_diamond.Pivot = { 23.0f / 2.0f,45.0f / 2.0f };
    spr_diamond.TexPos.y = (static_cast<float>(Now_Mode) - 1.0f) * spr_diamond.TexSize.y;

    spr_shine_diamond.Initialize(Device, L"./resources/Sprite/UIs/ShineEnergy.png");
    spr_shine_diamond.Position = { 640.0f, 660.0f };
    spr_shine_diamond.Scale = { 1.2f, 1.2f };
    spr_shine_diamond.TexSize = { spr_shine_diamond.GetTextureWidth()*0.5f,spr_shine_diamond.GetTextureHeight() };
    spr_shine_diamond.Pivot = { spr_shine_diamond.TexSize.x / 2.0f,spr_shine_diamond.TexSize.y / 2.0f };
    spr_shine_diamond.Color.w = 0.0f;

    animTimer = 0.0f;
    waitTimer = 0.0f;
    diamondAnim = false;
	cosTimer = 0.0f;


    inputInterval = 0.0f;

    //SE
    se_charge_max = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/ChargeEnergyMax.wav");
}


void EnergyGauge::Update(float ElapsedTime)
{
    lerpRatio = 10.0f * ElapsedTime;

    // 色の入れ替えがあれば、下地と動作する色を入れ替える
    inputInterval -= ElapsedTime;
    SwapColor();

    // ダイヤのアニメーション処理
    if (diamondAnim)DiamondAnimation(ElapsedTime);
    ShineDiamond(ElapsedTime);

#ifdef _DEBUG
    

#endif
}


void EnergyGauge::SpriteRender(ID3D11DeviceContext* Immediate_Context)
{
    // スプライト表示

    const float texture_width = spr_gauges[0].GetTextureWidth();
    const float texture_height = spr_gauges[0].GetTextureHeight();

    const float ends_size = spr_gauge_ends[0].GetTextureWidth();

    constexpr float PosX = 235.0f;
    constexpr float PosY = 680.0f;
    DirectX::XMFLOAT2 scale = { 1.0f,1.0f };

    const float ratio =  static_cast<float>(*Energy) / static_cast<float>(MaxEnergyCount);
    gaugeRatio = Calcu3D::LerpFloat(gaugeRatio, ratio, lerpRatio);

    //端
    //-----ゲージが振り切った時の処理-----//
    spr_gauge_ends[0].Position = { PosX - ends_size,PosY };
    spr_gauge_ends[1].Position = { PosX + texture_width * gaugeRatio - 0,PosY };
    spr_gauge_ends[2].Position = { PosX + texture_width * gaugeRatio + ends_size + 10.0f + 0,PosY };
    spr_gauge_ends[3].Position = { PosX + texture_width + ends_size * 2.0f + 10.0f,PosY };
    //左側
    {
        if (gaugeRatio < ifRatio[0])
        {
            gaugeRatio = 0.0f;
            spr_gauge_ends[0].TexSize.x = Calcu3D::LerpFloat(spr_gauge_ends[0].TexSize.x, 0.0f, lerpRatio * 10.0f);
            spr_gauge_ends[1].TexSize.x = 0.0f;
        }
        else if(gaugeRatio < ifRatio[1])
        {
            spr_gauge_ends[0].TexSize.x = Calcu3D::LerpFloat(spr_gauge_ends[0].TexSize.x, ends_size, lerpRatio * 2.0f);
            spr_gauge_ends[1].TexSize.x = Calcu3D::LerpFloat(spr_gauge_ends[1].TexSize.x, 0.0f, lerpRatio * 2.0f);
        }
        else
        {
            spr_gauge_ends[0].TexSize.x = ends_size;
            spr_gauge_ends[1].TexSize.x = Calcu3D::LerpFloat(spr_gauge_ends[1].TexSize.x, ends_size, lerpRatio * 2.0f);
        }
	        
    }
    //右側
    {
        if (gaugeRatio > ifRatio[2])
        {
            gaugeRatio = 1.0f;
            spr_gauge_ends[2].TexSize.x = 0.0f;
            spr_gauge_ends[3].TexSize.x = Calcu3D::LerpFloat(spr_gauge_ends[3].TexSize.x, 0.0f, lerpRatio * 10.0f);

            spr_gauge_ends[2].TexPos.x = ends_size;
            spr_gauge_ends[3].TexPos.x = Calcu3D::LerpFloat(spr_gauge_ends[3].TexPos.x, ends_size, lerpRatio * 10.0f);

            if(!isChargeMax)
            {
                se_charge_max->Stop();
                se_charge_max->Play(false, OptionSystem::Instance().GetSeVolume());

                isChargeMax = true;
            }
        }
        else if (gaugeRatio > ifRatio[3])
        {
            spr_gauge_ends[2].TexSize.x = Calcu3D::LerpFloat(spr_gauge_ends[2].TexSize.x, 0.0f, lerpRatio * 2.0f);
            spr_gauge_ends[3].TexSize.x = Calcu3D::LerpFloat(spr_gauge_ends[3].TexSize.x, ends_size, lerpRatio * 2.0f);

            spr_gauge_ends[2].TexPos.x = Calcu3D::LerpFloat(spr_gauge_ends[2].TexPos.x, ends_size, lerpRatio * 2.0f);
            spr_gauge_ends[3].TexPos.x = Calcu3D::LerpFloat(spr_gauge_ends[3].TexPos.x, 0.0f, lerpRatio * 2.0f);

            float p = PosX + texture_width + ends_size * 3.0f + 10.0f;
            spr_gauge_ends[2].Position.x = Calcu3D::LerpFloat(spr_gauge_ends[2].Position.x, p, lerpRatio * 10.0f);
        }
        else
        {
            spr_gauge_ends[2].TexSize.x = Calcu3D::LerpFloat(spr_gauge_ends[2].TexSize.x, ends_size, lerpRatio * 2.0f);
            spr_gauge_ends[3].TexSize.x = ends_size;

            spr_gauge_ends[2].TexPos.x = Calcu3D::LerpFloat(spr_gauge_ends[2].TexPos.x, 0.0f, lerpRatio * 2.0f);
            spr_gauge_ends[3].TexPos.x = 0.0f;

            spr_gauge_ends[2].Position = { PosX + texture_width * gaugeRatio + ends_size + 10.0f + 0,PosY };
            spr_gauge_ends[3].Position = { PosX + texture_width + ends_size * 2.0f + 10.0f,PosY };

            isChargeMax = false;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        spr_gauge_ends[i].Color = colors[static_cast<int>(Now_Mode)-1][i / 2];
        spr_gauge_ends[i].Render(Immediate_Context);
    }

    // 左側ゲージ
    scale.x *= gaugeRatio;
    spr_gauges[1].Position = { PosX, PosY };
    spr_gauges[1].Scale = scale;
    spr_gauges[1].Color = colors[static_cast<int>(Now_Mode) - 1][0];
    spr_gauges[1].Render(Immediate_Context);

    // 右側ゲージ
    scale.x = (1.0f - gaugeRatio);
    spr_gauges[0].Position = { PosX + texture_width * gaugeRatio + 10.0f + ends_size * 2.0f, PosY };
    spr_gauges[0].Scale = scale;
    spr_gauges[0].Color = colors[static_cast<int>(Now_Mode) - 1][1];
    spr_gauges[0].Render(Immediate_Context);

    //ダイヤ
    const float px = 210.0f + 55.0f + (spr_gauges[0].GetTextureWidth() * gaugeRatio);
    if (isChargeMax)
    {
        spr_shine_diamond.Position.x = px;
        spr_shine_diamond.TexPos.x = spr_shine_diamond.TexSize.x * (static_cast<float>(Now_Mode) - 1.0f);
        spr_shine_diamond.Render(Immediate_Context);
    }
    spr_diamond.Position.x = px;
    spr_diamond.Render(Immediate_Context);


}


void EnergyGauge::Finalize()
{
    se_charge_max->Release();
}


void EnergyGauge::f_Gui()
{
#if _DEBUG
    ImGui::Begin("EnergyGauge");

    ImGui::DragFloat("scrollTime", &scrollTime, 0.01f);
    ImGui::DragFloat("maxWaitTime", &maxWaitTime, 0.1f);

    ImGui::End();
#endif
}


void EnergyGauge::Reset()
{
    //*Energy = 0;
    Now_Mode = Type::BLUE;
}



void EnergyGauge::SwapColor()
{
    //constexpr float mulValue = { 0.7f };

    const GamePadButton transformationButton = GamePad::BTN_RIGHT_TRIGGER | GamePad::BTN_RIGHT_SHOULDER;

    if (inputInterval > 0.0f)return;

    if (changeColor)
    {
#if 0
        // 赤ー＞青に変形
        if (*Energy >= LimitEnergySize)
        {
            // エネルギーを設定して
            //*Energy = static_cast<int>(mulValue * static_cast<float>(LimitBlueEnergy));

            if (Now_Mode == Type::BLUE) return;

            std::swap(spr_gauges[0], spr_gauges[1]);
            Now_Mode = Type::BLUE;
        }
        else if (*Energy <= -LimitEnergySize)
        {
            // エネルギーを設定して
            //*Energy = static_cast<int>(mulValue * static_cast<float>(LimitRedEnergy));

            if (Now_Mode == Type::RED) return;

            std::swap(spr_gauges[0], spr_gauges[1]);
            Now_Mode = Type::RED;
        }
#else
        // 色の入れ替え
        //if(*Energy >= MaxEnergyCount)
        {

            if(Now_Mode == Type::BLUE)
            {
                Now_Mode = Type::RED;
                maxWaitTime = 0.0f;
            }
            else
            {
                Now_Mode = Type::BLUE;
                maxWaitTime = 0.0f;
            }
            diamondAnim = true;
            gaugeRatio = 0.0f;

            constexpr float maxInputInterval = 1.0f;
            inputInterval = maxInputInterval;
        }

        changeColor = false;
#endif

    }
}

void EnergyGauge::DiamondAnimation(float elapsedTime)
{
    waitTimer += elapsedTime;
    if (waitTimer > maxWaitTime)
    {
        //位置 /*return 1 - pow(1 - x, 5);*/
        const float r = 10.0f * elapsedTime;
        moveTimer += elapsedTime;
        if (spr_diamond.TexPos.x < spr_diamond.GetTextureWidth() * 0.2f)
        {
            spr_diamond.Position.y = Calcu3D::LerpFloat(spr_diamond.Position.y, 660.0f - 35.0f, r);
        }
        else
        {
            spr_diamond.Position.y = Calcu3D::LerpFloat(spr_diamond.Position.y, 660.0f, r * 0.5f);
        }

        //アニメーション処理
        animTimer += elapsedTime;
        if (animTimer > scrollTime)
        {
            spr_diamond.TexPos.x += spr_diamond.TexSize.x;
            animTimer = 0.0f;
        }
        if (spr_diamond.TexPos.x >= spr_diamond.GetTextureWidth())
        {
            //アニメーション終わり
            diamondAnim = false;
            spr_diamond.Position.y = 660.0f;
            spr_diamond.TexPos.x = 0.0f;
            spr_diamond.TexPos.y = (static_cast<float>(Now_Mode) - 1.0f) * spr_diamond.TexSize.y;

            waitTimer = 0.0f;
            moveTimer = 0.0f;
        }
        
    }
}

void EnergyGauge::ShineDiamond(float elapsedTime)
{
    const float r = 10.0f * elapsedTime;
    cosTimer += DirectX::XMConvertToRadians(270) * elapsedTime;
    cosTimer = Calcu3D::ClampRotate(cosTimer);

    if (isChargeMax)
    {
        spr_shine_diamond.Color.w = Calcu3D::LerpFloat(spr_shine_diamond.Color.w, 1.0f, r);
        spr_shine_diamond.Scale.x = spr_shine_diamond.Scale.y = { 1.5f + 0.25f * cosf(cosTimer) };
    }
    else
    {
        spr_shine_diamond.Color.w = Calcu3D::LerpFloat(spr_shine_diamond.Color.w, 0.0f, r);
        spr_shine_diamond.Scale = { 1,1 };
    }
}
