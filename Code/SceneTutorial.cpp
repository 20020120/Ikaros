#include "SceneTutorial.h"
#include"SceneManager.h"
#include"SceneGame.h"
#include "SceneLoading.h"

#include"ProjectileManager.h"
#include "EffectManager.h"
#include"EnemyManager.h"
#include "SceneTitle.h"
#include "gAudio.h"
//#include "OptionSystem.h"

SceneTutorial::SceneTutorial()
{
    
}


SceneTutorial::~SceneTutorial()
{
    
}

void SceneTutorial::Initialize(ID3D11Device* device)
{
    //--------------------<������>--------------------//

    backGround = std::make_unique<BackGround>(device);

    camera = std::make_unique<Camera>(device);

    player = std::make_unique<Player>(device);
    player->Initialize();
    ProjectileManager::Instance().Initialize(device);

    explainManager.Initialize(device);

    option = std::make_unique<Option>(device, Option::MODE::TUTORIAL);
    openOption = false;
    closeOptionInterval = 0.0f;

    // �G�̊Ǘ��N���X�̏�����
    EnemyManager::Instance().Initialize(device);
    player->SetIsTutorial(true);
    player->SetPosition({ 0.0f,0.0f,0.0f });

    GameSystem::Instance().Initialize(device, player->GetGameSystemData());
   
    GameSystem::Instance().SetCameraDirection(GameSystem::CAMERA_DIRECTION::SIDE);
    CurrentTutorial = 0;

    bgm = Audio::Instance().LoadAudioSource("./resources/Sounds/BGM/SceneTutorial.wav");
    bgm->Play(true, OptionSystem::Instance().GetBgmVolume());
}

void SceneTutorial::Update(float elapsedTime)
{
    bgm->SetVolume(OptionSystem::Instance().GetBgmVolume());
    
    //�����{�^������������Q�[����ʂ�
#ifdef _DEBUG
    if (GetAsyncKeyState('Z') & 0x8000)
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame()));
        return;
    }

#endif



    //�I�v�V�����̍X�V
    if (OptionUpdate(elapsedTime))return;

    explainManager.Update(elapsedTime);
    //****************************************************************
    // 
    // �`���[�g���A���̃e�L�X�g���\������Ă���Ƃ������艺�͍X�V����Ȃ�
    // 
    //****************************************************************
    if(explainManager.GetShowTutorialText())
    {
        return;
    }

    if(CurrentTutorial!= explainManager.GetCurrentTutorialState())
    {
        EnemyManager::Instance().ResetIsClearEnemies();
    }
    CurrentTutorial = explainManager.GetCurrentTutorialState();


    switch (CurrentTutorial)
    {
    case 0:   // None
        // �Ȃ�
        break;
    case 1:   // Move
         MoveTutorial(elapsedTime);    // �ړ��̃`���[�g���A��
        break;
    case 2:   // Attack
        Attacktutorial(elapsedTime);  // �U���̃`���[�g���A��
        player->ReSetCompleteDodgeTutorial();
        break;
    case 3:  // Dodge
        DodgeTutorial(elapsedTime);   // ����̃`���[�g���A��
        break;
    case 4:  // Element1
        break;
    case 5:  // Element2
        Energy1Tutorial(elapsedTime); // �ό`�̃`���[�g���A��
        break;
    case 6:
        break;
    }
    
    EffectManager::Instance().Update(elapsedTime);
    

    
    if(explainManager.GetPermissionPerformancePlayer())
    {
        EnemyManager::Instance().ResetSpawnTimer();
        EnemyManager::Instance().SetStageIndex(EnemyManager::STAGE::STAGE1);
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame()));
        return;
    }

    //****************************************************************
    // 
    // ������艺�͑ޏꉉ�o�̎��X�V����Ȃ�
    // 
    //****************************************************************


    // ��𐬌����ɃX���[�ɂȂ�
    player->HitSlow(elapsedTime);


    // �G�̍X�V
    EnemyManager::Instance().TutorialUpdate(elapsedTime);
    EnemyManager::Instance().TutorialSpownEnemy(explainManager.GetCurrentTutorialState());
    // �J�������v���C���[�Ɍ�����
 // �J�������v���C���[�Ɍ�����
    camera->SetAttention(player->GetAttentionCamera());
    camera->SetPlayerPos(player->GetPosition());
    camera->f_Gui();
    camera->Update(elapsedTime);
    // �v���C���[�̍X�V
    player->Update(elapsedTime);


    GameSystem::Instance().Update(elapsedTime);

    // �e�̍X�V
    ProjectileManager::Instance().Update(elapsedTime);
    // �G�t�F�N�g�̍X�V
}

