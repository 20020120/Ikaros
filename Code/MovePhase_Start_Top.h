#pragma once

#include "BaseMovePhase.h"
#include "Timer.h"


class Start_FromBack_Top final : public BaseMove
{
public:
    Start_FromBack_Top(XMFLOAT3& scale_, XMFLOAT3 target_scale_);
    ~Start_FromBack_Top()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void MoveFront(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle);
    void SpeedRatio(float elapsedTime, const float& now_time);
    void RotationX180(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void LerpPositionY(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position);
    void RotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void ScaleUp(float elapsedTime, const float& now_time);

private:
    Timer timer;
    float speed;
    XMFLOAT3& scale;
    const XMFLOAT3 target_scale;
};



class Start_FromFront_Top final : public BaseMove
{
public:
    Start_FromFront_Top(XMFLOAT3& scale_, XMFLOAT3 target_scale_);
    ~Start_FromFront_Top()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void MoveFront(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle);
    void SpeedRatio(float elapsedTime, const float& now_time);
    void RotationX180(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void LerpPositionY(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position);
    void RotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle);
    void ScaleUp(float elapsedTime, const float& now_time);

private:
    Timer timer;
    float speed;
    XMFLOAT3& scale;
    const XMFLOAT3 target_scale;
};



class Start_FromLeft_Top final : public BaseMove
{
public:
    Start_FromLeft_Top(XMFLOAT3 start);
    ~Start_FromLeft_Top()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void LerpPositionY(float elapsedTime, DirectX::XMFLOAT3& position);

private:
    Timer timer;
    static constexpr float all_time = 1.5f;
    std::vector<XMFLOAT3> control_point;

};



class Start_FromRight_Top final : public BaseMove
{
public:
    Start_FromRight_Top(XMFLOAT3 start);
    ~Start_FromRight_Top()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void LerpPositionY(float elapsedTime, DirectX::XMFLOAT3& position);

private:
    Timer timer;
    static constexpr float all_time = 1.5f;
    std::vector<XMFLOAT3> control_point;

};







class Start_FromLeft_RotateY_Top final : public BaseMove
{
public:
    Start_FromLeft_RotateY_Top(XMFLOAT3 start);
    ~Start_FromLeft_RotateY_Top()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void LerpPositionY(float elapsedTime, DirectX::XMFLOAT3& position);
    void LerpRotateY360(const float ratio, DirectX::XMFLOAT3& angle);

private:
    Timer timer;
    static constexpr float all_time = 1.5f;
    std::vector<XMFLOAT3> control_point;

};



class Start_FromRight_RotateY_Top final : public BaseMove
{
public:
    Start_FromRight_RotateY_Top(XMFLOAT3 start);
    ~Start_FromRight_RotateY_Top()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void LerpPositionY(float elapsedTime, DirectX::XMFLOAT3& position);
    void LerpRotateY360(const float ratio, DirectX::XMFLOAT3& angle);

private:
    Timer timer;
    static constexpr float all_time = 1.5f;
    std::vector<XMFLOAT3> control_point;

};