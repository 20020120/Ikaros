#include"GeometricPrimitive.h"
#include"misc.h"


#include"TextureLoder.h"
GeometricPrimitive::GeometricPrimitive(ID3D11Device* Device)
{

    Vertex vertices[24]{};
      // サイズが  1.0の正立方体データを作成する（重心を原点にする）。正立方体のコントロールポイント数は  8個、
       // 1つのコントロールポイントの位置には法線の向きが違う頂点が  3個あるので頂点情報の総数は 8x3=24個、
       // 頂点情報配列（vertices）にすべて頂点の位置・法線情報を格納する。
    
    //Vertex{float3 position,float3  Nomal}
    
    //--------------------<上面>--------------------//

    vertices[0] = { DirectX::XMFLOAT3{-0.5,0.5,-0.5 },DirectX::XMFLOAT3{0,1,0} };
    vertices[1] = { DirectX::XMFLOAT3{ 0.5,0.5, 0.5 },DirectX::XMFLOAT3{0,1,0} };
    vertices[2] = { DirectX::XMFLOAT3{-0.5,0.5, 0.5 },DirectX::XMFLOAT3{0,1,0} };
    vertices[3] = { DirectX::XMFLOAT3{ 0.5,0.5,-0.5 },DirectX::XMFLOAT3{0,1,0} };
 

    //--------------------<前面>--------------------//

    vertices[4] = { DirectX::XMFLOAT3{-0.5, 0.5, 0.5 },DirectX::XMFLOAT3{0,0,-1} };
    vertices[5] = { DirectX::XMFLOAT3{ 0.5,-0.5, 0.5 },DirectX::XMFLOAT3{0,0,-1} };
    vertices[6] = { DirectX::XMFLOAT3{-0.5,-0.5, 0.5 },DirectX::XMFLOAT3{0,0,-1} };

    //vertices[4] 
    vertices[7] = { DirectX::XMFLOAT3{ 0.5, 0.5, 0.5 },DirectX::XMFLOAT3{0,0,-1} };
    //vertices[5] 

    //--------------------<右面>--------------------//

    vertices[8] =  { DirectX::XMFLOAT3{ 0.5, 0.5, 0.5 },DirectX::XMFLOAT3{1,0,0} };
    vertices[9] =  { DirectX::XMFLOAT3{ 0.5,-0.5,-0.5 },DirectX::XMFLOAT3{1,0,0} };
    vertices[10] = { DirectX::XMFLOAT3{ 0.5,-0.5, 0.5 },DirectX::XMFLOAT3{1,0,0} };

    //vertices[8] 
    vertices[11] = { DirectX::XMFLOAT3{ 0.5, 0.5, -0.5 }, DirectX::XMFLOAT3{1,0,0} };
    //vertices[9]  


    //--------------------<左面>--------------------//

    vertices[12] = { DirectX::XMFLOAT3{-0.5, 0.5,-0.5 },DirectX::XMFLOAT3{-1,0,0} };
    vertices[13] = { DirectX::XMFLOAT3{-0.5,-0.5, 0.5 },DirectX::XMFLOAT3{-1,0,0} };
    vertices[14] = { DirectX::XMFLOAT3{-0.5,-0.5,-0.5 },DirectX::XMFLOAT3{-1,0,0} };
    vertices[15] = { DirectX::XMFLOAT3{-0.5, 0.5, 0.5 },DirectX::XMFLOAT3{-1,0,0} };

    //vertices[12] 
    //vertices[13]  

    //--------------------<後面>--------------------//

    vertices[16] = { DirectX::XMFLOAT3{ 0.5, 0.5,-0.5 },DirectX::XMFLOAT3{0,0,1} };
    vertices[17] = { DirectX::XMFLOAT3{-0.5,-0.5,-0.5 },DirectX::XMFLOAT3{0,0,1} };
    vertices[18] = { DirectX::XMFLOAT3{ 0.5,-0.5,-0.5 },DirectX::XMFLOAT3{0,0,1} };

    //vertices[16] 
    vertices[19] = { DirectX::XMFLOAT3{-0.5, 0.5,-0.5 }, DirectX::XMFLOAT3{0,0,1} };
    //vertices[18]  


    //--------------------<下面>--------------------//

    vertices[20] = { DirectX::XMFLOAT3{-0.5,-0.5, 0.5 },DirectX::XMFLOAT3{0,-1,0} };
    vertices[21] = { DirectX::XMFLOAT3{ 0.5,-0.5,-0.5 },DirectX::XMFLOAT3{0,-1,0} };
    vertices[22] = { DirectX::XMFLOAT3{-0.5,-0.5,-0.5 },DirectX::XMFLOAT3{0,-1,0} };

    //vertices[16] 
    vertices[23] = { DirectX::XMFLOAT3{ 0.5,-0.5, 0.5 }, DirectX::XMFLOAT3{0,-1,0} };
    //vertices[18]  


    uint32_t indices[36]{
        0,1,2,1,0,3,       //上

        4,5,6,7,5,4,        //手前

        8,9,10,9,8,11,      //右
        12,13,14,12,15,13,  //左
        16,17,18,16,19,17,  //奥
        20,21,22,20,23,21   //下

    };
      // 正立方体は 6面持ち、1つの面は 2つの  3角形ポリゴンで構成されるので 3角形ポリゴンの総数は  6x2=12個、
        // 正立方体を描画するために  12回の  3角形ポリゴン描画が必要、よって参照される頂点情報は  12x3=36回、
        // 3角形ポリゴンが参照する頂点情報のインデックス（頂点番号）を描画順に配列（indices）に格納する。
        // 時計回りが表面になるように格納すること。
    


     CreateComBuffer(Device, vertices, 24, indices, 36);
    
     HRESULT hr{ S_OK };
    
     D3D11_INPUT_ELEMENT_DESC input_element_desc[]
     {
         { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
           D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
         { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
           D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
     };
       CreateVsFromCso(Device, "GeometricPrimitive_vs.cso", VertexShader.GetAddressOf(),
         InputLayout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
       CreatePsFromCso(Device, "GeometricPrimitive_PS.cso", PixelShader.GetAddressOf());
    
     D3D11_BUFFER_DESC buffer_desc{};
       buffer_desc.ByteWidth = sizeof(Constants);
       buffer_desc.Usage = D3D11_USAGE_DEFAULT;
       buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
       hr = Device->CreateBuffer(&buffer_desc, nullptr, ConstantBuffer.GetAddressOf());
       _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void GeometricPrimitive::Render(ID3D11DeviceContext* ImmidiateContext, const DirectX::XMFLOAT4X4& World, const DirectX::XMFLOAT4& MaterialColor)
{

     uint32_t stride{ sizeof(Vertex) };
     uint32_t offset{ 0 };
     ImmidiateContext->IASetVertexBuffers    (0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);
     ImmidiateContext->IASetIndexBuffer      (IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
     ImmidiateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
     ImmidiateContext->IASetInputLayout      (InputLayout.Get());
     ImmidiateContext->VSSetShader           (VertexShader.Get(), nullptr, 0);
     ImmidiateContext->PSSetShader           (PixelShader.Get(), nullptr, 0);
    
      Constants data{ World, MaterialColor };
      ImmidiateContext->UpdateSubresource(ConstantBuffer.Get(), 0, 0, &data, 0, 0);
      ImmidiateContext->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
    
     D3D11_BUFFER_DESC buffer_desc{};
     IndexBuffer->GetDesc(&buffer_desc);
     ImmidiateContext->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
}



void GeometricPrimitive::CreateComBuffer(ID3D11Device* Device, Vertex* Vertices, size_t VertexCount, uint32_t* Indices, size_t IndexCount)
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
