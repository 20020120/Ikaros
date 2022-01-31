#pragma once
#include "Camera.h"
#include "Timer.h"

class TitleCamera final
{
public:
    TitleCamera(DirectX::XMFLOAT3* target_):target(target_){};
    ~TitleCamera() = default;

public:
    void Initialize(ID3D11Device* device);
    void Update(float elapsedTime);
    void ConstantBufferUpdate(ID3D11DeviceContext* dc);

public: // GetŠÖ”
    const auto& GetPhaseIndex() { return phase_index; }
    auto& GetView() { return camera->GetView(); }
    auto& GetProjection() { return camera->GetProjection(); }

public: // SetŠÖ”
    void SetNextCameraPos(const DirectX::XMFLOAT3& pos) { next_camera_position = pos; }
    void StartDemoCameraShake() { camera->SetDemoCameraShake(); }

private:
    bool CameraPosLerp(float elapsetTime);

private:
    using pFunc = void(TitleCamera::*)(float);

    pFunc phase[6] =
    {
        &TitleCamera::Phase0,
        &TitleCamera::Phase1,
        &TitleCamera::Phase2,
        &TitleCamera::Phase3,
        &TitleCamera::Phase4,
        &TitleCamera::Phase5,
    };

    void Phase0(float elapsedTime);
    void Phase1(float elapsedTime);
    void Phase2(float elapsedTime);
    void Phase3(float elapsedTime);
    void Phase4(float elapsedTime);
    void Phase5(float elapsedTime);

public:
    static constexpr DirectX::XMFLOAT3 left_front_pos = { 8.41f,0.18f,-17.0f };
    static constexpr DirectX::XMFLOAT3 back_pos = { 0.0f,8.0f,20.0f };

private:
    std::unique_ptr<Camera> camera;
    Timer timer;

    DirectX::XMFLOAT3 next_camera_position;
    DirectX::XMFLOAT3* target;

    short phase_index;
};
