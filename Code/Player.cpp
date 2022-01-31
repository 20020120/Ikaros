#include "Player.h"
#include <imgui.h>

#include"Bullet.h"
#include "SceneManager.h"
#include"ProjectileManager.h"
#include "EnemyManager.h"
#include "Collision.h"
#include"User_Function.h"
#include"MathSupport.h"
#include "gAudio.h"
#include "OptionSystem.h"

Player::Player(ID3D11Device* d)
    :BaseObject()
{
    ModelRenderInitializer Ini;
    Ini.SetALLPath(
        "./resources/Models/Characters/Players/Player.nk",
        //"./resources/Models/Characters/Players/PlayerModel.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/PhisicsBase_PS.cso",
        "./resources/Shaders/None_Gs.cso"
    );

    // ���f���̏�����
    LoadModel(d, Ini);
    Ini.SetModelPath("./resources/Models/Characters/Players/AfterIMageModel.nk");
    Ini.SetPsPath("./resources/Shaders/AfterImage_PS.cso");
    mAfterImage.Model = std::make_unique<ModelComponent>(d, Ini);

    //--------------------<�X�v���C�g��������>--------------------//
    spr_HitArea = std::make_unique<Sprite_Batch>(d, L"./resources/Sprite/UIs/HitArea.png", 1);
    
    //--------------------<�����蔻������p��������>--------------------//
    hitAreaSize = { 3,3,3 };

    dodgeSize = { 25.0f,25.0f,30.0f };

    attackArea = { 25.0f,20.0f, 20.0f };

    //particleEmitter.Initialize(d);

    

    Light = std::make_unique<GamePointLight>(d, 3);
    DirectX::XMFLOAT3 p = t.Position;
    p.y += 6.0f;
    Light->SetPosition(p);
    Light->SetRange(100.0f);
    Light->SetColor({ 0.1f,0.5f,0.8f });


    spr_CutIn0.Initialize(d, L"./resources/Sprite/CutIn/view_1_red.png");
    spr_CutIn0.Position = { 1280.0f,360.0f };
    spr_CutIn0.Pivot = { 0.0f,360.0f };
    spr_CutIn0.TexSize = { 1280.0f,720.0f };

    spr_CutIn1.Initialize(d, L"./resources/Sprite/CutIn/CutInFrame.png");
    spr_CutIn1.Position = { 1280.0f,360.0f };
    spr_CutIn1.Pivot = { 0.0f,360.0f };
    spr_CutIn1.TexSize = { 1280.0f,720.0f };

    sprBlackCurtain.Initialize(d, L"./resources/Sprite/UIs/Black.png");
    sprBlackCurtain.Position = {};
    sprBlackCurtain.TexSize = { 1280.0f,720.0f };
    sprBlackCurtain.Color = { 1.0f,1.0f,1.0f,0.0f };
    sprBlackCurtain.Scale = { 1.0f,1.0f };

    ClearMask = std::make_unique<Sprite_Batch>(d,
        L"./resources/Sprite/Shaders/AlphaMask.png", 1,
        L"./resources/Sprite/Shaders/dissolve.png",
        L"./resources/Sprite/Shaders/load_screen.png");
    mClearDissolvePower = 0.0f;
    //SE
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/ModeChangeSE.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/Hit_SameColor.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/Hit_DifferentColor.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/HitInvincible.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/CloseAttack.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/GetEnergy.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/RedBulletShot.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/BlueBulletShot.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/Dodge.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/CriticalDodge.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/Heal.wav"));

    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/LastSlash.wav"));

    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/BossBomb.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/BossBomb2.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/BossBomb3.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/BossBomb4.wav"));

    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Enemy/LaserCharge.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Enemy/LaserBeam.wav"));
    

    volumes.emplace_back(0.9f);//ModeChange
    volumes.emplace_back(0.75f);//HitSameColor
    volumes.emplace_back(0.75f);//HitDifferentColor
    volumes.emplace_back(0.5f);//HitInvincible
    volumes.emplace_back(0.5f);//CloseAttack
    volumes.emplace_back(1.0f);//GetEnergy
    volumes.emplace_back(0.4f);//RedBulletShot
    volumes.emplace_back(0.2f);//BlueBulletShot
    volumes.emplace_back(0.75f);//Dodge
    volumes.emplace_back(0.75f);//CriticalDodge
    volumes.emplace_back(1.0f);//Heal

    //****************************************************************
    // 
    // �G�t�F�N�g�̃��[�h
    // 
    //****************************************************************
    efk_MuzzleFlash = std::make_unique<Effect>("./resources/Effects/muzleflash.efk");
    efk_RedDodgeSuccess = std::make_unique<Effect>("./resources/Effects/dodge_success_red.efk");
    //efk_RedDodgeSuccess = std::make_unique<Effect>("./resources/Effects/dodge_success.efk");
    efk_BlueDodgeSuccess = std::make_unique<Effect>("./resources/Effects/dodge_success_blue.efk");
    //efk_BlueDodgeSuccess = std::make_unique<Effect>("./resources/Effects/dodge_success.efk");

    efk_WeakDamaged = std::make_unique<Effect>("./resources/Effects/player_hit.efk");
    efk_UnWeakDamaged = std::make_unique<Effect>("./resources/Effects/player_hit_block.efk");
    efk_SonicBoom = std::make_unique<Effect>("./resources/Effects/sonic_boom.efk");
    efk_RedSlash = std::make_unique<Effect>("./resources/Effects/slash_red.efk");
    efk_BlueSlash = std::make_unique<Effect>("./resources/Effects/slash_blue.efk");
    efk_Heal = std::make_unique<Effect>("./resources/Effects/player_heal.efk");
    efk_SlashFinisher = std::make_unique<Effect>("./resources/Effects/white_out.efk");
    efk_Charge = std::make_unique<Effect>("./resources/Effects/laser_charge.efk");

    // �����߂Ƃ肵���[��
    Gs_CBuffer.Init(d, &mGsData);
    Gs_CBuffer.Set_GSSlot(4);
}

void Player::Initialize()
{
    mClearDissolvePower = 0.0f;
    //--------------------<������>--------------------//
    StackPerformanceTime = 0.0f;
    EndGame = false;
    TutorialPerformState = 0;
    // �X�P�[��
    t.Scale = {
        0.035f,
        0.035f,
        0.035f
    };

    IsSlash = false;

    RotRatio = 0.0f;
    
    //--------------------<�v���C���[�̃^�C�v��������>--------------------//
    type = Type::BLUE;
    Model->f_PlayAnimation(AnimationName::wing,true);
    Model->Update(0.1f);

    mAfterImage.Model->f_PlayAnimation(AnimationName::wing, true);
    mAfterImage.Model->Update(0.1f);

    //--------------------<���x��������>--------------------//
    CalcSpeedFromHP();

    //--------------------<�̗͂�������>--------------------//
    HitPoint = MaxHitPoint;
    alive = true;

    //--------------------<�G�l���M�[��������>--------------------//
    EnergyGauge = 0;

    // �e�҂����Ԃ�������
    StackShootInterval = 0.0f;
    StackInvincibleTime = 0.0f;

    // ���G�����������
    IsInvincible = false;

    // �e�̌�����������
    BulletVelocity = { 0.0f,0.0f,3.0f };

    BulletVelocityUp = { 0.0f,2.0f,3.0f };
    BulletVelocityDown = { 0.0f,-2.0f,3.0f };

    BulletVelocityRight = { 2.0f,0.0f,3.0f };
    BulletVelocityLeft = { -2.0f,0.0f,3.0f };

    //--------------------<����񐔂�������>--------------------//
    DodgeCount = MaxDodgeCount;

    PerformanceAngleRatio = 0.0f;

    //--------------------<GameSystem�ɑ��M����f�[�^>--------------------//

    GameSystemDatas.HP = &HitPoint;
    GameSystemDatas.Energy = &EnergyGauge;
    GameSystemDatas.Dodge = &DodgeCount;
    GameSystemDatas.position = &t.Position;

    // �C�x���g�t���O��ǂݍ���
    Flags.Load("./resources/Models/Characters/Players/PlayerEventFlag.flg");

    //--------------------<���o�̂��߂̏�����>--------------------//
    t.Position = { -170.0f,100.0f,-100.0f };
    IsPerforming = true;

    // �`���[�g���A���p
    Tutorial_IsBreakCloseAttack = false;
    CompleteDodgeTutorial = false;
    IsTutorial = false;
    CompleteElement1Tutorial = false;

    //--------------------<�K�E�Z�֌W�̏�����>--------------------//
    IsFinisher = false;
    IsLightPerformance = false;

    // �c����������
    mAfterImage.t = t;
    mAfterImage.r = r;

    SlashDir = Side;

    //--------------------<���C�g�̐F>--------------------//
    PlayerColor = { 0.1f,0.5f,0.8f };
    Light->SetColor(PlayerColor);

    //���o�ݒ��������
    IsLightPerformance = OptionSystem::Instance().GetDirectingSetting();
    PreferSkipPerformance = false;
    StackSkipTimer = 0.0f;
    EnemiesStop = false;
}

void Player::Update(float elapsedTime)
{
    CompleteDodgeTutorial = false;
    if(IsTutorial)
    {
        HitPoint = MaxHitPoint;
    }

    // ���C�g
    DirectX::XMFLOAT3 p = t.Position;
    p.y += 6.0f;
    p.x += 6.0f;
    Light->SetPosition(p);
    switch (type)
    {
    case Type::NONE: break;
    case Type::RED:
        PlayerColor = Calcu3D::LerpFloat3(PlayerColor, { 0.9f,0.2f,0.2f }, 2.0f * elapsedTime);
        break;
    case Type::BLUE:
        PlayerColor = Calcu3D::LerpFloat3(PlayerColor, { 0.1f,0.5f,0.8f }, 2.0f * elapsedTime);
        break;
    default: ;
    }

    Light->SetColor(PlayerColor);

    //--------------------<���S���̉��o>--------------------//

    if(HitPoint<=0)
    {
        PerformanceDead(elapsedTime);
        return ;
    }


    //--------------------<ImGui�̃f�o�b�O�E�C���h�E>--------------------//
    GuiMenu();

    //--------------------<�p�[�e�B�N���̍X�V>--------------------//
    auto frontVec = GetFrontVec();
    frontVec *= -1.0f;
    //particleEmitter.SetVelocity(frontVec);
    //particleEmitter.SetPosition(t.Position);
    //particleEmitter.Update(elapsedTime);

    CalcParticle();

    Model->Update(elapsedTime);
    mAfterImage.Model->Update(elapsedTime);
    // �������Ă��Ȃ���Αҋ@�A�j���[�V�����𗬂�
    if(!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(AnimationName::wait, true);
        mAfterImage.Model->f_PlayAnimation(AnimationName::wait, true);
    }

    //--------------------<�K�E�Z���͑S�Ă̍X�V���󂯕t���Ȃ�>--------------------//
    if (IsFinisher)
    {
        ShowAfterImage = false;
        StackInvincibleTime = 10.0f;

        // �K�E�Z�̌y�ʐݒ�
        if(IsLightPerformance)
        {
            // �K�E�Z�̍X�V����
            if (type == Type::BLUE)
            {
                Finisher_SlashLight(elapsedTime);
            }

            else if (type == Type::RED)
            {
                Finisher_BeamLight(elapsedTime);
            }
        }
        else
        {
            // �K�E�Z�̍X�V����
            if (type == Type::BLUE)
            {
                Finisher_Slash(elapsedTime);
            }
            else if (type == Type::RED)
            {
                Finisher_Beam(elapsedTime);
            }
        }

        return;
    }

    // ���o���X�V
    IsLightPerformance = OptionSystem::Instance().GetDirectingSetting();

    // �ړ����X�V
    (this->*Move[static_cast<int>(GameSystem::Instance().GetCameraDirection())])(elapsedTime);

    // ���_�ύX���͂����Ȃ�����
    if (!GameSystem::Instance().GetChangingView())
    {

    }

    // ���x���X�V
    CalcSpeedFromHP();

    // ���
    inputDodge(elapsedTime);
    

    //�ˌ�
    InputShooting(elapsedTime);

    //�ߋ����U��
    InputCloseAttack(elapsedTime);

    // �G�l���M�[���X�V�i�ό`�j
    UpdateEnergy();

    // ���G���X�V
    UpdateInvTime(elapsedTime);

    if (!IsFinisher)
    {

        if (!IsDebug)
        {
            //�Փ˔���
            VsProjectile(elapsedTime);
            VsEnemy(elapsedTime);
        }
    }

    // �c�����X�V
    UpdateAfterImage(elapsedTime);

    // �s���������X�V
    LimitMoveArea();

    // �X�L�b�v�̍��摜���X�V
    if (!IsPerforming)
    {
        sprBlackCurtain.Color.w -= elapsedTime * 5.0f;
    }
    if(sprBlackCurtain.Color.w <=0.0f )
    {
        sprBlackCurtain.Color.w = 0.0f;
    }
    
    

}

