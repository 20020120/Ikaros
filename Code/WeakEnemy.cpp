

#include "WeakEnemy.h"

#include "BaseProjectile.h"
#include "Bullet.h"
#include "Energy.h"
#include "MovePhase_Start_Side.h"
#include "MovePhase_Start_Top.h"
#include "MovePhase_Idle.h"
#include "MovePhase_Rotate.h"
#include "MovePhase_Rush.h"

#include "GameSystem.h"
#include "MovePhase_End_Side.h"
#include "MovePhase_End_Top.h"
#include "ProjectileManager.h"
#include "SceneManager.h"

//EnemyStraightFromBackToFront::EnemyStraightFromBackToFront(ID3D11Device* Device, const Type& type_) : Enemy(
//    Device,
//    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
//    EnemyPSFilePath::Lambert
//)
//{
//    // 属性の設定
//    type = type_;
//
//    // 関数ポインタの設定
//    MovePhase[0] = &EnemyStraightFromBackToFront::MoveToFrontPhase;
//    MovePhase[1] = &EnemyStraightFromBackToFront::MoveRotatePhase;
//    MovePhase[2] = &EnemyStraightFromBackToFront::MoveToBackPhase;
//    move_state = 0;
//
//    // 速度の設定
//    status.speed = 26.0f;
//
//    // 初期姿勢の設定
//    r.Angle = {};
//}
//
//void EnemyStraightFromBackToFront::Initialize()
//{
//    t.Position.z = -50.0f;
//}
//
//
//
//void EnemyStraightFromBackToFront::Move(float elapsedTime)
//{
//    (this->*MovePhase[move_state])(elapsedTime);
//
//    if(move_state > 1)
//        Shot(elapsedTime);
//}
//
//
//void EnemyStraightFromBackToFront::MoveToFrontPhase(float elapsedTime)
//{
//    // 前方向(モデルのz軸方向)に進む処理
//    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z) };
//    DirectX::XMFLOAT4X4 rotate;
//    DirectX::XMStoreFloat4x4(&rotate, R);
//    DirectX::XMFLOAT3 forward = { rotate._31,rotate._32,rotate._33 };
//    DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forward));
//    DirectX::XMStoreFloat3(&forward, Forward);
//
//    // yとzのみ位置を更新
//    t.Position.y += forward.y * status.speed * elapsedTime;
//    t.Position.z += forward.z * status.speed * elapsedTime;
//
//
//    // 次のフェーズに行く条件
//    if (t.Position.z < turn_z_axis_size) return;
//
//    move_state++;
//}
//
//
//void EnemyStraightFromBackToFront::MoveRotatePhase(float elapsedTime)
//{
//    constexpr float half_pi = DirectX::XMConvertToRadians(180);
//
//    r.Angle.y += Calcu3D::LerpFloat(0.0f, half_pi, 0.5f * elapsedTime);
//
//
//
//    constexpr float epsilon_radian = DirectX::XMConvertToRadians(5);
//
//    if (r.Angle.y < half_pi + epsilon_radian) return;
//
//    r.Angle.y = half_pi;
//    status.speed = 8.0f;
//    move_state++;
//}
//
//
//void EnemyStraightFromBackToFront::MoveToBackPhase(float elapsedTime)
//{
//    // 前方向(モデルのz軸方向)に進む処理
//    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z) };
//    DirectX::XMFLOAT4X4 rotate;
//    DirectX::XMStoreFloat4x4(&rotate, R);
//    DirectX::XMFLOAT3 forward = { rotate._31,rotate._32,rotate._33 };
//    DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forward));
//    DirectX::XMStoreFloat3(&forward, Forward);
//
//    // yとzのみ位置を更新
//    t.Position.y += forward.y * status.speed * elapsedTime;
//    t.Position.z += forward.z * status.speed * elapsedTime;
//}
//
//
//
//EnemyStraightToTarget::EnemyStraightToTarget(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_) : Enemy(
//    Device,
//    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
//    EnemyPSFilePath::Lambert
//)
//{
//    // 属性の設定
//    type = type_;
//    // 目標地点の設定(ポインタ)
//    target = target_;
//
//    // 関数ポインタの設定
//    MovePhase[0] = &EnemyStraightToTarget::MoveFromTopPhase;
//    MovePhase[1] = &EnemyStraightToTarget::MoveRotateToTargetPhase;
//    MovePhase[2] = &EnemyStraightToTarget::MoveToTargetPhase;
//    move_state = 0;
//
//    // 初期速度の設定
//    status.speed = 7.0f;
//}
//
//
//void EnemyStraightToTarget::Initialize()
//{
//    t.Position.y = 30.0f;
//    t.Position.z = 30.0f;
//}
//
//
//void EnemyStraightToTarget::Move(float elapsedTime)
//{
//    (this->*MovePhase[move_state])(elapsedTime);
//
//    if (move_state > 0)
//        Shot(elapsedTime);
//}
//
//
//void EnemyStraightToTarget::MoveFromTopPhase(float elapsedTime)
//{
//    // 上部から下に移動
//
//    constexpr DirectX::XMFLOAT3 direction = { 0.0f,-1.0f,0.0f };
//
//    // yのみ位置を更新
//    t.Position.y += direction.y * status.speed * elapsedTime;
//
//    // 一定まで移動したら次のフェーズへ
//    if (t.Position.y > 20.0f) return;
//    t.Position.y = 20.0f;
//
//    move_state++;
//}
//
//
//void EnemyStraightToTarget::MoveRotateToTargetPhase(float elapsedTime)
//{
//    // 目標までの正規化したベクトル
//    DirectX::XMVECTOR to_target_direction = DirectX::XMVector3Normalize(Calcu3D::MakeVectorAtoB(t.Position, *target));
//
//    // 正規化した正面方向のベクトル
//    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z);
//    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
//    DirectX::XMVECTOR up = DirectX::XMVector3Normalize(M.r[1]);
//
//    // 正規化された二つのベクトルの内積を逆コサインする -> 角度が求まる
//    const float angle = std::acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, front)));
//
//    // 内積で左右判定 (見た目は上下)
//    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, up));
//    constexpr float ratio = 0.98f;
//    r.Angle.x += dot > 0.0f ? Calcu3D::LerpFloat(0.0f, -angle, ratio * elapsedTime) : Calcu3D::LerpFloat(0.0f, angle, ratio * elapsedTime);
//
//
//    // 向いているベクトルと目標までのベクトルが１０度以下なら次のフェーズへ
//    constexpr float epsilon_radian = DirectX::XMConvertToRadians(1.0f);
//    if (angle > 0.0f + epsilon_radian) return;
//    move_state++;
//}
//
//
//void EnemyStraightToTarget::MoveToTargetPhase(float elapsedTime)
//{
//    // 前方向(モデルのz軸方向)に進む処理
//    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z) };
//    DirectX::XMFLOAT4X4 rotate;
//    DirectX::XMStoreFloat4x4(&rotate, R);
//    DirectX::XMFLOAT3 forward = { rotate._31,rotate._32,rotate._33 };
//    DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forward));
//    DirectX::XMStoreFloat3(&forward, Forward);
//
//    // yとzのみ位置を更新
//    t.Position.y += forward.y * status.speed * elapsedTime;
//    t.Position.z += forward.z * status.speed * elapsedTime;
//
//
//    //if (t.Position.y > -26.0f && t.Position.y < 26.0f) return;
//    //
//    //move_state = 1;
//}




