#include "Boss_5.h"

#include "Bullet.h"
#include "EnemyManager.h"
#include "GameSystem.h"
#include "ProjectileManager.h"

void Boss_5::Behavior(float elapsed_time)
{
    //--------------------<最初の動き>--------------------//
    if (IsFirstMotion)
    {
        EnemyManager::Instance().SetCameraFocusBoss(true);
        FirstMotion(elapsed_time);
        return;
    }

    // もし体力が１いかになったら
    if (status.hitPoint <= 1.0f)
    {
        status.hitPoint = 1.0f;
        efk_Spark->Stop(hdl_Spark);
        efk_Smoke->Stop(hdl_Smoke);
        EnemyManager::Instance().SetIsLastPerformance(true);
        LastMotion(elapsed_time);
        return;
    }

    // 色補完
    switch (type) {
    case Type::NONE: break;
    case Type::RED:
        LightColor = Calcu3D::LerpFloat3(LightColor, { 1.0f,0.0f,0.0f }, 5.0f * elapsed_time);
        break;
    case Type::BLUE:
        LightColor = Calcu3D::LerpFloat3(LightColor, { 0.0f,0.0f,1.0f }, 5.0f * elapsed_time);
        break;
    default:;
    }

    ChangeView();

    if (IsPerformance)
    {
        ChangeViewPerformance(elapsed_time);
    }

    if (!IsPerformance && CompleteChangeView)
    {
        AttackManager(elapsed_time);
    }

    if(!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(AnimationName::stand, true);
    }

    if (mAnimationFlag.Check("NextPerform", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(AnimationName::get_rib_of_loop);
    }

    mBeamEmitter[0].Model->Update(elapsed_time);
    mBeamEmitter[1].Model->Update(elapsed_time);

    //無敵時間更新
    invTimer -= elapsed_time;

    PlayEffects(elapsed_time);

    //--------------------<ポイントライトを更新>--------------------//
    mPointLight->SetPosition(t.Position);
    mPointLight->SetColor(LightColor);
    mPointLight->SetRange(LightRange);


    GuiMenu();
}

void Boss_5::Move(float elapsed_time)
{
    BaseBoss::Move(elapsed_time);
}

void Boss_5::Collision()
{
    if (!CompleteChangeView) return;

    //****************************************************************
    // 
    // モーション待ち中の体力管理
    // 
    //****************************************************************
    if (CurrentMode == TOP && status.hitPoint < MaxHitPoint * 0.5f)
    {
        return;
    }


    VsProjectile();
}


Boss_5::Boss_5(ID3D11Device* d)
    :BaseBoss(d, "./resources/Models/Characters/Boss/BossWhite.nk", "./resources/Shaders/Boss_PBR_PS.cso")
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

    //--------------------<イベントフラグの初期化>--------------------//
    laserFlag.Load("./resources/Models/Characters/Boss/BossLaserFlag.flg");
    mAnimationFlag.Load("./resources/Models/Characters/Boss/Boss1White.flg");

    //--------------------<ポイントライト初期化>--------------------//
    mPointLight = std::make_unique<GamePointLight>(d, 4);
    mPointLight->SetColor({ 1.0f,0.0f,0.0f });

    // 登場
    EnemyManager::Instance().SetStartBossAppear(true);
    EnemyManager::Instance().SetStartBossBGM(true);

    status.hitPoint = static_cast<float>(MaxHitPoint);


    // エフェクトを初期化
    efk_Angry = std::make_unique<Effect>("./resources/Effects/angry.efk");
    efk_Smoke = std::make_unique<Effect>("./resources/Effects/smoke.efk");
    efk_Spark = std::make_unique<Effect>("./resources/Effects/plasma.efk");
    efk_LastBomb = std::make_unique<Effect>("./resources/Effects/bomb_more.efk");
}


Boss_5::~Boss_5()
{
}

