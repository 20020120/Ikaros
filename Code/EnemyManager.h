#pragma once

#include <vector>
#include <memory>
#include "Enemy.h"
#include "Timer.h"
#include"Renderer.h"

#include "gAudioSource.h"

// EnemyManager     ���V���O���g��

// ------------------------ ���� ------------------------ //
// SceneGame���œ��삵�Ă���Player,Projectile,Enemy�Ŏg�p����Ă���
// ��O��SceneTitle��Initialize()��ResetSpawnTimer()���Ă΂�Ă���
// ----------------------------------------------------- //




class EnemyManager
{
public:    // �����ݒ�p
    enum class STAGE
    {
        STAGE1,
        STAGE2,
        STAGE3,
        STAGE4,
        STAGE5,

        //EXTRA1,
        //EXTRA2,
        //EXTRA3,
    };

public:   // �G�̐����֌W
    enum class ENEMY_KIND
    {
        Straight_None,                          // ���i���邾��
        Straight_None_Start_Back,               // ���i�̂�           (�o���C�x���g��납��)
        Straight_None_Start_Front,              // ���i�̂�           (�o���C�x���g�O����)
        Straight_None_Start_Side,               // ���i�̂�           (�o���C�x���g������)
        Straight_None_Bezier_Top,               // �J�[�u�̂�            (�c�V���[�̂ݑΉ�)
        Straight_None_Bezier_Side,              // �J�[�u�̂�            (���V���[�̂ݑΉ�)
        Straight_None_Horizon_Top,              // �����i�̂�

        Straight_Shot_Start_Back,               // ���i�ˌ�           (�o���C�x���g��납��)
        Straight_Shot_Start_Front,              // ���i�ˌ�           (�o���C�x���g�O����)
        Straight_Shot_Start_Side,               // ���i�ˌ�           (�o���C�x���g������)
        Straight_Shot_Bezier_Top,               // �J�[�u�ˌ�            (�c�V���[�̂ݑΉ�)
        Straight_Shot_Bezier_Side,              // �J�[�u�ˌ�            (���V���[�̂ݑΉ�)

        Straight_TargetShot_Start_Back,         // ���i�ڕW�ˌ�        (�o���C�x���g��납��)
        Straight_TargetShot_Start_Front,        // ���i�ڕW�ˌ�        (�o���C�x���g�O����)
        Straight_TargetShot_Start_Side,         // ���i�ڕW�ˌ�        (�o���C�x���g������)
        Straight_TargetShot_Bezier_Top,         // �J�[�u�ڕW�ˌ�         (�c�V���[�̂ݑΉ�)
        Straight_TargetShot_Bezier_Side,        // �J�[�u�ڕW�ˌ�         (���V���[�̂ݑΉ�)

        
        
        //Wave_None,                              // �g��ɓ�������
        //Wave_Shot_B,                            // �g��ɓ����Ȃ��猂�� (�e)
        //Wave_Shot_E,                            // �g��ɓ����Ȃ��猂�� (�G�l���M�[)
        //Lock_Shot_B,                            // �Œ�C��           (�e)
        //Lock_Shot_E,                            // �Œ�C��           (�G�l���M�[)
        Lock_MultiShot_B,                       // �g�U�ˌ�����       �i�e�j�@�@�@�@�@�@�@�@             �N�W���^�̓G�\��
        Lock_MultiShot_E,                       // �g�U�ˌ�����       �i�G�l���M�[�j�@�@�@�@             �N�W���^�̓G�\��
        Lock_TargetMultiShot_B,                 // �g�U�ڕW�ˌ�����     (�e)
        Lock_TargetMultiShot_B_Hard,            // �g�U�ڕW�ˌ�����     (�e)
        Lock_TargetSingleShot,                  // �ˌ�����           �i�e�j
        Lock_TargetSingleShot_Hard,             // �ˌ�����           �i�e�j
        Lock_Straight_Target,                   // �_�������Ă���ڕW�ɓˌ�����                       �E�c�{�^�̓G�\��
        Lock_Straight_Target_Hard,              // �_�������Ă���ڕW�ɓˌ�����                       �E�c�{�^�̓G�\��
        Translate_Shot_B_Top,                   // x�����Ɏˌ�����    �i�e�j(�c�V���[�̂ݑΉ�)
        Translate_Shot_B_Top_Hard,              // x�����Ɏˌ�����    �i�e�j(�c�V���[�̂ݑΉ�)
        Translate_Shot_B_Side,                  // y�����Ɏˌ�����    �i�e�j(���V���[�̂ݑΉ�)
        Translate_Shot_B_Side_Hard,             // y�����Ɏˌ�����    �i�e�j(���V���[�̂ݑΉ�)
        Rotate_AroundShot_E,                    // ��]���Ȃ���A�S�����ɃG�l���M�[������                 �q�g�f�^�̗\��
        Move_Round_TargetShot_Side,             // ���V���Ȃ���ڕW�Ɏˌ�����i���V���[�̂ݑΉ��j
        Move_Round_TargetShot_Side_Hard,        // ���V���Ȃ���ڕW�Ɏˌ�����i���V���[�̂ݑΉ��j
        //Back_Front_Straight_None,               // ��납�痈�đO���Ń^�[��������A�O�i��������

