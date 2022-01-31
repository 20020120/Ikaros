#pragma once

#include "ModelComponent.h"
#include <vector>

//****************************************************************
// 
//  隕石
// 
//****************************************************************
class Meteorite final
{
public:
    Meteorite(ID3D11Device* d, int ModelType,DirectX::XMFLOAT3 IniPosition);
    ~Meteorite();


    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);

private:
    std::unique_ptr<ModelComponent> Model;

    Transform t{};
    Rotate r{};

    DirectX::XMFLOAT3 RotSpeed{}; // 回転速度
    float Speed{};
};



//****************************************************************
// 
//  管理クラス
// 
//****************************************************************

class DecorationManager
{
public:
    DecorationManager(ID3D11Device* d);
    ~DecorationManager() = default;

    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);
private:
    // 隕石管理クラス
    std::vector<std::unique_ptr<Meteorite>> MeteoriteVec;
};



