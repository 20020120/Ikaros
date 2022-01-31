#include<d3d11.h>
#include<DDSTextureLoader.h>
#include<directxmath.h>
#include<vector>
#include<string>
#include<filesystem>

#include<memory>


#include"TextureLoder.h"
#include<wrl.h>
using namespace Microsoft::WRL;


static std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> Resources;

HRESULT LoadTextureFromFile(ID3D11Device* Device, const wchar_t* FileName, ID3D11ShaderResourceView** ShaderResourceView, D3D11_TEXTURE2D_DESC* Texture2dDesc)
{
    using namespace DirectX;
    using namespace std;

    HRESULT hr{ S_OK };    
    ComPtr<ID3D11Resource> res;

    std::filesystem::path dds_filename(FileName);
    dds_filename.replace_extension("dds");
    if (std::filesystem::exists(dds_filename.c_str()))
    {
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;
        Device->GetImmediateContext(immediate_context.GetAddressOf());
        hr = DirectX::CreateDDSTextureFromFile(Device, immediate_context.Get(), dds_filename.c_str(),
            res.GetAddressOf(), ShaderResourceView);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    else
    {
        hr = CreateWICTextureFromFile(Device, FileName, res.GetAddressOf(), ShaderResourceView);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        Resources.insert(make_pair(FileName, *ShaderResourceView));
    }

    ComPtr<ID3D11Texture2D> Texture2d;

    hr = res.Get()->QueryInterface<ID3D11Texture2D>(Texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    Texture2d->GetDesc(Texture2dDesc);

    return hr;
}

//****************************************************************
// 
//  ジオメトリシェーダーを作成
// 
//****************************************************************

HRESULT CreateGsFromCso(ID3D11Device* Device, const char* CsoName, ID3D11GeometryShader** GeometryShader)
{
    HRESULT hr{ S_OK };

    // ファイルを開けるか確認
    FILE* fp{ nullptr };
    fopen_s(&fp, CsoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not Found");
    fseek(fp, 0, SEEK_END);
    long Cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);
    std::unique_ptr<unsigned char[]> Cso_Data{ std::make_unique<unsigned char[]>(Cso_sz) };
    fread(Cso_Data.get(), Cso_sz, 1, fp);

    hr = Device->CreateGeometryShader(Cso_Data.get(), Cso_sz, nullptr, GeometryShader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    fclose(fp);

    return hr;
}



HRESULT CreateVsFromCso(ID3D11Device* Device, const char* CsoName, ID3D11VertexShader** VertexShader, ID3D11InputLayout** InputLayout, D3D11_INPUT_ELEMENT_DESC* InputElementDesc, UINT NumElements)
{
    //char buffer[MAX_PATH];
    //GetCurrentDirectoryA(MAX_PATH, buffer);
    //char fname[MAX_PATH];
    //sprintf_s(fname, MAX_PATH, "%s\\%s\n", buffer, CsoName);
    //OutputDebugStringA((fname));

    FILE* fp{ nullptr };
    errno_t code = fopen_s(&fp, CsoName, "rb");
    //if (code)
    //{
    //    char errcode[MAX_PATH];
    //    sprintf_s(errcode, MAX_PATH, "error_code : %d \n", code);
    //    OutputDebugStringA(errcode);
    //}
    _ASSERT_EXPR_A(fp, "CSO File not Found");

    fseek(fp, 0, SEEK_END);
    long Cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> Cso_Data{ std::make_unique<unsigned char[]>(Cso_sz) };

    fread(Cso_Data.get(), Cso_sz, 1, fp);

    HRESULT hr{ S_OK };
    hr = Device->CreateVertexShader(Cso_Data.get(), Cso_sz, nullptr, VertexShader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    fclose(fp);
    if (InputLayout)
    {
        hr = Device->CreateInputLayout(InputElementDesc, NumElements,
            Cso_Data.get(), Cso_sz, InputLayout);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    return hr;
}

HRESULT CreatePsFromCso(ID3D11Device* Device, const char* CsoName, ID3D11PixelShader** PixelShader)
{
    //char buffer[MAX_PATH];
    //GetCurrentDirectoryA(MAX_PATH, buffer);
    //char fname[MAX_PATH];
    //sprintf_s(fname, MAX_PATH, "%s\\%s\n", buffer, CsoName);
    //OutputDebugStringA(fname);

    FILE* fp{ nullptr };

    errno_t code = fopen_s(&fp, CsoName, "rb");
    //if(code)
    //{
    //    char errcode[MAX_PATH];
    //    sprintf_s(errcode, MAX_PATH, "error_code : %d \n", code);
    //    OutputDebugStringA(errcode);
    //}
    _ASSERT_EXPR_A(fp, "CSO File Not Found");

    fseek(fp, 0, SEEK_END);
    long Cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> CsoData{ std::make_unique<unsigned char[]>(Cso_sz) };
    fread(CsoData.get(), Cso_sz, 1, fp);
    fclose(fp);
    HRESULT hr{ S_OK };
    hr = Device->CreatePixelShader(CsoData.get(), Cso_sz, nullptr, PixelShader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace);

    return hr;
}

void ReleaseAllTextures()
{
    Resources.clear();
}

//****************************************************************
// 
// UNIT16 ダミーテクスチャ作成関数
// 
//****************************************************************
HRESULT f_MakeDummyTexture(ID3D11Device* Device, ID3D11ShaderResourceView** ShaderResourceView,
    DWORD Value /*0xRRGGBBRR*/, UINT Dimension)
{
    HRESULT hr{ S_OK };

    D3D11_TEXTURE2D_DESC texture2d_desc{};
    texture2d_desc.Width = Dimension;
    texture2d_desc.Height = Dimension;
    texture2d_desc.MipLevels = 1;
    texture2d_desc.ArraySize = 1;
    texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture2d_desc.SampleDesc.Count = 1;
    texture2d_desc.SampleDesc.Quality = 0;
    texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
    texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    size_t texels = Dimension * Dimension;
    std::unique_ptr<DWORD[]> sysmem{ std::make_unique<DWORD[]>(texels) };
    for (size_t i = 0; i < texels; ++i)
    {
        sysmem[i] = Value;
    }


    D3D11_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = sysmem.get();
    subresource_data.SysMemPitch = sizeof(DWORD) * Dimension;

    ComPtr<ID3D11Texture2D> texture2d;
    hr = Device->CreateTexture2D(&texture2d_desc, &subresource_data, texture2d.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
    shader_resource_view_desc.Format = texture2d_desc.Format;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shader_resource_view_desc.Texture2D.MipLevels = 1;
    hr = Device->CreateShaderResourceView(texture2d.Get(), &shader_resource_view_desc, ShaderResourceView);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


    return hr;
}