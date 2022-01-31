#include "Boss_3.h"
#include"GameSystem.h"

#include<imgui.h>

#include "Bullet.h"
#include"EnemyManager.h"
#include "Energy.h"
#include "ProjectileManager.h"
#include"MathSupport.h"
#include "SceneManager.h"
#include "StraightEnemy.h"

Boss_3::Boss_3(ID3D11Device* d)
    :Enemy(d, "./resources/Models/Characters/Boss/Boss2.nk", "./resources/Shaders/Boss_PBR_PS.cso")
{
    //--------------------<初期化>--------------------//
    Initialize();

    //--------------------<ライトのモデルを初期化>--------------------//
    ModelRenderInitializer Ini;
    Ini.SetALLPath(
        "./resources/Models/Characters/Boss/Laser.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/PhisicsBase_PS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );

    mBeamEmitter[0].Model = std::make_unique<ModelComponent>(d, Ini);
    mBeamEmitter[0].t = t;
    mBeamEmitter[0].r = r;
    mBeamEmitter[1].Model = std::make_unique<ModelComponent>(d, Ini);
    mBeamEmitter[1].t = t;
    mBeamEmitter[1].r = r;

    //--------------------<ポイントライト初期化>--------------------//
    mPointLight = std::make_unique<GamePointLight>(d, 4);
    mPointLight->SetColor({ 1.0f,0.0f,0.0f });
    
    //--------------------<イベントフラグの初期化>--------------------//
    laserFlag.Load("./resources/Models/Characters/Boss/BossLaserFlag.flg");

    // 登場
    EnemyManager::Instance().SetStartBossAppear(true);
    EnemyManager::Instance().SetStartBossBGM(true);
}

void Boss_3::Initialize()
{
    //--------------------<姿勢を初期化>--------------------//

    t.Position = { 0.0f,0.0f,90.0f };
    t.Scale = { 0.04f,0.04f,0.04f };
    r.Angle = { 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f };

    EnemyManager::Instance().SetCameraFocusBoss(true);
    EnemyManager::Instance().SetTargetPosition(t.Position);

    IsFirstMotion = true;


    status.hitPoint = MaxHp; // 体力の初期化
    IsInputChangeView = false;
    CompleteChangeView = true;
    CurrentAttackNumber = 0;

    LightColor = { 0.0f,0.0f,1.0f };

    // 状態を設定
    CurrentMode = TOP;
    type = Type::BLUE;

    maxInvTimer = 0.3f;

    ShowBeamEmitter = false;

    // 弾を一回全部消す
    ProjectileManager::Instance().Clear();



    //****************************************************************
    // 
    // 関数ポインタの初期化
    // 
    //****************************************************************

    std::vector<pBehavior> vec{};


    //--------------------<Topの攻撃>--------------------//
    vec.emplace_back(&Boss_3::T_SummonEnemies);
    vec.emplace_back(&Boss_3::T_RandomAllRangeAttack);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_3::T_ShotBulletToPlayer);
    vec.emplace_back(&Boss_3::T_SlowShotBullet);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_3::T_RandomAllRangeAttack);
    vec.emplace_back(&Boss_3::AppealTime);
    vec.emplace_back(&Boss_3::T_RandomAllRangeAttack);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_3::T_Energy);
    vec.emplace_back(&Boss_3::AppealTime);
    vec.emplace_back(&Boss_3::T_Energy);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();


    //--------------------<Sideの攻撃>--------------------//
    vec.emplace_back(&Boss_3::S_MoveFirstPosition);
    vec.emplace_back(&Boss_3::AppealTime);
    vec.emplace_back(&Boss_3::S_FastShot);
    vec.emplace_back(&Boss_3::S_SlowShot);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_3::S_FastShot);
    vec.emplace_back(&Boss_3::AppealTime);
    vec.emplace_back(&Boss_3::S_SlowShot);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_3::S_NagiriShot);
    vec.emplace_back(&Boss_3::S_SlowShot);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_3::S_ShotEnergyToPlayer);
    vec.emplace_back(&Boss_3::S_ShotEnergyToPlayer);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();


    //--------------------<発狂後Topの攻撃>--------------------//
    vec.emplace_back(&Boss_3::T_MoveFirstPosition);
    vec.emplace_back(&Boss_3::T_Energy);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();


    vec.emplace_back(&Boss_3::T_SummonEnemies);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_3::T_RandomAllRangeAttack);
    vec.emplace_back(&Boss_3::AppealTime);
    vec.emplace_back(&Boss_3::T_RandomAllRangeAttack);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();


    
    vec.emplace_back(&Boss_3::T_SlowShotBullet);
    vec.emplace_back(&Boss_3::T_SlowShotBullet);
    vec.emplace_back(&Boss_3::AppealTime);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();


    vec.emplace_back(&Boss_3::T_ShotBulletToPlayer);
    vec.emplace_back(&Boss_3::T_SlowShotBullet);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();



    vec.emplace_back(&Boss_3::T_MutualAttack);
    vec.emplace_back(&Boss_3::T_MutualAttack);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();


    vec.emplace_back(&Boss_3::T_Energy);
    vec.emplace_back(&Boss_3::AppealTime);
    vec.emplace_back(&Boss_3::T_Energy);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    //--------------------<発狂後Sideの攻撃>--------------------//

    vec.emplace_back(&Boss_3::S_MoveFirstPosition);
    vec.emplace_back(&Boss_3::AppealTime);
    vec.emplace_back(&Boss_3::S_FastShot);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[ANGRY_SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_3::S_SlowShot);
    vec.emplace_back(&Boss_3::S_WallBullet);
    vec.emplace_back(&Boss_3::AppealTime);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[ANGRY_SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_3::S_Beam);
    vec.emplace_back(&Boss_3::AppealTime);
    vec.emplace_back(&Boss_3::S_SlowShot);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[ANGRY_SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_3::S_NagiriShot);
    vec.emplace_back(&Boss_3::S_Cross);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[ANGRY_SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_3::S_ShotEnergyToPlayer);
    vec.emplace_back(&Boss_3::S_ShotEnergyToPlayer);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[ANGRY_SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_3::S_WallBullet);
    vec.emplace_back(&Boss_3::AppealTime);
    vec.emplace_back(&Boss_3::S_ShotEnergyToPlayer);
    vec.emplace_back(&Boss_3::EndAttack);
    BehaviorVec[ANGRY_SIDE].emplace_back(vec);
    vec.clear();
}

