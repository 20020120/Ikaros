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
//  �`���[�g���A���̓G�̐U�镑�����Ǘ�����N���X
// 
//****************************************************************

// �ӂ�܂��Ǘ��N���X�ɂӂ�܂���ǉ�
void EnemyBehaviorManager::AddBehavior(std::function<void(float)>pFunction)
{
    BehaviorVec.emplace_back(pFunction);
}

// ���̂ӂ�܂��֑J��
void EnemyBehaviorManager::Step()
{
    CurrentBehavior++;
}

// �ӂ�܂��̍X�V
void EnemyBehaviorManager::Update(float elapsedTime)
{
    (this->BehaviorVec[CurrentBehavior])(elapsedTime);
}

AttackTutorialEnemy::AttackTutorialEnemy(ID3D11Device* d, const Type& type, DirectX::XMFLOAT3 WaitPosition_,int Index_)
    :Enemy(d, "./resources/Models/Characters/Enemies/Shark_Red.nk", "./resources/Shaders/LambertPS.cso")
{
    // ������
    Index = Index_; // �ԍ���������
    WaitPosition = WaitPosition_; // �ҋ@�ʒu��������
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
    //���G���ԍX�V
    invTimer -= elapsed_time;
}

void AttackTutorialEnemy::Entry(float elapsedTime)
{
    MoveRatio += elapsedTime;
    // ���߂�ꂽ�ꏊ�܂ňړ�����
    t.Position = Calcu3D::LerpFloat3(InitialPosition, WaitPosition, MoveRatio);
    // �����𖞂������玟��
    if(MoveRatio>=1.0f)
    {
        MoveRatio = 1.0f;
        CurrentBehavior++;
    }
}

void AttackTutorialEnemy::Wait(float elapsedTime)
{
    // �������Ȃ�
}

void AttackTutorialEnemy::Collision()
{
    const int projCount = ProjectileManager::Instance().GetProjectileCount();
    for (int i = 0; i < projCount; i++)
    {
        BaseProjectile& proj = ProjectileManager::Instance().GetProjectile(i);

        const int projType = static_cast<int>(proj.GetProjType());

        //�e�̐e���G�l�~�[�Ȃ�continue
        if (proj.GetParent() == BaseProjectile::Parent::BLUEENEMY || proj.GetParent() == BaseProjectile::Parent::REDENEMY)continue;

        GameSystem& gameSystem = GameSystem::Instance();

        //�Փ˔��� 
        if (Collision::CubeVsCube(t.Position, hitAreaSize, proj.GetPosition(), proj.GetHitAreaSize()))
        {
            //�_���[�W����
            if (HitProcessing(projType))
            {
                ProjectileManager::Instance().Remove(&proj);
                EnemyManager::Instance().TutorialSetIsAliveShotTutorialEnemy(true);
            }
        }
    }

    //�G���A�O�ɏo����폜
    

   // if ( -60 > pos.z > -100 ) Destroy();

   //if (t.Position.z >= -70.0f || t.Position.z <= -100.0f) return;
    if (t.Position.z >= -70.0f) return;
    Destroy();

}

//****************************************************************
// 
//  ����̃`���[�g���A���̓G
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


// ������
void DodgeTutorialEnemy::Initialize()
{
    Enemy::Initialize();
}

// �`��
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
    //���G���ԍX�Vs
}

void DodgeTutorialEnemy::Entry(float elapsedTime)
{
    MoveRatio += elapsedTime;
    // ���߂�ꂽ�ꏊ�܂ňړ�����
    t.Position = Calcu3D::LerpFloat3(InitialPosition, WaitPosition, MoveRatio);
    // �����𖞂������玟��
    if (MoveRatio >= 1.0f)
    {
        MoveRatio = 1.0f;
        CurrentBehavior++;
    }
}

void DodgeTutorialEnemy::Shot(float elapsedTime)
{
    StackShotInterval += elapsedTime;

    if(StackShotInterval>ShotInterval)  // �ˌ�
    {
        // ���ʃx�N�g�����擾
        const auto front = GetFrontVec();

        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        // �O�E���E�΂߂ɔ���
        auto b0 = new Bullet(p_device, BaseProjectile::Parent::BLUEENEMY,
                             t.Position, front);
        ProjectileManager::Instance().RegisterProjectile(b0);

        StackShotInterval = 0.0f;
    }
}


//****************************************************************
// 
//  �G�l���M�[�̃`���[�g���A���̓G
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
    // ���߂�ꂽ�ꏊ�܂ňړ�����
    t.Position = Calcu3D::LerpFloat3(InitialPosition, WaitPosition, MoveRatio);
    // �����𖞂������玟��
    if (MoveRatio >= 1.0f)
    {
        MoveRatio = 1.0f;
        CurrentBehavior++;
    }
}

void EnergyTutorialEnemy::Shot(float elapsedTime)
{
    StackShotInterval += elapsedTime;

    if (StackShotInterval > ShotInterval)  // �ˌ�
    {
        // ���ʃx�N�g�����擾
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
    // ���߂�ꂽ�ꏊ�܂ňړ�����
    t.Position = Calcu3D::LerpFloat3(InitialPosition, WaitPosition, MoveRatio);
    // �����𖞂������玟��
    if (MoveRatio >= 1.0f)
    {
        MoveRatio = 1.0f;
        CurrentBehavior++;
    }
}

void EnergyTutorialEnemy2::Shot(float elapsedTime)
{
    StackShotInterval += elapsedTime;

    if (StackShotInterval > ShotInterval)  // �ˌ�
    {
        // ���ʃx�N�g�����擾
        const auto front = GetFrontVec();

        ID3D11Device* p_device = SceneManager::Instance().GetDevice();

        Energy* b0 = new Energy(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, front);
        ProjectileManager::Instance().RegisterProjectile(b0);
        StackShotInterval = 0.0f;
    }
}