//--------------------<���f���̕`��>--------------------//
void Player::Render(ID3D11DeviceContext* dc)
{

    Gs_CBuffer.SetBuffer(dc, &mGsData,GEOMETRY);

    // ���C�g���X�V
    Light->Update(dc);

    //particleEmitter.Render(dc);
    // ���G���Ԃ̕\��

    if (IsInvincible)
    {
        const int IntTime = static_cast<int>((StackInvincibleTime) * 10.0f);
        switch (IntTime % 3)
        {
        case 0:
            Model->Render(dc, t, r);
           // AfterImageModel->Render(dc, afImage_t, afImage_r);
            break;
        default:
            break;
        }
    }
    else
    {
        Model->Render(dc, t, r);
    }

    //  �c��
    if (ShowAfterImage)
    {
        mAfterImage.Model->Render(dc, mAfterImage.t, mAfterImage.r);
    }
    

}

//--------------------<�X�v���C�g�̕`��>--------------------//
void Player::Render2D(ID3D11DeviceContext* dc,bool draw)
{
    if (!draw)
    {
        using namespace  DirectX;
        // �����蔻��~�̕`��
        const XMFLOAT2 pos = {
            t.Position.z,
               t.Position.y
        };
        constexpr XMFLOAT2 scale = { 10.0f,10.0f };
        constexpr XMFLOAT2 txPos = { 0.0f,0.0f };
        constexpr XMFLOAT2 cutSize = { 25.0f,25.0f };
        constexpr XMFLOAT2 pivot = { 0.5f,0.5f };
        constexpr XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
    }
    
    spr_CutIn0.Render(dc);
    spr_CutIn1.Render(dc);

    sprBlackCurtain.Render(dc);
    ClearMask->SetDissolvePower(mClearDissolvePower);
    ClearMask->Begin(dc);
    ClearMask->Render(dc, { 0.0f,0.0f }, { 1.0f,1.0f }, { 0.0f,0.0f }, { 1280.0f,720.0f }, { 0.0f,0.0f }, 0.0f, { 1.0f,1.0f,1.0f,1.0f });
    ClearMask->End(dc);
}



void Player::Finalize()
{
    //particleEmitter.Finalize();

    //�T�E���h���
    for(const auto& s : se)
    {
        s->Release();
    }
}

//****************************************************************
// 
//  �����o�֐��i�N���X�O����A�N�Z�X�����֐��j
// 
//****************************************************************

// ��_���[�W
bool Player::Damaged(const int damage)
{
    if (!IsInvincible)
    {
        //  �̗͂����炷
        HitPoint -= damage;
        GamePad::Instance().SetVibration(100.0f, 100.0f, 0.5f);
        mCameraShakeFunc();
        // ���G���ԊJ�n
        StackInvincibleTime = 0.0f;
        IsInvincible = true;
        return true;
    }
    return false;
}

// �G�l���M�[�擾
void Player::GetEnergy(const Type t, const int getEnergy)
{
    switch (t)
	{
    case Type::NONE:

        break;
    case Type::RED:

        // ���݂̑������玟�ɏW�߂��������̐F�ŉ��Z�ɂ������̂�
        (type == Type::BLUE) ? EnergyGauge += getEnergy     // ���[�h���F�̎��A���̕ό`�͐ԐF�Ȃ̂Ł@�ԃG�l���M�[����������ɉ��Z�@�{
                            : EnergyGauge -= getEnergy;     // �@�@�@�@�ԐF�@�@�@�@�@�@�@�@�F�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@���Z�@�|

        break;
    case Type::BLUE:

        // ���݂̑������玟�ɏW�߂��������̐F�ŉ��Z�ɂ������̂�
        (type == Type::BLUE) ? EnergyGauge -= getEnergy      // ���[�h���F�̎��A���̕ό`�͐ԐF�Ȃ̂Ł@�ԃG�l���M�[����������Ɍ��Z�@�|
                             : EnergyGauge += getEnergy;     // �@�@�@�@�ԐF�@�@�@�@�@�@�@�@�F�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@���Z�@�{

        break;
    default:

        break;;
    }
    se.at(SE::GET_ENERGY)->Stop();
    se.at(SE::GET_ENERGY)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::GET_ENERGY));
}



//****************************************************************
// 
//  �����o�֐��i�N���X���A�N�Z�X�j
// 
//****************************************************************

// �v���C���[�̃��[�h��ϊ�������֐�
void Player::ChangeMode()
{
#if 0
    // ��->�� 
    if (type == Type::RED && EnergyGauge <= LimitBlueEnergy)
    {
        // �A�j���[�V����
        Model->f_PlayAnimation(AnimationName::transform);
        EndTransform = false;
        // �^�C�v
        type = Type::BLUE;
    }
    // ��->��
    else if (type == Type::BLUE && EnergyGauge >= LimitRedEnergy)
    {
        Model->f_PlayAnimation(AnimationName::inv_transform);
        EndTransform = false;
        type = Type::RED;
    }
    // �̗͉�
    HitPoint++;
    // ����\�񐔉�
    DodgeCount = MaxDodgeCount;


    constexpr float mulValue = { 0.7f };
    //�G�l���M�[�ݒ�
    if (type == Type::RED)
    {
        EnergyGauge = static_cast<int>(mulValue * static_cast<float>(LimitRedEnergy));
    }
    else if (type == Type::BLUE)
    {
        EnergyGauge = static_cast<int>(mulValue * static_cast<float>(LimitBlueEnergy));
    }
#else

    // �����̕ω�
    if(type == Type::BLUE)
    {
        type = Type::RED;

        // �A�j���[�V����
        Model->f_PlayAnimation(AnimationName::inv_transform);
        mAfterImage.Model->f_PlayAnimation(AnimationName::inv_transform);
        EndTransform = false;

    }
    else if(type == Type::RED)
    {
        type = Type::BLUE;

        // �A�j���[�V����
        Model->f_PlayAnimation(AnimationName::transform);
        mAfterImage.Model->f_PlayAnimation(AnimationName::transform);
        EndTransform = false;

    }

    // �̗͉�
    HitPoint += HealHpCount;
    // ����\�񐔉�
    DodgeCount = MaxDodgeCount;


    //�G�l���M�[�ݒ�
    EnergyGauge = MinEnergyCount;


    se.at(SE::MODE_CHANGE)->Stop();
    se.at(SE::MODE_CHANGE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::MODE_CHANGE));

    // �G�t�F�N�g�Đ�
    efk_Heal->Play(t.Position, {}, 0.10f);

    //--------------------<�̗͂̏���ݒ�>--------------------//
    HitPoint = min(MaxHitPoint, HitPoint);

    //--------------------<�̗͂̉����ݒ�>--------------------//
    HitPoint = max(HitPoint, 0);
#endif

}

//--------------------<�ړ����͏���>--------------------//
const auto& Player::GetMoveVec() const
{
    auto gamepad = GamePad::Instance();
    // ���͒l�����o��
    const float ax = gamepad.GetAxisLX();
    const float ay = gamepad.GetAxisLY();

    return XMFLOAT2(ax, ay);
}

//--------------------<��]�̍X�V>--------------------//
void Player::RotUpdate(float vecY)
{
    // �ړ����̃v���C���[�̊p�x���v�Z����
        // ��]�̕⊮��
    const float RotRatio = { 0.1f };
    float End = 0.0f;

    if (vecY > 0.1f)   // �����
    {
        End = DirectX::XMConvertToRadians(30.0f);
    }
    else if (vecY < -0.1f) // ������
    {
        End = DirectX::XMConvertToRadians(-30.0f);
    }
    r.Angle.z = Calcu3D::LerpFloat(r.Angle.z, End, RotRatio);

}


//--------------------<���x��̗͂���v�Z>--------------------//
void Player::CalcSpeedFromHP()
{
    // �̗͂��ő�l�̉��������߂�
    const int rate = 100 / MaxHitPoint * HitPoint;

    //rate��0.0f~1.0f�ɕϊ�
    const float clampRate = static_cast<float>(rate) * 0.01f;

    // clampRate���瑬�x���Z�o
    const float res = (MinSpeed * clampRate) + (MaxSpeed * (1.0f - clampRate));
    Speed = res;
}