void Boss_3::Render(ID3D11DeviceContext* dc)
{
    Enemy::Render(dc);
    mPointLight->Update(dc);

    if (ShowBeamEmitter)
    {
        mBeamEmitter[0].Model->Render(dc, mBeamEmitter[0].t, mBeamEmitter[0].r);
        mBeamEmitter[1].Model->Render(dc, mBeamEmitter[1].t, mBeamEmitter[1].r);
    }
}

void Boss_3::Finalize()
{

}

void Boss_3::Behavior(float elapsed_time)
{
    //--------------------<最初の動き>--------------------//
    if (IsFirstMotion)
    {
        EnemyManager::Instance().SetCameraFocusBoss(true);
        FirstMotion(elapsed_time);
        return;
    }

    //無敵時間更新
    invTimer -= elapsed_time;

    ChangeView();

    if (IsPerformance)
    {
        ChangeViewPerformance(elapsed_time);
    }


    if (!IsPerformance && CompleteChangeView)
    {
         AttackManager(elapsed_time);
    }


    //--------------------<ポイントライトを更新>--------------------//
    mPointLight->SetPosition(t.Position);
    mPointLight->SetColor(LightColor);
    mPointLight->SetRange(LightRange);

    GuiMenu();
}

void Boss_3::Move(float elapsed_time)
{

}

void Boss_3::Collision()
{
    Enemy::Collision();
}

//--------------------<最初の動き>--------------------//
void Boss_3::FirstMotion(float elapsedTime)
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

void Boss_3::AttackManager(float elapsedTime)
{
    if (CompleteAttack)
    {
        switch (CurrentMode) {
        case TOP:
            CurrentAttackComboNumber = static_cast<int>(rand() % 4);
            CompleteAttack = false;
            break;
        case SIDE:
            CurrentAttackComboNumber = static_cast<int>(rand() % 3) + 1;
            CompleteAttack = false;
            break;
        case ANGRY_TOP:
            CurrentAttackComboNumber = static_cast<int>(rand() % 5) + 1;
            CompleteAttack = false;
            break;
        case ANGRY_SIDE:
            CurrentAttackComboNumber = static_cast<int>(rand() % 5) + 1;
            CompleteAttack = false;
            break;
        default:;
        }
    }

    (this->*BehaviorVec[CurrentMode][CurrentAttackComboNumber][CurrentAttackNumber])(elapsedTime);
}



