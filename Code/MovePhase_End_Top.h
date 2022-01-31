#pragma once

#include "BaseMovePhase.h"
#include "Timer.h"


class End_ToLeft_Top final : public BaseMove
{
public:
    End_ToLeft_Top();
    ~End_ToLeft_Top()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void MoveFront(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle);
    void SpeedRatio(float elapsedTime, const float& now_time);
    void LerpRotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void RotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void LerpRotateY(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void LerpPositionY(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position);

private:
    Timer timer;
    float speed;
};



class End_ToRight_Top final : public BaseMove
{
public:
    End_ToRight_Top();
    ~End_ToRight_Top()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void MoveFront(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle);
    void SpeedRatio(float elapsedTime, const float& now_time);
    void LerpRotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void RotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void LerpRotateY(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void LerpPositionY(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position);

private:
    Timer timer;
    float speed;
};