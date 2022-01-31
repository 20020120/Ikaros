

#include "Timer.h"

#include <string>

#include "SpriteBatch.h"



void Timer::Initialize(ID3D11Device* Device, COUNT count, const float default_start_time_, const wchar_t* filename, int max_digit)
{
    timer = std::make_unique<Digits<float>>(default_start_time_, max_digit);

    // 三項演算子
    pCount = static_cast<int>(count) ? /* DOWN = true */&Timer::CountDown : /* Up = false */&Timer::CountUp;


    stop = false;


    // 描画する場合のみスプライトの実体を生成
    if (!filename) return;

    // 三項演算子
    pRender = static_cast<int>(max_digit) ? /* (max_digit > 0) == true */&Timer::ClampSizeRender : /* (max_digits == 0) == false */&Timer::FreeSizeRender;

    if (max_digit <= 0) max_digit = 4 /*7桁は１000の位*/;
    spr_number = std::make_unique<Sprite_Batch>(Device,filename,max_digit);

    number_size.x = static_cast<float>(spr_number->GetTextureWidth() / 10);
    number_size.y = static_cast<float>(spr_number->GetTextureHeight());

}


void Timer::Update(float elapsedTime)
{
    if (stop) return;

    // 時間の更新
    (this->*pCount)(elapsedTime);
}


void Timer::SpriteRender(ID3D11DeviceContext* Immediate_Context, DirectX::XMFLOAT2 Pos)
{
    if (spr_number == nullptr) return;


    /* 2Dスプライトの描画 */
    (this->*pRender)(Immediate_Context, Pos);
}


void Timer::CountUp(float elapsedTime)
{
    // タイマーが限界値になったら更新しない
    if (timer->GetDigit() == max_time) return;


    timer->Add(elapsedTime);


    //TODO: 限界値 999.0f　まで
    // 限界地を超えたら
    if (timer->GetDigit() < max_time) return;
    timer->Set(max_time);

}


void Timer::CountDown(float elapsedTime)
{
    // タイマーが０になったら更新しない
    if (timer->GetDigit() == zero_time) return;


    timer->Subtract(-elapsedTime);


    // ゼロより下回ったら
    if (timer->GetDigit() > zero_time) return;
    timer->Set(zero_time);
}


void Timer::ClampSizeRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos)
{
    constexpr DirectX::XMFLOAT2 Scale = { 1.0f,1.0f };


    std::vector<float> digit_place;
    digit_place.resize(timer->max_digits_place);
    timer->SliceDigits(digit_place);

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


void Timer::FreeSizeRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos)
{
    constexpr DirectX::XMFLOAT2 Scale = { 1.0f,1.0f };


    // 数値を文字に変換
    std::string str_number = std::to_string(timer->GetDigit());

    // 現在の文字位置(相対位置)
    float carriage = 0;

    // 一文字づつRender()する
    spr_number->Begin(Immediate_Context);
    for (const char c : str_number)
    {
        int number = (c & 0x0F);
        //文字を表示。アスキーコードの位置にある文字位置を切り抜いて表示
        spr_number->Render(Immediate_Context, { Pos.x + carriage,Pos.y }, Scale, { number_size.x * number, 0.0f }, number_size, { 0.5f,0.5f });

        //文字位置を幅分ずらす
        carriage += number_size.x * Scale.x;
    }
    spr_number->End(Immediate_Context);

}