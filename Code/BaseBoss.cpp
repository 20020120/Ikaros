#include "BaseBoss.h"

#include "Bullet.h"
#include "EnemyManager.h"
#include "Energy.h"
#include "GameSystem.h"
#include "ProjectileManager.h"
#include "SceneManager.h"

#include"User_Function.h"
#include"MathSupport.h"
#include "gAudio.h"
#include"OptionSystem.h"


void BaseBoss::Initialize()
{
    
}

void BaseBoss::Render(ID3D11DeviceContext* dc)
{
    Enemy::Render(dc);
}

void BaseBoss::Finalize()
{
    
}

BaseBoss::BaseBoss(ID3D11Device* d, const char* ModelFilePath, const char* PSFilePath)
    :Enemy(d,ModelFilePath,PSFilePath)
{
    IsDraw = true;
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Enemy/Shout.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/Shot4.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/BossShot.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Enemy/LaserCharge.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Enemy/LaserBeam.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Enemy/Shout2.wav"));
}

BaseBoss::~BaseBoss()
{
    
}

void BaseBoss::T_MoveFirstPosition(float elapsedTime)
{
    t.Position.z -= Speed * elapsedTime;
    t.Position.y = 0.0f;
    if (t.Position.z < 20.0f)
    {
        CurrentAttackNumber++;
    }
}

void BaseBoss::S_MoveFirstPosition(float elapsedTime)
{
    t.Position.z += Speed * elapsedTime;
    t.Position.x = 0.0f;
    if (t.Position.z > 40.0f)
    {
        CurrentAttackNumber++;
    }
}

void BaseBoss::EndAttack(float elapsedTime)
{
    CompleteAttack = true;
    CurrentAttackNumber = 0;
}



void BaseBoss::FirstMotion(float elapsedTime)
{
    // ボスのほうにカメラを向けてボス登場モーション
    StackTimer += elapsedTime;
    

    // それが終わったら
    if (StackTimer > 5.0f)
    {
        t.Position.z -= 100.0f * elapsedTime;
        r.Angle = { 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f };
    }

    if (t.Position.z < 20.0f)
    {
        IsFirstMotion = false;
        hitAreaSize = { 10.0f,10.0f,10.0f };
        CompleteAttack = false;
        CurrentAttackComboNumber = 0;
        StackTimer = 0.0f;
    }

}

void BaseBoss::ShotEnergyShotRadian(BaseProjectile::Parent p, DirectX::XMFLOAT3 Radian) const
{
    const DirectX::XMFLOAT3 front = GetShotVec(Radian);

    constexpr float BulletSpeed = 1.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();

    se[RED_BULLET_SHOT]->Stop();
    se[RED_BULLET_SHOT]->Play(false, OptionSystem::Instance().GetSeVolume() * ShotVolume2);

    // 前・左右斜めに発射
    auto b0 = new Energy(p_device, p,
        t.Position, front * BulletSpeed);
    ProjectileManager::Instance().RegisterProjectile(b0);
}

void BaseBoss::AppealTime(float elapsedTime)
{

    StackAttackInterval += elapsedTime;
    if (StackAttackInterval >= 2.0f)
    {
        StackAttackInterval = 0.0f;
        CurrentAttackNumber++;
    }
}

    void BaseBoss::AppealTime2(float elapsedTime)
    {

        StackAttackInterval += elapsedTime;
        if (StackAttackInterval >= 0.5f)
        {
            StackAttackInterval = 0.0f;
            CurrentAttackNumber++;
        }
    }
void BaseBoss::ChangeView(float elapsedTime)
{

    // 視点変更完了後のⅠフレーム処理
    if (!CompleteChangeView && !GameSystem::Instance().GetChangingView())
    {
        CurrentAttackComboNumber = 0;
        CompleteChangeView = true;
        CompleteAttack = false;
        CurrentAttackNumber = 0;
    }

    if (CompleteAttack && CompleteChangeView)
    {
        if (
            (CurrentMode == TOP && status.hitPoint < MaxHp * 0.75f) ||
            (CurrentMode == SIDE && status.hitPoint < MaxHp * 0.5f) ||
            (CurrentMode == ANGRY_TOP && status.hitPoint < MaxHp * 0.25f)
            )
        {
            IsPerformance = true;
            r.Angle = { 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f };

            switch (CurrentMode) {
            case TOP:
                CurrentMode = SIDE;
                type = Type::BLUE;
                break;
            case SIDE:
                type = Type::RED;
                CurrentMode = ANGRY_TOP;
                break;
            case ANGRY_TOP:
                type = Type::BLUE;
                CurrentMode = ANGRY_SIDE;
                break;
            case ANGRY_SIDE:
                break;
            default:;
            }
        }
    }



    if (IsInputChangeView)
    {
        GameSystem::Instance().ChangeView();
        IsInputChangeView = false;
        CompleteChangeView = false;
    }
}


void BaseBoss::ChangeViewPerformance(float elapsedTime)
{
    EnemyManager::Instance().SetCameraFocusBoss(true);
    EnemyManager::Instance().SetTargetPosition(t.Position);
    ProjectileManager::Instance().Clear();
    switch (CurrentMode) {
    case TOP:

        break;
    case SIDE:

        break;
    case ANGRY_TOP:

        break;
    case ANGRY_SIDE:

        break;
    default:;
    }


    StackTimer += elapsedTime;
    if (StackTimer >= 3.0f)
    {
        IsPerformance = false;
        IsInputChangeView = true;
        StackTimer = 0.0f;
    }
}


