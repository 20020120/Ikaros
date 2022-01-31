

#include<Windows.h>
#include<filesystem>
#include"misc.h"
#include "SkinnedMesh.h"
#include"TextureLoder.h"
#include<functional>
#include<sstream>
//#include"Win32Wrapper.h"

#include <fstream>
#include <stdio.h>


#define KFBX_ANSI_to_UTF8(inANSI, outUTF8) { \
    wchar_t tempWideChar[1024]; \
    MultiByteToWideChar( CP_ACP, 0, inANSI, -1, tempWideChar, 1024); \
    WideCharToMultiByte( CP_UTF8, 0, tempWideChar, -1, outUTF8, 1024, NULL, NULL); \
}
  


SkinnedMesh::SkinnedMesh(ID3D11Device* Device, const char* FbxFilename, float sampling_rate, bool Triangulate)
{
    FilePath = FbxFilename;
    std::filesystem::path cereal_filename(FbxFilename);
    cereal_filename.replace_extension("nk");
    if (std::filesystem::exists(cereal_filename.c_str()))
    {
        f_DeSerealize();
    }
    else
    {
        //--------------------<文字列を日本語対応させる>--------------------//
        char UFT8filepath[256];
        KFBX_ANSI_to_UTF8(FbxFilename, UFT8filepath);
        
           
        FbxManager* fbx_manager{ FbxManager::Create() };
        FbxScene* fbx_scene{ FbxScene::Create(fbx_manager,"") };

        FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager,"") };

        bool import_status{ false };
        import_status = fbx_importer->Initialize(UFT8filepath);

        _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

        import_status = fbx_importer->Import(fbx_scene);
        _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

        FbxGeometryConverter fbx_converter(fbx_manager);
        if (Triangulate)
        {
            fbx_converter.Triangulate(fbx_scene, true/*Replace*/, false/*Legacy*/);
            fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
        }


        std::function<void(FbxNode*)> traverse{ [&](FbxNode* fbx_node)
            {
                SceneOfFbx::Node& node{SceneView.NodeVec.emplace_back()};
                node.Attribute = fbx_node->GetNodeAttribute() ?
                    fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
                node.Name = fbx_node->GetName();
                node.UniqueId = fbx_node->GetUniqueID();

                node.ParentIndex = SceneView.IndexOf(
                    fbx_node->GetParent() ?
                    fbx_node->GetParent()->GetUniqueID() : 0
                );

                for (int child_index = 0; child_index < fbx_node->GetChildCount(); child_index++)
                {
                    traverse(fbx_node->GetChild(child_index));
                }
            }
        };

        traverse(fbx_scene->GetRootNode());


        f_FetchMeshVec(fbx_scene, MeshVec);

        //****************************************************************
        // 
        // UNIT19
        // 
        //****************************************************************
        f_FetchMaterialVec(fbx_scene, MaterialVec);

        if (static_cast<int>(MaterialVec.size()) <= 0)
        {
            f_FetchDummyMaterialVec(fbx_scene, MaterialVec);
        }



        //--------------------<アニメーションデータの取得>--------------------//

        f_FetchAnimation(fbx_scene, AnimationVec, sampling_rate);
        if (static_cast<int>(AnimationVec.size()) <= 0)
        {
            f_FetchDummyAnimation(fbx_scene, AnimationVec, sampling_rate);
        }

        //--------------------<ダミーのフラグ変数の追加>--------------------//
        if(EventFlagMap.size()<=0)
        f_FetchDummyEventFlag();
        f_FetchDummySoundFlag();

        fbx_manager->Destroy();
     

    }
    f_CreateComObject(Device, FbxFilename);

}





