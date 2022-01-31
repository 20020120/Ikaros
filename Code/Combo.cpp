
#include "Combo.h"

#include <string>

#include "SpriteBatch.h"



void Combo::Initialize(ID3D11Device* Device)
{
    Clear();

    timer = std::make_unique<Timer>();
    timer->Initialize(Device, COUNT::DOWN, 3);

    //spr_icon = std::make_unique<Sprite_Batch>(Device, L"Data/Assets/Texture/combo.png");
    spr_number = std::make_unique<Sprite_Batch>(Device, L"./resources/Sprite/UIs/yone.png", 3);

    number_size.x = spr_number->GetTextureWidth() / 10;
    number_size.y = spr_number->GetTextureHeight();
}

void Combo::Update(float ElapsedTime)
{

    // コンボの持続時間の更新
    if (timer->NowTime() && is_combo)
    {

        timer->Update(ElapsedTime);


        if (timer->NowTime() <= 0.0f)
        {
            Prepare();
        }
    }

    // 最大コンボ数の記録
    if (combo > max_comboed) max_comboed = combo;
}

void Combo::SpriteRender(ID3D11DeviceContext* Immediate_Context ,const DirectX::XMFLOAT2 Pos)
{
    if (!is_combo)return;

    FreeSizeRender(Immediate_Context, Pos);
}


void Combo::Finalize()
{
}


void Combo::FreeSizeRender(ID3D11DeviceContext* Immediate_Context, const DirectX::XMFLOAT2& Pos)
{
    constexpr DirectX::XMFLOAT2 Scale = { 1.0f,1.0f };


    // 数値を文字に変換
    std::string str_number = std::to_string(combo);

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