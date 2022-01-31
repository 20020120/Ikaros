
#include "StraightEnemy.h"

#include "GameSystem.h"
#include "MovePhase_Idle.h"
#include "MovePhase_Rush.h"
#include "MovePhase_Start_Side.h"
#include "MovePhase_Start_Top.h"
#include "MathSupport.h"


EnemyStraight_None::EnemyStraight_None(ID3D11Device* Device, const Type& type_) : Enemy(
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

void EnemyStraight_None::Initialize()
{
    move_state = std::make_unique<MoveState>();
    BaseMove* move_front = new RushFront(status.speed, 1.0f);

    move_state->SetMove(move_front);
}

void EnemyStraight_None::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);
}



EnemyStraight_None_Start_Back::EnemyStraight_None_Start_Back(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Ankou : EnemyModelFilePath::Red_Ankou,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Ankou;

    // スケールの設定
    t.Scale = EnemyKindsScale::Ankou * 0.5f;

    // 出現演出中か
    is_appear = true;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Ankou::AnimationName::move, true);
}

void EnemyStraight_None_Start_Back::Initialize()
{
    // 右向きにする
    r.Angle.y = 0.0f;


    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 6.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);


    move_state = std::make_unique<MoveState>();
    BaseMove* move_front = new RushFront(status.speed, 5.0f);


    // カメラの向きの取得
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    BaseMove* start_back = nullptr;
    BaseMove* death = new Death();

    // カメラの向きに合わせて、射撃方法と出現行動の設定をする
    if (camera_direction == c_d::SIDE)
    {
        t.Position.x = -13.0f;
        t.Position.z = -turn_z_axis_size;
        start_back = new Start_FromBack_Side(t.Scale,EnemyKindsScale::Ankou);

        move_state->AddTransition(start_back, move_front, [&](float) {return t.Position.x > -0.1f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.x = 0.0f;
                    timer.Reset();
                    is_appear = false;
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }
    else if (camera_direction == c_d::TOP)
    {
        t.Position.y = -13.0f;
        t.Position.z = -turn_z_axis_size;
        start_back = new Start_FromBack_Top(t.Scale, EnemyKindsScale::Ankou);

        move_state->AddTransition(start_back, move_front, [&](float) {return t.Position.y > -0.1f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.y = 0.0f;
                    timer.Reset();
                    is_appear = false;
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }


    move_state->SetMove(start_back);
}

void EnemyStraight_None_Start_Back::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);
}



EnemyStraight_None_Start_Front::EnemyStraight_None_Start_Front(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Ankou : EnemyModelFilePath::Red_Ankou,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Ankou;

    // スケールの設定
    t.Scale = EnemyKindsScale::Ankou * 0.5f;

    // 出現演出中か
    is_appear = true;

    Model->f_PlayAnimation(EnemyAnim::Ankou::AnimationName::move, true);
}

void EnemyStraight_None_Start_Front::Initialize()
{
    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 6.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);


    move_state = std::make_unique<MoveState>();
    BaseMove* move_front = new RushFront(status.speed, 5.0f);


    // カメラの向きの取得
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    BaseMove* start_back = nullptr;
    BaseMove* death = new Death();

    // カメラの向きに合わせて、射撃方法と出現行動の設定をする
    if (camera_direction == c_d::SIDE)
    {
        t.Position.x = -13.0f;
        t.Position.z = turn_z_axis_size;
        start_back = new Start_FromFront_Side(t.Scale, EnemyKindsScale::Ankou);

        move_state->AddTransition(start_back, move_front, [&](float) {return t.Position.x > -0.1f; })
            .SetInitialize(move_front, [&]()
            {
                t.Position.x = 0.0f;
                timer.Reset();
                is_appear = false;
            })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
            .SetInitialize(death, [&]() {Destroy(); });
    }
    else if(camera_direction == c_d::TOP)
    {
        t.Position.y = -13.0f;
        t.Position.z = turn_z_axis_size;
        start_back = new Start_FromFront_Top(t.Scale, EnemyKindsScale::Ankou);

        move_state->AddTransition(start_back, move_front, [&](float) {return t.Position.y > -0.1f; })
            .SetInitialize(move_front, [&]()
            {
                t.Position.y = 0.0f;
                timer.Reset();
                is_appear = false;
            })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
            .SetInitialize(death, [&]() {Destroy(); });
    }


    move_state->SetMove(start_back);
}

void EnemyStraight_None_Start_Front::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);
}