EnemyBack_Front_Straight_None::EnemyBack_Front_Straight_None(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 8.0f;

    // 右向きにする
    r.Angle.y = 0.0f;
}

void EnemyBack_Front_Straight_None::Initialize()
{
    t.Position.z = -60.0f;

    move_state = std::make_unique<MoveState>();
    BaseMove* move_front = new RushFront(status.speed, 1.0f);
    BaseMove* rotate_inverse_Y = new RotateInverseY;

    move_state->SetMove(move_front)
        .AddTransition(move_front, rotate_inverse_Y, [&](float) {return (t.Position.z >= turn_z_axis_size); })
        .SetFinalize(rotate_inverse_Y, [&]() {r.Angle.y = 3.14f; })
        .AddTransition(rotate_inverse_Y, move_front, [&](float) {return (r.Angle.y > 3.14f + 0.01f); });

}

void EnemyBack_Front_Straight_None::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);
}



Test_Enemy::Test_Enemy(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 8.0f;

    // 右向きにする
    //r.Angle.y = 0.0f;

    //r.Angle.y = DirectX::XMConvertToRadians(-180.0f);
}


void Test_Enemy::Initialize()
{
    //t.Position.y = -13.0f;
    t.Position.x = -13.0f;
    t.Position.y = -30.0f;
    t.Position.z = 25.0f;
    //t.Position.z = -60.0f;

    int randmm = 0;

    move_state = std::make_unique<MoveState>();
    BaseMove* start_side = new Start_FromYDown_Side(t.Position);
    BaseMove* idle = new Idle(3,&randmm,1);
    BaseMove* end_side = new End_ToDown_Side();
    //BaseMove* end_side = new End_ToRight_Top();

    //move_state->SetMove(start_side).AddTransition(start_side, idle, [&](float) {return (t.Position.z >= turn_z_axis_size); });
    //move_state->SetMove(start_side).AddTransition(start_side, idle, [&](float) {return (t.Position.x >= 0.0f); })
    //    .AddTransition(idle, end_side, [&](float) {return true; });
    move_state->SetMove(start_side)
        .AddTransition(start_side, idle, [&](float) {return (t.Position.x >= 0.0f); })
        .AddTransition(idle, end_side, [&](float) {return true; });


}


