#pragma once

#include "BaseMovePhase.h"
#include "Timer.h"


class End_ToUp_Side final : public BaseMove
{
public:
    End_ToUp_Side();
    ~End_ToUp_Side()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void MoveFront(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle);
    void SpeedRatio(float elapsedTime, const float& now_time);
    void LerpRotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void RotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void LerpRotateX(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void LerpPositionX(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position);

private:
    Timer timer;
    float speed;
};



class End_ToDown_Side final : public BaseMove
{
public:
    End_ToDown_Side();
    ~End_ToDown_Side()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void MoveFront(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle);
    void SpeedRatio(float elapsedTime, const float& now_time);
    void LerpRotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void RotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void LerpRotateX(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void LerpPositionX(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position);

private:
    Timer timer;
    float speed;
};