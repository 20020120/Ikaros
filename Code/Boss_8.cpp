#include"Boss_8.h"

#include "Bullet.h"
#include"MathSupport.h"
#include "User_Function.h"
#include"EnemyManager.h"
#include "GameSystem.h"
#include"ProjectileManager.h"
#include "SceneManager.h"

void Boss_8::Behavior(float elapsed_time)
{
    Model->Update(elapsed_time);

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
        Model->f_PlayAnimation(AnimationName::idle, true);
    }

    //無敵時間更新
    invTimer -= elapsed_time;
    PlayEffects(elapsed_time);

    //--------------------<ポイントライトを更新>--------------------//
    mPointLight->SetPosition(t.Position);
    mPointLight->SetColor(LightColor);
    mPointLight->SetRange(LightRange);

    

    GuiMenu();
}

void Boss_8::Move(float elapsed_time)
{
    BaseBoss::Move(elapsed_time);
}

void Boss_8::Collision()
{
    BaseBoss::Collision();
}

Boss_8::Boss_8(ID3D11Device* d)
    :BaseBoss(d, "./resources/Models/Characters/Boss/Boss2.nk", "./resources/Shaders/Boss_PBR_PS.cso")
{
    Initialize();

    //--------------------<ライトのモデルを初期化>--------------------//
    ModelRenderInitializer Ini;
    Ini.SetALLPath(
        "./resources/Models/Characters/Boss/Laser.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/PhisicsBase_PS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );


    //--------------------<イベントフラグの初期化>--------------------//
    mEventFlag.Load("./resources/Models/Characters/Enemies/Boss2EventFlag.flg");

    //--------------------<ポイントライト初期化>--------------------//
    mPointLight = std::make_unique<GamePointLight>(d, 4);
    mPointLight->SetColor({ 1.0f,0.0f,0.0f });

    // 登場
    EnemyManager::Instance().SetStartBossAppear(true);
    EnemyManager::Instance().SetStartBossBGM(true);

    status.hitPoint = static_cast<float>(MaxHitPoint);
    ZoomUp = true;

    // エフェクトを初期化
    efk_Angry = std::make_unique<Effect>("./resources/Effects/angry.efk");
    efk_Smoke = std::make_unique<Effect>("./resources/Effects/smoke.efk");
    efk_Spark = std::make_unique<Effect>("./resources/Effects/plasma.efk");
    efk_Spark = std::make_unique<Effect>("./resources/Effects/plasma.efk");
    efk_LastBomb = std::make_unique<Effect>("./resources/Effects/bomb_more.efk");
}

Boss_8::~Boss_8()
{
}

void Boss_8::LastMotion(float elapsedTime)
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


void Boss_8::Initialize()
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

    std::vector<pBehavior> vec{};



    vec.emplace_back(&Boss_8::T_MixEnergy);
    vec.emplace_back(&Boss_8::T_HighTempoShot);
    vec.emplace_back(&Boss_8::T_TargetShot);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_8::T_Beam);
    vec.emplace_back(&Boss_8::T_TargetShot);
    vec.emplace_back(&Boss_8::T_MixEnergy);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_8::T_StepAllRangeAttack);
    vec.emplace_back(&Boss_8::T_TargetShot);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_8::T_MixEnergy);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_8::T_DiffuseShot);
    vec.emplace_back(&Boss_8::T_TargetShot);
    vec.emplace_back(&Boss_8::T_DiffuseShot);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();



    vec.emplace_back(&Boss_8::S_MoveFirstPosition);
    vec.emplace_back(&Boss_8::S_TargetShot);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_8::S_DiffuseShot);
    vec.emplace_back(&Boss_8::S_DiffuseShot);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_8::S_V_CrossNagiriAttack);
    vec.emplace_back(&Boss_8::S_Energy);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_8::S_TargetShot);
    vec.emplace_back(&Boss_8::S_Energy);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();


    vec.emplace_back(&Boss_8::S_Horming);
    vec.emplace_back(&Boss_8::AppealTime2);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();


    vec.emplace_back(&Boss_8::T_MoveFirstPosition);
    vec.emplace_back(&Boss_8::T_StepAllRangeAttack);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_8::T_StepAllRangeAttack);
    vec.emplace_back(&Boss_8::T_StepAllRangeAttack);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_8::T_MixEnergy);
    vec.emplace_back(&Boss_8::T_StepAllRangeAttack);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_8::T_MixEnergy);
    vec.emplace_back(&Boss_8::T_TargetShot);
    vec.emplace_back(&Boss_8::T_TargetShot);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_8::T_MixEnergy);
    vec.emplace_back(&Boss_8::T_Horming);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_8::T_MixEnergy);
    vec.emplace_back(&Boss_8::T_Horming);
    vec.emplace_back(&Boss_8::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();


}

