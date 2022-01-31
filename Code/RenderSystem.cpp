#include"RenderSystem.h"

#include<filesystem>
#include <fstream>
#include"misc.h"
#include"TextureLoder.h"

ModelRenderSystem::ModelRenderSystem(ID3D11Device* device, ModelRenderInitializer Ini)
{
    LoadModel(Ini.GetPath(0));
    CreateComObject(device, Ini);
}

ModelRenderSystem::~ModelRenderSystem()
{

}

void ModelRenderSystem::Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& World,
    const DirectX::XMFLOAT4& MaterialColor, const ModelAnimationResource::KeyFrame* keyframe)
{
    using namespace DirectX;


    for (ModelRenderResource::Mesh& mesh : Resource.MeshVec)
    {
        uint32_t stride{ sizeof(ModelRenderResource::Vertex) };
        uint32_t offset{ 0 };
        dc->IASetVertexBuffers(0, 1, mesh.VertexBuffer.GetAddressOf(), &stride, &offset);
        dc->IASetIndexBuffer(mesh.IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        dc->IASetInputLayout(InputLayout.Get());

        dc->VSSetShader(VertexShader.Get(), nullptr, 0);
        dc->GSSetShader(GeometryShader.Get(), nullptr, 0);
        dc->PSSetShader(PixelShader.Get(), nullptr, 0);


        ModelRenderResource::Constants data;
        

        if (keyframe && keyframe->NodeVec.size() > 0)
        {
            const ModelAnimationResource::KeyFrame::Node& meshnode{ keyframe->NodeVec.at(mesh.NodeIndex) };
            XMStoreFloat4x4(&data.World, XMLoadFloat4x4(&meshnode.GlobalTransform) * XMLoadFloat4x4(&World));

            const size_t bone_count{ mesh.BindPose.BoneVec.size() };
            _ASSERT_EXPR(bone_count < ModelRenderResource::MAXBONEVALUE, L"The value of the 'bone_count' has exceeded MAX_BONES.");



            for (int bone_index = 0; bone_index < bone_count; ++bone_index)
            {
                const ModelRenderResource::Skelton::Bone& bone{ mesh.BindPose.BoneVec.at(bone_index) };
                const ModelAnimationResource::KeyFrame::Node& bone_node{ keyframe->NodeVec.at(bone.NodeIndex) };
                XMStoreFloat4x4(&data.BoneTransform[bone_index],
                    XMLoadFloat4x4(&bone.offset_transform) *
                    XMLoadFloat4x4(&bone_node.GlobalTransform) *
                    XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.DefaultGlobalTransform))
                );
            }
        }
        else
        {


            XMStoreFloat4x4(&data.World, XMLoadFloat4x4(&mesh.DefaultGlobalTransform) * XMLoadFloat4x4(&World));

            for (size_t boneindex = 0; boneindex < ModelRenderResource::MAXBONEVALUE; ++boneindex)
            {
                data.BoneTransform[boneindex] = {
                    1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1
                };
            }
        }
        
        for (const ModelRenderResource::Mesh::Subset& subset : mesh.SubsetVec)
        {
            const ModelRenderResource::Material& material{ Resource.MaterialVec.at(subset.MaterialUniqueId) };

            // XMStoreFloat4x4(&data.World, XMLoadFloat4x4(&mesh.DefaultGlobalTransform) * XMLoadFloat4x4(&World));

            DirectX::XMStoreFloat4(&data.MaterialColor, DirectX::XMLoadFloat4(&MaterialColor) * DirectX::XMLoadFloat4(&material.Kd));
            dc->UpdateSubresource(ConstantBuffer.Get(), 0, 0, &data, 0, 0);
            dc->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
            dc->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());

            for (int i = 0; i < ARRAYSIZE(material.ShaderResourceView); i++)
            {
                dc->PSSetShaderResources(i, 1, material.ShaderResourceView[i].GetAddressOf());
            }
            //dc->PSSetShaderResources(1, 1, material.ShaderResourceView[1].GetAddressOf());

            dc->DrawIndexed(subset.IndexCount, subset.StartIndexLocation, 0);
        }
    }
    //--------------------<シェーダーの初期化>--------------------//
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->GSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);

}

void ModelRenderSystem::LoadModel(std::string FilePath)
{
    std::filesystem::path cereal_filename(FilePath);
    cereal_filename.replace_extension("nk");

    std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
    cereal::BinaryInputArchive deserialization(ifs);
    deserialization(Resource,AnimeResources);
}



void ModelRenderSystem::CreateComObject(ID3D11Device* device,ModelRenderInitializer Ini)
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
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA ,0 },

        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

        { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

        { "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

        { "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

    };



    for (auto& it = Resource.MaterialVec.begin(); it != Resource.MaterialVec.end(); ++it)
    {
        for (size_t texture_index = 0; texture_index < TextureType::Value; ++texture_index)
        {
            if (it->second.TextureFileName[texture_index].size() > 0)
            {

                std::filesystem::path path(Ini.GetPath(0));
                std::filesystem::path path2(it->second.TextureFileName[texture_index]);
                path.replace_filename(path2);

                D3D11_TEXTURE2D_DESC texture2d_desc;
                LoadTextureFromFile(device, path.c_str(),
                    it->second.ShaderResourceView[texture_index].GetAddressOf(), &texture2d_desc);
            }
            else
            {
                f_MakeDummyTexture(device,
                    it->second.ShaderResourceView[texture_index].GetAddressOf(),
                    texture_index == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);
            }
        }


    }

    CreateGsFromCso(device, Ini.GetPath(ModelRenderInitializer::Type::GEOMETRY).c_str(), GeometryShader.ReleaseAndGetAddressOf());

    CreateVsFromCso(device, Ini.GetPath(ModelRenderInitializer::Type::VERTEX).c_str(), VertexShader.ReleaseAndGetAddressOf(),
        InputLayout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));


    CreatePsFromCso(device, Ini.GetPath(ModelRenderInitializer::Type::PIXEL).c_str(), PixelShader.ReleaseAndGetAddressOf());





    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(ModelRenderResource::Constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, nullptr, ConstantBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


//****************************************************************
// 
// モデルを初期化するためのクラス
// 
//****************************************************************
void ModelRenderInitializer::SetALLPath(std::string M, std::string Vs, std::string Ps, std::string Gs)
{
    ModelPath = M;
    VS_ShaderPath = Vs;
    PS_ShaderPath = Ps;
    GS_ShaderPath = Gs;
}

void ModelRenderInitializer::SetModelPath(std::string Path)
{
    ModelPath = Path;
}

void ModelRenderInitializer::SetVsPath(std::string Path)
{
    VS_ShaderPath = Path;
}

void ModelRenderInitializer::SetPsPath(std::string Path)
{
    PS_ShaderPath = Path;
}

void ModelRenderInitializer::SetGsPath(std::string Path)
{
    GS_ShaderPath = Path;
}

const std::string ModelRenderInitializer::GetPath(unsigned short t)
{
    switch (t)
    {
    case ModelRenderInitializer::MODEL:
        return ModelPath;
    case ModelRenderInitializer::VERTEX:
        return VS_ShaderPath;
    case ModelRenderInitializer::PIXEL:
        return PS_ShaderPath;
    case ModelRenderInitializer::GEOMETRY:
        return GS_ShaderPath;
    default:
        assert("There Is Not That Type");
        break;
    }
}
