

#include "Boss_1.h"

#include "MovePhase_Idle.h"
#include "MovePhase_Rotate.h"
#include "MovePhase_Rush.h"


#include "SceneManager.h"
#include "ProjectileManager.h"
#include "Bullet.h"


Boss_1::Boss_1(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_) : Enemy(
    Device,
    EnemyModelFilePath::Red_Boss0,
    EnemyPSFilePath::Lambert
)
{
    type = type_;

    target = target_;

    Initialize();
}

void Boss_1::Initialize()
{
    //--------------------<変数初期化>--------------------//
    t.Position = { 0.0f,0.0f,60.0f };
    t.Scale = { 0.035f,0.035f,0.035f };

    r.Angle = { 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };

    // はやさ
    status.speed = 20.0f;

    // 射撃レート
    status.shotInterval = 1.0f;

    // 速度の倍率
    speed_ratio = 1.0f;


    // 突撃回数の管理
    rush_count = 0;

    // 抽選ナンバー
    random_min_1 = 100;

    /* 関数ポインタ */
    // 攻撃

    constexpr DirectX::XMFLOAT3 origin_pos = { 0.0f,0.0f,30.0f };

    move_phase                                = std::make_unique<MoveState>();
    BaseMove*           return_origin_pos     = new ReturnOriginPosition(origin_pos);                   // unique_id 0
    BaseMove*           idle                  = new Idle(3,&random_min_1,4);                  //  1
    BaseMove*           return_teleport       = new ReturnTeleport(origin_pos);                         //  2
    BaseMove*           rotate_to_up_l        = new RotateToUpLerp;                                     //  3
    BaseMove*           rotate_to_down_l      = new RotateToDownLerp;                                   //  4
    RotateXToTargetLerp* rotate_to_target_l   = new RotateXToTargetLerp(target,1.0f);              //  5
    BaseMove*           shake_body            = new ShakeBody;                                          //  6
    BaseMove*           rush_front            = new RushFront(20.0f,9.0f);           //  7
    BaseMove*           rush_target           = new RushTarget(20.0f,9.0f);               //  8
    BaseMove*           next_pos_horizon      = new NextPositionRushHorizon(&rush_count,origin_pos);    //  9
    BaseMove*           next_pos_vertical     = new NextPositionRushVertical(&rush_count);              // 10
    BaseMove*           rotate_to_target      = new RotateXToTarget(target,&rush_count);                // 11

    p_angle = &rotate_to_target_l->GetAngle();
    move_phase->SetMove(return_origin_pos);

    move_phase->AddTransition(return_origin_pos, idle, [&](float elapsedTime) {return IsInOriginArea(); })
        .AddTransition(return_teleport, idle, [&](float elapsedTime) {return IsInOriginArea(); })

        .AddTransition(idle, rotate_to_up_l, [&](float) {return random_min_1 == 1; })
        .AddTransition(idle, rotate_to_down_l, [&](float) {return random_min_1 == 2; })
        .AddTransition(idle, rotate_to_target_l, [&](float) {return random_min_1 == 3; })
        .AddTransition(idle, shake_body, [&](float) {return random_min_1 == 4; })

        .AddTransition(rotate_to_up_l, rush_front, [&](float) {return (r.Angle.x < -1.57f + 0.01f); })
        .AddTransition(rotate_to_down_l, rush_front, [&](float) {return (r.Angle.x > 1.57f - 0.01f); })
        .AddTransition(rotate_to_target_l, rush_target, [&](float) {return (*p_angle < 0.0f + 0.01f); })
        .AddTransition(shake_body, rush_front, [](float) {return true; })

        .AddTransition(rush_front, next_pos_horizon, [&](float) {return IsLimitOverZ(); })
        .AddTransition(rush_front, next_pos_vertical, [&](float) {return IsLimitOverY(); })
        .AddTransition(rush_target, rotate_to_target, [&](float) {return (IsInScreenArea() == false); })


        .AddTransition(next_pos_horizon, rush_front, [&](float) {return (rush_count < max_rush_count); })
        .AddTransition(next_pos_horizon, return_teleport, [&](float) {return (rush_count >= max_rush_count); })
        .AddTransition(next_pos_vertical, rush_front, [&](float) {return (rush_count < max_rush_count); })
        .AddTransition(next_pos_vertical, return_teleport, [&](float) {return (rush_count >= max_rush_count); })
        .AddTransition(rotate_to_target, rush_target, [&](float) {return (rush_count < max_rush_count); })
        .AddTransition(rotate_to_target, return_origin_pos, [&](float) {return (rush_count >= max_rush_count); });

    move_phase->SetInitialize(idle, [&]() {random_min_1 = 100; })
        .SetInitialize(return_origin_pos, [&]() {rush_count = 0; r.Angle = { 0.0f,-2.0f * 1.57f,0.0f }; })
        .SetInitialize(return_teleport, [&]() {rush_count = 0; r.Angle = { 0.0f,-2.0f * 1.57f,0.0f }; });

}


