#pragma once
#include "StaticModelRenderSystem.h"
#include"Leaf.h"


//****************************************************************
// 
//  アニメーションがないモデル
// 
//****************************************************************
class StaticModelComponent
{
public:
    StaticModelComponent(ID3D11Device* device, ModelRenderInitializer Ini);
    ~StaticModelComponent();

    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************

    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);

    // インライン関数
    void SetWorldMat(const int Index, const DirectX::XMFLOAT4X4 W) const
    {
        System->SetWorldMat(Index, W);
    }

    void SetPositionColor(const int Index, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4& color)
    {
        System->SetPositionColor(Index, pos, color);
    }

private:
    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************
    // 描画のシステムクラス
    std::shared_ptr<StaticModelRenderSystem> System;
    //色
    DirectX::XMFLOAT4 MaterialColor;





};
