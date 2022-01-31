#include"SpriteBatch.h"

#include <Effekseer.h>
#include<sstream>
#include"TextureLoder.h"

////MY_MEMO�FUNIT10
//static map<wstring, ComPtr<ID3D11ShaderResourceView>> Resources;
//
//HRESULT Sprite_Batch::LoadTextureFromFile(ID3D11Device* Device, const wchar_t* FileName, ID3D11ShaderResourceView** ShaderResourceView, D3D11_TEXTURE2D_DESC* Texture2dDesc)
//{
//    using namespace DirectX;
//    using namespace std;
//
//    HRESULT hr{ S_OK };
//    ComPtr<ID3D11Resource> res;
//
//    auto it = Resources.find(FileName);
//
//    if (it != Resources.end())
//    {
//        *ShaderResourceView = it->second.Get();
//        (*ShaderResourceView)->AddRef();
//        (*ShaderResourceView)->GetResource(res.GetAddressOf());
//    }
//    else
//    {
//        hr = CreateWICTextureFromFile(Device, FileName, res.GetAddressOf(), ShaderResourceView);
//        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//        Resources.insert(make_pair(FileName, *ShaderResourceView));
//    }
//
//    ComPtr<ID3D11Texture2D> Texture2d;
//
//    hr = res.Get()->QueryInterface<ID3D11Texture2D>(Texture2d.GetAddressOf());
//    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//    Texture2d->GetDesc(Texture2dDesc);
//
//    return hr;
//}
//
//HRESULT Sprite_Batch::CreateVsFromCso(ID3D11Device* Device, const char* CsoName, ID3D11VertexShader** VertexShader, ID3D11InputLayout** InputLayout, D3D11_INPUT_ELEMENT_DESC* InputElementDesc, UINT NumElements)
//{
//    FILE* fp{ nullptr };
//    fopen_s(&fp, CsoName, "rb");
//    _ASSERT_EXPR_A(fp, "CSO File not Found");
//
//    fseek(fp, 0, SEEK_END);
//    long Cso_sz{ ftell(fp) };
//    fseek(fp, 0, SEEK_SET);
//
//    unique_ptr<unsigned char[]> Cso_Data{ make_unique<unsigned char[]>(Cso_sz) };
//
//    fread(Cso_Data.get(), Cso_sz, 1, fp);
//
//    HRESULT hr{ S_OK };
//    hr = Device->CreateVertexShader(Cso_Data.get(), Cso_sz, nullptr, VertexShader);
//    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//    if (InputLayout)
//    {
//        hr = Device->CreateInputLayout(InputElementDesc, NumElements,
//            Cso_Data.get(), Cso_sz, InputLayout);
//        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//    }
//    return hr;
//}
//
//HRESULT Sprite_Batch::CreatePsFromCso(ID3D11Device* Device, const char* CsoName, ID3D11PixelShader** PixelShader)
//{
//    FILE* fp{ nullptr };
//    fopen_s(&fp, CsoName,"rb");
//    _ASSERT_EXPR_A(fp, "CSO File Not Found");
//
//    fseek(fp, 0, SEEK_END);
//    long Cso_sz{ftell(fp)};
//    fseek(fp, 0, SEEK_SET);
//
//    unique_ptr<unsigned char[]> CsoData{ make_unique<unsigned char[]>(Cso_sz) };
//    fread(CsoData.get(), Cso_sz, 1, fp);
//    fclose(fp);
//    HRESULT hr{ S_OK };
//    hr = Device->CreatePixelShader(CsoData.get(), Cso_sz, nullptr, PixelShader);
//    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace);
//
//    return hr;
//}
//
//
//void Sprite_Batch::ReleaseAllTextures()
//{
//    Resources.clear();
//}

Sprite_Batch::Sprite_Batch(ID3D11Device* Device, const wchar_t* Filename, size_t MaxSprite, const wchar_t* DissolveFilename, const wchar_t* DstFilename)
    :MaxVertices(MaxSprite*6)
    