void SceneTutorial::Render(ID3D11DeviceContext* dc)
{
    camera->f_CameraUpdate(dc);
    backGround->Render(dc);

    player->Render(dc);
    EnemyManager::Instance().Render(dc);
    ProjectileManager::Instance().Render(dc);

    EffectManager::Instance().Render(camera->GetView(), camera->GetProjection());

    GameSystem::Instance().Render(dc);

    explainManager.Render(dc);

    // �I�v�V�����\��
    option->Render(dc);

}

void SceneTutorial::Finalize()
{
    explainManager.Finalize();

    bgm->Release();

    player->Finalize();

    option->ReleaseSounds();

    EnemyManager::Instance().Finalize();

    GameSystem::Instance().Finalize();

}

void SceneTutorial::Attacktutorial(float elapsedTime)
{
    // �ˌ��œG��|�����Ƃ�
    if (EnemyManager::Instance().GetIsAliveShotTutorialEnemy())
    {
        explainManager.SetCompleteShotTutorial(true);
    }
    // �ߐڍU���œG��|�����Ƃ�
    if (player->GetTutorial_IsBreakCloseAttack())
    {
        explainManager.SetCompleteCloseTutorial(true);
    }



}

//--------------------<����̃`���[�g���A��>--------------------//
void SceneTutorial::DodgeTutorial(float elapsedTime)
{
    // ��𐬌�
    if(player->GetCompleteDodgeTutorial())
    {
        explainManager.AddDodgeCounts();
    }

}

//--------------------<�����P�̃`���[�g���A��>--------------------//
void SceneTutorial::Energy1Tutorial(float elapsedTime)
{
    // ���[�h��ύX
    if(player->GetCompleteElement1Tutorial())
    {
        explainManager.SetCompleteElement1Tutorial(true);
    }
}




void SceneTutorial::MoveTutorial(float elapsedTime)
{
    // �����v���C���[�������Ă�����
    if (player->GetIsMove())
    {
        explainManager.UpdateMoveTime(elapsedTime);
    }


}


//--------------------<�I�v�V����>--------------------------//
bool SceneTutorial::OptionUpdate(float elapsedTime)
{
    closeOptionInterval -= elapsedTime;
    option->GuiMenu();
    option->Update(elapsedTime);
    IsOpenOption();

    //�^�C�g���ɖ߂�
    if (option->GetIsBackTitle())
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle()));
        return true;
    }
    //���g���C
    if (option->GetIsRetryGame())
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame()));
        return true;
    }

    if (openOption || (closeOptionInterval > 0.0f))return true;

	return false;
}
void SceneTutorial::IsOpenOption()
{
    constexpr float maxInterval = 1.0f;

    if (openOption && !option->GetIsOpenOption())
    {
        openOption = false;
        closeOptionInterval = maxInterval;
        explainManager.PadUpdate();
    }

    if (GamePad::Instance().GetButtonDown() & GamePad::BTN_START)
    {
        if (!(option->GetIsNowLerp()))
        {
            if (openOption)
            {
                openOption = false;
                closeOptionInterval = maxInterval;
                option->Finalize();
                explainManager.PadUpdate();
            }
            else
            {
                openOption = true;
                option->Initialize();
            }
        }
    }
}