EnemyStraight_None_Start_Side::EnemyStraight_None_Start_Side(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Ankou : EnemyModelFilePath::Red_Ankou,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // スケールの設定
    t.Scale = EnemyKindsScale::Ankou;

    // 出現演出中か
    is_appear = true;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Ankou::AnimationName::move, true);
}

void EnemyStraight_None_Start_Side::Initialize()
{
    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 6.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    int random_1 = 0;

    move_state = std::make_unique<MoveState>();
    BaseMove* move_front = new RushFront(status.speed, 5.0f);
    BaseMove* idle = new Idle(1, &random_1, 1);


    // カメラの向きの取得
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    BaseMove* start_back = nullptr;
    BaseMove* death = new Death();

    // カメラの向きに合わせて、射撃方法と出現行動の設定をする
    if (camera_direction == c_d::SIDE)
    {
        if (t.Position.y > 0.0f)
        {
            t.Position.x = -13.0f;
            t.Position.y += -55.0f;
            t.Position.z = 25.0f;

            start_back = new Start_FromYDown_RotateX_Side(t.Position);
        }
        else
        {
            t.Position.x = -13.0f;
            t.Position.y += 55.0f;
            t.Position.z = 25.0f;

            start_back = new Start_FromYUp_RotateX_Side(t.Position);
        }

        move_state->AddTransition(start_back, idle, [&](float) {return t.Position.x > -0.1f; })
            .AddTransition(idle, move_front, [&](float) {return timer.NowTime() <= 6.5f/*seconds*/ - 1.5f/*start*/ - 0.5f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.x = 0.0f;
                    timer.Reset();
                    is_appear = false;
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }
    else if (camera_direction == c_d::TOP)
    {
        if (t.Position.x > 0.0f)
        {
            t.Position.x += -90.0f;
            t.Position.y = -13.0f;
            t.Position.z = 20.0f;

            start_back = new Start_FromLeft_RotateY_Top(t.Position);
        }
        else
        {
            t.Position.x += 90.0f;
            t.Position.y = -13.0f;
            t.Position.z = 20.0f;

            start_back = new Start_FromRight_RotateY_Top(t.Position);
        }

        move_state->AddTransition(start_back, idle, [&](float) {return t.Position.y > -0.1f; })
            .AddTransition(idle, move_front, [&](float) {return timer.NowTime() <= 6.5f/*seconds*/ - 1.5f/*start*/ - 0.5f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.y = 0.0f;
                    timer.Reset();
                    is_appear = false;
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }


    move_state->SetMove(start_back);
}

void EnemyStraight_None_Start_Side::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);
}











EnemyStraight_None_Horizon_Top::EnemyStraight_None_Horizon_Top(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Ankou : EnemyModelFilePath::Red_Ankou,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Ankou;

    // スケールの設定
    t.Scale = EnemyKindsScale::Ankou;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Ankou::AnimationName::move, true);
}

void EnemyStraight_None_Horizon_Top::Initialize()
{
    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 6.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    move_state = std::make_unique<MoveState>();
    BaseMove* move_front = new RushFront(status.speed, 2.0f);
    BaseMove* death = new Death();

    // 沸き位置のx値が正だったら
    if(t.Position.x > 0.0f)
    {
        // 左向きにする
        r.Angle.y = DirectX::XMConvertToRadians(-90.0f);
    }

    // x値が負だったら
    else
    {
        // 右向きにする
        r.Angle.y = DirectX::XMConvertToRadians(90.0f);
    }

    move_state->SetMove(move_front)
        .AddTransition(move_front, death, [&](float) {return timer.NowTime() <= 0.0f; })
        .SetInitialize(death, [&]() {Destroy(); });
}

void EnemyStraight_None_Horizon_Top::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    //RotateZ(elapsedTime);

    timer.Update(elapsedTime);
}

void EnemyStraight_None_Horizon_Top::RotateZ(float elapsedTime)
{
    constexpr float angle_speed = DirectX::XMConvertToRadians(270.0f);

    r.Angle.z += angle_speed * elapsedTime;
}






EnemyStraight_None_Bezier_Top::EnemyStraight_None_Bezier_Top(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Ankou : EnemyModelFilePath::Red_Ankou,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Ankou;

    // スケールの設定
    t.Scale = EnemyKindsScale::Ankou;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Ankou::AnimationName::move, true);
}

void EnemyStraight_None_Bezier_Top::Initialize()
{
    t.Position.z = 25.0f;

    // xの値が48.0fより下なら、z値を33.0fにする
    if(fabsf(t.Position.x) <= 48.0f)
    {
        t.Position.z = 33.0f;
    }

    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 6.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    move_state = std::make_unique<MoveState>();
    BaseMove* move_bezier = new RushBezier_Top(t.Position);
    BaseMove* death = new Death();

    move_state->SetMove(move_bezier)
        .AddTransition(move_bezier, death, [&](float) {return timer.NowTime() <= 0.0f; })
        .SetInitialize(death, [&]() {Destroy(); });
}