        Boss1,                                   // �{�X1
        Boss2,                                   // �{�X2
        Boss3,                                   // �{�X3
        Boss4,                                   // �{�X4
        Boss5,                                   // �{�X5
        Boss_Ex1,                                // �G�N�X�g���{�X1
        Boss_Ex2,                                // �G�N�X�g���{�X2
        Boss_Ex3,                                // �G�N�X�g���{�X3

    };
private:
    // �G�̐����p�f�[�^
    struct SpawnData final
    {
        float spawn_time;                                       // ��������
        std::vector<ENEMY_KIND> kinds = {};                     // �G�̎��
        std::vector<uint8_t> sizes = {};                        // �G�̗�
        std::vector<std::vector<float>> pos_widthss = {};       // �G�̐����ʒu float�l (����ʂȂ�c���A�@�c��ʂȂ牡��) (z���̈ʒu�� y = 0.0f)
        std::vector<std::vector<BaseObject::Type>> typess = {}; // �G�̑���
        std::vector<std::vector<float>> z_shiftss = {};         // �d�Ȃ�Ȃ��p�� z�l �̂��炷��
    };


private:
    EnemyManager()
    {
        spawn_timer.Initialize(nullptr, COUNT::UP, 0.0f);
    }
    ~EnemyManager()
    {

    }
public:
    static EnemyManager& Instance()
    {
        static EnemyManager instance;
        return instance;
    }
    //--------< �֐� >--------//
    void Initialize(ID3D11Device* d, STAGE stage = STAGE::STAGE1);
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);
    void Finalize();
    void UpdateBossAppear(float elapsedTime);
    void Render2D(ID3D11DeviceContext* dc);

    // �G�l�~�[���擾
    [[nodiscard]] int GetEnemyCount() const { return static_cast<int>(enemies.size()); }
    // �G�l�~�[�擾
    [[nodiscard]] Enemy& GetEnemy(int index) const { return *enemies.at(index); }
    // �G�l�~�[�o�^
    void RegisterEnemy(Enemy* enemy);
    // �G�l�~�[�폜
    void Clear();
    // �G�l�~�[�폜
    void Remove(Enemy* enemy);
    //�G�l�~�[���S
    void Death(Enemy* enemy);
	// �ڕW�n�_�̓o�^
    void RegisterTargetPosition(const DirectX::XMFLOAT3* target_);

    const auto& GetCameraFocusBoss() const { return CameraFocusBoss; }
    void SetCameraFocusBoss(const bool t) { CameraFocusBoss = t; }

    const auto& GetTargetPosition() const { return targetPosition; }
    void SetTargetPosition(DirectX::XMFLOAT3 p) { targetPosition = p; }

    //--------------------<�{�X�̓o��t���O>--------------------//
    void SetStartBossAppear(bool a)
    {
        IsStartBossAppear = a;
        BossAppearState = 0;
    }
    bool GetEndBossAppear()const { return IsEndBossAppear; }

    // �{�X��܂ōs������^�C�g���ɖ߂��Ă��A�ŏ�����ɂȂ�Ȃ��̂�
    // �^�C�g����Initialize()�ł��̊֐����Ăяo���B
    void ResetSpawnTimer() { spawn_timer.Reset(); }

    void SetGameEnd(bool a) { GameEnd = a; }
    bool GetGameEnd()const  { return GameEnd; }

    void SetIsLastPerformance(bool a) { IsLastPerformance = a; }
    bool GetIsLastPerformance()const { return IsLastPerformance; }

    // �X�e�[�W�̕ύX�́A����SetStageIndex()���g�p���Ă���A
    // TitleManager��sStageSelect()�ł����ύX�ł��Ȃ�
    void SetStageIndex(STAGE stage_) { stage = stage_; }
    STAGE GetStageIndex() { return stage; }

    // �v���C���[�̕ό`(�Q�[�W�g�p)�̉� ( �ό`�@= return true, �ό`�s�� = return false )
    bool IsPlayerTransformable() { return !is_player_not_transfrom_time; /* ���̕ϐ��͕ό`���ł��Ȃ��Ƃ���true�ɂȂ�̂�,  ! ���Z�q��bool�l�𔽑΂ɂ��� */ }

    void SetLastPerformanceState(int a) { LastPerformanceState = a; }
    int GetLastPerformanceState()const { return LastPerformanceState; }

    // �{�X���BGM�𗬂����ǂ���
    bool GetStartBossBGM();
    void SetStartBossBGM(bool a) { StartBossBGM = true; }

    // �{�X�풆���ǂ���

    bool GetIsBossBattle() { return is_boss_battle; }

    [[nodiscard]] float fGetZoomPower() const { return ZoomPower; }
    void fSetZoomPower(float ZoomPower_) { ZoomPower = ZoomPower_; }

    //****************************************************************
    // 
    //  �`���[�g���A��
    // 
    //****************************************************************

    void TutorialSpownEnemy(int CurrentTutorialState); // �`���[�g���A���p�̓G�̐���
    void TutorialUpdate(float elapsedTime);
    void TutorialSetIsAliveShotTutorialEnemy(bool a) { IsAliveShotTutorialEnemy = a; }
    auto GetIsAliveShotTutorialEnemy() const { return IsAliveShotTutorialEnemy; }

    void TutorialSetIsAliveAttackTutorialEnemy1(bool a) { IsAliveAttackTutorialEnemy1 = a; }
    auto GetIsAliveAttackTutorialEnemy1() const { return IsAliveAttackTutorialEnemy1; }
    void TutorialSetIsAliveAttackTutorialEnemy2(bool a) { IsAliveAttackTutorialEnemy2 = a; }
    auto GetIsAliveAttackTutorialEnemy2() const { return IsAliveAttackTutorialEnemy2; }
    void Tutorial_SetIsAliveElementTutorialEnemy1(bool a) { IsAliveElementTutorialEnemy1 = a; }
    void Tutorial_SetIsAliveElementTutorialEnemy2(bool a) { IsAliveElementTutorialEnemy2 = a; }
    void ResetIsClearEnemies() { IsClearEnemies = false; }
