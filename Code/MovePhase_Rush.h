#pragma once

#include "BaseMovePhase.h"
#include "Timer.h"


class RushFront final : public BaseMove
{
public:
    RushFront(const float speed, const float rush_speed_ratio) : speed(speed), lerp_ratio(rush_speed_ratio), now_ratio(0.0f) {};
    ~RushFront()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    const float speed;
    const float lerp_ratio;
    float now_ratio;

};



class RushZ final : public BaseMove
{
public:
    RushZ(const float speed, const float rush_speed_ratio) : speed(speed), lerp_ratio(rush_speed_ratio), now_ratio(0.0f) {};
    ~RushZ()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    const float speed;
    const float lerp_ratio;
    float now_ratio;

};


class RushRound final : public BaseMove
{
public:
    RushRound(const float pos_y, const float speed) : ope((pos_y < 0.0f) ? -1.0f : 1.0f), speed(speed)
    {
        timer.Initialize(nullptr, COUNT::UP, 0.0f);
    };
    ~RushRound()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    void Rotate(float elapsedTime, DirectX::XMFLOAT3& angle);

private:
    const float ope;
    const float speed;
    Timer timer;

};


class RushTarget final : public BaseMove
{
public:
    RushTarget(const float speed, const float rush_ratio) : speed(speed), rush_speed_ratio(rush_ratio) {};
    ~RushTarget()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    const float speed;
    const float rush_speed_ratio;
};


class RushFront_LimitTimer final : public BaseMove
{
public:
    RushFront_LimitTimer(const float speed, const float time) : speed(speed)
    {
        timer.Initialize(nullptr, COUNT::DOWN, time);
    };
    ~RushFront_LimitTimer()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3 & position, DirectX::XMFLOAT3 & angle) override;

private:
    const float speed;
    Timer timer;
};



class RushBezier_Top final : public BaseMove
{
public:
    RushBezier_Top(DirectX::XMFLOAT3 start);
    ~RushBezier_Top()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    Timer timer;
    std::vector<DirectX::XMFLOAT3> control_point;
    float start_angle_y = 0.0f;
    float end_angle_y = 0.0f;
};



class RushBezier_Side final : public BaseMove
{
public:
    RushBezier_Side(DirectX::XMFLOAT3 start);
    ~RushBezier_Side()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    Timer timer;
    std::vector<DirectX::XMFLOAT3> control_point;
    float start_angle_x = 0.0f;
    float end_angle_x = 0.0f;
};




class RushWaveVertical final : public BaseMove
{
public:
    RushWaveVertical(const float speed, const float rush_ratio) : speed(speed), rush_speed_ratio(rush_ratio) {};
    ~RushWaveVertical()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    const float speed;
    const float rush_speed_ratio;
    float curveAng{};
    float curveSpeed{ 30.0f };
};



class RushWaveHorizon final : public BaseMove
{
public:
    RushWaveHorizon(const float speed, const float rush_ratio) : speed(speed), rush_speed_ratio(rush_ratio) {};
    ~RushWaveHorizon()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    const float speed;
    const float rush_speed_ratio;
    float curveAng{};
    float curveSpeed{ 30.0f };
};



class NextPositionRushHorizon final : public BaseMove
{
public:
    NextPositionRushHorizon(int* rush_count, const DirectX::XMFLOAT3 origin_position) : p_rush_count(rush_count), origin_position(origin_position) {};
    ~NextPositionRushHorizon()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    int* p_rush_count = nullptr;
    const DirectX::XMFLOAT3 origin_position;
};


class NextPositionRushVertical final : public BaseMove
{
public:
    NextPositionRushVertical(int* rush_count) : p_rush_count(rush_count) {};
    ~NextPositionRushVertical()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    int* p_rush_count = nullptr;
};