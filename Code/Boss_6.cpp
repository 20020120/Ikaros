#include"Boss_6.h"

#include "Bullet.h"
#include"EnemyManager.h"
#include "GameSystem.h"
#include "ProjectileManager.h"
#include "SceneManager.h"
#include"MathSupport.h"
#include "OptionSystem.h"
#include"User_Function.h"


void Boss_6::Behavior(float elapsed_time)
{
    //--------------------<�ŏ��̓���>--------------------//
    if (IsFirstMotion)
    {
        EnemyManager::Instance().SetCameraFocusBoss(true);
        FirstMotion(elapsed_time);
        return;
    }

    // �����̗͂��P�����ɂȂ�����
    if (status.hitPoint <= 1.0f)
    {
        status.hitPoint = 1.0f;
        efk_Spark->Stop(hdl_Spark);
        efk_Smoke->Stop(hdl_Smoke);
        EnemyManager::Instance().SetIsLastPerformance(true);
        LastMotion(elapsed_time);
        return;
    }

    // �F�⊮
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

    ChangeView(elapsed_time);

    if (IsPerformance)
    {
        ChangeViewPerformance(elapsed_time);
    }

    if (!IsPerformance && CompleteChangeView)
    {
        AttackManager(elapsed_time);
    }

    // �A�j���[�V�����Đ�
    if (!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(AnimationName::stand, true);
    }

    if (mAnimationFlag.Check("NextPerform", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(AnimationName::get_rib_of_loop);
    }


    mBeamEmitter[0].Model->Update(elapsed_time);

    mBeamEmitter->t.Position = t.Position;
    mBeamEmitter->t.Position.z -= 10.0f;
    
    mBeamEmitter[1].Model->Update(elapsed_time);

    //���G���ԍX�V
    invTimer -= elapsed_time;

    PlayEffects(elapsed_time);

    //--------------------<�|�C���g���C�g���X�V>--------------------//
    mPointLight->SetPosition(t.Position);
    mPointLight->SetColor(LightColor);
    mPointLight->SetRange(LightRange);

    GuiMenu();
}

void Boss_6::Move(float elapsed_time)
{
    BaseBoss::Move(elapsed_time);
}

void Boss_6::Collision()
{
    BaseBoss::Collision();
}

Boss_6::Boss_6(ID3D11Device* d)
    :BaseBoss(d, "./resources/Models/Characters/Boss/BossWhite.nk", "./resources/Shaders/Boss_PBR_PS.cso")
{
    //--------------------<������>--------------------//
    Initialize();

    //--------------------<���C�g�̃��f����������>--------------------//
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

    //--------------------<�C�x���g�t���O�̏�����>--------------------//
    laserFlag.Load("./resources/Models/Characters/Boss/BossLaserFlag.flg");
    mAnimationFlag.Load("./resources/Models/Characters/Boss/Boss1White.flg");

    //--------------------<�|�C���g���C�g������>--------------------//
    mPointLight = std::make_unique<GamePointLight>(d, 4);
    mPointLight->SetColor({ 1.0f,0.0f,0.0f });

    

    // �o��
    EnemyManager::Instance().SetStartBossAppear(true);
    EnemyManager::Instance().SetStartBossBGM(true);

    status.hitPoint = static_cast<float>(MaxHitPoint);

    // �G�t�F�N�g��������
    efk_Angry = std::make_unique<Effect>("./resources/Effects/angry.efk");
    efk_Smoke = std::make_unique<Effect>("./resources/Effects/smoke.efk");
    efk_Spark = std::make_unique<Effect>("./resources/Effects/plasma.efk");
    efk_LastBomb = std::make_unique<Effect>("./resources/Effects/bomb_more.efk");
}

Boss_6::~Boss_6()
{
}

void Boss_6::Initialize()
{

    //--------------------<�p����������>--------------------//

    t.Position = { 0.0f,0.0f,90.0f };
    t.Scale = { 0.04f,0.04f,0.04f };
    r.Angle = { 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f };

    EnemyManager::Instance().SetCameraFocusBoss(true);
    EnemyManager::Instance().SetTargetPosition(t.Position);

    IsFirstMotion = true;

    shotRight = false;

    status.hitPoint = MaxHitPoint; // �̗͂̏�����
    IsInputChangeView = false;
    CompleteChangeView = true;
    CurrentAttackNumber = 0;
    StackAttackInterval = 0.0f;
    CurrentAttackComboNumber = 0;
    ShotRadian = {};
    Ratio = {};
    ZoomUp = true;

    //LightRange = { 0.0f };
    //LightColor = { 1.0f,0.0f,0.0f };

    type = Type::RED;
    EnemyManager::Instance().SetStartBossAppear(true);


    //****************************************************************
    // 
    // �֐��|�C���^��������
    // 
    //****************************************************************
    std::vector<pBehavior> vec{};

    //--------------------<Top�̍U��>--------------------//

    vec.emplace_back(&Boss_6::T_Energy);
    vec.emplace_back(&Boss_6::T_StepAllRangeAttack);
    vec.emplace_back(&Boss_6::T_StepToPlayerShot);
    vec.emplace_back(&Boss_6::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();


    vec.emplace_back(&Boss_6::T_Energy);
    vec.emplace_back(&Boss_6::T_StepToPlayerShot);
    vec.emplace_back(&Boss_6::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();



    vec.emplace_back(&Boss_6::T_Horming);
    vec.emplace_back(&Boss_6::AppealTime);
    vec.emplace_back(&Boss_6::T_Horming);
    vec.emplace_back(&Boss_6::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_6::AppealTime);
    vec.emplace_back(&Boss_6::T_StepToPlayerShot);
    vec.emplace_back(&Boss_6::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_6::T_Energy);
    vec.emplace_back(&Boss_6::T_Energy);
    vec.emplace_back(&Boss_6::T_StepAllRangeAttack);
    vec.emplace_back(&Boss_6::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    
    vec.emplace_back(&Boss_6::AppealTime);
    vec.emplace_back(&Boss_6::T_StepToPlayerShot);
    vec.emplace_back(&Boss_6::T_Beam);
    vec.emplace_back(&Boss_6::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();


    //--------------------<Side>--------------------//

    vec.emplace_back(&Boss_6::S_MoveFirstPosition);
    vec.emplace_back(&Boss_6::S_Boonmerang);
    vec.emplace_back(&Boss_6::S_Energy);
    vec.emplace_back(&Boss_6::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_6::S_Boonmerang);
    vec.emplace_back(&Boss_6::S_Boonmerang);
    vec.emplace_back(&Boss_6::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_6::T_Horming);
    vec.emplace_back(&Boss_6::S_Energy);
    vec.emplace_back(&Boss_6::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_6::S_DiffuseShot);
    vec.emplace_back(&Boss_6::T_Horming);
    vec.emplace_back(&Boss_6::S_Boonmerang);
    vec.emplace_back(&Boss_6::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_6::S_DiffuseShot);
    vec.emplace_back(&Boss_6::S_SummonEnemy);
    vec.emplace_back(&Boss_6::AppealTime);
    vec.emplace_back(&Boss_6::S_Boonmerang);
    vec.emplace_back(&Boss_6::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_6::S_Boonmerang);
    vec.emplace_back(&Boss_6::AppealTime);
    vec.emplace_back(&Boss_6::T_Horming);
    vec.emplace_back(&Boss_6::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_6::S_Boonmerang);
    vec.emplace_back(&Boss_6::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_6::T_Horming);
    vec.emplace_back(&Boss_6::S_Boonmerang);
    vec.emplace_back(&Boss_6::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();
    IsRoar = false;
}

void Boss_6::PlayEffects(float elapsedTime)
{

    //--------------------<�G�t�F�N�g�̏�Ԃ�J�ڂ�����>--------------------//
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

    //--------------------<��Ԃɉ������G�t�F�N�g���Đ�����>--------------------//

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

    // �G�t�F�N�g�̈ʒu����������
    efk_Smoke->SetPosition(hdl_Smoke, t.Position);
    efk_Spark->SetPosition(hdl_Spark, t.Position);

    // �G�t�F�N�g���Ƃ߂�
    if (StackSmokeEffectSeconds >= 6.0f)
    {
        efk_Smoke->Stop(hdl_Smoke);
    }

    // �G�t�F�N�g���Ƃ߂�
    if (StackSparkEffectSeconds >= 4.0f)
    {
        efk_Spark->Stop(hdl_Spark);
    }

}


void Boss_6::LastMotion(float elapsedTime)
{
    const int State = EnemyManager::Instance().GetLastPerformanceState();
    if (State == 2)
    {
        t.Position = { 0.0f,0.0f,50.0f };
        IsDraw = false;
        mPointLight->SetRange(0.0f);
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
            // �G�t�F�N
            hdl_LastBomb = efk_LastBomb->Play(t.Position, {}, 0.4f);
        }
    }
}

void Boss_6::Render(ID3D11DeviceContext* dc)
{
    mPointLight->Update(dc);
    Enemy::Render(dc);
    if (ShowBeamEmitter)
    {
        mBeamEmitter[0].Model->Render(dc, mBeamEmitter[0].t, mBeamEmitter[0].r);
        mBeamEmitter[1].Model->Render(dc, mBeamEmitter[1].t, mBeamEmitter[1].r);
    }
}

void Boss_6::Finalize()
{
    BaseBoss::Finalize();
}

void Boss_6::ChangeView(float elapsedTime)
{
    // ���_�ύX������̇T�t���[������
    if (!CompleteChangeView && !GameSystem::Instance().GetChangingView())
    {
        CurrentAttackComboNumber = 0;
        CompleteChangeView = true;
        CompleteAttack = false;
        CurrentAttackNumber = 0;
        ZoomPower = 0.0f;
        EnemyManager::Instance().fSetZoomPower(0.0f);
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
            Model->f_PlayAnimation(AnimationName::B);
            ZoomPower = 0.0f;
            ZoomUp = true;
            IsRoar = false;
        }
    }

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

    if (IsInputChangeView)
    {
        GameSystem::Instance().ChangeView();
        IsInputChangeView = false;
        CompleteChangeView = false;
    }
}

void Boss_6::AttackManager(float elapsedTime)
{

    if (CompleteAttack)
    {
        switch (CurrentMode) {
        case TOP:
            CurrentAttackComboNumber = rand() % 6;
            CompleteAttack = false;
            break;
        case SIDE:
            CurrentAttackComboNumber = rand() % 7 + 1;
            CompleteAttack = false;
            break;
        }
    }

    (this->*BehaviorVec[CurrentMode][CurrentAttackComboNumber][CurrentAttackNumber])(elapsedTime);
}

void Boss_6::GuiMenu()
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

void Boss_6::T_Beam(float elapsedTime)
{

    //--------------------<�r�[������>--------------------//
    switch (AttackState)
    {
    case 0:
    {
        ShowBeamEmitter = true;
        mBeamEmitter[0].Model->f_PlayAnimation(0);
        Model->f_PlayAnimation(AnimationName::beam_begin);
       
        AttackState++;
        CompleteAttack = false;
        /*FallThrough*/


    }
    case 1:

        if(mAnimationFlag.Check("Laser",Model->GetCurrentAnimationNumber(),Model->GetCurrentAnimationFrame()))
        {
            Model->f_PlayAnimation(AnimationName::beam_loop, true);
            se[LaserCharge]->Stop();
            se[LaserCharge]->Play(false);
        }


        if (laserFlag.Check("ShotBeam", mBeamEmitter[0].Model->GetCurrentAnimationNumber(), mBeamEmitter[0].Model->GetCurrentAnimationFrame()))
        {
            AttackState++;
        }
        break;
    case 2:
    {
        StackAttackInterval = 0.0f;
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        mPointLight->SetRange(-4.0f);
        auto b0 = new Beam(p_device, BaseProjectile::Parent::REDENEMY,
            t.Position);
        ProjectileManager::Instance().RegisterProjectile(b0);
        se[LaserCharge]->Stop();
        se[LaserShot]->Stop();
        se[LaserShot]->Play(false);
        AttackState++;
    }
    case 3:

        if (laserFlag.Check("EndBeam", mBeamEmitter[0].Model->GetCurrentAnimationNumber(), mBeamEmitter[0].Model->GetCurrentAnimationFrame()))
        {
            mPointLight->SetRange(0.0f);
        }

        StackAttackInterval += elapsedTime;
        if (StackAttackInterval > 5.0f)
        {
            AttackState++;
            mPointLight->SetRange(0.0f);
            Model->f_PlayAnimation(AnimationName::beam_end);
        }
        break;
    case 4:
        CurrentAttackNumber++;
        AttackState = 0;
        StackAttackInterval = 0.0f;
        ShowBeamEmitter = false;
        se[LaserShot]->Stop();
        break;

    }

}

void Boss_6::T_StepAllRangeAttack(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        // �������ĉE�Ɉړ�
        Ratio += elapsedTime * 3.0f;

        t.Position.x = Calcu3D::LerpFloat(0.0f, 20.0f, Ratio);  // �ړ�
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-180.0f), DirectX::XMConvertToRadians(-540.0f), Ratio);
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 1:
      // �ł�
    {
        se[BLUE_BULLET_SHOT]->Stop();
        se[BLUE_BULLET_SHOT]->Play(false,
            OptionSystem::Instance().GetSeVolume() * ShotVolume);
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        const int Max = 10;
        // �S�̂ɍU��
        for (int i = 0; i < Max; i++)
        {
            // ���v�� 360�ǂɂȂ�悤��

            float Deg = static_cast<float>(i) * (360.0f / static_cast<float>(Max)); // Y���̊p�x�i�f�B�O���[�j
            if (Deg > 360.0f) Deg -= 360.0f;

            Deg = DirectX::XMConvertToRadians(Deg);
            const DirectX::XMFLOAT3 Vel = { sinf(Deg),0.0f,cosf(Deg) }; // �i�s����

            auto b = new Bullet(p_device, BaseProjectile::Parent::REDENEMY, t.Position, Vel);
            ProjectileManager::Instance().RegisterProjectile(b);
        }
        AttackState++;
    }
        break;
    case 2:
        // �������č��Ɉړ�
        Ratio += elapsedTime * 2.0f;
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-540.0f), DirectX::XMConvertToRadians(-180.0f), Ratio);
        t.Position.x = Calcu3D::LerpFloat(20.0f, -20.0f, Ratio);  // �ړ�
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 3:
        // ����
          // �ł�
    {
        se[BLUE_BULLET_SHOT]->Stop();
        se[BLUE_BULLET_SHOT]->Play(false,
            OptionSystem::Instance().GetSeVolume() * ShotVolume);
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        const int Max = 10;
        // �S�̂ɍU��
        for (int i = 0; i < Max; i++)
        {
            // ���v�� 360�ǂɂȂ�悤��

            float Deg = static_cast<float>(i) * (360.0f / static_cast<float>(Max)); // Y���̊p�x�i�f�B�O���[�j
            if (Deg > 360.0f) Deg -= 360.0f;

            Deg = DirectX::XMConvertToRadians(Deg);
            const DirectX::XMFLOAT3 Vel = { sinf(Deg),0.0f,cosf(Deg) }; // �i�s����

            auto b = new Bullet(p_device, BaseProjectile::Parent::REDENEMY, t.Position, Vel);
            ProjectileManager::Instance().RegisterProjectile(b);
        }
        AttackState++;
    }
        break;
    case 4:
        // �߂�
        Ratio += elapsedTime * 2.0f;
        t.Position.x = Calcu3D::LerpFloat(-20.0f, 0.0f, Ratio);  // �ړ�
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-180.0f), DirectX::XMConvertToRadians(-540.0f), Ratio);
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
            r.Angle.y = DirectX::XMConvertToRadians(-180.0f);
        }
        break;
    case 5:
        // �ł�
    {
        se[BLUE_BULLET_SHOT]->Stop();
        se[BLUE_BULLET_SHOT]->Play(false,
            OptionSystem::Instance().GetSeVolume() * ShotVolume);
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        const int Max = 10;
        // �S�̂ɍU��
        for (int i = 0; i < Max; i++)
        {
            // ���v�� 360�ǂɂȂ�悤��

            float Deg = static_cast<float>(i) * (360.0f / static_cast<float>(Max)); // Y���̊p�x�i�f�B�O���[�j
            if (Deg > 360.0f) Deg -= 360.0f;

            Deg = DirectX::XMConvertToRadians(Deg);
            const DirectX::XMFLOAT3 Vel = { sinf(Deg),0.0f,cosf(Deg) }; // �i�s����

            auto b = new Bullet(p_device, BaseProjectile::Parent::REDENEMY, t.Position, Vel);
            ProjectileManager::Instance().RegisterProjectile(b);
        }
        AttackState++;
    }
        break;
    case 6:
        /// <summary>
        /// �ŏ�����
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