void EnemyStraight_None_Bezier_Top::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);

    if (timer.NowTime() <= 6.5f) return;

    r.Angle.z = Calcu3D::LerpFloat(r.Angle.z, 6.28f, 5.0f * elapsedTime);
}







EnemyStraight_None_Bezier_Side::EnemyStraight_None_Bezier_Side(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Ankou : EnemyModelFilePath::Red_Ankou,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Ankou;

    // スケールの設定
    t.Scale = EnemyKindsScale::Ankou;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Ankou::AnimationName::move, true);
}

void EnemyStraight_None_Bezier_Side::Initialize()
{
    t.Position.z = 35.0f;

    // xの値が30.0fより下なら、z値を33.0fにする
    if (fabsf(t.Position.y) <= turn_y_axis_size)
    {
        t.Position.y += (t.Position.y > 0.0f) ? +10.0f : -10.0f;
        t.Position.z = 55.0f;
    }

    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 9.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    move_state = std::make_unique<MoveState>();
    BaseMove* move_bezier = new RushBezier_Side(t.Position);
    BaseMove* death = new Death();

    move_state->SetMove(move_bezier)
        .AddTransition(move_bezier, death, [&](float) {return timer.NowTime() <= 0.0f; })
        .SetInitialize(death, [&]() {Destroy(); });
}

void EnemyStraight_None_Bezier_Side::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);

    if (timer.NowTime() <= 6.5f) return;

    r.Angle.z = Calcu3D::LerpFloat(r.Angle.z, 6.28f, 5.0f * elapsedTime);
}





/////////////////////////////////////////    射撃    /////////////////////////////////////////    






