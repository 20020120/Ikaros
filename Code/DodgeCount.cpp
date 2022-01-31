

#include "DodgeCount.h"

#include "User_Function.h"


void DodgeCount::Initialize(ID3D11Device* Device, int* dodge)
{
    // ��𐔂̏�����
    //CountReset();

    dodge_count = dodge;
    DodgeCountMax = *dodge;

    // �\������X�v���C�g�̐���
    spr_dodge.Initialize(Device, L"./resources/Sprite/UIs/DodgePoint.png", *dodge);
    spr_frame.Initialize(Device, L"./resources/Sprite/UIs/DodgeFrame.png");

    spr_frame.Position = { 149.0f,22.5f };
    spr_frame.TexSize = { spr_frame.GetTextureWidth(),spr_frame.GetTextureHeight() };
    spr_frame.Color = { 0,1,1,1 };

    spr_dodge.Position = { 165.0f,30.0f };
    spr_dodge.TexSize = { spr_dodge.GetTextureWidth(),spr_dodge.GetTextureHeight() };
    spr_dodge.Color = { 0,1,1,1 };
}


void DodgeCount::Update(float ElapsedTime)
{
    // �F�ς���
	DirectX::XMFLOAT4 color = { 1,1,1,1 };
    if(*dodge_count > 3)//��
    {
        color = { 0,1,1,1 };
    }
    else if(*dodge_count > 1)//�I�����W
    {
        color = { 1.0f,0.75f,0.15f,1.0f };
    }
    else//��
    {
        color = { 1,0,0,1 };
    }
    constexpr float r = 0.05f;
    spr_dodge.Color = Calcu3D::LerpFloat4(spr_dodge.Color, color, r);
    spr_frame.Color = Calcu3D::LerpFloat4(spr_frame.Color, color, r);
}


void DodgeCount::Render(ID3D11DeviceContext* Immediate_Context)
{
    if (*dodge_count < 0) return;

    for (int i = 0; i < *dodge_count; ++i)
    {
	    spr_dodge.Position.x = 165.0f + static_cast<float>(i) * (spr_dodge.TexSize.x * 0.85f) * spr_dodge.Scale.x;
        spr_dodge.Render(Immediate_Context);
    }

    spr_frame.Render(Immediate_Context);
}


void DodgeCount::Finalize()
{
    
}


bool DodgeCount::CanDodge()
{
    // ��𐔂��ő��𐔈ȓ��Ȃ�true��Ԃ� (0 ~ 5)
    return (*dodge_count <= DodgeCountMax);
}


void DodgeCount::Reset()
{
    /* ��𐔂����Z�b�g���� */
    dodge_count = 0;
}


void DodgeCount::Add()
{
    /* ��𐔂��P���₷ */
    dodge_count++;
}