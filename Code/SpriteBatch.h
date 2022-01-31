#pragma once
#include<d3d11.h>
#include<directxmath.h>
#include<vector>

using namespace DirectX;

#include"Texture.h"
#include"ConstantBuffer.h"
#include<wrl.h>
using namespace Microsoft::WRL;

using namespace std;
class Sprite_Batch
{
private:
    
    Microsoft::WRL::ComPtr<ID3D11VertexShader> Vertex_Shader;
    //ID3D11VertexShader* Vertex_Shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> Pixel_Shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> Input_Layout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> Vertex_Buffer;

    //MY_MEMO：UNIT5
    Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> Shader_Resource_View;
    D3D11_TEXTURE2D_DESC Texture2D_Desc;

    // ラスタライザーステート
    Microsoft::WRL::ComPtr< ID3D11RasterizerState> Rs_State[2]{ nullptr };



    //頂点フォーマット
    struct Vertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT4 Color;
        DirectX::XMFLOAT2 Texcoord;  //coord->コーディネート
    };


    const size_t MaxVertices;
    std::vector<Vertex> Vertices;

public:
    void Begin(ID3D11DeviceContext* Dc);
    void End(ID3D11DeviceContext* Dc);
    void render(ID3D11DeviceContext* Immidiate_Context,
        float posX, float posY,
        float scaleX, float scaleY,
        float TextureSizeX, float TextureSizeY, float color_r, float color_g, float color_b, float color_a, float angle);
    //MY_MEMO：UNIT6
    void render(ID3D11DeviceContext* Immidiate_Context,
        float posx, float posy,         //描画する位置
        float width, float height,  //元の画像の大きさ
        float r, float g, float b, float a, //色
        float angle, //角度
        float cut_posx, float cut_posy, //画像の切り取りたい部分の左上の座標
        float DrawSizeX, float DrawSizeY //画像を切り取る大きさ(描画の時の大きさ)
    );
    void render(ID3D11DeviceContext* Immidiate_Context,
        float posX, float posY,
        float Width, float Height);

    void render(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT2 pos,//矩形の左上の座標（スクリーン座標系）
        const DirectX::XMFLOAT2 scale,//描画するスケール
        const DirectX::XMFLOAT2 texpos, //描画する矩形の左上座標
        const DirectX::XMFLOAT2 texsize,//描画する矩形のサイズ
        const DirectX::XMFLOAT2 pivot,//基準点
        const float angle,//degree
        const XMFLOAT4 color//カラー
    );

    void Render(ID3D11DeviceContext* Immediate_Context,
        const XMFLOAT2 Pos    /* 位置 */,
        const XMFLOAT2 Scale  /* 大きさ */,
        const XMFLOAT2 TexPos /* 切り抜き位置 */,
        const XMFLOAT2 CutSize/* 切り抜きサイズ */,
        const XMFLOAT2 Pivot  /* 基準点 (0 ~ 1) */,
        const float Angle     /* 角度 */ = 0.0f,
        const XMFLOAT4 Color  /* 色 */ = {1.0f,1.0f,1.0f,1.0f}
    );

    void Render(ID3D11DeviceContext* Immediate_Context,
        const float PosX, const float PosY,
        const float ScaleX, const float ScaleY,
        const float TexPosX, const float TexPosY,
        const float CutSizeX, const float CutSizeY,
        const float PivotX, const float PivotY,
        const float Angle = 0.0f,
        const float ColorR = 1.0f, const float ColorG = 1.0f, const float ColorB = 1.0f, const float ColorA = 1.0f)
    {
        Render(Immediate_Context, { PosX,PosY }, { ScaleX,ScaleY }, { TexPosX,TexPosY }, { CutSizeX,CutSizeY }, { PivotX,PivotY }, Angle, { ColorR,ColorG,ColorB,ColorA });
    };

    void Render(ID3D11DeviceContext* Immediate_Context,
        const XMFLOAT2 Pos    /* 位置 */,
        const XMFLOAT2 Scale  /* 大きさ */,
        const XMFLOAT2 Pivot  /* 基準点 (0 ~ 1) */ = {0.5f,0.5f},
        const float Angle     /* 角度 */ = 0.0f,
        const XMFLOAT4 Color  /* 色 */ = { 1.0f,1.0f,1.0f,1.0f }
    )
    {
        Render(Immediate_Context, Pos, Scale, { 0.0f,0.0f }, { static_cast<float>(this->Texture2D_Desc.Width), static_cast<float>(this->Texture2D_Desc.Height) }, Pivot, Angle, Color);
    }

    void Render(ID3D11DeviceContext* Immediate_Context,
        const float PosX, const float PosY,
        const float ScaleX, const float ScaleY,
        const float PivotX = 0.5f, const float PivotY = 0.5f,
        const float Angle = 0.0f,
        const float ColorR = 1.0f, const float ColorG = 1.0f, const float ColorB = 1.0f, const float ColorA = 1.0f)
    {
        Render(Immediate_Context, { PosX,PosY }, { ScaleX,ScaleY }, { 0.0f,0.0f }, { static_cast<float>(this->Texture2D_Desc.Width), static_cast<float>(this->Texture2D_Desc.Height) }, { PivotX,PivotY }, Angle, { ColorR,ColorG,ColorB,ColorA });
    }

    void SetDissolvePower(float p) { Disolvedata.Power = p; }

public: // Get関数
    float GetTextureWidth() { return static_cast<float>(Texture2D_Desc.Width); }
    float GetTextureHeight() { return static_cast<float>(Texture2D_Desc.Height); }
    
private:
    //****************************************************************
    // 
    // ディゾルブ
    // 
    //****************************************************************
    Texture DissolveTexture;  // ディゾルブのマスク画像
    Texture DstTexture;       // ディゾルブ先の画像
    bool IsSetDissolveTexture;

    struct DissolveData
    {
        float Power{};
        DirectX::XMFLOAT3 pad0{};
    };
    DissolveData Disolvedata;

    ConstantBuffer cBuffer{};

public:
    //************************************
    Sprite_Batch(ID3D11Device* Device, const wchar_t* Filename, size_t MaxSprite, const wchar_t* DissolveFilename = nullptr, const wchar_t* DstFilename = nullptr);
    ~Sprite_Batch();
    //************************************

    //ゲッター
    //Vertex* getVertices() { return v; }
};