EnemyStraight_Shot_Start_Back::EnemyStraight_Shot_Start_Back(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // 射撃レートの設定
    status.shotInterval = 1.5f;

    // スケールの設定
    t.Scale = EnemyKindsScale::Move_Shark * 0.5f;

    // 出現演出中か
    is_appear = true;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyStraight_Shot_Start_Back::Initialize()
{
    // 右向きにする
    r.Angle.y = 0.0f;


    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 6.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);


    move_state = std::make_unique<MoveState>();
    BaseMove* move_front = new RushFront(status.speed, 5.0f);


    // カメラの向きの取得
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    BaseMove* start_back = nullptr;
    BaseMove* death = new Death();

    // カメラの向きに合わせて、射撃方法と出現行動の設定をする
    if (camera_direction == c_d::SIDE)
    {
        t.Position.x = -13.0f;
        t.Position.z = -turn_z_axis_size;
        start_back = new Start_FromBack_Side(t.Scale, EnemyKindsScale::Move_Shark);

        move_state->AddTransition(start_back, move_front, [&](float) {return t.Position.x > -0.1f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.x = 0.0f;
                    timer.Reset();
                    is_appear = false;
                    status.shotTimer = 1.5f + 0.1f;
                    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }
    else if (camera_direction == c_d::TOP)
    {
        t.Position.y = -13.0f;
        t.Position.z = -turn_z_axis_size;
        start_back = new Start_FromBack_Top(t.Scale, EnemyKindsScale::Move_Shark);

        move_state->AddTransition(start_back, move_front, [&](float) {return t.Position.y > -0.1f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.y = 0.0f;
                    timer.Reset();
                    is_appear = false;
                    status.shotTimer = 1.5f + 0.1f;
                    //Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }


    move_state->SetMove(start_back);
}

void EnemyStraight_Shot_Start_Back::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);
}

void EnemyStraight_Shot_Start_Back::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //無敵時間更新
    invTimer -= elapsedTime;

    Shot(elapsedTime);
}

void EnemyStraight_Shot_Start_Back::Shot(float elapsedTime)
{

    // 攻撃開始アニメが終了したら
    if (!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
    }

    if (CanShot(elapsedTime) == false)return;

    const size_t index = move_state->GetNowMove();


    if (timer.NowTime() <= 1.0f) return;
    if (index != STATE::mRush) return;


    BulletShot(elapsedTime);
}


EnemyStraight_Shot_Start_Front::EnemyStraight_Shot_Start_Front(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // 射撃レートの設定
    status.shotInterval = 1.5f;

    // スケールの設定
    t.Scale = EnemyKindsScale::Move_Shark * 0.5f;

    // 出現演出中か
    is_appear = true;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyStraight_Shot_Start_Front::Initialize()
{
    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 6.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);


    move_state = std::make_unique<MoveState>();
    BaseMove* move_front = new RushFront(status.speed, 5.0f);


    // カメラの向きの取得
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    BaseMove* start_back = nullptr;
    BaseMove* death = new Death();

    // カメラの向きに合わせて、射撃方法と出現行動の設定をする
    if (camera_direction == c_d::SIDE)
    {
        t.Position.x = -13.0f;
        t.Position.z = turn_z_axis_size;
        start_back = new Start_FromFront_Side(t.Scale, EnemyKindsScale::Move_Shark);

        move_state->AddTransition(start_back, move_front, [&](float) {return t.Position.x > -0.1f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.x = 0.0f;
                    timer.Reset();
                    is_appear = false;
                    status.shotTimer = 1.5f + 0.1f;
                    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }
    else if (camera_direction == c_d::TOP)
    {
        t.Position.y = -13.0f;
        t.Position.z = turn_z_axis_size;
        start_back = new Start_FromFront_Top(t.Scale, EnemyKindsScale::Move_Shark);

        move_state->AddTransition(start_back, move_front, [&](float) {return t.Position.y > -0.1f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.y = 0.0f;
                    timer.Reset();
                    is_appear = false;
                    status.shotTimer = 1.5f + 0.1f;
                    //Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }


    move_state->SetMove(start_back);
}

void EnemyStraight_Shot_Start_Front::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);
}

void EnemyStraight_Shot_Start_Front::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //無敵時間更新
    invTimer -= elapsedTime;

    Shot(elapsedTime);
}

void EnemyStraight_Shot_Start_Front::Shot(float elapsedTime)
{
    // 攻撃開始アニメが終了したら
    if (!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
    }


    if (CanShot(elapsedTime) == false)return;

    const size_t index = move_state->GetNowMove();


    if (timer.NowTime() <= 1.0f) return;
    if (index != STATE::mRush) return;

    BulletShot(elapsedTime);
}



EnemyStraight_Shot_Start_Side::EnemyStraight_Shot_Start_Side(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // 射撃レートの設定
    status.shotInterval = 1.5f;

    // スケールの設定
    t.Scale = EnemyKindsScale::Move_Shark;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyStraight_Shot_Start_Side::Initialize()
{
    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 6.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);
    

    move_state = std::make_unique<MoveState>();
    BaseMove* move_front = new RushFront(status.speed, 5.0f);
    BaseMove* idle = new Idle(1, nullptr, 1);


    // カメラの向きの取得
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    BaseMove* start_back = nullptr;
    BaseMove* death = new Death();

    // カメラの向きに合わせて、射撃方法と出現行動の設定をする
    if (camera_direction == c_d::SIDE)
    {
        if (t.Position.y > 0.0f)
        {
            t.Position.x = -13.0f;
            t.Position.y += -55.0f;
            t.Position.z = 25.0f;

            start_back = new Start_FromYDown_RotateX_Side(t.Position);
        }
        else
        {
            t.Position.x = -13.0f;
            t.Position.y += 55.0f;
            t.Position.z = 25.0f;

            start_back = new Start_FromYUp_RotateX_Side(t.Position);
        }

        move_state->AddTransition(start_back, idle, [&](float) {return t.Position.x > -0.1f; })
            .AddTransition(idle, move_front, [&](float) {return timer.NowTime() <= 6.5f/*seconds*/ - 1.5f/*start*/ - 0.5f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.x = 0.0f;
                    timer.Reset();
                    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }
    else if (camera_direction == c_d::TOP)
    {
        if (t.Position.x > 0.0f)
        {
            t.Position.x += -90.0f;
            t.Position.y = -13.0f;
            t.Position.z = 20.0f;

            start_back = new Start_FromLeft_RotateY_Top(t.Position);
        }
        else
        {
            t.Position.x += 90.0f;
            t.Position.y = -13.0f;
            t.Position.z = 20.0f;

            start_back = new Start_FromRight_RotateY_Top(t.Position);
        }

        move_state->AddTransition(start_back, idle, [&](float) {return t.Position.y > -0.1f; })
            .AddTransition(idle, move_front, [&](float) {return timer.NowTime() <= 6.5f/*seconds*/ - 1.5f/*start*/ - 0.5f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.y = 0.0f;
                    timer.Reset();
                    status.shotTimer = 1.5f + 0.1f;
                    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }


    move_state->SetMove(start_back);
}

void EnemyStraight_Shot_Start_Side::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);
}

void EnemyStraight_Shot_Start_Side::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //無敵時間更新
    invTimer -= elapsedTime;

    Shot(elapsedTime);
}

void EnemyStraight_Shot_Start_Side::Shot(float elapsedTime)
{
    // 攻撃開始アニメが終了したら
    if (!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
    }

    if (CanShot(elapsedTime) == false)return;

    const size_t index = move_state->GetNowMove();


    if (timer.NowTime() <= 1.0f) return;
    if (index != (STATE::mIdle | STATE::mRush)) return;

    BulletShot(elapsedTime);
}




EnemyStraight_Shot_Bezier_Top::EnemyStraight_Shot_Bezier_Top(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // 射撃レートの設定
    status.shotInterval = 1.5f;

    // スケールの設定
    t.Scale = EnemyKindsScale::Move_Shark;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyStraight_Shot_Bezier_Top::Initialize()
{
    t.Position.z = 25.0f;

    // xの値が48.0fより下なら、z値を33.0fにする
    if (fabsf(t.Position.x) <= 48.0f)
    {
        t.Position.z = 33.0f;
    }


    // イベントフラグを読み込み
    Flags.Load("./resources/Models/Characters/Enemies/Shark.flg");

    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 6.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    move_state = std::make_unique<MoveState>();
    BaseMove* move_bezier = new RushBezier_Top(t.Position);
    BaseMove* death = new Death();

    move_state->SetMove(move_bezier)
        .AddTransition(move_bezier, death, [&](float) {return timer.NowTime() <= 0.0f; })
        .SetInitialize(death, [&]() {Destroy(); });
}

void EnemyStraight_Shot_Bezier_Top::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);
}

void EnemyStraight_Shot_Bezier_Top::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //無敵時間更新
    invTimer -= elapsedTime;

    Shot(elapsedTime);
}

void EnemyStraight_Shot_Bezier_Top::Shot(float elapsedTime)
{
    // 攻撃開始アニメが終了したら
    if (Flags.Check("attack_transition", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
    }

    if (CanShot(elapsedTime) == false)return;

    if (timer.NowTime() <= 5.8f && Model->GetCurrentAnimationNumber() == EnemyAnim::Shark::AnimationName::move)
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
    }

    if (timer.NowTime() <= 1.0f) return;
    

    BulletShot(elapsedTime);
}






EnemyStraight_Shot_Bezier_Side::EnemyStraight_Shot_Bezier_Side(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // 射撃レートの設定
    status.shotInterval = 1.5f;

    // スケールの設定
    t.Scale = EnemyKindsScale::Move_Shark;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyStraight_Shot_Bezier_Side::Initialize()
{
    t.Position.z = 35.0f;

    // xの値が30.0fより下なら、z値を33.0fにする
    if (fabsf(t.Position.y) <= turn_y_axis_size)
    {
        t.Position.y += (t.Position.y > 0.0f) ? +10.0f : -10.0f;
        t.Position.z = 55.0f;
    }


    // イベントフラグを読み込み
    Flags.Load("./resources/Models/Characters/Enemies/Shark.flg");

    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 9.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    move_state = std::make_unique<MoveState>();
    BaseMove* move_bezier = new RushBezier_Side(t.Position);
    BaseMove* death = new Death();

    move_state->SetMove(move_bezier)
        .AddTransition(move_bezier, death, [&](float) {return timer.NowTime() <= 0.0f; })
        .SetInitialize(death, [&]() {Destroy(); });
}

void EnemyStraight_Shot_Bezier_Side::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);

    if (timer.NowTime() <= 6.5f) return;

    r.Angle.z = Calcu3D::LerpFloat(r.Angle.z, 6.28f, 5.0f * elapsedTime);
}

void EnemyStraight_Shot_Bezier_Side::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //無敵時間更新
    invTimer -= elapsedTime;

    Shot(elapsedTime);
}

void EnemyStraight_Shot_Bezier_Side::Shot(float elapsedTime)
{
    // 攻撃開始アニメが終了したら
    if (Flags.Check("attack_transition", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
    }

    if (CanShot(elapsedTime) == false)return;

    if (timer.NowTime() <= 8.8f && Model->GetCurrentAnimationNumber() == EnemyAnim::Shark::AnimationName::move)
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
    }

    if (timer.NowTime() <= 1.0f) return;
    
    BulletShot(elapsedTime);
}










EnemyStraight_TargetShot_Start_Back::EnemyStraight_TargetShot_Start_Back(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, short shot_max_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 目標の設定
    target = target_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // 射撃クールタイマー
    status.shotTimer = shot_max_ == 1 ? 0.0f : 2.0f;

    // 射撃レートの設定
    status.shotInterval = shot_max_ == 1 ? 2.0f : 0.3f;

    // 最大射撃回数
    shot_max = shot_max_;

    // スケールの設定
    t.Scale = EnemyKindsScale::Move_Shark * 0.5f;

    // 出現演出中か
    is_appear = true;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyStraight_TargetShot_Start_Back::Initialize()
{
    // 右向きにする
    r.Angle.y = 0.0f;


    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 6.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);


    move_state = std::make_unique<MoveState>();
    BaseMove* move_front = new RushFront(status.speed, 5.0f);


    // カメラの向きの取得
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    BaseMove* start_back = nullptr;
    BaseMove* death = new Death();

    // カメラの向きに合わせて、射撃方法と出現行動の設定をする
    if (camera_direction == c_d::SIDE)
    {
        t.Position.x = -13.0f;
        t.Position.z = -turn_z_axis_size;
        start_back = new Start_FromBack_Side(t.Scale, EnemyKindsScale::Move_Shark);

        move_state->AddTransition(start_back, move_front, [&](float) {return t.Position.x > -0.1f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.x = 0.0f;
                    timer.Reset();
                    is_appear = false;
                    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }
    else if (camera_direction == c_d::TOP)
    {
        t.Position.y = -13.0f;
        t.Position.z = -turn_z_axis_size;
        start_back = new Start_FromBack_Top(t.Scale, EnemyKindsScale::Ankou);

        move_state->AddTransition(start_back, move_front, [&](float) {return t.Position.y > -0.1f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.y = 0.0f;
                    timer.Reset();
                    is_appear = false;
                    //Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }


    move_state->SetMove(start_back);
}

void EnemyStraight_TargetShot_Start_Back::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);
}

void EnemyStraight_TargetShot_Start_Back::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //無敵時間更新
    invTimer -= elapsedTime;

    Shot(elapsedTime);
}

void EnemyStraight_TargetShot_Start_Back::Shot(float elapsedTime)
{

    // 攻撃開始アニメが終了したら
    if (!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
    }


    if (CanShot(elapsedTime) == false)return;

    const size_t index = move_state->GetNowMove();


    if (timer.NowTime() <= 2.5f) return;
    if (index != STATE::mRush) return;
    if(shot_count >= shot_max) return;

    BulletTargetShot(elapsedTime,*target);

    shot_count++;
}


EnemyStraight_TargetShot_Start_Front::EnemyStraight_TargetShot_Start_Front(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, short shot_max_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 目標の設定
    target = target_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // 射撃クールタイマー
    status.shotTimer = shot_max_ == 1 ? 0.0f : 2.0f;

    // 射撃レートの設定
    status.shotInterval = shot_max_ == 1 ? 2.0f : 0.3f;

    // 最大射撃回数
    shot_max = shot_max_;

    // スケールの設定
    t.Scale = EnemyKindsScale::Move_Shark * 0.5f;

    // 出現演出中か
    is_appear = true;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyStraight_TargetShot_Start_Front::Initialize()
{
    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 6.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);


    move_state = std::make_unique<MoveState>();
    BaseMove* move_front = new RushFront(status.speed, 5.0f);


    // カメラの向きの取得
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    BaseMove* start_back = nullptr;
    BaseMove* death = new Death();

    // カメラの向きに合わせて、射撃方法と出現行動の設定をする
    if (camera_direction == c_d::SIDE)
    {
        t.Position.x = -13.0f;
        t.Position.z = turn_z_axis_size;
        start_back = new Start_FromFront_Side(t.Scale, EnemyKindsScale::Move_Shark);

        move_state->AddTransition(start_back, move_front, [&](float) {return t.Position.x > -0.1f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.x = 0.0f;
                    timer.Reset();
                    is_appear = false;
                    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }
    else if (camera_direction == c_d::TOP)
    {
        t.Position.y = -13.0f;
        t.Position.z = turn_z_axis_size;
        start_back = new Start_FromFront_Top(t.Scale, EnemyKindsScale::Move_Shark);

        move_state->AddTransition(start_back, move_front, [&](float) {return t.Position.y > -0.1f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.y = 0.0f;
                    timer.Reset();
                    is_appear = false;
                    //Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);

                    // 射撃レートの再設定
                    status.shotInterval = (status.shotInterval >= 2.0f - FLT_EPSILON) ? 1.3f : 0.3f;
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }


    move_state->SetMove(start_back);
}

void EnemyStraight_TargetShot_Start_Front::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);
}

void EnemyStraight_TargetShot_Start_Front::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //無敵時間更新
    invTimer -= elapsedTime;

    Shot(elapsedTime);
}

void EnemyStraight_TargetShot_Start_Front::Shot(float elapsedTime)
{
    // 攻撃開始アニメが終了したら
    if (!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
    }

    if (CanShot(elapsedTime) == false)return;

    const size_t index = move_state->GetNowMove();


    if (timer.NowTime() <= 2.5f) return;
    if (index != STATE::mRush) return;
    if (shot_count >= shot_max) return;

    BulletTargetShot(elapsedTime, *target);

    shot_count++;
}



EnemyStraight_TargetShot_Start_Side::EnemyStraight_TargetShot_Start_Side(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, short shot_max_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 目標の設定
    target = target_;

    // 速度の設定
    status.speed = 15.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // 射撃クールタイマー
    status.shotTimer = shot_max_ == 1 ? 0.0f : 2.0f;

    // 射撃レートの設定
    status.shotInterval = shot_max_ == 1 ? 2.0f : 0.3f;

    // 最大射撃回数
    shot_max = shot_max_;

    // スケールの設定
    t.Scale = EnemyKindsScale::Move_Shark;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyStraight_TargetShot_Start_Side::Initialize()
{
    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 6.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);


    move_state = std::make_unique<MoveState>();
    BaseMove* move_front = new RushFront(status.speed, 5.0f);
    BaseMove* idle = new Idle(1, nullptr, 1);


    // カメラの向きの取得
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    BaseMove* start_back = nullptr;
    BaseMove* death = new Death();

    // カメラの向きに合わせて、射撃方法と出現行動の設定をする
    if (camera_direction == c_d::SIDE)
    {
        if (t.Position.y > 0.0f)
        {
            t.Position.x = -13.0f;
            t.Position.y += -55.0f;
            t.Position.z = 25.0f;

            start_back = new Start_FromYDown_RotateX_Side(t.Position);
        }
        else
        {
            t.Position.x = -13.0f;
            t.Position.y += 55.0f;
            t.Position.z = 25.0f;

            start_back = new Start_FromYUp_RotateX_Side(t.Position);
        }

        move_state->AddTransition(start_back, idle, [&](float) {return t.Position.x > -0.1f; })
            .AddTransition(idle, move_front, [&](float) {return timer.NowTime() <= 6.5f/*seconds*/ - 1.5f/*start*/ - 0.5f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.x = 0.0f;
                    timer.Reset();
                    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }
    else if (camera_direction == c_d::TOP)
    {
        if (t.Position.x > 0.0f)
        {
            t.Position.x += -90.0f;
            t.Position.y = -13.0f;
            t.Position.z = 20.0f;

            start_back = new Start_FromLeft_RotateY_Top(t.Position);
        }
        else
        {
            t.Position.x += 90.0f;
            t.Position.y = -13.0f;
            t.Position.z = 20.0f;

            start_back = new Start_FromRight_RotateY_Top(t.Position);
        }

        move_state->AddTransition(start_back, idle, [&](float) {return t.Position.y > -0.1f; })
            .AddTransition(idle, move_front, [&](float) {return timer.NowTime() <= 6.5f/*seconds*/ - 1.5f/*start*/ - 0.5f; })
            .SetInitialize(move_front, [&]()
                {
                    t.Position.y = 0.0f;
                    timer.Reset();
                    //Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
                })
            .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 0.0f); })
                    .SetInitialize(death, [&]() {Destroy(); });
    }


    move_state->SetMove(start_back);
}

void EnemyStraight_TargetShot_Start_Side::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);
}

void EnemyStraight_TargetShot_Start_Side::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //無敵時間更新
    invTimer -= elapsedTime;

    Shot(elapsedTime);
}