void Test_Enemy::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);
}






EnemyRotate_AroundShot_E::EnemyRotate_AroundShot_E(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Hitode : EnemyModelFilePath::Red_Hitode,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 射撃レート
    status.shotInterval = 0.5f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Hitode;

    // スケールの設定
    t.Scale = EnemyKindsScale::Hitode;

    // 出現演出中か
    is_appear = true;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Hitode::AnimationName::no_spin,true);
}

void EnemyRotate_AroundShot_E::Initialize()
{
    // 射撃回数をリセット
    shot_count = 0;


    // イベントフラグを読み込み
    Flags.Load("./resources/Models/Characters/Enemies/hitode.flg");


    // 時間フラグ
    constexpr float seconds = 9.0f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);


    move_state = std::make_unique<MoveState>();
    BaseMove* idle = new RushFront(status.speed, 5.0f);


    // カメラの向きの取得
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    BaseMove* start_back = nullptr;
    BaseMove* rotate = new Idle(1.0f,nullptr,1);
    BaseMove* end = nullptr;
    BaseMove* death = new Death();

    // 回転をしつづける時間
    constexpr float rotate_seconds = 5.41f;

    // カメラの向きに合わせて、射撃方法と出現行動の設定をする
    if (camera_direction == c_d::SIDE)
    {
        SelectedShot = &EnemyRotate_AroundShot_E::AroundShot_Side;

        if(t.Position.y > 0.0f)
        {
            t.Position.x = -13.0f;
            t.Position.y += -55.0f;
            t.Position.z = 30.0f;

            start_back = new Start_FromYDown_RotateX_Side(t.Position);
            end = new End_ToDown_Side();
        }
        else
        {
            t.Position.x = -13.0f;
            t.Position.y += 55.0f;
            t.Position.z = 30.0f;

            start_back = new Start_FromYUp_RotateX_Side(t.Position);
            end = new End_ToUp_Side();
        }
        //rotate = new RotateX(rotate_seconds);
        //rotate = new RotateZ(rotate_seconds);


        // 遷移条件の理由上このif文内で始めの行動遷移を設定する
        move_state->AddTransition(start_back, idle, [&](float) {return t.Position.x > -0.1f; })
            .SetInitialize(idle, [&]()
                {
                    t.Position.x = 0.0f;
                    timer.Reset();
                    r.Angle.x = 0.0f;
                    r.Angle.y = -3.14f;
                    is_appear = false;
                    Model->f_PlayAnimation(EnemyAnim::Hitode::AnimationName::spin_begin);
                });


    }

    else if (camera_direction == c_d::TOP)
    {
        SelectedShot = &EnemyRotate_AroundShot_E::AroundShot_Top;

        if(t.Position.x > 0.0f)
        {
            t.Position.x += -90.0f;
            t.Position.y = -13.0f;
            t.Position.z = 25.0f;

            start_back = new Start_FromLeft_RotateY_Top(t.Position);
            end = new End_ToLeft_Top();
        }
        else
        {
            t.Position.x += 90.0f;
            t.Position.y = -13.0f;
            t.Position.z = 25.0f;

            start_back = new Start_FromRight_RotateY_Top(t.Position);
            end = new End_ToRight_Top();
        }
        //rotate = new RotateY(rotate_seconds);
        //rotate = new RotateZ(rotate_seconds);


        // 遷移条件の理由上このif文内で始めの行動遷移を設定する
        move_state->AddTransition(start_back, idle, [&](float) {return t.Position.y > -0.1f; })
            .SetInitialize(idle, [&]()
                {
                    t.Position.y = 0.0f;
                    timer.Reset();
                    r.Angle.x = 0.0f;
                    r.Angle.y = -3.14f;
                    is_appear = false;
                    Model->f_PlayAnimation(EnemyAnim::Hitode::AnimationName::spin_begin);
                });


    }


    // 遷移条件を上のif文内でいくつか先に設定してある
    move_state->AddTransition(idle, rotate, [&](float) {return timer.NowTime() <= 9.0f/*seconds*/ - 1.5f; })
                .SetInitialize(rotate, [&]()
                    {
                        timer.Reset();
                    }
                )
        .AddTransition(rotate, end, [&](float) {return (timer.NowTime() <= 9.0f/*seconds*/ - 5.41f/*rotate_seconds*/ - 0.8f); })
        .SetFinalize(rotate, [&]() {timer.Reset(); is_appear = true; })
        .AddTransition(end, death, [&](float) {return (timer.NowTime() <= 9.0f - 3.0f); })
        .SetInitialize(death, [&]() {Destroy(); });


    move_state->SetMove(start_back);
}