//--------------------<�����͏���>--------------------//
void Player::inputDodge(float elapsedTime)
{
    // ����\�񐔂��X�V
    if (DodgeCount < 1)
    {
        CanDodge = false;
    }
    else
    {
        CanDodge = true;
    }

    // ����\
    if (CanDodge && !ReadyDodge&& !IsSlash)
    {
        //  �������
        if(GamePad::Instance().GetButtonDown() &GamePad::BTN_Y)
        {
            DodgeVecX = GetMoveVec().x;
            DodgeVecY = GetMoveVec().y;
            // �W���X�g����̎�t���Ԃ����Z�b�g
            StackDodgeTime = 0.0f;
            BeforeDodgeAngle = r.Angle.z; // �p�x��������
            ReadyDodge = true;
            RotRatio = 0.0f;
            if (IsTutorial==false)
            {
                DodgeCount--;
            }

            //�W���X�g��𐬌����ǂ���
            const int pIndex = IsHitProjectile(dodgeSize);
            const int eIndex = IsHitEnemy(dodgeSize);
            if (pIndex >= 0 || eIndex >= 0)
            {
                dodgeSuccess = true;
                StackHitStopSeconds = 0.0f;
                StackInvincibleTime = 0.0f;

                //�G�l���M�[�擾
                DodgeSuccessEnergy(pIndex, eIndex);
                CompleteDodgeTutorial = true;
                StackAfterImageTime = 0.7f;
                ShowAfterImage = true;

                se.at(CRITICAL_DODGE)->Stop();
                se.at(CRITICAL_DODGE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(CRITICAL_DODGE));
            }
            else
            {
                se.at(DODGE)->Stop();
                se.at(DODGE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(DODGE));
            }

            // �c���̈ʒu���X�V����
            mAfterImage.t.Position = t.Position;
            mAfterImage.r = r;
            
        }
    }

    // �W���X�g����̎�t���Ԃ��X�V
    StackDodgeTime += elapsedTime;
    if (StackDodgeTime >= DodgeTime && ReadyDodge)
    {
        ReadyDodge = false; // �W���X�g����̎�t���ԊJ�n�����莞�Ԍo�߂Ŏ�t�I��
        dodgeSuccess = false;
    }
    else if(StackDodgeTime >= DodgeTime)
    {
        StackDodgeTime = DodgeTime;
    }

}
void Player::DodgeSuccessEnergy(const int pIndex, const int eIndex)
{
    bool dodgeProj = false;
    bool dodgeEnemy = false;

    if (pIndex >= 0 && eIndex < 0)dodgeProj = true;
    else if (pIndex < 0 && eIndex >= 0)dodgeEnemy = true;
    else
    {
        BaseProjectile& proj = ProjectileManager::Instance().GetProjectile(pIndex);
        Enemy& enemy = EnemyManager::Instance().GetEnemy(eIndex);

        float projLength = Collision::Distance(t.Position, proj.GetPosition());
        float eneLength = Collision::Distance(t.Position, enemy.GetPosition());

        if (projLength < eneLength)dodgeProj = true;
        else if (projLength > eneLength)dodgeEnemy = true;
    }

    const int parryOtherRatio = 2;
    const int parryEnergyRatio = 3;
    if(dodgeProj)
    {
        BaseProjectile& proj = ProjectileManager::Instance().GetProjectile(pIndex);

        switch (proj.GetProjType())
        {
        case BaseProjectile::ProjType::REDBULLET:
            GetEnergy(Type::RED, increaseEnergy * parryOtherRatio);
            hdl_RedDodgeSuccess = efk_RedDodgeSuccess->Play(t.Position, {}, 0.15f);
            break;
        case BaseProjectile::ProjType::BLUEBULLET:
            GetEnergy(Type::BLUE, increaseEnergy * parryOtherRatio);
            hdl_BlueDodgeSuccess = efk_BlueDodgeSuccess->Play(t.Position, {}, 0.15f);
            break;
        case BaseProjectile::ProjType::REDENERGY:
            hdl_RedDodgeSuccess = efk_RedDodgeSuccess->Play(t.Position, {}, 0.15f);
            GetEnergy(Type::RED, increaseEnergy * parryEnergyRatio);
            break;
        case BaseProjectile::ProjType::BLUEENERGY:
            GetEnergy(Type::BLUE, increaseEnergy * parryEnergyRatio);
            hdl_BlueDodgeSuccess = efk_BlueDodgeSuccess->Play(t.Position, {}, 0.15f);
            break;
	    }
    }
    else if(dodgeEnemy)
    {
        Enemy& enemy = EnemyManager::Instance().GetEnemy(eIndex);

	    switch(enemy.GetType())
	    {
        case Type::RED:
            GetEnergy(Type::RED, increaseEnergy * parryOtherRatio);
            hdl_RedDodgeSuccess = efk_RedDodgeSuccess->Play(t.Position, {}, 0.15f);
            break;
        case Type::BLUE:
            GetEnergy(Type::BLUE, increaseEnergy * parryOtherRatio);
            hdl_BlueDodgeSuccess = efk_BlueDodgeSuccess->Play(t.Position, {}, 0.15f);
            break;
        default: ;
        }
    }
}


//--------------------<��𒆂̉�]����>--------------------//
void Player::RotDodge(float elapsedTime)
{
    const float AfterDodgeAngle = BeforeDodgeAngle + DirectX::XMConvertToRadians(360.0f);

    RotRatio += elapsedTime * 1.2f;
    r.Angle.z = Calcu3D::LerpFloat(BeforeDodgeAngle, DirectX::XMConvertToRadians(720.0f), RotRatio);

    if (RotRatio>=0.95f)
    {
        r.Angle.z = BeforeDodgeAngle;
    }
}

//--------------------<�G�l���M�X�V����>--------------------//

void Player::UpdateEnergy()
{
    //energy�␳
    if (EnergyGauge > MaxEnergyCount)EnergyGauge = MaxEnergyCount;
    else if (EnergyGauge < MinEnergyCount)EnergyGauge = MinEnergyCount;

    const GamePadButton transformationButton = GamePad::BTN_RIGHT_TRIGGER | GamePad::BTN_RIGHT_SHOULDER;

    if (GamePad::Instance().GetButtonDown() & transformationButton&&EnemyManager::Instance().IsPlayerTransformable())
    {
        if(EnergyGauge >= MaxEnergyCount)
        {
            // �ό`
            IsFinisher = true;
        }
#if 0
        else if (EnergyGauge <= LimitBlueEnergy)
        {
            // �ό`�����o�t�ƕK�E�Z�̂�


            //�G�l���M�[�ݒ�
            constexpr float mulValue = { 0.7f };
            EnergyGauge = static_cast<int>(mulValue * static_cast<float>(LimitRedEnergy));


            // �̗͉�
            HitPoint++;
            // ����\�񐔉�
            DodgeCount = MaxDodgeCount;
            // �G�t�F�N�g�Đ�
            efk_Heal->Play(t.Position, {}, 0.10f);

            //--------------------<�̗͂̏���ݒ�>--------------------//
            HitPoint = min(MaxHitPoint, HitPoint);

            //--------------------<�̗͂̉����ݒ�>--------------------//
            HitPoint = max(HitPoint, 0);
        }
#endif

    }

    if(!EndTransform&&Flags.Check("EndChangeToWing",Model->GetCurrentAnimationNumber(),Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(AnimationName::wing, true);
        mAfterImage.Model->f_PlayAnimation(AnimationName::wing, true);
        EndTransform = true;
        CompleteElement1Tutorial = true;
        AttentionCamera = false;\
    }
    else if(!EndTransform && Flags.Check("EndChangeToHuman", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(AnimationName::wait, true);
        mAfterImage.Model->f_PlayAnimation(AnimationName::wait, true);
        EndTransform = true;
        AttentionCamera = false;
        CompleteElement1Tutorial = true;
    }
    else if(!EndTransform&&Flags.Check("EndBeamMotion",Model->GetCurrentAnimationNumber(),Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(AnimationName::wing, true);
        mAfterImage.Model->f_PlayAnimation(AnimationName::wing, true);
        EndTransform = true;
        CompleteElement1Tutorial = true;
        AttentionCamera = false;
    }
         

}

//--------------------<���G���ԍX�V>--------------------//
void Player::UpdateInvTime(float elapsedTime)
{
    StackInvincibleTime += elapsedTime;

    // ���G�ɂȂ��Ă��炪��莞�Ԃ𒴂�����
    if (StackInvincibleTime > LimitInvincibleTime)
    {
        IsInvincible = false;
    }
}



//--------------------<�ˌ����͏���>--------------------//
void Player::InputShooting(float elapsedTime)
{
    // �ˌ��\
    if(CanShoot&&!IsFinisher&&!IsSlash)
    {
        //  �ˌ�����
        if (GamePad::Instance().GetButton() & GamePad::BTN_B)
        {
            // �f�o�C�X���擾
            const auto& device = SceneManager::Instance().GetDevice();

            Bullet* bullet0{ nullptr };
            Bullet* bullet1{ nullptr };
            Bullet* bullet2{ nullptr };

            // �e�𐶐��i���[�h�ɂ���Ďˌ������e��ς���j
            switch (type) {
            case Type::NONE:

                break;
            case Type::RED:   // ��s�@
                if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::SIDE)
                {
                    DirectX::XMFLOAT3 launchPosition{ t.Position };
                    launchPosition.z += 7.0f;
                    launchPosition.y += 3.0f;
                    bullet0 = new Bullet(device, BaseProjectile::Parent::REDPLAYER, launchPosition, BulletVelocity      , 30.0f * 1.2f);
                    bullet1 = new Bullet(device, BaseProjectile::Parent::REDPLAYER, launchPosition, BulletVelocityUp    , 30.0f * 1.2f);
                    bullet2 = new Bullet(device, BaseProjectile::Parent::REDPLAYER, launchPosition, BulletVelocityDown  , 30.0f * 1.2f);
                }
                else if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
                {
                    DirectX::XMFLOAT3 launchPosition{ t.Position };
                    launchPosition.z += 7.0f;
                    bullet0 = new Bullet(device, BaseProjectile::Parent::REDPLAYER, launchPosition, BulletVelocity      , 30.0f * 1.2f);
                    launchPosition.x = t.Position.x + 3.0f;
                    bullet1 = new Bullet(device, BaseProjectile::Parent::REDPLAYER, launchPosition, BulletVelocityRight , 30.0f * 1.2f);
                    launchPosition.x = t.Position.x - 3.0f;
                    bullet2 = new Bullet(device, BaseProjectile::Parent::REDPLAYER, launchPosition, BulletVelocityLeft  , 30.0f * 1.2f);
                }

                ProjectileManager::Instance().RegisterProjectile(bullet0);
                ProjectileManager::Instance().RegisterProjectile(bullet1);
                ProjectileManager::Instance().RegisterProjectile(bullet2);

                se.at(SE::RED_BULLET_SHOT)->Stop();
                se.at(SE::RED_BULLET_SHOT)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(RED_BULLET_SHOT));

                Model->f_PlayAnimation(AnimationName::Shot_Recoil);

                break;
            case Type::BLUE:  // �l�^
                bullet0 = new Bullet(device, BaseProjectile::Parent::BLUEPLAYER, t.Position, BulletVelocity,50.0f);
                ProjectileManager::Instance().RegisterProjectile(bullet0);

                se.at(SE::BLUE_BULLET_SHOT)->Stop();
                se.at(SE::BLUE_BULLET_SHOT)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(BLUE_BULLET_SHOT));

                DirectX::XMFLOAT3 LaunchPosition{ t.Position };
                LaunchPosition.z += 3.0f;
                LaunchPosition.y -= 1.0f;

                // �G�t�F�N�g���Đ�
                hdl_MuzzleFlash = efk_MuzzleFlash->Play(LaunchPosition, {}, 0.1f);


                break;
            default:
                assert("Player Type Is Broken");
                break;
            }

            // �ˌ����Ԃ����Z�b�g
            StackShootInterval = 0.0f;
            CanShoot = false;

          
        }
    }
    else
    {
        // �ˌ��\�łȂ����
        if(type==Type::BLUE)
        {
            StackShootInterval += elapsedTime * 2.0f;    // �ˌ����莞�Ԍo�߂܂�
        }
        else
        {
            StackShootInterval += elapsedTime;     // �ˌ����莞�Ԍo�߂܂�
        }

        if(StackShootInterval>=ShootInterval)  // ���̒e�����ĂȂ��悤�ɂ���
        {
            CanShoot = true;
        }
    }

}