void Boss_6::T_Energy(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::change_camera);
        AttackState++;
    case 1:
        // �O�����ɔ���
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

void Boss_6::T_Horming(float elapsedTime)
{
    static int Count = 0;
    StackAttackInterval += elapsedTime;
    if (StackAttackInterval > 0.3f)
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
        CurrentAttackNumber++;
    }

}

void Boss_6::T_StepToPlayerShot(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        // �������ĉE�Ɉړ�
        Ratio += elapsedTime * 3.0f;

        t.Position.x = Calcu3D::LerpFloat(0.0f, 20.0f, Ratio);  // �ړ�
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-180.0f), DirectX::XMConvertToRadians(-540.0f), Ratio);
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 1:
        // ����
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
        // ����
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
        // ����
         // ����
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
        // �������č��Ɉړ�
        Ratio += elapsedTime * 2.0f;
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-540.0f), DirectX::XMConvertToRadians(-180.0f), Ratio);
        t.Position.x = Calcu3D::LerpFloat(20.0f, -20.0f, Ratio);  // �ړ�
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 5:
        // ����
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
        // ����
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
        // ����
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
        // �߂�
        Ratio += elapsedTime * 2.0f;
        t.Position.x = Calcu3D::LerpFloat(-20.0f, 0.0f, Ratio);  // �ړ�
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-180.0f), DirectX::XMConvertToRadians(-540.0f), Ratio);
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
            r.Angle.y = DirectX::XMConvertToRadians(-180.0f);
        }
        break;
    case 9:
        // ����
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
        // ����
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
        // ����
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
       /// �ŏ�����
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

