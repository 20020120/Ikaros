#pragma once

#include "Enemy.h"
#include "Timer.h"
#include "BaseMovePhase.h"


class Boss_1 final : public Enemy
{
public:
    Boss_1(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_ = {});
    ~Boss_1()override = default;

public:
    void Initialize() override;

public:
    void Behavior(float elapsedTime) override;
    void Move(float elapsedTime) override;
    void Attack(float elapsedTime);

    void Collision() override;

private:
    bool IsInOriginArea();
    bool IsInScreenArea();
    bool IsLimitOverY();
    bool IsLimitOverZ();

private:
    // UŒ‚
    void SingleShot(float elapsedTime) const;
    void TwinShot(float elapsedTime) const;
    void TripleShot(float elapsedTime) const;

private: // ’è”‚Ì’è‹`
    static constexpr uint64_t max_rush_count = 3;

private: // •Ï”‚Ì’è‹`
    const DirectX::XMFLOAT3* target;
    float speed_ratio;

    std::unique_ptr<MoveState> move_phase;
    int random_min_1;
    int rush_count;
    float* p_angle = nullptr;

    enum MOVE
    {
        mReturnOriginPosition       = 0x01 << 0,
        mIdle                       = 0x01 << 1,
        mReturnTeleport             = 0x01 << 2,
        mRotateToUpLerp             = 0x01 << 3,
        mRotateToDownLerp           = 0x01 << 4,
        mRotateToTargetLerp         = 0x01 << 5,
        mShakeBody                  = 0x01 << 6,
        mRushFront                  = 0x01 << 7,
        mRushTarget                 = 0x01 << 8,
        mNextPositionRushHorizon    = 0x01 << 9,
        mNextPositionRushVertical   = 0x01 << 10,
        mRotateToTarget             = 0x01 << 11,
    };
};