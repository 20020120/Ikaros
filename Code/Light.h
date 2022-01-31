#pragma once

#include"ConstantBuffer.h"
#include"ModelComponent.h"

//****************************************************************
// 
// �f�B���N�V�������C�g
// 
//****************************************************************
class DirectionLight
{
public:
    DirectionLight(ID3D11Device*d);
    ~DirectionLight();

    void Update(ID3D11DeviceContext* dc);// �X�V����(DeviceContext���K�v)

    void GuiMenu();

    void SetDirection(DirectX::XMFLOAT3 dir) { data.direction = dir; }
private:
    ConstantBuffer cb{};

    struct Data
    {
        DirectX::XMFLOAT3 direction{}; // ���C�g�̌���
        float pad0;
        DirectX::XMFLOAT4 Color{}; // ���C�g�̐F
        float Power{}; //���C�g�̋���
        DirectX::XMFLOAT3 pad1{}; //�p�f�B���O
    };
    Data data{};
    
};


//****************************************************************
// 
// �|�C���g���C�g
// 
//****************************************************************
class PointLight
{
public:
    PointLight(ID3D11Device* d, int Ps_Slot);
    ~PointLight();

    void Update(ID3D11DeviceContext* dc);

    void SetPosition(DirectX::XMFLOAT3 pos, int Index);
    void SetColor(DirectX::XMFLOAT3 col, int Index);
    void SetRange(float range, int Index);

private:
    ConstantBuffer c_buf{};
    static constexpr int Value = 10;

    struct Data
    {
        DirectX::XMFLOAT4 Position[Value]{};
        DirectX::XMFLOAT4 Color[Value]{};
        DirectX::XMFLOAT4 InfluenceRange[Value]{};
    };

    Data data{};

    int Ps_Slot{};

    //std::unique_ptr<ModelComponent> Model[Value];
};

//****************************************************************
// 
//  �Q�[����ʗp�|�C���g���C�g
// 
//****************************************************************

class GamePointLight
{
public:
    GamePointLight(ID3D11Device* d, int Index);
    ~GamePointLight();


    void Update(ID3D11DeviceContext* dc);

    void SetPosition(DirectX::XMFLOAT3 pos);
    void SetColor(DirectX::XMFLOAT3 col);
    void SetRange(float range);

private:
    ConstantBuffer c_buf{};

    struct Data
    {
        DirectX::XMFLOAT4 Position{};
        DirectX::XMFLOAT4 Color{};
        DirectX::XMFLOAT4 InfluenceRange{};
    };

    Data data{};

    int Ps_Slot{};

    //std::unique_ptr<ModelComponent> Model;


};



//****************************************************************
// 
//  �X�|�b�g���C�g
// 
//****************************************************************
class SpotLight
{
public:
    SpotLight(ID3D11Device* d,int Slot);
    ~SpotLight();

    void Update(ID3D11DeviceContext* dc);

    void SetPotition(DirectX::XMFLOAT3 pos);
    void SetAngle(float ang);
    void SetColor(DirectX::XMFLOAT3 col);
    void SetDirection(DirectX::XMFLOAT3 dir);
    void SetRange(float range);

private:

    ConstantBuffer c_buf{};
    struct Data
    {
        DirectX::XMFLOAT4 Position{}; // �ʒu
        float Angle{};    // �p�x
        DirectX::XMFLOAT3 pad0{};
        DirectX::XMFLOAT4 Color{};    // �F 
        DirectX::XMFLOAT3 Direction{};// �ˏo����
        float Range{};                // �e���͈�
    };

    Data data{};
};