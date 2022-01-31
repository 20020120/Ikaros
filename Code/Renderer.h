#pragma once

#include"SpriteBatch.h"
#include<memory>
class Renderer
{
public:
    Renderer() = default;
    ~Renderer() = default;
    //void Initialize(ID3D11Device* d, const wchar_t* FileName)
    //{
    //    spr = std::make_unique<Sprite_Batch>(d, FileName, 1);
    //}
    void Initialize(ID3D11Device* d, const wchar_t* FileName, size_t MaxSprite = 1)
    {
        spr = std::make_unique<Sprite_Batch>(d, FileName, MaxSprite);
    }
    void Initialize(ID3D11Device* d, const wchar_t* FileName, const wchar_t* DissolveFileName, const wchar_t* DstFileName)
    {
        spr = std::make_unique<Sprite_Batch>(d, FileName, 1, DissolveFileName, DstFileName);
    }



    void Render(ID3D11DeviceContext* dc)
    {
        spr->Begin(dc);
        spr->render(dc, Position, Scale, TexPos, TexSize, Pivot, Angle, Color);
        spr->End(dc);
    }

    void SetDissolvePower(float p) { spr->SetDissolvePower(p); }

    float GetTextureWidth() const { return spr->GetTextureWidth(); }
    float GetTextureHeight() const { return spr->GetTextureHeight(); }


    std::unique_ptr<Sprite_Batch> spr;

    DirectX::XMFLOAT2 Position = { 0.0f,0.0f };
    DirectX::XMFLOAT2 Scale = { 1.0f,1.0f };
    DirectX::XMFLOAT2 TexSize={0.0f,0.0f};
    DirectX::XMFLOAT2 TexPos = { 0.0f,0.0f };
    DirectX::XMFLOAT2 Pivot = { 0.0f,0.0f };
    DirectX::XMFLOAT4 Color = { 1.0f,1.0f,1.0f,1.0f };
    float Angle;
};
