#pragma once
#include "StaticModelRenderSystem.h"
#include"Leaf.h"


//****************************************************************
// 
//  �A�j���[�V�������Ȃ����f��
// 
//****************************************************************
class StaticModelComponent
{
public:
    StaticModelComponent(ID3D11Device* device, ModelRenderInitializer Ini);
    ~StaticModelComponent();

    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************

    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);

    // �C�����C���֐�
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
    // �ϐ�
    // 
    //****************************************************************
    // �`��̃V�X�e���N���X
    std::shared_ptr<StaticModelRenderSystem> System;
    //�F
    DirectX::XMFLOAT4 MaterialColor;





};