void Boss_3::ChangeView()
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
                type = Type::RED;
                break;
            case SIDE:
                CurrentMode = ANGRY_TOP;
                type = Type::BLUE;
                break;
            case ANGRY_TOP:
                CurrentMode = ANGRY_SIDE;
                type = Type::RED;
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


void Boss_3::ChangeViewPerformance(float elapsedTime)
{
    EnemyManager::Instance().SetCameraFocusBoss(true);
    EnemyManager::Instance().SetTargetPosition(t.Position);

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




void Boss_3::GuiMenu()
{
#ifdef _DEBUG
    ImGui::Begin("Boss2");
    if (ImGui::Button("Next"))
    {
        status.hitPoint -= (MaxHp * 0.25f + 1.0f);
    }
    ImGui::Text(("HitPoint=" + to_string(status.hitPoint)).c_str());

    ImGui::Checkbox("CompleteAttack", &CompleteAttack);
    ImGui::End();
#endif

}



//****************************************************************
// 
//  関数ポインタ
// 
//****************************************************************

void Boss_3::T_MoveFirstPosition(float elapsedTime)
{
    t.Position.z -= Speed * elapsedTime;
    t.Position.y = 0.0f;
    if (t.Position.z < 20.0f)
    {
        CurrentAttackNumber++;
    }
}

void Boss_3::S_MoveFirstPosition(float elapsedTime)
{
    t.Position.z += Speed * elapsedTime;
    t.Position.x = 0.0f;
    if (t.Position.z > 40.0f)
    {
        CurrentAttackNumber++;
    }
}

void Boss_3::EndAttack(float elapsedTime)
{
    CompleteAttack = true;
    CurrentAttackNumber = 0;
}


void Boss_3::Shot_ShotRadian(const BaseProjectile::Parent p, const XMFLOAT3 Radian, DirectX::XMFLOAT3 pos)
{
    const DirectX::XMFLOAT3 front = Calcu3D::GetFrontVec(Radian);

    constexpr float BulletSpeed = 1.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    auto b0 = new Bullet(p_device, p,
        pos, front * BulletSpeed);
    ProjectileManager::Instance().RegisterProjectile(b0);

}

void Boss_3::Shot_ShotVector(const BaseProjectile::Parent p, const XMFLOAT3 Vec, DirectX::XMFLOAT3 pos)
{
    constexpr float BulletSpeed = 1.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    auto b0 = new Bullet(p_device, p,
        pos, Vec * BulletSpeed);
    ProjectileManager::Instance().RegisterProjectile(b0);

}

void Boss_3::Shot_ShotVectorFast(const BaseProjectile::Parent p, const DirectX::XMFLOAT3 Vec, DirectX::XMFLOAT3 pos)
{
    constexpr float BulletSpeed = 1.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    auto b0 = new Bullet(p_device, p,
        pos, Vec * BulletSpeed,40.0f);
    ProjectileManager::Instance().RegisterProjectile(b0);
}

void Boss_3::Shot_ShotVectorSlow(const BaseProjectile::Parent p, const DirectX::XMFLOAT3 Vec, DirectX::XMFLOAT3 pos)
{
    constexpr float BulletSpeed = 1.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    auto b0 = new Bullet(p_device, p,
        pos, Vec * BulletSpeed,20.0f);
    ProjectileManager::Instance().RegisterProjectile(b0);
}


void Boss_3::Shot_ShotRadianSlow(const BaseProjectile::Parent p, const XMFLOAT3 Radian, DirectX::XMFLOAT3 pos)
{
    const DirectX::XMFLOAT3 front = Calcu3D::GetFrontVec(Radian);

    constexpr float BulletSpeed = 20.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    auto b0 = new Bullet(p_device, p,
        pos, front, BulletSpeed);
    ProjectileManager::Instance().RegisterProjectile(b0);

}



void Boss_3::Shot_ShotRadianFast(const BaseProjectile::Parent p, const XMFLOAT3 Radian, DirectX::XMFLOAT3 pos)
{
    const DirectX::XMFLOAT3 front = Calcu3D::GetFrontVec(Radian);

    constexpr float BulletSpeed = 50.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    auto b0 = new Bullet(p_device, p,
        pos, front, BulletSpeed);
    ProjectileManager::Instance().RegisterProjectile(b0);

}

void Boss_3::Shot_EnergyRadian(const BaseProjectile::Parent p, const XMFLOAT3 Radian, DirectX::XMFLOAT3 pos)
{
    const DirectX::XMFLOAT3 front = Calcu3D::GetFrontVec(Radian);

    constexpr float BulletSpeed = 1.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    auto b0 = new Energy(p_device, p,
        pos, front * BulletSpeed);
    ProjectileManager::Instance().RegisterProjectile(b0);
}

void Boss_3::Shot_EnergyVector(const BaseProjectile::Parent p, const XMFLOAT3 Vec, DirectX::XMFLOAT3 pos)
{

    constexpr float BulletSpeed = 1.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    auto b0 = new Energy(p_device, p,
        pos, Vec * BulletSpeed);
    ProjectileManager::Instance().RegisterProjectile(b0);
}


void Boss_3::AppealTime(float elapsedTime)
{
    StackAttackInterval += elapsedTime;
    if(StackAttackInterval>=2.0f)
    {
        StackAttackInterval = 0.0f;
        CurrentAttackNumber++;
    }
}


void Boss_3::T_ShotEnergyToPlayer(float elapsedTime)
{

    switch (AttackState)
    {
    case 0:
    /*初期化*/

        // ターゲットを初期化
        target = ProjectileManager::Instance().GetTarget();

        AttackState++;
        /*FallThrough*/

    case 1:
        // 右に移動しながらプレイヤーの向きにエネルギーを打つ
        Ratio += elapsedTime;
        t.Position.x = Calcu3D::LerpFloat(0.0f, 20.0f, Ratio);
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-180.0f), DirectX::XMConvertToRadians(-540.0f), Ratio);

        StackAttackInterval += elapsedTime;
        if(StackAttackInterval>=0.2f)
        {
            // 向きを計算
            target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 ShotVec = target - t.Position;
            ShotVec = Calcu3D::Normalize(ShotVec);
            Shot_EnergyVector(BaseProjectile::Parent::BLUEENEMY, ShotVec, t.Position);
            StackAttackInterval = 0.0f;
        }
        
        if(Ratio>=1.0f)
        {
            r.Angle.y = DirectX::XMConvertToRadians(-540.0f);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        // 左に移動しながらプレイヤーの向きにエネルギーを打つ
        Ratio += elapsedTime;
        t.Position.x = Calcu3D::LerpFloat(20.0f, -20.0f, Ratio);
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-540.0f), DirectX::XMConvertToRadians(-180.0f), Ratio);

        StackAttackInterval += elapsedTime;
        if (StackAttackInterval >= 0.2f)
        {
            // 向きを計算
            target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 ShotVec = target - t.Position;
            ShotVec = Calcu3D::Normalize(ShotVec);
            Shot_EnergyVector(BaseProjectile::Parent::BLUEENEMY, ShotVec, t.Position);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 1.0f)
        {
            r.Angle.y = DirectX::XMConvertToRadians(-180.0f);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 3:
        // 中央に移動しながらプレイヤーの向きにエネルギーを打つ
        Ratio += elapsedTime;
        t.Position.x = Calcu3D::LerpFloat(-20.0f, 0.0f, Ratio);
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-180.0f), DirectX::XMConvertToRadians(-540.0f), Ratio);

        StackAttackInterval += elapsedTime;
        if (StackAttackInterval >= 0.2f)
        {
            // 向きを計算
            target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 ShotVec = target - t.Position;
            ShotVec = Calcu3D::Normalize(ShotVec);
            Shot_EnergyVector(BaseProjectile::Parent::BLUEENEMY, ShotVec, t.Position);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 1.0f)
        {
            r.Angle.y = DirectX::XMConvertToRadians(-180.0f);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 4:
        // 初期化
        CurrentAttackNumber++;
        AttackState = 0;

        break;
        default:
            break;
    }

}

