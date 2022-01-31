#include "EnemyManager.h"
#include "ProjectileManager.h"
#include "Collision.h"
#include "Bullet.h"
#include "Energy.h"
#include "SceneManager.h"
#include "User_Function.h"
#include "MathSupport.h"
#include"EffectResoureceManaeger.h"
#include "Enemy.h"

#include "GameSystem.h"
#include "gAudio.h"
#include "OptionSystem.h"

Enemy::Enemy(ID3D11Device* d, const char* ModelFilePath, const char* PSFilePath)
{
    //--------------------<���f���̏�����>--------------------//
    ModelRenderInitializer Ini{};
    Ini.SetALLPath(
        ModelFilePath,
        "./resources/Shaders/SkinnedMesh_VS.cso",
        PSFilePath,
        "./resources/Shaders/GeometryShader.cso"
    );

    //--------------------<�p���Ɋւ���\���̂̏�����>--------------------//


    t.Scale = {
     0.02f,
     0.02f,
     0.02f
    };

    r.Angle = {
        0.0f,
        DirectX::XMConvertToRadians(-180),
        0.0f
    };

    LoadModel(d, Ini);

    maxInvTimer = 0.2f;

    //--------------------<�����蔻������p��������>--------------------//aq
    hitAreaSize = { 5.5f,5.5f,5.5f };

    efk_WeakDamaged = EffectResourceManager::Instance().LoadEffect("./resources/Effects/enemy_hit.efk");
    efk_UnDamaged = EffectResourceManager::Instance().LoadEffect("./resources/Effects/enemy_hit_block.efk");
    efk_Bomb = EffectResourceManager::Instance().LoadEffect("./resources/Effects/bomb_2.efk");
    efk_MoreBomb = EffectResourceManager::Instance().LoadEffect("./resources/Effects/bomb_more.efk");

    se_WeakDamaged = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Enemy/HitPlus.wav");
    se_UnDamaged = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Enemy/HitMinus.wav");

}


void Enemy::Update(float elapsedTime)
{
    // ���� (�����A���G����)
    Behavior(elapsedTime);

    // �A�j���[�V�����̍X�V
    Model->Update(elapsedTime);

    // �����蔻�� (�v���C���[�̒e�A�G���A)
    Collision();
}


void Enemy::Render(ID3D11DeviceContext* dc)
{
    Model->Render(dc, t, r);
    
}


void Enemy::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //���G���ԍX�V
    invTimer -= elapsedTime;
}


void Enemy::Collision()
{
    //�e�Ƃ̓����蔻��
    VsProjectile();

    //�G���A�O�ɏo����폜

    // if ( -60 > pos.z > -100 ) Destroy();

    //if (t.Position.z >= -70.0f || t.Position.z <= -100.0f) return;
    if (t.Position.z >= -70.0f) return;
    Destroy();
}


bool Enemy::CanShot(float elapsedTime)
{
    status.shotTimer += elapsedTime;
    if (status.shotTimer > status.shotInterval)
    {
        status.shotTimer = 0.0f;
        return true;
    }
    return false;
}


void Enemy::BulletShot(float elapsedTime)
{
    if (type == Type::NONE) return;

    // �e�̐i�s����
    // �O����(���f����z������)�ɐi�ޏ���
    DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z).r[2];
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));

    // �e�̐���
    const BaseProjectile::Parent parent_type = (type == Type::RED) ? BaseProjectile::Parent::REDENEMY : BaseProjectile::Parent::BLUEENEMY;
    
    BaseProjectile* bullet = new Bullet(SceneManager::Instance().GetDevice(),
        parent_type,
        t.Position + forward * 1.5f,
        forward,
        status.speed * 2.0f);

    ProjectileManager::Instance().RegisterProjectile(bullet);
}


void Enemy::BulletTargetShot(float elapsedTime, const DirectX::XMFLOAT3 target)
{
    if (type == Type::NONE) return;

    // �e�̐i�s����
    DirectX::XMVECTOR Direction = Calcu3D::MakeVectorAtoB(t.Position, target, true);
    DirectX::XMFLOAT3 direction;
    DirectX::XMStoreFloat3(&direction, Direction);


    // �e�̐���
    const BaseProjectile::Parent parent_type = (type == Type::RED) ? BaseProjectile::Parent::REDENEMY : BaseProjectile::Parent::BLUEENEMY;

    BaseProjectile* bullet = new Bullet(SceneManager::Instance().GetDevice(),
        parent_type,
        t.Position,
        direction,
        status.speed * 1.5f);

    ProjectileManager::Instance().RegisterProjectile(bullet);
}


