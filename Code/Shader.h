#pragma once

#include <d3d11.h>

//****************************************************************
// 
// Shader基底クラス
// 
//****************************************************************

class Shader
{
public:
    Shader() = default;
    virtual ~Shader() = default;

    // 初期化
    virtual  void Initialize(ID3D11Device* d) = 0;
    // 更新処理
    virtual  void Update(ID3D11DeviceContext* dc) = 0;
};
