#pragma once

#include <memory>
#include "Timer.h"
class Sprite_Batch;


class Combo
{
public:
    Combo() = default;
    ~Combo() = default;

public:
    void Initialize(ID3D11Device* Device);
    void Update(float ElapsedTime);
    void SpriteRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2 Pos = {});
    void Finalize();

public: // Get関数
    int GetCombo() { return combo; }
    int GetMaxCombo() { return max_comboed; }

public: // Set関数
    void Clear()
    {
        is_combo = false;
        combo = 0;
        max_comboed = INT_MIN;
    }
    void AddCombo()
    {
        is_combo = true;
        ++combo;
        timer->Reset();
    }
private:
    void Prepare()  // コンボ一時終了 コンボ情報のリセット
    {
        is_combo = false;
        combo = 0;
    }

    // 桁が変動する
    void FreeSizeRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos);

private:
    // 定数

    // 変数
    bool is_combo = false;                      // コンボ中
    int combo = 0;                              // 現在コンボ数
    int max_comboed = INT_MIN;                  // 過去最大コンボ
    std::unique_ptr<Timer> timer;
    std::unique_ptr<Sprite_Batch> spr_icon;
    std::unique_ptr<Sprite_Batch> spr_number;
    DirectX::XMFLOAT2 number_size;
};