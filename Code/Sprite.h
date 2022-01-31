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

    //MY_MEMO�FUNIT5
    ID3D11ShaderResourceView* Shader_Resource_View;
    D3D11_TEXTURE2D_DESC Texture2D_Desc;
    


    //���_�t�H�[�}�b�g
    struct Vertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT4 Color;
        DirectX::XMFLOAT2 Texcoord;  //coord->�R�[�f�B�l�[�g
    };


public:
    /*void render(ID3D11DeviceContext* Immidiate_Context,
        float posX, float posY,
        float scaleX,float scaleY,
        float TextureSizeX, float TextureSizeY, float color_r, float color_g, float color_b, float color_a, float angle);*/
    //MY_MEMO�FUNIT6
    void render(ID3D11DeviceContext* Immidiate_Context,
                        float posx, float posy,         //�`�悷��ʒu
                        float width, float height,  //���̉摜�̑傫��
                        float r, float g, float b, float a, //�F
                        float angle, //�p�x
                        float cut_posx, float cut_posy, //�摜�̐؂��肽�������̍���̍��W
                        float DrawSizeX, float DrawSizeY //�摜��؂���傫��(�`��̎��̑傫��)
                       );

    void Textout(ID3D11DeviceContext* dc, std::string s,
        float x, float y, float width, float height,
        float r, float g, float b, float a);
private:
    void Bind_And_Draw(ID3D11DeviceContext* Immidiate_Context);
    //�X�v���C�g�̎p�����v�Z����֐�
    Sprite::Vertex* Calculate_Posture(ID3D11DeviceContext* Immidiate_Context, float posX, float posY, float TextureSizeX, float TextureSizeY, float color_r, float color_g, float color_b, float color_a, float angle);

public:
    //************************************
    Sprite(ID3D11Device* Device,const wchar_t* Filename);
    ~Sprite();
    //************************************

    //�Q�b�^�[
    //Vertex* getVertices() { return v; }
};
