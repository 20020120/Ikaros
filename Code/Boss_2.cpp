#include"GameSystem.h"

#include<imgui.h>

#include"Boss_2.h"
#include"EnemyManager.h"
#include "SceneManager.h"
#include"ProjectileManager.h"
#include "Bullet.h"
#include"MathSupport.h"

Boss_2::Boss_2(ID3D11Device* d)
    :BaseBoss(d, "./resources/Models/Characters/Boss/Boss2.nk", "./resources/Shaders/PhisicsBase_PS.cso")
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

    //--------------------<�|�C���g���C�g������>--------------------//
    mPointLight = std::make_unique<GamePointLight>(d, 4);
    mPointLight->SetColor({ 1.0f,0.0f,0.0f });

    // �o��
    EnemyManager::Instance().SetStartBossAppear(true);
    EnemyManager::Instance().SetStartBossBGM(true);
}

void Boss_2::Initialize()
{
    //--------------------<�p����������>--------------------//

    t.Position = { 0.0f,0.0f,90.0f };
    t.Scale = { 0.04f,0.04f,0.04f };
    r.Angle = { 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f };

    EnemyManager::Instance().SetCameraFocusBoss(true);
    EnemyManager::Instance().SetTargetPosition(t.Position);

    IsFirstMotion = true;

    shotRight = false;

    status.hitPoint = MaxHp; // �̗͂̏�����
    IsInputChangeView = false;
    CompleteChangeView = true;
    CurrentAttackNumber = 0;
    StackAttackInterval = 0.0f;
    CurrentAttackComboNumber = 0;
    ShotRadian = {};
    Ratio = {};


    LightRange = { 0.0f };
    LightColor = { 1.0f,0.0f,0.0f };

    type = Type::RED;


    //****************************************************************
    // 
    // �֐��|�C���^�̏�����
    // 
    //****************************************************************
    std::vector<pBehavior> vec{};

    //--------------------<Top�̍U��>--------------------//
    vec.emplace_back(&Boss_2::T_Horming);
    vec.emplace_back(&Boss_2::T_Beam);
    vec.emplace_back(&Boss_2::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    
    vec.emplace_back(&Boss_2::T_Horming);
    vec.emplace_back(&Boss_2::T_Nagiri);
    vec.emplace_back(&Boss_2::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_2::T_AllRangeAttack);
    vec.emplace_back(&Boss_2::T_MoveBeam);
    vec.emplace_back(&Boss_2::T_Nagiri);
    vec.emplace_back(&Boss_2::EndAttack);
    BehaviorVec[TOP].emplace_back(vec);
    vec.clear();

    //--------------------<Side�̍U��>--------------------//
    vec.emplace_back(&Boss_2::S_MoveFirstPosition);
    vec.emplace_back(&Boss_2::S_CrossAttack0);
    vec.emplace_back(&Boss_2::S_CrossAttack1);
    vec.emplace_back(&Boss_2::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_2::S_RandamAttack);
    vec.emplace_back(&Boss_2::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_2::S_CrossAttack0);
    vec.emplace_back(&Boss_2::S_CrossAttack1);
    vec.emplace_back(&Boss_2::S_Beam);
    vec.emplace_back(&Boss_2::S_CrossAttack0);
    vec.emplace_back(&Boss_2::S_CrossAttack1);
    vec.emplace_back(&Boss_2::S_CrossAttack0);
    vec.emplace_back(&Boss_2::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_2::S_CrossAttack0);
    vec.emplace_back(&Boss_2::S_RandamAttack);
    vec.emplace_back(&Boss_2::S_CrossAttack1);
    vec.emplace_back(&Boss_2::S_RandamAttack);
    vec.emplace_back(&Boss_2::EndAttack);
    BehaviorVec[SIDE].emplace_back(vec);
    vec.clear();

    
    //--------------------<�������Top�̍U��>--------------------//
    vec.emplace_back(&Boss_2::T_MoveFirstPosition);
    vec.emplace_back(&Boss_2::T_RandamSpeedAttack);
    vec.emplace_back(&Boss_2::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_2::T_MoveBeam);
    vec.emplace_back(&Boss_2::T_RandamSpeedAttack);
    vec.emplace_back(&Boss_2::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_2::T_Beam2);
    vec.emplace_back(&Boss_2::T_AllRangeAttack);
    vec.emplace_back(&Boss_2::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_2::T_RandamSpeedAttack);
    vec.emplace_back(&Boss_2::T_Nagiri);
    vec.emplace_back(&Boss_2::EndAttack);
    BehaviorVec[ANGRY_TOP].emplace_back(vec);
    vec.clear();



    //--------------------<�������Side�̍U��>--------------------//
    vec.emplace_back(&Boss_2::S_MoveFirstPosition);
    vec.emplace_back(&Boss_2::S_NagiriFast);
    vec.emplace_back(&Boss_2::S_NagiriSlow);
    vec.emplace_back(&Boss_2::S_NagiriFast2);
    vec.emplace_back(&Boss_2::S_NagiriSlow2);
    vec.emplace_back(&Boss_2::EndAttack);
    BehaviorVec[ANGRY_SIDE].emplace_back(vec);
    vec.clear();


    vec.emplace_back(&Boss_2::S_RandamAttack);
    vec.emplace_back(&Boss_2::S_NagiriSlow2);
    vec.emplace_back(&Boss_2::S_RandamAttack);
    vec.emplace_back(&Boss_2::S_NagiriFast2);
    vec.emplace_back(&Boss_2::EndAttack);
    BehaviorVec[ANGRY_SIDE].emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_2::S_Horming);
    vec.emplace_back(&Boss_2::S_Beam);
    vec.emplace_back(&Boss_2::EndAttack);
    BehaviorVec[ANGRY_SIDE].emplace_back(vec);
    vec.clear();


}

