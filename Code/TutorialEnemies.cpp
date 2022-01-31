#include"EnemyManager.h"
#include"TutorialEnemies.h"
#include<functional>

#include "Collision.h"
#include "GameSystem.h"
#include "ProjectileManager.h"
#include"Bullet.h"
#include "Energy.h"
#include "SceneManager.h"

//****************************************************************
// 
//  チュートリアルの敵の振る舞いを管理するクラス
// 
//****************************************************************

// ふるまい管理クラスにふるまいを追加
void EnemyBehaviorManager::AddBehavior(std::function<void(float)>pFunction)
{
    BehaviorVec.emplace_back(pFunction);
}

// 次のふるまいへ遷移
void EnemyBehaviorManager::Step()
{
    CurrentBehavior++;
}

// ふるまいの更新
void EnemyBehaviorManager::Update(float elapsedTime)
{
    (this->BehaviorVec[CurrentBehavior])(elapsedTime);
}

AttackTutorialEnemy::AttackTutorialEnemy(ID3D11Device* d, const Type& type, DirectX::XMFLOAT3 WaitPosition_,int Index_)
    :Enemy(d, "./resources/Models/Characters/Enemies/Shark_Red.nk", "./resources/Shaders/LambertPS.cso")
{
    // 初期化
    Index = Index_; // 番号を初期化
    WaitPosition = WaitPosition_; // 待機位置を初期化
    InitialPosition = { 0.0f,10.0f,100.0f };
    MoveRatio = 0.0f;
    t.Position = InitialPosition;
    status.hitPoint = 1.0f;
    this->type = Type::RED;

    BehaviorVec.emplace_back(&AttackTutorialEnemy::Entry);
    BehaviorVec.emplace_back(&AttackTutorialEnemy::Wait);
}

AttackTutorialEnemy::~AttackTutorialEnemy()
{
    if (Index == 1)
    {
        EnemyManager::Instance().TutorialSetIsAliveAttackTutorialEnemy1(false);
    }
    else if(Index==2)
    {
        EnemyManager::Instance().TutorialSetIsAliveAttackTutorialEnemy2(false);
    }
}

void AttackTutorialEnemy::Initialize()
{
    Enemy::Initialize();
}

void AttackTutorialEnemy::Render(ID3D11DeviceContext* dc)
{
    Enemy::Render(dc);
}

void AttackTutorialEnemy::Finalize()
{
    
}

void AttackTutorialEnemy::Behavior(float elapsed_time)
{
    Move(elapsed_time);
    Collision();
}

void AttackTutorialEnemy::Move(float elapsed_time)
{
    (this->*BehaviorVec[CurrentBehavior])(elapsed_time);
    //無敵時間更新
    invTimer -= elapsed_time;
}

void AttackTutorialEnemy::Entry(float elapsedTime)
{
    MoveRatio += elapsedTime;
    // 決められた場所まで移動する
    t.Position = Calcu3D::LerpFloat3(InitialPosition, WaitPosition, MoveRatio);
    // 条件を満たしたら次へ
    if(MoveRatio>=1.0f)
    {
        MoveRatio = 1.0f;
        CurrentBehavior++;
    }
}

void AttackTutorialEnemy::Wait(float elapsedTime)
{
    // 何もしない
}

void AttackTutorialEnemy::Collision()
{
    const int projCount = ProjectileManager::Instance().GetProjectileCount();
    for (int i = 0; i < projCount; i++)
    {
        BaseProjectile& proj = ProjectileManager::Instance().GetProjectile(i);

        const int projType = static_cast<int>(proj.GetProjType());

        //弾の親がエネミーならcontinue
        if (proj.GetParent() == BaseProjectile::Parent::BLUEENEMY || proj.GetParent() == BaseProjectile::Parent::REDENEMY)continue;

        GameSystem& gameSystem = GameSystem::Instance();

        //衝突判定 
        if (Collision::CubeVsCube(t.Position, hitAreaSize, proj.GetPosition(), proj.GetHitAreaSize()))
        {
            //ダメージ処理
            if (HitProcessing(projType))
            {
                ProjectileManager::Instance().Remove(&proj);
                EnemyManager::Instance().TutorialSetIsAliveShotTutorialEnemy(true);
            }
        }
    }

    //エリア外に出たら削除
    

   // if ( -60 > pos.z > -100 ) Destroy();

   //if (t.Position.z >= -70.0f || t.Position.z <= -100.0f) return;
    if (t.Position.z >= -70.0f) return;
    Destroy();

}