//--------------------<�ړ�>--------------------//
void Player::MoveHorizontal(float elapsedTime)
{
    // ����s�����͐�p�̓���������
    const auto moveVec = GetMoveVec();

    if (!ReadyDodge)
    {
        RotUpdate(moveVec.y);

        const float AddSpeedY = Speed * moveVec.y * elapsedTime;
        const float AddSpeedZ = Speed * moveVec.x * elapsedTime;
        t.Position.y += AddSpeedY;
        t.Position.z += AddSpeedZ;
    }
    else
    {
        RotDodge(elapsedTime);
        const float AddSpeedY = Speed * moveVec.y * elapsedTime*1.4f;
        const float AddSpeedZ = Speed * moveVec.x * elapsedTime*1.4f;
        t.Position.y += AddSpeedY;
        t.Position.z += AddSpeedZ;
    }
}
void Player::MoveVertical(float elapsedTime)
{
    // ����s�����͐�p�̓���������
    const auto moveVec = GetMoveVec();

    if (!ReadyDodge)
    {
        RotUpdate(-moveVec.x);

        const float AddSpeedX = Speed * moveVec.x * elapsedTime;
        const float AddSpeedZ = Speed * moveVec.y * elapsedTime;
        t.Position.x += AddSpeedX;
        t.Position.z += AddSpeedZ;
    }
    else
    {
        RotDodge(elapsedTime);
        const float AddSpeedX = Speed * moveVec.x * elapsedTime;
        const float AddSpeedZ = Speed * moveVec.y * elapsedTime;
        t.Position.x += AddSpeedX;
        t.Position.z += AddSpeedZ;
    }
}

//--------------------<�v���C���[�̍s���͈͐���>--------------------//
void Player::LimitMoveArea()
{
    GameSystem& gameSystem = GameSystem::Instance();

    //�Փ˔���
    if (gameSystem.GetCameraDirection() == GameSystem::CAMERA_DIRECTION::SIDE)//���X�N���[��
    {
        // ���̍s������
        if (LeftTop.x > t.Position.z)
        {
            t.Position.z = LeftTop.x;
        }
        // �E�̍s������
        else if (RightBottom.x < t.Position.z)
        {
            t.Position.z = RightBottom.x;
        }
        // ��̍s������
        if (LeftTop.y < t.Position.y)
        {
            t.Position.y = LeftTop.y;
        }
        else if (RightBottom.y > t.Position.y)
        {
            t.Position.y = RightBottom.y;
        }
    }
    else if (gameSystem.GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)//�c�X�N���[��
    {
        // ���̍s������
        if (LeftTop.x > t.Position.x)
        {
            t.Position.x = LeftTop.x;
        }
        // �E�̍s������
        else if (RightBottom.x < t.Position.x)
        {
            t.Position.x = RightBottom.x;
        }
        // ��̍s������
        if (LeftTop.y < t.Position.z)
        {
            t.Position.z = LeftTop.y;
        }
        else if (RightBottom.y > t.Position.z)
        {
            t.Position.z = RightBottom.y;
        }
    }


    //�c���؂�ւ��̕␳
    if(GameSystem::Instance().GetCameraDirection()==GameSystem::CAMERA_DIRECTION::SIDE)
    {
        t.Position.x = 0.0f;
    }
    else if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
    {
        t.Position.y = 0.0f;
    }


}

//--------------------<�ߋ����U��>--------------------//
void Player::InputCloseAttack(float elapsedTime)
{
   // // ��ʐ؂�ւ����͏������Ȃ�
   // if (Ga0meSystem::Instance().GetChangingView()) return;

    attackInterval -= elapsedTime;
    StackCloseAttackInterval += elapsedTime;

    if (!ReadyDodge&&StackCloseAttackInterval > CloseAttackInterval)
    {
        if (GamePad::Instance().GetButton() & GamePad::BTN_A)
        {
            if (attackInterval < 0.0f)
            {
                DirectX::XMFLOAT3 efkAngle = { 0.0f,0.0f,0.0f };
                // �U���̉�]
                if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
                {
                    efkAngle = { 0.0f,0.0f,DirectX::XMConvertToRadians(90.0f) };
                }

                attackInterval = MaxAttackInterval;

                se.at(SE::CLOSE_ATTACK)->Stop();
                se.at(SE::CLOSE_ATTACK)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::CLOSE_ATTACK));

                switch (type)
                {
                case Type::NONE:
                    break;
                case Type::RED:

                    switch (SlashDir)
                    {
                    case Side:
                        Model->f_PlayAnimation(AnimationName::Slash_side);
                        break;
                    case Top:
                        Model->f_PlayAnimation(AnimationName::Slash_top);
                        break;
                    default:;
                    }

                    hdl_RedSlash = efk_RedSlash->Play(t.Position, efkAngle, 0.15f);
                    break;
                case Type::BLUE:
                    hdl_BlueSlash = efk_BlueSlash->Play(t.Position, efkAngle, 0.15f);
                    break;
                default:;
                }

                switch (GameSystem::Instance().GetCameraDirection())
                {
                case GameSystem::CAMERA_DIRECTION::SIDE:
                    SlashDir = Side;
                    break;
                case GameSystem::CAMERA_DIRECTION::TOP:
                    SlashDir = Top;
                    break;
                default:;
                }

                IsSlash = true;
                StackCloseAttackInterval = 0.0f;
            }
        }
    }

    //�U�����͏�ɔ�������
    if (attackInterval > 0.0f)
    {
        const int index = IsHitEnemy(attackArea);
        if (index >= 0)
        {
            Enemy& enemy = EnemyManager::Instance().GetEnemy(index);

			constexpr float attackPower = 1.5f;
            if (enemy.GetType() == type)
            {
                enemy.Damage(attackPower, enemy.GetMaxInvTimer());
            }
            else
            {
                enemy.Damage(attackPower * static_cast<float>(damageRatio), enemy.GetMaxInvTimer());
            }
            Tutorial_IsBreakCloseAttack = true; // �`���[�g���A���p
        }
    }
    else
    {
        IsSlash = false;
    }

    // �U���̉�]

    if (type == Type::BLUE)
    {
        switch (SlashDir)
        {
        case Side:
            r.Angle.x = Calcu3D::LerpFloat(0.0f, DirectX::XMConvertToRadians(360.0f), 1.0f - attackInterval * 2.0f);
            r.Angle.x = Calcu3D::ClampRotate(r.Angle.x);
            break;
        case Top:
            r.Angle.y = Calcu3D::LerpFloat(0.0f, DirectX::XMConvertToRadians(360.0f), 1.0f - attackInterval * 2.0f);
            r.Angle.y = Calcu3D::ClampRotate(r.Angle.y);
            break;
        default:;
        }

    }
    
    


   
}


//--------------------<�K�E�Z>--------------------//

void Player::SlowForFinisher(float& elapsedTime)
{
    if(IsFinisher)
    {
        elapsedTime *= 0.5f;
    }
}

//--------------------<�J�������Ǐ]���邩�ǂ���>--------------------//
bool Player::OnCameraTarget_FlingFinisher() const
{
    if (!IsFinisher) return false;
    if (type != Type::RED) return false;
    if (FinisherState != 9) return false;
    return true;
}

void Player::FlyModeFinisher(float elapsedTime)
{
    const float Time= elapsedTime *= 2.0f; // �ʏ푬�x�ɖ߂�
    DirectX::XMFLOAT3 front = GetFrontVec();
    // ���̊֐��������̒萔
    constexpr float BackSpeed = 40.0f;

    switch (FinisherState)
    {
    case 0:  // ������
        FinisherRatio = 1.0f;
        FinisherState++;
        /*FallThrough*/
        break;
    
    case 1:
    {
        const float L_Speed = Calcu3D::LerpFloat(BackSpeed, 0.0f, FinisherRatio);
        t.Position.z -= L_Speed * Time;
        t.Position.y += L_Speed * Time;

        FinisherRatio -= 4.0f * Time;

        r.Angle.x += DirectX::XMConvertToRadians(80.0f) * Time;

        if (r.Angle.x >= DirectX::XMConvertToRadians(20.0f))
        {
            FinisherState++;
        }
        break;
    }
    case 2: // ������
        FinisherRatio = 0.0f;
        FinisherState++;
        /*FallThrough*/
        break;
    case 3:
    {
        // �O�������ɐi��
        constexpr float EndAngleY = DirectX::XMConvertToRadians(-90.0f);
        constexpr float EndAngleX = DirectX::XMConvertToRadians(0.0f);
        constexpr float BeginSpeed = 50.0f;
        constexpr float EndSpeed = 500.0f;
        FinisherRatio += 2.0f * Time;

        const float speed = Calcu3D::LerpFloat(BeginSpeed, EndSpeed, FinisherRatio);
        r.Angle.y = Calcu3D::LerpFloat(r.Angle.y, EndAngleY, 2.0f * Time);
        r.Angle.x = Calcu3D::LerpFloat(r.Angle.x, EndAngleX, 2.0f * Time);
        t.Position += front * speed * Time;

        if (t.Position.x <= -300.0f)
        {
            FinisherState++;
        }
    }
        break;

    case 4: // ������
        FinisherRatio = 0.0f;
        FinisherState++;
        /*FallThrough*/
        break;

    case 5:
    {
        // ������ɐi��
        constexpr float speed = 1500.0f;
        constexpr float EndAngleX = DirectX::XMConvertToRadians(-90.0f);
        r.Angle.x = Calcu3D::LerpFloat(r.Angle.x, EndAngleX, 2.0f * Time);
        t.Position += front * speed * Time;

            if(t.Position.y>=300.0f)
            {
                FinisherState++;
            }
    }
        break;
    case 6: // ������
        FinisherRatio = 0.0f;
        FinisherState++;
        /*FallThrough*/
        break;
    case 7:
    {

            // ����Ȃ������
        constexpr float BeginSpeed = 1500.0f;
        constexpr float EndSpeed = 500.0f;

        FinisherRatio += 2.0f * Time;
        const float speed = Calcu3D::LerpFloat(BeginSpeed, EndSpeed, FinisherRatio);

        constexpr float EndAngleY = DirectX::XMConvertToRadians(-270.0f);
        constexpr float EndAngleX = DirectX::XMConvertToRadians(90.0f);
        r.Angle.y = Calcu3D::LerpFloat(r.Angle.y, EndAngleY, 3.0f*Time);
        r.Angle.x = Calcu3D::LerpFloat(r.Angle.x, EndAngleX, 3.0f*Time);

        t.Position += front * speed * Time;
        if(t.Position.z<0.0f)
        {
            t.Position.z = 0.0f;
        }

            if(t.Position.y<=100.0f)
            {
                r.Angle.x = XMConvertToRadians(90.0f);
                r.Angle.y = XMConvertToRadians(90.0f);
                t.Position.z = 0.0f;
                FinisherState++;
            }
    }
        break;

    case 8: // ������
        FinisherState++;
        FinisherRatio = 0.0f;
        /*FallThrough*/
    case 9:
        //  �Ǐ]�悤��0.5�b�X�g�b�v����
        FinisherRatio += Time;

        if(FinisherRatio>=0.5f)
        {
            FinisherState++;
        }

        break;
    case 10:
        FinisherRatio = 0.0f;
        FinisherState++;
        /*FallThrough*/

    case 11 :
    {
        constexpr float speed = 1000.0f;
        t.Position += front * speed * Time;
        if (t.Position.y <= 5.0f)
        {
            t.Position.y = 5.0f;
            FinisherState++;
        }
    }
        break;
    case 12:
        FinisherState++;
        FinisherRatio = 0.0f;
        r.Angle.x = 0.0f;
        /*FallThrough*/
    case 13:
    {
        constexpr float speed = 1000.0f;
        t.Position += front * speed * Time;
            if(t.Position.x>-2.0f)
            {
                FinisherState++;
            }
            break;
    }

    case 14:
        FinisherState++;
        FinisherRatio = 0.0f;
        r.Angle.x = 0.0f;
        /*FallThrough*/
    case 15:
        t.Position.x += 15.0f * Time;
        if(t.Position.x>7.0f)
        {
            FinisherState++;
        }
        break;
    case 16:
        FinisherState++;
        FinisherRatio = 0.0f;
    case 17:
        t.Position.x += 1000.0f * Time;

        if (t.Position.x > 100.0f)
        {
            FinisherState++;
        }
        break;

    case 18:
        t.Position = {
        0.0f,
        0.0f,
        -100.0f
        };
        r.Angle = {
            0.0f,
            0.0f,
            0.0f
        };
        FinisherState++;
        /*FallThrough*/
    case 19:
        t.Position.z += 50.0f * Time;

        if(t.Position.z>-30.0f)
        {
            FinisherState++;
        }
        break;

    case 20:
        FinisherState = 0;
        IsFinisher = false;
        //enemy�Ƀ_���[�W��^����
        AttackAllEnemy(FinisherDamage);
        StackInvincibleTime = 0.0f;
        ChangeMode();
    default:;
    }

}