void Boss_3::T_ShotBulletToPlayer(float elapsedTime)
{

     

    switch (AttackState)
    {
    case 0:
        // 初期化
        target = ProjectileManager::Instance().GetTarget();
        AttackState++;
    /*FallThrough*/
    case 1:
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;
        if (StackAttackInterval >= 0.3f)
        {
            // プレイヤーの向きのベクトルを中心に三回打つ
            // ターゲットを初期化
            target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 ShotVec = target - t.Position;
            ShotVec = Calcu3D::Normalize(ShotVec);
            float rad = Calcu3D::GetRadianTwoVectors({ ShotVec.x,ShotVec.z }, {});
            Shot_ShotVector(BaseProjectile::Parent::BLUEENEMY, ShotVec, t.Position);

            StackAttackInterval = 0.0f;
        }

        if(Ratio>=3.0f)
        {
            Ratio = 0.0f;
            StackAttackInterval = 0.0f;
            AttackState++;
        }

        break;
    case 2:
        /*初期化*/
        CurrentAttackNumber++;
        AttackState = 0;
        
        break;
        default:
            break;
    }

}


void Boss_3::T_MutualAttack(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        Ratio += elapsedTime * 1.5f;
        StackAttackInterval += elapsedTime;

        // 移動
        t.Position.x = Calcu3D::LerpFloat(0.0f, 30.0f, Ratio);

        // 射撃
        if(StackAttackInterval>=0.1f)
        {
            DirectX::XMFLOAT3 vec = Calcu3D::Normalize(ProjectileManager::Instance().GetTarget() - t.Position);
            Shot_ShotVector(BaseProjectile::Parent::BLUEENEMY, vec, t.Position);
            StackAttackInterval = 0.0f;
        }

        if(Ratio>=1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 1:
        Ratio += elapsedTime * 3.0f;
        StackAttackInterval += elapsedTime;

        // 移動
        t.Position.x = Calcu3D::LerpFloat(30.0f, -30.0f, Ratio);

        // 射撃
        if (StackAttackInterval >= 0.1f)
        {
            DirectX::XMFLOAT3 vec = Calcu3D::Normalize(ProjectileManager::Instance().GetTarget() - t.Position);
            Shot_EnergyVector(BaseProjectile::Parent::REDENEMY, vec, t.Position);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        // 戻る
        Ratio += elapsedTime * 3.0f;
        StackAttackInterval += elapsedTime;

        // 移動
        t.Position.x = Calcu3D::LerpFloat(-30.0f, 0.0f, Ratio);
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 3:
        CurrentAttackNumber++;
        AttackState = 0;

        break;
        default:
            break;
    }

}

void Boss_3::S_WallBullet(float elapsedTime)
{
    static int Num = 0;
    switch (AttackState)
    {
    case 0:
        StackAttackInterval += elapsedTime;
        if (StackAttackInterval >= 0.2f)
        {
            auto b1 = new CurveBullet(BaseProjectile::Parent::REDENEMY, t.Position, -1, 1);
            ProjectileManager::Instance().RegisterProjectile(b1);
            StackAttackInterval = 0.0f;
            Num++;
        }

        if(Num>=5)
        {
            Num = 0;
            AttackState++;
        }
        break;
    case 1:

        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;
        if(StackAttackInterval>0.1f)
        {
            auto b0 = new StayToTargetBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1, { static_cast<float>(rand()%40)-20.0f,0.0f,10.0f }, 1.3f);
            ProjectileManager::Instance().RegisterProjectile(b0);
            StackAttackInterval = 0.0f;
        }
        if(Ratio>=1.0f)
        {
            AttackState++;
            Ratio = 0.0f;
        }
        break;
    case 2:
        AttackState = 0;
        CurrentAttackNumber++;
        break;
    }
}

