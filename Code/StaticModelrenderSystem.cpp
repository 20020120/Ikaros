#include"StaticModelRenderSystem.h"

#include<filesystem>
#include <fstream>

#include "misc.h"
#include "TextureLoder.h"

// 初期化
StaticModelRenderSystem::StaticModelRenderSystem(ID3D11Device* d, ModelRenderInitializer Ini)
{
    LoadModel(Ini.GetPath(0));
    CreateComObject(d, Ini);

}

void StaticModelRenderSystem::Render(ID3D11DeviceContext* dc)
{
    HRESULT hr{ S_OK };

    if(DrawValue<=0)   return;
    using namespace DirectX;

    for (ModelRenderResource::Mesh& mesh : Resource.MeshVec)
    {
        uint32_t stride{ sizeof(ModelRenderResource::Vertex) };
        uint32_t strides[2]{ sizeof(ModelRenderResource::Vertex),sizeof(Constants) };

        uint32_t offset{ 0 };
        uint32_t offsets[2]{ 0,0 };


        

        dc->IASetVertexBuffers(0, 1, mesh.VertexBuffer.GetAddressOf(), &stride, &offset);
        


        dc->IASetIndexBuffer(mesh.IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        dc->IASetInputLayout(InputLayout.Get());

        dc->VSSetShader(VertexShader.Get(), nullptr, 0);
        dc->GSSetShader(nullptr, nullptr, 0);
        dc->PSSetShader(PixelShader.Get(), nullptr, 0);

//        ModelRenderResource::Constants data;

        //XMStoreFloat4x4(&data.World, XMLoadFloat4x4(&mesh.DefaultGlobalTransform) * XMLoadFloat4x4(&World));

        for (const ModelRenderResource::Mesh::Subset& subset : mesh.SubsetVec)
        {
            const ModelRenderResource::Material& material{ Resource.MaterialVec.at(subset.MaterialUniqueId) };

            // XMStoreFloat4x4(&data.World, XMLoadFloat4x4(&mesh.DefaultGlobalTransform) * XMLoadFloat4x4(&World));

            //DirectX::XMStoreFloat4(&data.MaterialColor, DirectX::XMLoadFloat4(&MaterialColor) * DirectX::XMLoadFloat4(&material.Kd));


            dc->UpdateSubresource(ConstantBuffer.Get(), 0, 0, &data, 0, 0);
            

            dc->VSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());

            dc->PSSetShaderResources(0, 1, material.ShaderResourceView[0].GetAddressOf());
            dc->PSSetShaderResources(1, 1, material.ShaderResourceView[1].GetAddressOf());

            //dc->DrawIndexed(subset.IndexCount, subset.StartIndexLocation, 0);
            dc->DrawIndexedInstanced(
                subset.IndexCount,
                DrawValue,     // 描画する枚数
                subset.StartIndexLocation,
                0, 
                0); // 描画するスタート位置
        }
    }


    //--------------------<シェーダーの初期化>--------------------//
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->GSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);

 
}


void StaticModelRenderSystem::LoadModel(std::string FilePath)
{
    std::filesystem::path cereal_filename(FilePath);
    cereal_filename.replace_extension("nk");

    std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
    cereal::BinaryInputArchive deserialization(ifs);
    deserialization(Resource);
}

void StaticModelRenderSystem::CreateComObject(ID3D11Device* device, ModelRenderInitializer Ini)
{
    for (ModelRenderResource::Mesh& mesh : Resource.MeshVec)
    {
        HRESULT hr{ S_OK };
        D3D11_BUFFER_DESC buffer_desc{};
        D3D11_SUBRESOURCE_DATA subresource_data{};
        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(ModelRenderResource::Vertex) * mesh.VertexVec.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        subresource_data.pSysMem = mesh.VertexVec.data();
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh.VertexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.IndicesVec.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresource_data.pSysMem = mesh.IndicesVec.data();
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh.IndexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    }


    HRESULT hr = S_OK;

    //  モデルの頂点情報
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA ,0 },

        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

    };

    


    for (auto& it = Resource.MaterialVec.begin(); it != Resource.MaterialVec.end(); ++it)
    {
        if (it->second.TextureFileName[0].size() > 0)
        {
            std::filesystem::path path(Ini.GetPath(ModelRenderInitializer::Type::MODEL).c_str());
            path.replace_filename(it->second.TextureFileName[0]);
            D3D11_TEXTURE2D_DESC texture2ddesc;
            LoadTextureFromFile(device, path.c_str(),
                it->second.ShaderResourceView[0].GetAddressOf(), &texture2ddesc);
        }
        else
        {
            f_MakeDummyTexture(device, it->second.ShaderResourceView[0].GetAddressOf(), 0xFFFFFFFF, 16);
        }

    }

  

    CreateGsFromCso(device, Ini.GetPath(ModelRenderInitializer::Type::GEOMETRY).c_str(), GeometryShader.ReleaseAndGetAddressOf());
    CreateVsFromCso(device, Ini.GetPath(ModelRenderInitializer::Type::VERTEX).c_str(), VertexShader.ReleaseAndGetAddressOf(),
        InputLayout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));

    CreatePsFromCso(device, Ini.GetPath(ModelRenderInitializer::Type::PIXEL).c_str(), PixelShader.ReleaseAndGetAddressOf());


    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(Constants);
    //buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;

    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = device->CreateBuffer(&buffer_desc, nullptr, ConstantBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}


