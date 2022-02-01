#include "EnemyManager.h"

#include "StraightEnemy.h"
#include "WaveEnemy.h"
#include "LockEnemy.h"
#include "WeakEnemy.h"
#include "TutorialEnemies.h"
#include"Boss_0.h"
#include"Boss_1.h"
#include "GameSystem.h"
#include "SceneManager.h"
#include"Boss_2.h"
#include "Boss_3.h"
#include "Boss_4.h"
#include"Boss_5.h"
#include "Boss_6.h"
#include "Boss_7.h"
#include "Boss_8.h"
#include "OptionSystem.h"
#include "gAudio.h"

#define TO_STRING(VariableName) # VariableName

void EnemyManager::Initialize(ID3D11Device* d, STAGE stage_)
{
    enemies.clear();
    IsLastPerformance = false;

#if _DEBUG

    //stage_ = STAGE::NORMAL;

#endif

    is_boss_battle = false;


    // �����f�[�^�̐ݒ�
    SetSpawnData(stage_);
    //stage = stage_;


    //�_�~�[�G�l�~�[����
    MakeDummyEnemy();
    IsClearEnemies = false;

    // �X�v���C�g��������
    sprWarning.Initialize(d, L"./resources/Sprite/CutIn/Warning.png");
    sprWarning.Scale = { 0.65f,0.65f };
    sprWarning.Pivot = { 0.0f,540.0f };
    sprWarning.TexSize = { 1920.0f,1080.0f };
    sprWarning.Position = { 1280.0f,360.0f };

    //se�̏�����
    se_Bomb = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Enemy/Bomb.wav");


    is_player_not_transfrom_time = false;

    IsStartBossAppear = false;
    IsEndBossAppear = false;
    BossAppearState = 0;
    StartBossBGM = false;
    PlayingBossBGM = false;
    GameEnd = false;
}

void EnemyManager::Update(float elapsedTime)
{

    // ���S�̂��ߖ��t���[������
    CameraFocusBoss = false;
    is_player_not_transfrom_time = false;

    // �G�l�~�[�o��
    EnemyAppear(elapsedTime);

    // �{�G�l�~�[�̍X�V
	for(const auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);

	}

    //�_�~�[�G�l�~�[�X�V
	for(const auto& enemy : dummies)
	{
		enemy->Update(elapsedTime);
	}

    // �j������
    for (std::unique_ptr<Enemy>& enemy : removes)
    {
        std::vector<std::unique_ptr<Enemy>>::iterator it =
            std::find(enemies.begin(), enemies.end(), enemy);
        if (it != enemies.end()) { enemies.erase(it); }
        if (enemy != nullptr) { enemy.release(); }
    }
    removes.clear();

    UpdateBossAppear(elapsedTime);


    // �J�����̐؂�ւ� (45.0f ~ 49.0f �̊Ԃň�x�������s)
    ChangeCameraOnce();

    // �{�X��4�b�O�̓v���C���[���ό`�ł��Ȃ�����
    BossJustThen();

    //--------------------<�f�o�b�O���j���[>--------------------//
    GuiMenu();
}

void EnemyManager::Render(ID3D11DeviceContext* dc)
{
    for (const auto& enemy : enemies)
    {
        enemy->Render(dc);
    }
}

void EnemyManager::Render2D(ID3D11DeviceContext* dc)
{
    sprWarning.Render(dc);
}


void EnemyManager::Finalize()
{
    Clear();

    se_Bomb->Release();

    // �����ł͎��Ԃ����Z�b�g���Ȃ�
}

//--------------------<�{�X�o�ꎞ�̉��o���X�V>--------------------//
void EnemyManager::UpdateBossAppear(float elapsedTime)
{
    if (!IsStartBossAppear) return;

    static float BossAppearWaitTime = 0.0f; /*Static�ϐ��͈�������*/

    switch (BossAppearState)
    {
    case 0:
         /*������*/
        BossAppearState++;
        /*FallThrough*/
    case 1:
        sprWarning.Position.x = Calcu3D::LerpFloat(sprWarning.Position.x, 0.0f, 10.0f * elapsedTime);

        if(sprWarning.Position.x<=1.0f)
        {
            sprWarning.Position.x = 0.0f;
            BossAppearState++;
        }
        break;
    case 2:
        /*�ҋ@����*/
        BossAppearWaitTime += elapsedTime;
        if(BossAppearWaitTime>=0.5f)
        {
            BossAppearWaitTime = 0.0f;
            BossAppearState++;
        }
        break;
    case 3:
        sprWarning.Position.x = Calcu3D::LerpFloat(sprWarning.Position.x, -1280.0f, 10.0f * elapsedTime);

        if (sprWarning.Position.x <= -1279.0f)
        {
            sprWarning.Position.x = -1280.0f;
            BossAppearState++;
        }
        break;
    case 4:
        /*�ŏ�����*/
        IsStartBossAppear = false;
        IsEndBossAppear = true;
        BossAppearState = 0;
        break;
    default: ;
    }
}



