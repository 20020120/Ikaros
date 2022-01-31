#include"Boss_4.h"

#include "EnemyManager.h"
#include"ProjectileManager.h"
#include"Bullet.h"
#include "GameSystem.h"
#include "OptionSystem.h"
#include "SceneManager.h"

void Boss_4::Behavior(float elapsed_time)
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
    switch (type) { case Type::NONE: break;
    case Type::RED:
        LightColor = Calcu3D::LerpFloat3(LightColor, { 1.0f,0.0f,0.0f }, 10.0f * elapsed_time);
        break;
    case Type::BLUE:
        LightColor = Calcu3D::LerpFloat3(LightColor, { 0.0f,0.0f,1.0f }, 10.0f * elapsed_time);
        break;
    default: ; }

    ChangeView(elapsed_time);

    if (IsPerformance)
    {
        ChangeViewPerformance(elapsed_time);
    }

    if (!IsPerformance && CompleteChangeView)
    {
        AttackManager(elapsed_time);
    }

    if (!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(AnimationName::idle, true);
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

void Boss_4::Move(float elapsed_time)
{
    BaseBoss::Move(elapsed_time);
}

void Boss_4::Collision()
{
    BaseBoss::Collision();
}

Boss_4::Boss_4(ID3D11Device* d)
    :BaseBoss(d, "./resources/Models/Characters/Boss/Boss2.nk", "./resources/Shaders/Boss_PBR_PS.cso")
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
    mEventFlag.Load("./resources/Models/Characters/Enemies/Boss2EventFlag.flg");

    //--------------------<ポイントライト初期化>--------------------//
    mPointLight = std::make_unique<GamePointLight>(d, 4);
    mPointLight->SetColor({ 1.0f,0.0f,0.0f });
    
    // 登場
    EnemyManager::Instance().SetStartBossAppear(true);
    EnemyManager::Instance().SetStartBossBGM(true);

    // エフェクトを初期化
    efk_Angry = std::make_unique<Effect>("./resources/Effects/angry.efk");
    efk_Smoke = std::make_unique<Effect>("./resources/Effects/smoke.efk");
    efk_Spark = std::make_unique<Effect>("./resources/Effects/plasma.efk");
    efk_LastBomb = std::make_unique<Effect>("./resources/Effects/bomb_more.efk");
}

Boss_4::~Boss_4()
{
}

