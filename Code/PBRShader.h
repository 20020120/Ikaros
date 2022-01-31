#pragma once

#include "Shader.h"

#include <wrl.h>
#include<string>
#include <vector>

class PBRShader :public Shader
{
public:
    PBRShader(
              const WCHAR* baseColorPath,  // ベースカラー
              const WCHAR* metallicPath,   // メタリック
              const WCHAR* roughnessPath   // 粗さ
             );

    ~PBRShader();

    void Initialize(ID3D11Device* d) override;
    void Update(ID3D11DeviceContext* dc) override;
private:
    // モデルの色
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV_BaseColor;
    // メタリック
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV_Metallic;
    // 粗さ
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV_Roughness;

    std::wstring str_BaseColor{};
    std::wstring str_Metallic{};
    std::wstring str_Roughness{};

}; 
