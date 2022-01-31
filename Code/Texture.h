#pragma once
#include <cassert>
#include <d3d11.h>
#include<wrl.h>
#include<WICTextureLoader.h>
#include "misc.h"

//****************************************************************
// 
//  テクスチャを送る
// 
//****************************************************************
struct TextureResource
{
    int Slot = -1; // セットするスロット
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV=nullptr;
};


class Texture
{
public:
    Texture() = default;
    ~Texture() = default;

    // テクスチャを登録する
    void Register(ID3D11Device* d, int slot, const wchar_t* texturePath)
    {
        HRESULT hr = S_OK;

        // リソースの初期化
        resource.Slot = slot;

        // テクスチャの読み込み
        hr = DirectX::CreateWICTextureFromFile(
            d,
            texturePath,
            nullptr,
            resource.SRV.GetAddressOf()
        );

        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    // ピクセルシェーダーに送る
    void SetTexture(ID3D11DeviceContext* dc)
    {
        assert(resource.Slot != -1); // PixelShaderのどのスロットにテクスチャを送るか指定してください

        dc->PSSetShaderResources(resource.Slot, 1, resource.SRV.GetAddressOf());
    }

private:
    TextureResource resource;
};