void Player::FlyModeFinisherMini(float elapsedTime)
{
    const float Time = elapsedTime *= 2.0f; // �ʏ푬�x�ɖ߂�
    switch (FinisherState)
    {
    case 0:
        // ������
        // �G�t�F�N�g�Ƌ��ɏu�Ԉړ�
        t.Position = { -200.0f,100.0f,0.0f };
        r.Angle.x = XMConvertToRadians(90.0f);
        r.Angle.y = XMConvertToRadians(90.0f);
        FinisherRatio = 0.0f;
        FinisherState++;
        /*FallThrough*/
    case 1:
        //  �Ǐ]�悤��0.5�b�X�g�b�v����
        FinisherRatio += Time;

        if (FinisherRatio >= 0.5f)
        {
            FinisherState++;
            FinisherRatio = 0.0f;
        }
        break;
    case 2:
    {
        constexpr float speed = 1000.0f;
        t.Position += GetFrontVec() * speed * Time;
        if (t.Position.y <= 5.0f)
        {
            t.Position.y = 5.0f;
            FinisherState++;
        }
    }
        break;
    case 3:

        break;


    }
}

//--------------------<�c�����v�Z>--------------------//
void Player::UpdateAfterImage(float elapsedTime)
{

    if (ShowAfterImage)
    {
        StackAfterImageTime -= elapsedTime;

        if(StackAfterImageTime<=0.0f)
        {
            ShowAfterImage = false;
        }
    }


    switch (type)
    {
    case Type::NONE: break;
    case Type::RED:
        mAfterImage.Model->SetMaterialColor({ 0.8f,0.8f,0.8f,StackAfterImageTime });
        break;
    case Type::BLUE:
        mAfterImage.Model->SetMaterialColor({ 0.8f,0.8f,0.8f,StackAfterImageTime });
        break;
    default: ;
    }
    

}


void Player::CalcParticle()
{
  


}

void Player::PerformanceDead(float elapsedTime)
{


    StackPerformanceTime += elapsedTime;
    switch (GameSystem::Instance().GetCameraDirection())
    {
    case GameSystem::CAMERA_DIRECTION::SIDE:

        r.Angle.x = Calcu3D::LerpFloat(r.Angle.x, DirectX::XMConvertToRadians(30.0f), 10.0f * elapsedTime);
        t.Position.y -= 30.0f * elapsedTime;
        t.Position.z -= 30.0f * elapsedTime;
        r.Angle.z += DirectX::XMConvertToRadians(360.0f) * elapsedTime;

        if(StackPerformanceTime>=2.0f)
        {
            alive = false;
        }

        break;
    case GameSystem::CAMERA_DIRECTION::TOP: 
        r.Angle.z += DirectX::XMConvertToRadians(360.0f) * elapsedTime;
        t.Position.x-= 30.0f * elapsedTime;
        if (StackPerformanceTime >= 2.0f)
        {
            alive = false;
        }

        break;
    default: ;
    }


}


//--------------------<�v���C���[�̍ŏ��̃|�[�Y>--------------------//
void Player::FirstPose(float elapsedTime)
{
    

    // ���o�ݒ�
    if(IsLightPerformance)
    {

        PerformRate = 0.0f;
        PerformState = 0;
        IsPerforming = false;
        r.Angle.y = 0.0f;
        t.Position = { 0.0f,0.0f,0.0f };
        sprBlackCurtain.Color.w = 0.0f;
        return;
    }


    // �A�j���[�V�����̍X�V
    Model->Update(elapsedTime);
    static const auto FirstPosition = t.Position;
    static const DirectX::XMFLOAT3 FirstRadian = {DirectX::XMConvertToRadians(54.0f),DirectX::XMConvertToRadians(120.0f),DirectX::XMConvertToRadians(20.0f), };

    // ���o���΂�
    const auto DecideButton = GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_Y | GamePad::BTN_X;
    if(GamePad::Instance().GetButton()&DecideButton)
    {
        // �Ó]
        StackSkipTimer += elapsedTime;
    }
    if(StackSkipTimer>=0.3f)
    {
        PreferSkipPerformance = true;
    }

    if(PreferSkipPerformance)
    {
        sprBlackCurtain.Color.w += elapsedTime * 5.0f;
    }
    if(sprBlackCurtain.Color.w>=1.0f)
    {
        sprBlackCurtain.Color.w = 1.0f;
        PerformRate = 0.0f;
        PerformState = 0;
        IsPerforming = false;
        r.Angle.y = 0.0f;
        t.Position = { 0.0f,0.0f,0.0f };
        return;
    }

    switch(PerformState)
    {
    case 0:
        // ������
        // �ʒu
        //t.Position = { -100.0f,50.0f,-100.0f };
        hdl_SonicBoom = efk_SonicBoom->Play(t.Position, {}, 0.2f);
        PerformState++;
        /*FallThrough*/
    case 1:
    {
        // �x�W�G�Ȑ����g���Ĉʒu���ق���
        PerformRate += elapsedTime * 0.75f;
        std::vector<DirectX::XMFLOAT3> vec{};
        vec.emplace_back(FirstPosition);
        vec.emplace_back(DirectX::XMFLOAT3{ -150.0f,40.0f,-300.0f });
        vec.emplace_back(DirectX::XMFLOAT3{ -100.0f,10.0f, -180.0f });
        vec.emplace_back(DirectX::XMFLOAT3{ 0.0f,0.0f,-100.0f });
        t.Position = Calcu3D::BezierCurve(vec, PerformRate);

        PerformanceAngleRatio += elapsedTime * 1.2f;
        const DirectX::XMFLOAT3 EndRad = { Calcu3D::ConvertDegreeToRadian(30.0f,100.0f,60.0f) };
        r.Angle = Calcu3D::LerpFloat3(FirstRadian, EndRad, PerformanceAngleRatio);
        if(PerformanceAngleRatio>=1.0f)
        {
            PerformState++;
            PerformanceAngleRatio = 0.0f;
            DirectX::XMFLOAT3 rad = r.Angle;
            rad = {
                -40.0f,
                -180.0f,
                0.0f
            };
            hdl_SonicBoom = efk_SonicBoom->Play(t.Position, rad, 0.15f);
        }

    }
        break;

    case 2:
    {
        const DirectX::XMFLOAT3 FirstRad2 = { Calcu3D::ConvertDegreeToRadian(30.0f,100.0f,60.0f) };

        // �x�W�G�Ȑ����g���Ĉʒu���ق���
        PerformRate += elapsedTime * 0.5f;
        std::vector<DirectX::XMFLOAT3> vec{};
        vec.emplace_back(FirstPosition);
        vec.emplace_back(DirectX::XMFLOAT3{ -150.0f,40.0f,-300.0f });
        vec.emplace_back(DirectX::XMFLOAT3{ -100.0f,10.0f, -180.0f });
        vec.emplace_back(DirectX::XMFLOAT3{ 0.0f,0.0f,-100.0f });
        t.Position = Calcu3D::BezierCurve(vec, PerformRate);

        PerformanceAngleRatio += elapsedTime * 2.2f;
        const DirectX::XMFLOAT3 EndRad = { Calcu3D::ConvertDegreeToRadian(21.0f,60.0f,310.0f) };
        r.Angle = Calcu3D::LerpFloat3(FirstRad2, EndRad, PerformanceAngleRatio);
        if (PerformanceAngleRatio >= 1.0f)
        {
            PerformState++;
            PerformanceAngleRatio = 0.0f;
            hdl_SonicBoom = efk_SonicBoom->Play(t.Position, EndRad, 0.15f);
        }
    }
        break;
    case 3:
    {
        const DirectX::XMFLOAT3 FirstRad2 = { Calcu3D::ConvertDegreeToRadian(21.0f,60.0f,310.0f) };
        // �x�W�G�Ȑ����g���Ĉʒu���ق���
        PerformRate += elapsedTime * 0.5f;
        std::vector<DirectX::XMFLOAT3> vec{};
        vec.emplace_back(FirstPosition);
        vec.emplace_back(DirectX::XMFLOAT3{ -150.0f,40.0f,-300.0f });
        vec.emplace_back(DirectX::XMFLOAT3{ -100.0f,10.0f, -180.0f });
        vec.emplace_back(DirectX::XMFLOAT3{ 25.0f,0.0f,-100.0f });
        t.Position = Calcu3D::BezierCurve(vec, PerformRate);

        PerformanceAngleRatio += elapsedTime * 2.2f;
        const DirectX::XMFLOAT3 EndRad = { Calcu3D::ConvertDegreeToRadian(0.0f,90.0f,0.0f) };
        r.Angle = Calcu3D::LerpFloat3(FirstRad2, EndRad, PerformanceAngleRatio);
        if (PerformanceAngleRatio >= 1.0f)
        {
            PerformState++;
            PerformanceAngleRatio = 0.0f;
            hdl_SonicBoom = efk_SonicBoom->Play(t.Position, EndRad, 0.15f);
        }
    }
        break;
    case 4:
        // ��u�X���[��
        t.Position.x += 10.0f * elapsedTime;
        PerformRate += elapsedTime;

        if(PerformRate>=0.5f)
        {
            PerformState++;
            PerformRate = 0.0f;
        }
        
        break;
    case 5:
        {
            PerformState++;
        }
        break;
    case 6:
        //      po-zu
        
        PerformState++;
        
    case 7:
        // �X�V
        PerformState++;
        break;
    case 8:
    {
            // hennkei
        PerformState++;
    }
        
    case 9:
        // kousinn
        PerformState++;
        break;
    case 10:
       //  syokiitiniidou
    {
        r.Angle.y = Calcu3D::LerpFloat(r.Angle.y, DirectX::XMConvertToRadians(-20.0f), 10.0f * elapsedTime);
        r.Angle.z = Calcu3D::LerpFloat(r.Angle.z, DirectX::XMConvertToRadians(40.0f), 8.0f * elapsedTime);

        DirectX::XMFLOAT3 move = GetFrontVec();
        t.Position += move * 120.0f * elapsedTime;

        if (r.Angle.y <= DirectX::XMConvertToRadians(-19.0f)&&t.Position.x<=0.0f)
        {
            PerformState++;
            r.Angle.y = DirectX::XMConvertToRadians(-20.0f);
            t.Position.x = 0.0f;
        }
    }
        break;
    case 11:
    {
        r.Angle.y = Calcu3D::LerpFloat(r.Angle.y, DirectX::XMConvertToRadians(0.0f), 8.0f * elapsedTime);
        r.Angle.z = Calcu3D::LerpFloat(r.Angle.z, DirectX::XMConvertToRadians(0.0f), 8.0f * elapsedTime);
        DirectX::XMFLOAT3 move = GetFrontVec();
        t.Position += move * 80.0f * elapsedTime;
        if(t.Position.z>0.0f)
        {
            PerformState++;
        }
    }
        break;
    case 12:

        // �ŏ�����

        PerformRate = 0.0f;
        PerformState = 0;
        IsPerforming = false;
        r.Angle.y = 0.0f;
        break;
        default:
            break;
    }

        
    GuiMenu();
    
}

