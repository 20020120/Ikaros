

#include "Score.h"

#include <string>

#include "SpriteBatch.h"



void Score::Initialize(ID3D11Device* Device, int max_digit)
{
    // �O�����Z�q
    pRender = static_cast<int>(max_digit) ? /* (max_digit > 0) == true */&Score::ClampSizeRender : /* (max_digits == 0) == false */&Score::FreeSizeRender;

    score = std::make_unique<Digits<int>>(0, max_digit);

    if (max_digit <= 0) max_digit = 7 /*7���͂P�O�O���̈�*/;
    spr_number = std::make_unique<Sprite_Batch>(Device, L"./resources/Sprite/UIs/yone.png", max_digit);
    
    number_size.x = spr_number->GetTextureWidth() / 10;
    number_size.y = spr_number->GetTextureHeight();
}


void Score::Update(float ElapsedTime)
{
    
}


void Score::SpriteRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos)
{
    if (spr_number == nullptr) return;

    (this->*pRender)(Immediate_Context, Pos);
}


void Score::Finalize()
{
    
}


void Score::f_Gui()
{
#if _DEBUG

#endif
}



void Score::ClampSizeRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos)
{
    constexpr DirectX::XMFLOAT2 Scale   = { 1.0f,1.0f };

    
    std::vector<int> digit_place;
    digit_place.resize(score->max_digits_place);
    score->SliceDigits(digit_place);

    spr_number->Begin(Immediate_Context);
    for (size_t i = digit_place.size(); i > 0; --i)
    {
        spr_number->Render(Immediate_Context,
            { Pos.x + number_size.x * (i - 1), Pos.y },
            Scale,
            { number_size.x * digit_place.at(digit_place.size() - i),0.0f },
            number_size,
            { 0.5f,0.5f }
        );
    }
    spr_number->End(Immediate_Context);
}


void Score::FreeSizeRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos)
{
    constexpr DirectX::XMFLOAT2 Scale = { 1.0f,1.0f };


    // ���l�𕶎��ɕϊ�
    std::string str_number = std::to_string(score->GetDigit());

    // ���݂̕����ʒu(���Έʒu)
    float carriage = 0;
    
    // �ꕶ���Â�Render()����
    spr_number->Begin(Immediate_Context);
    for (const char c : str_number)
    {
        int number = (c & 0x0F);
        //������\���B�A�X�L�[�R�[�h�̈ʒu�ɂ��镶���ʒu��؂蔲���ĕ\��
        spr_number->Render(Immediate_Context, { Pos.x + carriage,Pos.y }, Scale, { number_size.x * number, 0.0f }, number_size, { 0.5f,0.5f });

        //�����ʒu�𕝕����炷
        carriage += number_size.x * Scale.x;
    }
    spr_number->End(Immediate_Context);

}
