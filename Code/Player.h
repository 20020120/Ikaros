#pragma once
#include "BaseObject.h"

#include"Controller.h"

#include"SpriteBatch.h"

#include "EventFlag.h"

#include "GameSystem.h"
//#include"ParticleEmitter.h"
#include"Effect.h"
#include"Light.h"
#include "gAudioSource.h"
#include"ConstantBuffer.h"
#include<functional>
class Player final : protected BaseObject
{
public:
    Player(ID3D11Device* d);
    ~Player() override = default;


    void Initialize() override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;
    void Render2D(ID3D11DeviceContext* dc, bool draw);
    void Finalize() override;

    //****************************************************************
    // 
    //  �Q�b�^�[
    // 
    //****************************************************************

    [[nodiscard]] const auto& GetPosition() const { return t.Position; }
    [[nodiscard]] auto* GetGameSystemData() { return &GameSystemDatas; }

    bool GetEndGame()const { return EndGame; }

    void SetPosition(DirectX::XMFLOAT3 pos) { t.Position = pos; }
    void SetLightPerformance(bool a) { IsLightPerformance = a; }
    //****************************************************************
    // 
    //  �����o�֐��i�N���X�O����A�N�Z�X�����֐��j
    // 
    //****************************************************************
    bool Damaged(const int damage); // ��_���[�W
    void GetEnergy(Type t, const int getEnergy); // �G�l���M�[�ƐڐG
    [[nodiscard]] const auto GetIsInvincible() const { return IsInvincible; } // ���G���Ԃ��ǂ���
    [[nodiscard]] const auto GetFrontVec() const { return BaseObject::GetFrontVec(); } // ���G���Ԃ��ǂ���

    void SlowForFinisher(float& elapsedTime); // �K�E�Z���̓X���[�ɂ���

    bool GetIsAlive() const { return alive; }

    bool OnCameraTarget_FlingFinisher()const;

    // �K�E�Z���ǂ���
    bool GetIsFinisher() const { return IsFinisher; }
    bool GetIsBeamFinisher() { return  IsBeamFinisher; }

    //�c���؂�ւ��̍ۂ̕␳
    void ChangeCameraDirCorrect();

    // �q�b�g�X�g�b�v
    void HitSlow(float& elapsedTime);
    void Slow(float& elapsedTime);

    auto GetIsPerforming() const { return IsPerforming; }
    bool GetAttentionCamera() const { return AttentionCamera; } // �J�����𒍖ڂ�����
    bool GetIsFinisher() { return IsFinisher; }
    int GetPerformanceState()const { return PerformState; }
    // �v���C���[�̍ŏ��̌��߃|�[�Y
    void FirstPose(float elapsedTime);

    // �v���C���[�̍Ō�̈ꌂ
    bool  LastAttack(float elapsedTime);

    bool GetEnemiesStop()const { return EnemiesStop; }
    //****************************************************************
    // 
    //   �`���[�g���A���p�̊֐�
    // 
    //****************************************************************

    bool GetIsMove(); // �ړ����Ă��邩�ǂ���
    bool GetTutorial_IsBreakCloseAttack() { return Tutorial_IsBreakCloseAttack; }
    bool GetCompleteDodgeTutorial() { return CompleteDodgeTutorial; }
    bool GetCompleteElement1Tutorial() const { return CompleteElement1Tutorial; }
    void SetIsTutorial(bool a) { IsTutorial = a; }
    void ReSetCompleteDodgeTutorial() { CompleteDodgeTutorial = false; }

    void PerformEndTutorial(float elapsedTime); // �`���[�g���A���̏I�����o

    // �J�����V�F�C�N�֐���ǉ�
    void fSetCameraShakeFunc(std::function<void()> Function_);

private:

    //****************************************************************
    // 
    // �����o�֐�
    // 
    //****************************************************************
    //�֐��|�C���^
    using pFunc = void(Player::*)(float);

    // ���[�h�`�F���W
    void ChangeMode();

    // �ړ�
    pFunc Move[2] = {
        &Player::MoveHorizontal,
        &Player::MoveVertical,
    };
    void MoveVertical(float elapsedTime);
    void MoveHorizontal(float elapsedTime);

    // �ړ�����
    [[nodiscard]] const auto& GetMoveVec() const;

    // ��]�̍X�V
    void RotUpdate(float vecY);

    // �ړ����x���c��̗͂̊�������Z�o
    void CalcSpeedFromHP();