void Boss_2::Render(ID3D11DeviceContext* dc)
{
    Enemy::Render(dc);
    mPointLight->Update(dc);
    if (ShowBeamEmitter)
    {
        mBeamEmitter[0].Model->Render(dc, mBeamEmitter[0].t, mBeamEmitter[0].r);
        mBeamEmitter[1].Model->Render(dc, mBeamEmitter[1].t, mBeamEmitter[1].r);
    }
}

void Boss_2::Finalize()
{
    
}

void Boss_2::Behavior(float elapsed_time)
{
    //--------------------<�ŏ��̓���>--------------------//
    if(IsFirstMotion)
    {
        EnemyManager::Instance().SetCameraFocusBoss(true);
        FirstMotion(elapsed_time);
        return;
    }


    ChangeView(elapsed_time);

    if(IsPerformance)
    {
        ChangeViewPerformance(elapsed_time);
    }

    if (!IsPerformance && CompleteChangeView)
    {
        AttackManager(elapsed_time);
    }

    mBeamEmitter[0].Model->Update(elapsed_time);
    mBeamEmitter[1].Model->Update(elapsed_time);

    //���G���ԍX�V
    invTimer -= elapsed_time;

    //--------------------<�|�C���g���C�g���X�V>--------------------//
    mPointLight->SetPosition(t.Position);
    mPointLight->SetColor(LightColor);
    mPointLight->SetRange(LightRange);

    GuiMenu();
}

void Boss_2::Move(float elapsed_time)
{
    
}

void Boss_2::Collision()
{
    VsProjectile();
}

