#include"Boss_7.h"
#include"MathSupport.h"
#include "User_Function.h"
#include"EnemyManager.h"
#include"ProjectileManager.h"
#include "Audio.h"

void Boss_7::Behavior(float elapsed_time)
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

    // アニメーション再生
    if(!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(AnimationName::stand, true);
    }

    //無敵時間更新
    invTimer -= elapsed_time;

    //--------------------<残り体力に応じてエフェクトを再生>--------------------//
    PlayEffects(elapsed_time);



    //--------------------<ポイントライトを更新>--------------------//
    mPointLight->SetPosition(t.Position);
    mPointLight->SetColor(LightColor);
    mPointLight->SetRange(LightRange);

    


    GuiMenu();
}

void Boss_7::Move(float elapsed_time)
{
    BaseBoss::Move(elapsed_time);
}

void Boss_7::Collision()
{

    VsProjectile();
}

Boss_7::Boss_7(ID3D11Device* d)
    :BaseBoss(d, "./resources/Models/Characters/Boss/BossWhite.nk", "./resources/Shaders/Boss_PBR_PS.cso")
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
    laserFlag.Load("./resources/Models/Characters/Boss/BossLaserFlag.flg");
    mAnimationFlag.Load("./resources/Models/Characters/Boss/Boss1White.flg");


    //--------------------<ポイントライト初期化>--------------------//
    mPointLight = std::make_unique<GamePointLight>(d, 4);
    mPointLight->SetColor({ 1.0f,0.0f,0.0f });

    // 登場
    EnemyManager::Instance().SetStartBossAppear(true);
    EnemyManager::Instance().SetStartBossBGM(true);
    ZoomUp = true;
    status.hitPoint = static_cast<float>(MaxHitPoint);

    // エフェクトを初期化
    efk_Angry = std::make_unique<Effect>("./resources/Effects/angry.efk");
    efk_Smoke = std::make_unique<Effect>("./resources/Effects/smoke.efk");
    efk_Spark = std::make_unique<Effect>("./resources/Effects/plasma.efk");
    efk_LastBomb = std::make_unique<Effect>("./resources/Effects/bomb_more.efk");

    
    


    mGeometryData.Power = 0.0f;

    Geometry_CBuffer.Init(d, &mGeometryData);
    Geometry_CBuffer.Set_GSSlot(4);
}

Boss_7::~Boss_7()
{
}

void Boss_7::Initialize()
{
    //--------------------<姿勢を初期化>--------------------//

    t.Position = { 0.0f,0.0f,90.0f };
    t.Scale = { 0.04f,0.04f,0.04f };
    r.Angle = { 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f };

    EnemyManager::Instance().SetCameraFocusBoss(true);
    EnemyManager::Instance().SetTargetPosition(t.Position);

    EnemyManager::Instance().SetStartBossAppear(true);
    IsFirstMotion = true;

    shotRight = false;

    status.hitPoint = static_cast<float>(MaxHitPoint);
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

    //--------------------<Topの攻撃>--------------------//

    vec.emplace_back(&Boss_7::T_Energy);
    vec.emplace_back(&Boss_7::T_DiffuseShot);
    vec.emplace_back(&Boss_7::AppealTime2);
    vec.emplace_back(&Boss_7::EndAttack);
    BehaviorVec.emplace_back(vec);
    vec.clear();


    vec.emplace_back(&Boss_7::T_DiffuseShot);
    vec.emplace_back(&Boss_7::AppealTime2);
    vec.emplace_back(&Boss_7::T_Energy2);
    vec.emplace_back(&Boss_7::EndAttack);
    BehaviorVec.emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_7::T_Homing);
    vec.emplace_back(&Boss_7::AppealTime2);
    vec.emplace_back(&Boss_7::EndAttack);
    BehaviorVec.emplace_back(vec);
    vec.clear();


    vec.emplace_back(&Boss_7::T_Energy2);
    vec.emplace_back(&Boss_7::T_ShotToPlayer);
    vec.emplace_back(&Boss_7::EndAttack);
    BehaviorVec.emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_7::T_ShotToPlayer);
    vec.emplace_back(&Boss_7::AppealTime2);
    vec.emplace_back(&Boss_7::EndAttack);
    BehaviorVec.emplace_back(vec);
    vec.clear();




}



void Boss_7::Render(ID3D11DeviceContext* dc)
{
    

    Geometry_CBuffer.SetBuffer(dc, &mGeometryData, GEOMETRY);
    mPointLight->Update(dc);
    if (IsDraw)
    {
        Enemy::Render(dc);
    }

}

void Boss_7::Finalize()
{
    BaseBoss::Finalize();
}

void Boss_7::ChangeView()
{
    
}

void Boss_7::AttackManager(float elapsedTime)
{
    if (CompleteAttack)
    {
        switch (CurrentMode) {
        case TOP:
            CurrentAttackComboNumber = rand() % 5;
            CompleteAttack = false;
            break;
        case SIDE:
            CurrentAttackComboNumber = rand() % 4;
            CompleteAttack = false;
            break;
        }
    }

    (this->*BehaviorVec[CurrentAttackComboNumber][CurrentAttackNumber])(elapsedTime);
}

void Boss_7::GuiMenu()
{
#ifdef _DEBUG
    ImGui::Begin("Boss2");
    if (ImGui::Button("Next"))
    {
        status.hitPoint -= (MaxHitPoint * 0.25f + 1.0f);
    }
    ImGui::Text(("HitPoint=" + to_string(status.hitPoint)).c_str());

    ImGui::Checkbox("CompleteAttack", &CompleteAttack);
    ImGui::SliderFloat("power", &mGeometryData.Power, 0.0f, 10.0f);

    ImGui::End();
#endif
}