void Boss_4::Initialize()
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
    ZoomUp = true;

    LightRange = { 0.0f };
    LightColor = { 1.0f,0.0f,0.0f };

    type = Type::RED;

    EnemyManager::Instance().SetStartBossAppear(true);


    //****************************************************************
    // 
    // 関数ポインタを初期化
    // 
    //****************************************************************

    std::vector<pBehavior> vec{};

    //--------------------<Topの攻撃>--------------------//
    vec.emplace_back(&Boss_4::AppealTime2);
    vec.emplace_back(&Boss_4::T_Energy);
    vec.emplace_back(&Boss_4::T_DiffuseBullet);
    vec.emplace_back(&Boss_4::T_CurveShot);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::T_DiffuseBullet);
    vec.emplace_back(&Boss_4::T_Energy);
    vec.emplace_back(&Boss_4::T_RotateAttack);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::T_Horming);
    vec.emplace_back(&Boss_4::AppealTime);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::T_CurveShot);
    vec.emplace_back(&Boss_4::AppealTime);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();


    //--------------------<Topの攻撃>--------------------//
    vec.emplace_back(&Boss_4::T_MoveFirstPosition);
    vec.emplace_back(&Boss_4::T_DiffuseBullet);
    vec.emplace_back(&Boss_4::T_CurveShot);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::T_DiffuseBullet);
    vec.emplace_back(&Boss_4::T_CurveShot);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::T_RotateAttack);
    vec.emplace_back(&Boss_4::T_DiffuseBullet);
    vec.emplace_back(&Boss_4::AppealTime2);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::T_RotateAttack2);
    vec.emplace_back(&Boss_4::AppealTime2);
    vec.emplace_back(&Boss_4::T_CurveShot);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::T_Horming);
    vec.emplace_back(&Boss_4::AppealTime2);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::T_CurveShot);
    vec.emplace_back(&Boss_4::T_Horming);
    vec.emplace_back(&Boss_4::AppealTime2);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::T_CurveShot);
    vec.emplace_back(&Boss_4::T_Horming);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();


    //--------------------<Sideの攻撃>--------------------//

    vec.emplace_back(&Boss_4::S_MoveFirstPosition);
    vec.emplace_back(&Boss_4::S_Beam);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::S_CurveBullet);
    vec.emplace_back(&Boss_4::S_Energy);
    vec.emplace_back(&Boss_4::S_CurveBullet);
    vec.emplace_back(&Boss_4::AppealTime2);
    vec.emplace_back(&Boss_4::S_RandSpeedAttack);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::S_RandSpeedAttack);
    vec.emplace_back(&Boss_4::S_RandSpeedAttack);
    vec.emplace_back(&Boss_4::AppealTime2);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::S_Beam);
    vec.emplace_back(&Boss_4::S_Energy);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::S_Energy);
    vec.emplace_back(&Boss_4::S_RandSpeedAttack);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::S_RandSpeedAttack);
    vec.emplace_back(&Boss_4::S_CurveBullet);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();


    //*******************************************************************//
    vec.emplace_back(&Boss_4::S_MoveFirstPosition);
    vec.emplace_back(&Boss_4::S_Beam);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[ANGRY_SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::S_CurveBullet);
    vec.emplace_back(&Boss_4::AppealTime);
    vec.emplace_back(&Boss_4::S_RandSpeedAttack);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[ANGRY_SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::AppealTime);
    vec.emplace_back(&Boss_4::S_DiffuseShot2);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[ANGRY_SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_4::AppealTime);
    vec.emplace_back(&Boss_4::S_CurveBullet2);
    vec.emplace_back(&Boss_4::AppealTime);
    vec.emplace_back(&Boss_4::S_Energy);
    vec.emplace_back(&Boss_4::S_Beam);
    vec.emplace_back(&Boss_4::AppealTime);
    vec.emplace_back(&Boss_4::EndAttack);
    BehaviorVec[ANGRY_SIDE].emplace_back(vec);
    vec.clear();

    status.hitPoint = MaxHitPoint;
    IsRoar = false;
}

void Boss_4::Render(ID3D11DeviceContext* dc)
{
    mPointLight->Update(dc);
    Enemy::Render(dc);
    if (ShowBeamEmitter)
    {
        mBeamEmitter[0].Model->Render(dc, mBeamEmitter[0].t, mBeamEmitter[0].r);
        mBeamEmitter[1].Model->Render(dc, mBeamEmitter[1].t, mBeamEmitter[1].r);
    }
}

void Boss_4::Finalize()
{
    BaseBoss::Finalize();
}

void Boss_4::AttackManager(float elapsedTime)
{
    if (CompleteAttack)
    {
        switch (CurrentMode) {
        case TOP:
            CurrentAttackComboNumber = static_cast<int>(rand() % 4);
            CompleteAttack = false;
            break;
        case SIDE:
            CurrentAttackComboNumber = rand() % 5 + 1;
            CompleteAttack = false;
            break;
        case ANGRY_TOP:
            CurrentAttackComboNumber = static_cast<int>(rand() % 6) + 1;
            CompleteAttack = false;
            break;
        case ANGRY_SIDE:
            CurrentAttackComboNumber = static_cast<int>(rand() % 2) + 1;
            CompleteAttack = false;
            break;
        default:;
        }
    }

    (this->*BehaviorVec[CurrentMode][CurrentAttackComboNumber][CurrentAttackNumber])(elapsedTime);

}

void Boss_4::PlayEffects(float elapsedTime)
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