void EnemyManager::SetSpawnData(STAGE stage)
{

    constexpr BaseObject::Type red = BaseObject::Type::RED;
    constexpr BaseObject::Type blue = BaseObject::Type::BLUE;

#if _DEBUG
    constexpr float time = 0.0f;
    //spawn_timer.Reset();
    //spawn_timer.Add(normal_boss_time);
#endif


    // �����f�[�^�̃��Z�b�g
    spawn_datas.clear();


    
    // �}�N��
#define SetSpawnDataMacro(Value) \
    if(stage == STAGE::STAGE##Value){\
    if (spawn_timer.NowTime() >= (easy_boss_time + ((static_cast<int>(Value) == 2) ? -4.0f : 0.0f) )){\
        spawn_timer.Reset();\
        spawn_timer.Add(easy_boss_time);\
        GameSystem::Instance().RetryBoss(easy_boss_time);\
        spawn_datas = {\
             { SpawnData{{easy_boss_time + ((static_cast<int>(Value) == 2) ? -4.0f : 0.0f)},{ENEMY_KIND::Boss##Value},{1},{{0.0f}},{{red}},{{0.0f}}}},\
        };\
        is_boss_battle = true;\
        return;\
    }\
    spawn_timer.Reset();\
    SetSpawnDatas_Stage##Value();\
    return;\
    }

    // �X�e�[�W 1
    SetSpawnDataMacro(1);
    // �X�e�[�W 2
    SetSpawnDataMacro(2);
    // �X�e�[�W 3
    SetSpawnDataMacro(3);
    //// �X�e�[�W 4
    //SetSpawnDataMacro(4)
    //// �X�e�[�W 5
    //SetSpawnDataMacro(5)

    if (stage == STAGE::STAGE4)
    {
        if (spawn_timer.NowTime() >= normal_boss_time)
        {
            spawn_timer.Reset();
            spawn_timer.Add(normal_boss_time);
            GameSystem::Instance().RetryBoss(normal_boss_time);
            spawn_datas = {
                 { SpawnData{{normal_boss_time},{ENEMY_KIND::Boss_Ex2},{1},{{0.0f}},{{red}},{{0.0f}}}},
            };
            is_boss_battle = true;
            return;
        }
        spawn_timer.Reset();
        SetSpawnDatas_Stage4();
        return;
    }


    if (stage == STAGE::STAGE5)
    {
        if (spawn_timer.NowTime() >= normal_boss_time - 4.0f)
        {
            spawn_timer.Reset();
            spawn_timer.Add(normal_boss_time);
            GameSystem::Instance().RetryBoss(normal_boss_time);
            spawn_datas = {
                 { SpawnData{{normal_boss_time},{ENEMY_KIND::Boss_Ex3},{1},{{0.0f}},{{red}},{{0.0f}}}},
            };
            is_boss_battle = true;
            return;
        }
        spawn_timer.Reset();
        SetSpawnDatas_Stage5();
        return;
    }


#if 0
    // �X�e�[�W 1
    if(stage == STAGE::STAGE1)
    {
        // �O��{�X��܂ōs���Ă�����
        if (spawn_timer.NowTime() >= easy_boss_time)
        {
            spawn_timer.Reset();
            spawn_timer.Add(easy_boss_time);
            GameSystem::Instance().RetryBoss(easy_boss_time);

            spawn_datas = {
                 {
                     SpawnData{
                     {easy_boss_time},
                     {ENEMY_KIND::Boss1},
                     {1},
                     {{0.0f}},
                     {{red}}                          ,
                     {{0.0f}}}
                 },
            };



            return;
        }

        spawn_timer.Reset();

#if _DEBUG
        spawn_timer.Add(time);
#endif

        // �����f�[�^�̐ݒ� 1
        SetSpawnDatas_Stage1();
    }

    // �X�e�[�W 2
    else if (stage == STAGE::STAGE2)
    {
        // �O��{�X��܂ōs���Ă�����
        if (spawn_timer.NowTime() >= easy_boss_time)
        {
            spawn_timer.Reset();
            spawn_timer.Add(easy_boss_time);
            GameSystem::Instance().RetryBoss(easy_boss_time);

            spawn_datas = {
                 {
                     SpawnData{
                     {easy_boss_time},
                     {ENEMY_KIND::Boss1},
                     {1},
                     {{0.0f}},
                     {{red}}                          ,
                     {{0.0f}}}
                 },
            };



            return;
        }

        spawn_timer.Reset();

#if _DEBUG
        spawn_timer.Add(time);
#endif

        // �����f�[�^�̐ݒ� 2
        SetSpawnDatas_Stage2();
    }

    // �X�e�[�W 3
    else if (stage == STAGE::STAGE3)
    {
        // �O��{�X��܂ōs���Ă�����
        if (spawn_timer.NowTime() >= easy_boss_time)
        {
            spawn_timer.Reset();
            spawn_timer.Add(easy_boss_time);
            GameSystem::Instance().RetryBoss(easy_boss_time);

            spawn_datas = {
                 {
                     SpawnData{
                     {easy_boss_time},
                     {ENEMY_KIND::Boss1},
                     {1},
                     {{0.0f}},
                     {{red}}                          ,
                     {{0.0f}}}
                 },
            };



            return;
        }

        spawn_timer.Reset();

#if _DEBUG
        spawn_timer.Add(time);
#endif

        // �����f�[�^�̐ݒ� 3
        SetSpawnDatas_Stage3();
    }

#endif

#undef SetSpawnDataMacro


    // �}�N��
#define SetSpawnDataExtraMacro(Value) \
    if(stage == STAGE::EXTRA##Value){\
    if (spawn_timer.NowTime() >= normal_boss_time){\
        spawn_timer.Reset();\
        spawn_timer.Add(normal_boss_time);\
        GameSystem::Instance().RetryBoss(normal_boss_time);\
        spawn_datas = {\
             { SpawnData{{normal_boss_time},{ENEMY_KIND::Boss_Ex##Value},{1},{{0.0f}},{{red}},{{0.0f}}}},\
        };\
        return;\
    }\
    spawn_timer.Reset();\
    SetSpawnDatas_Extra##Value();\
    return;\
    }


    //// �X�e�[�W �G�N�X�g�� 1
    //SetSpawnDataExtraMacro(1)
    //// �X�e�[�W �G�N�X�g�� 2
    //SetSpawnDataExtraMacro(2)
    //// �X�e�[�W �G�N�X�g�� 3
    //SetSpawnDataExtraMacro(3)


#if 0
    // �X�e�[�W �G�N�X�g�� 1
    else if(stage == STAGE::EXTRA1)
    {
        if (spawn_timer.NowTime() >= normal_boss_time)
        {
            spawn_timer.Reset();
            spawn_timer.Add(normal_boss_time);
            GameSystem::Instance().RetryBoss(normal_boss_time);

            spawn_datas = {
                 {
                     SpawnData{
                     {normal_boss_time},
                     {ENEMY_KIND::Boss2},
                     {1},
                     {{0.0f}},
                     {{red}}                          ,
                     {{0.0f}}}
                 },
            };




            return;
        }

        spawn_timer.Reset();

#if _DEBUG
        spawn_timer.Add(time);
#endif

        // �����f�[�^�̐ݒ� extra1
        SetSpawnDatas_Extra1();
    }


    // �X�e�[�W �G�N�X�g�� 2
    else if (stage == STAGE::EXTRA2)
    {
    if (spawn_timer.NowTime() >= normal_boss_time)
    {
        spawn_timer.Reset();
        spawn_timer.Add(normal_boss_time);
        GameSystem::Instance().RetryBoss(normal_boss_time);

        spawn_datas = {
             {
                 SpawnData{
                 {normal_boss_time},
                 {ENEMY_KIND::Boss2},
                 {1},
                 {{0.0f}},
                 {{red}}                          ,
                 {{0.0f}}}
             },
        };




        return;
    }

    spawn_timer.Reset();

#if _DEBUG
    spawn_timer.Add(time);
#endif

    // �����f�[�^�̐ݒ� extra2
    SetSpawnDatas_Extra2();
    }


    // �X�e�[�W �G�N�X�g�� 3
    else if (stage == STAGE::EXTRA3)
    {
    if (spawn_timer.NowTime() >= normal_boss_time)
    {
        spawn_timer.Reset();
        spawn_timer.Add(normal_boss_time);
        GameSystem::Instance().RetryBoss(normal_boss_time);

        spawn_datas = {
             {
                 SpawnData{
                 {normal_boss_time},
                 {ENEMY_KIND::Boss2},
                 {1},
                 {{0.0f}},
                 {{red}}                          ,
                 {{0.0f}}}
             },
        };




        return;
    }

    spawn_timer.Reset();

#if _DEBUG
    spawn_timer.Add(time);
#endif

    // �����f�[�^�̐ݒ� extra3
    SetSpawnDatas_Extra3();
    }
#endif

#undef SetSpawnDataExtraMacro

}



void EnemyManager::Clear()
{
    spawn_datas.clear();

    for (std::unique_ptr<Enemy>& enemy : removes)
    {
        std::vector<std::unique_ptr<Enemy>>::iterator it =
            std::find(enemies.begin(), enemies.end(), enemy);
        if (it != enemies.end()) { enemies.erase(it); }
        if (enemy != nullptr) { enemy.release(); }
    }
    removes.clear();

    enemies.clear();

    dummies.clear();
}


void EnemyManager::RegisterEnemy(Enemy* enemy)
{
    enemies.emplace_back(enemy);
}


void EnemyManager::Remove(Enemy* enemy)
{
    removes.emplace_back(enemy);
}

void EnemyManager::Death(Enemy* enemy)
{
    se_Bomb->Stop();
    se_Bomb->Play(false, OptionSystem::Instance().GetSeVolume() * 0.8f);

    removes.emplace_back(enemy);
}


//�G�l�~�[�o��
void EnemyManager::EnemyAppear(float elapsedTime)
{

    // �^�C�}�[�̍X�V
    spawn_timer.Update(elapsedTime);

    if (spawn_datas.empty())return;

    // �z��̈�ԏ��߂̐����f�[�^�̂݊m�F����
    auto data = spawn_datas.begin();

    // �������Ԃ������f�[�^�̃t���O�l�ȏ�Ȃ���s
    if (spawn_timer.NowTime() < data->spawn_time) return;

    const GameSystem::CAMERA_DIRECTION camera_direction = GameSystem::Instance().GetCameraDirection();

    // ��ޕ����[�v
    for(int kind_i = 0;kind_i < data->kinds.size();++kind_i)
    {

        constexpr float default_spawn_z = 60.0f;            // ������Z��60.0f�ŌŒ�


        // �Y���̎�ނ̕�������������
        for(int value_i = 0; value_i < data->sizes.at(kind_i);++value_i)
        {
            // ��������G�̎���
            Enemy* enemy = SpawnEnemy(data->kinds.at(kind_i),data->typess.at(kind_i).at(value_i));

            /* �����������̂̏����ݒ� */

            // �ʒu���ݒ�
            if (camera_direction == GameSystem::CAMERA_DIRECTION::SIDE)
            { enemy->t.Position.y = data->pos_widthss.at(kind_i).at(value_i); }
            else
            { enemy->t.Position.x = data->pos_widthss.at(kind_i).at(value_i); }


            enemy->t.Position.z = default_spawn_z;

            enemy->Initialize();

            enemy->t.Position.z += data->z_shiftss.at(kind_i).at(value_i);

            /* �����ݒ肨��� */

            // ���������G�̎��Ԃ�Manager�ɓo�^
            RegisterEnemy(enemy);
        }
    }

    // �{�X���o�������Ƃ��A�J������side�̂܂܂�������Top�ɕς���
    if(data->kinds.at(0) == ENEMY_KIND::Boss1 ||
        data->kinds.at(0) == ENEMY_KIND::Boss2 ||
        data->kinds.at(0) == ENEMY_KIND::Boss3 ||
        data->kinds.at(0) == ENEMY_KIND::Boss4 ||
        data->kinds.at(0) == ENEMY_KIND::Boss5 ||
        data->kinds.at(0) == ENEMY_KIND::Boss_Ex1 ||
        data->kinds.at(0) == ENEMY_KIND::Boss_Ex2 ||
        data->kinds.at(0) == ENEMY_KIND::Boss_Ex3)
    {

        is_boss_battle = true;

        if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::SIDE)
        {
            GameSystem::Instance().ChangeView();
        }

    }



    // �������I����������f�[�^���폜����
    EraseSpawnData();
}

bool EnemyManager::GetStartBossBGM()
{
    if(!PlayingBossBGM)
    {
        if(StartBossBGM)
        {
            PlayingBossBGM = true;
            return true;
        }

    }
    return false;
}


void EnemyManager::MakeDummyEnemy()
{
    ID3D11Device* device = SceneManager::Instance().GetDevice();

    Enemy* enemy_dummy[] =
    {
        new EnemyDummy(device,EnemyModelFilePath::Blue_Utubo),
        new EnemyDummy(device,EnemyModelFilePath::Red_Utubo),
        new EnemyDummy(device,EnemyModelFilePath::Blue_Ankou),
        new EnemyDummy(device,EnemyModelFilePath::Red_Ankou),
        new EnemyDummy(device,EnemyModelFilePath::Blue_Shark),
        new EnemyDummy(device,EnemyModelFilePath::Red_Shark),
        new EnemyDummy(device,EnemyModelFilePath::Blue_Hitode),
        new EnemyDummy(device,EnemyModelFilePath::Red_Hitode),
        new EnemyDummy(device,EnemyModelFilePath::Boss,"./resources/Shaders/Boss_PBR_PS.cso"),
        new EnemyDummy(device,"./resources/Models/Characters/Boss/Laser.nk", "./resources/Shaders/Boss_PBR_PS.cso"),
        new EnemyDummy(device,EnemyModelFilePath::Boss2,"./resources/Shaders/Boss_PBR_PS.cso")
    };


    for(auto& enemy : enemy_dummy)
    {
        enemy->t.Position = { 0,150,150 };

        dummies.emplace_back(enemy);
    }
}


//****************************************************************
//
//  �f�o�b�O���j���[
// 
//****************************************************************

void EnemyManager::GuiMenu()
{
#ifdef _DEBUG

    ID3D11Device* device = SceneManager::Instance().GetDevice();


    ImGui::Begin("EnemyManager");
    if (ImGui::Button("SpawnBoss_0"))
    {
        Enemy* e = new Boss_0(device);
        RegisterEnemy(e);
    }
    /*if (ImGui::Button("TestEnemy"))
    {
        Enemy* e = new Test_Enemy(device, BaseObject::Type::RED);
        if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
        {
            e->t.Position.x = target->x;
        }
        else
        {
            e->t.Position.y = target->y;
        }
        e->Initialize();
        RegisterEnemy(e);
    }
    if (ImGui::Button("EnemyLock_Straight_Target"))
    {
        Enemy* e = new EnemyLock_Straight_Target(device, BaseObject::Type::RED,target);
        if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
        {
            e->t.Position.x = target->x;
        }
        else
        {
            e->t.Position.y = target->y;
        }
        e->Initialize();
        RegisterEnemy(e);
    }
    if (ImGui::Button("EnemyStraight_None_Start_Back"))
    {
        Enemy* e = new EnemyStraight_TargetShot_Start_Back(device, BaseObject::Type::RED, target, 3);
        if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
        {
            e->t.Position.x = target->x;
        }
        else
        {
            e->t.Position.y = target->y;
        }
        e->Initialize();
        RegisterEnemy(e);
    }
    if (ImGui::Button("EnemyStraight_None_Start_Front_Side"))
    {
        Enemy* e = new EnemyStraight_None_Start_Front(device, BaseObject::Type::RED);
        if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
        {
            e->t.Position.x = target->x;
        }
        else
        {
            e->t.Position.y = target->y;
        }
        e->Initialize();
        RegisterEnemy(e);
    }
    if (ImGui::Button("EnemyLock_TargetMultiShot_B"))
    {
        Enemy* e = new EnemyLock_TargetMultiShot_B(device, BaseObject::Type::RED, target);
        if(GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
        {
            e->t.Position.x = target->x;
        }
        else
        {
            e->t.Position.y = target->y;
        }
        e->t.Position.z = 60.0f;
        e->Initialize();
        RegisterEnemy(e);
    }
    if (ImGui::Button("EnemyRotate_AroundShot_E"))
    {
        Enemy* e = new EnemyRotate_AroundShot_E(device, BaseObject::Type::RED);
        if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
        {
            e->t.Position.x = target->x;
        }
        else
        {
            e->t.Position.y = target->y;
        }
        e->Initialize();
        RegisterEnemy(e);
    }
    if (ImGui::Button("EnemyStraight_None_Start_Side"))
    {
        Enemy* e = new EnemyStraight_None_Start_Side(device, BaseObject::Type::RED);
        if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
        {
            e->t.Position.x = target->x;
        }
        else
        {
            e->t.Position.y = target->y;
        }
        e->Initialize();
        RegisterEnemy(e);
    }
    if (ImGui::Button("EnemyStraight_None_Horizon_Top"))
    {
        Enemy* e = new EnemyStraight_None_Horizon_Top(device, BaseObject::Type::RED);
        if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
        {
            e->t.Position.x = target->x;
        }
        else
        {
            e->t.Position.y = target->y;
        }
        e->Initialize();
        RegisterEnemy(e);
    }
    if (ImGui::Button("EnemyStraight_None_Bezier_Top"))
    {
        Enemy* e = new EnemyStraight_None_Bezier_Top(device, BaseObject::Type::RED);
        if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
        {
            e->t.Position.x = target->x;
        }
        else
        {
            e->t.Position.y = target->y;
        }
        e->t.Position.z = 27.0f;

        e->Initialize();
        RegisterEnemy(e);
    }
    if (ImGui::Button("EnemyStraight_None_Bezier_Side"))
    {
        Enemy* e = new EnemyStraight_None_Bezier_Side(device, BaseObject::Type::RED);
        if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
        {
            e->t.Position.x = target->x;
        }
        else
        {
            e->t.Position.y = target->y;
        }
        e->t.Position.z = 27.0f;

        e->Initialize();
        RegisterEnemy(e);
    }*/


    if (ImGui::Button("EnemyMove_Round_TargetShot_Side"))
    {
        Enemy* e = new EnemyMove_Round_TargetShot_Side(device, BaseObject::Type::RED, target);
        e->t.Position.z = 60.0f;
        if(target->y < 0.0f)
        {
            e->t.Position.y = -23.0f;
        }
        else
        {
            e->t.Position.y = 23.0f;
        }
            
        e->Initialize();
        RegisterEnemy(e);
    }

    if (ImGui::Button("Boss_1"))
    {
        Enemy* e = new Boss_1(device,BaseObject::Type::RED,target);
        RegisterEnemy(e);
    }

    if (ImGui::Button("Boss_2"))
    {
        Enemy* e = new Boss_2(device);
        RegisterEnemy(e);
    }
    if (ImGui::Button("Boss_3"))
    {
        Enemy* e = new Boss_3(device);
        RegisterEnemy(e);
    }

    if (ImGui::Button("Boss_4"))
    {
        Enemy* e = new Boss_4(device);
        RegisterEnemy(e);
    }
    if (ImGui::Button("Boss_5"))
    {
        Enemy* e = new Boss_5(device);
        RegisterEnemy(e);
    }
    if (ImGui::Button("Boss_6"))
    {
        Enemy* e = new Boss_6(device);
        RegisterEnemy(e);
    }
    if (ImGui::Button("Boss_7"))
    {
        Enemy* e = new Boss_7(device);
        RegisterEnemy(e);
    }
    if (ImGui::Button("Boss_8"))
    {
        Enemy* e = new Boss_8(device);
        RegisterEnemy(e);
    }



    ImGui::Text("timer %.1f", spawn_timer.NowTime());

    if(ImGui::Button("Add time 10"))
    {
        spawn_timer.Add(10.0f);
    }

    if(stage == STAGE::STAGE1 || stage == STAGE::STAGE2 || stage == STAGE::STAGE3)
    {
        ImGui::Text("NORMAL MODE");
        if(ImGui::Button("skip to boss"))
        {
            const float now_time = spawn_timer.NowTime();
            const float add_time = easy_boss_time - now_time - 7.0f;
            spawn_timer.Add(add_time);

            // �c�V���[�ɂ���
            if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::SIDE)
            {
                GameSystem::Instance().ChangeView();
            }
        }
    }
    else if (/*stage == STAGE::EXTRA1 || stage == STAGE::EXTRA2 || stage == STAGE::EXTRA1*/stage == STAGE::STAGE4 || stage == STAGE::STAGE5)
    {
        ImGui::Text("EXTRA MODE");
        if (ImGui::Button("skip to boss"))
        {
            const float now_time = spawn_timer.NowTime();
            const float add_time = normal_boss_time - now_time - 7.0f;
            spawn_timer.Add(add_time);

            // �c�V���[�ɂ���
            if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::SIDE)
            {
                GameSystem::Instance().ChangeView();
            }
        }
    }

    ImGui::End();
#endif

}


void EnemyManager::RegisterTargetPosition(const DirectX::XMFLOAT3* target_)
{
    target = target_;
}


void EnemyManager::SetSpawnDatas_Stage1()
{

    constexpr BaseObject::Type red  = BaseObject::Type::RED;
    constexpr BaseObject::Type blue = BaseObject::Type::BLUE;

    /*
     *  SpawnData�̍\��
     *
     *  �������`�̕ϐ��͔z�� (s�Ȃ�z��, ss�Ȃ�z��̔z��ɂȂ��Ă���)
     *
     *
     *
     *  - �����t���O(�b)                    spawn_time
     *                                                         = 7.0f
     *  - �G�̎�ޔz��@                    kinds
     *                                                          kinds[0]                 ,      kinds[1]        ...
     *  - �G�̗�                           sizes
     *                                                         = 2                       ,      = 1             ...
     *  - ��������ʒu(y��z)�̒l�z�� (�G�̗ʕ�) pos_widthss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{10.0f}  ,     = 0�Ԗ�{-10.0f}   ...
     *  - �����z��(�G�̗ʕ�)                 typess
     *                                                         = 0�Ԗ�{red},1�Ԗ�{blue}    ,     = 0�Ԗ�{red}      ...
     *  - ���炷z�l�̗ʔz��                  z_shiftss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{-4.0f}  ,     = 0�Ԗ�{3.0f}     ...
     *
     */

    using E_K = ENEMY_KIND;



    spawn_datas = {

        // �X�^�[�g

        // ���V���[�e�B���O

                    // ��������                 // �G�̎��(�z��)                                                 // ��(�z��)                // ��������ʒu�́@:���Ȃ�y :�c�Ȃ�z �̒l(�z��̔z��)           // ����(�z��̔z��)                    // ��������ۂɂ��炷z�l�̗�(�z��̔z��)
        
        {SpawnData{{2.0f},{E_K::Straight_None_Bezier_Side},                                  {1},    {{17.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{2.7f},{E_K::Straight_None_Bezier_Side},                                  {1},    {{22.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{3.4f},{E_K::Straight_TargetShot_Bezier_Side},                            {1},    {{29.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{8.0f},{E_K::Straight_None_Bezier_Side},                                  {1},    {{-17.0f}},                               {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{8.7f},{E_K::Straight_None_Bezier_Side},                                  {1},    {{-22.0f}},                               {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{9.4f},{E_K::Straight_TargetShot_Bezier_Side},                            {1},    {{-29.0f}},                               {{red}}                          ,{{0.0f}}   }},



        {SpawnData{{13.0f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{17.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{13.7f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{22.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{14.4f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{29.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{13.3f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{-17.0f}},                               {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{14.0f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{-22.0f}},                               {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{14.7f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{-29.0f}},                               {{red}}                          ,{{0.0f}}   }},


        {SpawnData{{16.0f},{E_K::Straight_None_Start_Back},                                  {2},    {{5.0f,-5.0f}},                           {{red,red}}                          ,{{0.0f,0.0f}}   }},
        {SpawnData{{16.7f},{E_K::Straight_None_Start_Back},                                  {2},    {{10.0f,-10.0f}},                         {{red,red}}                          ,{{0.0f,0.0f}}   }},


        {SpawnData{{18.0f},{E_K::Lock_Straight_Target},                                     {2},    {{-14.0f,-25.0f}},                         {{red,red}}                          ,{{0.0f,0.0f}}   }},

        {SpawnData{{19.8f},{E_K::Lock_MultiShot_E},                                         {1},    {{5.0f}},                                  {{red}}                          ,{{0.0f}}   }},


        {SpawnData{{20.0f},{E_K::Straight_None_Start_Front},                                {4},    {{-7.0f,7.0f,-20.0f,20.0f}},               {{red,red,blue,blue}}            ,{{-10.0f,-10.0f,5.0f,5.0f}}   }},
        
        {SpawnData{{25.2f},{E_K::Lock_MultiShot_B},                                         {2},    {{27.0f,-27.0f}},                          {{blue,blue}}                      ,{{0.0f,0.0f}}   }},


        {SpawnData{{31.0f},{E_K::Rotate_AroundShot_E},                                      {1},    {{0.0f}},                                  {{blue}}                          ,{{0.0f}}   }},

        {SpawnData{{36.0f},{E_K::Lock_MultiShot_B},                                         {2},    {{14.0f,-14.0f}},                          {{blue,blue}}                      ,{{0.0f,0.0f}}   }},
        {SpawnData{{36.4f},{E_K::Straight_None_Start_Back},                                 {2},    {{17.0f,-17.0f}},                          {{blue,blue}}                      ,{{0.0f,0.0f}}   }},
        

        {SpawnData{{40.0f},{E_K::Lock_Straight_Target},                                     {4},    {{-10.0f,10.0f,-17.0f,17.0f}},             {{blue,blue,red,red}}              ,{{0.0f,0.0f,25.0f,25.0f}}   }},
        {SpawnData{{42.0f},{E_K::Straight_None_Start_Front},                                {3},    {{23.0f,-23.0f,0.0f}},                     {{red,red,red}}                  ,{{-10.0f,-10.0f,-10.0f}}   }},





        ////////////////////////////////
        ////        ��ʐ؂�ւ�      ////
        ////////////////////////////////


        // �c�V���[�e�B���O
#if 0
        {SpawnData{{53.1f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{60.0f}},                                {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{53.3f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{48.5f}},                                {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{53.5f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{56.0f}},                                {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{53.7f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{50.0f}},                                {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{53.9f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{59.0f}},                                {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{54.1f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{49.0f}},                                {{red}}                          ,{{0.0f}}   }},

        {SpawnData{{53.0f},{E_K::Straight_None_Start_Side},                                  {1},    {{-3.0f}},                                {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{53.2f},{E_K::Straight_None_Start_Side},                                  {1},    {{-8.0f}},                                {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{53.4f},{E_K::Straight_None_Start_Side},                                  {1},    {{-13.0f}},                               {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{53.6f},{E_K::Straight_None_Start_Side},                                  {1},    {{-18.0f}},                               {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{53.8f},{E_K::Straight_None_Start_Side},                                  {1},    {{-23.0f}},                               {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{54.0f},{E_K::Straight_None_Start_Side},                                  {1},    {{-28.0f}},                               {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{54.2f},{E_K::Straight_None_Start_Side},                                  {1},    {{-33.0f}},                               {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{54.4f},{E_K::Straight_TargetShot_Start_Side},                            {1},    {{-38.0f}},                               {{red}}                          ,{{10.0f}}   }},


        {SpawnData{{57.1f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{-60.0f}},                               {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{57.3f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{-48.5f}},                               {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{57.5f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{-56.0f}},                               {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{57.7f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{-50.0f}},                               {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{57.9f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{-59.0f}},                               {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{58.1f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{-49.0f}},                               {{blue}}                          ,{{0.0f}}   }},

        {SpawnData{{57.0f},{E_K::Straight_None_Start_Side},                                  {1},    {{3.0f}},                                 {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{57.2f},{E_K::Straight_None_Start_Side},                                  {1},    {{8.0f}},                                 {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{57.4f},{E_K::Straight_None_Start_Side},                                  {1},    {{13.0f}},                                {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{57.6f},{E_K::Straight_None_Start_Side},                                  {1},    {{18.0f}},                                {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{57.8f},{E_K::Straight_None_Start_Side},                                  {1},    {{23.0f}},                                {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{58.0f},{E_K::Straight_None_Start_Side},                                  {1},    {{28.0f}},                                {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{58.2f},{E_K::Straight_None_Start_Side},                                  {1},    {{33.0f}},                                {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{58.4f},{E_K::Straight_TargetShot_Start_Side},                            {1},    {{38.0f}},                                {{blue}}                          ,{{10.0f}}   }},
#endif
        {SpawnData{{53.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{55.0f}},                                 {{red}}                          ,{{-55.0f}}   }},
        {SpawnData{{53.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{55.0f,55.0f}},                           {{red,red}}                      ,{{-50.0f,-60.0f}}   }},

        {SpawnData{{55.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{-55.0f}},                                {{red}}                          ,{{-70.0f}}   }},
        {SpawnData{{55.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{-55.0f,-55.0f}},                         {{red,red}}                      ,{{-65.0f,-75.0f}}   }},

        {SpawnData{{57.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{55.0f}},                                 {{red}}                          ,{{-45.0f}}   }},
        {SpawnData{{57.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{55.0f,55.0f}},                           {{red,red}}                      ,{{-40.0f,-50.0f}}   }},

        {SpawnData{{59.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{-55.0f}},                                {{red}}                          ,{{-80.0f}}   }},
        {SpawnData{{59.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{-55.0f,-55.0f}},                         {{red,red}}                      ,{{-75.0f,-85.0f}}   }},

        {SpawnData{{61.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{55.0f}},                                 {{red}}                          ,{{-65.0f}}   }},
        {SpawnData{{61.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{55.0f,55.0f}},                           {{red,red}}                      ,{{-60.0f,-70.0f}}   }},

        {SpawnData{{63.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{-55.0f}},                                {{blue}}                          ,{{-55.0f}}   }},
        {SpawnData{{63.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{-55.0f,-55.0f}},                         {{blue,blue}}                      ,{{-50.0f,-60.0f}}   }},

        {SpawnData{{65.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{55.0f}},                                 {{blue}}                          ,{{-45.0f}}   }},
        {SpawnData{{65.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{55.0f,55.0f}},                           {{blue,blue}}                      ,{{-40.0f,-50.0f}}   }},

        {SpawnData{{67.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{-55.0f}},                                {{blue}}                          ,{{-80.0f}}   }},
        {SpawnData{{67.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{-55.0f,-55.0f}},                         {{blue,blue}}                      ,{{-75.0f,-85.0f}}   }},

        {SpawnData{{69.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{-55.0f}},                                {{blue}}                          ,{{-70.0f}}   }},
        {SpawnData{{69.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{-55.0f,-55.0f}},                         {{blue,blue}}                      ,{{-65.0f,-75.0f}}   }},
        
        //{SpawnData{{71.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{55.0f}},                                 {{blue}}                          ,{{-55.0f}}   }},
        //{SpawnData{{71.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{55.0f,55.0f}},                           {{blue,blue}}                      ,{{-50.0f,-60.0f}}   }},


        {SpawnData{{50.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{-12.0f}},                                {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{50.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{-7.0f,-17.0f}},                          {{red,red}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{52.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{21.0f}},                                 {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{52.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{16.0f,26.0f}},                           {{red,red}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{54.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{-7.0f}},                                 {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{54.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{-2.0f,-12.0f}},                          {{red,red}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{56.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{12.0f}},                                 {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{56.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{7.0f,17.0f}},                            {{red,red}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{58.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{-12.0f}},                                {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{58.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{-7.0f,-17.0f}},                          {{red,red}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{60.0f},{E_K::Straight_Shot_Start_Back},                                 {1},    {{21.0f}},                                 {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{60.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{16.0f,26.0f}},                           {{blue,blue}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{62.0f},{E_K::Straight_Shot_Start_Back},                                 {1},    {{-7.0f}},                                 {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{62.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{-2.0f,-12.0f}},                          {{blue,blue}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{64.0f},{E_K::Straight_Shot_Start_Back},                                 {1},    {{12.0f}},                                 {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{64.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{7.0f,17.0f}},                            {{blue,blue}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{66.0f},{E_K::Straight_Shot_Start_Back},                                 {1},    {{-12.0f}},                                {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{66.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{-7.0f,-17.0f}},                          {{blue,blue}}                      ,{{10.0f,10.0f}}   }},
        
        //{SpawnData{{68.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{21.0f}},                                 {{blue}}                          ,{{10.0f}}   }},
        //{SpawnData{{68.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{16.0f,26.0f}},                           {{blue,blue}}                      ,{{10.0f,10.0f}}   }},





        //{SpawnData{{60.0f},{E_K::Lock_MultiShot_E},                                         {2},    {{-20.0f,20.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        //{SpawnData{{61.0f},{E_K::Lock_Straight_Target},                                     {4},    {{-15.0f,15.0f,-31.0f,31.0f}},             {{blue,blue,blue,blue}}          ,{{0.0f,0.0f,25.0f,25.0f}}   }},
        



#if 0
        //{SpawnData{{67.0f},{E_K::Lock_MultiShot_B},                                          {1},    {{-35.0f}},                               {{blue}}                         ,{{10.0f}}   }},
        {SpawnData{{67.3f},{E_K::Lock_MultiShot_B},                                          {1},    {{-15.0f}},                               {{blue}}                         ,{{2.0f}}   }},
        {SpawnData{{68.0f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{60.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{68.1f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{65.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{68.2f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{44.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{68.3f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{58.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{68.4f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{53.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{68.5f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{60.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{68.6f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{43.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{68.7f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{67.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{68.8f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{53.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{68.9f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{61.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{69.0f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{65.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{69.1f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{48.0f}},                                {{blue}}                         ,{{0.0f}}   }},
#endif
            


        {SpawnData{{72.0f},{E_K::Rotate_AroundShot_E},                                       {1},    {{0.0f}},                                 {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{73.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   }},
        {SpawnData{{73.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   }},
        {SpawnData{{73.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   }},
        {SpawnData{{74.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   }},

        {SpawnData{{73.5f},{E_K::Lock_MultiShot_B},                                          {1},    {{10.0f}},                                {{blue}}                         ,{{10.0f}}   }},
        {SpawnData{{74.6f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},
        {SpawnData{{75.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},
        {SpawnData{{75.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},
        {SpawnData{{75.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},

        {SpawnData{{74.5f},{E_K::Lock_MultiShot_B},                                          {1},    {{-10.0f}},                               {{blue}}                         ,{{10.0f}}   }},
        {SpawnData{{76.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-95.0f}}   }},
        {SpawnData{{76.6f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-95.0f}}   }},
        {SpawnData{{77.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-95.0f}}   }},
        {SpawnData{{77.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-95.0f}}   }},

        {SpawnData{{77.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},
        {SpawnData{{78.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},
        {SpawnData{{78.6f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},
        {SpawnData{{79.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},

        {SpawnData{{79.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   }},
        {SpawnData{{79.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   }},
        {SpawnData{{80.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   }},
        {SpawnData{{80.6f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   }},


        { SpawnData{{easy_boss_time},{E_K::Boss1},                                                     {1},    {{0.0f}},                                 {{red}}                          ,{{0.0f}}   } },


        // 60�b�o��
        // 90�b�o��
        // 120�b�o��

    };
}


void EnemyManager::SetSpawnDatas_Stage2()
{

    constexpr BaseObject::Type red = BaseObject::Type::RED;
    constexpr BaseObject::Type blue = BaseObject::Type::BLUE;

    /*
     *  SpawnData�̍\��
     *
     *  �������`�̕ϐ��͔z�� (s�Ȃ�z��, ss�Ȃ�z��̔z��ɂȂ��Ă���)
     *
     *
     *
     *  - �����t���O(�b)                    spawn_time
     *                                                         = 7.0f
     *  - �G�̎�ޔz��@                    kinds
     *                                                          kinds[0]                 ,      kinds[1]        ...
     *  - �G�̗�                           sizes
     *                                                         = 2                       ,      = 1             ...
     *  - ��������ʒu(y��z)�̒l�z�� (�G�̗ʕ�) pos_widthss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{10.0f}  ,     = 0�Ԗ�{-10.0f}   ...
     *  - �����z��(�G�̗ʕ�)                 typess
     *                                                         = 0�Ԗ�{red},1�Ԗ�{blue}    ,     = 0�Ԗ�{red}      ...
     *  - ���炷z�l�̗ʔz��                  z_shiftss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{-4.0f}  ,     = 0�Ԗ�{3.0f}     ...
     *
     */

    using E_K = ENEMY_KIND;



    spawn_datas = {

        // �X�^�[�g (�v���C���[)

        // ���V���[�e�B���O

                    // ��������                 // �G�̎��(�z��)                                                 // ��(�z��)                // ��������ʒu�́@:���Ȃ�y :�c�Ȃ�z �̒l(�z��̔z��)           // ����(�z��̔z��)                    // ��������ۂɂ��炷z�l�̗�(�z��̔z��)


        // 4�C�̏W�c
        {SpawnData{{ 2.0f},{E_K::Straight_None_Start_Back},                                {2},    {{17.0f,12.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{ 2.4f},{E_K::Straight_None_Start_Back},                                {2},    {{17.0f,12.0f}},                                {{red,red}}                         ,{{2.0f,2.0f}}   }},
        
        // + 1
        {SpawnData{{ 3.5f},{E_K::Straight_TargetShot_Start_Back},                          {1},    {{0.0f}},                                       {{red}}                             ,{{0.0f}}   }},


        //  �ҋ@ : �߂�����܂Ŏ��Ԃ�u��


        // 4�C�̏W�c
        {SpawnData{{ 6.0f},{E_K::Straight_None_Start_Back},                                {2},    {{-17.0f,-12.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{ 6.4f},{E_K::Straight_None_Start_Back},                                {2},    {{-17.0f,-12.0f}},                                {{red,red}}                         ,{{2.0f,2.0f}}   }},

        // + 1
        {SpawnData{{ 7.5f},{E_K::Straight_TargetShot_Start_Back},                          {1},    {{0.0f}},                                       {{red}}                             ,{{0.0f}}   }},


        //  �ҋ@ : �߂�����܂Ŏ��Ԃ�u��



        // �R�C�̏W�c���R�@" ���@" ���̌`
        {SpawnData{{10.0f},{E_K::Straight_None_Start_Back},                                {1},    {{0.0f - 0.0f}},                          {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{10.3f},{E_K::Straight_Shot_Start_Back},                                {2},    {{7.0f - 0.0f,-7.0f - 0.0f}},            {{blue,blue}}                      ,{{0.0f,0.0f}}   }},

        //  �ҋ@ : ��ʔ����s������

        {SpawnData{{14.0f},{E_K::Straight_None_Start_Back},                                {1},    {{0.0f - 8.0f}},                          {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{14.3f},{E_K::Straight_Shot_Start_Back},                                {2},    {{7.0f - 8.0f,-7.0f - 8.0f}},            {{blue,blue}}                      ,{{0.0f,0.0f}}   }},

        //  �ҋ@ :  ��ʔ����s������

        {SpawnData{{18.0f},{E_K::Straight_None_Start_Back},                                {1},    {{0.0f + 8.0f}},                          {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{18.3f},{E_K::Straight_Shot_Start_Back},                                {2},    {{7.0f + 8.0f,-7.0f + 8.0f}},            {{blue,blue}}                      ,{{0.0f,0.0f}}   }},


        // �R�C�̎��V����W�c��3��
        {SpawnData{{26.0f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{26.3f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{26.6f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{red}}                         ,{{0.0f}}   }},

        {SpawnData{{28.0f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{-23.0f}},                                {{blue}}                        ,{{0.0f}}   }},
        {SpawnData{{28.3f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{-23.0f}},                                {{blue}}                        ,{{0.0f}}   }},
        {SpawnData{{28.6f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{-23.0f}},                                {{blue}}                        ,{{0.0f}}   }},

        {SpawnData{{29.0f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{5.0f}}   }},
        {SpawnData{{29.3f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{5.0f}}   }},
        {SpawnData{{29.6f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{5.0f}}   }},

        {SpawnData{{36.5f},{E_K::Rotate_AroundShot_E},                                     {4},    {{19.0f,11.0f,-11.0f,-19.0f}},             {{red,red,red,red}}              ,{{0.0f,0.0f,0.0f,0.0f}}   }},


        ////////////////////////////////
        ////        ��ʐ؂�ւ�      ////
        ////////////////////////////////


        // �c�V���[�e�B���O
        {SpawnData{{50.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{30.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{53.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-30.0f}},                                {{blue}}                         ,{{0.0f}}   }},


        {SpawnData{{56.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{25.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{58.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-25.0f}},                                {{blue}}                         ,{{0.0f}}   }},


        {SpawnData{{61.1f},{E_K::Straight_None_Start_Back},                                {1},     {{-7.0f}},                                 {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{61.4f},{E_K::Straight_None_Start_Back},                                {1},     {{-7.0f}},                                 {{red}}                          ,{{10.0f}}   }},
        //{SpawnData{{60.3f},{E_K::Straight_None_Start_Back},                                {2},     {{-7.0f,-12.0f}},                          {{red,red}}                      ,{{10.0f,10.0f}}   }},
        {SpawnData{{63.0f},{E_K::Straight_None_Start_Back},                                {1},     {{7.0f}},                                  {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{63.3f},{E_K::Straight_None_Start_Back},                                {1},     {{7.0f}},                                  {{red}}                          ,{{10.0f}}   }},

        {SpawnData{{65.0f},{E_K::Straight_None_Start_Back},                                {2},     {{-7.0f,-12.0f}},                          {{red,red}}                      ,{{10.0f,10.0f}}   }},
        {SpawnData{{65.3f},{E_K::Straight_None_Start_Back},                                {2},     {{7.0f,12.0f}},                            {{red,red}}                      ,{{10.0f,10.0f}}   }},
        {SpawnData{{68.0f},{E_K::Straight_None_Start_Back},                                {4},     {{-23.0f,-18.0f,18.0f,23.0f}},             {{red,red,red,red}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{68.3f},{E_K::Straight_None_Start_Back},                                {4},     {{-23.0f,-18.0f,18.0f,23.0f}},             {{red,red,red,red}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},


        {SpawnData{{61.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{25.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{63.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-25.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{66.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{5.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{66.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-5.0f}},                                {{blue}}                         ,{{0.0f}}   }},



        {SpawnData{{69.0f},{E_K::Lock_MultiShot_B},                                        {1},    {{15.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{73.0f},{E_K::Lock_MultiShot_B},                                        {1},    {{-15.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{69.2f},{E_K::Translate_Shot_B_Top},                                     {1},    {{22.0f}},                                 {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{70.2f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-22.0f}},                                {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{71.2f},{E_K::Translate_Shot_B_Top},                                     {1},    {{22.0f}},                                 {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{72.2f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-22.0f}},                                {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{73.2f},{E_K::Translate_Shot_B_Top},                                     {1},    {{22.0f}},                                 {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{74.2f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-22.0f}},                                {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{75.2f},{E_K::Translate_Shot_B_Top},                                     {1},    {{22.0f}},                                 {{red}}                         ,{{0.0f}}   }},
        

        { SpawnData{{easy_boss_time - 4.0f},{E_K::Boss2},                                                     {1},    {{0.0f}},                                 {{blue}}                          ,{{0.0f}}   } },

        // 60�b�o��
        // 90�b�o��
        // 120�b�o��

    };
}


void EnemyManager::SetSpawnDatas_Stage3()
{

    constexpr BaseObject::Type red = BaseObject::Type::RED;
    constexpr BaseObject::Type blue = BaseObject::Type::BLUE;

    /*
     *  SpawnData�̍\��
     *
     *  �������`�̕ϐ��͔z�� (s�Ȃ�z��, ss�Ȃ�z��̔z��ɂȂ��Ă���)
     *
     *
     *
     *  - �����t���O(�b)                    spawn_time
     *                                                         = 7.0f
     *  - �G�̎�ޔz��@                    kinds
     *                                                          kinds[0]                 ,      kinds[1]        ...
     *  - �G�̗�                           sizes
     *                                                         = 2                       ,      = 1             ...
     *  - ��������ʒu(y��z)�̒l�z�� (�G�̗ʕ�) pos_widthss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{10.0f}  ,     = 0�Ԗ�{-10.0f}   ...
     *  - �����z��(�G�̗ʕ�)                 typess
     *                                                         = 0�Ԗ�{red},1�Ԗ�{blue}    ,     = 0�Ԗ�{red}      ...
     *  - ���炷z�l�̗ʔz��                  z_shiftss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{-4.0f}  ,     = 0�Ԗ�{3.0f}     ...
     *
     */

    using E_K = ENEMY_KIND;



    spawn_datas = {

        // �X�^�[�g (�v���C���[)

        // ���V���[�e�B���O

                    // ��������                 // �G�̎��(�z��)                                                 // ��(�z��)                // ��������ʒu�́@:���Ȃ�y :�c�Ȃ�z �̒l(�z��̔z��)           // ����(�z��̔z��)                    // ��������ۂɂ��炷z�l�̗�(�z��̔z��)

        {SpawnData{{ 1.5f},{E_K::Lock_TargetSingleShot},                                    {2},    {{15.0f,-15.0f}},                          {{red,red}}                    ,{{0.0f,0.0f}}   }},



        {SpawnData{{ 8.5f},{E_K::Rotate_AroundShot_E},                                      {2},    {{18.0f,-18.0f}},                          {{red,red}}                      ,{{0.0f,0.0f}}   }},

        {SpawnData{{ 17.0f},{E_K::Lock_TargetSingleShot},                                    {2},    {{15.0f,-15.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},

        {SpawnData{{ 21.0f},{E_K::Straight_Shot_Start_Side},                                 {4},    {{19.0f,10.0f,-10.0f,-19.0f}},               {{blue,blue,blue,blue}}          ,{{0.0f,0.0f,0.0f,0.0f}}   }},
        {SpawnData{{ 21.5f},{E_K::Straight_Shot_Start_Side},                                 {4},    {{25.0f,2.6f,-2.6f,-25.0f}},               {{blue,blue,blue,blue}}          ,{{0.0f,0.0f,0.0f,0.0f}}   }},


        //{SpawnData{{ 6.0f},{E_K::Lock_TargetSingleShot},                                    {2},    {{15.0f,-15.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        //{SpawnData{{ 9.0f},{E_K::Lock_TargetSingleShot},                                    {2},    {{23.0f,-23.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        {SpawnData{{24.2f},{E_K::Lock_MultiShot_B},                                         {2},    {{15.0f,-15.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        {SpawnData{{26.2f},{E_K::Lock_MultiShot_B},                                         {2},    {{23.0f,-23.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},


        {SpawnData{{29.5f},{E_K::Rotate_AroundShot_E},                                      {2},    {{8.0f,-8.0f}},                          {{blue,blue}}                      ,{{0.0f,0.0f}}   }},



        {SpawnData{{31.0f},{E_K::Straight_None_Start_Back},                                 {2},    {{7.0f,-7.0f}},                            {{red,red}}                      ,{{0.0f,0.0f}}   }},
        {SpawnData{{31.3f},{E_K::Straight_None_Start_Back},                                 {1},    {{0.0f}},                                  {{red}}                          ,{{0.0f}}   }},

        {SpawnData{{33.0f},{E_K::Straight_None_Start_Back},                                 {2},    {{17.0f,3.0f}},                            {{red,red}}                      ,{{0.0f,0.0f}}   }},
        {SpawnData{{33.3f},{E_K::Straight_None_Start_Back},                                 {1},    {{10.0f}},                                 {{red}}                          ,{{0.0f}}   }},

        {SpawnData{{35.0f},{E_K::Straight_None_Start_Back},                                 {2},    {{23.0f,9.0f}},                            {{red,red}}                      ,{{0.0f,0.0f}}   }},
        {SpawnData{{35.3f},{E_K::Straight_None_Start_Back},                                 {1},    {{16.0f}},                                 {{red}}                          ,{{0.0f}}   }},


        {SpawnData{{37.0f},{E_K::Straight_None_Start_Back},                                 {4},    {{20.0f,6.0f,-20.0f,-6.0f}},               {{red,red,red,red}}              ,{{0.0f,0.0f,0.0f,0.0f}}   }},
        {SpawnData{{37.3f},{E_K::Straight_Shot_Start_Back},                                 {2},    {{13.0f,-13.0f}},                          {{red,red}}                      ,{{0.0f,0.0f}}   }},

        {SpawnData{{39.0f},{E_K::Straight_Shot_Start_Back},                                 {2},    {{-10.0f,4.0f}},                           {{red,red}}                      ,{{0.0f,0.0f}}   }},
        {SpawnData{{39.3f},{E_K::Straight_TargetShot_Start_Back},                           {1},    {{-3.0f}},                                 {{red}}                          ,{{0.0f}}   }},

        {SpawnData{{41.0f},{E_K::Straight_TargetShot_Start_Back},                           {2},    {{14.0f,-14.0f}},                          {{red,red}}                      ,{{0.0f,0.0f}}   }},
        {SpawnData{{41.3f},{E_K::Straight_Shot_Start_Back},                                 {2},    {{7.0f,-7.0f}},                            {{red,red}}                      ,{{0.0f,0.0f}}   }},
        {SpawnData{{41.4f},{E_K::Straight_None_Start_Back},                                 {1},    {{0.0f}},                                  {{red}}                          ,{{0.0f}}   }},

        

        ////////////////////////////////
        ////        ��ʐ؂�ւ�      ////
        ////////////////////////////////


        // �c�V���[�e�B���O

#if 0
        {SpawnData{{52.0f},{E_K::Lock_MultiShot_B},                                         {2},    {{15.0f,-15.0f}},                          {{red,red}}                          ,{{0.0f,0.0f}}   }},



        {SpawnData{{56.0f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{56.3f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{56.6f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{59.0f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{59.3f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{59.6f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{63.0f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{63.3f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{63.6f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},

        {SpawnData{{56.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{37.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{59.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-37.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{62.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{37.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{65.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-37.0f}},                                {{blue}}                         ,{{0.0f}}   }},


        {SpawnData{{67.0f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{67.3f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{67.6f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{69.0f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{69.3f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{69.6f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{71.0f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{71.3f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{71.6f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{73.0f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{73.3f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{73.6f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{75.0f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{75.3f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{75.6f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{77.0f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{77.3f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{77.6f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},



        {SpawnData{{69.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{37.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{72.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-37.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{76.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{37.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{79.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-37.0f}},                                {{blue}}                         ,{{0.0f}}   }},

#endif
            
        {SpawnData{{52.0f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{30.0f,-40.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.1f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{65.0f,-48.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.2f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{44.0f,-60.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.3f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{58.0f,-50.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.4f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{65.0f,-30.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.5f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{41.0f,-67.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.6f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{53.0f,-49.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.7f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{47.0f,-61.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.8f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{35.0f,-43.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.9f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{65.0f,-57.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{53.0f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{30.0f,-45.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{53.1f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{48.0f,-22.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},



        {SpawnData{{58.5f},{E_K::Straight_None_Start_Back},                                  {3},    {{28.0f,33.0f,38.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{59.5f},{E_K::Straight_None_Start_Back},                                  {3},    {{-28.0f,-33.0f,-38.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{60.5f},{E_K::Straight_None_Start_Back},                                  {3},    {{-28.0f,-33.0f,-38.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{61.5f},{E_K::Straight_None_Start_Back},                                  {3},    {{-28.0f,-33.0f,-38.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{57.0f},{E_K::Straight_None_Start_Front},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{blue,blue,blue,blue}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{58.0f},{E_K::Straight_None_Start_Front},                                {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{blue,blue,blue,blue}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{59.0f},{E_K::Straight_None_Start_Front},                                {4},    {{2.0f,7.0f,12.0f,17.0f}},                 {{blue,blue,blue,blue}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{60.0f},{E_K::Straight_None_Start_Front},                                {4},    {{-7.0f,-12.0f,-17.0f,-23.0f}},            {{blue,blue,blue,blue}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{61.0f},{E_K::Straight_None_Start_Front},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{blue,blue,blue,blue}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{62.0f},{E_K::Straight_None_Start_Front},                                {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{blue,blue,blue,blue}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{63.0f},{E_K::Straight_None_Start_Front},                                {4},    {{2.0f,7.0f,12.0f,17.0f}},                 {{blue,blue,blue,blue}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},


        //{SpawnData{{62.8f},{E_K::Lock_Straight_Target},                                     {2},    {{-23.0f,23.0f}},                          {{red,red}}                      ,{{0.0f,0.0f}}   }},
        //{SpawnData{{63.8f},{E_K::Lock_Straight_Target},                                     {2},    {{-10.0f,10.0f}},                          {{red,red}}                      ,{{0.0f,0.0f}}   }},





        {SpawnData{{66.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{43.0f}},                                 {{red}}                         ,{{0.0f}}   } },
        { SpawnData{{67.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-43.0f}},                                {{red}}                         ,{{0.0f}}   } },
        { SpawnData{{69.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{43.0f}},                                 {{red}}                         ,{{0.0f}}   } },
        { SpawnData{{70.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-43.0f}},                                {{red}}                         ,{{0.0f}}   }},

        {SpawnData{{65.0f},{E_K::Straight_None_Start_Back},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{red,red,red,red}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{66.5f},{E_K::Straight_None_Start_Back},                                {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{red,red,red,red}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{68.0f},{E_K::Straight_None_Start_Back},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{red,red,red,red}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{69.5f},{E_K::Straight_None_Start_Back},                                {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{red,red,red,red}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},





        {SpawnData{{72.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{43.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{73.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-43.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{75.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{43.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{76.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-43.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{71.0f},{E_K::Straight_None_Start_Back},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   } },
        { SpawnData{{72.5f},{E_K::Straight_None_Start_Back},                                {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   } },
        { SpawnData{{74.0f},{E_K::Straight_None_Start_Back},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   } },
        { SpawnData{{75.5f},{E_K::Straight_None_Start_Back},                                {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},




        {SpawnData{{78.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-13.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{79.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{13.0f}},                                 {{blue}}                         ,{{0.0f}}   }},


        { SpawnData{{easy_boss_time},{E_K::Boss3},                                                     {1},    {{0.0f}},                                 {{blue}}                          ,{{0.0f}}   } },

    };
}

#if 0

void EnemyManager::SetSpawnDatas_Stage4()
{
    constexpr BaseObject::Type red = BaseObject::Type::RED;
    constexpr BaseObject::Type blue = BaseObject::Type::BLUE;

    /*
     *  SpawnData�̍\��
     *
     *  �������`�̕ϐ��͔z�� (s�Ȃ�z��, ss�Ȃ�z��̔z��ɂȂ��Ă���)
     *
     *
     *
     *  - �����t���O(�b)                    spawn_time
     *                                                         = 7.0f
     *  - �G�̎�ޔz��@                    kinds
     *                                                          kinds[0]                 ,      kinds[1]        ...
     *  - �G�̗�                           sizes
     *                                                         = 2                       ,      = 1             ...
     *  - ��������ʒu(y��z)�̒l�z�� (�G�̗ʕ�) pos_widthss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{10.0f}  ,     = 0�Ԗ�{-10.0f}   ...
     *  - �����z��(�G�̗ʕ�)                 typess
     *                                                         = 0�Ԗ�{red},1�Ԗ�{blue}    ,     = 0�Ԗ�{red}      ...
     *  - ���炷z�l�̗ʔz��                  z_shiftss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{-4.0f}  ,     = 0�Ԗ�{3.0f}     ...
     *
     */

    using E_K = ENEMY_KIND;



    spawn_datas = {

        // �X�^�[�g (�v���C���[)

        // ���V���[�e�B���O

                    // ��������                 // �G�̎��(�z��)                                                 // ��(�z��)                // ��������ʒu�́@:���Ȃ�y :�c�Ȃ�z �̒l(�z��̔z��)           // ����(�z��̔z��)                    // ��������ۂɂ��炷z�l�̗�(�z��̔z��)

        {SpawnData{{ 2.0f},{E_K::Lock_TargetSingleShot},                                   {2},    {{13.0f,-13.0f}},                          {{blue,blue}}                      ,{{0.0f,0.0f}}   }},


        {SpawnData{{ 3.0f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{ 3.3f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{ 3.6f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{0.0f}}   }},


        {SpawnData{{ 5.0f},{E_K::Rotate_AroundShot_E},                                     {1},    {{0.0f}},                                 {{blue}}                         ,{{0.0f}}   }},



        {SpawnData{{16.0f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{16.3f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        //{SpawnData{{16.6f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{17.0f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{-23.0f}},                                {{blue}}                        ,{{0.0f}}   }},
        {SpawnData{{17.3f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{-23.0f}},                                {{blue}}                        ,{{0.0f}}   }},
        //{SpawnData{{17.6f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{-23.0f}},                                {{blue}}                        ,{{0.0f}}   }},

        //{SpawnData{{20.0f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{5.0f}}   }},
        //{SpawnData{{20.3f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{5.0f}}   }},
        //{SpawnData{{20.6f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{5.0f}}   }},
        

        {SpawnData{{27.0f},{E_K::Lock_TargetSingleShot},                                   {2},    {{23.0f,-23.0f}},                          {{blue,blue}}                      ,{{0.0f,0.0f}}   }},
        {SpawnData{{27.9f},{E_K::Lock_TargetSingleShot},                                   {2},    {{13.0f,-13.0f}},                          {{blue,blue}}                      ,{{0.0f,0.0f}}   }},


        {SpawnData{{29.0f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{5.0f}}   }},
        {SpawnData{{29.3f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{5.0f}}   }},
        {SpawnData{{29.6f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{23.0f}},                                {{blue}}                         ,{{5.0f}}   }},

        {SpawnData{{30.0f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{-23.0f}},                                {{blue}}                        ,{{0.0f}}   }},
        {SpawnData{{30.3f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{-23.0f}},                                {{blue}}                        ,{{0.0f}}   }},
        {SpawnData{{30.6f},{E_K::Move_Round_TargetShot_Side},                              {1},    {{-23.0f}},                                {{blue}}                        ,{{0.0f}}   }},

        


        ////////////////////////////////
        ////        ��ʐ؂�ւ�      ////
        ////////////////////////////////


        // �c�V���[�e�B���O
        {SpawnData{{52.0f},{E_K::Lock_MultiShot_E},                                         {2},    {{15.0f,-15.0f}},                          {{red,red}}                      ,{{0.0f,0.0f}}   }},


        {SpawnData{{58.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{55.0f}},                                 {{red}}                          ,{{-45.0f}}   }},
        {SpawnData{{58.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{55.0f,55.0f}},                           {{red,red}}                      ,{{-40.0f,-50.0f}}   }},

        {SpawnData{{60.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{-55.0f}},                                {{red}}                          ,{{-75.0f}}   }},
        {SpawnData{{60.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{-55.0f,-55.0f}},                         {{red,red}}                      ,{{-70.0f,-80.0f}}   }},


        {SpawnData{{57.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{-12.0f}},                                {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{57.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{-7.0f,-17.0f}},                          {{red,red}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{59.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{12.0f}},                                 {{red}}                          ,{{10.0f}}   }},
        {SpawnData{{59.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{7.0f,17.0f}},                            {{red,red}}                      ,{{10.0f,10.0f}}   }},




        {SpawnData{{63.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{55.0f}},                                 {{blue}}                          ,{{-55.0f}}   }},
        {SpawnData{{63.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{55.0f,55.0f}},                           {{blue,blue}}                      ,{{-50.0f,-60.0f}}   }},

        {SpawnData{{65.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{-55.0f}},                                {{blue}}                          ,{{-70.0f}}   }},
        {SpawnData{{65.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{-55.0f,-55.0f}},                         {{blue,blue}}                      ,{{-65.0f,-75.0f}}   }},

        {SpawnData{{67.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{55.0f}},                                 {{blue}}                          ,{{-45.0f}}   }},
        {SpawnData{{67.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{55.0f,55.0f}},                           {{blue,blue}}                      ,{{-40.0f,-50.0f}}   }},

        {SpawnData{{69.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{-55.0f}},                                {{blue}}                          ,{{-80.0f}}   }},
        {SpawnData{{69.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{-55.0f,-55.0f}},                         {{blue,blue}}                      ,{{-75.0f,-85.0f}}   }},

        {SpawnData{{71.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{55.0f}},                                 {{blue}}                          ,{{-65.0f}}   }},
        {SpawnData{{71.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{55.0f,55.0f}},                           {{blue,blue}}                      ,{{-60.0f,-70.0f}}   }},

        {SpawnData{{73.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{-55.0f}},                                {{blue}}                          ,{{-55.0f}}   }},
        {SpawnData{{73.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{-55.0f,-55.0f}},                         {{blue,blue}}                      ,{{-50.0f,-60.0f}}   }},

        {SpawnData{{75.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{55.0f}},                                 {{blue}}                          ,{{-45.0f}}   }},
        {SpawnData{{75.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{55.0f,55.0f}},                           {{blue,blue}}                      ,{{-40.0f,-50.0f}}   }},

        {SpawnData{{77.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{-55.0f}},                                {{blue}}                          ,{{-80.0f}}   }},
        {SpawnData{{77.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{-55.0f,-55.0f}},                         {{blue,blue}}                      ,{{-75.0f,-85.0f}}   }},

        {SpawnData{{79.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{-55.0f}},                                {{blue}}                          ,{{-70.0f}}   }},
        {SpawnData{{79.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{-55.0f,-55.0f}},                         {{blue,blue}}                      ,{{-65.0f,-75.0f}}   }},

        {SpawnData{{81.0f},{E_K::Straight_None_Horizon_Top},                                {1},    {{55.0f}},                                 {{blue}}                          ,{{-55.0f}}   }},
        {SpawnData{{81.3f},{E_K::Straight_None_Horizon_Top},                                {2},    {{55.0f,55.0f}},                           {{blue,blue}}                      ,{{-50.0f,-60.0f}}   }},


        {SpawnData{{60.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{-12.0f}},                                {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{60.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{-7.0f,-17.0f}},                          {{blue,blue}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{62.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{21.0f}},                                 {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{62.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{16.0f,26.0f}},                           {{blue,blue}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{64.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{-7.0f}},                                 {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{64.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{-2.0f,-12.0f}},                          {{blue,blue}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{66.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{12.0f}},                                 {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{66.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{7.0f,17.0f}},                            {{blue,blue}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{68.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{-12.0f}},                                {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{68.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{-7.0f,-17.0f}},                          {{blue,blue}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{70.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{21.0f}},                                 {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{70.3f},{E_K::Straight_Shot_Start_Back},                                 {2},    {{16.0f,26.0f}},                           {{blue,blue}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{72.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{-7.0f}},                                 {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{72.3f},{E_K::Straight_Shot_Start_Back},                                 {2},    {{-2.0f,-12.0f}},                          {{blue,blue}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{74.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{12.0f}},                                 {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{74.3f},{E_K::Straight_Shot_Start_Back},                                 {2},    {{7.0f,17.0f}},                            {{blue,blue}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{76.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{-12.0f}},                                {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{76.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{-7.0f,-17.0f}},                          {{blue,blue}}                      ,{{10.0f,10.0f}}   }},

        {SpawnData{{78.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{21.0f}},                                 {{blue}}                          ,{{10.0f}}   }},
        {SpawnData{{78.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{16.0f,26.0f}},                           {{blue,blue}}                      ,{{10.0f,10.0f}}   }},

        //{SpawnData{{64.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{-7.0f}},                                 {{red}}                          ,{{10.0f}}   }},
        //{SpawnData{{64.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{-2.0f,-12.0f}},                          {{red,red}}                      ,{{10.0f,10.0f}}   }},


        

        { SpawnData{{easy_boss_time},{E_K::Boss4},                                                     {1},    {{0.0f}},                                 {{blue}}                          ,{{0.0f}}   } },

    };
}


void EnemyManager::SetSpawnDatas_Stage5()
{

    constexpr BaseObject::Type red = BaseObject::Type::RED;
    constexpr BaseObject::Type blue = BaseObject::Type::BLUE;

    /*
     *  SpawnData�̍\��
     *
     *  �������`�̕ϐ��͔z�� (s�Ȃ�z��, ss�Ȃ�z��̔z��ɂȂ��Ă���)
     *
     *
     *
     *  - �����t���O(�b)                    spawn_time
     *                                                         = 7.0f
     *  - �G�̎�ޔz��@                    kinds
     *                                                          kinds[0]                 ,      kinds[1]        ...
     *  - �G�̗�                           sizes
     *                                                         = 2                       ,      = 1             ...
     *  - ��������ʒu(y��z)�̒l�z�� (�G�̗ʕ�) pos_widthss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{10.0f}  ,     = 0�Ԗ�{-10.0f}   ...
     *  - �����z��(�G�̗ʕ�)                 typess
     *                                                         = 0�Ԗ�{red},1�Ԗ�{blue}    ,     = 0�Ԗ�{red}      ...
     *  - ���炷z�l�̗ʔz��                  z_shiftss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{-4.0f}  ,     = 0�Ԗ�{3.0f}     ...
     *
     */

    using E_K = ENEMY_KIND;



    spawn_datas = {

        // �X�^�[�g (�v���C���[)

        // ���V���[�e�B���O

                    // ��������                 // �G�̎��(�z��)                                                 // ��(�z��)                // ��������ʒu�́@:���Ȃ�y :�c�Ȃ�z �̒l(�z��̔z��)           // ����(�z��̔z��)                    // ��������ۂɂ��炷z�l�̗�(�z��̔z��)

        
        {SpawnData{{2.0f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{17.0f}},                                 {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{2.7f},{E_K::Straight_Shot_Bezier_Side},                                 {1},    {{22.0f}},                                 {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{3.4f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{29.0f}},                                 {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{2.3f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{-17.0f}},                                {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{3.0f},{E_K::Straight_Shot_Bezier_Side},                                 {1},    {{-22.0f}},                                {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{3.7f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{-29.0f}},                                {{red}}                         ,{{0.0f}}   }},

        {SpawnData{{3.6f},{E_K::Rotate_AroundShot_E},                                       {2},    {{15.0f,-15.0f}},                          {{red,red}}                     ,{{0.0f,0.0f}}   }},


        {SpawnData{{9.6f},{E_K::Straight_Shot_Start_Front},                                {4},    {{-12.0f,12.0f,-23.0f,23.0f}},             {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{7.8f},{E_K::Lock_Straight_Target},                                     {2},    {{-10.0f,10.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        {SpawnData{{8.8f},{E_K::Lock_Straight_Target},                                     {2},    {{-23.0f,23.0f}},                          {{red,red}}                      ,{{0.0f,0.0f}}   }},


        //{SpawnData{{12.9f},{E_K::Rotate_AroundShot_E},                                      {2},    {{15.0f,-15.0f}},                                  {{blue,blue}}                         ,{{0.0f,0.0f}}   }},
        //{SpawnData{{9.5f},{E_K::Rotate_AroundShot_E},                                      {1},    {{-15.0f}},                                 {{blue}}                         ,{{0.0f}}   }},


        {SpawnData{{19.0f},{E_K::Lock_MultiShot_B},                                         {2},    {{22.0f,-22.0f}},                          {{red,red}}                      ,{{0.0f,0.0f}}   }},
        {SpawnData{{19.2f},{E_K::Lock_MultiShot_E},                                         {2},    {{15.0f,-15.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        {SpawnData{{24.9f},{E_K::Lock_TargetSingleShot},                                    {2},    {{23.0f,-23.0f}},                          {{blue,blue}}                      ,{{0.0f,0.0f}}   }},



        {SpawnData{{36.0f},{E_K::Straight_Shot_Start_Back,E_K::Straight_None_Start_Back},  {5,5},    {{23.0f,19.0f,15.0f,11.0f,7.0f},{ -7.0f,-11.0f,-15.0f,-19.0f,-23.0f}},            {{blue,blue,blue,blue,blue},{red,red,red,red,red}}              ,{{0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f}}   } },
        {SpawnData{{37.5f},{E_K::Straight_Shot_Start_Back,E_K::Straight_None_Start_Back},  {4,6},    {{13.0f,9.0f,5.0f,1.0f},{ -3.0f,-7.0f,-11.0f,-15.0f,-19.0f,-23.0f}},            {{blue,blue,blue,blue},{red,red,red,red,red,red}}              ,{{0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f}}   } },
        {SpawnData{{39.0f},{E_K::Straight_Shot_Start_Back,E_K::Straight_None_Start_Back},  {6,4},    {{23.0f,19.0f,15.0f,11.0f,7.0f,3.0f}, {-1.0f,-5.0f,-9.0f,-13.0f}},            {{blue,blue,blue,blue,blue,blue},{red,red,red,red}}              ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f}}   } },
        {SpawnData{{40.5f},{E_K::Straight_Shot_Start_Back,E_K::Straight_None_Start_Back},  {4,6},    {{13.0f,9.0f,5.0f,1.0f}, {-3.0f, -7.0f,-11.0f,-15.0f,-19.0f,-23.0f}},            {{blue,blue,blue,blue},{red,red,red,red,red,red}}              ,{{0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f}}   } },
        {SpawnData{{42.0f},{E_K::Straight_Shot_Start_Back,E_K::Straight_None_Start_Back},  {6,4},    {{23.0f,19.0f,15.0f,11.0f,7.0f,3.0f},{-1.0f,-5.0f,-9.0f,-13.0f}},            {{blue,blue,blue,blue,blue,blue},{red,red,red,red}}              ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f}}   } },



        ////////////////////////////////
        ////        ��ʐ؂�ւ�      ////
        ////////////////////////////////


        // �c�V���[�e�B���O
        //{SpawnData{{53.1f},{E_K::Straight_None_Bezier_Top},                                  {1},    {{60.0f}},                                {{blue}}                          ,{{0.0f}}   }},

        {SpawnData{{52.0f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{60.0f,-60.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.1f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{65.0f,-65.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.2f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{44.0f,-44.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.3f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{58.0f,-58.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.4f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{53.0f,-53.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.5f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{60.0f,-60.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.6f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{43.0f,-43.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.7f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{67.0f,-67.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.8f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{53.0f,-53.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{52.9f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{61.0f,-61.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{53.0f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{65.0f,-65.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},
        {SpawnData{{53.1f},{E_K::Straight_None_Bezier_Top},                                  {2},    {{48.0f,-48.0f}},                                {{red,red}}                         ,{{0.0f,0.0f}}   }},



        {SpawnData{{58.5f},{E_K::Straight_None_Start_Back},                                  {3},    {{28.0f,33.0f,38.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{59.5f},{E_K::Straight_None_Start_Back},                                  {3},    {{-28.0f,-33.0f,-38.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{60.5f},{E_K::Straight_None_Start_Back},                                  {3},    {{-28.0f,-33.0f,-38.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{61.5f},{E_K::Straight_None_Start_Back},                                  {3},    {{-28.0f,-33.0f,-38.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{57.0f},{E_K::Straight_None_Start_Front},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{blue,blue,blue,blue}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{58.0f},{E_K::Straight_None_Start_Front},                                {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{blue,blue,blue,blue}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{59.0f},{E_K::Straight_TargetShot_Start_Front},                          {4},    {{2.0f,7.0f,12.0f,17.0f}},                 {{blue,blue,blue,blue}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{60.0f},{E_K::Straight_None_Start_Front},                                {4},    {{-7.0f,-12.0f,-17.0f,-23.0f}},            {{blue,blue,blue,blue}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{61.0f},{E_K::Straight_None_Start_Front},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{blue,blue,blue,blue}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{62.0f},{E_K::Straight_TargetShot_Start_Front},                          {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{blue,blue,blue,blue}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{63.0f},{E_K::Straight_None_Start_Front},                                {4},    {{2.0f,7.0f,12.0f,17.0f}},                 {{blue,blue,blue,blue}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},


        //{SpawnData{{62.8f},{E_K::Lock_Straight_Target},                                     {2},    {{-23.0f,23.0f}},                          {{red,red}}                      ,{{0.0f,0.0f}}   }},
        //{SpawnData{{63.8f},{E_K::Lock_Straight_Target},                                     {2},    {{-10.0f,10.0f}},                          {{red,red}}                      ,{{0.0f,0.0f}}   }},





        {SpawnData{{66.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{43.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{67.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-43.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{69.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{43.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{70.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-43.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{65.0f},{E_K::Straight_None_Start_Back},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{66.5f},{E_K::Straight_None_Start_Back},                                {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{68.0f},{E_K::Straight_None_Start_Back},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{69.5f},{E_K::Straight_None_Start_Back},                                {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},





        {SpawnData{{72.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{43.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{73.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-43.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{75.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{43.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{76.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-43.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{71.0f},{E_K::Straight_None_Start_Back},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{72.5f},{E_K::Straight_None_Start_Back},                                {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{74.0f},{E_K::Straight_None_Start_Back},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{75.5f},{E_K::Straight_None_Start_Back},                                {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},




        {SpawnData{{78.5f},{E_K::Translate_Shot_B_Top},                                     {1},    {{-13.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{79.0f},{E_K::Translate_Shot_B_Top},                                     {1},    {{13.0f}},                                 {{blue}}                         ,{{0.0f}}   }},


        { SpawnData{{easy_boss_time},{E_K::Boss5},                                                     {1},    {{0.0f}},                                 {{blue}}                          ,{{0.0f}}   } },


        // 60�b�o��
        // 90�b�o��
        // 120�b�o��

    };
}

#endif

#if 0
void EnemyManager::SetSpawnDatas_Extra2()
{
    constexpr BaseObject::Type red = BaseObject::Type::RED;
    constexpr BaseObject::Type blue = BaseObject::Type::BLUE;

    /*
     *  SpawnData�̍\��
     *
     *  �������`�̕ϐ��͔z�� (s�Ȃ�z��, ss�Ȃ�z��̔z��ɂȂ��Ă���)
     *
     *
     *
     *  - �����t���O(�b)                    spawn_time
     *                                                         = 7.0f
     *  - �G�̎�ޔz��@                    kinds
     *                                                          kinds[0]                 ,      kinds[1]        ...
     *  - �G�̗�                           sizes
     *                                                         = 2                       ,      = 1             ...
     *  - ��������ʒu(y��z)�̒l�z�� (�G�̗ʕ�) pos_widthss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{10.0f}  ,     = 0�Ԗ�{-10.0f}   ...
     *  - �����z��(�G�̗ʕ�)                 typess
     *                                                         = 0�Ԗ�{red},1�Ԗ�{blue}    ,     = 0�Ԗ�{red}      ...
     *  - ���炷z�l�̗ʔz��                  z_shiftss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{-4.0f}  ,     = 0�Ԗ�{3.0f}     ...
     *
     */

    using E_K = ENEMY_KIND;



    spawn_datas = {

        // �X�^�[�g

        // ���V���[�e�B���O
        // �O�@�`�@60�b�܂�

                    // ��������                 // �G�̎��(�z��)                                                 // ��(�z��)                // ��������ʒu�́@:���Ȃ�y :�c�Ȃ�z �̒l(�z��̔z��)           // ����(�z��̔z��)                    // ��������ۂɂ��炷z�l�̗�(�z��̔z��)

        {SpawnData{{2.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{0.0f}},                                  {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{2.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{7.0f,-7.0f}},                            {{red,red}}                      ,{{0.0f,0.0f}}   }},

        {SpawnData{{3.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{0.0f -  8.0f}},                          {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{3.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{7.0f -  8.0f,-7.0f -  8.0f}},            {{red,red}}                      ,{{0.0f,0.0f}}   }},

        {SpawnData{{4.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{0.0f +  8.0f}},                          {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{4.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{7.0f +  8.0f,-7.0f +  8.0f}},            {{red,red}}                      ,{{0.0f,0.0f}}   }},



        {SpawnData{{9.0f},{E_K::Straight_TargetShot_Start_Back},                           {1},    {{0.0f}},                                  {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{9.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{7.0f,-7.0f}},                            {{red,red}}                      ,{{0.0f,0.0f}}   }},

        {SpawnData{{11.0f},{E_K::Straight_TargetShot_Start_Back},                          {1},    {{0.0f + 15.0f}},                          {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{11.3f},{E_K::Straight_None_Start_Back},                                {2},    {{7.0f + 15.0f,-7.0f + 15.0f}},            {{red,red}}                      ,{{0.0f,0.0f}}   }},

        {SpawnData{{12.0f},{E_K::Straight_TargetShot_Start_Back},                          {1},    {{0.0f - 15.0f}},                          {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{12.3f},{E_K::Straight_None_Start_Back},                                {2},    {{7.0f - 15.0f,-7.0f - 15.0f}},            {{red,red}}                      ,{{0.0f,0.0f}}   }},


        {SpawnData{{18.0f},{E_K::Lock_TargetSingleShot_Hard},                              {2},    {{15.0f,-15.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        {SpawnData{{24.0f},{E_K::Lock_TargetSingleShot_Hard},                              {2},    {{23.0f,-23.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        //{SpawnData{{27.2f},{E_K::Lock_TargetMultiShot_B},                                  {2},    {{15.0f,-15.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        //{SpawnData{{29.2f},{E_K::Lock_TargetMultiShot_B},                                  {2},    {{23.0f,-23.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        {SpawnData{{19.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{14.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{20.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{-14.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{21.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{23.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{22.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{-23.0f}},                                 {{red}}                              ,{{0.0f}}   }},


        {SpawnData{{26.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{13.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{26.5f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{-13.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{27.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{23.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{27.5f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{-23.0f}},                                 {{red}}                              ,{{0.0f}}   }},

        {SpawnData{{28.5f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{13.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{29.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{-13.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{29.5f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{23.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{30.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{-23.0f}},                                 {{red}}                              ,{{0.0f}}   }},

        {SpawnData{{31.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{13.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{31.5f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{-13.0f}},                                 {{red}}                              ,{{0.0f}}   }},

        


        {SpawnData{{40.0f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{40.3f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{40.6f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{40.9f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{41.2f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{41.5f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{41.8f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{42.1f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{45.0f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{45.3f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{45.6f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{45.9f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{46.2f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{46.5f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{46.8f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{47.1f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},


        {SpawnData{{48.0f},{E_K::Straight_None_Start_Back},                                {1},    {{0.0f}},                                  {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{48.3f},{E_K::Straight_None_Start_Back},                                {2},    {{7.0f,-7.0f}},                            {{red,red}}                      ,{{0.0f,0.0f}}   }},

        {SpawnData{{49.0f},{E_K::Straight_None_Start_Back},                                {1},    {{0.0f - 8.0f}},                          {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{49.3f},{E_K::Straight_None_Start_Back},                                {2},    {{7.0f - 8.0f,-7.0f - 8.0f}},            {{red,red}}                      ,{{0.0f,0.0f}}   }},

        {SpawnData{{50.0f},{E_K::Straight_None_Start_Back},                                {1},    {{0.0f + 8.0f}},                          {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{50.3f},{E_K::Straight_None_Start_Back},                                {2},    {{7.0f + 8.0f,-7.0f + 8.0f}},            {{red,red}}                      ,{{0.0f,0.0f}}   }},


        {SpawnData{{54.0f},{E_K::Straight_None_Start_Front,E_K::Straight_None_Start_Back}, {5,5},  {{23.0f,18.0f,13.0f,8.0f,3.0f},{23.0f,18.0f,13.0f,8.0f,3.0f}},                       {{red,red,red,red,red},{red,red,red,red,red}}    ,{{0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f}}}},
        {SpawnData{{55.5f},{E_K::Straight_None_Start_Front,E_K::Straight_None_Start_Back}, {5,5},  {{-23.0f,-18.0f,-13.0f,-8.0f,-3.0f},{-23.0f,-18.0f,-13.0f,-8.0f,-3.0f}},             {{red,red,red,red,red},{red,red,red,red,red}}    ,{{0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f}}}},
        {SpawnData{{57.0f},{E_K::Straight_None_Start_Front,E_K::Straight_None_Start_Back}, {7,7},  {{23.0f,18.0f,13.0f,8.0f,3.0f,-2.0f,-7.0f},{23.0f,18.0f,13.0f,8.0f,3.0f,-2.0f,-7.0f}},         {{red,red,red,red,red,red,red},{red,red,red,red,red,red,red}}    ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f}}}},
        {SpawnData{{58.5f},{E_K::Straight_None_Start_Front,E_K::Straight_None_Start_Back}, {7,7},  {{-23.0f,-18.0f,-13.0f,-8.0f,-3.0f,2.0f,7.0f},{-23.0f,-18.0f,-13.0f,-8.0f,-3.0f,2.0f,7.0f}},   {{red,red,red,red,red,red,red},{red,red,red,red,red,red,red}}    ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f}}}},
        {SpawnData{{59.0f},{E_K::Straight_None_Start_Front,E_K::Straight_None_Start_Back}, {9,9},  {{23.0f,18.0f,13.0f,8.0f,3.0f,-2.0f,-7.0f,-12.0f,-17.0f},{23.0f,18.0f,13.0f,8.0f,3.0f,-2.0f,-7.0f,-12.0f,-17.0f}},     {{red,red,red,red,red,red,red,red,red},{red,red,red,red,red,red,red,red,red}}    ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f}}}},
        {SpawnData{{60.5f},{E_K::Straight_None_Start_Front,E_K::Straight_None_Start_Back}, {9,9},  {{-23.0f,-18.0f,-13.0f,-8.0f,-3.0f,2.0f,7.0f,12.0f,17.0f},{-23.0f,-18.0f,-13.0f,-8.0f,-3.0f,2.0f,7.0f,12.0f,17.0f}},   {{red,red,red,red,red,red,red,red,red},{red,red,red,red,red,red,red,red,red}}    ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f}}}},




        ////////////////////////////////
        ////        ��ʐ؂�ւ�      ////
        ////////////////////////////////

        // 70�b����`
        // �c�V���[�e�B���O
        
        {SpawnData{{76.0f - 2.0f},{E_K::Straight_TargetShot_Start_Back},                          {1},    {{-7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{76.3f - 2.0f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{76.6f - 2.0f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{79.0f - 2.0f},{E_K::Straight_TargetShot_Start_Back},                          {1},    {{7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{79.3f - 2.0f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{79.6f - 2.0f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{83.0f - 2.0f},{E_K::Straight_TargetShot_Start_Back},                          {1},    {{-7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{83.3f - 2.0f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{83.6f - 2.0f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},

        {SpawnData{{76.0f - 2.0f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{37.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{79.5f - 2.0f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{-37.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{82.0f - 2.0f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{37.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{85.5f - 2.0f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{-37.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{88.5f},{E_K::Straight_None_Start_Back},                                {3},    {{28.0f,33.0f,38.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{89.5f},{E_K::Straight_None_Start_Back},                                {3},    {{-28.0f,-33.0f,-38.0f}},                 {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{90.5f},{E_K::Straight_None_Start_Back},                                {3},    {{-28.0f,-33.0f,-38.0f}},                 {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{91.5f},{E_K::Straight_None_Start_Back},                                {3},    {{-28.0f,-33.0f,-38.0f}},                 {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{87.0f},{E_K::Straight_Shot_Start_Front},                               {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{red,red,red,red}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{88.0f},{E_K::Straight_Shot_Start_Front},                               {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{red,red,red,red}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{89.0f},{E_K::Straight_Shot_Start_Front},                               {4},    {{2.0f,7.0f,12.0f,17.0f}},                 {{red,red,red,red}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{90.0f},{E_K::Straight_Shot_Start_Front},                               {4},    {{-7.0f,-12.0f,-17.0f,-23.0f}},            {{red,red,red,red}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{91.0f},{E_K::Straight_Shot_Start_Front},                               {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{red,red,red,red}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{92.0f},{E_K::Straight_Shot_Start_Front},                               {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{red,red,red,red}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{93.0f},{E_K::Straight_Shot_Start_Front},                               {4},    {{2.0f,7.0f,12.0f,17.0f}},                 {{red,red,red,red}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},



        {SpawnData{{96.0f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{96.6f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{99.0f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{99.6f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{103.0f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{103.6f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        //{SpawnData{{106.0f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        //{SpawnData{{106.6f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        //{SpawnData{{109.0f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        //{SpawnData{{107.6f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        //{SpawnData{{117.0f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        //{SpawnData{{108.6f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},




        {SpawnData{{103.8f},{E_K::Lock_TargetSingleShot},                                  {2},    {{25.0f,-25.0f}},                          {{red,red}}                    ,{{0.0f,0.0f}}   }},


        {SpawnData{{109.0f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-19.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{109.3f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-19.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{110.8f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{19.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{111.1f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{19.0f}},                                 {{blue}}                         ,{{0.0f}}   }},



        {SpawnData{{114.0f},{E_K::Lock_TargetSingleShot_Hard},                             {2},    {{15.0f,-15.0f}},                          {{red,red}}                    ,{{0.0f,0.0f}}   }},

        {SpawnData{{115.0f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-19.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{115.3f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-19.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{116.8f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{19.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{117.1f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{19.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{118.6f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-19.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{118.9f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-19.0f}},                                {{blue}}                         ,{{0.0f}}   }},



        { SpawnData{{normal_boss_time},{E_K::Boss_Ex2},                                                    {1},    {{0.0f}},                                 {{red}}                          ,{{0.0f}}   } },

    };
}


void EnemyManager::SetSpawnDatas_Extra3()
{

    constexpr BaseObject::Type red = BaseObject::Type::RED;
    constexpr BaseObject::Type blue = BaseObject::Type::BLUE;

    /*
     *  SpawnData�̍\��
     *
     *  �������`�̕ϐ��͔z�� (s�Ȃ�z��, ss�Ȃ�z��̔z��ɂȂ��Ă���)
     *
     *
     *
     *  - �����t���O(�b)                    spawn_time
     *                                                         = 7.0f
     *  - �G�̎�ޔz��@                    kinds
     *                                                          kinds[0]                 ,      kinds[1]        ...
     *  - �G�̗�                           sizes
     *                                                         = 2                       ,      = 1             ...
     *  - ��������ʒu(y��z)�̒l�z�� (�G�̗ʕ�) pos_widthss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{10.0f}  ,     = 0�Ԗ�{-10.0f}   ...
     *  - �����z��(�G�̗ʕ�)                 typess
     *                                                         = 0�Ԗ�{red},1�Ԗ�{blue}    ,     = 0�Ԗ�{red}      ...
     *  - ���炷z�l�̗ʔz��                  z_shiftss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{-4.0f}  ,     = 0�Ԗ�{3.0f}     ...
     *
     */

    using E_K = ENEMY_KIND;



    spawn_datas = {

        // �X�^�[�g

        // ���V���[�e�B���O
        // �O�@�`�@60�b�܂�

                    // ��������                 // �G�̎��(�z��)                                                 // ��(�z��)                // ��������ʒu�́@:���Ȃ�y :�c�Ȃ�z �̒l(�z��̔z��)           // ����(�z��̔z��)                    // ��������ۂɂ��炷z�l�̗�(�z��̔z��)

        {SpawnData{{2.0f},{E_K::Straight_Shot_Bezier_Side},                                 {1},    {{17.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{2.7f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{22.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{3.4f},{E_K::Straight_TargetShot_Bezier_Side},                           {1},    {{29.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{2.3f},{E_K::Straight_Shot_Bezier_Side},                                 {1},    {{-17.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{3.0f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{-22.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{3.7f},{E_K::Straight_TargetShot_Bezier_Side},                           {1},    {{-29.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{3.2f},{E_K::Lock_TargetSingleShot_Hard},                                    {2},    {{15.0f,-15.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},


        {SpawnData{{7.1f},{E_K::Straight_Shot_Bezier_Side},                                 {1},    {{20.0f}},                                 {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{7.3f},{E_K::Straight_Shot_Bezier_Side},                                 {1},    {{8.5f}},                                  {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{7.5f},{E_K::Straight_TargetShot_Bezier_Side},                           {1},    {{16.0f}},                                 {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{7.7f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{10.0f}},                                 {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{7.9f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{19.0f}},                                 {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{7.1f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{9.0f}},                                  {{blue}}                          ,{{0.0f}}   }},


        {SpawnData{{12.0f},{E_K::Lock_MultiShot_B},                                         {2},    {{15.0f,25.0f}},                           {{blue,blue}}                    ,{{0.0f,0.0f}}   }},

        {SpawnData{{13.1f},{E_K::Straight_Shot_Bezier_Side},                                {1},    {{-20.0f}},                                {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{13.3f},{E_K::Straight_Shot_Bezier_Side},                                {1},    {{-8.5f}},                                 {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{13.5f},{E_K::Straight_TargetShot_Bezier_Side},                          {1},    {{-16.0f}},                                {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{13.7f},{E_K::Straight_None_Bezier_Side},                                {1},    {{-10.0f}},                                {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{13.9f},{E_K::Straight_None_Bezier_Side},                                {1},    {{-19.0f}},                                {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{13.1f},{E_K::Straight_None_Bezier_Side},                                {1},    {{-9.0f}},                                 {{blue}}                          ,{{0.0f}}   }},


        {SpawnData{{13.6f},{E_K::Straight_Shot_Start_Front},                                {4},    {{-12.0f,12.0f,-23.0f,23.0f}},             {{blue,blue,red,red}}            ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{13.8f},{E_K::Lock_Straight_Target},                                     {2},    {{-10.0f,10.0f}},                          {{red,red}}                      ,{{0.0f,0.0f}}   }},
        {SpawnData{{14.8f},{E_K::Lock_Straight_Target},                                     {2},    {{-23.0f,23.0f}},                          {{red,red}}                      ,{{0.0f,0.0f}}   }},


        {SpawnData{{14.9f},{E_K::Rotate_AroundShot_E},                                      {1},    {{0.0f}},                                  {{red}}                          ,{{0.0f}}   }},

        {SpawnData{{22.0f},{E_K::Straight_None_Start_Front},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{23.0f},{E_K::Straight_None_Start_Front},                                {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{24.0f},{E_K::Straight_TargetShot_Start_Front},                          {4},    {{2.0f,7.0f,12.0f,17.0f}},                 {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{25.0f},{E_K::Straight_None_Start_Front},                                {4},    {{-7.0f,-12.0f,-17.0f,-23.0f}},            {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{26.0f},{E_K::Straight_None_Start_Front},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{27.0f},{E_K::Straight_TargetShot_Start_Front},                          {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{28.0f},{E_K::Straight_None_Start_Front},                                {4},    {{2.0f,7.0f,12.0f,17.0f}},                 {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        // ���ꂨ���̂�37.0f�b


        {SpawnData{{33.0f},{E_K::Straight_TargetShot_Bezier_Side},                          {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{35.0f},{E_K::Straight_TargetShot_Bezier_Side},                          {4},    {{-7.0f,-12.0f,-17.0f,-23.0f}},            {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{35.5f},{E_K::Rotate_AroundShot_E},                                      {1},    {{0.0f}},                                  {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{36.0f},{E_K::Lock_TargetSingleShot_Hard},                               {2},    {{15.0f,-15.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        {SpawnData{{39.0f},{E_K::Lock_TargetSingleShot_Hard},                               {2},    {{23.0f,-23.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        {SpawnData{{44.2f},{E_K::Lock_TargetMultiShot_B_Hard},                              {2},    {{15.0f,-15.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        {SpawnData{{46.2f},{E_K::Lock_TargetMultiShot_B_Hard},                              {2},    {{23.0f,-23.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        // ���ꂪ�����̂�60.0f�b



        {SpawnData{{54.0f + 0.8f},{E_K::Straight_None_Start_Back, E_K::Straight_TargetShot_Start_Back},  {10,1},    {{23.0f,19.0f,15.0f,11.0f,7.0f,-7.0f,-11.0f,-15.0f,-19.0f,-23.0f},{0.0f}},            {{blue,blue,blue,blue,blue,blue,blue,blue,blue,blue},{blue}}              ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f}}   }},
        {SpawnData{{55.5f + 0.8f},{E_K::Straight_None_Start_Back, E_K::Straight_TargetShot_Start_Back},  {10,1},    {{13.0f,9.0f,5.0f,1.0f,-3.0f,-7.0f,-11.0f,-15.0f,-19.0f,-23.0f},{21.0f}},            {{blue,blue,blue,blue,blue,blue,blue,blue,blue,blue},{blue}}              ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f}}   }},
        {SpawnData{{57.0f + 0.8f},{E_K::Straight_None_Start_Back, E_K::Straight_TargetShot_Start_Back},  {10,1},    {{23.0f,19.0f,15.0f,11.0f,7.0f,3.0f,-1.0f,-5.0f,-9.0f,-13.0f},{-21.0f}},            {{blue,blue,blue,blue,blue,blue,blue,blue,blue,blue},{blue}}              ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f}}   }},
        {SpawnData{{58.5f + 0.8f},{E_K::Straight_None_Start_Back, E_K::Straight_TargetShot_Start_Back},  {10,1},    {{13.0f,9.0f,5.0f,1.0f,-3.0f,-7.0f,-11.0f,-15.0f,-19.0f,-23.0f},{21.0f}},            {{blue,blue,blue,blue,blue,blue,blue,blue,blue,blue},{blue}}              ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f}}   }},
        {SpawnData{{60.0f + 0.8f},{E_K::Straight_None_Start_Back, E_K::Straight_TargetShot_Start_Back},  {10,1},    {{23.0f,19.0f,15.0f,11.0f,7.0f,3.0f,-1.0f,-5.0f,-9.0f,-13.0f},{-21.0f}},            {{blue,blue,blue,blue,blue,blue,blue,blue,blue,blue},{blue}}              ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f}}   }},
        



        ////////////////////////////////
        ////        ��ʐ؂�ւ�      ////
        ////////////////////////////////

        // 70�b����`
        // �c�V���[�e�B���O

        {SpawnData{{72.0f},{E_K::Rotate_AroundShot_E},                                       {1},    {{0.0f}},                                 {{red}}                          ,{{0.0f}}   }},


        {SpawnData{{73.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   }},
        {SpawnData{{73.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   }},
        {SpawnData{{73.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   }},
        {SpawnData{{74.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   }},


        {SpawnData{{74.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-55.0f}}   }},
        {SpawnData{{74.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-55.0f}}   }},
        {SpawnData{{74.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-55.0f}}   }},
        {SpawnData{{75.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-55.0f}}   }},

        {SpawnData{{75.1f},{E_K::Lock_TargetSingleShot_Hard},                                     {1},    {{-15.0f}},                               {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{75.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   }},
        {SpawnData{{75.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   }},
        {SpawnData{{75.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   }},
        {SpawnData{{76.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   }},

        {SpawnData{{76.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},
        {SpawnData{{76.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},
        {SpawnData{{76.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},
        {SpawnData{{77.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},

        {SpawnData{{77.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-40.0f}}   } },
        {SpawnData{{77.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-40.0f}}   } },
        {SpawnData{{77.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-40.0f}}   } },
        {SpawnData{{78.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-40.0f}}   } },

        {SpawnData{{78.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-90.0f}}   } },
        {SpawnData{{78.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-90.0f}}   } },
        {SpawnData{{78.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-90.0f}}   } },
        {SpawnData{{79.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-90.0f}}   } },

        {SpawnData{{79.1f},{E_K::Lock_TargetSingleShot_Hard},                                     {1},    {{15.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{79.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   } },
        {SpawnData{{79.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   } },
        {SpawnData{{79.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   } },
        {SpawnData{{80.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   } },

        {SpawnData{{80.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-75.0f}}   } },
        {SpawnData{{80.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-75.0f}}   } },
        {SpawnData{{80.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-75.0f}}   } },
        {SpawnData{{81.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-75.0f}}   } },

        {SpawnData{{81.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-60.0f}}   } },
        {SpawnData{{81.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-60.0f}}   } },
        {SpawnData{{81.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-60.0f}}   } },
        {SpawnData{{82.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-60.0f}}   } },

        {SpawnData{{82.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-55.0f}}   } },
        {SpawnData{{82.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-55.0f}}   } },
        {SpawnData{{82.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-55.0f}}   } },
        {SpawnData{{83.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-55.0f}}   } },

        {SpawnData{{83.1f},{E_K::Lock_TargetSingleShot_Hard},                                     {2},    {{30.0f,-30.0f}},                         {{blue,blue}}                    ,{{0.0f,0.0f}}   }},

        {SpawnData{{83.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-90.0f}}   } },
        {SpawnData{{83.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-90.0f}}   } },
        {SpawnData{{83.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-90.0f}}   } },
        {SpawnData{{84.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-90.0f}}   } },

        {SpawnData{{84.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-65.0f}}   } },
        {SpawnData{{84.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-65.0f}}   } },
        {SpawnData{{84.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-65.0f}}   } },
        {SpawnData{{85.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-65.0f}}   } },

        {SpawnData{{72.0f},{E_K::Rotate_AroundShot_E},                                       {1},    {{43.0f}},                                 {{blue}}                          ,{{0.0f}}   }},

        {SpawnData{{85.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   } },
        {SpawnData{{85.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   } },
        {SpawnData{{85.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   } },
        {SpawnData{{86.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   } },



        {SpawnData{{90.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{26.0f,20.0f,14.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{90.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{10.0f,16.0f,22.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{91.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{43.0f,37.0f,31.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   }},

        {SpawnData{{92.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{-43.0f,-37.0f,-31.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   }},

        {SpawnData{{93.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{8.0f,2.0f,-4.0f}},                      {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{94.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{-8.0f,-2.0f,4.0f}},                     {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{95.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{-10.0f,-16.0f,-22.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{96.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{-26.0f,-20.0f,-14.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{97.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{-28.0f,-34.0f,-40.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{98.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{28.0f,34.0f,40.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },


        {SpawnData{{100.1f},{E_K::Straight_None_Start_Back},                                  {3},    {{26.0f,20.0f,14.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{100.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{43.0f,37.0f,31.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   }},
        {SpawnData{{100.2f},{E_K::Straight_None_Start_Back},                                  {3},    {{8.0f,2.0f,-4.0f}},                      {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{100.3f},{E_K::Straight_None_Start_Back},                                  {3},    {{-10.0f,-16.0f,-22.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{100.4f},{E_K::Straight_None_Start_Back},                                  {3},    {{-28.0f,-34.0f,-40.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{100.6f},{E_K::Lock_Straight_Target},                                      {1},    {{-30.0f}},                               {{red}}                           ,{{0.0f}}   } },

        {SpawnData{{103.9f},{E_K::Straight_None_Start_Back},                                  {3},    {{-43.0f,-37.0f,-31.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   }},
        {SpawnData{{104.1f},{E_K::Straight_None_Start_Back},                                  {3},    {{-8.0f,-2.0f,4.0f}},                     {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{104.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{-26.0f,-20.0f,-14.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{104.2f},{E_K::Straight_None_Start_Back},                                  {3},    {{10.0f,16.0f,22.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{104.3f},{E_K::Straight_None_Start_Back},                                  {3},    {{28.0f,34.0f,40.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{106.9f},{E_K::Straight_None_Start_Back},                                  {3},    {{43.0f,37.0f,31.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{106.8f},{E_K::Straight_None_Start_Back},                                  {3},    {{26.0f,20.0f,14.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{107.1f},{E_K::Straight_None_Start_Back},                                  {3},    {{-10.0f,-16.0f,-22.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{107.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{8.0f,2.0f,-4.0f}},                      {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{107.2f},{E_K::Straight_None_Start_Back},                                  {3},    {{-28.0f,-34.0f,-40.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{107.4f},{E_K::Lock_Straight_Target},                                      {1},    {{30.0f}},                                {{red}}                           ,{{0.0f}}   } },

        {SpawnData{{109.7f},{E_K::Straight_None_Start_Back},                                  {3},    {{-43.0f,-37.0f,-31.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   }},
        {SpawnData{{109.8f},{E_K::Straight_None_Start_Back},                                  {3},    {{-26.0f,-20.0f,-14.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{109.9f},{E_K::Straight_None_Start_Back},                                  {3},    {{-8.0f,-2.0f,4.0f}},                     {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{110.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{10.0f,16.0f,22.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{110.1f},{E_K::Straight_None_Start_Back},                                  {3},    {{28.0f,34.0f,40.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },


        {SpawnData{{112.6f},{E_K::Straight_None_Start_Back},                                  {3},    {{26.0f,20.0f,14.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{112.7f},{E_K::Straight_None_Start_Back},                                  {3},    {{43.0f,37.0f,31.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   }},
        {SpawnData{{112.8f},{E_K::Straight_None_Start_Back},                                  {3},    {{8.0f,2.0f,-4.0f}},                      {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{112.9f},{E_K::Straight_None_Start_Back},                                  {3},    {{-10.0f,-16.0f,-22.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{113.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{-28.0f,-34.0f,-40.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{115.5f},{E_K::Straight_None_Start_Back},                                  {3},    {{-43.0f,-37.0f,-31.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   }},
        {SpawnData{{115.6f},{E_K::Straight_None_Start_Back},                                  {3},    {{-26.0f,-20.0f,-14.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{115.7f},{E_K::Straight_None_Start_Back},                                  {3},    {{-8.0f,-2.0f,4.0f}},                     {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{115.8f},{E_K::Straight_None_Start_Back},                                  {3},    {{10.0f,16.0f,22.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{115.9f},{E_K::Straight_None_Start_Back},                                  {3},    {{28.0f,34.0f,40.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },




        { SpawnData{{normal_boss_time},{E_K::Boss_Ex3},                                                    {1},    {{0.0f}},                                 {{red}}                          ,{{0.0f}}   } },

    };
}

#else

void EnemyManager::SetSpawnDatas_Stage4()
{
    constexpr BaseObject::Type red = BaseObject::Type::RED;
    constexpr BaseObject::Type blue = BaseObject::Type::BLUE;

    /*
     *  SpawnData�̍\��
     *
     *  �������`�̕ϐ��͔z�� (s�Ȃ�z��, ss�Ȃ�z��̔z��ɂȂ��Ă���)
     *
     *
     *
     *  - �����t���O(�b)                    spawn_time
     *                                                         = 7.0f
     *  - �G�̎�ޔz��@                    kinds
     *                                                          kinds[0]                 ,      kinds[1]        ...
     *  - �G�̗�                           sizes
     *                                                         = 2                       ,      = 1             ...
     *  - ��������ʒu(y��z)�̒l�z�� (�G�̗ʕ�) pos_widthss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{10.0f}  ,     = 0�Ԗ�{-10.0f}   ...
     *  - �����z��(�G�̗ʕ�)                 typess
     *                                                         = 0�Ԗ�{red},1�Ԗ�{blue}    ,     = 0�Ԗ�{red}      ...
     *  - ���炷z�l�̗ʔz��                  z_shiftss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{-4.0f}  ,     = 0�Ԗ�{3.0f}     ...
     *
     */

    using E_K = ENEMY_KIND;



    spawn_datas = {

        // �X�^�[�g

        // ���V���[�e�B���O
        // �O�@�`�@60�b�܂�

                    // ��������                 // �G�̎��(�z��)                                                 // ��(�z��)                // ��������ʒu�́@:���Ȃ�y :�c�Ȃ�z �̒l(�z��̔z��)           // ����(�z��̔z��)                    // ��������ۂɂ��炷z�l�̗�(�z��̔z��)

        {SpawnData{{2.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{0.0f}},                                  {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{2.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{7.0f,-7.0f}},                            {{red,red}}                      ,{{0.0f,0.0f}}   }},

        {SpawnData{{3.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{0.0f - 8.0f}},                          {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{3.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{7.0f - 8.0f,-7.0f - 8.0f}},            {{red,red}}                      ,{{0.0f,0.0f}}   }},

        {SpawnData{{4.0f},{E_K::Straight_None_Start_Back},                                 {1},    {{0.0f + 8.0f}},                          {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{4.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{7.0f + 8.0f,-7.0f + 8.0f}},            {{red,red}}                      ,{{0.0f,0.0f}}   }},



        {SpawnData{{9.0f},{E_K::Straight_TargetShot_Start_Back},                           {1},    {{0.0f}},                                  {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{9.3f},{E_K::Straight_None_Start_Back},                                 {2},    {{7.0f,-7.0f}},                            {{red,red}}                      ,{{0.0f,0.0f}}   }},

        {SpawnData{{11.0f},{E_K::Straight_TargetShot_Start_Back},                          {1},    {{0.0f + 15.0f}},                          {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{11.3f},{E_K::Straight_None_Start_Back},                                {2},    {{7.0f + 15.0f,-7.0f + 15.0f}},            {{red,red}}                      ,{{0.0f,0.0f}}   }},

        {SpawnData{{12.0f},{E_K::Straight_TargetShot_Start_Back},                          {1},    {{0.0f - 15.0f}},                          {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{12.3f},{E_K::Straight_None_Start_Back},                                {2},    {{7.0f - 15.0f,-7.0f - 15.0f}},            {{red,red}}                      ,{{0.0f,0.0f}}   }},


        {SpawnData{{18.0f},{E_K::Lock_TargetSingleShot_Hard},                              {2},    {{15.0f,-15.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        {SpawnData{{24.0f},{E_K::Lock_TargetSingleShot_Hard},                              {2},    {{23.0f,-23.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        //{SpawnData{{27.2f},{E_K::Lock_TargetMultiShot_B},                                  {2},    {{15.0f,-15.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        //{SpawnData{{29.2f},{E_K::Lock_TargetMultiShot_B},                                  {2},    {{23.0f,-23.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        {SpawnData{{19.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{14.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{20.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{-14.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{21.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{23.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{22.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{-23.0f}},                                 {{red}}                              ,{{0.0f}}   }},


        {SpawnData{{26.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{13.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{26.5f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{-13.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{27.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{23.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{27.5f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{-23.0f}},                                 {{red}}                              ,{{0.0f}}   }},

        {SpawnData{{28.5f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{13.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{29.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{-13.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{29.5f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{23.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{30.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{-23.0f}},                                 {{red}}                              ,{{0.0f}}   }},

        {SpawnData{{31.0f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{13.0f}},                                 {{red}}                              ,{{0.0f}}   }},
        {SpawnData{{31.5f},{E_K::Lock_Straight_Target_Hard},                                     {1},    {{-13.0f}},                                 {{red}}                              ,{{0.0f}}   }},




        {SpawnData{{40.0f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{40.3f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{40.6f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{40.9f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{41.2f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{41.5f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{41.8f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{42.1f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{45.0f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{45.3f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{45.6f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{45.9f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{46.2f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{46.5f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{46.8f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{47.1f},{E_K::Move_Round_TargetShot_Side_Hard},                         {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},


        {SpawnData{{48.0f},{E_K::Straight_None_Start_Back},                                {1},    {{0.0f}},                                  {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{48.3f},{E_K::Straight_None_Start_Back},                                {2},    {{7.0f,-7.0f}},                            {{red,red}}                      ,{{0.0f,0.0f}}   }},

        {SpawnData{{49.0f},{E_K::Straight_None_Start_Back},                                {1},    {{0.0f - 8.0f}},                          {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{49.3f},{E_K::Straight_None_Start_Back},                                {2},    {{7.0f - 8.0f,-7.0f - 8.0f}},            {{red,red}}                      ,{{0.0f,0.0f}}   }},

        {SpawnData{{50.0f},{E_K::Straight_None_Start_Back},                                {1},    {{0.0f + 8.0f}},                          {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{50.3f},{E_K::Straight_None_Start_Back},                                {2},    {{7.0f + 8.0f,-7.0f + 8.0f}},            {{red,red}}                      ,{{0.0f,0.0f}}   }},


        {SpawnData{{54.0f},{E_K::Straight_None_Start_Front,E_K::Straight_None_Start_Back}, {5,5},  {{23.0f,18.0f,13.0f,8.0f,3.0f},{23.0f,18.0f,13.0f,8.0f,3.0f}},                       {{red,red,red,red,red},{red,red,red,red,red}}    ,{{0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f}}}},
        {SpawnData{{55.5f},{E_K::Straight_None_Start_Front,E_K::Straight_None_Start_Back}, {5,5},  {{-23.0f,-18.0f,-13.0f,-8.0f,-3.0f},{-23.0f,-18.0f,-13.0f,-8.0f,-3.0f}},             {{red,red,red,red,red},{red,red,red,red,red}}    ,{{0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f}}}},
        {SpawnData{{57.0f},{E_K::Straight_None_Start_Front,E_K::Straight_None_Start_Back}, {7,7},  {{23.0f,18.0f,13.0f,8.0f,3.0f,-2.0f,-7.0f},{23.0f,18.0f,13.0f,8.0f,3.0f,-2.0f,-7.0f}},         {{red,red,red,red,red,red,red},{red,red,red,red,red,red,red}}    ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f}}}},
        {SpawnData{{58.5f},{E_K::Straight_None_Start_Front,E_K::Straight_None_Start_Back}, {7,7},  {{-23.0f,-18.0f,-13.0f,-8.0f,-3.0f,2.0f,7.0f},{-23.0f,-18.0f,-13.0f,-8.0f,-3.0f,2.0f,7.0f}},   {{red,red,red,red,red,red,red},{red,red,red,red,red,red,red}}    ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f}}}},
        {SpawnData{{59.0f},{E_K::Straight_None_Start_Front,E_K::Straight_None_Start_Back}, {9,9},  {{23.0f,18.0f,13.0f,8.0f,3.0f,-2.0f,-7.0f,-12.0f,-17.0f},{23.0f,18.0f,13.0f,8.0f,3.0f,-2.0f,-7.0f,-12.0f,-17.0f}},     {{red,red,red,red,red,red,red,red,red},{red,red,red,red,red,red,red,red,red}}    ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f}}}},
        {SpawnData{{60.5f},{E_K::Straight_None_Start_Front,E_K::Straight_None_Start_Back}, {9,9},  {{-23.0f,-18.0f,-13.0f,-8.0f,-3.0f,2.0f,7.0f,12.0f,17.0f},{-23.0f,-18.0f,-13.0f,-8.0f,-3.0f,2.0f,7.0f,12.0f,17.0f}},   {{red,red,red,red,red,red,red,red,red},{red,red,red,red,red,red,red,red,red}}    ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f}}}},




        ////////////////////////////////
        ////        ��ʐ؂�ւ�      ////
        ////////////////////////////////

        // 70�b����`
        // �c�V���[�e�B���O

        {SpawnData{{76.0f - 2.0f},{E_K::Straight_TargetShot_Start_Back},                          {1},    {{-7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{76.3f - 2.0f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{76.6f - 2.0f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{79.0f - 2.0f},{E_K::Straight_TargetShot_Start_Back},                          {1},    {{7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{79.3f - 2.0f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{79.6f - 2.0f},{E_K::Straight_None_Start_Back},                                {1},    {{7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{83.0f - 2.0f},{E_K::Straight_TargetShot_Start_Back},                          {1},    {{-7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{83.3f - 2.0f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                   {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},
        {SpawnData{{83.6f - 2.0f},{E_K::Straight_None_Start_Back},                                {1},    {{-7.0f}},                                  {{blue,blue,blue,blue}}              ,{{10.0f,10.0f,10.0f,10.0f}}   }},

        {SpawnData{{76.0f - 2.0f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{32.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{79.5f - 2.0f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{-32.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{82.0f - 2.0f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{37.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{85.5f - 2.0f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{-37.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{88.5f},{E_K::Straight_None_Start_Back},                                {3},    {{28.0f,33.0f,38.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{89.5f},{E_K::Straight_None_Start_Back},                                {3},    {{-28.0f,-33.0f,-38.0f}},                 {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{90.5f},{E_K::Straight_None_Start_Back},                                {3},    {{-28.0f,-33.0f,-38.0f}},                 {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{91.5f},{E_K::Straight_None_Start_Back},                                {3},    {{-28.0f,-33.0f,-38.0f}},                 {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{87.0f},{E_K::Straight_Shot_Start_Front},                               {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{red,red,red,red}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{88.0f},{E_K::Straight_Shot_Start_Front},                               {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{red,red,red,red}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{89.0f},{E_K::Straight_Shot_Start_Front},                               {4},    {{2.0f,7.0f,12.0f,17.0f}},                 {{red,red,red,red}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{90.0f},{E_K::Straight_Shot_Start_Front},                               {4},    {{-7.0f,-12.0f,-17.0f,-23.0f}},            {{red,red,red,red}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{91.0f},{E_K::Straight_Shot_Start_Front},                               {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{red,red,red,red}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{92.0f},{E_K::Straight_Shot_Start_Front},                               {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{red,red,red,red}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},
        {SpawnData{{93.0f},{E_K::Straight_Shot_Start_Front},                               {4},    {{2.0f,7.0f,12.0f,17.0f}},                 {{red,red,red,red}}              ,{{-8.0f,-8.0f,-8.0f,-8.0f}}   }},



        {SpawnData{{96.0f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{28.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{96.6f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{-28.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{99.0f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{99.6f},{E_K::Translate_Shot_B_Top_Hard},                               {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{103.0f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{17.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{103.6f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-17.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        //{SpawnData{{106.0f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        //{SpawnData{{106.6f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        //{SpawnData{{109.0f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        //{SpawnData{{107.6f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        //{SpawnData{{117.0f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        //{SpawnData{{108.6f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},




        {SpawnData{{103.8f},{E_K::Lock_TargetSingleShot},                                  {2},    {{25.0f,-25.0f}},                          {{red,red}}                    ,{{0.0f,0.0f}}   }},


        {SpawnData{{109.0f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-19.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{109.3f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-19.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{110.8f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{111.1f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},



        {SpawnData{{114.0f},{E_K::Lock_TargetSingleShot_Hard},                             {2},    {{15.0f,-15.0f}},                          {{red,red}}                    ,{{0.0f,0.0f}}   }},

        {SpawnData{{115.0f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-19.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{115.3f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-19.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{116.8f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{19.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{117.1f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{19.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{118.6f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{118.9f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-23.0f}},                                {{blue}}                         ,{{0.0f}}   }},



        { SpawnData{{normal_boss_time},{E_K::Boss_Ex2},                                                    {1},    {{0.0f}},                                 {{red}}                          ,{{0.0f}}   } },

    };
}


void EnemyManager::SetSpawnDatas_Stage5()
{

    constexpr BaseObject::Type red = BaseObject::Type::RED;
    constexpr BaseObject::Type blue = BaseObject::Type::BLUE;

    /*
     *  SpawnData�̍\��
     *
     *  �������`�̕ϐ��͔z�� (s�Ȃ�z��, ss�Ȃ�z��̔z��ɂȂ��Ă���)
     *
     *
     *
     *  - �����t���O(�b)                    spawn_time
     *                                                         = 7.0f
     *  - �G�̎�ޔz��@                    kinds
     *                                                          kinds[0]                 ,      kinds[1]        ...
     *  - �G�̗�                           sizes
     *                                                         = 2                       ,      = 1             ...
     *  - ��������ʒu(y��z)�̒l�z�� (�G�̗ʕ�) pos_widthss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{10.0f}  ,     = 0�Ԗ�{-10.0f}   ...
     *  - �����z��(�G�̗ʕ�)                 typess
     *                                                         = 0�Ԗ�{red},1�Ԗ�{blue}    ,     = 0�Ԗ�{red}      ...
     *  - ���炷z�l�̗ʔz��                  z_shiftss
     *                                                         = 0�Ԗ�{0.0f},1�Ԗ�{-4.0f}  ,     = 0�Ԗ�{3.0f}     ...
     *
     */

    using E_K = ENEMY_KIND;



    spawn_datas = {

        // �X�^�[�g

        // ���V���[�e�B���O
        // �O�@�`�@60�b�܂�

                    // ��������                 // �G�̎��(�z��)                                                 // ��(�z��)                // ��������ʒu�́@:���Ȃ�y :�c�Ȃ�z �̒l(�z��̔z��)           // ����(�z��̔z��)                    // ��������ۂɂ��炷z�l�̗�(�z��̔z��)

        {SpawnData{{2.0f},{E_K::Straight_Shot_Bezier_Side},                                 {1},    {{17.0f}},                                 {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{2.7f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{22.0f}},                                 {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{3.4f},{E_K::Straight_TargetShot_Bezier_Side},                           {1},    {{29.0f}},                                 {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{2.3f},{E_K::Straight_Shot_Bezier_Side},                                 {1},    {{-17.0f}},                                {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{3.0f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{-22.0f}},                                {{red}}                         ,{{0.0f}}   }},
        {SpawnData{{3.7f},{E_K::Straight_TargetShot_Bezier_Side},                           {1},    {{-29.0f}},                                {{red}}                         ,{{0.0f}}   }},

        {SpawnData{{3.2f},{E_K::Lock_TargetSingleShot_Hard},                                    {2},    {{15.0f,-15.0f}},                          {{red,red}}                    ,{{0.0f,0.0f}}   }},


        {SpawnData{{7.1f},{E_K::Straight_Shot_Bezier_Side},                                 {1},    {{20.0f}},                                 {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{7.3f},{E_K::Straight_Shot_Bezier_Side},                                 {1},    {{8.5f}},                                  {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{7.5f},{E_K::Straight_TargetShot_Bezier_Side},                           {1},    {{16.0f}},                                 {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{7.7f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{10.0f}},                                 {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{7.9f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{19.0f}},                                 {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{7.1f},{E_K::Straight_None_Bezier_Side},                                 {1},    {{9.0f}},                                  {{blue}}                          ,{{0.0f}}   }},


        {SpawnData{{12.0f},{E_K::Lock_MultiShot_B},                                         {2},    {{15.0f,25.0f}},                           {{blue,blue}}                    ,{{0.0f,0.0f}}   }},

        {SpawnData{{13.1f},{E_K::Straight_Shot_Bezier_Side},                                {1},    {{-20.0f}},                                {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{13.3f},{E_K::Straight_Shot_Bezier_Side},                                {1},    {{-8.5f}},                                 {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{13.5f},{E_K::Straight_TargetShot_Bezier_Side},                          {1},    {{-16.0f}},                                {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{13.7f},{E_K::Straight_None_Bezier_Side},                                {1},    {{-10.0f}},                                {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{13.9f},{E_K::Straight_None_Bezier_Side},                                {1},    {{-19.0f}},                                {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{13.1f},{E_K::Straight_None_Bezier_Side},                                {1},    {{-9.0f}},                                 {{blue}}                          ,{{0.0f}}   }},


        {SpawnData{{13.6f},{E_K::Straight_Shot_Start_Front},                                {4},    {{-12.0f,12.0f,-23.0f,23.0f}},             {{blue,blue,red,red}}            ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{13.8f},{E_K::Lock_Straight_Target},                                     {2},    {{-10.0f,10.0f}},                          {{red,red}}                      ,{{0.0f,0.0f}}   }},
        {SpawnData{{14.8f},{E_K::Lock_Straight_Target},                                     {2},    {{-23.0f,23.0f}},                          {{red,red}}                      ,{{0.0f,0.0f}}   }},


        {SpawnData{{14.9f},{E_K::Rotate_AroundShot_E},                                      {1},    {{0.0f}},                                  {{red}}                          ,{{0.0f}}   }},

        {SpawnData{{22.0f},{E_K::Straight_None_Start_Front},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{23.0f},{E_K::Straight_None_Start_Front},                                {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{24.0f},{E_K::Straight_TargetShot_Start_Front},                          {4},    {{2.0f,7.0f,12.0f,17.0f}},                 {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{25.0f},{E_K::Straight_None_Start_Front},                                {4},    {{-7.0f,-12.0f,-17.0f,-23.0f}},            {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{26.0f},{E_K::Straight_None_Start_Front},                                {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{27.0f},{E_K::Straight_TargetShot_Start_Front},                          {4},    {{-2.0f,-7.0f,-12.0f,-17.0f}},             {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{28.0f},{E_K::Straight_None_Start_Front},                                {4},    {{2.0f,7.0f,12.0f,17.0f}},                 {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        // ���ꂨ���̂�37.0f�b


        {SpawnData{{33.0f},{E_K::Straight_TargetShot_Bezier_Side},                          {4},    {{7.0f,12.0f,17.0f,23.0f}},                {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{35.0f},{E_K::Straight_TargetShot_Bezier_Side},                          {4},    {{-7.0f,-12.0f,-17.0f,-23.0f}},            {{red,red,red,red}}              ,{{-5.0f,-5.0f,-5.0f,-5.0f}}   }},
        {SpawnData{{35.5f},{E_K::Rotate_AroundShot_E},                                      {1},    {{0.0f}},                                  {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{36.0f},{E_K::Lock_TargetSingleShot_Hard},                               {2},    {{15.0f,-15.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        {SpawnData{{39.0f},{E_K::Lock_TargetSingleShot_Hard},                               {2},    {{23.0f,-23.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        {SpawnData{{44.2f},{E_K::Lock_TargetMultiShot_B_Hard},                              {2},    {{15.0f,-15.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        {SpawnData{{46.2f},{E_K::Lock_TargetMultiShot_B_Hard},                              {2},    {{23.0f,-23.0f}},                          {{blue,blue}}                    ,{{0.0f,0.0f}}   }},
        // ���ꂪ�����̂�60.0f�b



        {SpawnData{{54.0f + 0.8f},{E_K::Straight_None_Start_Back, E_K::Straight_TargetShot_Start_Back},  {10,1},    {{23.0f,19.0f,15.0f,11.0f,7.0f,-7.0f,-11.0f,-15.0f,-19.0f,-23.0f},{0.0f}},            {{blue,blue,blue,blue,blue,blue,blue,blue,blue,blue},{blue}}              ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f}}   }},
        {SpawnData{{55.5f + 0.8f},{E_K::Straight_None_Start_Back, E_K::Straight_TargetShot_Start_Back},  {10,1},    {{13.0f,9.0f,5.0f,1.0f,-3.0f,-7.0f,-11.0f,-15.0f,-19.0f,-23.0f},{21.0f}},            {{blue,blue,blue,blue,blue,blue,blue,blue,blue,blue},{blue}}              ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f}}   }},
        {SpawnData{{57.0f + 0.8f},{E_K::Straight_None_Start_Back, E_K::Straight_TargetShot_Start_Back},  {10,1},    {{23.0f,19.0f,15.0f,11.0f,7.0f,3.0f,-1.0f,-5.0f,-9.0f,-13.0f},{-21.0f}},            {{blue,blue,blue,blue,blue,blue,blue,blue,blue,blue},{blue}}              ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f}}   }},
        {SpawnData{{58.5f + 0.8f},{E_K::Straight_None_Start_Back, E_K::Straight_TargetShot_Start_Back},  {10,1},    {{13.0f,9.0f,5.0f,1.0f,-3.0f,-7.0f,-11.0f,-15.0f,-19.0f,-23.0f},{21.0f}},            {{blue,blue,blue,blue,blue,blue,blue,blue,blue,blue},{blue}}              ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f}}   }},
        {SpawnData{{60.0f + 0.8f},{E_K::Straight_None_Start_Back, E_K::Straight_TargetShot_Start_Back},  {10,1},    {{23.0f,19.0f,15.0f,11.0f,7.0f,3.0f,-1.0f,-5.0f,-9.0f,-13.0f},{-21.0f}},            {{blue,blue,blue,blue,blue,blue,blue,blue,blue,blue},{blue}}              ,{{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},{0.0f}}   }},




        ////////////////////////////////
        ////        ��ʐ؂�ւ�      ////
        ////////////////////////////////

        // 70�b����`
        // �c�V���[�e�B���O

        {SpawnData{{72.0f},{E_K::Rotate_AroundShot_E},                                       {1},    {{0.0f}},                                 {{red}}                          ,{{0.0f}}   }},


        {SpawnData{{73.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   }},
        {SpawnData{{73.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   }},
        {SpawnData{{73.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   }},
        {SpawnData{{74.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   }},


        {SpawnData{{74.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-55.0f}}   }},
        {SpawnData{{74.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-55.0f}}   }},
        {SpawnData{{74.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-55.0f}}   }},
        {SpawnData{{75.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-55.0f}}   }},

        {SpawnData{{75.1f},{E_K::Lock_TargetSingleShot_Hard},                                     {1},    {{-15.0f}},                               {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{75.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   }},
        {SpawnData{{75.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   }},
        {SpawnData{{75.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   }},
        {SpawnData{{76.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   }},

        {SpawnData{{76.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},
        {SpawnData{{76.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},
        {SpawnData{{76.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},
        {SpawnData{{77.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-65.0f}}   }},

        {SpawnData{{77.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-40.0f}}   } },
        {SpawnData{{77.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-40.0f}}   } },
        {SpawnData{{77.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-40.0f}}   } },
        {SpawnData{{78.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-40.0f}}   } },

        {SpawnData{{78.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-90.0f}}   } },
        {SpawnData{{78.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-90.0f}}   } },
        {SpawnData{{78.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-90.0f}}   } },
        {SpawnData{{79.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-90.0f}}   } },

        {SpawnData{{79.1f},{E_K::Lock_TargetSingleShot_Hard},                                     {1},    {{15.0f}},                                {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{79.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   } },
        {SpawnData{{79.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   } },
        {SpawnData{{79.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   } },
        {SpawnData{{80.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-50.0f}}   } },

        {SpawnData{{80.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-75.0f}}   } },
        {SpawnData{{80.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-75.0f}}   } },
        {SpawnData{{80.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-75.0f}}   } },
        {SpawnData{{81.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                               {{red}}                          ,{{-75.0f}}   } },

        {SpawnData{{81.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-60.0f}}   } },
        {SpawnData{{81.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-60.0f}}   } },
        {SpawnData{{81.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-60.0f}}   } },
        {SpawnData{{82.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-60.0f}}   } },

        {SpawnData{{82.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-55.0f}}   } },
        {SpawnData{{82.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-55.0f}}   } },
        {SpawnData{{82.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-55.0f}}   } },
        {SpawnData{{83.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-55.0f}}   } },

        {SpawnData{{83.1f},{E_K::Lock_TargetSingleShot_Hard},                                     {2},    {{30.0f,-30.0f}},                         {{blue,blue}}                    ,{{0.0f,0.0f}}   }},

        {SpawnData{{83.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-90.0f}}   } },
        {SpawnData{{83.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-90.0f}}   } },
        {SpawnData{{83.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-90.0f}}   } },
        {SpawnData{{84.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-90.0f}}   } },

        {SpawnData{{84.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-65.0f}}   } },
        {SpawnData{{84.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-65.0f}}   } },
        {SpawnData{{84.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-65.0f}}   } },
        {SpawnData{{85.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{-55.0f}},                                {{red}}                         ,{{-65.0f}}   } },

        {SpawnData{{72.0f},{E_K::Rotate_AroundShot_E},                                       {1},    {{43.0f}},                                 {{blue}}                          ,{{0.0f}}   }},

        {SpawnData{{85.0f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   } },
        {SpawnData{{85.4f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   } },
        {SpawnData{{85.8f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   } },
        {SpawnData{{86.2f},{E_K::Straight_None_Horizon_Top},                                 {1},    {{55.0f}},                                {{red}}                          ,{{-80.0f}}   } },



        {SpawnData{{90.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{26.0f,20.0f,14.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{90.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{10.0f,16.0f,22.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{91.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{43.0f,37.0f,31.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   }},

        {SpawnData{{92.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{-43.0f,-37.0f,-31.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   }},

        {SpawnData{{93.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{8.0f,2.0f,-4.0f}},                      {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{94.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{-8.0f,-2.0f,4.0f}},                     {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{95.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{-10.0f,-16.0f,-22.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{96.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{-26.0f,-20.0f,-14.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{97.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{-28.0f,-34.0f,-40.0f}},                 {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{98.0f},{E_K::Straight_None_Start_Back},                                  {3},    {{28.0f,34.0f,40.0f}},                    {{blue,blue,blue}}                ,{{10.0f,10.0f,10.0f}}   } },


        //{SpawnData{{100.1f},{E_K::Straight_None_Start_Back},                                  {2},    {{26.0f,20.0f}},                    {{blue,blue}}                ,{{10.0f,10.0f}}   } },
        //{SpawnData{{100.5f},{E_K::Straight_None_Start_Back},                                  {2},    {{26.0f,20.0f}},                    {{blue,blue}}                ,{{10.0f,10.0f}}   } },
        //{SpawnData{{100.9f},{E_K::Straight_None_Start_Back},                                  {2},    {{26.0f,20.0f}},                    {{blue,blue}}                ,{{10.0f,10.0f}}   } },



        {SpawnData{{100.1f},{E_K::Straight_Shot_Start_Back},                                  {3},    {{26.0f,20.0f,14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{100.5f},{E_K::Straight_None_Start_Back},                                  {3},    {{26.0f,20.0f,14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{100.9f},{E_K::Straight_None_Start_Back},                                  {3},    {{26.0f,20.0f,14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{101.3f},{E_K::Straight_None_Start_Back},                                  {3},    {{26.0f,20.0f,14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{101.7f},{E_K::Straight_None_Start_Back},                                  {3},    {{26.0f,20.0f,14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{102.1f},{E_K::Straight_None_Start_Back},                                  {3},    {{26.0f,20.0f,14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{102.5f},{E_K::Straight_None_Start_Back},                                  {3},    {{26.0f,20.0f,14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{102.9f},{E_K::Straight_None_Start_Back},                                  {3},    {{26.0f,20.0f,14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{103.3f},{E_K::Straight_None_Start_Back},                                  {3},    {{26.0f,20.0f,14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },

        {SpawnData{{102.0f},{E_K::Lock_Straight_Target_Hard},                                  {1},    {{-15.0f}},                               {{red}}                          ,{{0.0f}}   }},
        {SpawnData{{103.0f},{E_K::Lock_Straight_Target_Hard},                                  {1},    {{-25.0f}},                               {{red}}                          ,{{0.0f}}   }},


        {SpawnData{{105.1f},{E_K::Straight_Shot_Start_Back},                                  {3},    {{-26.0f,-20.0f,-14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{105.5f},{E_K::Straight_None_Start_Back},                                  {3},    {{-26.0f,-20.0f,-14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{105.9f},{E_K::Straight_None_Start_Back},                                  {3},    {{-26.0f,-20.0f,-14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{106.3f},{E_K::Straight_None_Start_Back},                                  {3},    {{-26.0f,-20.0f,-14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{106.7f},{E_K::Straight_None_Start_Back},                                  {3},    {{-26.0f,-20.0f,-14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{107.1f},{E_K::Straight_None_Start_Back},                                  {3},    {{-26.0f,-20.0f,-14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        {SpawnData{{107.5f},{E_K::Straight_None_Start_Back},                                  {3},    {{-26.0f,-20.0f,-14.0f}},                    {{red,red,red}}                ,{{10.0f,10.0f,10.0f}}   } },
        
        {SpawnData{{107.0f},{E_K::Lock_Straight_Target_Hard},                                  {1},    {{15.0f}},                               {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{108.0f},{E_K::Lock_Straight_Target_Hard},                                  {1},    {{25.0f}},                               {{blue}}                          ,{{0.0f}}   }},


        {SpawnData{{110.0f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{17.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{110.3f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{17.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{110.6f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{110.9f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},


        {SpawnData{{110.4f},{E_K::Lock_Straight_Target_Hard},                                  {1},    {{15.0f}},                               {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{111.1f},{E_K::Lock_Straight_Target_Hard},                                  {1},    {{25.0f}},                               {{blue}}                          ,{{0.0f}}   }},



        {SpawnData{{113.0f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-17.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{113.3f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-17.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{113.6f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},
        {SpawnData{{113.9f},{E_K::Translate_Shot_B_Top_Hard},                              {1},    {{-23.0f}},                                 {{blue}}                         ,{{0.0f}}   }},

        {SpawnData{{113.4f},{E_K::Lock_Straight_Target_Hard},                                  {1},    {{-15.0f}},                               {{blue}}                          ,{{0.0f}}   }},
        {SpawnData{{114.1f},{E_K::Lock_Straight_Target_Hard},                                  {1},    {{-25.0f}},                               {{blue}}                          ,{{0.0f}}   }},
            


        { SpawnData{{normal_boss_time - 4.0f},{E_K::Boss_Ex3},                                                    {1},    {{0.0f}},                                 {{red}}                          ,{{0.0f}}   } },

    };
}

#endif



Enemy* EnemyManager::SpawnEnemy(const ENEMY_KIND kind, BaseObject::Type enemy_type)
{
    // �^�C�v�̃����_������ (0 ~ 1)
    if (enemy_type == BaseObject::Type::NONE)
    {
        enemy_type = (rand() % 2) > 0 ? BaseObject::Type::RED : BaseObject::Type::BLUE;
    }

    // ��Փx�ʂɌ��܂����̓G�̘A���ˌ��񐔂�����I�I
    // �ʏ�͂P
    short max_shot = 1;
    if(/*stage == STAGE::EXTRA1 || stage == STAGE::EXTRA2 || stage == STAGE::EXTRA3*/stage == STAGE::STAGE4 || stage == STAGE::STAGE5)
    {
        // �G�N�X�g���̎��͂R�A��
        max_shot = 3;
    }


    ID3D11Device* device = SceneManager::Instance().GetDevice();

    // TODO:target��ݒ肷�� ���Y��Ȃ�

    switch (kind)
    {
    case ENEMY_KIND::Straight_None:
        return new EnemyStraight_None(device,enemy_type);
        break;

    case ENEMY_KIND::Straight_None_Start_Back:
        return new EnemyStraight_None_Start_Back(device, enemy_type);
        break;

    case ENEMY_KIND::Straight_None_Start_Front:
        return new EnemyStraight_None_Start_Front(device, enemy_type);
        break;

    case ENEMY_KIND::Straight_None_Start_Side:
        return new EnemyStraight_None_Start_Side(device, enemy_type);
        break;

    case ENEMY_KIND::Straight_None_Bezier_Top:
        return new EnemyStraight_None_Bezier_Top(device, enemy_type);
        break;

    case ENEMY_KIND::Straight_None_Bezier_Side:
        return new EnemyStraight_None_Bezier_Side(device, enemy_type);
        break;

    case ENEMY_KIND::Straight_None_Horizon_Top:
        return new EnemyStraight_None_Horizon_Top(device, enemy_type);
        break;



    case ENEMY_KIND::Straight_Shot_Start_Back:
        return new EnemyStraight_Shot_Start_Back(device, enemy_type);
        break;

    case ENEMY_KIND::Straight_Shot_Start_Front:
        return new EnemyStraight_Shot_Start_Front(device, enemy_type);
        break;

    case ENEMY_KIND::Straight_Shot_Start_Side:
        return new EnemyStraight_Shot_Start_Side(device, enemy_type);
        break;

    case ENEMY_KIND::Straight_Shot_Bezier_Top:
        return new EnemyStraight_Shot_Bezier_Top(device, enemy_type);
        break;

    case ENEMY_KIND::Straight_Shot_Bezier_Side:
        return new EnemyStraight_Shot_Bezier_Side(device, enemy_type);
        break;



    case ENEMY_KIND::Straight_TargetShot_Start_Back:
        return new EnemyStraight_TargetShot_Start_Back(device, enemy_type, target, max_shot);
        break;

    case ENEMY_KIND::Straight_TargetShot_Start_Front:
        return new EnemyStraight_TargetShot_Start_Front(device, enemy_type, target, max_shot);
        break;

    case ENEMY_KIND::Straight_TargetShot_Start_Side:
        return new EnemyStraight_TargetShot_Start_Side(device, enemy_type, target, max_shot);
        break;

    case ENEMY_KIND::Straight_TargetShot_Bezier_Top:
        return new EnemyStraight_TargetShot_Bezier_Top(device, enemy_type, target, max_shot);
        break;

    case ENEMY_KIND::Straight_TargetShot_Bezier_Side:
        return new EnemyStraight_TargetShot_Bezier_Side(device, enemy_type, target, max_shot);
        break;


    //case ENEMY_KIND::Wave_None:
    //    return new EnemyWave_None(device, enemy_type);
    //    break;
    //
    //case ENEMY_KIND::Wave_Shot_B:
    //    return new EnemyWave_Shot_B(device, enemy_type);
    //    break;
    //
    //case ENEMY_KIND::Wave_Shot_E:
    //    return new EnemyWave_Shot_E(device, enemy_type);
    //    break;
    //
    //case ENEMY_KIND::Lock_Shot_B:
    //    return new EnemyLock_Shot_B(device, enemy_type, target);
    //    break;
    //
    //case ENEMY_KIND::Lock_Shot_E:
    //    return new EnemyLock_Shot_E(device, enemy_type, target);
    //    break;

    case ENEMY_KIND::Lock_MultiShot_B:
        return new EnemyLock_MultiShot_B(device, enemy_type);
        break;

    case ENEMY_KIND::Lock_MultiShot_E:
        return new EnemyLock_MultiShot_E(device, enemy_type);
        break;

    case ENEMY_KIND::Lock_TargetMultiShot_B:
        return new EnemyLock_TargetMultiShot_B(device, enemy_type, target);
        break;

    case ENEMY_KIND::Lock_TargetMultiShot_B_Hard:
        return new EnemyLock_TargetMultiShot_B(device, enemy_type, target, true);
        break;

    case ENEMY_KIND::Lock_TargetSingleShot:
        return new EnemyLock_TargetSingleShot(device, enemy_type, target);
        break;

    case ENEMY_KIND::Lock_TargetSingleShot_Hard:
        return new EnemyLock_TargetSingleShot(device, enemy_type, target, true);
        break;

    case ENEMY_KIND::Lock_Straight_Target:
        return new EnemyLock_Straight_Target(device, enemy_type, target);
        break;

    case ENEMY_KIND::Lock_Straight_Target_Hard:
        return new EnemyLock_Straight_Target(device, enemy_type, target, true);
        break;


    case ENEMY_KIND::Translate_Shot_B_Top:
        return new EnemyTranslate_Shot_B_Top(device, enemy_type);
        break;

    case ENEMY_KIND::Translate_Shot_B_Top_Hard:
        return new EnemyTranslate_Shot_B_Top(device, enemy_type, true);
        break;

    case ENEMY_KIND::Translate_Shot_B_Side:
        return new EnemyTranslate_Shot_B_Side(device, enemy_type);
        break;

    case ENEMY_KIND::Translate_Shot_B_Side_Hard:
        return new EnemyTranslate_Shot_B_Side(device, enemy_type, true);
        break;

    case ENEMY_KIND::Rotate_AroundShot_E:
        return new EnemyRotate_AroundShot_E(device, enemy_type);
        break;

    case ENEMY_KIND::Move_Round_TargetShot_Side:
        return new EnemyMove_Round_TargetShot_Side(device, enemy_type, target);
        break;

    case ENEMY_KIND::Move_Round_TargetShot_Side_Hard:
        return new EnemyMove_Round_TargetShot_Side(device, enemy_type, target, true);
        break;

    //case ENEMY_KIND::Back_Front_Straight_None:
    //    return new EnemyBack_Front_Straight_None(device, enemy_type);
    //    break;

    case ENEMY_KIND::Boss1:
        return new Boss_7(device);
        break;

    case ENEMY_KIND::Boss2:
        return new Boss_5(device);
        break;

    case ENEMY_KIND::Boss3:
        return new Boss_6(device);
        break;

    case ENEMY_KIND::Boss4:
        return new Boss_0(device);
        break;

    case ENEMY_KIND::Boss5:
        return new Boss_3(device);
        break;

    case ENEMY_KIND::Boss_Ex1:
        return new Boss_4(device);
        break;

    case ENEMY_KIND::Boss_Ex2:
        return new Boss_4(device);
        break;

    case ENEMY_KIND::Boss_Ex3:
        return new Boss_8(device);
        break;

    }

    return nullptr;
}



void EnemyManager::EraseSpawnData()
{
    // spawn_datas�̐ݒ莞�Ԃ������ɕ��ׂ�
    std::sort(spawn_datas.begin(), spawn_datas.end(), [](const SpawnData& a,const SpawnData& b) {return a.spawn_time < b.spawn_time; });

    // �폜����
    std::vector<SpawnData> removes;
    for(std::vector<SpawnData>::iterator& it = spawn_datas.begin(); it != spawn_datas.end();)
    {
        if (it->spawn_time > spawn_timer.NowTime()) {
            // �C�e���[�^�[�̎w�������f�[�^�����ݎ��Ԃ��傫�����
            // �폜��������߂�
            break;
        }

        // �������Ԃ��߂��Ă���΁A�����ς݂Ȃ̂ō폜����
        it = spawn_datas.erase(it);
    }
}




void EnemyManager::ChangeCameraOnce()
{
    const float time = spawn_timer.NowTime();

    // Side����Top�ɕς��鏈���Ȃ̂ŁATop��Ԃ̏ꍇ�S�đ���return������
    if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP) return;


    // �X�e�[�W��STAGE�̎��̏���
    if(stage == STAGE::STAGE1 || stage == STAGE::STAGE2 || stage == STAGE::STAGE3/* || stage == STAGE::STAGE4 || stage == STAGE::STAGE5*/)
    {

        // �^�C�}�[���J�����J�ڂ�3�b�O����P�b��܂Ńv���C���[�̕ό`���ł��Ȃ�����
        if(time >= 46.0f && time <= 51.0f)
        {
            is_player_not_transfrom_time = true;
        }

        // 49.0f~50.0f�b�̊Ԃ̂݃J�����J�ڂ����s�\
        if (time < 49.0f || time > 50.0f) return;
    }

    // �X�e�[�W��EXTRA�Ƃ��̏���
    else if(/*stage == STAGE::EXTRA1 || stage == STAGE::EXTRA2 || stage == STAGE::EXTRA3*/stage == STAGE::STAGE4 || stage == STAGE::STAGE5)
    {
        // �^�C�}�[���J�����J�ڂ�3�b�O����P�b��܂Ńv���C���[�̕ό`���ł��Ȃ�����
        if (time >= 66.0f && time <= 71.0f)
        {
            is_player_not_transfrom_time = true;
        }

        // 49.0f~50.0f�b�̊Ԃ̂݃J�����J�ڂ����s�\
        if (time < 69.0f || time > 70.0f) return;
    }
    

    GameSystem::Instance().ChangeView();
}



void EnemyManager::BossJustThen()
{
    const float time = spawn_timer.NowTime();

    if (stage == STAGE::STAGE1 || stage == STAGE::STAGE2 || stage == STAGE::STAGE3/* || stage == STAGE::STAGE4 || stage == STAGE::STAGE5*/)
    {
        // �^�C�}�[���{�X4�b�O����v���C���[�̕ό`���ł��Ȃ�����
        if (time >= easy_boss_time - 4.0f && time < easy_boss_time + 6.0f)
        {
            is_player_not_transfrom_time = true;
        }
    }

    else if (/*stage == STAGE::EXTRA1 || stage == STAGE::EXTRA2 || stage == STAGE::EXTRA3*/stage == STAGE::STAGE4 || stage == STAGE::STAGE5)
    {
        // �^�C�}�[���{�X4�b�O����v���C���[�̕ό`���ł��Ȃ�����
        if (time >= normal_boss_time - 4.0f && time < normal_boss_time + 6.0f)
        {
            is_player_not_transfrom_time = true;
        }
    }
}





//****************************************************************
// 
//  �`���[�g���A��
// 
//****************************************************************

void EnemyManager::TutorialUpdate(float elapsedTime)
{

    // �{�G�l�~�[�̍X�V
    for (const auto& enemy : enemies)
    {
        enemy->Update(elapsedTime);
    }

    //�_�~�[�G�l�~�[�X�V
    for (const auto& enemy : dummies)
    {
        enemy->Update(elapsedTime);
    }

    // �j������
    for (std::unique_ptr<Enemy>& enemy : removes)
    {
        std::vector<std::unique_ptr<Enemy>>::iterator it =
            std::find(enemies.begin(), enemies.end(), enemy);
        if (it != enemies.end()) { enemies.erase(it); }
        if (enemy != nullptr) { enemy.release(); }
    }
    removes.clear();


    //--------------------<�f�o�b�O���j���[>--------------------//
    GuiMenu();
}

void EnemyManager::TutorialSpownEnemy(int CurrentTutorialState)
{
    //enum TutorialState // �`���[�g���A���̎��
    //{
    //    None,     // �Ȃ�
    //    Move,     // �ړ�
    //    Attack,   // �U��
    //    Dodge,    // ���
    //    Element1, // �����P 
    //    Element2, // �����Q
    //    Value,    // ��
    //};

    // �U���̃`���[�g���A���̓G����
    switch (CurrentTutorialState)
    {
    case 0:   // None
        // �Ȃ�
        break;
    case 1:   // Move
        // �Ȃ�
        break;
    case 2:   // Attack
        // �G�𐶐�
        if(!IsAliveAttackTutorialEnemy1)
        {
            ID3D11Device* device = SceneManager::Instance().GetDevice();
            Enemy* enemy = new AttackTutorialEnemy(device, BaseObject::Type::RED, { 0.0f,10.0f,40.0f }, 1);
            IsAliveAttackTutorialEnemy1 = true;
            RegisterEnemy(enemy);
        }
        if (!IsAliveAttackTutorialEnemy2)
        {
            ID3D11Device* device = SceneManager::Instance().GetDevice();
            Enemy* enemy = new AttackTutorialEnemy(device, BaseObject::Type::BLUE, { 0.0f,-10.0f,40.0f }, 2);
            IsAliveAttackTutorialEnemy2 = true;
            RegisterEnemy(enemy);
        }

        break;
    case 3:   // Dodge

        if(!IsClearEnemies)
        {
            enemies.clear();
            removes.clear();
            IsClearEnemies = true;

            ID3D11Device* device = SceneManager::Instance().GetDevice();
            Enemy* enemy = new DodgeTutorialEnemy(device, BaseObject::Type::RED, { 0.0f,-10.0f,40.0f });
            RegisterEnemy(enemy);

        }



        break;
    case 4:   // Element1

        if (!IsClearEnemies)
        {
            enemies.clear();
            removes.clear();
            IsClearEnemies = true;

        }

       

        break;
    case 5:   // Element2
    {
        ID3D11Device* device = SceneManager::Instance().GetDevice();
        if (!IsAliveElementTutorialEnemy1)
        {
            Enemy* enemy = new EnergyTutorialEnemy(device, BaseObject::Type::RED, { 0.0f,-10.0f,40.0f });
            RegisterEnemy(enemy);
            IsAliveElementTutorialEnemy1 = true;
        }
        if (!IsAliveElementTutorialEnemy2)
        {
            Enemy* enemy2 = new EnergyTutorialEnemy2(device, BaseObject::Type::BLUE, { 0.0f,10.0f,40.0f });
            RegisterEnemy(enemy2);
            IsAliveElementTutorialEnemy2 = true;
        }
    }
        break;
    case 6:
    {
        ID3D11Device* device = SceneManager::Instance().GetDevice();
        if (!IsAliveElementTutorialEnemy1)
        {
            Enemy* enemy = new EnergyTutorialEnemy(device, BaseObject::Type::RED, { 0.0f,-10.0f,40.0f });
            RegisterEnemy(enemy);
            IsAliveElementTutorialEnemy1 = true;
        }
        if (!IsAliveElementTutorialEnemy2)
        {
            Enemy* enemy2 = new EnergyTutorialEnemy2(device, BaseObject::Type::BLUE, { 0.0f,10.0f,40.0f });
            RegisterEnemy(enemy2);
            IsAliveElementTutorialEnemy2 = true;
        }
    }
        break;

    default:

        break;
    }

    // ����̃`���[�g���A���̓G����

    // �G�l���M�[�̃`���[�g���A���̓G����


}

#undef TO_STRING