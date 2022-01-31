#pragma once

#include "ModelComponent.h"
#include <memory>
class GameOverBoss
{
public:
    GameOverBoss(ID3D11Device* d);
    ~GameOverBoss()=default;

    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);

public:

    Transform t;
    Rotate r;
    std::unique_ptr<ModelComponent> model;

};