void Boss_4::LastMotion(float elapsedTime)
{
    const int State = EnemyManager::Instance().GetLastPerformanceState();
    if (State == 2)
    {
        t.Position = { 0.0f,0.0f,50.0f };
        IsDraw = false;
    }
    else if (State == 6)
    {
        IsDraw = true;
    }
    else if (State == 8)
    {
        Model->f_PlayAnimation(AnimationName::die
        );
    }
    else if (State == 9)
    {
        if (!LastBomb)
        {
            // エフェクト
            efk_LastBomb->Play(t.Position, {}, 0.4f);
        }
    }
}
void Boss_4::ChangeView(float elapsedTime)
{
    // 視点変更完了後のⅠフレーム処理
    if (!CompleteChangeView && !GameSystem::Instance().GetChangingView())
    {
        CurrentAttackComboNumber = 0;
        CompleteChangeView = true;
        CompleteAttack = false;
        CurrentAttackNumber = 0;
        ZoomUp = false;
        ZoomPower = 0.0f;
        EnemyManager::Instance().fSetZoomPower(ZoomPower);
        switch (CurrentMode) {
        case TOP:
            break;
        case SIDE:
            status.hitPoint = MaxHitPoint * 0.6f;
            break;
        case ANGRY_TOP:
            status.hitPoint = MaxHitPoint * 0.3f;
            break;
        case ANGRY_SIDE: break;
        default:;
        }

    }

    if (CompleteAttack && CompleteChangeView)
    {
        if (
            (CurrentMode == TOP && status.hitPoint < MaxHitPoint * 0.6f) ||
            (CurrentMode == SIDE && status.hitPoint < MaxHitPoint * 0.3f)
            )
        {
            IsPerformance = true;
            r.Angle = { 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f };
            ZoomPower = 0.0f;
            ZoomUp = true;
            IsRoar = false;
            switch (CurrentMode) {
            case TOP:
                CurrentMode = SIDE;
                type = Type::BLUE;
                Model->f_PlayAnimation(AnimationName::angle_change);
                break;
            case SIDE:
                Model->f_PlayAnimation(AnimationName::angry);
                type = Type::RED;
                CurrentMode = ANGRY_TOP;
                break;
            case ANGRY_TOP: break;
            case ANGRY_SIDE: break;
            default:;
            }

            type = Type::BLUE;

        }
    }

    ZoomPower += elapsedTime;
    if (ZoomPower >= 0.2f)
    {
        if (!ZoomUp)
        {
            ZoomPower -= elapsedTime * 100.0f;
            ZoomPower = std::max(ZoomPower, 0.0f);
        }
        else
        {
            if(!IsRoar)
            {
                se[0]->Play(false);
                IsRoar = true;
            }
            
            ZoomPower += elapsedTime * 100.0f;
            ZoomPower = std::min(ZoomPower, 100.0f);
            if (ZoomPower >= 100.0f)
            {
                ZoomUp = false;
            }
        }
    }
    EnemyManager::Instance().fSetZoomPower(ZoomPower);
    if (IsInputChangeView)
    {
        GameSystem::Instance().ChangeView();
        IsInputChangeView = false;
        CompleteChangeView = false;
    }
}


void Boss_4::GuiMenu()
{
#ifdef _DEBUG
    ImGui::Begin("Boss2");
    if (ImGui::Button("Next"))
    {
        status.hitPoint -= (MaxHp * 0.25f + 1.0f);
    }
    ImGui::Text(("HitPoint=" + to_string(status.hitPoint)).c_str());

    ImGui::Checkbox("CompleteAttack", &CompleteAttack);

    ImGui::DragFloat3("Emitter0Pos", &mBeamEmitter[0].t.Position.x);
    ImGui::DragFloat3("Emitter1Pos", &mBeamEmitter[1].t.Position.x);
    ImGui::End();
#endif
}