bool  Player::LastAttack(float elapsedTime)
{
    Model->Update(elapsedTime);

    switch (PerformState)
    {
    case 0:
        PerformState++;
        break;
    case 1:
        // �Ó]
        sprBlackCurtain.Color.w += elapsedTime * 5.0f;
        if (sprBlackCurtain.Color.w >= 1.0f)
        {
            PerformState++;
        }
        break;
    case 2:
        // ������
        t.Position = { 0.0f,-5.0f,0.0f };
        r.Angle = { 0.0f,0.0f,0.0f };

        PerformState++;
        /*FallThrough*/

    case 3:
        // �Ó]����
        sprBlackCurtain.Color.w -= elapsedTime * 5.0f;
        if (sprBlackCurtain.Color.w <= 0.0f)
        {
            // �A�j���[�V�������Đ�
            Model->f_PlayAnimation(AnimationName::finisher_begin);
            sprBlackCurtain.Color.w = 0.0f;
            PerformState++;
        }
        break;
    case 4:
        if(Flags.Check("LastAttack1",Model->GetCurrentAnimationNumber(),Model->GetCurrentAnimationFrame()))
        {
            PerformState++;
            StackPerformanceTime = 0.0f;
        }

        break;
    case 5:
        t.Position.z += 20.0f * elapsedTime;

        if(Flags.Check("LastAttack0",Model->GetCurrentAnimationNumber(),Model->GetCurrentAnimationFrame()))
        {
            PerformState++;
            StackPerformanceTime = 0.0f;
        }
        break;
    case 6:
        // �Ó]
        sprBlackCurtain.Color.w += elapsedTime * 5.0f;
        if (sprBlackCurtain.Color.w >= 1.0f)
        {
            // ������
            t.Position = { 0.0f,0.0f,85.0f };
            r.Angle = { 0.0f,0.0f,0.0f };
            se[LastSlash0]->Play(false);
            PerformState++;
        }
        break;
    case 7:
        // �Ó]����
        sprBlackCurtain.Color.w -= elapsedTime * 5.0f;
        if (sprBlackCurtain.Color.w <= 0.0f)
        {
            sprBlackCurtain.Color.w = 0.0f;
            Model->f_PlayAnimation(AnimationName::finisher_end);
            PerformState++;
        }
        break;
    case 8:
        // �ҋ@

        if (Flags.Check("LastAttack2", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            Model->f_PlayAnimation(AnimationName::finisher_end_pose, true);
            PerformState++;
            StackPerformanceTime = 0.0f;
            se[BossBomb0]->Play(false);
            se[BossBomb2]->Play(false);
            se[BossBomb3]->Play(false);
            se[BossBomb4]->Play(false);
            
            
        }
        break;

    case 9:
        // ����

        StackPerformanceTime += elapsedTime;
        if(StackPerformanceTime>=1.2f)
        {
            StackPerformanceTime = 0.0f;
            PerformState++;
        }
        
        break;
    case 10:
         // �Ó]
        mClearDissolvePower += elapsedTime * 0.8f;
        if (mClearDissolvePower >= 1.0f)
        {
            PerformState++;
        }
        break;
    case 11:
        // ������


        return true;
        break;


    default: 
        break;
    }
    
    return  false;
}

//****************************************************************
// 
//  �`���[�g���A���p�̊֐�
// 
//****************************************************************
bool Player::GetIsMove() // �ړ����Ă��邩�ǂ���
{
    auto mvec = GetMoveVec();
    if(mvec.x>0.01f|| mvec.x < -0.01f || mvec.y>0.01f || mvec.y < -0.01f )
    {
        return true;
    }
    return false;
}


//****************************************************************
    





//--------------------<�v���C���[�ƒe�̏Փ˔���>--------------------//
void Player::VsProjectile(float elapsedTime)
{
	const int index = IsHitProjectile(hitAreaSize);

    if (index >= 0 && StackDodgeTime >= DodgeTime)
    {

        BaseProjectile& proj = ProjectileManager::Instance().GetProjectile(index);
        const int myType = static_cast<int>(type);
        const int projType = static_cast<int>(proj.GetProjType());

        //�e�Ȃ�
        if(projType == static_cast<int>(BaseProjectile::ProjType::REDBULLET) || projType == static_cast<int>(BaseProjectile::ProjType::BLUEBULLET))
        {
            const int damage = 1;

            //�v���C���[�̐F�ƒe�̐F�������ꍇ
            if (myType == projType)
            {
                if(Damaged(damage))
                {
                    se.at(SE::HIT_SAME_COLOR)->Stop();
                    se.at(SE::HIT_SAME_COLOR)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_SAME_COLOR));
                    hdl_UnWeakDamaged = efk_UnWeakDamaged->Play(proj.GetPosition(), {}, 0.155f);
                    efk_UnWeakDamaged->SetAngle(hdl_UnWeakDamaged, { 0.0f,DirectX::XMConvertToRadians(90.0f),0.0f });

                }
                else
                {
                    se.at(SE::HIT_INVINCIBLE)->Stop();
                    se.at(SE::HIT_INVINCIBLE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_INVINCIBLE));
                }
                
            }
            //�Ⴄ�ꍇ
            else
            {
                if(Damaged(damage * damageRatio))                                     
                {
                    se.at(SE::HIT_DIFFERENT_COLOR)->Stop();
                    se.at(SE::HIT_DIFFERENT_COLOR)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_DIFFERENT_COLOR));
                    efk_WeakDamaged->Play(proj.GetPosition(), {}, 0.04f);
                }
                else
                {
                    se.at(SE::HIT_INVINCIBLE)->Stop();
                    se.at(SE::HIT_INVINCIBLE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_INVINCIBLE));
                }
                
           
            }

            LimitInvincibleTime = 0.5f;

            //�e��Hit�񐔂����炷
            proj.Hit();
        }
        //�G�l���M�[�Ȃ�
        else if (projType == static_cast<int>(BaseProjectile::ProjType::REDENERGY))
        {
            //�ԃG�l���M�[�擾
            GetEnergy(Type::RED, increaseEnergy);
            //�e�폜
            ProjectileManager::Instance().Remove(&proj);
        }
        else if (projType == static_cast<int>(BaseProjectile::ProjType::BLUEENERGY))
        {
            //�G�l���M�[�擾
            GetEnergy(Type::BLUE, increaseEnergy);
            //�e�폜
            ProjectileManager::Instance().Remove(&proj);
        }
    }
}
int Player::IsHitProjectile(const DirectX::XMFLOAT3 HitAreaSize) const
{
    if (dodgeSuccess)return -1;

    int hitProjIndex = -1;

	const int projCount = ProjectileManager::Instance().GetProjectileCount();
    for (int i = 0; i < projCount; i++)
    {
        BaseProjectile& proj = ProjectileManager::Instance().GetProjectile(i);

        const int myType = static_cast<int>(type);
        const int projType = static_cast<int>(proj.GetProjType());

        //�e�̐e���v���C���[�Ȃ�continue
        if (proj.GetParent() == BaseProjectile::Parent::BLUEPLAYER || proj.GetParent() == BaseProjectile::Parent::REDPLAYER)continue;

        GameSystem& gameSystem = GameSystem::Instance();

        //�Փ˔���
        if (Collision::CubeVsCube(t.Position, HitAreaSize, proj.GetPosition(), proj.GetHitAreaSize()))
        {
            if (hitProjIndex < 0)hitProjIndex = i;
            else
            {
                if (Collision::Distance(t.Position, proj.GetPosition()) < Collision::Distance(t.Position, ProjectileManager::Instance().GetProjectile(hitProjIndex).GetPosition()))
                {
                    hitProjIndex = i;
                }
            }
        }
	        
    }
    return hitProjIndex;
}


//--------------------<�v���C���[�ƓG�̏Փ˔���>--------------------//
void Player::VsEnemy(float elapsedTime)
{
    const int index = IsHitEnemy(hitAreaSize);

    if (index >= 0 && StackDodgeTime >= DodgeTime)
    {
	    const Enemy& enemy = EnemyManager::Instance().GetEnemy(index);

        const int damage = 1;

    	//�v���C���[�̐F�ƓG�̐F�������Ȃ�
        if (type == enemy.GetType())
        {
            if (Damaged(damage))
            {
                se.at(SE::HIT_SAME_COLOR)->Stop();
                se.at(SE::HIT_SAME_COLOR)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_SAME_COLOR));
            }
            else
            {
                se.at(SE::HIT_INVINCIBLE)->Stop();
                se.at(SE::HIT_INVINCIBLE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_INVINCIBLE));
            }
        }
        //�Ⴄ�ꍇ
        else
        {
            if (Damaged(damage * damageRatio))
            {
                se.at(SE::HIT_DIFFERENT_COLOR)->Stop();
                se.at(SE::HIT_DIFFERENT_COLOR)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_DIFFERENT_COLOR));
            }
            else
            {
                se.at(SE::HIT_INVINCIBLE)->Stop();
                se.at(SE::HIT_INVINCIBLE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_INVINCIBLE));
            }
        }

        //�_���[�W����
        LimitInvincibleTime = 2.0f;
    }
    
}
int Player::IsHitEnemy(const DirectX::XMFLOAT3 HitAreaSize) const
{
    if (dodgeSuccess)return -1;

    int hitEneIndex = -1;

    const int eneCount = EnemyManager::Instance().GetEnemyCount();
    for (int i = 0; i < eneCount; i++)
    {
        Enemy& enemy = EnemyManager::Instance().GetEnemy(i);

        GameSystem& gameSystem = GameSystem::Instance();

        //�o�ꒆ�Ȃ瓖����Ȃ�
        if (enemy.IsAppear())continue;

        //�Փ˔���
        if (Collision::CubeVsCube(t.Position, HitAreaSize, enemy.GetPosition(), enemy.GetHitAreaSize() ))
        {
            if (hitEneIndex < 0) hitEneIndex = i;
            else
            {
                if (Collision::Distance(t.Position, enemy.GetPosition()) < Collision::Distance(t.Position, EnemyManager::Instance().GetEnemy(hitEneIndex).GetPosition()))
                {
                    hitEneIndex = i;
                }
            }
        }
    }

    return hitEneIndex;
}
void Player::AttackAllEnemy(float damage)
{
	for(int i = 0;i<EnemyManager::Instance().GetEnemyCount();i++)
	{
        Enemy& enemy = EnemyManager::Instance().GetEnemy(i);
        enemy.Damage(damage, enemy.GetMaxInvTimer(), true);
	}
}