    // ������
    void inputDodge(float elapsedTime);

    //�W���X�g��𐬌����ɃG�l���M�[�𑝂₷
    void DodgeSuccessEnergy(const int pIndex, const int eIndex);

    // ����̉�]
    void RotDodge(float elapsedTime);

    // �ˌ�����
    void InputShooting(float elapsedTime);

    // �G�l���M�[�X�V
    void UpdateEnergy();

    // ���G���ԍX�V
    void UpdateInvTime(float elapsedTime);

    // �s������
    void LimitMoveArea();

    //�ߋ����U��
    void InputCloseAttack(float elapsedTime);

    // �K�E�Z
    void FlyModeFinisher(float elapsedTime);    // ��s�@�`�Ԃ̕K�E�Z�i�ːi�j

    void FlyModeFinisherMini(float elapsedTime);// ��s�@�`�Ԃ̕K�E�Z�i�ȈՔŁj

    void Finisher_Beam(float elapsedTime); // �l�^�̕K�E�Z
    void Finisher_Slash(float elapsedTime); // �l�^�̕K�E�Z

    // �y�ʐݒ�̃t�B�j�b�V���[
    void Finisher_BeamLight(float elapsedTime); // �l�^�̕K�E�Z
    void Finisher_SlashLight(float elapsedTime); // �l�^�̕K�E�Z

    // �p�[�e�B�N���̌v�Z
    void CalcParticle();

    // ���S���̉��o
    void PerformanceDead(float elapsedTime);

    // �c���̈ʒu���v�Z����
    void UpdateAfterImage(float elapsedTime);

    //�Փ˔���
    void VsProjectile(float elapsedTime);
    [[nodiscard]] int IsHitProjectile(const DirectX::XMFLOAT3 HitAreaSize) const;//�������Ă�����index�������ĂȂ�������-1
    void VsEnemy(float elapsedTime);
    [[nodiscard]] int IsHitEnemy(const DirectX::XMFLOAT3 HitAreaSize) const;//�������Ă�����index�������ĂȂ�������-1
    //�S�Ă̓G�Ƀ_���[�W��^����
    void AttackAllEnemy(float damage);



    // �c���̌v�Z
    void CalcAfterImage(float elapsedTime);


    //--------------------<�f�o�b�O�p>--------------------//
    void GuiMenu();

    //****************************************************************
    // 
    // �����o�ϐ�
    // 
    //****************************************************************
    enum AnimationName {
        wait,
        FinisherBeamBegin,
        FinisherBeamLoop,
        FinisherBeamEnd,
        Shot_Recoil,
        Slash_side,
        Slash_top,
        FinisherSwordBegin,
        FinisherSwordSlash,
        GameOver,
        GameClearBegin,
        GameClearLoop,
        transform,
        inv_transform,
        wing,
        finisher_begin,
        finisher_end,
        finisher_end_pose,
    };

    //--------------------<�G�l���M�[>--------------------//
    int EnergyGauge{};  // ���[�h�`�F���W�Ɏg���G�l���M�[�̃Q�[�W(-20~20)
    static constexpr int MaxEnergyCount = { 40 };
    static constexpr int MinEnergyCount = { 0 };
    bool canTransform{ false };
    int increaseEnergy = 5;

    //--------------------<�ό`>--------------------//

    bool EndTransform{};
    static constexpr int HealHpCount = { 4 };

    //--------------------<�̗�>--------------------//
    static constexpr int MaxHitPoint{ 15 }; // �̗͂̍ő�l
    static constexpr int MinHitPoint{ 0 }; // �̗͂̍ő�l
    int HitPoint{};   // �̗�
    bool alive{ true };

    int damageRatio = 2;

    //--------------------<�X�s�[�h>--------------------//
    static constexpr float MaxSpeed{ 60.0f };  // �ő呬�x
    static constexpr float MinSpeed{ 30.0f };  // �ŏ����x
    float Speed{};    // ���x


    //--------------------<���>--------------------//
    bool ReadyDodge{}; // ���ҋ@
    int DodgeCount{}; // ����\��
    static constexpr int MaxDodgeCount = { 5 }; // �ő�����
    bool CanDodge{}; // ����\���ǂ���
    float StackDodgeTime{}; // �W���X�g����̎�t����
    static constexpr float DodgeTime = { 1.0f }; // �W���X�g����̎�t����
    float RotRatio{};  // ��]����p�̕ϐ�

