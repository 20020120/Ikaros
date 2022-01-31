#pragma once

#include "ModelComponent.h"
#include <memory>
class ClearShip
{
public:
    ClearShip(ID3D11Device* d);
    ~ClearShip() = default;

    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);
    void RankingInitialize();
public:

    Transform t;
    Rotate r;
    std::unique_ptr<ModelComponent> model;
};

class ClearSkyDome
{
public:
    ClearSkyDome(ID3D11Device* d);
    ~ClearSkyDome() = default;

    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);

public:

    Transform t;
    Rotate r;
    std::unique_ptr<ModelComponent> model;
};