void BaseBoss::Shot_ShotRadian(const BaseProjectile::Parent p, const XMFLOAT3 Radian, DirectX::XMFLOAT3 pos)
{
    const DirectX::XMFLOAT3 front = GetShotVec(Radian);

    constexpr float BulletSpeed = 1.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射

    se[BLUE_BULLET_SHOT]->Stop();
    se[BLUE_BULLET_SHOT]->Play(false,
        OptionSystem::Instance().GetSeVolume() * ShotVolume);

    auto b0 = new Bullet(p_device, p,
        pos, front * BulletSpeed);
    ProjectileManager::Instance().RegisterProjectile(b0);

}

void BaseBoss::Shot_ShotRadianSlow(const BaseProjectile::Parent p, const XMFLOAT3 Radian, DirectX::XMFLOAT3 pos)
{
    const DirectX::XMFLOAT3 front = GetShotVec(Radian);

    constexpr float BulletSpeed = 20.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射

    se[BLUE_BULLET_SHOT]->Stop();
    se[BLUE_BULLET_SHOT]->Play(false,
        OptionSystem::Instance().GetSeVolume() * ShotVolume);
    auto b0 = new Bullet(p_device, p,
        pos, front, BulletSpeed);
    ProjectileManager::Instance().RegisterProjectile(b0);

}

void BaseBoss::Shot_ShotRadianFast(const BaseProjectile::Parent p, const XMFLOAT3 Radian, DirectX::XMFLOAT3 pos)
{
    const DirectX::XMFLOAT3 front = GetShotVec(Radian);

    constexpr float BulletSpeed = 50.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    se[BLUE_BULLET_SHOT]->Stop();
    se[BLUE_BULLET_SHOT]->Play(false,
        OptionSystem::Instance().GetSeVolume() * ShotVolume);
    auto b0 = new Bullet(p_device, p,
        pos, front, BulletSpeed);
    ProjectileManager::Instance().RegisterProjectile(b0);

}



DirectX::XMFLOAT3 BaseBoss::GetShotVec(const DirectX::XMFLOAT3 rad)const
{
    const DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(rad.x, rad.y, rad.z);
    DirectX::XMFLOAT3 front{};
    DirectX::XMVector3Normalize(transform.r[2]);
    DirectX::XMStoreFloat3(&front, transform.r[2]);
    return front;
}

void BaseBoss::T_ShotHoming(int FirstVec,float Ratio)
{
    // ホーミング

    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    HomingBullet* b0;
    if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
    {

        se[BLUE_BULLET_SHOT]->Stop();
        se[BLUE_BULLET_SHOT]->Play(false,
            OptionSystem::Instance().GetSeVolume() * ShotVolume);
        b0 = new HomingBullet(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, FirstVec, Ratio);
    }
    else
    {
        se[BLUE_BULLET_SHOT]->Stop();
        se[BLUE_BULLET_SHOT]->Play(false,
            OptionSystem::Instance().GetSeVolume() * ShotVolume);
        b0 = new HomingBullet(p_device, BaseProjectile::Parent::REDENEMY,
            t.Position, FirstVec, Ratio);
    }

    ProjectileManager::Instance().RegisterProjectile(b0);

}

void BaseBoss::T_ShotHoming2(int FirstVec)
{
    // ホーミング

    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    HomingBullet* b0;
    if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
    {
        se[RED_BULLET_SHOT]->Stop();
        se[RED_BULLET_SHOT]->Play(false, OptionSystem::Instance().GetSeVolume()* ShotVolume2);
        b0 = new HomingBullet(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, FirstVec, 7.0f);
    }
    else
    {
        se[BLUE_BULLET_SHOT]->Stop();
        se[BLUE_BULLET_SHOT]->Play(false,
            OptionSystem::Instance().GetSeVolume() * ShotVolume);
        b0 = new HomingBullet(p_device, BaseProjectile::Parent::REDENEMY,
            t.Position, FirstVec, 7.0f);
    }

    ProjectileManager::Instance().RegisterProjectile(b0);

}




void BaseBoss::Shot_ShotVector(const BaseProjectile::Parent p, const XMFLOAT3 Vec, DirectX::XMFLOAT3 pos)
{
    constexpr float BulletSpeed = 1.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    se[BLUE_BULLET_SHOT]->Stop();
    se[BLUE_BULLET_SHOT]->Play(false,
        OptionSystem::Instance().GetSeVolume() * ShotVolume);
    auto b0 = new Bullet(p_device, p,
        pos, Vec * BulletSpeed);
    ProjectileManager::Instance().RegisterProjectile(b0);

}

void BaseBoss::Shot_ShotVectorFast(const BaseProjectile::Parent p, const DirectX::XMFLOAT3 Vec, DirectX::XMFLOAT3 pos)
{
    constexpr float BulletSpeed = 1.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    se[BLUE_BULLET_SHOT]->Stop();
    se[BLUE_BULLET_SHOT]->Play(false,
        OptionSystem::Instance().GetSeVolume() * ShotVolume);
    auto b0 = new Bullet(p_device, p,
        pos, Vec * BulletSpeed, 40.0f);
    ProjectileManager::Instance().RegisterProjectile(b0);
}

void BaseBoss::Shot_ShotVectorSlow(const BaseProjectile::Parent p, const DirectX::XMFLOAT3 Vec, DirectX::XMFLOAT3 pos)
{
    constexpr float BulletSpeed = 1.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    se[BLUE_BULLET_SHOT]->Stop();
    se[BLUE_BULLET_SHOT]->Play(false,
        OptionSystem::Instance().GetSeVolume() * ShotVolume);
    auto b0 = new Bullet(p_device, p,
        pos, Vec * BulletSpeed, 20.0f);
    ProjectileManager::Instance().RegisterProjectile(b0);
}