    float DodgeVecX{}; // �ǂ̕����ɉ�����邩
    float DodgeVecY{}; // �ǂ̕����ɉ�����邩
    static constexpr float DodgeSpeed = 40.0f; // ��𑬓x
    bool dodgeSuccess{ false };
    DirectX::XMFLOAT3 dodgeSize{};
    float BeforeDodgeAngle{}; // ���O�̃v���C���[�̊p�x
    DirectX::XMFLOAT2 dodgeAreaScale{ 1,1 };//�����蔻��p

    //--------------------<�ˌ�>--------------------//
    static constexpr float ShootInterval = { 0.2f }; //�ˌ����Ă��玟�Ɍ��Ă�܂ł̊Ԋu�i�b�j
    float StackShootInterval{}; //�ˌ����Ă���̌o�ߎ��ԁi�ˌ��ł��邩�̔���Ɏg���j
    bool CanShoot{}; // 

    DirectX::XMFLOAT3 BulletVelocity{}; // �e�̑��x
    DirectX::XMFLOAT3 BulletVelocityUp{}; // �e�̑��x
    DirectX::XMFLOAT3 BulletVelocityDown{}; // �e�̑��x
    DirectX::XMFLOAT3 BulletVelocityRight{}; // �e�̑��x
    DirectX::XMFLOAT3 BulletVelocityLeft{}; // �e�̑��x

    //--------------------<���G����>--------------------//
    float StackInvincibleTime{}; //�_���[�W���󂯂Ă���̌o�ߎ���
    float LimitInvincibleTime = { 2.0f }; //���G���Ԃ̒����i�b�j
    bool IsInvincible{};  // ���G���ǂ���

    //--------------------<�v���C���[�̍s���͈͐���>--------------------//
    static constexpr  DirectX::XMFLOAT2 LeftTop = { -47.0f,26.0f };
    static constexpr  DirectX::XMFLOAT2 RightBottom = { 47.0f,-26.0f };

    //--------------------<�ߋ����U��>--------------------//
    float attackInterval{ 0.0f };
    static constexpr float MaxAttackInterval{ 0.5f };
    DirectX::XMFLOAT3 attackArea{};
    float StackCloseAttackInterval{};
    const float CloseAttackInterval{ 1.3f };
    bool IsSlash{}; // �a���Ă邩�ǂ���


    //--------------------<�K�E�Z>--------------------//

    bool IsFinisher{};  // �K�E�Z���Ă邩�ǂ���
    int FinisherState{}; // �K�E�Z�̑J�ڊǗ�
    float FinisherRatio{}; // �K�E�Z�̊֐����Ŏg������`�⊮�̊����i���������Ӂj
    float FinisherDamage = { 4.0f };
    bool EnemiesStop{}; // �G�̓������~�߂�

    bool IsBeamFinisher{};
    //--------------------<�q�b�g�X�g�b�v>--------------------//
    float StackHitStopSeconds{}; // �q�b�g�X�g�b�v�̌o�ߎ���
    const float HitStopSecond{ 0.2f };

    //--------------------<���o�Ɋւ���ϐ�>--------------------//
    int PerformState{}; // ���o���Ǘ�����ϐ�
    bool IsPerforming{}; //���o�����ǂ���
    float PerformRate{}; // ���o�̕⊮��
    float PerformanceAngleRatio{ 0.0f };
    bool IsLightPerformance{};
    Renderer spr_CutIn0{}; // �J�b�g�C���̉摜
    Renderer spr_CutIn1{}; // �J�b�g�C���̉摜
    bool IsCutIn{};
    float StackSkipTimer{}; // �X�L�b�v�̒��������ԂɊւ���ϐ�
    bool PreferSkipPerformance{}; // �X�L�b�v������

    bool IsLastPerformance{};

    enum CameraDirection
    {
        Side,
        Top,
    }SlashDir{};


    float StackAfterImageTime{};

    bool ShowAfterImage{};

    int AfterImageState{};

    float StackPerformanceTime = 0.0f;
    bool EndGame = false;

    //--------------------<�`���[�g���A��>--------------------//
    bool Tutorial_IsBreakCloseAttack{ false }; //�ߐڍU���̃`���[�g���A���������ǂ���
    bool CompleteDodgeTutorial{ false };       // ����̃`���[�g���A���������ǂ���
    bool IsTutorial{ false }; // �`���[�g���A�����ǂ���
    bool CompleteElement1Tutorial{ false };   //  �G�l���M�[�̃`���[�g���A�����I������
    int TutorialPerformState{}; // �`���[�g���A���̉��o�Ǘ��ϐ�

