
#include "WaveEnemy.h"

#include "GameSystem.h"
#include "MovePhase_Rush.h"


EnemyWave_None::EnemyWave_None(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 8.0f;
}

void EnemyWave_None::Initialize()
{
    move_state = std::make_unique<MoveState>();
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();

    BaseMove* move_wave;
    if (camera_direction == c_d::SIDE)
    {
        move_wave = new RushWaveVertical(status.speed, 1.0f);
    }
    else
    {
        move_wave = new RushWaveHorizon(status.speed, 1.0f);
    }

    move_state->SetMove(move_wave);
}

void EnemyWave_None::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);
}



EnemyWave_Shot_B::EnemyWave_Shot_B(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 8.0f;

    // 発射レート
    status.shotInterval = 2.0f;
}

void EnemyWave_Shot_B::Initialize()
{
    move_state = std::make_unique<MoveState>();
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();

    BaseMove* move_wave;
    if (camera_direction == c_d::SIDE)
    {
        move_wave = new RushWaveVertical(status.speed, 1.0f);
    }
    else
    {
        move_wave = new RushWaveHorizon(status.speed, 1.0f);
    }

    move_state->SetMove(move_wave);
}

void EnemyWave_Shot_B::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);
}

void EnemyWave_Shot_B::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    invTimer -= elapsedTime;

    // クールダウン
    if (CanShot(elapsedTime) == false) return;

    //弾発射
    BulletShot(elapsedTime);
}



EnemyWave_Shot_E::EnemyWave_Shot_E(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 8.0f;

    // 発射レート
    status.shotInterval = 2.0f;
}

void EnemyWave_Shot_E::Initialize()
{
    move_state = std::make_unique<MoveState>();
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();

    BaseMove* move_wave;
    if (camera_direction == c_d::SIDE)
    {
        move_wave = new RushWaveVertical(status.speed, 1.0f);
    }
    else
    {
        move_wave = new RushWaveHorizon(status.speed, 1.0f);
    }

    move_state->SetMove(move_wave);
}

void EnemyWave_Shot_E::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);
}

void EnemyWave_Shot_E::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    invTimer -= elapsedTime;

    // クールダウン
    if (CanShot(elapsedTime) == false) return;

    //弾発射
    EnergyShot(elapsedTime);
}