void Boss_3::S_Beam(float elapsedTime)
{
    //--------------------<ビーム発射>--------------------//
    switch (AttackState)
    {
    case 0:
    {
        ShowBeamEmitter = true;
        mBeamEmitter[0].Model->f_PlayAnimation(0);
        mBeamEmitter[1].Model->f_PlayAnimation(0);

        mBeamEmitter[0].t = t;
        mBeamEmitter[0].t.Position.y += 20.0f;

        mBeamEmitter[1].t = t;
        mBeamEmitter[1].t.Position.y -= 20.0f;

        Ratio = 0.0f;
        AttackState++;

        /*FallThrough*/


    }
    case 1:
        if (laserFlag.Check("ShotBeam", mBeamEmitter[0].Model->GetCurrentAnimationNumber(), mBeamEmitter[0].Model->GetCurrentAnimationFrame()))
        {
            AttackState++;
        }

        Ratio += elapsedTime;
        if (Ratio > 0.4f)
        {
            DirectX::XMFLOAT3 p = t.Position;
            float ang = static_cast<float>(rand() % 80) - 40.0f;
            if (shotRight)
            {
                p.y -= 10.0f;
            }
            else
            {
                p.y += 10.0f;
            }

            shotRight = !shotRight;

            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, { DirectX::XMConvertToRadians(ang),DirectX::XMConvertToRadians(180.0f),0.0f }, p);
            Ratio = 0.0f;
        }

        break;
    case 2:
    {

        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        DirectX::XMFLOAT3 p = mBeamEmitter[0].t.Position;
        auto b0 = new Beam(p_device, BaseProjectile::Parent::REDENEMY, p);
        ProjectileManager::Instance().RegisterProjectile(b0);

        p = mBeamEmitter[1].t.Position;
        auto b1 = new Beam(p_device, BaseProjectile::Parent::REDENEMY, p);
        ProjectileManager::Instance().RegisterProjectile(b1);

        Ratio = 0.0f;
        AttackState++;
    }
    case 3:
        if (laserFlag.Check("EndBeam", mBeamEmitter[0].Model->GetCurrentAnimationNumber(), mBeamEmitter[0].Model->GetCurrentAnimationFrame()))
        {
        }
        Ratio += elapsedTime;
        if (Ratio > 0.8f)
        {
            DirectX::XMFLOAT3 p = t.Position;
            float ang = static_cast<float>(rand() % 40) - 20.0f;
            if (shotRight)
            {
                p.y -= 10.0f;
            }
            else
            {
                p.y += 10.0f;
            }

            shotRight = !shotRight;

            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, { DirectX::XMConvertToRadians(ang),DirectX::XMConvertToRadians(180.0f),0.0f }, p);
            Ratio = 0.0f;
        }

        StackAttackInterval += elapsedTime;
        if (StackAttackInterval > 5.0f)
        {
            AttackState++;
        }
        break;
    case 4:
        CurrentAttackNumber++;
        AttackState = 0;
        StackAttackInterval = 0.0f;
        ShowBeamEmitter = false;
        break;
    default:;
    }
}