void Player::fSetCameraShakeFunc(std::function<void()> Function_)
{
    mCameraShakeFunc = Function_;
}


//--------------------<�c���؂�ւ��̍ۂ̕␳>--------------------//
void Player::ChangeCameraDirCorrect()
{
	if(GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::SIDE)
	{
		//side->top
        t.Position.y = Calcu3D::LerpFloat(t.Position.y, 0.0f, 0.1f);
    }
    else if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
    {
        //top->side
        t.Position.x = Calcu3D::LerpFloat(t.Position.x, 0.0f, 0.1f);
    }
}

//--------------------<�q�b�g�X�g�b�v>--------------------//
void Player::HitSlow(float& elapsedTime)
{
    StackHitStopSeconds += elapsedTime;

    if (StackHitStopSeconds < HitStopSecond)
    {
        elapsedTime *= 0.2f;
    }
}

void Player::Slow(float &elapsedTime)
{
    elapsedTime *= 0.3f;
}

//****************************************************************
// 
//  �r�[���̕K�E�Z
// 
//****************************************************************
void Player::Finisher_Beam(float elapsedTime)
{
    switch (FinisherState)
    {
    case 0:
        // �J�����𒍖ڂ�����
        AttentionCamera = true;
        // ���o���̘g��\������
        ProjectileManager::Instance().Clear();
        // �p�x��ς���
        r.Angle.y = DirectX::XMConvertToRadians(60.0f);
        r.Angle.x = 0.0f;
        r.Angle.z = 0.0f;

        // �J�b�g�C���J�n
        spr_CutIn0.Position.x = Calcu3D::LerpFloat(spr_CutIn0.Position.x, 0.0f, 50.0f * elapsedTime);
        if(spr_CutIn0.Position.x<2.0f)
        {
            spr_CutIn0.Position.x = 0.0f;
            FinisherState++;
        }
        break;
    case 1:
        // �ҋ@����
        FinisherRatio += elapsedTime;
        if(FinisherRatio>=0.4f)
        {
            FinisherRatio = 0.0f;
            FinisherState++;
        }
        break;
    case 2:
        // �J�b�g�C���J�n
        spr_CutIn0.Position.x = Calcu3D::LerpFloat(spr_CutIn0.Position.x, -1280.0f, 50.0f * elapsedTime);
        if (spr_CutIn0.Position.x < -1275.0f)
        {
            spr_CutIn0.Position.x = -1275.0f;
            FinisherState++;
        }
        break;
    case 3:
        // ������
        // �J�����𒍖ڂ�����
        AttentionCamera = true;
        // ���o���̘g��\������

        // �p�x��ς���
        r.Angle.y = DirectX::XMConvertToRadians(60.0f);
        r.Angle.x = 0.0f;
        r.Angle.z = 0.0f;

        // �A�j���[�V�������Đ�
        Model->f_PlayAnimation(AnimationName::FinisherBeamBegin, false);
        FinisherState++;

        /*FallThrough*/
        break;
    case 4:
        // �ό`�I���Ƌ��ɂ��ߓ���
        if(Flags.Check("EndStartBeamMotion", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            FinisherState++;
        }

        break;
    case 5:
        // ���ߓ���
        Model->f_PlayAnimation(AnimationName::FinisherBeamLoop, true);
        se[BEAMCHARGE]->Play(false, OptionSystem::Instance().GetSeVolume());
        hdl_Charge = efk_Charge->Play(t.Position, {}, 0.2f);
        FinisherState++;
        /*Fallthrough*/

    case 6:
        // ���ߏI���
        FinisherRatio += elapsedTime;
        if (FinisherRatio >= 1.0f)
        {
            FinisherState++;
            FinisherRatio = 0.0f;
        }
        break;
    case 7:
    {
        // �r�[���ł�
        se[BEAMCHARGE]->Stop();
        se[BEAM]->Play(false, OptionSystem::Instance().GetSeVolume());

        ID3D11Device* p_device = SceneManager::Instance().GetDevice();

        Light->SetRange(-4.0f);
        auto b0 = new Beam(p_device, BaseProjectile::Parent::REDPLAYER,
            t.Position);

        ProjectileManager::Instance().RegisterProjectile(b0);

        // �J������������
        AttentionCamera = false;

        // �p�x��������
        r.Angle.y = 0.0f;
        FinisherState++;
    }
        break;
    case 8:
        // �r�[�������I���
        FinisherRatio += elapsedTime;
        if (FinisherRatio >= 3.0f)
        {
            Model->f_PlayAnimation(AnimationName::FinisherBeamEnd);
            FinisherState++;
            FinisherRatio = 0.0f;
            Light->SetRange(30.0f);
        }
        break;
    case 9:
        AttentionCamera = true;
        // �ŏ�����
        FinisherState = 0;
        IsFinisher = false;
        StackInvincibleTime = 0.0f;
        type = Type::BLUE;
        // �A�j���[�V����
        EndTransform = false;
        // �̗͉�
        HitPoint += HealHpCount;
        // ����\�񐔉�
        DodgeCount = MaxDodgeCount;
        se[BEAM]->Stop();

        //�G�l���M�[�ݒ�
        EnergyGauge = MinEnergyCount;

        se.at(SE::MODE_CHANGE)->Stop();
        se.at(SE::MODE_CHANGE)->Play(false, OptionSystem::Instance().GetSeVolume()* volumes.at(SE::MODE_CHANGE));

        // �G�t�F�N�g�Đ�
        efk_Heal->Play(t.Position, {}, 0.10f);

        //UI�ɕς�������Ƃ�m�点��
        GameSystem::Instance().SetIsChangeColor(true);

        //se�Đ�
        se.at(HEAL)->Stop();
        se.at(HEAL)->Play(false, OptionSystem::Instance().GetSeVolume()* volumes.at(HEAL));

        //--------------------<�̗͂̏���ݒ�>--------------------//
        HitPoint = min(MaxHitPoint, HitPoint);

        //--------------------<�̗͂̉����ݒ�>--------------------//
        HitPoint = max(HitPoint, 0);
        // �e�����ׂď���
        ProjectileManager::Instance().Clear();

        break;
    default: 
        break;
    }


}

void Player::Finisher_BeamLight(float elapsedTime)
{
    switch (FinisherState)
    {
    case 0:
        // �J�b�g�C���J�n
        spr_CutIn0.Position.x = Calcu3D::LerpFloat(spr_CutIn0.Position.x, 0.0f, 50.0f * elapsedTime);
        ProjectileManager::Instance().Clear();
        if (spr_CutIn0.Position.x < 2.0f)
        {
            spr_CutIn0.Position.x = 0.0f;
            FinisherState++;
        }
        break;
    case 1:
        // �ҋ@����
        FinisherRatio += elapsedTime;
        if (FinisherRatio >= 0.2f)
        {
            Model->f_PlayAnimation(AnimationName::FinisherBeamLoop, true);
            FinisherRatio = 0.0f;
            FinisherState++;
        }
        break;
    case 2:
        // �J�b�g�C���J�n
        spr_CutIn0.Position.x = Calcu3D::LerpFloat(spr_CutIn0.Position.x, -1280.0f, 50.0f * elapsedTime);
        if (spr_CutIn0.Position.x < -1275.0f)
        {
            spr_CutIn0.Position.x = -1275.0f;
            FinisherState++;
        }
        break;
    case 3:
    
        FinisherState++;
        /*FallThrough*/
        break;
    case 4:
        // �r�[���ł�
    {
        se[BEAM]->Play(false, OptionSystem::Instance().GetSeVolume());
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        Light->SetRange(-4.0f);
        auto b0 = new Beam(p_device, BaseProjectile::Parent::REDPLAYER,
            t.Position);
        ProjectileManager::Instance().RegisterProjectile(b0);
        FinisherRatio = 0.0f;
        FinisherState++;
    }
        /*FallThrough*/
    case 5:
       // �r�[�������I���
        FinisherRatio += elapsedTime;
        if (FinisherRatio >= 3.0f)
        {
            FinisherState++;
            FinisherRatio = 0.0f;
            Light->SetRange(30.0f);
            Model->f_PlayAnimation(AnimationName::FinisherBeamEnd);
        }
        break;

    case 6:
      
        // �ŏ�����
        FinisherState = 0;
        IsFinisher = false;
        StackInvincibleTime = 0.0f;
        type = Type::BLUE;
        // �A�j���[�V����
        EndTransform = false;
        // �̗͉�
        HitPoint += HealHpCount;
        // ����\�񐔉�
        DodgeCount = MaxDodgeCount;

        //UI�ɕς�������Ƃ�m�点��
        GameSystem::Instance().SetIsChangeColor(true);


        //�G�l���M�[�ݒ�
        EnergyGauge = MinEnergyCount;

        se.at(SE::MODE_CHANGE)->Stop();
        se.at(SE::MODE_CHANGE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::MODE_CHANGE));

        // �G�t�F�N�g�Đ�
        efk_Heal->Play(t.Position, {}, 0.10f);

        //--------------------<�̗͂̏���ݒ�>--------------------//
        HitPoint = min(MaxHitPoint, HitPoint);

        //--------------------<�̗͂̉����ݒ�>--------------------//
        HitPoint = max(HitPoint, 0);
        // �e�����ׂď���
        ProjectileManager::Instance().Clear();
        break;
    default:
        break;
    }

}