void Boss_8::Render(ID3D11DeviceContext* dc)
{
    mPointLight->Update(dc);
    BaseBoss::Render(dc);
   
}

void Boss_8::Finalize()
{
    BaseBoss::Finalize();
}


void Boss_8::FirstMotion(float elapsedTime)
{
    // ボスのほうにカメラを向けてボス登場モーション

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
    if(!Model->GetIsPlayAnimation()&& StackTimer < 20.0f)
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

    if(StackTimer>=20.0f)
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

void Boss_8::ChangeViewPerformance(float elapsedTime)
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

void Boss_8::PlayEffects(float elapsedTime)
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



void Boss_8::ChangeView()
{
    // 視点変更完了後のⅠフレーム処理
    if (!CompleteChangeView && !GameSystem::Instance().GetChangingView())
    {
        CurrentAttackComboNumber = 0;
        CompleteChangeView = true;
        CompleteAttack = false;
        CurrentAttackNumber = 0;

        se[0]->Play(false);

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
            (CurrentMode == TOP && status.hitPoint < MaxHitPoint * 0.6f)||
            (CurrentMode == SIDE && status.hitPoint < MaxHitPoint * 0.3f)
            )
        {
            IsPerformance = true;
            r.Angle = { 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f };

            switch (CurrentMode) {
            case TOP:
                CurrentMode = SIDE;
                Model->f_PlayAnimation(AnimationName::angle_change);
                break;
            case SIDE:
                Model->f_PlayAnimation(AnimationName::angry);
                CurrentMode = ANGRY_TOP;
                break;
            case ANGRY_TOP: break;
            case ANGRY_SIDE: break;
            default:;
            }
            
            type = Type::BLUE;

        }
    }



    if (IsInputChangeView)
    {
        GameSystem::Instance().ChangeView();
        IsInputChangeView = false;
        CompleteChangeView = false;
    }
}

void Boss_8::AttackManager(float elapsedTime)
{
    if (CompleteAttack)
    {
        switch (CurrentMode) {
        case TOP:
            CurrentAttackComboNumber = rand() % 5;
            CompleteAttack = false;
            break;
        case SIDE:
            CurrentAttackComboNumber = rand() % 4 + 1;
            CompleteAttack = false;
            break;
        case ANGRY_TOP :
            CurrentAttackComboNumber = rand() % 5 + 1;
                CompleteAttack = false;
            break;
        case ANGRY_SIDE: break;
        default: ;
        }
    }

    (this->*BehaviorVec[CurrentMode][CurrentAttackComboNumber][CurrentAttackNumber])(elapsedTime);
}

void Boss_8::GuiMenu()
{
#ifdef _DEBUG
    ImGui::Begin("Boss2");
    if (ImGui::Button("Next"))
    {
        status.hitPoint -= (MaxHitPoint * 0.25f + 1.0f);
    }
    ImGui::Text(("HitPoint=" + to_string(status.hitPoint)).c_str());

    ImGui::Checkbox("CompleteAttack", &CompleteAttack);

    ImGui::End();
#endif
}


void Boss_8::T_HighTempoShot(float elapsedTime)
{
    StackAttackInterval += elapsedTime;
    Ratio += elapsedTime;
    if (StackAttackInterval >= 0.2f)
    {
        Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle, t.Position);
        DirectX::XMFLOAT3 rad = r.Angle;
        rad.y += DirectX::XMConvertToRadians(45.0f);
        Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, rad, t.Position);
        rad.y -= DirectX::XMConvertToRadians(90.0f);
        Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, rad, t.Position);
        StackAttackInterval = 0.0f;
    }

    if (Ratio >= 1.0f)
    {
        Ratio = 0.0f;
        CurrentAttackNumber++;
    }
}

void Boss_8::T_Horming(float elapsedTime)
{
    static int Count = 0;
    StackAttackInterval += elapsedTime;

    switch (AttackState)
    {
    case 0:
        // アニメーション再生
        Model->f_PlayAnimation(AnimationName::allrange_attackhoming_attack);
        AttackState++;
        /*FallThrough*/
    case 1:

        if (StackAttackInterval > 0.3f)
        {
            T_ShotHoming(1);
            T_ShotHoming(-1);
            StackAttackInterval = 0.0f;
            Count++;
        }
        if (Count > 4)
        {
            Count = 0;
            StackAttackInterval = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        // 最初期化
        AttackState = 0;
        StackAttackInterval = 0.0f;
        CurrentAttackNumber++;
        break;
    default:

        break;
    }


    
}

void Boss_8::T_TargetShot(float elapsedTime)
{
    switch(AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::allrange_attackhoming_attack);
        AttackState++;
        /*FallThrough*/
    case 1:
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;
        if (StackAttackInterval >= 0.4f)
        {
            auto b0 = new StayToTargetBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1, { static_cast<float>(rand() % 40) - 20.0f,0.0f,10.0f }, 1.3f);
            ProjectileManager::Instance().RegisterProjectile(b0);
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
        Ratio = 0.0f;
        StackAttackInterval = 0.0f;
        CurrentAttackNumber++;
        AttackState = 0;
        break;
    default: ;
    }


  

}

