#include "PBRShader.h"

#include <WICTextureLoader.h>
#include "misc.h"


PBRShader::PBRShader(const WCHAR* baseColorPath, const WCHAR* metallicPath, const WCHAR* roughnessPath)
{
    str_BaseColor = baseColorPath;
    str_Metallic = metallicPath;
    str_Roughness = roughnessPath;
}

PBRShader::~PBRShader()
{
    
}


//****************************************************************
// 
// ShaderResourceViewを初期化
// 
//****************************************************************
void PBRShader::Initialize(ID3D11Device* d)
{
    HRESULT hr{ S_OK };

    //--------------------<テクスチャの読み込み>--------------------//

    // BaseColor
    hr = DirectX::CreateWICTextureFromFile(
        d,                                                     // ID3D11Device
        str_BaseColor.c_str(),                                 // テクスチャのパス
        nullptr,                                               // ID3D11Resource
        SRV_BaseColor.GetAddressOf()                           // シェーダーリソースビュー
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


    // Metallic
    hr = DirectX::CreateWICTextureFromFile(
        d,                                                     // ID3D11Device
        str_Metallic.c_str(),                                  // テクスチャのパス
        nullptr,                                               // ID3D11Resource
        SRV_Metallic.GetAddressOf()                            // シェーダーリソースビュー
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


    // Roughness
    hr = DirectX::CreateWICTextureFromFile(
        d,                                                     // ID3D11Device
        str_Roughness.c_str(),                                  // テクスチャのパス
        nullptr,                                               // ID3D11Resource
        SRV_Roughness.GetAddressOf()                            // シェーダーリソースビュー
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


//****************************************************************
// 
//  更新処理
// 
//****************************************************************
void PBRShader::Update(ID3D11DeviceContext* dc)
{
    //--------------------<PSShaderにセット>--------------------//

    // BaseColor（Slot==2）
    dc->PSSetShaderResources(2, 1, SRV_BaseColor.GetAddressOf());

    // Metallic （Slot==3）
    dc->PSSetShaderResources(3, 1, SRV_Metallic.GetAddressOf());

    // Roughness（Slot==4）
    dc->PSSetShaderResources(4, 1, SRV_Roughness.GetAddressOf());

}