void Boss_4::T_CurveShot(float elapsedTime)
{
    static int ShotCount{};

    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::allrange_attack);
        AttackState++;
        /*FallThrough*/
    case 1:
        StackAttackInterval += elapsedTime;
        if (StackAttackInterval >= 0.3f)
        {
            StackAttackInterval = 0.0f;
            se[BLUE_BULLET_SHOT]->Stop();
            se[BLUE_BULLET_SHOT]->Play(false, OptionSystem::Instance().GetSeVolume());

            auto b0 = new CurveBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1, 1);
            ProjectileManager::Instance().RegisterProjectile(b0);
            auto b1 = new CurveBullet(BaseProjectile::Parent::REDENEMY, t.Position, -1, 1);
            ProjectileManager::Instance().RegisterProjectile(b1);
            ShotCount++;
        }

        // 2回うったら終わり
        if (ShotCount >= 2)
        {
            AttackState++;
            ShotCount = 0;
            StackAttackInterval = 0.0f;
        }
        break;
    case 2:
        CurrentAttackNumber++;
        AttackState = 0;
        break;
    }



}
void Boss_4::T_RotateAttack(float elapsedTime)
{

    switch (AttackState)
    {
    case 0:
        t.Position.z = Calcu3D::LerpFloat(t.Position.z, 0.0f, 10.0f * elapsedTime);
        if(t.Position.z<=1.0f)
        {
            t.Position.z = 0.0f;
            AttackState++;
        }
        break;
    case 1:
        // 回転攻撃
        Ratio += elapsedTime * 0.1f;
        StackAttackInterval += elapsedTime;
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-150.0f), DirectX::XMConvertToRadians(-540.0f), Ratio);
        r.Angle.y = Calcu3D::ClampRotate(r.Angle.y);

        // 向きに沿って攻撃
        if (StackAttackInterval >= 0.2f)
        {
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle, t.Position);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
            r.Angle.y = DirectX::XMConvertToRadians(-180.0f);
        }


        break;
    case 2:
        t.Position.z = Calcu3D::LerpFloat(t.Position.z, 20.0f, 10.0f * elapsedTime);
        if (t.Position.z >= 19.0f)
        {
            t.Position.z = 20.0f;
            AttackState++;
        }
        break;
    case 3:
        CurrentAttackNumber++;
        AttackState = 0;
        break;
    default: ;
    }


}

void Boss_4::T_RotateAttack2(float elapsedTime)
{

    switch (AttackState)
    {
    case 0:
        t.Position.z = Calcu3D::LerpFloat(t.Position.z, 0.0f, 10.0f * elapsedTime);
        if (t.Position.z <= 1.0f)
        {
            t.Position.z = 0.0f;
            AttackState++;
        }
        break;
    case 1:
        // 回転攻撃
        Ratio += elapsedTime * 0.15f;
        StackAttackInterval += elapsedTime;
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-150.0f), DirectX::XMConvertToRadians(-540.0f), Ratio);
        r.Angle.y = Calcu3D::ClampRotate(r.Angle.y);

        // 向きに沿って攻撃
        if (StackAttackInterval >= 0.3f)
        {
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle, t.Position);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
            r.Angle.y = DirectX::XMConvertToRadians(-180.0f);
        }


        break;
    case 2:
        t.Position.z = Calcu3D::LerpFloat(t.Position.z, 20.0f, 10.0f * elapsedTime);
        if (t.Position.z >= 19.0f)
        {
            t.Position.z = 20.0f;
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



void Boss_4::T_Energy(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::allrange_attack);
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

        if(Ratio>=0.7f)
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
    default: ;
    }   
}


void Boss_4::S_Energy(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::allrange_attack);
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
            rad.x -= DirectX::XMConvertToRadians(45.0f);
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
            rad.x += DirectX::XMConvertToRadians(45.0f);
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

void Boss_4::T_DiffuseBullet(float elapsedTime) 
{
      StackAttackInterval += elapsedTime;
      Ratio += elapsedTime;
      if (StackAttackInterval >= 0.3f)
      {
          Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle,t.Position);
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
          CurrentAttackNumber++;
      }
}

void Boss_4::S_Beam(float elapsedTime)
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
        se[LaserCharge]->Stop();
        se[LaserCharge]->Play(false, OptionSystem::Instance().GetSeVolume());
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
        se[LaserCharge]->Stop();
        se[LaserShot]->Play(false, OptionSystem::Instance().GetSeVolume());
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

void Boss_4::T_Horming(float elapsedTime)
{
    BaseBoss::T_ShotHoming(1);
    BaseBoss::T_ShotHoming(-1);
    CurrentAttackNumber++;
}

