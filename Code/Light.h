#pragma once

#include"ConstantBuffer.h"
#include"ModelComponent.h"

//****************************************************************
// 
// ディレクションライト
// 
//****************************************************************
class DirectionLight
{
public:
    DirectionLight(ID3D11Device*d);
    ~DirectionLight();

    void Update(ID3D11DeviceContext* dc);// 更新処理(DeviceContextが必要)

    void GuiMenu();

    void SetDirection(DirectX::XMFLOAT3 dir) { data.direction = dir; }
private:
    ConstantBuffer cb{};

    struct Data
    {
        DirectX::XMFLOAT3 direction{}; // ライトの向き
        float pad0;
        DirectX::XMFLOAT4 Color{}; // ライトの色
        float Power{}; //ライトの強さ
        DirectX::XMFLOAT3 pad1{}; //パディング
    };
    Data data{};
    
};


//****************************************************************
// 
// ポイントライト
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
//  ゲーム画面用ポイントライト
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
//  スポットライト
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
        DirectX::XMFLOAT4 Position{}; // 位置
        float Angle{};    // 角度
        DirectX::XMFLOAT3 pad0{};
        DirectX::XMFLOAT4 Color{};    // 色 
        DirectX::XMFLOAT3 Direction{};// 射出方向
        float Range{};                // 影響範囲
    };

    Data data{};
};