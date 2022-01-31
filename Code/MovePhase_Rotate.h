#pragma once

#include "BaseMovePhase.h"
#include "Timer.h"


class RotateXToTarget final : public BaseMove
{
public:
    RotateXToTarget(const DirectX::XMFLOAT3* target, int* rush_count) :p_target(target),p_rush_count(rush_count) {};
    ~RotateXToTarget()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    const DirectX::XMFLOAT3* p_target = nullptr;
    int* p_rush_count = nullptr;
};



class RotateYToTarget final : public BaseMove
{
public:
    RotateYToTarget(const DirectX::XMFLOAT3* target, int* rush_count) :p_target(target), p_rush_count(rush_count) {};
    ~RotateYToTarget()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    const DirectX::XMFLOAT3* p_target = nullptr;
    int* p_rush_count = nullptr;
};



class RotateXToTargetLerp final : public BaseMove
{
public:
    RotateXToTargetLerp(const DirectX::XMFLOAT3* target,const float ratio) :p_target(target),ratio(ratio) {};
    ~RotateXToTargetLerp()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

public:
    float& GetAngle() { return angle_; }

private:
    const DirectX::XMFLOAT3* p_target = nullptr;
    const float ratio;
    float angle_ = 3.14f;
};



class RotateYToTargetLerp final : public BaseMove
{
public:
    RotateYToTargetLerp(const DirectX::XMFLOAT3* target, const float ratio) :p_target(target), ratio(ratio) {};
    ~RotateYToTargetLerp()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

public:
    float& GetAngle() { return angle_; }

private:
    const DirectX::XMFLOAT3* p_target = nullptr;
    const float ratio;
    float angle_ = 3.14f;
};




class RotateXToTargetLerp_LimitTimer final : public BaseMove
{
public:
    RotateXToTargetLerp_LimitTimer(const DirectX::XMFLOAT3* target, const float time) :p_target(target), limit_time(time)
    {
        timer.Initialize(nullptr, COUNT::DOWN, time);
    };
    ~RotateXToTargetLerp_LimitTimer()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

public:
    float& GetAngle() { return angle_; }

private:
    const DirectX::XMFLOAT3* p_target = nullptr;
    Timer timer;
    const float limit_time;
    float angle_ = 3.14f;
};


class RotateYToTargetLerp_LimitTimer final : public BaseMove
{
public:
    RotateYToTargetLerp_LimitTimer(const DirectX::XMFLOAT3* target, const float time) :p_target(target), limit_time(time)
    {
        timer.Initialize(nullptr, COUNT::DOWN, time);
    };
    ~RotateYToTargetLerp_LimitTimer()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

public:
    float& GetAngle() { return angle_; }

private:
    const DirectX::XMFLOAT3* p_target = nullptr;
    Timer timer;
    const float limit_time;
    float angle_ = 3.14f;
};





class RotateX final : public BaseMove
{
public:
    RotateX(const float time) :all_time(time), half_time(time * 0.5f)
    {
        timer.Initialize(nullptr, COUNT::DOWN, time);
        angle_speed = 0.0f;
    }
    ~RotateX()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    Timer timer;
    const float all_time;
    const float half_time;
    float angle_speed;
};


class RotateY final : public BaseMove
{
public:
    RotateY(const float time) :all_time(time), half_time(time * 0.5f)
    {
        timer.Initialize(nullptr, COUNT::DOWN, time);
        angle_speed = 0.0f;
    }
    ~RotateY()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    Timer timer;
    const float all_time;
    const float half_time;
    float angle_speed;
};


class RotateZ final : public BaseMove
{
public:
    RotateZ(const float time) :all_time(time), half_time(time * 0.5f)
    {
        timer.Initialize(nullptr, COUNT::DOWN, time);
        angle_speed = 0.0f;
    }
    ~RotateZ()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    Timer timer;
    const float all_time;
    const float half_time;
    float angle_speed;
};







class RotateToUpLerp final : public BaseMove
{
public:
    RotateToUpLerp() = default;
    ~RotateToUpLerp()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;
};


class RotateToDownLerp final : public BaseMove
{
public:
    RotateToDownLerp() = default;
    ~RotateToDownLerp()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;
};


class RotateInverseY final : public BaseMove
{
public:
    RotateInverseY() = default;
    ~RotateInverseY()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    static constexpr float half_pi = DirectX::XMConvertToRadians(180);
};