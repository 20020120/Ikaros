#pragma once

#include "ModelComponent.h"
#include <vector>

//****************************************************************
// 
//  覐�
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

    DirectX::XMFLOAT3 RotSpeed{}; // ��]���x
    float Speed{};
};



//****************************************************************
// 
//  �Ǘ��N���X
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
    // 覐ΊǗ��N���X
    std::vector<std::unique_ptr<Meteorite>> MeteoriteVec;
};



