#pragma once

#include "Enemy.h"
#include "BaseMovePhase.h"
#include "Timer.h"


class EnemyStraight_None final : public Enemy
{
public:
    EnemyStraight_None(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;

private:
    std::unique_ptr<MoveState> move_state;
};



class EnemyStraight_None_Start_Back final : public Enemy
{
public:
    EnemyStraight_None_Start_Back(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

    enum STATE
    {
        mStart,
        mRush,
    };
};



class EnemyStraight_None_Start_Front final : public Enemy
{
public:
    EnemyStraight_None_Start_Front(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

    enum STATE
    {
        mStart,
        mRush,
    };
};



class EnemyStraight_None_Start_Side final : public Enemy
{
public:
    EnemyStraight_None_Start_Side(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

};




// ↑縦シュー限定の敵、　横シュー非対応　
class EnemyStraight_None_Horizon_Top final : public Enemy
{
public:
    EnemyStraight_None_Horizon_Top(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;

private:
    void RotateZ(float elapsedTime);

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

};



// ↑縦シュー限定の敵、　横シュー非対応　
class EnemyStraight_None_Bezier_Top final : public Enemy
{
public:
    EnemyStraight_None_Bezier_Top(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

};





// →横シュー限定の敵、　縦シュー非対応　
class EnemyStraight_None_Bezier_Side final : public Enemy
{
public:
    EnemyStraight_None_Bezier_Side(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

};





/////////////////// 射撃 ///////////////////


class EnemyStraight_Shot_Start_Back final : public Enemy
{
public:
    EnemyStraight_Shot_Start_Back(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsdTime);
private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

    enum STATE
    {
        mStart = 1,
        mRush = 0,
    };
};



class EnemyStraight_Shot_Start_Front final : public Enemy
{
public:
    EnemyStraight_Shot_Start_Front(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsdTime);

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

    enum STATE
    {
        mStart = 1,
        mRush = 0,
    };
};



class EnemyStraight_Shot_Start_Side final : public Enemy
{
public:
    EnemyStraight_Shot_Start_Side(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsdTime);

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

    enum STATE
    {
        mStart,
        mIdle,
        mRush,
    };

};







// ↑縦シュー限定の敵、　横シュー非対応　
class EnemyStraight_Shot_Bezier_Top final : public Enemy
{
public:
    EnemyStraight_Shot_Bezier_Top(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsdTime);

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

};


// →横シュー限定の敵、　縦シュー非対応　
class EnemyStraight_Shot_Bezier_Side final : public Enemy
{
public:
    EnemyStraight_Shot_Bezier_Side(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsdTime);

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

};



class EnemyStraight_TargetShot_Horizon final : public Enemy
{
public:
    EnemyStraight_TargetShot_Horizon(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsdTime);

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;


    const DirectX::XMFLOAT3* target;
};






class EnemyStraight_TargetShot_Start_Back final : public Enemy
{
public:
    EnemyStraight_TargetShot_Start_Back(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target, short shot_max);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsdTime);
private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

    const DirectX::XMFLOAT3* target;

    enum STATE
    {
        mStart = 1,
        mRush = 0,
    };

    short shot_count = 0;
    short shot_max = 0;
};


class EnemyStraight_TargetShot_Start_Front final : public Enemy
{
public:
    EnemyStraight_TargetShot_Start_Front(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, short shot_max);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsdTime);

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

    const DirectX::XMFLOAT3* target;

    enum STATE
    {
        mStart = 1,
        mRush = 0,
    };

    short shot_count = 0;
    short shot_max = 0;
};


class EnemyStraight_TargetShot_Start_Side final : public Enemy
{
public:
    EnemyStraight_TargetShot_Start_Side(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, short shot_max);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsdTime);

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

    const DirectX::XMFLOAT3* target;

    enum STATE
    {
        mStart = 1,
        mIdle = 0,
        mRush = 2,
    };

    short shot_count = 0;
    short shot_max = 0;

};




// ↑縦シュー限定の敵、　横シュー非対応　
class EnemyStraight_TargetShot_Bezier_Top final : public Enemy
{
public:
    EnemyStraight_TargetShot_Bezier_Top(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, short shot_max);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsdTime);

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

    const DirectX::XMFLOAT3* target;

    short shot_count = 0;
    short shot_max = 0;

};


// →横シュー限定の敵、　縦シュー非対応　
class EnemyStraight_TargetShot_Bezier_Side final : public Enemy
{
public:
    EnemyStraight_TargetShot_Bezier_Side(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, short shot_max);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    void Shot(float elapsdTime);

private:
    std::unique_ptr<MoveState> move_state;

    Timer timer;

    const DirectX::XMFLOAT3* target;

    short shot_count = 0;
    short shot_max = 0;

};