private:
    // ���f���̈�m�ۗp�̃_�~�[����
    void MakeDummyEnemy();

    // �G�̐�������
    void EnemyAppear(float elapsedTime);

    // �G�̐ݒ�
    void SetSpawnData(STAGE stage);
    // �����p�f�[�^�̐ݒ�
    void SetSpawnDatas_Stage1();
    void SetSpawnDatas_Stage2();
    void SetSpawnDatas_Stage3();
    void SetSpawnDatas_Stage4();
    void SetSpawnDatas_Stage5();
    void SetSpawnDatas_Extra1(){};
    //void SetSpawnDatas_Extra2();
    //void SetSpawnDatas_Extra3();

public:
    // �G�̐����֐�
    Enemy* SpawnEnemy(const ENEMY_KIND kind, BaseObject::Type enemy_type);
private:
    // �����ς݂̃f�[�^�̍폜����
    void EraseSpawnData();

private:
    // �J���������؂�ւ� (��񂾂����s timer��47.0f ~ 49.0f�̂Ƃ�����)
    void ChangeCameraOnce();

    // �{�X�O�̕ό`�֎~
    void BossJustThen();

    //--------------------<�f�o�b�O�p>--------------------//
    void GuiMenu();

private:
    static constexpr float easy_boss_time   = 86.0f;
    static constexpr float normal_boss_time = 125.0f;


private:
    //--------< �ϐ� >--------//
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Enemy>> removes;

    //�_�~�[�G�l�~�[
    std::vector<std::unique_ptr<Enemy>> dummies;

    // ���݂̃X�e�[�W���
    STAGE stage = STAGE::STAGE1;
    

    // �G�o���p�̃t���O�^�C�}�[
    Timer spawn_timer;

    // �G�̏o���f�[�^
    std::vector<SpawnData> spawn_datas;

    // �ڕW�n�_
    const DirectX::XMFLOAT3* target;

    // �J���������ݒ�
    bool CameraFocusBoss{ false };
    DirectX::XMFLOAT3 targetPosition{};

	//enemy�j��sound
    std::unique_ptr<AudioSource> se_Bomb;


    // �v���C���[���ό`(�Q�[�W���g�p)�ł��邩�̃t���O     �� true��������ł��Ȃ�
    //
    //  �J�����̑J��(side~top)���ɂł��Ȃ��悤�ɂ���������
    //
    bool is_player_not_transfrom_time = false;


    //--------------------<�{�X�̓o��>--------------------//
    int BossAppearState{};    // �{�X�̓o��G�t�F�N�g
    Renderer sprWarning{};
    bool IsStartBossAppear{ false };
    bool IsEndBossAppear{ false };

    bool StartBossBGM{ false };  // �{�X��BGM�Đ��J�n
    bool PlayingBossBGM{ false }; // �{�X��BGM�Đ������ǂ���
    bool GameEnd{};

    bool IsLastPerformance{};
    int LastPerformanceState{}; // �Ō�̉��o

    bool is_boss_battle = false; // �{�X�풆��
    float ZoomPower{ 0.0f };
    //****************************************************************
    // 
    // �`���[�g���A���p�̕ϐ�
    // 
    //****************************************************************

    bool IsAliveShotTutorialEnemy;
    bool IsAliveAttackTutorialEnemy1;
    bool IsAliveAttackTutorialEnemy2;
    bool IsAliveElementTutorialEnemy1;
    bool IsAliveElementTutorialEnemy2;
    bool IsClearEnemies; // �G����x�S���N���A
};