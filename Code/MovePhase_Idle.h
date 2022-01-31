#pragma once

#include "BaseMovePhase.h"
#include "Timer.h"


class Idle final : public BaseMove
{
public:
    Idle(const float time, int* random_min_1,const int range) :time(time),p_random_min_1(random_min_1),range(range){};
    ~Idle()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    bool Is(const float elapsedTime);

private:
    bool inited = false;
    Timer timer;
    const float time;
    const int range;
    int* p_random_min_1;
};



class ShakeBody final : public BaseMove
{
public:
    ShakeBody() = default;
    ~ShakeBody()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;
};



class ReturnOriginPosition final : public BaseMove
{
public:
    ReturnOriginPosition(const DirectX::XMFLOAT3& origin_position, const float lerp_ratio = 0.85f) : origin_position(origin_position), lerp_ratio(lerp_ratio){};
    ~ReturnOriginPosition()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    const DirectX::XMFLOAT3 origin_position;
    const float lerp_ratio;
};



class ReturnTeleport final : public BaseMove
{
public:
    ReturnTeleport(const DirectX::XMFLOAT3& origin_position) : origin_position(origin_position) {};
    ~ReturnTeleport()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override;

private:
    const DirectX::XMFLOAT3 origin_position;
};



class Death final : public BaseMove
{
public:
    Death() = default;
    ~Death()override = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) override
    {};

    // 行動遷移させているオブジェクトを削除するフラグ用の行動

    // この遷移に移る条件がそろったときに Initializeで削除予約をする。

};