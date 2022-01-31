#pragma once

#include "BaseMovePhase.h"
#include "Timer.h"


class Start_FromBack_Side final : public BaseMove
{
public:
    Start_FromBack_Side(XMFLOAT3& scale_, XMFLOAT3 target_scale_);
    ~Start_FromBack_Side()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void MoveFront(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle);
    void SpeedRatio(float elapsedTime, const float& now_time);
    void RotationY180(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void LerpPositionX(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position);
    void RotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void ScaleUp(float elapsedTime, const float& now_time);

private:
    Timer timer;
    float speed;
    XMFLOAT3& scale;
    const XMFLOAT3 target_scale;
};


class Start_FromFront_Side final : public BaseMove
{
public:
    Start_FromFront_Side(XMFLOAT3& scale_, XMFLOAT3 target_scale_);
    ~Start_FromFront_Side()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void MoveFront(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle);
    void SpeedRatio(float elapsedTime, const float& now_time);
    void RotationY180(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void LerpPositionX(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position);
    void RotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void ScaleUp(float elapsedTime, const float& now_time);

private:
    Timer timer;
    float speed;
    XMFLOAT3& scale;
    const XMFLOAT3 target_scale;
};



class Start_FromYDown_Side final : public BaseMove
{
public:
    Start_FromYDown_Side(XMFLOAT3 start);
    ~Start_FromYDown_Side()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void LerpPositionX(float elapsedTime, DirectX::XMFLOAT3& position);

private:
    Timer timer;
    static constexpr float all_time = 1.5f;
    std::vector<XMFLOAT3> control_point;
};



class Start_FromYUp_Side final : public BaseMove
{
public:
    Start_FromYUp_Side(XMFLOAT3 start);
    ~Start_FromYUp_Side()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void LerpPositionX(float elapsedTime, DirectX::XMFLOAT3& position);

private:
    Timer timer;
    static constexpr float all_time = 1.5f;
    std::vector<XMFLOAT3> control_point;
};












class Start_FromYDown_RotateX_Side final : public BaseMove
{
public:
    Start_FromYDown_RotateX_Side(XMFLOAT3 start);
    ~Start_FromYDown_RotateX_Side()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void LerpPositionX(float elapsedTime, DirectX::XMFLOAT3& position);
    void LerpRotateX360(const float ratio, DirectX::XMFLOAT3& angle);

private:
    Timer timer;
    static constexpr float all_time = 1.5f;
    std::vector<XMFLOAT3> control_point;
};



class Start_FromYUp_RotateX_Side final : public BaseMove
{
public:
    Start_FromYUp_RotateX_Side(XMFLOAT3 start);
    ~Start_FromYUp_RotateX_Side()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void LerpPositionX(float elapsedTime, DirectX::XMFLOAT3& position);
    void LerpRotateX360(const float ratio, DirectX::XMFLOAT3& angle);

private:
    Timer timer;
    static constexpr float all_time = 1.5f;
    std::vector<XMFLOAT3> control_point;
};