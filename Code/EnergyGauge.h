#pragma once

#include <memory>
#include <array>
#include "d3d11.h"
#include "Controller.h"
#include "DirectXMath.h"
#include "Renderer.h"
#include "gAudioSource.h"

// エネルギーゲージクラス

class EnergyGauge
{
private: // enum class
    enum class Type
    {
        NONE, // デバッグ用
        RED,
        BLUE
    };

public:
    EnergyGauge() = default;
    ~EnergyGauge() = default;

public:
    void Initialize(ID3D11Device* Device,const int* Energy_);
    void Update(float ElapsedTime);
    void SpriteRender(ID3D11DeviceContext* Immediate_Context);
    void Finalize();

    void f_Gui();

public: // Get関数

public: // Set関数
    void Reset();

    void SetChangeColor(bool frag) { changeColor = frag; }

private:
    void SwapColor();
    void DiamondAnimation(float elapsedTime);
    void ShineDiamond(float elapsedTime);

private: // 定数の定義 
    static constexpr int MaxEnergyCount = { 40 };

private: // 変数の定義 
    const int* Energy;
    Renderer spr_gauges[2];
    Renderer spr_gauge_ends[4];
    Renderer spr_diamond;
    Renderer spr_shine_diamond;

    DirectX::XMFLOAT4 colors[2][2];
    float ifRatio[4] = {};

    Type Now_Mode;

    float gaugeRatio;
    float animTimer = 0.0f;//アニメーションするタイマー
    float scrollTime = 0.04f;//1コマ当たりの秒数
    float waitTimer = 0.0f;
    float maxWaitTime = 3.0f;
    float moveTimer = 0.0f;
    bool diamondAnim{ false };
    bool isChargeMax{ false };//ゲージが最大値か

    bool changeColor{ false };

    float cosTimer = 0.0f;
    float lerpRatio;

    float inputInterval = 0.0f;

    //溜まったSE
    std::unique_ptr<AudioSource> se_charge_max;
};