void SkinnedMesh::f_FetchMeshVec(FbxScene* fbx_scene, std::vector<Mesh>& meshes)
{
    for (const SceneOfFbx::Node& node : SceneView.NodeVec)
    {
        if (node.Attribute != FbxNodeAttribute::EType::eMesh)
        {
            continue;
        }

        FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.Name.c_str()) };
        FbxMesh* fbx_mesh{ fbx_node->GetMesh() };

        Mesh& mesh{ meshes.emplace_back() };
        mesh.UniqueId = fbx_mesh->GetNode()->GetUniqueID();
        mesh.Name = fbx_mesh->GetNode()->GetName();
        mesh.NodeIndex = SceneView.IndexOf(mesh.UniqueId);
        mesh.DefaultGlobalTransform=f_ToXMFLOAT4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());
        

        std::vector<BoneInfluence_Per_ControlPoint> bone_influences;
        f_FetchBoneInfluences(fbx_mesh, bone_influences);

        f_FetchSkelton(fbx_mesh, mesh.BindPose);

        std::vector<Mesh::Subset>& subsetvec{ mesh.SubsetVec };
        const int materialcount{ fbx_mesh->GetNode()->GetMaterialCount() };
        subsetvec.resize(materialcount > 0 ? materialcount : 1);

        for (int materialindex = 0; materialindex < materialcount; ++materialindex)
        {
            const FbxSurfaceMaterial* fbxmaterial{ fbx_mesh->GetNode()->GetMaterial(materialindex) };
            subsetvec.at(materialindex).MaterialName = fbxmaterial->GetName();
            subsetvec.at(materialindex).MaterialUniqueId = fbxmaterial->GetUniqueID();
        }
        if (materialcount > 0)
        {
            const int polygon_count{ fbx_mesh->GetPolygonCount() };
            for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
            {
                const int materialindex{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
                subsetvec.at(materialindex).IndexCount += 3;
            }

            uint32_t offset{ 0 };
            for (auto& it : subsetvec)
            {
                it.StartIndexLocation = offset;
                offset += it.IndexCount;
                it.IndexCount = 0;
            }
        }

        const int polygon_count{ fbx_mesh->GetPolygonCount() };
        mesh.VertexVec.resize(polygon_count * 3LL);
        mesh.IndicesVec.resize(polygon_count * 3LL);

        FbxStringList uv_names;
        fbx_mesh->GetUVSetNames(uv_names);
        const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };
        for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
        {
            const int materialindex{ materialcount > 0 ?
            fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };

            Mesh::Subset& subset{ subsetvec.at(materialindex) };
            const uint32_t offset{ subset.StartIndexLocation + subset.IndexCount };


            for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
            {
                const int vertex_index{ polygon_index * 3 + position_in_polygon };

                Vertex vertex;
                const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index, position_in_polygon) };
                vertex.Position.x = static_cast<float>(control_points[polygon_vertex][0]);
                vertex.Position.y = static_cast<float>(control_points[polygon_vertex][1]);
                vertex.Position.z = static_cast<float>(control_points[polygon_vertex][2]);

                const BoneInfluence_Per_ControlPoint& influences_per_control_point
                { bone_influences.at(polygon_vertex) };
                for (size_t influence_index = 0; influence_index < influences_per_control_point.size();
                    ++influence_index)
                {
                    
                   // int a = sizeof(vertex.BoneWeights) / sizeof(float);
                    if (influence_index < sizeof(vertex.BoneWeights)/sizeof(float))
                    {
                        vertex.BoneWeights[influence_index] =
                            influences_per_control_point.at(influence_index).BoneWeight;
                        vertex.BoneIndeices[influence_index] =
                            influences_per_control_point.at(influence_index).BoneIndex;
                    }
                }


                if (fbx_mesh->GetElementNormalCount() > 0)
                {
                    FbxVector4 normal;
                    fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
                    vertex.Nomal.x = static_cast<float>(normal[0]);
                    vertex.Nomal.y = static_cast<float>(normal[1]);
                    vertex.Nomal.z = static_cast<float>(normal[2]);
                }
                if (fbx_mesh->GetElementUVCount() > 0)
                {
                    FbxVector2 uv;
                    bool unmapped_uv;
                    fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon,
                        uv_names[0], uv, unmapped_uv);
                    vertex.Texcoord.x = static_cast<float>(uv[0]);
                    vertex.Texcoord.y = 1.0f - static_cast<float>(uv[1]);
                }

                //--------------------<法線情報を取得>--------------------//
                if (fbx_mesh->GenerateTangentsData(0, false))
                {
                    const FbxGeometryElementTangent* tangent = fbx_mesh->GetElementTangent(0);
                    vertex.Tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[0]);
                    vertex.Tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[1]);
                    vertex.Tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[2]);
                    vertex.Tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[3]);
                }
            

                mesh.VertexVec.at(vertex_index) = std::move(vertex);
                mesh.IndicesVec.at(static_cast<size_t>(offset) + position_in_polygon) = vertex_index;

               

                subset.IndexCount++;
            }

        }

        for (const Vertex& v : mesh.VertexVec)
        {
            mesh.BoundingBox[0].x = std::min<float>(mesh.BoundingBox[0].x, v.Position.x);
            mesh.BoundingBox[0].y = std::min<float>(mesh.BoundingBox[0].y, v.Position.y);
            mesh.BoundingBox[0].z = std::min<float>(mesh.BoundingBox[0].z, v.Position.z);
            mesh.BoundingBox[1].x = std::max<float>(mesh.BoundingBox[1].x, v.Position.x);
            mesh.BoundingBox[1].y = std::max<float>(mesh.BoundingBox[1].y, v.Position.y);
            mesh.BoundingBox[1].z = std::max<float>(mesh.BoundingBox[1].z, v.Position.z);
        }
    }



}



