#pragma once

#include "Enemy.h"
#include "BaseMovePhase.h"
#include "Timer.h"


class EnemyLock_Shot_B final : public Enemy
{
public:
    EnemyLock_Shot_B(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    bool IsInOriginPos(DirectX::XMFLOAT3 origin);

private:
    std::unique_ptr<MoveState> move_state;
    const DirectX::XMFLOAT3* target;
    DirectX::XMFLOAT3 origin_pos;
};


class EnemyLock_Shot_E final : public Enemy
{
public:
    EnemyLock_Shot_E(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    bool IsInOriginPos(DirectX::XMFLOAT3 origin);

private:
    std::unique_ptr<MoveState> move_state;
    const DirectX::XMFLOAT3* target;
    DirectX::XMFLOAT3 origin_pos;
};


class EnemyLock_Straight_Target final : public Enemy
{
public:
    EnemyLock_Straight_Target(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, bool hard_mode_ = false);
    void Initialize() override;
    void Move(float elapsedTime) override;

private:
    bool IsInOriginPos(DirectX::XMFLOAT3 origin);

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;


    const DirectX::XMFLOAT3* target;
    DirectX::XMFLOAT3 origin_pos;
    bool hard_mode = false;
};





class EnemyLock_MultiShot_B final : public Enemy
{
public:
    EnemyLock_MultiShot_B(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    bool IsInOriginPos(DirectX::XMFLOAT3 origin);
    void Shot(float elapsdTime);
    void BulletRegister(ID3D11Device* p_device, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 front);

private:
    using pFunc = void(EnemyLock_MultiShot_B::*)(float);
    pFunc SelectedShot;
    void TripleShot(float elapsedTime);
    void PentaShot(float elapsedTime);

private:
    std::unique_ptr<MoveState> move_state;
    Timer timer;
    DirectX::XMFLOAT3 origin_pos;

    enum STATE
    {
        sReturnOriginPos,
        sIdle,
        sEnd,
        sDeath
    };
};


class EnemyLock_MultiShot_E final : public Enemy
{
public:
    EnemyLock_MultiShot_E(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    bool IsInOriginPos(DirectX::XMFLOAT3 origin);
    void Shot(float elapsdTime);
    void EnergyRegister(ID3D11Device* p_device, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 front);

private:
    using pFunc = void(EnemyLock_MultiShot_E::*)(float);
    pFunc SelectedShot;
    void TripleShot(float elapsedTime);
    void PentaShot(float elapsedTime);

private:
    std::unique_ptr<MoveState> move_state;
    Timer timer;
    DirectX::XMFLOAT3 origin_pos;

    enum STATE
    {
        sReturnOriginPos,
        sIdle,
        sEnd,
        sDeath
    };
};




class EnemyLock_TargetSingleShot final : public Enemy
{
public:
    EnemyLock_TargetSingleShot(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, bool hard_mode = false);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    bool IsInOriginPos(DirectX::XMFLOAT3 origin);
    void Shot(float elapsdTime);

private:
    std::unique_ptr<MoveState> move_state;
    Timer timer;
    DirectX::XMFLOAT3 origin_pos;

    const DirectX::XMFLOAT3* target;

    enum STATE
    {
        sReturnOriginPos,
        sIdle,
        sEnd,
        sDeath
    };

};


class EnemyLock_TargetMultiShot_B final : public Enemy
{
public:
    EnemyLock_TargetMultiShot_B(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, bool hard_mode = false);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    bool IsInOriginPos(DirectX::XMFLOAT3 origin);
    void Shot(float elapsdTime);
    void BulletRegister(ID3D11Device* p_device, DirectX::XMFLOAT3 direction);

private:
    using pFunc = void(EnemyLock_TargetMultiShot_B::*)(float);
    pFunc SelectedShot;
    void TripleShot(float elapsedTime);
    void PentaShot(float elapsedTime);

private:
    std::unique_ptr<MoveState> move_state;
    Timer timer;
    DirectX::XMFLOAT3 origin_pos;

    const DirectX::XMFLOAT3* target;

    enum STATE
    {
        sReturnOriginPos,
        sIdle,
        sEnd,
        sDeath
    };

    // ŽËŒ‚‚Å‚«‚éŽžŠÔ
    const float limit_shot_time = 0.0f;
};



class EnemyTranslate_Shot_B_Top final : public Enemy
{
public:
    EnemyTranslate_Shot_B_Top(ID3D11Device* Device, const Type& type_, bool hard_mode = false);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsedTime);

private:
    std::unique_ptr<MoveState> move_state;
};


class EnemyTranslate_Shot_B_Side final : public Enemy
{
public:
    EnemyTranslate_Shot_B_Side(ID3D11Device* Device, const Type& type_, bool hard_mode = false);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsedTime);

private:
    std::unique_ptr<MoveState> move_state;
};