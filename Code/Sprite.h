#pragma once
#include<d3d11.h>
#include<directxmath.h>
#include <WICTextureLoader.h>
#include<string>
class Sprite
{
private:
    ID3D11VertexShader* Vertex_Shader;
    ID3D11PixelShader*   Pixel_Shader;
    ID3D11InputLayout*    Input_Layout;
    ID3D11Buffer*             Vertex_Buffer;

    //MY_MEMO：UNIT5
    ID3D11ShaderResourceView* Shader_Resource_View;
    D3D11_TEXTURE2D_DESC Texture2D_Desc;
    


    //頂点フォーマット
    struct Vertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT4 Color;
        DirectX::XMFLOAT2 Texcoord;  //coord->コーディネート
    };


public:
    /*void render(ID3D11DeviceContext* Immidiate_Context,
        float posX, float posY,
        float scaleX,float scaleY,
        float TextureSizeX, float TextureSizeY, float color_r, float color_g, float color_b, float color_a, float angle);*/
    //MY_MEMO：UNIT6
    void render(ID3D11DeviceContext* Immidiate_Context,
                        float posx, float posy,         //描画する位置
                        float width, float height,  //元の画像の大きさ
                        float r, float g, float b, float a, //色
                        float angle, //角度
                        float cut_posx, float cut_posy, //画像の切り取りたい部分の左上の座標
                        float DrawSizeX, float DrawSizeY //画像を切り取る大きさ(描画の時の大きさ)
                       );

    void Textout(ID3D11DeviceContext* dc, std::string s,
        float x, float y, float width, float height,
        float r, float g, float b, float a);
private:
    void Bind_And_Draw(ID3D11DeviceContext* Immidiate_Context);
    //スプライトの姿勢を計算する関数
    Sprite::Vertex* Calculate_Posture(ID3D11DeviceContext* Immidiate_Context, float posX, float posY, float TextureSizeX, float TextureSizeY, float color_r, float color_g, float color_b, float color_a, float angle);

public:
    //************************************
    Sprite(ID3D11Device* Device,const wchar_t* Filename);
    ~Sprite();
    //************************************

    //ゲッター
    //Vertex* getVertices() { return v; }
};