void Boss_3::T_SummonEnemies(float elapsedTime)
{

    StackAttackInterval += elapsedTime;
    if (StackAttackInterval >= 0.2f)
    {
        // エネルギー
        Shot_EnergyRadian(BaseProjectile::Parent::REDENEMY, r.Angle, t.Position);
        DirectX::XMFLOAT3 rad = r.Angle;
        rad.y += DirectX::XMConvertToRadians(45.0f);
        Shot_EnergyRadian(BaseProjectile::Parent::REDENEMY, rad, t.Position);
        rad.y -= DirectX::XMConvertToRadians(90.0f);
        Shot_EnergyRadian(BaseProjectile::Parent::REDENEMY, rad, t.Position);

        StackAttackInterval = 0.0f;
    }
   
    
    CurrentAttackNumber++;
}

void Boss_3::T_SummonEnemiesGreat(float elasedTime)
{
}

void Boss_3::T_RandomAllRangeAttack(float elapedTime)
{
    switch (AttackState)
    {
    case 0:
    {
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        const int Max = 12;
        // 全体に攻撃
        for (int i = 0; i < Max; i++)
        {
            // 合計が 360どになるように

            float Deg = static_cast<float>(i) * (360.0f / static_cast<float>(Max)); // Y軸の角度（ディグリー）
            if (Deg > 360.0f) Deg -= 360.0f;

            Deg = DirectX::XMConvertToRadians(Deg);
            const DirectX::XMFLOAT3 Vel = { sinf(Deg),0.0f,cosf(Deg) }; // 進行方向

            switch (rand()%3)
            {
            case 0:
                Shot_ShotVector(BaseProjectile::Parent::BLUEENEMY, Vel, t.Position);
                break;
            case 1:
                Shot_ShotVectorFast(BaseProjectile::Parent::BLUEENEMY, Vel, t.Position);
                break;
            case 2:
                Shot_ShotVectorSlow(BaseProjectile::Parent::BLUEENEMY, Vel, t.Position);
                break;
            default:
                break;
            }

        }
        AttackState++;
    }

    case 1:
    {
        // 終了
        AttackState = 0;
        CurrentAttackNumber++;
    }
    break;

    default:
        break;;
    }
}


