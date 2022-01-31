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
    void Initialize(ID3D11Device* Device, int max_digit = 0);   // �ő包����ݒ肷�邩�ǂ����ŕ\�����@���ω�����
    void Update(float ElapsedTime);
    void SpriteRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos = {});
    void Finalize();

    void f_Gui();

public: // Get�֐�
    int GetNowScore() { return score->GetDigit(); }

public: // Set�֐�
    void Add(const int add) { score->Add(add); }
    void Reset() { score->Reset(); }

private:
    //�@�֐��|�C���^ //
    using pFunc = void(Score::*)(ID3D11DeviceContext*, const DirectX::XMFLOAT2&);
    pFunc pRender = nullptr;

    // �����Œ�
    void ClampSizeRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos);
    // �����ϓ�����
    void FreeSizeRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos);

private: // �ϐ��̒�`
    std::unique_ptr<Digits<int>> score;
    std::unique_ptr<Sprite_Batch> spr_number;

    DirectX::XMFLOAT2 number_size;
};