    //--------------------<�J����>--------------------//
    bool AttentionCamera{}; // �J�����𒍖ڂ�����
    std::function<void()> mCameraShakeFunc{ nullptr };
    // �f�o�b�O�p
    bool IsDebug{ false };

    GameSystemData GameSystemDatas;

    // �X�v���C�g
    std::unique_ptr<Sprite_Batch> spr_HitArea;
    Renderer sprBlackCurtain{}; // �Ó]�p�̖�


    // �C�x���g�t���O
    EventFlagSystem Flags;

    //--------------------<�O��>--------------------//
    //ParticleEmitter particleEmitter;

    struct GsData
    {
        float pow{};
        DirectX::XMFLOAT3 pad0;
    }mGsData{};
    ConstantBuffer Gs_CBuffer{};

    std::unique_ptr<GamePointLight> Light;
    DirectX::XMFLOAT3 PlayerColor{};
    //****************************************************************
    // 
    // �T�E���h
    // 
    //****************************************************************
    std::vector<std::unique_ptr<AudioSource>> se;
    std::vector<float> volumes;
    enum SE
    {
        MODE_CHANGE,
        HIT_SAME_COLOR,
        HIT_DIFFERENT_COLOR,
        HIT_INVINCIBLE,
        CLOSE_ATTACK,
        GET_ENERGY,
        RED_BULLET_SHOT,
        BLUE_BULLET_SHOT,
        DODGE,
        CRITICAL_DODGE,
        HEAL,
        LastSlash0,
        BossBomb0,
        BossBomb2,
        BossBomb3,
        BossBomb4,
        BEAMCHARGE,
        BEAM,

    };

    //****************************************************************
    // 
    // �G�t�F�N�g
    // 
    //****************************************************************

    std::unique_ptr<Effect> efk_Charge;             // ����
    Effekseer::Handle hdl_Charge = -1;
    std::unique_ptr<Effect> efk_Beam;             // ���[�U�[
    Effekseer::Handle hdl_Beam = -1;
    std::unique_ptr<Effect> efk_MuzzleFlash;        // �}�Y���t���b�V��
    Effekseer::Handle hdl_MuzzleFlash = -1;
    std::unique_ptr<Effect> efk_RedDodgeSuccess;    // ��𐬌����i�ԁj
    Effekseer::Handle hdl_RedDodgeSuccess = -1;
    std::unique_ptr<Effect> efk_BlueDodgeSuccess;   // ��𐬌����i�j
    Effekseer::Handle hdl_BlueDodgeSuccess = -1;
    std::unique_ptr<Effect> efk_WeakDamaged;        // ��_���[�W(��_)
    Effekseer::Handle hdl_WeakDamaged = -1;
    std::unique_ptr<Effect> efk_UnWeakDamaged;      // ��_���[�W(��_����Ȃ�)
    Effekseer::Handle hdl_UnWeakDamaged = -1;
    std::unique_ptr<Effect> efk_InvisibleDamaged;   // ��_���[�W(���G)
    Effekseer::Handle hdl_InvisibleDamaged = -1;
    std::unique_ptr<Effect> efk_SonicBoom;          // �\�j�b�N�u�[��
    Effekseer::Handle hdl_SonicBoom = -1;
    std::unique_ptr<Effect> efk_RedSlash;           // �a���i�ԁj
    Effekseer::Handle hdl_RedSlash = -1;
    std::unique_ptr<Effect> efk_BlueSlash;          // �a���i�j
    Effekseer::Handle hdl_BlueSlash = -1;
    std::unique_ptr<Effect> efk_Heal;               // ��
    Effekseer::Handle hdl_Heal = -1;
    std::unique_ptr<Effect> efk_SlashFinisher;      // ���̕K�E�Z
    Effekseer::Handle hdl_SlashFinisher = -1;

    std::unique_ptr<Sprite_Batch> ClearMask{ nullptr };
    float mClearDissolvePower{ 0.0f };
    //****************************************************************
    // 
    // �c��
    // 
    //****************************************************************
    struct AfterImage
    {
        Transform t{};
        Rotate r{};
        std::unique_ptr<ModelComponent> Model{};
    }mAfterImage;
 };