#pragma once

#include "ModelComponent.h"
#include <memory>

#include "EventFlag.h"
#include "Light.h"

#include "gAudioSource.h"
#include "Effect.h"

class OverPlayer
{
public:
    OverPlayer(ID3D11Device* d);
    ~OverPlayer() = default;

    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);
    void Finalize();

private:
    void Performance(float elapsedTime);

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

    //getter,setter
    void SetStartTransform(bool frag) { startTransform = frag; }
    [[nodiscard]] bool GetTransformAnim()const { return transformAnim; }
    [[nodiscard]] bool GetFinishPerform()const { return finishPerform; }


    float ang = 0.0f;

    Transform t;
    Rotate r;
    std::unique_ptr<ModelComponent> model;

    std::unique_ptr<GamePointLight> Light;

    // イベントフラグ
    EventFlagSystem Flags;

    float posX;

    //変形して飛んでいく
    bool startTransform{ false };
    bool transformAnim{ false };
    bool finishTransform{ false };
    bool finishPerform{ false };

    int performState = 0;
    float performTimer = 0.0f;

    DirectX::XMFLOAT3 targetPos = { 0,0,0 };
    DirectX::XMFLOAT3 beforePos = { 0,0,0 };
    float ratio = 0.0f;

    std::unique_ptr<AudioSource> seTransform;

    std::unique_ptr<Effect> efkSonicBoom;
    Effekseer::Handle hdlSonicBoom = -1;
    float efkScale = 0.5f;
};