void Boss_8::T_MixEnergy(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::concentrated_attack);
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

void Boss_8::T_StepAllRangeAttack(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        // 向かって右に移動
        Ratio += elapsedTime * 3.0f;

        t.Position.x = Calcu3D::LerpFloat(0.0f, 20.0f, Ratio);  // 移動
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-180.0f), DirectX::XMConvertToRadians(-540.0f), Ratio);
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 1:
        // 打つ
    {
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        const int Max = 10;
        // 全体に攻撃
        for (int i = 0; i < Max; i++)
        {
            // 合計が 360どになるように

            float Deg = static_cast<float>(i) * (360.0f / static_cast<float>(Max)); // Y軸の角度（ディグリー）
            if (Deg > 360.0f) Deg -= 360.0f;

            Deg = DirectX::XMConvertToRadians(Deg);
            const DirectX::XMFLOAT3 Vel = { sinf(Deg),0.0f,cosf(Deg) }; // 進行方向

            auto b = new Bullet(p_device, BaseProjectile::Parent::REDENEMY, t.Position, Vel);
            ProjectileManager::Instance().RegisterProjectile(b);
        }
        AttackState++;
    }
    break;
    case 2:
        // 向かって左に移動
        Ratio += elapsedTime * 2.0f;
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-540.0f), DirectX::XMConvertToRadians(-180.0f), Ratio);
        t.Position.x = Calcu3D::LerpFloat(20.0f, -20.0f, Ratio);  // 移動
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 3:
        // 撃つ
          // 打つ
    {
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        const int Max = 10;
        // 全体に攻撃
        for (int i = 0; i < Max; i++)
        {
            // 合計が 360どになるように

            float Deg = static_cast<float>(i) * (360.0f / static_cast<float>(Max)); // Y軸の角度（ディグリー）
            if (Deg > 360.0f) Deg -= 360.0f;

            Deg = DirectX::XMConvertToRadians(Deg);
            const DirectX::XMFLOAT3 Vel = { sinf(Deg),0.0f,cosf(Deg) }; // 進行方向

            auto b = new Bullet(p_device, BaseProjectile::Parent::REDENEMY, t.Position, Vel);
            ProjectileManager::Instance().RegisterProjectile(b);
        }
        AttackState++;
    }
    break;
    case 4:
        // 戻る
        Ratio += elapsedTime * 2.0f;
        t.Position.x = Calcu3D::LerpFloat(-20.0f, 0.0f, Ratio);  // 移動
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-180.0f), DirectX::XMConvertToRadians(-540.0f), Ratio);
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
            r.Angle.y = DirectX::XMConvertToRadians(-180.0f);
        }
        break;
    case 5:
        // 打つ
    {
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        const int Max = 10;
        // 全体に攻撃
        for (int i = 0; i < Max; i++)
        {
            // 合計が 360どになるように

            float Deg = static_cast<float>(i) * (360.0f / static_cast<float>(Max)); // Y軸の角度（ディグリー）
            if (Deg > 360.0f) Deg -= 360.0f;

            Deg = DirectX::XMConvertToRadians(Deg);
            const DirectX::XMFLOAT3 Vel = { sinf(Deg),0.0f,cosf(Deg) }; // 進行方向

            auto b = new Bullet(p_device, BaseProjectile::Parent::REDENEMY, t.Position, Vel);
            ProjectileManager::Instance().RegisterProjectile(b);
        }
        AttackState++;
    }
    break;
    case 6:
        /// <summary>
        /// 最初期化
        /// </summary>
        /// <param name="elapsedTime"></param>
        AttackState = 0;
        Ratio = 0.0f;
        t.Position.x = 0.0f;
        r.Angle.y = DirectX::XMConvertToRadians(-180.0f);
        CurrentAttackNumber++;
        break;

    default:;
    }

}



void Boss_8::T_Beam(float elapsedTime)
{

    //--------------------<ビーム発射>--------------------//
    switch (AttackState)
    {
    case 0:
    {
       // アニメーション再生
        Model->f_PlayAnimation(AnimationName::beam_charge);
        AttackState++;
        CompleteAttack = false;
        /*FallThrough*/


    }
    case 1:
        if (mEventFlag.Check("EndCharge", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            AttackState++;
        }
        break;
    case 2:

        Model->f_PlayAnimation(AnimationName::beam_shoot);
        AttackState++;
    case 3:
    {
        StackAttackInterval = 0.0f;
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        mPointLight->SetRange(-4.0f);
        auto b0 = new Beam(p_device, BaseProjectile::Parent::REDENEMY,
            t.Position);
        ProjectileManager::Instance().RegisterProjectile(b0);
        AttackState++;
    }
    case 4:


        StackAttackInterval += elapsedTime;
        if (StackAttackInterval > 5.0f)
        {
            AttackState++;
            mPointLight->SetRange(0.0f);
        }
        break;
    case 5:
        CurrentAttackNumber++;
        AttackState = 0;
        StackAttackInterval = 0.0f;
        break;

    }

}

void Boss_8::T_DiffuseShot(float elapsedTime)
{

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
        CurrentAttackNumber++;
    }
}