void Boss_4::S_RandSpeedAttack(float elapsedTime)
{
    const float Begin = DirectX::XMConvertToRadians(90.0f);
    const float Begin2 = DirectX::XMConvertToRadians(-90.0f);


    Ratio += elapsedTime * 0.5f;
    StackAttackInterval += elapsedTime;

    if (StackAttackInterval > 0.07f)
    {
        DirectX::XMFLOAT3 Radian{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };

        const float rat = static_cast<float>(rand()) / RAND_MAX;
        Radian.x = Calcu3D::LerpFloat(Begin, Begin2, rat);


        Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, Radian, t.Position);

        StackAttackInterval = 0.0f;
    }

    if (Ratio > 1.0f)
    {
        Ratio = 0.0f;
        CurrentAttackNumber++;
        StackAttackInterval++;
    }
}

void Boss_4::S_DiffuseShot(float elapsedTime)
{
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
        CurrentAttackNumber++;
    }
}

void Boss_4::S_CurveBullet(float elapsedTime)
{
    static int ShotCount{};

    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::allrange_attackhoming_attack);
        /*FallThrough*/
    case 1:
        StackAttackInterval += elapsedTime;
        if (StackAttackInterval >= 0.3f)
        {
            StackAttackInterval = 0.0f;
            auto b0 = new CurveBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1, 1);
            ProjectileManager::Instance().RegisterProjectile(b0);
            auto b1 = new CurveBullet(BaseProjectile::Parent::REDENEMY, t.Position, -1, 1);
            ProjectileManager::Instance().RegisterProjectile(b1);
            ShotCount++;
        }

        // 3回うったら終わり
        if (ShotCount >= 3)
        {
            AttackState++;
            ShotCount = 0;
            StackAttackInterval = 0.0f;
        }
        break;
    case 2:
        CurrentAttackNumber++;
        AttackState = 0;
        break;
    }

    
}

void Boss_4::S_CurveBullet2(float elapsedTime)
{
    static int ShotCount{};

    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::allrange_attackhoming_attack);
        /*FallThrough*/
    case 1:
        StackAttackInterval += elapsedTime;
        if (StackAttackInterval >= 0.2f)
        {
            StackAttackInterval = 0.0f;
            auto b0 = new CurveBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1, 1);
            ProjectileManager::Instance().RegisterProjectile(b0);
            auto b1 = new CurveBullet(BaseProjectile::Parent::REDENEMY, t.Position, -1, 1);
            ProjectileManager::Instance().RegisterProjectile(b1);
            ShotCount++;
        }

        // 3回うったら終わり
        if (ShotCount >= 4)
        {
            AttackState++;
            ShotCount = 0;
            StackAttackInterval = 0.0f;
        }
        break;
    case 2:
        CurrentAttackNumber++;
        AttackState = 0;
        break;

    }
}

void Boss_4::S_DiffuseShot2(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::angle_change);
        AttackState++;

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

        Model->f_PlayAnimation(AnimationName::angle_change);
        AttackState++;

    case 3:
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
    case 4:
        Model->f_PlayAnimation(AnimationName::angle_change);
            AttackState++;
        
        break;
    case 5:
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
    case 6:
        CurrentAttackNumber++;
        AttackState = 0;
        break;

    }
}

void Boss_4::FirstMotion(float elapsedTime)
{
    // ボスのほうにカメラを向けてボス登場モーション

    ZoomPower += elapsedTime;
    if (ZoomPower >= 1.1f)
    {
        if (!ZoomUp)
        {
            ZoomPower -= elapsedTime * 100.0f;
            ZoomPower = std::max(ZoomPower, 0.0f);
        }
        else
        {
            if(!IsRoar)
            {
                se[0]->Play(false);
                IsRoar = true;
            }
            
            ZoomPower += elapsedTime * 100.0f;
            ZoomPower = std::min(ZoomPower, 100.0f);
            if (ZoomPower >= 100.0f)
            {
                ZoomUp = false;
            }
        }
    }
    EnemyManager::Instance().fSetZoomPower(ZoomPower);


    if (!Model->GetIsPlayAnimation() && StackTimer < 20.0f)
    {
        // 登場アニメを再生
        Model->f_PlayAnimation(AnimationName::appear);
        ProjectileManager::Instance().Clear();
    }

    if (mEventFlag.Check("EndFirstAnim", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        StackTimer = 20.0f;
        EnemyManager::Instance().fSetZoomPower(0.0f);
    }

    if (StackTimer >= 20.0f)
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
