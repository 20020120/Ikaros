#pragma once

#include "RenderSystem.h"

class StaticModelRenderSystem
{
public:

    //--------------------<�C���X�^���X�`��̍ő�l>--------------------//
    static constexpr int MaxValue = 1500;

    struct Constants
    {
        DirectX::XMFLOAT4 Position[MaxValue]{}; // �ʒu
        DirectX::XMFLOAT4 Color[MaxValue]{};
        
    };

    Constants data{};

    StaticModelRenderSystem(ID3D11Device* d, ModelRenderInitializer ini);
    ~StaticModelRenderSystem() = default;


    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************

    void Render(ID3D11DeviceContext* dc);

    // �C�����C���֐�
    void SetWorldMat(const int Index, const DirectX::XMFLOAT4X4 W)
    {
        assert(Index < MaxValue);  // �����̍ő�l���I�[�o�[
       
        DrawValue = Index;
    }

    void SetPositionColor(const int Index, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4& color)
    {
        assert(Index < MaxValue);  // �����̍ő�l���I�[�o�[
        data.Position[Index] =
        {
            pos.x,
            pos.y,
            pos.z,
            0.0f
        };

        data.Color[Index] = color;
        DrawValue = Index;
    }


protected:
    //--------------------<.nk�t�@�C�������[�h>--------------------//
    void LoadModel(std::string FilePath);

    //--------------------<Com�I�u�W�F�N�g�𐶐�>--------------------//
    void CreateComObject(ID3D11Device* device, ModelRenderInitializer Ini);

    ModelRenderResource Resource;
    std::vector<ModelAnimationResource> AnimeResources;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
    Microsoft::WRL::ComPtr <ID3D11PixelShader> PixelShader;
    Microsoft::WRL::ComPtr <ID3D11GeometryShader> GeometryShader;

    Microsoft::WRL::ComPtr <ID3D11InputLayout> InputLayout;
    Microsoft::WRL::ComPtr <ID3D11Buffer> ConstantBuffer;

    Microsoft::WRL::ComPtr<ID3D11Buffer> ModelDataBuffer;
    


    int DrawValue;  //�P�t���[���ɕ`�悷���


   

  

};