void Enemy::EnergyShot(float elapsedTime)
{
    if (type == Type::NONE) return;

    // �e�̐i�s����
    // �O����(���f����z������)�ɐi�ޏ���
    DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z).r[2];
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));


    // �e�̐���
    const BaseProjectile::Parent parent_type = (type == Type::RED) ? BaseProjectile::Parent::REDENEMY : BaseProjectile::Parent::BLUEENEMY;
    
    BaseProjectile* energy = new Energy(SceneManager::Instance().GetDevice(),
        parent_type,
        t.Position + forward * 1.5f,
        forward);

    ProjectileManager::Instance().RegisterProjectile(energy);
}


void Enemy::EnergyTargetShot(float elapsedTime, const DirectX::XMFLOAT3 target)
{
    if (type == Type::NONE) return;

    // �e�̐i�s����
    DirectX::XMVECTOR Direction = Calcu3D::MakeVectorAtoB(t.Position, target);
    DirectX::XMFLOAT3 direction;
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Normalize(Direction));


    // �e�̐���
    const BaseProjectile::Parent parent_type = (type == Type::RED) ? BaseProjectile::Parent::REDENEMY : BaseProjectile::Parent::BLUEENEMY;

    BaseProjectile* energy = new Energy(SceneManager::Instance().GetDevice(),
        parent_type,
        t.Position,
        direction);

    ProjectileManager::Instance().RegisterProjectile(energy);
}




void Enemy::Destroy()
{
    if (status.hitPoint <= 0.0f)
    {
        EnemyManager::Instance().Death(this);
        return;
    }
    EnemyManager::Instance().Remove(this);
}


void Enemy::VsProjectile()
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
                //ProjectileManager::Instance().Remove(&proj);
            }
        }
    }
}


bool Enemy::Damage(float damage, float invTimer,bool Finisehr)
{

    if(Finisehr)
    {
        efk_MoreBomb->Play(t.Position, {}, 0.25f);
    }

    if (this->invTimer <= 0.0f)
    {
        status.hitPoint -= damage;
        if (damage >= 1.0f)
        {
            if (!Finisehr)
            {
                efk_WeakDamaged->Play(t.Position, {}, 0.04f);
                se_WeakDamaged->Stop();
                se_WeakDamaged->Play(false, OptionSystem::Instance().GetSeVolume() * 0.75f);
            }
        }
        else
        {
            if (!Finisehr)
            {
                efk_UnDamaged->Play(t.Position, {}, 0.25f);
                se_UnDamaged->Stop();
                se_UnDamaged->Play(false, OptionSystem::Instance().GetSeVolume() * 0.5f);
            }
         
        }

        this->invTimer = invTimer;
        if (status.hitPoint <= 0.0f)
        {
            //�G�l���M�[����
            if (type == Type::RED)
            {
                Energy* energyRed = new Energy(SceneManager::Instance().GetDevice(), BaseProjectile::Parent::REDENEMY, t.Position, { 0,0,0 });
                ProjectileManager::Instance().RegisterProjectile(energyRed);
            }
            else if (type == Type::BLUE)
            {
                Energy* energyBlue = new Energy(SceneManager::Instance().GetDevice(), BaseProjectile::Parent::BLUEENEMY, t.Position, { 0,0,0 });
                ProjectileManager::Instance().RegisterProjectile(energyBlue);
            }
            Destroy();
            efk_Bomb->Play(t.Position, {}, 20.0f);
            GameSystem::Instance().KilledEnemy();
        }
        return true;
    }

    return false;
}

bool Enemy::HitProcessing(int projType)
{
    const int myType = static_cast<int>(type);

    float damage_ratio = 0.0f;

    //�G�l�~�[�̐F�ƒe�̐F�������ꍇ
    if (myType == projType)     // myType = {0: RED, 1: BLUE}, projType = {0: RED, 1: BLUE}
    {
        // �_���[�W������
        damage_ratio = 0.5f;
       
    }
    //�Ⴄ�ꍇ
    else
    {
        damage_ratio = 1.0f;
    }

    if (Damage(1.0f * damage_ratio, maxInvTimer))
    {
      


        return true;
    }
    return false;
}
