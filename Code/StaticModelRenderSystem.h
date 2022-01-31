#pragma once

#include "RenderSystem.h"

class StaticModelRenderSystem
{
public:

    //--------------------<インスタンス描画の最大値>--------------------//
    static constexpr int MaxValue = 1500;

    struct Constants
    {
        DirectX::XMFLOAT4 Position[MaxValue]{}; // 位置
        DirectX::XMFLOAT4 Color[MaxValue]{};
        
    };

    Constants data{};

    StaticModelRenderSystem(ID3D11Device* d, ModelRenderInitializer ini);
    ~StaticModelRenderSystem() = default;


    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************

    void Render(ID3D11DeviceContext* dc);

    // インライン関数
    void SetWorldMat(const int Index, const DirectX::XMFLOAT4X4 W)
    {
        assert(Index < MaxValue);  // 生成の最大値をオーバー
       
        DrawValue = Index;
    }

    void SetPositionColor(const int Index, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4& color)
    {
        assert(Index < MaxValue);  // 生成の最大値をオーバー
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
    //--------------------<.nkファイルをロード>--------------------//
    void LoadModel(std::string FilePath);

    //--------------------<Comオブジェクトを生成>--------------------//
    void CreateComObject(ID3D11Device* device, ModelRenderInitializer Ini);

    ModelRenderResource Resource;
    std::vector<ModelAnimationResource> AnimeResources;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
    Microsoft::WRL::ComPtr <ID3D11PixelShader> PixelShader;
    Microsoft::WRL::ComPtr <ID3D11GeometryShader> GeometryShader;

    Microsoft::WRL::ComPtr <ID3D11InputLayout> InputLayout;
    Microsoft::WRL::ComPtr <ID3D11Buffer> ConstantBuffer;

    Microsoft::WRL::ComPtr<ID3D11Buffer> ModelDataBuffer;
    


    int DrawValue;  //１フレームに描画する個数


   

  

};

