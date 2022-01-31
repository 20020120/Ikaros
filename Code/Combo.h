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

public: // Get�֐�
    int GetCombo() { return combo; }
    int GetMaxCombo() { return max_comboed; }

public: // Set�֐�
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
    void Prepare()  // �R���{�ꎞ�I�� �R���{���̃��Z�b�g
    {
        is_combo = false;
        combo = 0;
    }

    // �����ϓ�����
    void FreeSizeRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos);

private:
    // �萔

    // �ϐ�
    bool is_combo = false;                      // �R���{��
    int combo = 0;                              // ���݃R���{��
    int max_comboed = INT_MIN;                  // �ߋ��ő�R���{
    std::unique_ptr<Timer> timer;
    std::unique_ptr<Sprite_Batch> spr_icon;
    std::unique_ptr<Sprite_Batch> spr_number;
    DirectX::XMFLOAT2 number_size;
};