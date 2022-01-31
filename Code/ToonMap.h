#pragma once

#include"TextureLoder.h"

#include <cstdint>

#include<wrl.h>
#include"misc.h"


class ToonMap
{
public:
    // 初期化
    ToonMap(ID3D11Device* d, uint16_t slot);
    ~ToonMap() {}

    // PS_Shaderにセット
    void SetShaderResourceView(ID3D11DeviceContext* dc);
private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv{nullptr};

    int16_t Slot{ -1 };
};


//--------------------<初期化>--------------------//
inline ToonMap::ToonMap(ID3D11Device* d, uint16_t slot) {
    // テクスチャの情報を送るスロットを設定
    Slot = slot;

    HRESULT hr{ S_OK };

    // テクスチャを読み込む
    hr = DirectX::CreateWICTextureFromFile(
        d,                                                     // ID3D11Device
        L"./resources/Sprite/Shaders/ToonMap/ToonMap.png",     // テクスチャのパス
        nullptr,                                               // ID3D11Resource
        srv.GetAddressOf()                                     // シェーダーリソースビュー
    );

    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

inline void ToonMap::SetShaderResourceView(ID3D11DeviceContext* dc)
{
    // PS_Shaderにテクスチャを設定する

    assert(Slot != -1);  // PS_Shaderのどのスロットにテクスチャを送るか指定してください

    dc->PSSetShaderResources(Slot, 1, srv.GetAddressOf());
}