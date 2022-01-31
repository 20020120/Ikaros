#include"Sprite.h"
#include"misc.h"
#include<sstream>

#include <WICTextureLoader.h>

Sprite::Sprite(ID3D11Device* Device, const wchar_t* Filename)
    :Input_Layout()
    , Vertex_Buffer()
    , Pixel_Shader()
    , Vertex_Shader()
    , Shader_Resource_View()
    , Texture2D_Desc()
    
{
    //頂点情報のセット
    Vertex v[4];
    v[0] = { { -1.0,  +1.0, 0 }, { 1, 1, 1, 1 }, { 0, 0 } };
    v[1] = { { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 1, 0 } };
    v[2] = { { -1.0,  -1.0,  0 }, { 1, 1, 1, 1 }, { 0, 1 } };
    v[3] = { { +1.0, -1.0,  0 }, { 1, 1, 1, 1 }, { 1, 1 } };
    


    

    /* struct D3D11_INPUT_ELEMENT_DESC {
        LPCSTR                     SemanticName;   シェーダー入力シグネチャの要素に関連付けられたHLSLセマンティック（は？）
        UINT                       SemanticIndex;　　 要素のインデックス　例：4x4 行列にはSemanticNameを持つ 4 つのコンポーネントがある
        DXGI_FORMAT                Format;　　   要素のデータ型
        UINT                       InputSlot;　　　　　入力アセンブラを識別する整数値　範囲は０～１５
        UINT                       AlignedByteOffset;　頂点の先頭からのオフセット
        D3D11_INPUT_CLASSIFICATION InputSlotClass;　　単一の入力スロットの入力データクラスを識別します
        UINT                       InstanceDataStepRate;　　（は？）
    }*/

    

#if 0

    //頂点バッファオブジェクトの生成
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
    hr = Device->CreateBuffer(&Buffer_Desc, &Subresource_Data, & Vertex_Buffer);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#endif // 0

    //MY_MEMO：UNIT３
    //  参考URL：「DirectX11の頂点バッファとインデックスバッファ」http://gameworkslab.jp/2019/06/15/directx11%e3%81%ae%e9%a0%82%e7%82%b9%e3%83%90%e3%83%83%e3%83%95%e3%82%a1%e3%81%a8%e3%82%a4%e3%83%b3%e3%83%87%e3%83%83%e3%82%af%e3%82%b9%e3%83%90%e3%83%83%e3%83%95%e3%82%a1/2/
    // バッファリソースを作成する
    //頂点バッファオブジェクトの生成   
    D3D11_BUFFER_DESC Buffer_Desc{};
    Buffer_Desc.ByteWidth = sizeof(v);
    Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;   //バッファの読み取り・書き込み方法を変更する
    //Buffer_Desc.Usageの詳細URL   https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage

    Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;   //CPUのアクセスフラグ（必要ない場合は０）
    //Buffer_Desc.CPUAccessFlagsの詳細URL：https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_cpu_access_flag

    Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   //バッファをパイプラインにバインドする方法を識別する
    //Buffer_Desc.BindFlagsの詳細URL：https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_bind_flag

    Buffer_Desc.MiscFlags = 0;   //その他のフラグ（未使用の場合は０）
    Buffer_Desc.StructureByteStride = 0;  //コンピュートシェーダで使用するStructureBuffer（GPUで使用する配列）を使用する際に指定(は？)

    HRESULT hr{ S_OK };

    //MY_MEMO：UNIT3
    D3D11_SUBRESOURCE_DATA Subresource_Data{};
    Subresource_Data.pSysMem = v;
    Subresource_Data.SysMemPitch = 0;
    Subresource_Data.SysMemSlicePitch = 0;
    hr = Device->CreateBuffer(&Buffer_Desc, &Subresource_Data, &Vertex_Buffer);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    

   //MY_MEMO：UNIT5
    ID3D11Resource* Resource{};
    hr = DirectX::CreateWICTextureFromFile(Device, Filename, &Resource, &Shader_Resource_View);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    Resource->Release();

    //MY_MEMO：UNIT5
    ID3D11Texture2D* Texture2d{};
    hr = Resource->QueryInterface<ID3D11Texture2D>(&Texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    Texture2d->GetDesc(&Texture2D_Desc);
    Texture2d->Release();

    //頂点シェーダーオブジェクトの生成
    const char* cso_name{ "Sprite_vs.cso" };
    FILE* fp{};
    fopen_s(&fp, cso_name, "rb");
    _ASSERT_EXPR_A(fp, "CS File not found");

    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    
    hr = Device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, &Vertex_Shader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    //入力レイアウトオブジェクトの生成
    D3D11_INPUT_ELEMENT_DESC Input_Erement_Desc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    hr = Device->CreateInputLayout(Input_Erement_Desc, _countof(Input_Erement_Desc),
        cso_data.get(), cso_sz, &Input_Layout);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    

    //ピクセルシェーダーオブジェクトの生成
    const char* cso_ps_name{ "Sprite_ps.cso" };
    //FILE* fp{};
    //HACK ファイルに不備があったらチェック
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
}

//MY_MEMO：UNIT4「SPRITE-ROTATION」
//ラムダ式：点(x, y)が点(cx, cy)を中心に角(angle)で回転した時の座標を計算する
auto Rotate = [=](float& x, float& y, float cx, float cy, float Angle)
{
    //座標（ｘ,ｙ）を原点に移動
    x -= cx;
    y -= cy;

    //sin,cos
    float L_Sin  = sin(DirectX::XMConvertToRadians(Angle));
    float L_Cos = cos(DirectX::XMConvertToRadians(Angle));

    float L_tx{ x };
    float L_ty{ y };

    x = (L_tx * L_Cos) + -(L_ty * L_Sin);
    y = (L_tx * L_Sin) +  (L_ty * L_Cos);

    //座標（ｘ,ｙ）を元の位置に移動
    x += cx;
    y += cy;
};

//void Sprite::render(ID3D11DeviceContext* Immidiate_Context,
//    float posX,   float posY,   //矩形の左上の座標（スクリーン座標系）
//    float scaleX, float scaleY, //スケール
//    float TextureSizeX,  float TextureSizeY, //描画する画像の大きさ
//    float r,float g,float b,float a,  //色（R,G,B,A）
//    float angle/*degree*/)
//{
//    //テクスチャの姿勢を計算する関数
//    Vertex* v= Calculate_Posture(Immidiate_Context, posX, posY, TextureSizeX*scaleX, TextureSizeY*scaleY, r, g, b, a, angle);
//
//    ////MY_MEMO：UNIT5
//    v[0].Texcoord = { 0,0 };
//    v[1].Texcoord = { 1,0 };
//    v[2].Texcoord = { 0,1 };
//    v[3].Texcoord = { 1,1 };
//
//    Immidiate_Context->Unmap(Vertex_Buffer, 0);
//    Immidiate_Context->PSSetShaderResources(0, 1, &Shader_Resource_View);
//
//    Bind_And_Draw(Immidiate_Context);
//    
//}


void Sprite::render(ID3D11DeviceContext* Immidiate_Context,
    float DstposX, float DstposY,       //矩形の左上の座標（スクリーン座標系）    
    float DstSizeX, float DstSizeY,    //描画する画像の大きさ
    float r, float g, float b, float a,  //色（R,G,B,A）
    float angle/*degree*/ ,             //角度
    float Srcposx, float Srcposy,       //テクスチャの切り抜き位置
    float SrcWidth, float SrcHeight)    //テクスチャの切り抜きサイズ
{


    Immidiate_Context->VSSetShader(Vertex_Shader, nullptr, 0);
    Immidiate_Context->PSSetShader(Pixel_Shader, nullptr, 0);
    Immidiate_Context->PSSetShaderResources(0, 1, &Shader_Resource_View);

    //テクスチャの姿勢を計算する関数
    //Vertex* v = Calculate_Posture(Immidiate_Context, DstposX, DstposY, SrcWidth , SrcHeight, r, g, b, a, angle);

    D3D11_VIEWPORT Viewport{};
    UINT Num_Viewports{ 1 };
    Immidiate_Context->RSGetViewports(&Num_Viewports, &Viewport);

    

    //引数の値から矩形の各頂点の位置を計算する
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

    //MY_MEMO：回転の中心が画像の中心
    float L_cx = DstposX + (DstSizeX * 0.5f);
    float L_cy = DstposY + (DstSizeY * 0.5f);

    //MY_MEMO：回転の中心が画像の中心
    float rotate_centerX = Texture2D_Desc.Width * 0.5f;
    float rotate_centerY = Texture2D_Desc.Height * 0.5f;

    //各頂点に対して回転処理
    Rotate(left_topX, left_topY, rotate_centerX, rotate_centerY, angle);
    Rotate(left_bottomX, left_bottomY, rotate_centerX, rotate_centerY, angle);
    Rotate(right_topX, right_topY, rotate_centerX, rotate_centerY, angle);
    Rotate(right_bottomX, right_bottomY, rotate_centerX, rotate_centerY, angle);

    //スクリーン座標系からNDCへの座標変換を行う
    //0
    left_topX = 2.0f * left_topX / Viewport.Width - 1.0f;
    left_topY = 1.0f - 2.0f * left_topY / Viewport.Height;

    //２
    left_bottomX = 2.0f * left_bottomX / Viewport.Width - 1.0f;
    left_bottomY = 1.0f - 2.0f * left_bottomY / Viewport.Height;

    //１
    right_topX = 2.0f * right_topX / Viewport.Width - 1.0f;
    right_topY = 1.0f - 2.0f * right_topY / Viewport.Height;

    //３
    right_bottomX = 2.0f * right_bottomX / Viewport.Width - 1.0f;
    right_bottomY = 1.0f - 2.0f * right_bottomY / Viewport.Height;

    //テクセル座標からテクスチャ座標に変換
    //-----Texpos-----
   

    float x4 = Srcposx / Texture2D_Desc.Width;
    float y4 = Srcposy / Texture2D_Desc.Height;
    float x5 = (Srcposx + SrcWidth) / Texture2D_Desc.Width;
    float y5 = (Srcposy + SrcHeight) / Texture2D_Desc.Height;



    
    //計算結果で頂点バッファオブジェクトを更新する
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE Mapped_Subresource{};
    hr = Immidiate_Context->Map(Vertex_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped_Subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    Vertex* vertex{ reinterpret_cast<Vertex*>(Mapped_Subresource.pData) };
    //計算した頂点の値を代入
    if (vertex != nullptr)
    {
        vertex[0].pos = { left_topX,left_topY,0 };
        vertex[1].pos = { right_topX,right_topY,0 };
        vertex[2].pos = { left_bottomX,left_bottomY,0 };
        vertex[3].pos = { right_bottomX,right_bottomY,0 };
        //色を決定する

        vertex[0].Color = vertex[1].Color = vertex[2].Color = vertex[3].Color = { r,g,b,a };
    }

    //MY_MEMO：UNIT5
    vertex[0].Texcoord = { x4,y4 };
    vertex[1].Texcoord = { x5,y4 };
    vertex[2].Texcoord = { x4,y5 };
    vertex[3].Texcoord = { x5,y5 };


    Immidiate_Context->Unmap(Vertex_Buffer, 0);
    Immidiate_Context->PSSetShaderResources(0, 1, &Shader_Resource_View);

    //頂点バッファのバインド
    UINT stride{ sizeof(Vertex) };
    UINT offset{ 0 };
    Immidiate_Context->IASetVertexBuffers(0, 1, &Vertex_Buffer, &stride, &offset);

    //プリミティブタイプおよびデータの順序に関する情報のバインド
    Immidiate_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    //入力レイアウトオブジェクトのバインド
    Immidiate_Context->IASetInputLayout(Input_Layout);

    //シェーダーのバインド
    Immidiate_Context->VSSetShader(Vertex_Shader, nullptr, 0);
    Immidiate_Context->PSSetShader(Pixel_Shader, nullptr, 0);

    //プリミティブの描画
    Immidiate_Context->Draw(4, 0);
}

void Sprite::Textout(ID3D11DeviceContext* dc, std::string s, float x, float y, float width, float height, float r, float g, float b, float a)
{
    float sw = static_cast<float>(Texture2D_Desc.Width / 16);
    float sh = static_cast<float>(Texture2D_Desc.Height / 16);
    float Carriage{ 0.0f };
    for (const char c : s)
    {
        render(dc, x + Carriage, y, width, height, r, g, b, a, 0,
            sw * (c & 0x0F), sh * (c >> 4), sw, sh);
        Carriage += width;
    }
}


void Sprite::Bind_And_Draw(ID3D11DeviceContext* Immidiate_Context)
{
    //頂点バッファのバインド
    UINT stride{ sizeof(Vertex) };
    UINT offset{ 0 };
    Immidiate_Context->IASetVertexBuffers(0, 1, &Vertex_Buffer, &stride, &offset);

    //プリミティブタイプおよびデータの順序に関する情報のバインド
    Immidiate_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    //入力レイアウトオブジェクトのバインド
    Immidiate_Context->IASetInputLayout(Input_Layout);

    //シェーダーのバインド
    Immidiate_Context->VSSetShader(Vertex_Shader, nullptr, 0);
    Immidiate_Context->PSSetShader(Pixel_Shader, nullptr, 0);

    //プリミティブの描画
    Immidiate_Context->Draw(4, 0);
}

Sprite::Vertex*  Sprite::Calculate_Posture(ID3D11DeviceContext* Immidiate_Context,
    float posX, float posY, float TextureSizeX, float TextureSizeY, 
    float r, float g, float b, float a, 
    float angle)
{
    D3D11_VIEWPORT Viewport{};
    UINT Num_Viewports{ 1 };
    Immidiate_Context->RSGetViewports(&Num_Viewports, &Viewport);

    //引数の値から矩形の各頂点の位置を計算する
// -----Left_Top-----
    float x0 = posX;
    float y0 = posY;
    //-----Right_Top-----
    float x1 = posX + TextureSizeX;
    float y1 = posY;
    //-----Left_Bottom-----
    float x2 = posX;
    float y2 = posY + TextureSizeY;
    //-----Right_Bottom-----
    float x3 = posX + TextureSizeX;
    float y3 = posY + TextureSizeY;

    //MY_MEMO：回転の中心が画像の中心
    float L_cx = posX + (TextureSizeX * 0.5f);
    float L_cy = posY + (TextureSizeY * 0.5f);

    //各頂点に対して回転処理
    Rotate(x0, y0, L_cx, L_cy, angle);
    Rotate(x1, y1, L_cx, L_cy, angle);
    Rotate(x2, y2, L_cx, L_cy, angle);
    Rotate(x3, y3, L_cx, L_cy, angle);

    //スクリーン座標系からNDCへの座標変換を行う
    x0 = 2.0f * x0 / Viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / Viewport.Height;
    x1 = 2.0f * x1 / Viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / Viewport.Height;
    x2 = 2.0f * x2 / Viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / Viewport.Height;
    x3 = 2.0f * x3 / Viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / Viewport.Height;

    //計算結果で頂点バッファオブジェクトを更新する
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE Mapped_Subresource{};
    hr = Immidiate_Context->Map(Vertex_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped_Subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    
    Vertex* v{ reinterpret_cast<Vertex*>(Mapped_Subresource.pData) };
    

    if (v != nullptr)
    {
        v[0].pos = { x0,y0,0 };
        v[1].pos = { x1,y1,0 };
        v[2].pos = { x2,y2,0 };
        v[3].pos = { x3,y3,0 };
        //色を決定する

        v[0].Color = v[1].Color = v[2].Color = v[3].Color = { r,g,b,a };
    }
    return v;
}




Sprite::~Sprite()
{

    if(Vertex_Shader) Vertex_Shader->Release();
    if(Pixel_Shader)   Pixel_Shader->Release();
    if(Input_Layout)   Input_Layout->Release();
    if(Vertex_Buffer)  Vertex_Buffer->Release();
    if (Shader_Resource_View)Shader_Resource_View->Release();
    
    
}