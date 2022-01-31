#pragma once
#include<DirectXMath.h>
#include<d3d11.h>
#include<wrl.h>
#include<string>
using namespace Microsoft::WRL;

//#include"BoundingBox.h"

class StaticMesh
{
public:

    struct Vertex
    {
        DirectX::XMFLOAT3 Position{};
        DirectX::XMFLOAT3 Nomal{};
        DirectX::XMFLOAT2 Texcoord{};
    };

    struct Constants
    {
        DirectX::XMFLOAT4X4 World{};
        DirectX::XMFLOAT4 MaterialColor{};
    };

    struct Subset
    {
        std::wstring UseMtl{};
        uint32_t IndexStart{};
        uint32_t IndexCount{};
    };



private:

    Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer{};
    Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer{};

    Microsoft::WRL::ComPtr<ID3D11VertexShader>  VertexShader{};
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  PixelShader{};
    Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout{};
    Microsoft::WRL::ComPtr<ID3D11Buffer>  ConstantBuffer{};

    //****************************************************************
    // 
    // UNIT14
    // 
    //****************************************************************
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView{};
    //std::wstring TextureFileName{};


    //****************************************************************
    // 
    // UNIT15
    // 
    //****************************************************************
    std::vector<Subset> SubsetVec{};

    struct Material
    {
        std::wstring Name{};
        DirectX::XMFLOAT4 Ka{ 0.2f,0.2f,0.2f,1.0f };
        DirectX::XMFLOAT4 Kd{ 0.8f,0.8f,0.8f,1.0f };
        DirectX::XMFLOAT4 Ks{ 1.0f,1.0f,1.0f,1.0f };

        std::wstring TextureFileName[2];
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView[2];
    };

    std::vector<Material> MaterialVec{};

    //****************************************************************
    // 
    // BoundingBox
    // 
    //****************************************************************

    //std::unique_ptr<BoundingBox> boundingbox{};


    
    
public:
    StaticMesh(ID3D11Device*Device,const wchar_t* ObjFileName, bool IsFlip);
    virtual~StaticMesh() = default;

    void Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& World,
        const DirectX::XMFLOAT4& MaterialColor, ID3D11RasterizerState** RasterizerState);

protected:
    void CreateComBuffers(ID3D11Device* Device, Vertex* Vertices, size_t VertexCount, uint32_t* Indices, size_t IndexCount);

};