void Boss_6::S_Horming(float elapsedTime)
{
  
}

void Boss_6::S_Boonmerang(float elapsedTime)
{

    switch(AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::overrall);
        AttackState++;
        /*FallThrough*/
    case 1:
        StackAttackInterval += elapsedTime;
        Ratio += elapsedTime;

        if (StackAttackInterval >= 0.25f)
        {
            se[BLUE_BULLET_SHOT]->Stop();
            se[BLUE_BULLET_SHOT]->Play(false,
                OptionSystem::Instance().GetSeVolume() * ShotVolume);
            auto b0 = new BoomerangBullet(BaseProjectile::Parent::BLUEENEMY, t.Position, { 0.0f,30.0f,-90.0f });
            ProjectileManager::Instance().RegisterProjectile(b0);
            auto b1 = new BoomerangBullet(BaseProjectile::Parent::BLUEENEMY, t.Position, { 0.0f,25.0f,-80.0f });
            ProjectileManager::Instance().RegisterProjectile(b1);
            StackAttackInterval = 0.0f;
        }

        if (Ratio >= 1.0f)
        {
            StackAttackInterval = 0.0f;
            Ratio = 0.0f;
            AttackState++;
        
        }
    case 2:
        AttackState = 0;
        CurrentAttackNumber++;
        break;
        break;

    }

  

    
}