//****************************************************************
// 
//  回避のチュートリアルの敵
// 
//****************************************************************


DodgeTutorialEnemy::DodgeTutorialEnemy(ID3D11Device* d, const Type& type, DirectX::XMFLOAT3 WaitPosition)
    :Enemy(d, "./resources/Models/Characters/Enemies./Shark_Blue.nk", "./resources/Shaders/LambertPS.cso")
{
    Initialize();
    this->WaitPosition = WaitPosition;
    InitialPosition = { 0.0f,0.0f,100.0f };
    t.Position = InitialPosition;

    BehaviorVec.emplace_back(&DodgeTutorialEnemy::Entry);
    BehaviorVec.emplace_back(&DodgeTutorialEnemy::Shot);
}

DodgeTutorialEnemy::~DodgeTutorialEnemy()
{

}


// 初期化
void DodgeTutorialEnemy::Initialize()
{
    Enemy::Initialize();
}

// 描画
void DodgeTutorialEnemy::Render(ID3D11DeviceContext* dc)
{
    Enemy::Render(dc);
}

void DodgeTutorialEnemy::Finalize()
{
    Enemy::Finalize();
}

void DodgeTutorialEnemy::Behavior(float elapsed_time)
{
    Move(elapsed_time);
}

void DodgeTutorialEnemy::Move(float elapsed_time)
{
    (this->*BehaviorVec[CurrentBehavior])(elapsed_time);
    //無敵時間更新s
}

void DodgeTutorialEnemy::Entry(float elapsedTime)
{
    MoveRatio += elapsedTime;
    // 決められた場所まで移動する
    t.Position = Calcu3D::LerpFloat3(InitialPosition, WaitPosition, MoveRatio);
    // 条件を満たしたら次へ
    if (MoveRatio >= 1.0f)
    {
        MoveRatio = 1.0f;
        CurrentBehavior++;
    }
}

void DodgeTutorialEnemy::Shot(float elapsedTime)
{
    StackShotInterval += elapsedTime;

    if(StackShotInterval>ShotInterval)  // 射撃
    {
        // 正面ベクトルを取得
        const auto front = GetFrontVec();

        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        // 前・左右斜めに発射
        auto b0 = new Bullet(p_device, BaseProjectile::Parent::BLUEENEMY,
                             t.Position, front);
        ProjectileManager::Instance().RegisterProjectile(b0);

        StackShotInterval = 0.0f;
    }
}


//****************************************************************
// 
//  エネルギーのチュートリアルの敵
// 
//****************************************************************
EnergyTutorialEnemy::EnergyTutorialEnemy(ID3D11Device* d, const Type& type, DirectX::XMFLOAT3 WaitPosition)
    :Enemy(d, "./resources/Models/Characters/Enemies/Shark_Red.nk", "./resources/Shaders/LambertPS.cso")
{
    Initialize();

    this->WaitPosition = WaitPosition;
    InitialPosition = { 0.0f,0.0f,100.0f };
    t.Position = InitialPosition;

    StackShotInterval = 0.0f;
    BehaviorVec.emplace_back(&EnergyTutorialEnemy::Entry);
    BehaviorVec.emplace_back(&EnergyTutorialEnemy::Shot);
    this->type = type;
    status.hitPoint = 2.0f;
}

EnergyTutorialEnemy::~EnergyTutorialEnemy()
{
    EnemyManager::Instance().Tutorial_SetIsAliveElementTutorialEnemy1(false);
}

void EnergyTutorialEnemy::Initialize()
{
    Enemy::Initialize();
}

