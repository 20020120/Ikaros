#pragma once

#include "ModelComponent.h"
#include <memory>
#include"Light.h"

class ClearPlayer
{
public:
    ClearPlayer(ID3D11Device* d);
    ~ClearPlayer() = default;

    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);

public:
    enum AnimationName {
        wait,
        FinisherBeamBegin,
        FinisherBeamLoop,
        FinisherBeamEnd,
        Shot_Recoil,
        Slash_side,
        Slash_top,
        FinisherSwordBegin,
        FinisherSwordSlash,
        GameOver,
        GameClearBegin,
        GameClearLoop,
        transform,
        inv_transform,
        wing,
    };

    float ang = 0.0f;

    std::unique_ptr<GamePointLight> mPointLight{};

    Transform t;
    Rotate r;
    std::unique_ptr<ModelComponent> model;
};