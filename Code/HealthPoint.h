#pragma once

#include <memory>
#include "d3d11.h"
#include "Renderer.h"
#include "Font.h"

// �̗̓N���X

class HealthPoint
{
public:
    HealthPoint() = default;
    ~HealthPoint() = default;

public:
    void Initialize(ID3D11Device* Device, int* Default_HealthPoint);
    void Update(float ElapsedTime);
    void SpriteRender(ID3D11DeviceContext* Immediate_Context);
    void Finalize();

    void AlertUpdate(float elapsedTime);

public: // Get�֐�
    int NowHealthPoint() const { return *health_point; }

public: // Set�֐�
    void AddHP(const int& add);
    void SubtractHP(const int& subtract);

private: // �萔�̒�` 
    int MAXHP;                      // �C�ӂ̃^�C�~���O��const�Ƃ��ď������ł��Ȃ�����...
    static constexpr int ZEROHP = 0;

private: // �ϐ��̒�` 
    int* health_point;
    float hpRatio;
    float lerpRatio;
    Renderer spr_heart;
    Renderer spr_frame;
    Renderer spr_alert;

    std::unique_ptr<Font> font;
    DirectX::XMFLOAT2 percentagePos = { 58,35 };
    DirectX::XMFLOAT2 percentageScale = { 1.0f,1.2f };
    DirectX::XMFLOAT2 percentPos = { 103,65 };
    DirectX::XMFLOAT2 percentScale = { 0.5f,0.55f };

	DirectX::XMFLOAT4 fontColor = { 0,1,1,1 };

    int alertState = 0;
    float alertTimer = 0.0f;
}; 