void Player::Finisher_Slash(float elapsedTime)
{
    static DirectX::XMFLOAT3 StartPosition{};  // �a��ʒu

    switch (FinisherState)
    {
    case 0:
        // �J�b�g�C���J�n
        spr_CutIn1.Position.x = Calcu3D::LerpFloat(spr_CutIn1.Position.x, 0.0f, 50.0f * elapsedTime);
        ProjectileManager::Instance().Clear();
        EnemiesStop = true;
        if (spr_CutIn1.Position.x < 2.0f)
        {
            spr_CutIn1.Position.x = -20.0f;
            FinisherState++;
        }
        break;
    case 1:
        // �ҋ@����
        FinisherRatio += elapsedTime;
        if (FinisherRatio >= 0.4f)
        {
            FinisherRatio = 0.0f;
            FinisherState++;
        }
        break;
    case 2:
        // �J�b�g�C���I��
        spr_CutIn1.Position.x = Calcu3D::LerpFloat(spr_CutIn1.Position.x, -1280.0f, 50.0f * elapsedTime);
        if (spr_CutIn1.Position.x < -1275.0f)
        {
            spr_CutIn1.Position.x = -1275.0f;
            FinisherState++;
        }
        break;
    case 3:
        // ������
        StartPosition = t.Position;
        FinisherRatio = 0.0f;
        FinisherState++;
    case 4:
        FinisherRatio += elapsedTime * 2.0f;

        // ���܂����ʒu�Ɉړ�
        t.Position = Calcu3D::LerpFloat3(StartPosition, { 0.0f,0.0f,-15.0f }, FinisherRatio);
        if(FinisherRatio>=1.0f)
        {
            Model->f_PlayAnimation(AnimationName::inv_transform);
            mAfterImage.Model->f_PlayAnimation(AnimationName::inv_transform);
            FinisherState++;
            FinisherRatio = 0.0f;
        }
        break;
    case 5:
        if(Flags.Check("EndChangeToHuman", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            FinisherState++;
            FinisherRatio = 0.0f;
        }
        break;
    case 6:
        // ������
        // �J�����𒍖ڂ�����
        AttentionCamera = true;
        // ���o���̘g��\������
        // �p�x��ς���
        r.Angle.y = DirectX::XMConvertToRadians(60.0f);
        r.Angle.x = 0.0f;
        r.Angle.z = 0.0f;

        // �A�j���[�V�������Đ�
        Model->f_PlayAnimation(AnimationName::FinisherSwordBegin, false);
        mAfterImage.Model->f_PlayAnimation(AnimationName::FinisherSwordBegin, false);
        FinisherState++;

        /*FallThrough*/
        break;
    case 7:
        // �ό`�I���Ƌ��ɂ��ߓ���
        if (Flags.Check("a", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            Model->f_PlayAnimation(AnimationName::FinisherSwordSlash, false);
            mAfterImage.Model->f_PlayAnimation(AnimationName::FinisherSwordSlash, false);
            FinisherState++;
        }

        break;
    case 8:
            FinisherState++;
        break;

    case 9:
        // ���ߏI���
        if (Flags.Check("PlaySlashEffect", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            // �G�t�F�N�g�Đ�
            efk_SlashFinisher->Play(t.Position, { 0.0f,DirectX::XMConvertToRadians(-90.0f),DirectX::XMConvertToRadians(160.0f) }, 0.25f);
            se[BEAM]->Play(false, OptionSystem::Instance().GetSeVolume());
            FinisherState++;
        }
        break;
    case 10:
    {
      
        // �J������������
        AttentionCamera = false;
        // �p�x��������
        r.Angle.y = 0.0f;
        EnemiesStop = false;
        FinisherState++;
    }
    break;
    case 11:
        FinisherRatio += elapsedTime;
        if (Flags.Check("EndSlashFinisher", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            //enemy�Ƀ_���[�W��^����
            AttackAllEnemy(FinisherDamage);
            FinisherState++;
            FinisherRatio = 0.0f;
            Light->SetRange(30.0f);
            Model->f_PlayAnimation(AnimationName::wait, true);
            mAfterImage.Model->f_PlayAnimation(AnimationName::wait, true);
        }
        break;
    case 12:
        // �ŏ�����
        IsFinisher = false;
        FinisherState = 0;
        StackInvincibleTime = 0.0f;
        // �̗͉�
        HitPoint += HealHpCount;
        // ����\�񐔉�
        DodgeCount = MaxDodgeCount;
        CompleteElement1Tutorial = true;

        //�G�l���M�[�ݒ�
        EnergyGauge = MinEnergyCount;

        se.at(SE::MODE_CHANGE)->Stop();
        se.at(SE::MODE_CHANGE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::MODE_CHANGE));

        // �G�t�F�N�g�Đ�
        efk_Heal->Play(t.Position, {}, 0.10f);

        //UI�ɕς�������Ƃ�m�点��
        GameSystem::Instance().SetIsChangeColor(true);


        //--------------------<�̗͂̏���ݒ�>--------------------//
        HitPoint = min(MaxHitPoint, HitPoint);

        //--------------------<�̗͂̉����ݒ�>--------------------//
        HitPoint = max(HitPoint, 0);
        // �e�����ׂď���
        ProjectileManager::Instance().Clear();
        type = Type::RED;
        break;
    default:
        break;
    }


}


void Player::Finisher_SlashLight(float elapsedTime)
{
    static DirectX::XMFLOAT3 StartPosition{};  // �a��ʒu

    switch (FinisherState)
    {
    case 0:
        // �J�b�g�C���J�n
        spr_CutIn1.Position.x = Calcu3D::LerpFloat(spr_CutIn1.Position.x, 0.0f, 50.0f * elapsedTime);
        ProjectileManager::Instance().Clear();
        EnemiesStop = true;
        if (spr_CutIn1.Position.x < 2.0f)
        {
            spr_CutIn1.Position.x = -20.0f;
            FinisherState++;
        }
        break;
    case 1:
        // �ҋ@����
        FinisherRatio += elapsedTime;
        if (FinisherRatio >= 0.2f)
        {
            FinisherRatio = 0.0f;
            FinisherState++;
        }
        break;
    case 2:
        // �J�b�g�C���I��
        spr_CutIn1.Position.x = Calcu3D::LerpFloat(spr_CutIn1.Position.x, -1280.0f, 50.0f * elapsedTime);
        if (spr_CutIn1.Position.x < -1275.0f)
        {
            spr_CutIn1.Position.x = -1275.0f;
            FinisherState++;
        }
        break;
    case 3:
        // ������
        StartPosition = t.Position;
        FinisherRatio = 0.0f;
        FinisherState++;
    case 4:
        FinisherRatio += elapsedTime * 4.0f;

        // ���܂����ʒu�Ɉړ�
        t.Position = Calcu3D::LerpFloat3(StartPosition, { 0.0f,0.0f,-15.0f }, FinisherRatio);
        if (FinisherRatio >= 1.0f)
        {
            se[BEAM]->Play(false, OptionSystem::Instance().GetSeVolume());
            Model->f_PlayAnimation(AnimationName::inv_transform);
            mAfterImage.Model->f_PlayAnimation(AnimationName::inv_transform);
            FinisherState++;
            FinisherRatio = 0.0f;
        }
        break;
    case 5:
        if (Flags.Check("EndChangeToHuman", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            FinisherState++;
            FinisherRatio = 0.0f;
        }
        break;
    case 6:
        // ������
        // ���o���̘g��\������
        // �p�x��ς���
        r.Angle.y = DirectX::XMConvertToRadians(0.0f);
        r.Angle.x = 0.0f;
        r.Angle.z = 0.0f;

        // �A�j���[�V�������Đ�
        Model->f_PlayAnimation(AnimationName::FinisherSwordSlash, false);
        mAfterImage.Model->f_PlayAnimation(AnimationName::FinisherSwordSlash, false);
        FinisherState++;

        /*FallThrough*/
        break;
    case 7:
      
            FinisherState++;

        break;
    case 8:
        
        FinisherState++;
        break;

    case 9:
        // ���ߏI���
        if (Flags.Check("PlaySlashEffect", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            // �G�t�F�N�g�Đ�
            efk_SlashFinisher->Play(t.Position, { 0.0f,DirectX::XMConvertToRadians(-90.0f),DirectX::XMConvertToRadians(160.0f) }, 0.25f);
            FinisherState++;
        }
        break;
    case 10:
    {

        // �J������������
        AttentionCamera = false;
        // �p�x��������
        r.Angle.y = 0.0f;
        EnemiesStop = false;
        FinisherState++;
    }
    break;
    case 11:
        FinisherRatio += elapsedTime;
        if (Flags.Check("EndSlashFinisher", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            //enemy�Ƀ_���[�W��^����
            AttackAllEnemy(FinisherDamage);
            FinisherState++;
            FinisherRatio = 0.0f;
            Light->SetRange(30.0f);
            Model->f_PlayAnimation(AnimationName::wait, true);
            mAfterImage.Model->f_PlayAnimation(AnimationName::wait, true);
        }
        break;
    case 12:
        // �ŏ�����
        IsFinisher = false;
        FinisherState = 0;
        StackInvincibleTime = 0.0f;
        // �̗͉�
        HitPoint += HealHpCount;
        // ����\�񐔉�
        DodgeCount = MaxDodgeCount;
        CompleteElement1Tutorial = true;

        //�G�l���M�[�ݒ�
        EnergyGauge = MinEnergyCount;

        se.at(SE::MODE_CHANGE)->Stop();
        se.at(SE::MODE_CHANGE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::MODE_CHANGE));

        // �G�t�F�N�g�Đ�
        efk_Heal->Play(t.Position, {}, 0.10f);


        //UI�ɕς�������Ƃ�m�点��
        GameSystem::Instance().SetIsChangeColor(true);


        //--------------------<�̗͂̏���ݒ�>--------------------//
        HitPoint = min(MaxHitPoint, HitPoint);

        //--------------------<�̗͂̉����ݒ�>--------------------//
        HitPoint = max(HitPoint, 0);
        // �e�����ׂď���
        ProjectileManager::Instance().Clear();
        type = Type::RED;
        break;
    default:
        break;
    }

}


//--------------------<ImGui�̃f�o�b�O>--------------------//
void Player::GuiMenu()
{

#ifdef _DEBUG
    ImGui::Begin("Player");

    ImGui::Checkbox("Debug", &IsDebug);

    ImGui::SliderFloat("Ratio", &PerformRate, 0.0f, 1.0f);

    if (ImGui::Button("ChangeModel"))
    {
        ChangeMode();
    }
    //�ʒu
    ImGui::DragFloat3("Position", &t.Position.x);

    DirectX::XMFLOAT3 deg = {
        XMConvertToDegrees(r.Angle.x),
        XMConvertToDegrees(r.Angle.y),
        XMConvertToDegrees(r.Angle.z)
    };
    ImGui::DragFloat3("Angle", &deg.x);

    r.Angle = {
        XMConvertToRadians(deg.x),
        XMConvertToRadians(deg.y),
        XMConvertToRadians(deg.z)
    };




    // �̗͂Ƒ��x�̑���
    ImGui::SliderInt("HitPoint", &HitPoint,0,15);
    ImGui::SliderFloat("Speed", &Speed, MinSpeed, MaxSpeed, "%.1f");

    // �ˌ�
    ImGui::Checkbox("CanShoot", &CanShoot); //�ˌ��\
    ImGui::InputFloat("StackShootInterval", &StackShootInterval);

    //�G�l���M�[
    ImGui::SliderInt("EnergyGauge", &EnergyGauge, MinEnergyCount, MaxEnergyCount);
    if (ImGui::Button("GetRedEnergy"))
    {
        GetEnergy(Type::RED, increaseEnergy);
    }
    if (ImGui::Button("GetBlueEnergy"))
    {
        GetEnergy(Type::BLUE, increaseEnergy);
    }
    ImGui::SliderInt("increaseEnergy", &increaseEnergy, 0, 10);

    // �_���[�W
    if (ImGui::Button("Damage"))
    {
        Damaged(1);
    }

    // ���G����
    ImGui::Checkbox("IsInvincible", &IsInvincible);

    // ���
    ImGui::Checkbox("ReadyDodge", &ReadyDodge);
    ImGui::InputInt("DodgeCount", &DodgeCount);
    ImGui::Checkbox("DodgeSuccess", &dodgeSuccess);

    ImGui::Checkbox("Finisher", &IsFinisher);
    ImGui::InputInt("FinisherState", &FinisherState);
    if (ImGui::Button("Laser"))
    {
        //Model->f_PlayAnimation(AnimationName::laser);
    }

    if (ImGui::Button("BeamFinisher"))
    {
        IsFinisher = true;
    }

    ImGui::Checkbox("LightFinisher", &IsLightPerformance);

    ImGui::InputFloat("StackDodgeTime", &StackDodgeTime);

    ImGui::End();
#endif

}