void EnemyStraight_TargetShot_Start_Side::Shot(float elapsedTime)
{
    // 攻撃開始アニメが終了したら
    if (!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
    }

    if (CanShot(elapsedTime) == false)return;

    const size_t index = move_state->GetNowMove();


    if (timer.NowTime() <= 2.5f) return;
    if (index != (STATE::mIdle | STATE::mRush)) return;
    if (shot_count >= shot_max) return;

    BulletTargetShot(elapsedTime, *target);

    shot_count++;
}








EnemyStraight_TargetShot_Bezier_Top::EnemyStraight_TargetShot_Bezier_Top(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, short shot_max_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 目標の設定
    target = target_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // 射撃クールタイマー
    status.shotTimer = shot_max_ == 1 ? 0.0f : 2.0f;

    // 射撃レートの設定
    status.shotInterval = shot_max_ == 1 ? 2.0f : 0.7f;

    // 最大射撃回数
    shot_max = shot_max_;

    // スケールの設定
    t.Scale = EnemyKindsScale::Move_Shark;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyStraight_TargetShot_Bezier_Top::Initialize()
{
    t.Position.z = 25.0f;

    // xの値が48.0fより下なら、z値を33.0fにする
    if (fabsf(t.Position.x) <= 48.0f)
    {
        t.Position.z = 33.0f;
    }


    // イベントフラグを読み込み
    Flags.Load("./resources/Models/Characters/Enemies/Shark.flg");

    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 6.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    move_state = std::make_unique<MoveState>();
    BaseMove* move_bezier = new RushBezier_Top(t.Position);
    BaseMove* death = new Death();

    move_state->SetMove(move_bezier)
        .AddTransition(move_bezier, death, [&](float) {return timer.NowTime() <= 0.0f; })
        .SetInitialize(death, [&]() {Destroy(); });
}

void EnemyStraight_TargetShot_Bezier_Top::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);
}