void EnemyRotate_AroundShot_E::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);



    if (Flags.Check("stop", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(EnemyAnim::Hitode::AnimationName::no_spin,true);
    }
    
    if (timer.NowTime() <= 9.0f - 5.41f - 0.8f + 2.5f && Flags.Check("end", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(EnemyAnim::Hitode::AnimationName::spin_end);
    }

    if (Flags.Check("start", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(EnemyAnim::Hitode::AnimationName::spin_loop, true);
    }
}

void EnemyRotate_AroundShot_E::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    invTimer -= elapsedTime;


    Shot(elapsedTime);
}

void EnemyRotate_AroundShot_E::Shot(float elapsedTime)
{
    // クールダウン
    if (CanShot(elapsedTime) == false) return;


    if (timer.NowTime() <= 9.0f/*seconds*/ - 5.41f/*rotate_seconds*/ + 1.5f)return;
    if (move_state->GetNowMove() != STATE::sRotate) return;

    //弾発射
    (this->*SelectedShot)(elapsedTime);
}

void EnemyRotate_AroundShot_E::EnergyRegister(ID3D11Device* p_device, DirectX::XMFLOAT3 direction)
{
    if (type == BaseObject::Type::BLUE)
    {
        ProjectileManager::Instance().RegisterProjectile(new Energy(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, direction));
    }
    else if (type == BaseObject::Type::RED)
    {
        ProjectileManager::Instance().RegisterProjectile(new Energy(p_device, BaseProjectile::Parent::REDENEMY,
            t.Position, direction));
    }
}

void EnemyRotate_AroundShot_E::AroundShot_Side(float elapsedTime)
{
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();

    DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();                  // 単位行列
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(transform.r[2]);
    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(transform.r[0]);

    DirectX::XMFLOAT3 direction;
    DirectX::XMStoreFloat3(&direction, front);

    // 角度72度の変数
    constexpr float radian = DirectX::XMConvertToRadians(360.0f / 5.0f);

    // ずらし用の角度15度
    constexpr float shift_radian = DirectX::XMConvertToRadians(15.0f);

    for (short i = 0; i < 5; ++i)
    {
        // 前方向ベクトルをradian分x軸回転させる
        DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(right, radian * static_cast<float>(i) + shift_radian * static_cast<float>(shot_count))));

        // 前の斜めちょい上方向
        EnergyRegister(p_device, direction);
    }


    // 射撃回数を増加
    shot_count++;

}

