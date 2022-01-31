#pragma once

#include <memory>
#include "d3d11.h"
#include "digits.h"
class Sprite_Batch;



class Score
{
public:
    Score() = default;
    ~Score() = default;

public:
    void Initialize(ID3D11Device* Device, int max_digit = 0);   // 最大桁数を設定するかどうかで表示方法が変化する
    void Update(float ElapsedTime);
    void SpriteRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos = {});
    void Finalize();

    void f_Gui();

public: // Get関数
    int GetNowScore() { return score->GetDigit(); }

public: // Set関数
    void Add(const int add) { score->Add(add); }
    void Reset() { score->Reset(); }

private:
    //　関数ポインタ //
    using pFunc = void(Score::*)(ID3D11DeviceContext*, const DirectX::XMFLOAT2&);
    pFunc pRender = nullptr;

    // 桁が固定
    void ClampSizeRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos);
    // 桁が変動する
    void FreeSizeRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos);

private: // 変数の定義
    std::unique_ptr<Digits<int>> score;
    std::unique_ptr<Sprite_Batch> spr_number;

    DirectX::XMFLOAT2 number_size;
};