void EnemyStraight_TargetShot_Bezier_Top::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //無敵時間更新
    invTimer -= elapsedTime;

    Shot(elapsedTime);
}

void EnemyStraight_TargetShot_Bezier_Top::Shot(float elapsedTime)
{
    // 攻撃開始アニメが終了したら
    if (Flags.Check("attack_transition", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
    }

    if (CanShot(elapsedTime) == false)return;

    if (timer.NowTime() <= 5.8f && Model->GetCurrentAnimationNumber() == EnemyAnim::Shark::AnimationName::move)
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
    }

    if (timer.NowTime() <= 6.0f) return;
    if (shot_count >= shot_max) return;
    

    BulletTargetShot(elapsedTime, *target);

    shot_count++;
}






EnemyStraight_TargetShot_Bezier_Side::EnemyStraight_TargetShot_Bezier_Side(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, short shot_max_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{
    // 属性の設定
    type = type_;

    // 目標の設定
    target = target_;

    // 速度の設定
    status.speed = 30.0f;

    // 体力の設定
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // 射撃クールタイマー
    status.shotTimer = shot_max_ == 1 ? 0.0f : 2.0f;

    // 射撃レートの設定
    status.shotInterval = shot_max_ == 1 ? 2.0f : 0.7f;

    // 最大射撃回数
    shot_max = shot_max_;

    // スケールの設定
    t.Scale = EnemyKindsScale::Move_Shark;

    // 初期アニメーション
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyStraight_TargetShot_Bezier_Side::Initialize()
{
    t.Position.z = 35.0f;

    // xの値が30.0fより下なら、z値を33.0fにする
    if (fabsf(t.Position.y) <= turn_y_axis_size)
    {
        t.Position.y += (t.Position.y > 0.0f) ? +10.0f : -10.0f;
        t.Position.z = 55.0f;
    }


    // イベントフラグを読み込み
    Flags.Load("./resources/Models/Characters/Enemies/Shark.flg");

    // 移動し始めてから削除するまでの時間フラグ
    constexpr float seconds = 9.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    move_state = std::make_unique<MoveState>();
    BaseMove* move_bezier = new RushBezier_Side(t.Position);
    BaseMove* death = new Death();

    move_state->SetMove(move_bezier)
        .AddTransition(move_bezier, death, [&](float) {return timer.NowTime() <= 0.0f; })
        .SetInitialize(death, [&]() {Destroy(); });
}

void EnemyStraight_TargetShot_Bezier_Side::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);

    if (timer.NowTime() <= 6.5f) return;

    r.Angle.z = Calcu3D::LerpFloat(r.Angle.z, 6.28f, 5.0f * elapsedTime);
}

void EnemyStraight_TargetShot_Bezier_Side::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //無敵時間更新
    invTimer -= elapsedTime;

    Shot(elapsedTime);
}

void EnemyStraight_TargetShot_Bezier_Side::Shot(float elapsedTime)
{
    // 攻撃開始アニメが終了したら
    if (Flags.Check("attack_transition", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
    }

    if (CanShot(elapsedTime) == false)return;

    if (timer.NowTime() <= 8.8f && Model->GetCurrentAnimationNumber() == EnemyAnim::Shark::AnimationName::move)
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin);
    }

    if (timer.NowTime() <= 6.0f) return;
    if (shot_count >= shot_max) return;

    BulletTargetShot(elapsedTime, *target);

    shot_count++;
}