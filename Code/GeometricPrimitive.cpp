#include"GeometricPrimitive.h"
#include"misc.h"


#include"TextureLoder.h"
GeometricPrimitive::GeometricPrimitive(ID3D11Device* Device)
{

    Vertex vertices[24]{};
      // �T�C�Y��  1.0�̐������̃f�[�^���쐬����i�d�S�����_�ɂ���j�B�������̂̃R���g���[���|�C���g����  8�A
       // 1�̃R���g���[���|�C���g�̈ʒu�ɂ͖@���̌������Ⴄ���_��  3����̂Œ��_���̑����� 8x3=24�A
       // ���_���z��ivertices�j�ɂ��ׂĒ��_�̈ʒu�E�@�������i�[����B
    
    //Vertex{float3 position,float3  Nomal}
    
    //--------------------<���>--------------------//

    vertices[0] = { DirectX::XMFLOAT3{-0.5,0.5,-0.5 },DirectX::XMFLOAT3{0,1,0} };
    vertices[1] = { DirectX::XMFLOAT3{ 0.5,0.5, 0.5 },DirectX::XMFLOAT3{0,1,0} };
    vertices[2] = { DirectX::XMFLOAT3{-0.5,0.5, 0.5 },DirectX::XMFLOAT3{0,1,0} };
    vertices[3] = { DirectX::XMFLOAT3{ 0.5,0.5,-0.5 },DirectX::XMFLOAT3{0,1,0} };
 

    //--------------------<�O��>--------------------//

    vertices[4] = { DirectX::XMFLOAT3{-0.5, 0.5, 0.5 },DirectX::XMFLOAT3{0,0,-1} };
    vertices[5] = { DirectX::XMFLOAT3{ 0.5,-0.5, 0.5 },DirectX::XMFLOAT3{0,0,-1} };
    vertices[6] = { DirectX::XMFLOAT3{-0.5,-0.5, 0.5 },DirectX::XMFLOAT3{0,0,-1} };

    //vertices[4] 
    vertices[7] = { DirectX::XMFLOAT3{ 0.5, 0.5, 0.5 },DirectX::XMFLOAT3{0,0,-1} };
    //vertices[5] 

    //--------------------<�E��>--------------------//

    vertices[8] =  { DirectX::XMFLOAT3{ 0.5, 0.5, 0.5 },DirectX::XMFLOAT3{1,0,0} };
    vertices[9] =  { DirectX::XMFLOAT3{ 0.5,-0.5,-0.5 },DirectX::XMFLOAT3{1,0,0} };
    vertices[10] = { DirectX::XMFLOAT3{ 0.5,-0.5, 0.5 },DirectX::XMFLOAT3{1,0,0} };

    //vertices[8] 
    vertices[11] = { DirectX::XMFLOAT3{ 0.5, 0.5, -0.5 }, DirectX::XMFLOAT3{1,0,0} };
    //vertices[9]  


    //--------------------<����>--------------------//

    vertices[12] = { DirectX::XMFLOAT3{-0.5, 0.5,-0.5 },DirectX::XMFLOAT3{-1,0,0} };
    vertices[13] = { DirectX::XMFLOAT3{-0.5,-0.5, 0.5 },DirectX::XMFLOAT3{-1,0,0} };
    vertices[14] = { DirectX::XMFLOAT3{-0.5,-0.5,-0.5 },DirectX::XMFLOAT3{-1,0,0} };
    vertices[15] = { DirectX::XMFLOAT3{-0.5, 0.5, 0.5 },DirectX::XMFLOAT3{-1,0,0} };

    //vertices[12] 
    //vertices[13]  

    //--------------------<���>--------------------//

    vertices[16] = { DirectX::XMFLOAT3{ 0.5, 0.5,-0.5 },DirectX::XMFLOAT3{0,0,1} };
    vertices[17] = { DirectX::XMFLOAT3{-0.5,-0.5,-0.5 },DirectX::XMFLOAT3{0,0,1} };
    vertices[18] = { DirectX::XMFLOAT3{ 0.5,-0.5,-0.5 },DirectX::XMFLOAT3{0,0,1} };

    //vertices[16] 
    vertices[19] = { DirectX::XMFLOAT3{-0.5, 0.5,-0.5 }, DirectX::XMFLOAT3{0,0,1} };
    //vertices[18]  


    //--------------------<����>--------------------//

    vertices[20] = { DirectX::XMFLOAT3{-0.5,-0.5, 0.5 },DirectX::XMFLOAT3{0,-1,0} };
    vertices[21] = { DirectX::XMFLOAT3{ 0.5,-0.5,-0.5 },DirectX::XMFLOAT3{0,-1,0} };
    vertices[22] = { DirectX::XMFLOAT3{-0.5,-0.5,-0.5 },DirectX::XMFLOAT3{0,-1,0} };

    //vertices[16] 
    vertices[23] = { DirectX::XMFLOAT3{ 0.5,-0.5, 0.5 }, DirectX::XMFLOAT3{0,-1,0} };
    //vertices[18]  


    uint32_t indices[36]{
        0,1,2,1,0,3,       //��

        4,5,6,7,5,4,        //��O

        8,9,10,9,8,11,      //�E
        12,13,14,12,15,13,  //��
        16,17,18,16,19,17,  //��
        20,21,22,20,23,21   //��

    };
      // �������̂� 6�ʎ����A1�̖ʂ� 2��  3�p�`�|���S���ō\�������̂� 3�p�`�|���S���̑�����  6x2=12�A
        // �������̂�`�悷�邽�߂�  12���  3�p�`�|���S���`�悪�K�v�A����ĎQ�Ƃ���钸�_����  12x3=36��A
        // 3�p�`�|���S�����Q�Ƃ��钸�_���̃C���f�b�N�X�i���_�ԍ��j��`�揇�ɔz��iindices�j�Ɋi�[����B
        // ���v��肪�\�ʂɂȂ�悤�Ɋi�[���邱�ƁB
    


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