{
    
    IsSetDissolveTexture = false;

    if (DissolveFilename&& DstFilename)
    {
        IsSetDissolveTexture = true;
    }


    HRESULT hr{ S_OK };
    
    std::unique_ptr<Vertex[]> Vertices{ std::make_unique<Vertex[]>(MaxVertices) };

    
        D3D11_BUFFER_DESC Bufferdesc{};
        Bufferdesc.ByteWidth = sizeof(Vertex) * MaxVertices;   //�p�ӂ���o�b�t�@�̑傫��
        Bufferdesc.Usage = D3D11_USAGE_DYNAMIC; //�������݌`���̐ݒ�
        Bufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        Bufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        Bufferdesc.MiscFlags = 0;
        Bufferdesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA SubresourceData{};
        SubresourceData.pSysMem = Vertices.get();
        SubresourceData.SysMemPitch = 0;
        SubresourceData.SysMemSlicePitch = 0;

        hr = Device->CreateBuffer(&Bufferdesc, &SubresourceData, &Vertex_Buffer);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    

    /* struct D3D11_INPUT_ELEMENT_DESC {
        LPCSTR                     SemanticName;   �V�F�[�_�[���̓V�O�l�`���̗v�f�Ɋ֘A�t����ꂽHLSL�Z�}���e�B�b�N�i�́H�j
        UINT                       SemanticIndex;�@�@ �v�f�̃C���f�b�N�X�@��F4x4 �s��ɂ�SemanticName������ 4 �̃R���|�[�l���g������
        DXGI_FORMAT                Format;�@�@   �v�f�̃f�[�^�^
        UINT                       InputSlot;�@�@�@�@�@���̓A�Z���u�������ʂ��鐮���l�@�͈͂͂O�`�P�T
        UINT                       AlignedByteOffset;�@���_�̐擪����̃I�t�Z�b�g
        D3D11_INPUT_CLASSIFICATION InputSlotClass;�@�@�P��̓��̓X���b�g�̓��̓f�[�^�N���X�����ʂ��܂�
        UINT                       InstanceDataStepRate;�@�@�i�́H�j
    }*/

#if 0

    //���_�o�b�t�@�I�u�W�F�N�g�̐���
    D3D11_BUFFER_DESC Buffer_Desc{};
    Buffer_Desc.ByteWidth = sizeof(Vertices);
    Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
    Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    Buffer_Desc.CPUAccessFlags = 0;
    Buffer_Desc.MiscFlags = 0;
    Buffer_Desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA Subresource_Data{};
    Subresource_Data.pSysMem = Vertices;
    Subresource_Data.SysMemPitch = 0;
    Subresource_Data.SysMemSlicePitch = 0;
    hr = Device->CreateBuffer(&Buffer_Desc, &Subresource_Data, &Vertex_Buffer);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#endif // 0

    //MY_MEMO�FUNIT�R
    //  �Q�lURL�F�uDirectX11�̒��_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�vhttp://gameworkslab.jp/2019/06/15/directx11%e3%81%ae%e9%a0%82%e7%82%b9%e3%83%90%e3%83%83%e3%83%95%e3%82%a1%e3%81%a8%e3%82%a4%e3%83%b3%e3%83%87%e3%83%83%e3%82%af%e3%82%b9%e3%83%90%e3%83%83%e3%83%95%e3%82%a1/2/
    // �o�b�t�@���\�[�X���쐬����
    //���_�o�b�t�@�I�u�W�F�N�g�̐���   
#if 0
    D3D11_BUFFER_DESC Buffer_Desc{};
    Buffer_Desc.ByteWidth = sizeof(v);
    Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;   //�o�b�t�@�̓ǂݎ��E�������ݕ��@��ύX����
    //Buffer_Desc.Usage�̏ڍ�URL   https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage

    Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;   //CPU�̃A�N�Z�X�t���O�i�K�v�Ȃ��ꍇ�͂O�j
    //Buffer_Desc.CPUAccessFlags�̏ڍ�URL�Fhttps://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_cpu_access_flag

    Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   //�o�b�t�@���p�C�v���C���Ƀo�C���h������@�����ʂ���
    //Buffer_Desc.BindFlags�̏ڍ�URL�Fhttps://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_bind_flag

    Buffer_Desc.MiscFlags = 0;   //���̑��̃t���O�i���g�p�̏ꍇ�͂O�j
    Buffer_Desc.StructureByteStride = 0;  //�R���s���[�g�V�F�[�_�Ŏg�p����StructureBuffer�iGPU�Ŏg�p����z��j���g�p����ۂɎw��(�́H)



    //MY_MEMO�FUNIT3
    D3D11_SUBRESOURCE_DATA Subresource_Data{};
    Subresource_Data.pSysMem = v;
    Subresource_Data.SysMemPitch = 0;
    Subresource_Data.SysMemSlicePitch = 0;
    hr = Device->CreateBuffer(&Buffer_Desc, &Subresource_Data, &Vertex_Buffer);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#endif // 0

#if 0


    //MY_MEMO�FUNIT5
    ID3D11Resource* Resource{};
    hr = DirectX::CreateWICTextureFromFile(Device, Filename, &Resource, &Shader_Resource_View);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    Resource->Release();

    //MY_MEMO�FUNIT5
    ID3D11Texture2D* Texture2d{};
    hr = Resource->QueryInterface<ID3D11Texture2D>(&Texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    Texture2d->GetDesc(&Texture2D_Desc);
    Texture2d->Release();
#endif // 0

    //MY_MEMO : UNIT10
    hr = LoadTextureFromFile(Device, Filename, &Shader_Resource_View, &Texture2D_Desc);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


    //���̓��C�A�E�g�I�u�W�F�N�g�̐���
    D3D11_INPUT_ELEMENT_DESC Input_Erement_Desc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    //���_�V�F�[�_�[�I�u�W�F�N�g�̐���
    const char* cso_name{ "./resources/Shaders/Sprite_vs.cso" };
    hr = CreateVsFromCso(Device, cso_name, Vertex_Shader.GetAddressOf(), Input_Layout.GetAddressOf(), Input_Erement_Desc, _countof(Input_Erement_Desc));
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    
    //�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
    const char* cso_ps_name{ "./resources/Shaders/Sprite_ps.cso" };
    hr = CreatePsFromCso(Device, cso_ps_name, Pixel_Shader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


    //--------------------<�f�B�]���u�p�̃e�N�X�`�������[�h>--------------------//
    if (IsSetDissolveTexture)
    {
        DissolveTexture.Register(Device, 1, DissolveFilename);
        DstTexture.Register(Device, 2, DstFilename);
    }

    cBuffer.Init(Device, &Disolvedata);
    cBuffer.Set_PSSlot(0);
    Disolvedata.Power = 0.0f;

#if 0
        //HACK �t�@�C���ɕs������������`�F�b�N
    FILE* fp{};
    fopen_s(&fp, cso_ps_name, "rb");
    _ASSERT_EXPR_A(fp, "CS File not found");

    fseek(fp, 0, SEEK_END);
    long cso_ps_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> cso_ps_data{ std::make_unique<unsigned char[]>(cso_ps_sz) };
    fread(cso_ps_data.get(), cso_ps_sz, 1, fp);
    fclose(fp);

    hr = Device->CreatePixelShader(cso_ps_data.get(), cso_ps_sz, nullptr, &Pixel_Shader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#endif // 0
    
}

//MY_MEMO�FUNIT4�uSPRITE-ROTATION�v
//�����_���F�_(x, y)���_(cx, cy)�𒆐S�Ɋp(angle)�ŉ�]�������̍��W���v�Z����
auto Rotate = [=](float& x, float& y, float cx, float cy, float Angle)
{
    //���W�i��,���j�����_�Ɉړ�
    x -= cx;
    y -= cy;

    //sin,cos
    float L_Sin = sin(DirectX::XMConvertToRadians(Angle));
    float L_Cos = cos(DirectX::XMConvertToRadians(Angle));

    float L_tx{ x };
    float L_ty{ y };

    x = (L_tx * L_Cos) + -(L_ty * L_Sin);
    y = (L_tx * L_Sin) + (L_ty * L_Cos);

    //���W�i��,���j�����̈ʒu�Ɉړ�
    x += cx;
    y += cy;
};

void Sprite_Batch::render(ID3D11DeviceContext* Immidiate_Context,
    float posX, float posY,   //��`�̍���̍��W�i�X�N���[�����W�n�j
    float scaleX, float scaleY, //�X�P�[��
    float TextureSizeX, float TextureSizeY, //�`�悷��摜�̑傫��
    float r, float g, float b, float a,  //�F�iR,G,B,A�j
    float angle/*degree*/)
{
    //�e�N�X�`���̎p�����v�Z����֐�
    

    ////MY_MEMO�FUNIT5
    //v[0].Texcoord = { 0,0 };
    //v[1].Texcoord = { 1,0 };
    //v[2].Texcoord = { 0,1 };
    //v[3].Texcoord = { 1,1 };

    Immidiate_Context->Unmap(Vertex_Buffer.Get(), 0);
    Immidiate_Context->PSSetShaderResources(0, 1, Shader_Resource_View.GetAddressOf());

    

}

void Sprite_Batch::Begin(ID3D11DeviceContext* dc)
{
    Vertices.clear();
    dc->VSSetShader(Vertex_Shader.Get(), nullptr, 0);
    dc->PSSetShader(Pixel_Shader.Get(), nullptr, 0);
    dc->PSSetShaderResources(0, 1, Shader_Resource_View.GetAddressOf());
    if (!IsSetDissolveTexture)
    {
        dc->PSSetShaderResources(1, 1, Shader_Resource_View.GetAddressOf());
        dc->PSSetShaderResources(2, 1, Shader_Resource_View.GetAddressOf());
    }
}

void Sprite_Batch::render(ID3D11DeviceContext* Immidiate_Context,
    float DstposX, float DstposY,       //��`�̍���̍��W�i�X�N���[�����W�n�j    
    float DstSizeX, float DstSizeY,    //�`�悷��摜�̑傫��
    float r, float g, float b, float a,  //�F�iR,G,B,A�j
    float angle/*degree*/,             //�p�x
    float SrcBeginPosx, float SrcBeginPosy,       //�e�N�X�`���̐؂蔲���ʒu
    float SrcWidth, float SrcHeight)    //�e�N�X�`���̐؂蔲���T�C�Y
{

    D3D11_VIEWPORT Viewport{};
    UINT Num_Viewports{ 1 };
    Immidiate_Context->RSGetViewports(&Num_Viewports, &Viewport);
    
    //�����̒l�����`�̊e���_�̈ʒu���v�Z����
    // -----Left_Top-----
    float left_topX = DstposX;   // 0x
    float left_topY = DstposY;   // 0y
    //-----Right_Top-----
    float right_topX = DstposX + DstSizeX;  // 1x
    float right_topY = DstposY;             // 1y
    //-----Left_Bottom-----
    float left_bottomX = DstposX;              // 2x
    float left_bottomY = DstposY + DstSizeY;   // 2y
    //-----Right_Bottom-----
    float right_bottomX = DstposX + DstSizeX;  // 3x
    float right_bottomY = DstposY + DstSizeY;  // 3y


    //MY_MEMO�F��]�̒��S���摜�̒��S
    float rotate_centerX = Texture2D_Desc.Width * 0.5f;
    float rotate_centerY = Texture2D_Desc.Height * 0.5f;

    //�e���_�ɑ΂��ĉ�]����
    Rotate(left_topX,     left_topY,     rotate_centerX, rotate_centerY, angle);
    Rotate(left_bottomX,  left_bottomY,  rotate_centerX, rotate_centerY, angle);
    Rotate(right_topX,    right_topY,    rotate_centerX, rotate_centerY, angle);
    Rotate(right_bottomX, right_bottomY, rotate_centerX, rotate_centerY, angle);
    

    //�X�N���[�����W�n����NDC�ւ̍��W�ϊ����s��
    //0
    left_topX = 2.0f * left_topX / Viewport.Width - 1.0f;
    left_topY = 1.0f - 2.0f * left_topY / Viewport.Height;
    
    //�Q
    left_bottomX = 2.0f * left_bottomX / Viewport.Width - 1.0f;
    left_bottomY = 1.0f - 2.0f * left_bottomY / Viewport.Height;

    //�P
    right_topX = 2.0f * right_topX / Viewport.Width - 1.0f;
    right_topY = 1.0f - 2.0f * right_topY / Viewport.Height;

    //�R
    right_bottomX = 2.0f * right_bottomX / Viewport.Width - 1.0f;
    right_bottomY = 1.0f - 2.0f * right_bottomY / Viewport.Height;

   
    //�e�N�Z�����W����e�N�X�`�����W�ɕϊ�
    float u0{ SrcBeginPosx / Texture2D_Desc.Width };
    float v0{ SrcBeginPosy / Texture2D_Desc.Height };
    float u1{ (SrcBeginPosx +SrcWidth) / Texture2D_Desc.Width };
    float v1{ (SrcBeginPosx +SrcHeight) / Texture2D_Desc.Height };
   
    Vertices.push_back({ { left_topX,left_topY,      0 }, { r, g, b, a }, { u0, v0 } });
    Vertices.push_back({ { right_topX,right_topY,    0 }, { r, g, b, a }, { u1, v0 } });
    Vertices.push_back({ { left_bottomX,left_bottomY,0 }, { r, g, b, a }, { u0, v1 } });
    
    Vertices.push_back({ { left_bottomX,left_bottomY,  0 }, { r, g, b, a }, { u0, v1 } });
    Vertices.push_back({ { right_topX,right_topY,      0 }, { r, g, b, a }, { u1, v0 } });
    Vertices.push_back({ { right_bottomX,right_bottomY,0 }, { r, g, b, a }, { u1, v1 } });


    ////MY_MEMO�FUNIT5
    
    
}

void Sprite_Batch::render(ID3D11DeviceContext* Immidiate_Context, float DstposX, float DstposY, float DstsizeX, float DstsizeY)
{
    D3D11_VIEWPORT Viewport{};
    UINT Num_Viewports{ 1 };
    Immidiate_Context->RSGetViewports(&Num_Viewports, &Viewport);

    //�����̒l�����`�̊e���_�̈ʒu���v�Z����
    // -----Left_Top-----
    float left_topX    = DstposX;   // 0x
    float left_topY    = DstposY;   // 0y
    //-----Right_Top-----
    float right_topX   = DstposX + DstsizeX;  // 1x
    float right_topY   = DstposY;             // 1y
    //-----Left_Bottom-----
    float left_bottomX = DstposX;              // 2x
    float left_bottomY = DstposY + DstsizeY;   // 2y
    //-----Right_Bottom-----
    float right_bottomX = DstposX + DstsizeX;  // 3x
    float right_bottomY = DstposY + DstsizeY;  // 3y

    //�X�N���[�����W�n����NDC�ւ̍��W�ϊ����s��
    //0
    left_topX = 2.0f * left_topX / Viewport.Width - 1.0f;
    left_topY = 1.0f - 2.0f * left_topY / Viewport.Height;

    //�Q
    left_bottomX = 2.0f * left_bottomX / Viewport.Width - 1.0f;
    left_bottomY = 1.0f - 2.0f * left_bottomY / Viewport.Height;
    
    //�P
    right_topX = 2.0f * right_topX / Viewport.Width - 1.0f;
    right_topY = 1.0f - 2.0f * right_topY / Viewport.Height;

    //�R
    right_bottomX = 2.0f * right_bottomX / Viewport.Width - 1.0f;
    right_bottomY = 1.0f - 2.0f * right_bottomY / Viewport.Height;


    //�e�N�Z�����W����e�N�X�`�����W�ɕϊ�
    float u0{ 0.0f };
    float v0{ 0.0f  };
    float u1{ 1.0f };
    float v1{ 1.0f  };

    Vertices.push_back({ { left_topX,left_topY,      0 }, { 1.0f,1.0f,1.0f,1.0f }, { u0, v0 } });
    Vertices.push_back({ { right_topX,right_topY,    0 }, { 1.0f,1.0f,1.0f,1.0f }, { u1, v0 } });
    Vertices.push_back({ { left_bottomX,left_bottomY,0 }, { 1.0f,1.0f,1.0f,1.0f }, { u0, v1 } });

    Vertices.push_back({ { left_bottomX,left_bottomY,  0 }, { 1.0f,1.0f,1.0f,1.0f }, { u0, v1 } });
    Vertices.push_back({ { right_topX,right_topY,      0 }, { 1.0f,1.0f,1.0f,1.0f }, { u1, v0 } });
    Vertices.push_back({ { right_bottomX,right_bottomY,0 }, { 1.0f,1.0f,1.0f,1.0f }, { u1, v1 } });
}


void Sprite_Batch::render(ID3D11DeviceContext* immediate_context,
    const DirectX::XMFLOAT2 pos,//��`�̍���̍��W�i�X�N���[�����W�n�j
    const DirectX::XMFLOAT2 scale,//�`�悷��X�P�[��
    const DirectX::XMFLOAT2 texpos, //�`�悷���`�̍�����W
    const DirectX::XMFLOAT2 texsize,//�`�悷���`�̃T�C�Y
    const DirectX::XMFLOAT2 pivot,//��_
    const float angle,//degree
    const XMFLOAT4 color//�J���[
)
{
    //�X�N���[���i�r���[�|�[�g�j�̃T�C�Y���擾����
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    immediate_context->RSGetViewports(&num_viewports, &viewport);

    // left-top
    float x0{ pos.x - (pivot.x * scale.x) };
    float y0{ pos.y - (pivot.y * scale.y) };
    // right-top
    float x1{ pos.x + ((texsize.x - pivot.x) * scale.x) };
    float y1{ pos.y - (pivot.y * scale.y) };
    // left-bottom
    float x2{ pos.x - (pivot.x * scale.x) };
    float y2{ pos.y + ((texsize.y - pivot.y) * scale.y) };
    // right-bottom
    float x3{ pos.x + ((texsize.x - pivot.x) * scale.x) };
    float y3{ pos.y + ((texsize.y - pivot.y) * scale.y) };
    //��]
    //��]�̒��S����`�̒��S�_�ɂ����ꍇ
    float cx = pos.x;
    float cy = pos.y;
    Rotate(x0, y0, cx, cy, angle);
    Rotate(x1, y1, cx, cy, angle);
    Rotate(x2, y2, cx, cy, angle);
    Rotate(x3, y3, cx, cy, angle);

    //�X�N���[�����W�n����NDC�ւ̍��W�ϊ��������Ȃ�
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    //texture space�ɕϊ�
    float u0{ texpos.x / Texture2D_Desc.Width };
    float v0{ texpos.y / Texture2D_Desc.Height };
    float u1{ (texpos.x + texsize.x) / Texture2D_Desc.Width };
    float v1{ (texpos.y + texsize.y) / Texture2D_Desc.Height };


    Vertices.push_back({ { x0,y0,0 }, {color.x, color.y, color.z, color.w }, { u0, v0 } });
    Vertices.push_back({ { x1,y1,0 }, {color.x, color.y, color.z, color.w }, { u1, v0 } });
    Vertices.push_back({ { x2,y2,0 }, {color.x, color.y, color.z, color.w }, { u0, v1 } });

    Vertices.push_back({ { x2,y2,0 }, {color.x, color.y, color.z, color.w }, { u0, v1 } });
    Vertices.push_back({ { x1,y1,0 }, {color.x, color.y, color.z, color.w }, { u1, v0 } });
    Vertices.push_back({ { x3,y3,0 }, {color.x, color.y, color.z, color.w }, { u1, v1 } });

}

void Sprite_Batch::End(ID3D11DeviceContext* dc)
{
    //�v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE Mapped_Subresource{};
    hr = dc->Map(Vertex_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped_Subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


    size_t vertex_count = Vertices.size();
    _ASSERT_EXPR(MaxVertices>=vertex_count, "Buffer Overflow");

    Vertex* data{ reinterpret_cast<Vertex*>(Mapped_Subresource.pData) };
    if (data != nullptr)
    {
        const Vertex* p = Vertices.data();
        memcpy_s(data, MaxVertices * sizeof(Vertex), p, vertex_count * sizeof(Vertex));
    }

    dc->Unmap(Vertex_Buffer.Get(), 0);

    //���_�o�b�t�@�̃o�C���h
    UINT stride{ sizeof(Vertex) };
    UINT offset{ 0 };
    dc->IASetVertexBuffers(0, 1, Vertex_Buffer.GetAddressOf(), &stride, &offset);

    //�v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
    dc->IASetInputLayout(Input_Layout.Get());

    // �f�B�]���u�p�̃e�N�X�`�����o�C���h
    if(IsSetDissolveTexture)
    {
        DissolveTexture.SetTexture(dc);
        DstTexture.SetTexture(dc);
    }
    cBuffer.SetBuffer(dc, &Disolvedata, PIXEL);

    //�v���~�e�B�u�̕`��
    dc->Draw(static_cast<UINT>(vertex_count),0);
}


void Sprite_Batch::Render(ID3D11DeviceContext* Immediate_Context,
    const XMFLOAT2 Pos,
    const XMFLOAT2 Scale,
    const XMFLOAT2 TexPos,
    const XMFLOAT2 CutSize,
    const XMFLOAT2 Pivot,
    const float Angle,
    const XMFLOAT4 Color)
{
    // �X�P�[���l���ǂ��炩0�Ȃ�`�悵�Ȃ�
    if (Scale.x == 0.0f || Scale.y == 0.0f) return;


    D3D11_VIEWPORT Viewport{};
    UINT Num_Viewports{ 1 };
    Immediate_Context->RSGetViewports(&Num_Viewports, &Viewport);

    //�����̒l�����`�̊e���_�̈ʒu���v�Z����
    //�X�P�[�����A�g��k��������
    // -----Left_Top-----
    XMFLOAT2 left_top       = Pos;
    //-----Right_Top-----
    XMFLOAT2 right_top      = { Pos.x + (CutSize.x * Scale.x), Pos.y };
    //-----Left_Bottom-----
    XMFLOAT2 left_bottom    = { Pos.x, Pos.y + (CutSize.y * Scale.y) };
    //-----Right_Bottom-----
    XMFLOAT2 right_bottom   = { Pos.x + (CutSize.x * Scale.x), Pos.y + (CutSize.y * Scale.y) };


    //�e���_�ɑ΂��ĉ�]���� (��]�̒��S��Pivot)
    DirectX::XMFLOAT2 Pivot_ = { Pivot.x * CutSize.x, Pivot.y * CutSize.y };
    XMFLOAT2 Center = { (CutSize.x * Scale.x) / CutSize.x * Pivot_.x, (CutSize.y * Scale.y) / CutSize.y * Pivot_.y };
    Rotate(left_top.x, left_top.y,         Center.x, Center.y, Angle);
    Rotate(left_bottom.x, left_bottom.y,   Center.x, Center.y, Angle);
    Rotate(right_top.x, right_top.y,       Center.x, Center.y, Angle);
    Rotate(right_bottom.x, right_bottom.y, Center.x, Center.y, Angle);


    //�X�N���[�����W�n����NDC�ւ̍��W�ϊ����s��
    //0
    left_top.x = 2.0f * left_top.x / Viewport.Width - 1.0f;
    left_top.y = 1.0f - 2.0f * left_top.y / Viewport.Height;

    //�Q
    left_bottom.x = 2.0f * left_bottom.x / Viewport.Width - 1.0f;
    left_bottom.y = 1.0f - 2.0f * left_bottom.y / Viewport.Height;

    //�P
    right_top.x = 2.0f * right_top.x / Viewport.Width - 1.0f;
    right_top.y = 1.0f - 2.0f * right_top.y / Viewport.Height;

    //�R
    right_bottom.x = 2.0f * right_bottom.x / Viewport.Width - 1.0f;
    right_bottom.y = 1.0f - 2.0f * right_bottom.y / Viewport.Height;


    //�e�N�Z�����W����e�N�X�`�����W�ɕϊ�
    float u0{ TexPos.x / Texture2D_Desc.Width };
    float v0{ TexPos.y / Texture2D_Desc.Height };
    float u1{ (TexPos.x + CutSize.x) / Texture2D_Desc.Width };
    float v1{ (TexPos.y + CutSize.y) / Texture2D_Desc.Height };

    Vertices.push_back({ { left_top.x,left_top.y,      0 }, { Color.x, Color.y, Color.z, Color.w }, { u0, v0 } });
    Vertices.push_back({ { right_top.x,right_top.y,    0 }, { Color.x, Color.y, Color.z, Color.w }, { u1, v0 } });
    Vertices.push_back({ { left_bottom.x,left_bottom.y,0 }, { Color.x, Color.y, Color.z, Color.w }, { u0, v1 } });

    Vertices.push_back({ { left_bottom.x,left_bottom.y,  0 }, { Color.x, Color.y, Color.z, Color.w }, { u0, v1 } });
    Vertices.push_back({ { right_top.x,right_top.y,      0 }, { Color.x, Color.y, Color.z, Color.w }, { u1, v0 } });
    Vertices.push_back({ { right_bottom.x,right_bottom.y,0 }, { Color.x, Color.y, Color.z, Color.w }, { u1, v1 } });
}

Sprite_Batch::~Sprite_Batch()
{

    //if (Vertex_Shader) Vertex_Shader->Release();
    //if (Pixel_Shader)   Pixel_Shader->Release();
    //if (Input_Layout)   Input_Layout->Release();
    //if (Vertex_Buffer)  Vertex_Buffer->Release();
    //if (Shader_Resource_View)Shader_Resource_View->Release();
    ReleaseAllTextures();
    
}