#pragma once

#include "Enemy.h"
#include "BaseMovePhase.h"
#include "Timer.h"

//class EnemyStraightFromBackToFront final : public Enemy
//{
//public:
//    EnemyStraightFromBackToFront(ID3D11Device* Device, const Type& type_);
//    void Initialize() override;
//    void Move(float elapsedTime) override;
//
//private:
//    /* 関数ポインタ */
//    using pFunc = void(EnemyStraightFromBackToFront::*)(float);
//    pFunc MovePhase[3] = {};
//
//    void MoveToFrontPhase(float elapsedTime);
//    void MoveRotatePhase(float elapsedTime);
//    void MoveToBackPhase(float elapsedTime);
//    
//    int move_state;
//};
//
//class EnemyStraightToTarget final : public Enemy
//{
//public:
//    EnemyStraightToTarget(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_ = {});
//    void Initialize() override;
//    void Move(float elapsedTime) override;
//
//private:
//    /* 関数ポインタ */
//    using pFunc = void(EnemyStraightToTarget::*)(float);
//    pFunc MovePhase[3] = {};
//
//    void MoveFromTopPhase(float elapsedTime);
//    void MoveRotateToTargetPhase(float elapsedTime);
//    void MoveToTargetPhase(float elapsedTime);
//
//    int move_state;
//
//private: // 変数の定義
//    const DirectX::XMFLOAT3* target;
//};



class EnemyBack_Front_Straight_None final : public Enemy
{
public:
    EnemyBack_Front_Straight_None(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;

private:
    std::unique_ptr<MoveState> move_state;
};


class Test_Enemy final : public Enemy
{
public:
    Test_Enemy(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;

private:
    std::unique_ptr<MoveState> move_state;
};


class Enemy_None final : public Enemy
{
public:
    Enemy_None(ID3D11Device* Device, const Type& type_) : Enemy(
        Device,
        type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
        EnemyPSFilePath::Lambert
    )
    {
        type = type_;
    };
    void Initialize() override {};
    void Move(float elapsedTime) override
    {
        status.hitPoint = 0.0f;
    };
    void Collision() override{};
};


class EnemyRotate_AroundShot_E final : public Enemy
{
public:
    EnemyRotate_AroundShot_E(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsedTime);
    void EnergyRegister(ID3D11Device* p_device, DirectX::XMFLOAT3 direction);

private:
    using pFunc = void(EnemyRotate_AroundShot_E::*)(float);
    pFunc SelectedShot;
    void AroundShot_Side(float elapsedTime);
    void AroundShot_Top(float elapsedTime);

private:
    std::unique_ptr<MoveState> move_state;
    Timer timer;
    short shot_count;
    enum STATE
    {
        sStart,
        sIdle,
        sRotate,
        sEnd,
        sDeath
    };
};


class EnemyMove_Round_TargetShot_Side final : public Enemy
{
public:
    EnemyMove_Round_TargetShot_Side(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, bool hard_mode = false);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsedTime);

private:
    std::unique_ptr<MoveState> move_state;
    float bullet_speed = 0.0f;

    const DirectX::XMFLOAT3* target;
};