void Boss_5::Initialize()
{
    //--------------------<姿勢を初期化>--------------------//

    t.Position = { 0.0f,0.0f,90.0f };
    t.Scale = { 0.04f,0.04f,0.04f };
    r.Angle = { 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f };

    EnemyManager::Instance().SetCameraFocusBoss(true);
    EnemyManager::Instance().SetTargetPosition(t.Position);

    IsFirstMotion = true;

    shotRight = false;

    status.hitPoint = MaxHp; // 体力の初期化
    IsInputChangeView = false;
    CompleteChangeView = true;
    CurrentAttackNumber = 0;
    StackAttackInterval = 0.0f;
    CurrentAttackComboNumber = 0;
    ShotRadian = {};
    Ratio = {};


    //LightRange = { 0.0f };
    //LightColor = { 1.0f,0.0f,0.0f };

    type = Type::RED;

    //****************************************************************
    // 
    // 関数ポインタ
    // 
    //****************************************************************

    EnemyManager::Instance().SetStartBossAppear(true);

    std::vector<pBehavior> vec{};

    //--------------------<Topの攻撃>--------------------//

    vec.emplace_back(&Boss_5::T_Energy);
    vec.emplace_back(&Boss_5::T_Shot);
    vec.emplace_back(&Boss_5::AppealTime2);
    vec.emplace_back(&Boss_5::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_5::T_DiffuseBullet);
    vec.emplace_back(&Boss_5::T_Energy);
    vec.emplace_back(&Boss_5::T_Shot);
    vec.emplace_back(&Boss_5::AppealTime2);
    vec.emplace_back(&Boss_5::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_5::T_V_Homing);
    vec.emplace_back(&Boss_5::AppealTime2);
    vec.emplace_back(&Boss_5::T_DiffuseBullet);
    vec.emplace_back(&Boss_5::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_5::T_Shot);
    vec.emplace_back(&Boss_5::T_V_Homing);
    vec.emplace_back(&Boss_5::T_Shot);
    vec.emplace_back(&Boss_5::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    //--------------------<Sideの攻撃>--------------------//

    vec.emplace_back(&Boss_5::S_MoveFirstPosition);
    vec.emplace_back(&Boss_5::S_Shot);
    vec.emplace_back(&Boss_5::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_5::S_DiffuseShot);
    vec.emplace_back(&Boss_5::T_V_Homing);
    vec.emplace_back(&Boss_5::AppealTime2);
    vec.emplace_back(&Boss_5::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_5::S_DiffuseShot);
    vec.emplace_back(&Boss_5::S_Energy);
    vec.emplace_back(&Boss_5::AppealTime2);
    vec.emplace_back(&Boss_5::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_5::S_DiffuseShot);
    vec.emplace_back(&Boss_5::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_5::AppealTime2);
    vec.emplace_back(&Boss_5::T_V_Homing);
    vec.emplace_back(&Boss_5::S_Energy);
    vec.emplace_back(&Boss_5::S_Shot);
    vec.emplace_back(&Boss_5::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_5::T_V_Homing);
    vec.emplace_back(&Boss_5::S_Energy);
    vec.emplace_back(&Boss_5::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    status.hitPoint = static_cast<float>(MaxHitPoint);
}

void Boss_5::Render(ID3D11DeviceContext* dc)
{
    mPointLight->Update(dc);

    if (IsDraw)
    {
        Enemy::Render(dc);
    }
    if (ShowBeamEmitter)
    {
        mBeamEmitter[0].Model->Render(dc, mBeamEmitter[0].t, mBeamEmitter[0].r);
        mBeamEmitter[1].Model->Render(dc, mBeamEmitter[1].t, mBeamEmitter[1].r);
    }
}

void Boss_5::Finalize()
{
    BaseBoss::Finalize();
}

void Boss_5::LastMotion(float elapsedTime)
{
    const int State = EnemyManager::Instance().GetLastPerformanceState();
    if (State == 2)
    {
        t.Position = { 0.0f,0.0f,60.0f };
        IsDraw = false;
    }
    else if (State == 6)
    {
        IsDraw = true;
    }
    else if (State == 8)
    {
        Model->f_PlayAnimation(AnimationName::defeat);
    }
    else if (State == 9)
    {
        if (!LastBomb)
        {
            // エフェクト
            hdl_LastBomb = efk_LastBomb->Play(t.Position, {}, 0.4f);
        }
    }
}



void Boss_5::AttackManager(float elapsedTime)
{

    if (CompleteAttack)
    {
        switch (CurrentMode) {
        case TOP:
            CurrentAttackComboNumber = rand() % 4;
            CompleteAttack = false;
            break;
        case SIDE:
            CurrentAttackComboNumber = rand() % 5;
            CompleteAttack = false;
            break;
        }
    }

    (this->*BehaviorVec[CurrentMode][CurrentAttackComboNumber][CurrentAttackNumber])(elapsedTime);
}

void Boss_5::GuiMenu()
{
#ifdef _DEBUG
    ImGui::Begin("Boss2");
    if (ImGui::Button("Next"))
    {
        status.hitPoint -= (MaxHitPoint * 0.25f + 1.0f);
    }
    ImGui::Text(("HitPoint=" + to_string(status.hitPoint)).c_str());

    ImGui::Checkbox("CompleteAttack", &CompleteAttack);

    ImGui::DragFloat3("Emitter0Pos", &mBeamEmitter[0].t.Position.x);
    ImGui::DragFloat3("Emitter1Pos", &mBeamEmitter[1].t.Position.x);
    ImGui::End();
#endif
}

void Boss_5::ChangeView()
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
            (CurrentMode == TOP && status.hitPoint < MaxHitPoint * 0.5f)
            )
        {
            IsPerformance = true;
            r.Angle = { 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f };
                CurrentMode = SIDE;
                type = Type::BLUE;
                Model->f_PlayAnimation(AnimationName::get_rib_of_begein);
                se[0]->Play(false);
        }
    }



    if (IsInputChangeView)
    {
        GameSystem::Instance().ChangeView();
        IsInputChangeView = false;
        CompleteChangeView = false;
    }
}