void Boss_7::T_DiffuseShot(float elapsedTime)
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

        AttackState = 0;
        CurrentAttackNumber++;
        break;
    default: ;
    }

   
}

void Boss_7::T_Homing(float elapsedTime)
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
            T_ShotHoming(1, 3.0f);
            T_ShotHoming(-1, 3.0f);
            StackAttackInterval = 0.0f;
            Count++;
        }
        if (Count > 2)
        {
            Count = 0;
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

void Boss_7::T_ShotToPlayer(float elapsedTime)
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
        // 撃つ
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {

            DirectX::XMFLOAT3 target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 vec = target - t.Position;
            vec = Calcu3D::Normalize(vec);
            Shot_ShotVector(BaseProjectile::Parent::REDENEMY, vec, t.Position);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        // 撃つ
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {

            DirectX::XMFLOAT3 target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 vec = target - t.Position;
            vec = Calcu3D::Normalize(vec);
            Shot_ShotVector(BaseProjectile::Parent::REDENEMY, vec, t.Position);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 3:
        // 撃つ
         // 撃つ
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {

            DirectX::XMFLOAT3 target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 vec = target - t.Position;
            vec = Calcu3D::Normalize(vec);
            Shot_ShotVector(BaseProjectile::Parent::REDENEMY, vec, t.Position);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 4:
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
    case 5:
        // 撃つ
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {

            DirectX::XMFLOAT3 target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 vec = target - t.Position;
            vec = Calcu3D::Normalize(vec);
            Shot_ShotVector(BaseProjectile::Parent::REDENEMY, vec, t.Position);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 6:
        // 撃つ
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {

            DirectX::XMFLOAT3 target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 vec = target - t.Position;
            vec = Calcu3D::Normalize(vec);
            Shot_ShotVector(BaseProjectile::Parent::REDENEMY, vec, t.Position);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 7:
        // 撃つ
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {

            DirectX::XMFLOAT3 target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 vec = target - t.Position;
            vec = Calcu3D::Normalize(vec);
            Shot_ShotVector(BaseProjectile::Parent::REDENEMY, vec, t.Position);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 8:
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
    case 9:
        // 撃つ
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {

            DirectX::XMFLOAT3 target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 vec = target - t.Position;
            vec = Calcu3D::Normalize(vec);
            Shot_ShotVector(BaseProjectile::Parent::REDENEMY, vec, t.Position);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 10:
        // 撃つ
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {

            DirectX::XMFLOAT3 target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 vec = target - t.Position;
            vec = Calcu3D::Normalize(vec);
            Shot_ShotVector(BaseProjectile::Parent::REDENEMY, vec, t.Position);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 11:
        // 撃つ
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {

            DirectX::XMFLOAT3 target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 vec = target - t.Position;
            vec = Calcu3D::Normalize(vec);
            Shot_ShotVector(BaseProjectile::Parent::REDENEMY, vec, t.Position);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 12:
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

void Boss_7::T_Energy(float elapsedTime)
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

void Boss_7::T_Energy2(float elapsedTime)
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
            rad.y -= DirectX::XMConvertToRadians(45.0f);
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
    default:;
    }
}

void Boss_7::FirstMotion(float elapsedTime)
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
            if(!IsRoar)
            {
                IsRoar = true;
                se[0]->Play(false);
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


    if (!Model->GetIsPlayAnimation()&& StackTimer< 20.0f)
    {
        Model->f_PlayAnimation(AnimationName::angry);

       
        DirectX::XMFLOAT3 launtchPos = t.Position;
        launtchPos.y -= 10.0f;
        hdl_Angry = efk_Angry->Play(launtchPos, {}, 0.4f);
    }

    if(mAnimationFlag.Check("EndStart",Model->GetCurrentAnimationNumber(),Model->GetCurrentAnimationFrame()))
    {

        StackTimer = 20.0f;
        EnemyManager::Instance().fSetZoomPower(0.0f);
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

void Boss_7::LastMotion(float elapsedTime)
{
    const int State = EnemyManager::Instance().GetLastPerformanceState();
    if(State==2)
    {
        t.Position = { 0.0f,0.0f,50.0f };
        IsDraw = false;
    }
    else if(State==6)
    {
        IsDraw = true;
    }
    else if(State==8)
    {
        Model->f_PlayAnimation(AnimationName::defeat);
        if (!LastBomb)
        {
            // エフェクト
            hdl_LastBomb = efk_LastBomb->Play(t.Position, {}, 0.4f);
        }
    }
    else if(State==9)
    {
        
    }
}



void Boss_7::PlayEffects(float elapsedTime)
{

    //--------------------<エフェクトの状態を遷移させる>--------------------//
    if(mEffectState==EffectState::None&&status.hitPoint<=MaxHitPoint*0.75f)
    {
        mEffectState = EffectState::Min;
    }
    else if(mEffectState == EffectState::Min && status.hitPoint <= MaxHitPoint * 0.5f)
    {
        mEffectState = EffectState::Middle;
        efk_Spark->Stop(hdl_Spark);
        efk_Smoke->Stop(hdl_Smoke);
    }
    else if(mEffectState == EffectState::Middle && status.hitPoint <= MaxHitPoint * 0.25f)
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
        if(StackSmokeEffectInterVal>=7.0f)
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
    default: ;
    }

    // エフェクトの位置をこうしん
    efk_Smoke->SetPosition(hdl_Smoke, t.Position);
    efk_Spark->SetPosition(hdl_Spark, t.Position);

    // エフェクトをとめる
    if(StackSmokeEffectSeconds>=6.0f)
    {
        efk_Smoke->Stop(hdl_Smoke);
    }

    // エフェクトをとめる
    if (StackSparkEffectSeconds >= 4.0f)
    {
        efk_Spark->Stop(hdl_Spark);
    }

}