void EnemyRotate_AroundShot_E::AroundShot_Top(float elapsedTime)
{
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();

    DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();                  // 単位行列
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(transform.r[2]);
    DirectX::XMVECTOR up = DirectX::XMVector3Normalize(transform.r[1]);

    DirectX::XMFLOAT3 direction;
    DirectX::XMStoreFloat3(&direction, front);

    // 角度72度の変数
    constexpr float radian = DirectX::XMConvertToRadians(360.0f / 5.0f);

    // ずらし用の角度15度
    constexpr float shift_radian = DirectX::XMConvertToRadians(15.0f);


    for(short i = 0; i < 5; ++i)
    {
        // 前方向ベクトルをradian分y軸回転させる
        DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(up, radian * static_cast<float>(i) + shift_radian * static_cast<float>(shot_count))));

        // 前の斜めちょい上方向
        EnergyRegister(p_device, direction);
    }


    // 射撃回数を増加
    shot_count++;

}




EnemyMove_Round_TargetShot_Side::EnemyMove_Round_TargetShot_Side(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, bool hard_mode) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 目標の設定
    target = target_;

    // 射撃レート
    status.shotInterval = 1.5f;

    // 速度の設定
    status.speed = 25.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // スケールの設定
    t.Scale = EnemyKindsScale::Move_Shark;

    // 弾の速度
    bullet_speed = (hard_mode) ? 35.0f : 15.0f;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
}


void EnemyMove_Round_TargetShot_Side::Initialize()
{
    if (t.Position.y < 0.0f)
    {
        t.Position.y = -23.0f;
    }
    else
    {
        t.Position.y = 23.0f;
    }

    // 行動遷移の設定
    move_state = std::make_unique<MoveState>();

    BaseMove* move_round = new RushRound(t.Position.y, status.speed);

    move_state->SetMove(move_round);
}

void EnemyMove_Round_TargetShot_Side::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);
}

void EnemyMove_Round_TargetShot_Side::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //無敵時間更新
    invTimer -= elapsedTime;

    Shot(elapsedTime);
}

void EnemyMove_Round_TargetShot_Side::Shot(float elapsedTime)
{
    if (CanShot(elapsedTime) == false)return;


    if (type == Type::NONE) return;

    // 弾の進行方向
    DirectX::XMVECTOR Direction = Calcu3D::MakeVectorAtoB(t.Position, *target, true);
    DirectX::XMFLOAT3 direction;
    DirectX::XMStoreFloat3(&direction, Direction);


    // 弾の生成
    const BaseProjectile::Parent parent_type = (type == Type::RED) ? BaseProjectile::Parent::REDENEMY : BaseProjectile::Parent::BLUEENEMY;

    BaseProjectile* bullet = new Bullet(SceneManager::Instance().GetDevice(),
        parent_type,
        t.Position,
        direction,
        bullet_speed);

    ProjectileManager::Instance().RegisterProjectile(bullet);
}