void Boss_8::S_DiffuseShot(float elapsedTime)
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

void Boss_8::S_Energy(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:

        Model->f_PlayAnimation(AnimationName::concentrated_attack);
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


void Boss_8::S_TargetShot(float elapsedTime)
{
    StackAttackInterval += elapsedTime;
    Ratio += elapsedTime;


    if (Ratio >= 0.5f)
    {


        auto b0 = new StayToTargetBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1,
            { 0.0f,static_cast<float>(rand() % 60)-30.0f,10.0f }, 1.3f);
        ProjectileManager::Instance().RegisterProjectile(b0);

        auto b1 = new StayToTargetBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1, 
            { static_cast<float>(rand() % 40)*-1.0f,static_cast<float>(rand() % 60) - 30.0f,5.0f }, 1.6f);
        ProjectileManager::Instance().RegisterProjectile(b1);

        auto b2 = new StayToTargetBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1,
            { static_cast<float>(rand() % 40) * -1.0f,static_cast<float>(rand() % 60) - 30.0f,10.0f }, 1.9f);
        ProjectileManager::Instance().RegisterProjectile(b2);

        auto b3 = new StayToTargetBullet(BaseProjectile::Parent::REDENEMY, t.Position, 1, 
            { static_cast<float>(rand() % 40) * -1.0f,static_cast<float>(rand() % 60) - 30.0f,5.0f }, 2.3f);
        ProjectileManager::Instance().RegisterProjectile(b3);


        Ratio = 0.0f;
        StackAttackInterval = 0.0f;
        CurrentAttackNumber++;
    }
}

void Boss_8::S_V_CrossNagiriAttack(float elapsedTime)
{



    const float Begin = DirectX::XMConvertToRadians(90.0f);
    const float Begin2 = DirectX::XMConvertToRadians(-90.0f);

    const float End = 0.0f;

    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::nagiri_attack);
        AttackState++;
        /*FallThrough*/
    case 1:
        Ratio += elapsedTime * 1.0f;
        StackAttackInterval += elapsedTime;

        if (StackAttackInterval > 0.1f)
        {
            DirectX::XMFLOAT3 Radian{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
            DirectX::XMFLOAT3 Radian2{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
            Radian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
            Radian2.x = Calcu3D::LerpFloat(Begin2, End, Ratio);

            Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, Radian, t.Position);
            Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, Radian2, t.Position);

            StackAttackInterval = 0.0f;
        }

        if (Ratio > 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        
            CurrentAttackNumber++;
            StackAttackInterval = 0.0f;
        break;

    }


   

}

void Boss_8::S_Beam(float elapsedTime)
{
    //--------------------<ビーム発射>--------------------//
    switch (AttackState)
    {
    case 0:
    {
        Model->f_PlayAnimation(AnimationName::beam_charge);
        Ratio = 0.0f;
        AttackState++;

        /*FallThrough*/


    }
    case 1:
        

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

        if (mEventFlag.Check("EndCharge", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            AttackState++;
        }

        break;
    case 2:
    {

        Model->f_PlayAnimation(AnimationName::beam_shoot);
        StackAttackInterval = 0.0f;
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        mPointLight->SetRange(-4.0f);
        auto b0 = new Beam(p_device, BaseProjectile::Parent::REDENEMY,
            t.Position);
        ProjectileManager::Instance().RegisterProjectile(b0);
        Ratio = 0.0f;
        AttackState++;
    }
    case 3:
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
        break;
    default:;
    }
}

void Boss_8::S_Horming(float elapsedTime)
{

    static int Count = 0;
    StackAttackInterval += elapsedTime;

    switch (AttackState)
    {
    case 0:
        // アニメーション再生
        Model->f_PlayAnimation(AnimationName::allrange_attackhoming_attack);
        AttackState++;
        /*FallThrough*/
    case 1:

        if (StackAttackInterval > 0.15f)
        {
            T_ShotHoming(1);
            StackAttackInterval = 0.0f;
            Count++;
        }
        if (Count > 6)
        {
            Count = 0;
            StackAttackInterval = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        // 最初期化
        AttackState = 0;
        StackAttackInterval = 0.0f;
        CurrentAttackNumber++;
        break;
    default:

        break;
    }

}