void Boss_5::T_Shot(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::overrall);
        AttackState++;
        /*FallThrough*/
    case 1:
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;

        if (StackAttackInterval >= 0.4f)
        {
            auto b0 = new StayToTargetBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1, { -20.0f,0.0f,10.0f });
            ProjectileManager::Instance().RegisterProjectile(b0);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 2.0f)
        {
            Ratio = 0.0f;
            StackAttackInterval = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        AttackState = 0;
        CurrentAttackNumber++;
        break;
    }
    
}

void Boss_5::T_DiffuseBullet(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::overrall);
        AttackState++;
        /*FallThrough*/
    case 1:
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;
        if (StackAttackInterval >= 0.3f)
        {
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle, t.Position);
            DirectX::XMFLOAT3 rad = r.Angle;
            rad.y += DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, rad, t.Position);
            rad.y -= DirectX::XMConvertToRadians(90.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, rad, t.Position);
            StackAttackInterval = 0.0f;
        }
        if (Ratio >= 0.7f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        CurrentAttackNumber++;
        AttackState = 0;
        break;
    default: ;
    }

    
}

void Boss_5::T_V_Homing(float elapsedTime)
{
    static int Count = 0;

    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::homing);
        AttackState++;
        /*FallThrough*/
    case 1:
        StackAttackInterval += elapsedTime;
        if (StackAttackInterval > 0.15f)
        {
            T_ShotHoming(1);
            T_ShotHoming(-1);
            StackAttackInterval = 0.0f;
            Count++;
        }
        if (Count > 3)
        {
            Count = 0;
            StackAttackInterval = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        Ratio = 0.0f;
        AttackState = 0;
        CurrentAttackNumber++;
        break;
    default: ;
    }

    
}