void Boss_6::S_Energy(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
        Model->f_PlayAnimation(AnimationName::change_camera);
        AttackState++;

    case 1:
        // �O�����ɔ���
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

void Boss_6::S_SummonEnemy(float elapsedTime)
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
            Model->f_PlayAnimation(AnimationName::homing);
        }
        break;
    case 2:
    {
        static int Num = 0;
        // ����
        Ratio += elapsedTime;
        if (Ratio >= 0.15f)
        {

            DirectX::XMFLOAT3 target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 vec = target - t.Position;
            vec = Calcu3D::Normalize(vec);
            Shot_ShotVector(BaseProjectile::Parent::REDENEMY, vec, t.Position);

            auto b1 = new StayToTargetBullet(BaseProjectile::Parent::BLUEENEMY, t.Position, 1,
                { 0.0f,static_cast<float>(rand()%60)-30.0f,static_cast<float>(rand() % 60) - 30.0f }, 1.6f);
            ProjectileManager::Instance().RegisterProjectile(b1);

            Ratio = 0.0f;
            Num++;
        }

        if (Num >= 7)
        {
            Ratio = 0.0f;
            Num = 0;
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

void Boss_6::S_DiffuseShot(float elapsedTime)
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
        }
        break;
    case 2:
    {
        static int Num = 0;
        // ����
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {

            DirectX::XMFLOAT3 target = ProjectileManager::Instance().GetTarget();
            DirectX::XMFLOAT3 vec = target - t.Position;
            vec = Calcu3D::Normalize(vec);
            Shot_ShotVector(BaseProjectile::Parent::REDENEMY, vec, t.Position);

            auto b1 = new StayToTargetBullet(BaseProjectile::Parent::BLUEENEMY, t.Position, 1,
                { 0.0f,static_cast<float>(rand() % 60) - 30.0f,static_cast<float>(rand() % 60) - 30.0f }, 1.6f);
            ProjectileManager::Instance().RegisterProjectile(b1);

            Ratio = 0.0f;
            Num++;
        }

            if(Num>=5)
            {
                Ratio = 0.0f;
                Num = 0;
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

void Boss_6::FirstMotion(float elapsedTime)
{
    // �{�X�̂ق��ɃJ�����������ă{�X�o�ꃂ�[�V����
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

    if (!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(AnimationName::angry);
       
    }

    if (mAnimationFlag.Check("EndStart", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        StackTimer = 20.0f;
        EnemyManager::Instance().fSetZoomPower(0.0f);
    }


    // ���ꂪ�I�������
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