void SkinnedMesh::f_CreateComObject(ID3D11Device* Device, const char* FbxFilename)
{
    for (Mesh& mesh : MeshVec)
    {
        HRESULT hr{ S_OK };
        D3D11_BUFFER_DESC buffer_desc{};
        D3D11_SUBRESOURCE_DATA subresource_data{};
        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.VertexVec.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        subresource_data.pSysMem = mesh.VertexVec.data();
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;
        hr = Device->CreateBuffer(&buffer_desc, &subresource_data,
             mesh.VertexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.IndicesVec.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresource_data.pSysMem = mesh.IndicesVec.data();
        hr = Device->CreateBuffer(&buffer_desc, &subresource_data,
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


    for (auto& it = MaterialVec.begin(); it != MaterialVec.end(); ++it)
    {
#if 1
        if (it->second.TextureFileName[0].size() > 0)
        {
            std::filesystem::path path(FbxFilename);
            path.replace_filename(it->second.TextureFileName[0]);
            D3D11_TEXTURE2D_DESC texture2ddesc;
            LoadTextureFromFile(Device, path.c_str(),
                it->second.ShaderResourceView[0].GetAddressOf(), &texture2ddesc);
        }
        else
        {
            f_MakeDummyTexture(Device, it->second.ShaderResourceView[0].GetAddressOf(), 0xFFFFFFFF, 16);
        }


#else
        for (size_t texture_index = 0; texture_index < 2; ++texture_index)
        {
            if (it->second.TextureFileName[texture_index].size() > 0)
            {
                std::filesystem::path path(FbxFilename);
                path.replace_filename(it->second.TextureFileName[texture_index]);
                D3D11_TEXTURE2D_DESC texture2d_desc;
                LoadTextureFromFile(Device, path.c_str(),
                    it->second.ShaderResourceView[texture_index].GetAddressOf(), &texture2d_desc);
            }
            else
            {
                f_MakeDummyTexture(Device,
                    it->second.ShaderResourceView[texture_index].GetAddressOf(),
                    texture_index == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);
            }
        }



#endif // 0
    }



    //MY_MEMO：気に入らん
    char* path = WinAPIWrapper::Instance().Getfiledialog()->GetDirectory();
    std::string vsfile = path;
    vsfile += "/SkinnedMesh_VS.cso";
    std::string psfile = path;
    psfile += "/SkinnedMesh_PS.cso";


    CreateVsFromCso(Device, vsfile.c_str(), VertexShader.ReleaseAndGetAddressOf(),
        InputLayout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    CreatePsFromCso(Device, psfile.c_str(), PixelShader.ReleaseAndGetAddressOf());

    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(Constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = Device->CreateBuffer(&buffer_desc, nullptr, ConstantBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}












void SkinnedMesh::f_UpdateAnimation(Animation::KeyFrame& keyframe)
{
    
    if (!&keyframe) return;
    using namespace DirectX;
    size_t node_count{ keyframe.NodeVec.size() };
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        Animation::KeyFrame::Node& node{ keyframe.NodeVec.at(node_index) };
        XMMATRIX S{ XMMatrixScaling(node.Scaling.x, node.Scaling.y, node.Scaling.z) };
        XMMATRIX R{ XMMatrixRotationQuaternion(XMLoadFloat4(&node.Rotation)) };
        XMMATRIX T{ XMMatrixTranslation(node.Translation.x, node.Translation.y, node.Translation.z) };

        int64_t parent_index{ SceneView.NodeVec.at(node_index).ParentIndex };
        XMMATRIX P{ parent_index < 0 ? XMMatrixIdentity() :
            XMLoadFloat4x4(&keyframe.NodeVec.at(parent_index).GlobalTransform) };

        XMStoreFloat4x4(&node.GlobalTransform, S * R * T * P);
    }

}

bool SkinnedMesh::f_AppendAnimation(const char* animation_filename, float sampling_rate)
{
    //--------------------<文字列を日本語対応させる>--------------------//
    char UFT8filepath[256];
    KFBX_ANSI_to_UTF8(animation_filename, UFT8filepath);

    FbxManager* fbx_manager{ FbxManager::Create() };
    FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };

    FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };
    bool import_status{ false };

    import_status = fbx_importer->Initialize(UFT8filepath);

    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
    import_status = fbx_importer->Import(fbx_scene);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

    f_FetchAnimation(fbx_scene, AnimationVec, sampling_rate);

    fbx_manager->Destroy();

    return true;

}


void SkinnedMesh::f_BlendAnimation(const Animation::KeyFrame* keyframes[2], float factor, Animation::KeyFrame& keyframe)
{
    using namespace DirectX;

    size_t node_count{ keyframes[0]->NodeVec.size() };
    keyframe.NodeVec.resize(node_count);
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        XMVECTOR S[2]{
            XMLoadFloat3(&keyframes[0]->NodeVec.at(node_index).Scaling),
            XMLoadFloat3(&keyframes[1]->NodeVec.at(node_index).Scaling) };
        XMStoreFloat3(&keyframe.NodeVec.at(node_index).Scaling, XMVectorLerp(S[0], S[1], factor));

        XMVECTOR R[2]{
            XMLoadFloat4(&keyframes[0]->NodeVec.at(node_index).Rotation),
            XMLoadFloat4(&keyframes[1]->NodeVec.at(node_index).Rotation) };
        XMStoreFloat4(&keyframe.NodeVec.at(node_index).Rotation, XMQuaternionSlerp(R[0], R[1], factor));

        XMVECTOR T[2]{ XMLoadFloat3(&keyframes[0]->NodeVec.at(node_index).Translation),
            XMLoadFloat3(&keyframes[1]->NodeVec.at(node_index).Translation) };
        XMStoreFloat3(&keyframe.NodeVec.at(node_index).Translation, XMVectorLerp(T[0], T[1], factor));
    }


}

void SkinnedMesh::Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& World,
    const DirectX::XMFLOAT4& MaterialColor, const Animation::KeyFrame* keyframe)
{
    using namespace DirectX;


    for (Mesh& mesh : MeshVec)
    {
        uint32_t stride{ sizeof(Vertex) };
        uint32_t offset{ 0 };
        dc->IASetVertexBuffers(0, 1, mesh.VertexBuffer.GetAddressOf(), &stride, &offset);
        dc->IASetIndexBuffer(mesh.IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        dc->IASetInputLayout(InputLayout.Get());

        dc->VSSetShader(VertexShader.Get(), nullptr, 0);
        dc->PSSetShader(PixelShader.Get(), nullptr, 0);

        
        
        Constants data;

        if (keyframe && keyframe->NodeVec.size() > 0)
        {
            const Animation::KeyFrame::Node& meshnode{ keyframe->NodeVec.at(mesh.NodeIndex) };
            XMStoreFloat4x4(&data.World, XMLoadFloat4x4(&meshnode.GlobalTransform) * XMLoadFloat4x4(&World));

            const size_t bone_count{ mesh.BindPose.BoneVec.size() };
            _ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");



            for (int bone_index = 0; bone_index < bone_count; ++bone_index)
            {
                const Skelton::Bone& bone{ mesh.BindPose.BoneVec.at(bone_index) };
                const Animation::KeyFrame::Node& bone_node{ keyframe->NodeVec.at(bone.NodeIndex) };
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

            for (size_t boneindex = 0; boneindex < MAX_BONES; ++boneindex)
            {
                data.BoneTransform[boneindex] = {
                    1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1
                };
            }
        }
        for (const Mesh::Subset& subset : mesh.SubsetVec)
        {
            const Material& material{ MaterialVec.at(subset.MaterialUniqueId) };

           // XMStoreFloat4x4(&data.World, XMLoadFloat4x4(&mesh.DefaultGlobalTransform) * XMLoadFloat4x4(&World));

            DirectX::XMStoreFloat4(&data.MaterialColor, DirectX::XMLoadFloat4(&MaterialColor) * DirectX::XMLoadFloat4(&material.Kd));
            dc->UpdateSubresource(ConstantBuffer.Get(), 0, 0, &data, 0, 0);
            dc->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());

            dc->PSSetShaderResources(0, 1, material.ShaderResourceView[0].GetAddressOf());
            dc->PSSetShaderResources(1, 1, material.ShaderResourceView[1].GetAddressOf());

            dc->DrawIndexed(subset.IndexCount, subset.StartIndexLocation, 0);
        }
    }

}

void SkinnedMesh::f_SaveSkinnedMesh(std::string saveDir)
{
    f_Serealize(saveDir);
}

void SkinnedMesh::f_Serealize(std::string saveDir)
{
    std::filesystem::path cereal_filename(saveDir);
    cereal_filename.replace_extension("nk");

    std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
    cereal::BinaryOutputArchive serialization(ofs);
    serialization(SceneView, MeshVec, MaterialVec, AnimationVec, EventFlagMap,SoundFlagVec);
}

void SkinnedMesh::f_DeSerealize()
{
    std::filesystem::path cereal_filename(FilePath);
    cereal_filename.replace_extension("nk");

    std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
    cereal::BinaryInputArchive deserialization(ifs);
    deserialization(SceneView, MeshVec, MaterialVec, AnimationVec, EventFlagMap,SoundFlagVec);
}





void SkinnedMesh::f_FetchMaterialVec(FbxScene* fbx_scene, std::unordered_map<uint64_t, Material>& materials)
{
    const size_t nodecount{ SceneView.NodeVec.size() };

    for (size_t nodeindex = 0; nodeindex < nodecount; ++nodeindex)
    {
        const SceneOfFbx::Node& node{ SceneView.NodeVec.at(nodeindex) };
        const FbxNode* fbxnode{ fbx_scene->FindNodeByName(node.Name.c_str()) };

        const int matrialcount{ fbxnode->GetMaterialCount() };

        for (int materialindex = 0; materialindex < matrialcount; ++materialindex)
        {

            const FbxSurfaceMaterial* fbxmaterial{ fbxnode->GetMaterial(materialindex) };

            Material material;
            material.Name = fbxmaterial->GetName();
            material.UniqueId = fbxmaterial->GetUniqueID();

            FbxProperty fbxproperty;
            fbxproperty = fbxmaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

            if (fbxproperty.IsValid())
            {
                const FbxDouble3 color{ fbxproperty.Get<FbxDouble3>() };
                material.Kd.x = static_cast<float>(color[0]);
                material.Kd.y = static_cast<float>(color[1]);
                material.Kd.z = static_cast<float>(color[2]);
                material.Kd.w = 1.0f;

                const FbxFileTexture* fbxtexture{ fbxproperty.GetSrcObject<FbxFileTexture>() };
                material.TextureFileName[0] =
                    fbxtexture ? fbxtexture->GetRelativeFileName() : "";
            }


            fbxproperty = fbxmaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
            if (fbxproperty.IsValid())
            {
                const FbxDouble3 color{ fbxproperty.Get<FbxDouble3>() };
                material.Ka.x = static_cast<float>(color[0]);
                material.Ka.y = static_cast<float>(color[1]);
                material.Ka.z = static_cast<float>(color[2]);
                material.Ka.w = 1.0f;


            }

            fbxproperty = fbxmaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
            if (fbxproperty.IsValid())
            {
                const FbxDouble3 color{ fbxproperty.Get<FbxDouble3>() };
                material.Ks.x = static_cast<float>(color[0]);
                material.Ks.y = static_cast<float>(color[1]);
                material.Ks.z = static_cast<float>(color[2]);
                material.Ks.w = 1.0f;
            }

            fbxproperty = fbxmaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
            if (fbxproperty.IsValid())
            {
                const FbxFileTexture* file_texture{ fbxproperty.GetSrcObject<FbxFileTexture>() };
                material.TextureFileName[1] = file_texture ? file_texture->GetRelativeFileName() : "";
            }


            materials.emplace(material.UniqueId, std::move(material));
        }
    }


    MaterialVec.emplace();
}




//****************************************************************
// 
// ダミーマテリアルの作成
// 
//****************************************************************
void SkinnedMesh::f_FetchDummyMaterialVec(FbxScene* fbx_scene, std::unordered_map<uint64_t, Material>& materials)
{

    const size_t nodecount{ SceneView.NodeVec.size() };
    for (size_t nodeindex = 0; nodeindex < nodecount; ++nodeindex)
    {
        const SceneOfFbx::Node& node{ SceneView.NodeVec.at(nodeindex) };
        const FbxNode* fbxnode{ fbx_scene->FindNodeByName(node.Name.c_str()) };

        const int matrialcount{ fbxnode->GetMaterialCount() };

        const FbxSurfaceMaterial* fbxmaterial{ fbxnode->GetMaterial(0) };

        Material material;
        materials.emplace(material.UniqueId, std::move(material));
    }

}

void SkinnedMesh::f_FetchSkelton(FbxMesh* fbx_mesh, Skelton& bind_pose)
{
    const int deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
    for (int deformer_index = 0; deformer_index < deformer_count; ++deformer_index)
    {
        FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(deformer_index, FbxDeformer::eSkin));
        const int cluster_count = skin->GetClusterCount();
        bind_pose.BoneVec.resize(cluster_count);
        for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
        {
            FbxCluster* cluster = skin->GetCluster(cluster_index);

            Skelton::Bone& bone{ bind_pose.BoneVec.at(cluster_index) };
            bone.Name = cluster->GetLink()->GetName();
            bone.UniqueId = cluster->GetLink()->GetUniqueID();
            bone.ParentIndex = bind_pose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
            bone.NodeIndex = SceneView.IndexOf(bone.UniqueId);

            //'reference_global_init_position' is used to convert from local space of model(mesh) to
            // global space of scene.
            FbxAMatrix reference_global_init_position;
            cluster->GetTransformMatrix(reference_global_init_position);

            // 'cluster_global_init_position' is used to convert from local space of bone to
            // global space of scene.
            FbxAMatrix cluster_global_init_position;
            cluster->GetTransformLinkMatrix(cluster_global_init_position);

            // Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation
            // (translation, rotation and scale), the last row of the matrix represents the translation part of
            // the transformation.
            // Compose 'bone.offset_transform' matrix that trnasforms position from mesh space to bone space.
            // This matrix is called the offset matrix.
            bone.offset_transform
                = f_ToXMFLOAT4x4(cluster_global_init_position.Inverse() * reference_global_init_position);
        }
    }

}

//****************************************************************
// 
// アニメーションデータの取得
// 
//****************************************************************
void SkinnedMesh::f_FetchAnimation(FbxScene* fbx_scene, 
    std::vector<Animation>& animation_vec, float sampling_rate)
{
    FbxArray<FbxString*> animation_stack_names;
    fbx_scene->FillAnimStackNameArray(animation_stack_names);
    const int animation_stack_count{ animation_stack_names.GetCount() };

    for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
    {
        Animation& animation_clip{ animation_vec.emplace_back() };
        animation_clip.Name = animation_stack_names[animation_stack_index]->Buffer();

        FbxAnimStack* animation_stack{ fbx_scene->FindMember<FbxAnimStack>(animation_clip.Name.c_str()) };
        fbx_scene->SetCurrentAnimationStack(animation_stack);

        const FbxTime::EMode time_mode{ fbx_scene->GetGlobalSettings().GetTimeMode() };
        FbxTime one_second;
        one_second.SetTime(0, 0, 1, 0, 0, time_mode);
        animation_clip.SamplingRate = sampling_rate > 0 ?
            sampling_rate : static_cast<float>(one_second.GetFrameRate(time_mode));
        const FbxTime sampling_interval
        { static_cast<FbxLongLong>(one_second.Get() / animation_clip.SamplingRate) };
        const FbxTakeInfo* take_info{ fbx_scene->GetTakeInfo(animation_clip.Name.c_str()) };
        const FbxTime start_time{ take_info->mLocalTimeSpan.GetStart() };
        const FbxTime stop_time{ take_info->mLocalTimeSpan.GetStop() };

        for (FbxTime time = start_time; time < stop_time; time += sampling_interval)
        {
            Animation::KeyFrame& keyframe{ animation_clip.Sequence.emplace_back() };

            const size_t node_count{ SceneView.NodeVec.size() };
            keyframe.NodeVec.resize(node_count);

            for (size_t node_index = 0; node_index < node_count; ++node_index)
            {
                FbxNode* fbx_node{ fbx_scene->FindNodeByName(SceneView.NodeVec.at(node_index).Name.c_str()) };
                if (fbx_node)
                {
                    Animation::KeyFrame::Node& node{ keyframe.NodeVec.at(node_index) };
                    // 'global_transform' is a transformation matrix of a node with respect to
                    // the scene's global coordinate system.
                    node.GlobalTransform = f_ToXMFLOAT4x4(fbx_node->EvaluateGlobalTransform(time));
                    const FbxAMatrix& localtransform{ fbx_node->EvaluateLocalTransform(time) };
                    node.Scaling = f_ToXMFLOAT3(localtransform.GetS());
                    node.Rotation = f_ToXMFLOAT4(localtransform.GetQ());
                    node.Translation = f_ToXMFLOAT3(localtransform.GetT());
                }
            }
        }
    }
    for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
    {
        delete animation_stack_names[animation_stack_index];
    }
}

void SkinnedMesh::f_FetchDummyAnimation(FbxScene* fbx_scene, std::vector<Animation>& animation_vec, float sampling_rate)
{
    FbxArray<FbxString*> animation_stack_names;
    fbx_scene->FillAnimStackNameArray(animation_stack_names);
    const int animation_stack_count{ animation_stack_names.GetCount() };

 
        Animation& animation_clip{ animation_vec.emplace_back() };
        animation_clip.Name = "None";

        FbxAnimStack* animation_stack{ fbx_scene->FindMember<FbxAnimStack>(animation_clip.Name.c_str()) };
        fbx_scene->SetCurrentAnimationStack(animation_stack);

        const FbxTime::EMode time_mode{ fbx_scene->GetGlobalSettings().GetTimeMode() };
        FbxTime one_second;
        one_second.SetTime(0, 0, 1, 0, 0, time_mode);
        animation_clip.SamplingRate = sampling_rate > 0 ?
            sampling_rate : static_cast<float>(one_second.GetFrameRate(time_mode));
        const FbxTime sampling_interval
        { static_cast<FbxLongLong>(one_second.Get() / animation_clip.SamplingRate) };
        const FbxTakeInfo* take_info{ fbx_scene->GetTakeInfo(animation_clip.Name.c_str()) };
        const FbxTime start_time{ 0 };
        const FbxTime stop_time{ 1 };

        for (FbxTime time = start_time; time < stop_time; time += sampling_interval)
        {
            Animation::KeyFrame& keyframe{ animation_clip.Sequence.emplace_back() };

            const size_t node_count{ SceneView.NodeVec.size() };
            keyframe.NodeVec.resize(node_count);

            for (size_t node_index = 0; node_index < node_count; ++node_index)
            {
                FbxNode* fbx_node{ fbx_scene->FindNodeByName(SceneView.NodeVec.at(node_index).Name.c_str()) };
                if (fbx_node)
                {
                    Animation::KeyFrame::Node& node{ keyframe.NodeVec.at(node_index) };
                    // 'global_transform' is a transformation matrix of a node with respect to
                    // the scene's global coordinate system.
                    node.GlobalTransform = f_ToXMFLOAT4x4(fbx_node->EvaluateGlobalTransform(time));
                    const FbxAMatrix& localtransform{ fbx_node->EvaluateLocalTransform(time) };
                    node.Scaling = f_ToXMFLOAT3(localtransform.GetS());
                    node.Rotation = f_ToXMFLOAT4(localtransform.GetQ());
                    node.Translation = f_ToXMFLOAT3(localtransform.GetT());
                }
            }
        }
    
    for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
    {
        delete animation_stack_names[animation_stack_index];
    }

}

//****************************************************************
// 
// ダミーのイベントフラグの追加
// 
//****************************************************************
void SkinnedMesh::f_FetchDummyEventFlag()
{
    EventFlag ev;
    ev.Name = "Dummy";
    ev.Index = 0;
    ev.AnimName = -1;
    ev.BeginFrame = 0;
    ev.LengthOfFrame = 0;
    EventFlagMap[0] = ev;
}

void SkinnedMesh::f_FetchDummySoundFlag()
{
    SoundFlag so;
    so.Name = "Dummy";
    so.AnimeName = -1;
    so.BeginFrame = 0.0f;
    so.key = 0;
    so.SoundPath = "";
    SoundFlagVec.emplace_back(so);
}