void Boss_5::S_Shot(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        t.Position.y += 100.0f * elapsedTime;
        if (t.Position.y >= 50.0f)
        {
            t.Position = { -20.0f,50.0f,0.0f };
            r.Angle = { 0.0f,DirectX::XMConvertToRadians(90.0f),0.0f };
            AttackState++;
        }
        break;
    case 1:
        t.Position.y -= 20.0f * elapsedTime;
        if (t.Position.y <= 10.0f)
        {
            t.Position.y = 10.0f;
            AttackState++;
            Model->f_PlayAnimation(AnimationName::angry);
        }
        break;
    case 2:
    {
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;

        if (Ratio >= 0.4f)
        {

            

            auto b1 = new StayToTargetBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1, { 0.0f,20.0f,-10.0f }, 1.6f);
            ProjectileManager::Instance().RegisterProjectile(b1);

            auto b2 = new StayToTargetBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1, { 0.0f,10.0f,-5.0f });
            ProjectileManager::Instance().RegisterProjectile(b2);

            auto b3 = new StayToTargetBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1, { 0.0f,0.0f,0.0f });
            ProjectileManager::Instance().RegisterProjectile(b3);

            

            auto b5 = new StayToTargetBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1, { 0.0f,-20.0f,-10.0f }, 1.6f);
            ProjectileManager::Instance().RegisterProjectile(b5);

            auto b6 = new StayToTargetBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1, { 0.0f,-30.0f,-5.0f }, 1.9f);
            ProjectileManager::Instance().RegisterProjectile(b6);


            Ratio = 0.0f;
        }

        if (StackAttackInterval >= 1.0f)
        {
            Ratio = 0.0f;
            StackAttackInterval = 0.0f;
            AttackState++;
        }
        break;
    }
    case 3:
        t.Position.y += 100.0f * elapsedTime;
        if (t.Position.y >= 70.0f)
        {
            t.Position = { 0.0f,70.0f,40.0f };
            r.Angle = { 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
            AttackState++;
        }
        break;
    case 4:
        t.Position.y -= 20.0f * elapsedTime;
        if (t.Position.y <= 00.0f)
        {
            t.Position.y = 0.0f;
            AttackState++;
        }
        break;
    case 5:
        AttackState = 0;
        CurrentAttackNumber++;
        Ratio = 0.0f;
        break;
    }
    

}

void Boss_5::T_Energy(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::change_camera);
        AttackState++;
        /*FallThrough*/
    case 1:
        // 前方向に発射
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;
        if (StackAttackInterval >= 0.2f)
        {
            ShotEnergyShotRadian(BaseProjectile::Parent::BLUEENEMY, r.Angle);
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
            rad.y -= DirectX::XMConvertToRadians(45.0f);
            ShotEnergyShotRadian(BaseProjectile::Parent::REDENEMY, rad);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 0.7f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 3:
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;
        if (StackAttackInterval >= 0.2f)
        {
            DirectX::XMFLOAT3 rad = r.Angle;
            rad.y += DirectX::XMConvertToRadians(45.0f);
            ShotEnergyShotRadian(BaseProjectile::Parent::BLUEENEMY, rad);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 0.7f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 4:
        CurrentAttackNumber++;
        AttackState = 0;
        break;
    default:;
    }
}


void Boss_5::S_Energy(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::change_camera);
        AttackState++;
        /*FallThrough*/
    case 1:
        // 前方向に発射
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;
        if (StackAttackInterval >= 0.2f)
        {
            ShotEnergyShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle);
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
            rad.x -= DirectX::XMConvertToRadians(45.0f);
            ShotEnergyShotRadian(BaseProjectile::Parent::BLUEENEMY, rad);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 0.7f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 3:
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;
        if (StackAttackInterval >= 0.2f)
        {
            DirectX::XMFLOAT3 rad = r.Angle;
            rad.x += DirectX::XMConvertToRadians(45.0f);
            ShotEnergyShotRadian(BaseProjectile::Parent::REDENEMY, rad);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 0.7f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 4:
        CurrentAttackNumber++;
        AttackState = 0;
        break;
    default:;
    }
}

void Boss_5::S_DiffuseShot(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::overrall);
        AttackState++;
        /*FallThrough*/
    case 1:
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;
        if (StackAttackInterval >= 0.3f)
        {
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle, t.Position);
            DirectX::XMFLOAT3 rad = r.Angle;
            rad.x += DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, rad, t.Position);
            rad.x -= DirectX::XMConvertToRadians(90.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, rad, t.Position);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 0.7f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        AttackState = 0;
        CurrentAttackNumber++;
        break;
    default: ;
    }

    
}