void Boss_3::T_Energy(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        // 前方向に発射
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;
        if (StackAttackInterval >= 0.2f)
        {
            Shot_EnergyRadian(BaseProjectile::Parent::REDENEMY, r.Angle, t.Position);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 0.7f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 1:
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;
        if (StackAttackInterval >= 0.2f)
        {
            DirectX::XMFLOAT3 rad = r.Angle;
            rad.y -= DirectX::XMConvertToRadians(45.0f);
            Shot_EnergyRadian(BaseProjectile::Parent::REDENEMY, rad, t.Position);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 0.7f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;
        if (StackAttackInterval >= 0.2f)
        {
            DirectX::XMFLOAT3 rad = r.Angle;
            rad.y += DirectX::XMConvertToRadians(45.0f);
            Shot_EnergyRadian(BaseProjectile::Parent::REDENEMY, rad, t.Position);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 0.7f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 3:
        CurrentAttackNumber++;
        AttackState = 0;
        break;
    default:;
    }
}

void Boss_3::T_SlowShotBullet(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        // 初期化
        target = ProjectileManager::Instance().GetTarget();
        AttackState++;
        /*FallThrough*/
    case 1:
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;
        if (StackAttackInterval >= 0.3f)
        {
            // プレイヤーの向きのベクトルを中心に三回打つ
            // ターゲットを初期化
            target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 ShotVec = target - t.Position;
            ShotVec = Calcu3D::Normalize(ShotVec);
            float rad = Calcu3D::GetRadianTwoVectors({ ShotVec.x,ShotVec.z }, {});
            Shot_ShotVectorSlow(BaseProjectile::Parent::BLUEENEMY, ShotVec, t.Position);

            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 3.0f)
        {
            Ratio = 0.0f;
            StackAttackInterval = 0.0f;
            AttackState++;
        }

        break;
    case 2:
        /*初期化*/
        CurrentAttackNumber++;
        AttackState = 0;

        break;
    default:
        break;
    }

}


void Boss_3::S_FastShot(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        /*初期化*/
        AttackState++;
        /*FallThrough*/
    case 1:
        // 角に移動
        Ratio += elapsedTime;
        t.Position.y = Calcu3D::LerpFloat(0.0f, 20.0f, Ratio);
        r.Angle.x = Calcu3D::LerpFloat(0.0f, DirectX::XMConvertToRadians(45.0f), Ratio);
        if(Ratio>=1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime * 0.5f;
        DirectX::XMFLOAT3 ShotRadian = { 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };

       ShotRadian.x= Calcu3D::LerpFloat(DirectX::XMConvertToRadians(90.0f), DirectX::XMConvertToRadians(0.0f), Ratio);

        if (StackAttackInterval >= 0.2f)
        {
            Shot_ShotRadianFast(BaseProjectile::Parent::REDENEMY, ShotRadian, t.Position);
            StackAttackInterval = 0.0f;
        }
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            StackAttackInterval = 0.0f;
            AttackState++;
        }
        break;
    case 3:
        // 戻る
        Ratio += elapsedTime * 3.0f;
        t.Position.y = Calcu3D::LerpFloat(20.0f, 0.0f, Ratio);
        r.Angle.x = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(45.0f), 0.0f, Ratio);
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 4:
        CurrentAttackNumber++;
        AttackState = 0;
        break;
    }
}

void Boss_3::S_Cross(float elapsedTime)
{
    const float Begin = DirectX::XMConvertToRadians(90.0f);
    const float Begin2 = DirectX::XMConvertToRadians(-90.0f);

    const float End = 0.0f;

    Ratio += elapsedTime * 1.0f;
    StackAttackInterval += elapsedTime;

    if (StackAttackInterval > 0.3f)
    {
        DirectX::XMFLOAT3 Radian{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        DirectX::XMFLOAT3 Radian2{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        Radian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        Radian2.x = Calcu3D::LerpFloat(Begin2, End, Ratio);

        Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, Radian,t.Position);
        Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, Radian2,t.Position);

        StackAttackInterval = 0.0f;
    }

    if (Ratio > 1.0f)
    {
        Ratio = 0.0f;
        CurrentAttackNumber++;
        StackAttackInterval++;
    }
}