void Boss_1::Behavior(float elapsedTime)
{
    // 移動
    Move(elapsedTime);

    // 攻撃
    Attack(elapsedTime);

    //無敵時間更新
    invTimer -= elapsedTime;
}

void Boss_1::Move(float elapsedTime)
{
    move_phase->Update(elapsedTime, t.Position, r.Angle);
}


void Boss_1::Attack(float elapsedTime)
{
    if (CanShot(elapsedTime) == false) return;

    const size_t move_unique_id = move_phase->GetNowMove();

    constexpr size_t no_shot_id = (mReturnOriginPosition | mReturnTeleport | mRotateToUpLerp | mRotateToDownLerp | mShakeBody | mRushTarget);
    // ビットの論理積を用いて条件処理  (例) 0110 & 1101 = 0100 (true), 0010 & 1101 = 0000 (false)
    //if (move_unique_id & no_shot_id) return;


    constexpr size_t single_shot_id = (mIdle | mRotateToTargetLerp | mRotateToTarget);
    if (move_unique_id & single_shot_id)
    {
        TwinShot(elapsedTime);

        return;
    }

    TripleShot(elapsedTime);
}


void Boss_1::SingleShot(float elapsedTime) const
{
    if (type == Type::NONE) return;


    DirectX::XMFLOAT3 velocity = {};

    const float angleX = r.Angle.x + DirectX::XMConvertToRadians(180);
    velocity = { 0,sinf(angleX),cosf(angleX) };

    if (type == Type::RED)
    {
        ProjectileManager::Instance().RegisterProjectile(new Bullet(SceneManager::Instance().GetDevice(), BaseProjectile::Parent::REDENEMY,
            t.Position, velocity));

        return;
    }


    if (type == Type::BLUE)
    {
        ProjectileManager::Instance().RegisterProjectile(new Bullet(SceneManager::Instance().GetDevice(), BaseProjectile::Parent::BLUEENEMY,
            t.Position, velocity));

        return;
    }
}


void Boss_1::TwinShot(float elapsedTime) const
{
    if (type == Type::NONE) return;

    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    DirectX::XMFLOAT3 velocity = {};

    if (type == Type::RED)
    {

        velocity = { 0,1,-1 };

        ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, velocity));

        velocity.y *= -1.0f;

        ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, velocity));
        return;
    }


    if (type == Type::BLUE)
    {
        velocity = { 0,1,-1 };

        ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, velocity));

        velocity.y *= -1.0f;

        ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, velocity));

        return;
    }
}


void Boss_1::TripleShot(float elapsedTime) const
{
    if (type == Type::NONE) return;

    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    DirectX::XMFLOAT3 velocity = {};

    const float angleX = r.Angle.x + DirectX::XMConvertToRadians(180);
    velocity = { 0,sinf(angleX),cosf(angleX) };

    if (type == Type::RED)
    {
        ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, velocity));

        velocity = { 0,1,-1 };

        ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, velocity));

        velocity.y *= -1.0f;

        ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, velocity));

        return;
    }


    if (type == Type::BLUE)
    {
        ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, velocity));

        velocity = { 0,1,-1 };

        ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, velocity));

        velocity.y *= -1.0f;

        ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, velocity));

        return;
    }
}




bool Boss_1::IsInOriginArea()
{
    constexpr float epsilon_size = 2.0f;

    // 通常位置
    constexpr DirectX::XMFLOAT3 origin_position = { 0.0f,0.0f,30.0f };

    if (t.Position.z < origin_position.z - epsilon_size) return false;
    if (t.Position.y < origin_position.y - epsilon_size) return false;
    if (t.Position.z > origin_position.z + epsilon_size) return false;
    if (t.Position.y > origin_position.y + epsilon_size) return false;

    return true;
}


bool Boss_1::IsInScreenArea()
{
    bool flag[2] = { false,false };
    constexpr float turn_z_axis_size = 50.0f;
    constexpr float turn_y_axis_size = 30.0f;
    if (t.Position.z > -turn_z_axis_size && t.Position.z < turn_z_axis_size) flag[0] = true;
    if (t.Position.y > -turn_y_axis_size && t.Position.y < turn_y_axis_size) flag[1] = true;

    if (flag[0] == true && flag[1] == true) return true;

    return false;
}


void Boss_1::Collision()
{
    //弾との当たり判定
    VsProjectile();

    //エリア外に出たら削除
    //if (t.Position.z >= -60.0f || t.Position.z <= -100.0f) return;
    //Destroy();
}


bool Boss_1::IsLimitOverY()
{
    return (t.Position.y < -turn_y_axis_size || t.Position.y > turn_y_axis_size);
}


bool Boss_1::IsLimitOverZ()
{
    return (t.Position.z < -turn_z_axis_size || t.Position.z > turn_z_axis_size);
}