void Boss_5::FirstMotion(float elapsedTime)
{
    // ボスのほうにカメラを向けてボス登場モーション
    ProjectileManager::Instance().Clear();
    ZoomPower += elapsedTime;
    if (ZoomPower >= 1.5f)
    {
        if (!ZoomUp)
        {
            ZoomPower -= elapsedTime * 100.0f;
            ZoomPower = std::max(ZoomPower, 0.0f);
        }
        else
        {
            ZoomPower += elapsedTime * 100.0f;
            ZoomPower = std::min(ZoomPower, 100.0f);
            if (ZoomPower >= 100.0f)
            {
                ZoomUp = false;
            }
        }
    }
    EnemyManager::Instance().fSetZoomPower(ZoomPower);



    if (!Model->GetIsPlayAnimation()&& StackTimer <= 10.0f)
    {
        Model->f_PlayAnimation(AnimationName::angry);

        se[0]->Play(false);
        DirectX::XMFLOAT3 launtchPos = t.Position;
        launtchPos.y -= 10.0f;
        hdl_Angry = efk_Angry->Play(launtchPos, {}, 0.4f);
    }

    if (mAnimationFlag.Check("EndStart", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        StackTimer = 20.0f;
        EnemyManager::Instance().fSetZoomPower(0.0f); ZoomPower += elapsedTime;
    }


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


void Boss_5::PlayEffects(float elapsedTime)
{

    //--------------------<エフェクトの状態を遷移させる>--------------------//
    if (mEffectState == EffectState::None && status.hitPoint <= MaxHitPoint * 0.75f)
    {
        mEffectState = EffectState::Min;
    }
    else if (mEffectState == EffectState::Min && status.hitPoint <= MaxHitPoint * 0.5f)
    {
        mEffectState = EffectState::Middle;
        efk_Spark->Stop(hdl_Spark);
        efk_Smoke->Stop(hdl_Smoke);
    }
    else if (mEffectState == EffectState::Middle && status.hitPoint <= MaxHitPoint * 0.25f)
    {
        mEffectState = EffectState::Max;
    }

    StackSmokeEffectInterVal += elapsedTime;
    StackSmokeEffectSeconds += elapsedTime;
    StackSparkEffectInterVal += elapsedTime;
    StackSparkEffectInterVal += elapsedTime;

    //--------------------<状態に応じたエフェクトを再生する>--------------------//

    switch (mEffectState)
    {
    case EffectState::None:
        break;
    case EffectState::Min:
        if (StackSmokeEffectInterVal >= 7.0f)
        {
            DirectX::XMFLOAT3 launchPos{ t.Position };
            launchPos.x += static_cast<float>(rand() % 10) - 5.0f;
            hdl_Smoke = efk_Smoke->Play(launchPos, {}, 0.2f);
            StackSmokeEffectInterVal = 0.0f;
            StackSmokeEffectSeconds = 0.0f;
        }
        break;
    case EffectState::Middle:

        if (StackSmokeEffectInterVal >= 7.0f)
        {

            hdl_Smoke = efk_Smoke->Play(t.Position, {}, 0.2f);
            StackSmokeEffectInterVal = 0.0f;
            StackSmokeEffectSeconds = 0.0f;
        }

        if (StackSparkEffectInterVal >= 5.0f)
        {
            DirectX::XMFLOAT3 launchPos{ t.Position };
            launchPos.x += static_cast<float>(rand() % 10) - 5.0f;
            hdl_Spark = efk_Spark->Play(launchPos, {}, 0.3f);
            StackSparkEffectInterVal = 0.0f;
            StackSparkEffectSeconds = 0.0f;
        }

        break;
    case EffectState::Max:
        if (StackSmokeEffectInterVal >= 7.0f)
        {
            DirectX::XMFLOAT3 launchPos{ t.Position };
            launchPos.y += 5.0f;
            hdl_Smoke = efk_Smoke->Play(t.Position, {}, 0.2f);
            StackSmokeEffectInterVal = 0.0f;
            StackSmokeEffectSeconds = 0.0f;
        }

        if (StackSparkEffectInterVal >= 2.0f)
        {
            DirectX::XMFLOAT3 launchPos{ t.Position };
            launchPos.x += static_cast<float>(rand() % 10) - 5.0f;
            hdl_Spark = efk_Spark->Play(launchPos, {}, 0.3f);
            StackSparkEffectInterVal = 0.0f;
            StackSparkEffectSeconds = 0.0f;
        }

        break;
    default:;
    }

    // エフェクトの位置をこうしん
    efk_Smoke->SetPosition(hdl_Smoke, t.Position);
    efk_Spark->SetPosition(hdl_Spark, t.Position);

    // エフェクトをとめる
    if (StackSmokeEffectSeconds >= 6.0f)
    {
        efk_Smoke->Stop(hdl_Smoke);
    }

    // エフェクトをとめる
    if (StackSparkEffectSeconds >= 4.0f)
    {
        efk_Spark->Stop(hdl_Spark);
    }

}