void EnergyTutorialEnemy::Render(ID3D11DeviceContext* dc)
{
    Enemy::Render(dc);
}

void EnergyTutorialEnemy::Finalize()
{
    Enemy::Finalize();
}

void EnergyTutorialEnemy::Behavior(float elapsed_time)
{
    Move(elapsed_time);
    VsProjectile();
}

void EnergyTutorialEnemy::Move(float elapsed_time)
{
    (this->*BehaviorVec[CurrentBehavior])(elapsed_time);
    invTimer -= elapsed_time;
}

void EnergyTutorialEnemy::Entry(float elapsedTime)
{
    MoveRatio += elapsedTime;
    // 決められた場所まで移動する
    t.Position = Calcu3D::LerpFloat3(InitialPosition, WaitPosition, MoveRatio);
    // 条件を満たしたら次へ
    if (MoveRatio >= 1.0f)
    {
        MoveRatio = 1.0f;
        CurrentBehavior++;
    }
}

void EnergyTutorialEnemy::Shot(float elapsedTime)
{
    StackShotInterval += elapsedTime;

    if (StackShotInterval > ShotInterval)  // 射撃
    {
        // 正面ベクトルを取得
        const auto front = GetFrontVec();

        ID3D11Device* p_device = SceneManager::Instance().GetDevice();

        Energy* b0 = new Energy(p_device, BaseProjectile::Parent::REDENEMY,
            t.Position, front);
        
        
        ProjectileManager::Instance().RegisterProjectile(b0);
        StackShotInterval = 0.0f;
    }
}

EnergyTutorialEnemy2::EnergyTutorialEnemy2(ID3D11Device* d, const Type& type, DirectX::XMFLOAT3 WaitPosition)
    :Enemy(d, "./resources/Models/Characters/Enemies/Shark_Blue.nk", "./resources/Shaders/LambertPS.cso")
{
    Initialize();

    this->WaitPosition = WaitPosition;
    InitialPosition = { 0.0f,0.0f,100.0f };
    t.Position = InitialPosition;

    StackShotInterval = 0.0f;
    BehaviorVec.emplace_back(&EnergyTutorialEnemy2::Entry);
    BehaviorVec.emplace_back(&EnergyTutorialEnemy2::Shot);
    this->type = type;
    status.hitPoint = 2.0f;
}

EnergyTutorialEnemy2::~EnergyTutorialEnemy2()
{
    EnemyManager::Instance().Tutorial_SetIsAliveElementTutorialEnemy2(false);
}

void EnergyTutorialEnemy2::Initialize()
{
    Enemy::Initialize();
}

void EnergyTutorialEnemy2::Render(ID3D11DeviceContext* dc)
{
    Enemy::Render(dc);
}

void EnergyTutorialEnemy2::Finalize()
{
    Enemy::Finalize();
}

void EnergyTutorialEnemy2::Behavior(float elapsed_time)
{
    (this->*BehaviorVec[CurrentBehavior])(elapsed_time);
    invTimer -= elapsed_time;
    VsProjectile();
}

void EnergyTutorialEnemy2::Move(float elapsed_time)
{
    Enemy::Move(elapsed_time);
}

void EnergyTutorialEnemy2::Entry(float elapsedTime)
{
    MoveRatio += elapsedTime;
    // 決められた場所まで移動する
    t.Position = Calcu3D::LerpFloat3(InitialPosition, WaitPosition, MoveRatio);
    // 条件を満たしたら次へ
    if (MoveRatio >= 1.0f)
    {
        MoveRatio = 1.0f;
        CurrentBehavior++;
    }
}

void EnergyTutorialEnemy2::Shot(float elapsedTime)
{
    StackShotInterval += elapsedTime;

    if (StackShotInterval > ShotInterval)  // 射撃
    {
        // 正面ベクトルを取得
        const auto front = GetFrontVec();

        ID3D11Device* p_device = SceneManager::Instance().GetDevice();

        Energy* b0 = new Energy(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, front);
        ProjectileManager::Instance().RegisterProjectile(b0);
        StackShotInterval = 0.0f;
    }
}