void Boss_3::S_SlowShot(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        /*初期化*/
        AttackState++;
        /*FallThrough*/
    case 1:
        // 角に移動
        Ratio += elapsedTime;
        t.Position.y = Calcu3D::LerpFloat(0.0f, -20.0f, Ratio);
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime * 0.5f;
        DirectX::XMFLOAT3 ShotRadian = { 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };

        ShotRadian.x = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(0.0f), DirectX::XMConvertToRadians(-90.0f), Ratio);

        if (StackAttackInterval >= 0.2f)
        {
            Shot_ShotRadianSlow(BaseProjectile::Parent::REDENEMY, ShotRadian, t.Position);
            StackAttackInterval = 0.0f;
        }
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            StackAttackInterval = 0.0f;
            AttackState++;
        }
        break;
    case 3:
        // 戻る
        Ratio += elapsedTime * 3.0f;
        t.Position.y = Calcu3D::LerpFloat(-20.0f, 0.0f, Ratio);
       
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 4:
        CurrentAttackNumber++;
        AttackState = 0;
        break;
    }
}

void Boss_3::S_SummonEnemyWall(float elapsedTime)
{

}

void Boss_3::S_NagiriShot(float elapsedTime)
{
    constexpr float Begin = { DirectX::XMConvertToRadians(-100.0f) };
    constexpr float End = { DirectX::XMConvertToRadians(100.0f) };

    constexpr float s = 1.2f;


    StackAttackInterval += elapsedTime;
    const bool Attack = StackAttackInterval >  0.3f;

    // 薙ぎ払い攻撃
    switch (AttackState)
    {
    case 0:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, ShotRadian,t.Position);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 1:
        // 薙ぎ払い攻撃
        Ratio -= elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, ShotRadian, t.Position);
            StackAttackInterval = 0.0f;
        }
        if (Ratio < 0.0f)
        {
            AttackState++;
        }
        break;
    case 2:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);

        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, ShotRadian, t.Position);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 3:
        // 終了
        CurrentAttackNumber++;
        AttackState = 0;
        Ratio = 0.0f;
        StackAttackInterval = 0.0f;
        ShotRadian = { 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        break;
    }
}

void Boss_3::S_ShotEnergyToPlayer(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        /*初期化*/

            // ターゲットを初期化
        target = ProjectileManager::Instance().GetTarget();

        AttackState++;
        /*FallThrough*/

    case 1:
        // 右に移動しながらプレイヤーの向きにエネルギーを打つ
        Ratio += elapsedTime;
        t.Position.y = Calcu3D::LerpFloat(0.0f, 20.0f, Ratio);
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-180.0f), DirectX::XMConvertToRadians(-540.0f), Ratio);

        StackAttackInterval += elapsedTime;
        if (StackAttackInterval >= 0.2f)
        {
            // 向きを計算
            target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 ShotVec = target - t.Position;
            ShotVec = Calcu3D::Normalize(ShotVec);
            Shot_EnergyVector(BaseProjectile::Parent::REDENEMY, ShotVec, t.Position);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 1.0f)
        {
            r.Angle.y = DirectX::XMConvertToRadians(-540.0f);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        // 左に移動しながらプレイヤーの向きにエネルギーを打つ
        Ratio += elapsedTime;
        t.Position.y = Calcu3D::LerpFloat(20.0f, -20.0f, Ratio);
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-540.0f), DirectX::XMConvertToRadians(-180.0f), Ratio);

        StackAttackInterval += elapsedTime;
        if (StackAttackInterval >= 0.2f)
        {
            // 向きを計算
            target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 ShotVec = target - t.Position;
            ShotVec = Calcu3D::Normalize(ShotVec);
            Shot_EnergyVector(BaseProjectile::Parent::BLUEENEMY, ShotVec, t.Position);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 1.0f)
        {
            r.Angle.y = DirectX::XMConvertToRadians(-180.0f);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 3:
        // 中央に移動しながらプレイヤーの向きにエネルギーを打つ
        Ratio += elapsedTime;
        t.Position.y = Calcu3D::LerpFloat(-20.0f, 0.0f, Ratio);
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-180.0f), DirectX::XMConvertToRadians(-540.0f), Ratio);

        StackAttackInterval += elapsedTime;
        if (StackAttackInterval >= 0.2f)
        {
            // 向きを計算
            target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 ShotVec = target - t.Position;
            ShotVec = Calcu3D::Normalize(ShotVec);
            Shot_EnergyVector(BaseProjectile::Parent::BLUEENEMY, ShotVec, t.Position);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 1.0f)
        {
            r.Angle.y = DirectX::XMConvertToRadians(-180.0f);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 4:
        // 初期化
        CurrentAttackNumber++;
        AttackState = 0;

        break;
    default:
        break;
    }

}

void Boss_3::S_MoveShot(float elaspedTime)
{
}
