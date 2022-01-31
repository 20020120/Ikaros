#include"TextureLoder.h"
#include<filesystem>
#include<vector>
#include <fstream>
#include"misc.h"
#include "StaticMesh.h"

StaticMesh::StaticMesh(ID3D11Device* Device, const wchar_t* ObjFileName,bool IsFlip)
{
    std::vector<Vertex> vertexVec;
    std::vector<uint32_t> indicesVec;

    uint32_t current_index{ 0 };

    std::vector<DirectX::XMFLOAT3> positionsVec;
    std::vector<DirectX::XMFLOAT3> nomalVec;
    std::vector<DirectX::XMFLOAT2> TexcoordVec;
    std::vector<std::wstring> mtl_filenameVec;

    float Top{};
    float Left{};
    float Bottom{};
    float Right{};
    float Front{};
    float Depth{};

    std::wifstream fin(ObjFileName);
    _ASSERT_EXPR(fin, L"'OBJ file not found.");
    wchar_t command[256];

    while (fin)
    {
        fin >> command;
        
        if (0 == wcscmp(command, L"v"))
        {
            float x, y, z;
            fin >> x >> y >> z;
            positionsVec.emplace_back(DirectX::XMFLOAT3 { x,y,z });
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vn"))
        {
            float i, j, k;
            fin >> i >> j >> k;
            nomalVec.emplace_back(DirectX::XMFLOAT3{ i,j,k });
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vt"))
        {
            float u, v;
            fin >> u >> v;
            TexcoordVec.push_back({ u,IsFlip? 1.0f-v:v });
            fin.ignore(1024, L'\n');
        }
        else if (0==wcscmp(command, L"f"))
        {
            for (size_t i = 0; i < 3; i++)
            {
                Vertex vertex{};
                size_t v, vt, vn;

                fin >> v;
                vertex.Position = positionsVec.at(v - 1);

                //****************************************************************
                // 
                // UNIT16
                // 
                //****************************************************************
                //バウンディングボックスの大きさを定義
                
                //上
                if (Top <= vertex.Position.y)
                {
                    Top = vertex.Position.y;
                }

                //下
                if (Bottom >= vertex.Position.y)
                {
                    Bottom = vertex.Position.y;
                }

                //右
                if (Right <= vertex.Position.x)
                {
                    Right = vertex.Position.x;
                }

                //左
                if (Left >= vertex.Position.x)
                {
                    Left = vertex.Position.x;
                }
                
                //手前
                if (Front >= vertex.Position.z)
                {
                    Front = vertex.Position.z;
                }

                //奥
                if (Depth <= vertex.Position.z)
                {
                    Depth = vertex.Position.z;
                }

                if (L'/' == fin.peek())
                {
                    fin.ignore(1);
                    if (L'/' != fin.peek())
                    {
                        fin >> vt;
                        
                        vertex.Texcoord = TexcoordVec.at(vt - 1);
                        
                    }
                    if (L'/' == fin.peek())
                    {
                        fin.ignore(1);
                        fin >> vn;
                        vertex.Nomal = nomalVec.at(vn - 1);
                    }

                }
                vertexVec.push_back(vertex);
                indicesVec.push_back(current_index++);
            }
            fin.ignore(1024, L'\n');
        }
        else if (0==wcscmp(command,L"mtllib"))
        {
            wchar_t mtllib[256];
            fin >> mtllib;
            mtl_filenameVec.push_back(mtllib);
        }
        else if (0 == wcscmp(command, L"usemtl"))
        {
            wchar_t usemtl[MAX_PATH]{ 0 };
            fin >> usemtl;

            

            Subset a{ usemtl,static_cast<uint32_t>(indicesVec.size()),0 };
            SubsetVec.push_back(a);
        }
        else
        {
            fin.ignore(1024, L'\n');                        
        }
       
    }
    fin.close();

    //****************************************************************
    // 
    // UNIT15
    // 
    //****************************************************************
    std::vector<Subset>::reverse_iterator it = SubsetVec.rbegin();
    it->IndexCount = static_cast<uint32_t>(indicesVec.size()) - it->IndexStart;
    for (it = SubsetVec.rbegin() + 1; it != SubsetVec.rend(); ++it)
    {
        it->IndexCount = (it-1)->IndexStart - it->IndexStart;
    }

    
    std::filesystem::path mtl_filename(ObjFileName);
    mtl_filename.replace_filename(std::filesystem::path(mtl_filenameVec[0]).filename());

    fin.open(mtl_filename);
    assert(fin);
    while (fin)
    {
        fin >> command;
        if (0 == wcscmp(command, L"map_Kd"))
        {
            fin.ignore();
            wchar_t map_Kd[256];
            fin >> map_Kd;

            std::filesystem::path path(ObjFileName);
            path.replace_filename(std::filesystem::path(map_Kd).filename());
            //TextureFileName = path;
            MaterialVec.rbegin()->TextureFileName[0] = path;
            fin.ignore(1024, L'\n');
        }
        else if (0==wcscmp(command,L"map_bump")||0 == wcscmp(command, L"bump"))
        {
            fin.ignore();
            wchar_t map_bump[256];
            fin >> map_bump;

            std::filesystem::path path(ObjFileName);
            path.replace_filename(std::filesystem::path(map_bump).filename());


            MaterialVec.rbegin()->TextureFileName[1] = path;
            fin.ignore(1024, L'\n');
        }
        else if (0==wcscmp(command,L"newmtl"))
        {
            fin.ignore();
            wchar_t newmtl[256];
            Material material;
            fin >> newmtl;
            material.Name = newmtl;
            MaterialVec.push_back(material);
        }
        else if (0 == wcscmp(command, L"Kd"))
        {
            float r, g, b ;
            fin >> r >> g >> b ;
            MaterialVec.rbegin()->Kd = { r,g,b,1 };
            fin.ignore(1024, L'\n');
        }

        else if (0 == wcscmp(command, L"Ka"))
        {
            float r, g, b;
            fin >> r >> g >> b;
            MaterialVec.rbegin()->Ka = { r,g,b,1 };
            fin.ignore(1024, L'\n');
        }

        else if (0 == wcscmp(command, L"Ks"))
        {
            float r, g, b;
            fin >> r >> g >> b;
            MaterialVec.rbegin()->Ks = { r,g,b,1 };
            fin.ignore(1024, L'\n');
        }
        else
        {
            fin.ignore(1024, L'\n');
        }

    }

 /*   fin.open(mtl_filename);
    while (fin)
    {
        fin >> command;
       
        else if (0 == wcscmp(command, L"newmtl"))
        {
            fin.ignore();
            wchar_t newmtl[256];
            Material material;
            fin >> newmtl;
            material.Name = newmtl;
            MaterialVec.push_back(material);
        }
       
        else
        {
            fin.ignore(1024, L'\n');
        }

    }*/


        CreateComBuffers(Device, vertexVec.data(), vertexVec.size(), indicesVec.data(), indicesVec.size());


        //--------------------<テクスチャのロード>--------------------//
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        //LoadTextureFromFile(Device, TextureFileName.c_str(), ShaderResourceView.GetAddressOf(), &texture2d_desc);


        if (MaterialVec.size() == 0)
        {
            for (const auto& it : SubsetVec)
            {
                MaterialVec.push_back({it.UseMtl});
            }
        
        }
        

        
            for (Material& it : MaterialVec)
            {
                if (it.TextureFileName[0].size()>0)
                {
                    LoadTextureFromFile(
                        Device,
                        it.TextureFileName[0].c_str(),
                        it.ShaderResourceView[0].GetAddressOf(),
                        &texture2d_desc);
                }
                else
                {
                    f_MakeDummyTexture(Device, it.ShaderResourceView[0].GetAddressOf(), 0xFFFFFFFF, 16);
                }
                
                if (it.TextureFileName[1].size()> 0)
                {
                    LoadTextureFromFile(
                        Device,
                        it.TextureFileName[1].c_str(),
                        it.ShaderResourceView[1].GetAddressOf(),
                        &texture2d_desc);
                }
                else
                {
                    f_MakeDummyTexture(Device, it.ShaderResourceView[1].GetAddressOf(), 0xFFFFFFFF, 16);
                }
                
            }
        


        HRESULT hr{ S_OK };


        D3D11_INPUT_ELEMENT_DESC input_element_desc[]
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
              D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
              D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
             {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
            D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},

        };


        CreateVsFromCso(Device,
            "StaticMesh_VS.cso",
            VertexShader.GetAddressOf(),
            InputLayout.GetAddressOf(),
            input_element_desc,
            ARRAYSIZE(input_element_desc));

        CreatePsFromCso(Device,
            "StaticMesh_PS.cso",
            PixelShader.GetAddressOf());

        D3D11_BUFFER_DESC buffer_desc{};
        buffer_desc.ByteWidth = sizeof(Constants);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = Device->CreateBuffer(&buffer_desc, nullptr, ConstantBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    
        
    //****************************************************************
    // 
    // UNIT16
    // 
    //****************************************************************
        //boundingbox =std::make_unique<BoundingBox>(Device,Top,Left,Bottom,Right,Front,Depth );

}
    

void StaticMesh::Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& World, const DirectX::XMFLOAT4& MaterialColor, ID3D11RasterizerState** RasterizerState)
{

    uint32_t stride{ sizeof(Vertex) };
    uint32_t offset{ 0 };
    dc->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);
    dc->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->IASetInputLayout(InputLayout.Get());

    dc->VSSetShader(VertexShader.Get(), nullptr, 0);
    dc->PSSetShader(PixelShader.Get(), nullptr, 0);

    for (const auto& it : MaterialVec)
    {
        dc->PSSetShaderResources(0, 1, it.ShaderResourceView[0].GetAddressOf());
        dc->PSSetShaderResources(1, 1, it.ShaderResourceView[0].GetAddressOf());
        
        Constants data{ World, MaterialColor };
        DirectX::XMStoreFloat4(&data.MaterialColor, 
            DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&MaterialColor) ,DirectX::XMLoadFloat4(&it.Kd)));


        dc->UpdateSubresource(ConstantBuffer.Get(), 0, 0, &data, 0, 0);
        dc->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
        
        for (const auto& subset : SubsetVec)
        {
            if (it.Name == subset.UseMtl)
            {
                dc->DrawIndexed(subset.IndexCount, subset.IndexStart, 0);
            }
        } 
    }

    dc->RSSetState(RasterizerState[1]);
    //boundingbox->Render(dc, World, MaterialColor);
    dc->RSSetState(RasterizerState[0]);
}


void StaticMesh::CreateComBuffers(ID3D11Device* Device, Vertex* Vertices, size_t VertexCount, uint32_t* Indices, size_t IndexCount)
{
    HRESULT hr{ S_OK };

    D3D11_BUFFER_DESC buffer_desc{};
    D3D11_SUBRESOURCE_DATA subresource_data{};
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * VertexCount);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    subresource_data.pSysMem = Vertices;
    subresource_data.SysMemPitch = 0;
    subresource_data.SysMemSlicePitch = 0;
    hr = Device->CreateBuffer(&buffer_desc, &subresource_data, VertexBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * IndexCount);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    subresource_data.pSysMem = Indices;
    hr = Device->CreateBuffer(&buffer_desc, &subresource_data, IndexBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