//--------------------<�ŏ��̓���>--------------------//
void Boss_2::FirstMotion(float elapsedTime)
{

    // �{�X�̂ق��ɃJ�����������ă{�X�o�ꃂ�[�V����
    StackTimer += elapsedTime;
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


void Boss_2::AttackManager(float elapsedTime)
{
    if (CompleteAttack)
    {
        switch (CurrentMode) {
        case TOP:
            CurrentAttackComboNumber = static_cast<int>(rand() % 3);
            CompleteAttack = false;
            break;
        case SIDE:
            CurrentAttackComboNumber = static_cast<int>(rand() % 3) + 1;
            CompleteAttack = false;
            break;
        case ANGRY_TOP:
            CurrentAttackComboNumber = static_cast<int>(rand() % 3) + 1;
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





void Boss_2::GuiMenu()
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



//****************************************************************
// 
//  �֐��|�C���^�̒��g
// 
//****************************************************************

void Boss_2::T_MoveFirstPosition(float elapsedTime)
{
    t.Position.z -= Speed * elapsedTime;
    t.Position.y = 0.0f;
    if (t.Position.z < 20.0f)
    {
        CurrentAttackNumber++;
    }
}

void Boss_2::S_MoveFirstPosition(float elapsedTime)
{
    t.Position.z += Speed * elapsedTime;
    t.Position.x = 0.0f;
    if (t.Position.z > 40.0f)
    {
        CurrentAttackNumber++;
    }
}

void Boss_2::EndAttack(float elapsedTime)
{
    CompleteAttack = true;
    CurrentAttackNumber = 0;
}

void Boss_2::T_Beam(float elapsedTime)
{
    //--------------------<�r�[������>--------------------//
    switch (AttackState)
    {
    case 0:
    {
        ShowBeamEmitter = true;
        mBeamEmitter[0].Model->f_PlayAnimation(0);
        mBeamEmitter[1].Model->f_PlayAnimation(0);

        mBeamEmitter[0].t = t;
        mBeamEmitter[0].t.Position.x += 20.0f;

        mBeamEmitter[1].t = t;
        mBeamEmitter[1].t.Position.x -= 20.0f;

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
                p.x -= 10.0f;
            }
            else
            {
                p.x += 10.0f;
            }

            shotRight = !shotRight;

            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, { 0.0f,DirectX::XMConvertToRadians(ang + 180.0f),0.0f }, p);
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
        if(Ratio>0.8f)
        {
            DirectX::XMFLOAT3 p = t.Position;
            float ang = static_cast<float>(rand() % 40) - 20.0f;
            if (shotRight)
            {
                p.x -= 10.0f;
            }
            else
            {
                p.x += 10.0f;
            }

            shotRight = !shotRight;
            
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, {0.0f,DirectX::XMConvertToRadians(ang+180.0f),0.0f}, p);
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
    default: ;
    }
}

void Boss_2::T_Beam2(float elapsedTime)
{
    //--------------------<�r�[������>--------------------//
    switch (AttackState)
    {
    case 0:
    {
        ShowBeamEmitter = true;
        mBeamEmitter[0].Model->f_PlayAnimation(0);
        mBeamEmitter[1].Model->f_PlayAnimation(0);

        mBeamEmitter[0].t = t;
        mBeamEmitter[0].t.Position.x += 20.0f;

        mBeamEmitter[1].t = t;
        mBeamEmitter[1].t.Position.x -= 20.0f;

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
        if (Ratio > 0.25f)
        {
            DirectX::XMFLOAT3 p = t.Position;
            float ang = static_cast<float>(rand() % 80) - 40.0f;
            if (shotRight)
            {
                p.x -= 10.0f;
            }
            else
            {
                p.x += 10.0f;
            }

            shotRight = !shotRight;

            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, { 0.0f,DirectX::XMConvertToRadians(ang + 180.0f),0.0f }, p);
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
        if (Ratio > 0.5f)
        {
            DirectX::XMFLOAT3 p = t.Position;
            float ang = static_cast<float>(rand() % 40) - 20.0f;
            if (shotRight)
            {
                p.x -= 10.0f;
            }
            else
            {
                p.x += 10.0f;
            }

            shotRight = !shotRight;

            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, { 0.0f,DirectX::XMConvertToRadians(ang + 180.0f),0.0f }, p);
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


void Boss_2::T_MoveBeam(float elapsedTime)
{
    //--------------------<�r�[������>--------------------//
    switch (AttackState)
    {
    case 0:
    {
        ShowBeamEmitter = true;
        mBeamEmitter[0].Model->f_PlayAnimation(0);

        mBeamEmitter[0].t = t;
        mBeamEmitter[0].t.Position.x += 40.0f;


        AttackState++;

        /*FallThrough*/


    }
    case 1:
        if (laserFlag.Check("ShotBeam", mBeamEmitter[0].Model->GetCurrentAnimationNumber(), mBeamEmitter[0].Model->GetCurrentAnimationFrame()))
        {
            AttackState++;
        }
        break;
    case 2:
    {

        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        DirectX::XMFLOAT3 p = mBeamEmitter[0].t.Position;
        auto b0 = new MoveBeam(p_device, BaseProjectile::Parent::REDENEMY, p, { Speed * -1.0f,0.0f,0.0f });
        ProjectileManager::Instance().RegisterProjectile(b0);

        AttackState++;
    }
    case 3:
        if (laserFlag.Check("EndBeam", mBeamEmitter[0].Model->GetCurrentAnimationNumber(), mBeamEmitter[0].Model->GetCurrentAnimationFrame()))
        {
        }
        mBeamEmitter[0].t.Position.x -= Speed * elapsedTime;



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

void Boss_2::T_Nagiri(float elapsedTime)
{

    constexpr float Begin = { DirectX::XMConvertToRadians(100.0f) };
    constexpr float End = { DirectX::XMConvertToRadians(250.0f) };

    constexpr float s = 1.0f;


    StackAttackInterval += elapsedTime;
    const bool Attack = StackAttackInterval > 0.2f;

    // �ガ�����U��
    switch (AttackState)
    {
    case 0:
        // �ガ�����U��
        Ratio += elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, ShotRadian,t.Position);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 1:
        // �ガ�����U��
        Ratio -= elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, ShotRadian, t.Position);
            StackAttackInterval = 0.0f;
        }
        if (Ratio < 0.0f)
        {
            AttackState++;
        }
        break;
    case 2:
        // �ガ�����U��
        Ratio += elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);

        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, ShotRadian, t.Position);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 3:
        // �I��
        CurrentAttackNumber++;
        AttackState = 0;
        Ratio = 0.0f;
        StackAttackInterval = 0.0f;
        ShotRadian = { 0.0f,0.0f,0.0f };
        break;
    }
}



void Boss_2::T_AllRangeAttack(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
    {
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        const int Max = 20;
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

    case 1:
    {
        // �I��
        AttackState = 0;
        CurrentAttackNumber++;
    }
    break;

    default:
        break;;
    }
}

void Boss_2::T_Horming(float elapsedTime)
{
    static int Count = 0;
    StackAttackInterval += elapsedTime;
    if (StackAttackInterval > 0.2f)
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
        CurrentAttackNumber++;
    }


}

void Boss_2::S_Horming(float elapsedTime)
{
    static int Count = 0;
    StackAttackInterval += elapsedTime;
    if (StackAttackInterval > 0.2f)
    {
        T_ShotHoming2(1);
        T_ShotHoming2(-1);
        StackAttackInterval = 0.0f;
        Count++;
    }
    if (Count > 4)
    {
        Count = 0;
        StackAttackInterval = 0.0f;
        CurrentAttackNumber++;
    }


}




void Boss_2::S_CrossAttack0(float elapsedTime)
{
    const float Begin = DirectX::XMConvertToRadians(90.0f);
    const float Begin2 = DirectX::XMConvertToRadians(-90.0f);

    const float End = 0.0f;

    Ratio += elapsedTime * 1.0f;
    StackAttackInterval += elapsedTime;

    if (StackAttackInterval > 0.07f)
    {
        DirectX::XMFLOAT3 Radian{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        DirectX::XMFLOAT3 Radian2{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        Radian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        Radian2.x = Calcu3D::LerpFloat(Begin2, End, Ratio);

        Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, Radian,t.Position);

        StackAttackInterval = 0.0f;
    }

    if (Ratio > 1.0f)
    {
        Ratio = 0.0f;
        CurrentAttackNumber++;
        StackAttackInterval++;
    }
}



void Boss_2::S_CrossAttack1(float elapsedTime)
{
    const float Begin = DirectX::XMConvertToRadians(90.0f);
    const float Begin2 = DirectX::XMConvertToRadians(-90.0f);

    const float End = 0.0f;

    Ratio += elapsedTime * 1.0f;
    StackAttackInterval += elapsedTime;

    if (StackAttackInterval > 0.07f)
    {
        DirectX::XMFLOAT3 Radian{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        DirectX::XMFLOAT3 Radian2{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        Radian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        Radian2.x = Calcu3D::LerpFloat(Begin2, End, Ratio);

        Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, Radian2, t.Position);

        StackAttackInterval = 0.0f;
    }

    if (Ratio > 1.0f)
    {
        Ratio = 0.0f;
        CurrentAttackNumber++;
        StackAttackInterval++;
    }
}

void Boss_2::S_RandamAttack(float elapsedTime)
{
    const float Begin = DirectX::XMConvertToRadians(90.0f);
    const float Begin2 = DirectX::XMConvertToRadians(-90.0f);


    Ratio += elapsedTime * 0.5f;
    StackAttackInterval += elapsedTime;

    if (StackAttackInterval > 0.07f)
    {
        DirectX::XMFLOAT3 Radian{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };

        const float rat = static_cast<float>(rand())/ RAND_MAX;
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


void Boss_2::S_Beam(float elapsedTime)
{
    //--------------------<�r�[������>--------------------//
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


void Boss_2::T_RandamSpeedAttack(float elapsedTime)
{
    const float Begin = DirectX::XMConvertToRadians(90.0f);
    const float Begin2 = DirectX::XMConvertToRadians(-90.0f);


    Ratio += elapsedTime * 1.0f;
    StackAttackInterval += elapsedTime;

    if (StackAttackInterval > 0.07f)
    {
        DirectX::XMFLOAT3 Radian{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };

        const float rat = static_cast<float>(rand()) / RAND_MAX;
        Radian.y += Calcu3D::LerpFloat(Begin, Begin2, rat);

        switch (rand()%3)
        {
        case 0:

            Shot_ShotRadianSlow(BaseProjectile::Parent::BLUEENEMY, Radian, t.Position);

            break;
        case 1:

            Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, Radian, t.Position);
            break;
        case 2:

            Shot_ShotRadianFast(BaseProjectile::Parent::BLUEENEMY, Radian, t.Position);
            break;
        default: ;
        }

        StackAttackInterval = 0.0f;
    }

    if (Ratio > 1.0f)
    {
        Ratio = 0.0f;
        CurrentAttackNumber++;
        StackAttackInterval++;
    }
}


void Boss_2::S_NagiriSlow(float elapsedTime)
{
    const float Begin = DirectX::XMConvertToRadians(90.0f);
    const float Begin2 = DirectX::XMConvertToRadians(-90.0f);

    const float End = 0.0f;

    Ratio += elapsedTime * 1.0f;
    StackAttackInterval += elapsedTime;

    if (StackAttackInterval > 0.07f)
    {
        DirectX::XMFLOAT3 Radian{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        DirectX::XMFLOAT3 Radian2{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        Radian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        Radian2.x = Calcu3D::LerpFloat(Begin2, End, Ratio);

        Shot_ShotRadianSlow(BaseProjectile::Parent::BLUEENEMY, Radian, t.Position);

        StackAttackInterval = 0.0f;
    }

    if (Ratio > 1.0f)
    {
        Ratio = 0.0f;
        CurrentAttackNumber++;
        StackAttackInterval++;
    }
}

void Boss_2::S_NagiriSlow2(float elapsedTime)
{
    const float Begin = DirectX::XMConvertToRadians(90.0f);
    const float Begin2 = DirectX::XMConvertToRadians(-90.0f);

    const float End = 0.0f;

    Ratio += elapsedTime * 1.0f;
    StackAttackInterval += elapsedTime;

    if (StackAttackInterval > 0.07f)
    {
        DirectX::XMFLOAT3 Radian{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        DirectX::XMFLOAT3 Radian2{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        Radian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        Radian2.x = Calcu3D::LerpFloat(Begin2, End, Ratio);

        Shot_ShotRadianSlow(BaseProjectile::Parent::BLUEENEMY, Radian2, t.Position);

        StackAttackInterval = 0.0f;
    }

    if (Ratio > 1.0f)
    {
        Ratio = 0.0f;
        CurrentAttackNumber++;
        StackAttackInterval++;
    }
}


void Boss_2::S_NagiriFast(float elapsedTime)
{
    const float Begin = DirectX::XMConvertToRadians(90.0f);
    const float Begin2 = DirectX::XMConvertToRadians(-90.0f);

    const float End = 0.0f;

    Ratio += elapsedTime * 1.0f;
    StackAttackInterval += elapsedTime;

    if (StackAttackInterval > 0.07f)
    {
        DirectX::XMFLOAT3 Radian{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        DirectX::XMFLOAT3 Radian2{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        Radian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        Radian2.x = Calcu3D::LerpFloat(Begin2, End, Ratio);

        Shot_ShotRadianFast(BaseProjectile::Parent::BLUEENEMY, Radian, t.Position);

        StackAttackInterval = 0.0f;
    }

    if (Ratio > 1.0f)
    {
        Ratio = 0.0f;
        CurrentAttackNumber++;
        StackAttackInterval++;
    }
}

void Boss_2::S_NagiriFast2(float elapsedTime)
{
    const float Begin = DirectX::XMConvertToRadians(90.0f);
    const float Begin2 = DirectX::XMConvertToRadians(-90.0f);

    const float End = 0.0f;

    Ratio += elapsedTime * 1.0f;
    StackAttackInterval += elapsedTime;

    if (StackAttackInterval > 0.07f)
    {
        DirectX::XMFLOAT3 Radian{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        DirectX::XMFLOAT3 Radian2{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        Radian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        Radian2.x = Calcu3D::LerpFloat(Begin2, End, Ratio);

        Shot_ShotRadianFast(BaseProjectile::Parent::BLUEENEMY, Radian2, t.Position);

        StackAttackInterval = 0.0f;
    }

    if (Ratio > 1.0f)
    {
        Ratio = 0.0f;
        CurrentAttackNumber++;
        StackAttackInterval++;
    }
}