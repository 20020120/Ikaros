#pragma once

#include <memory>
#include "d3d11.h"
#include "digits.h"
#include "SpriteBatch.h"

enum class COUNT
{
    UP,
    DOWN,
};



// タイマー ※課題 : 数値表示の上限がある,桁固定だけでなく可動するようにする

class Timer
{
public:
    Timer() = default;
    ~Timer() = default;

public:
    void Initialize(ID3D11Device* Device, COUNT count, const float default_start_time_, const wchar_t* filename = nullptr, int max_digit = 0);   // 最大桁数を設定するかどうかで表示方法が変化する
    void Update(float elapsedTime);
    void SpriteRender(ID3D11DeviceContext* Immediate_Context, DirectX::XMFLOAT2 Pos = {});
    void Finalize();

public: // Get関数
    float NowTime() { return timer->GetDigit(); }

public: // Set関数
    void Reset() { timer->Reset(); }
    void Add(const float add) { timer->Add(add); }
    void Subtract(const float subtract) { timer->Subtract(subtract); }
    void Stop() { stop = true; }
    void UnlockStop() { stop = false; }

private:
    //　関数ポインタ //
    using pFunc = void(Timer::*)(float);
    pFunc pCount = nullptr;

    // 関数ポインタ用関数
    void CountUp(float elapsedTime);
    void CountDown(float elapsedTime);


    //　関数ポインタ //
    using pFuncRender = void(Timer::*)(ID3D11DeviceContext*, const DirectX::XMFLOAT2&);
    pFuncRender pRender = nullptr;

    // 桁が固定
    void ClampSizeRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos);
    // 桁が変動する
    void FreeSizeRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos);

private: // 定数
    static constexpr float zero_time = 0.0f;        // ゼロ
    static constexpr float max_time = 9999.0f;      // 最大値

private: // 変数
    std::unique_ptr<Digits<float>> timer;           // 時間数値
    std::unique_ptr<Sprite_Batch> spr_number;       // スプライト
    DirectX::XMFLOAT2 number_size;                  // 一文字の大きさ
    bool stop = false;                              // 一時停止
};