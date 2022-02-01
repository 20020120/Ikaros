#include "SceneGame.h"
#include"EffectManager.h"
#include "EnemyManager.h"
#include "ProjectileManager.h"
#include "GameSystem.h"
#include"MathSupport.h"

#include "SceneLoading.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneOver.h"
#include "SceneClear.h"
#include "gAudio.h"

SceneGame::SceneGame()
{
   
}

SceneGame::~SceneGame()
{

}

void SceneGame::Initialize(ID3D11Device* device)
{
    camera = std::make_unique<Camera>(device);
    GameSystem::Instance().SetCamera(camera.get());
    // �v���C���[�̏�����
    player = std::make_unique<Player>(device);
    player->Initialize();
    player->fSetCameraShakeFunc(camera->mCameraShakeFunc);

    GameSystem::Instance().Initialize(device, player->GetGameSystemData());
    //�G�l�~�[�}�l�[�W���[�̏�����
    EnemyManager::Instance().Initialize(device, EnemyManager::Instance().GetStageIndex());
    EnemyManager::Instance().RegisterTargetPosition(player->GetGameSystemData()->position);
    
    // �w�i
    Back_Ground = std::make_unique<BackGround>(device);
    Back_Ground->Initialize();

    ProjectileManager::Instance().Clear();

    //�I�v�V����
    option = std::make_unique<Option>(device);

    bloom = std::make_unique<Bloom>(device);

    // �f�B���N�V�������C�g��������
    directionLight = std::make_unique<DirectionLight>(device);

    // �Y�[���u���[��������
    mZoomBlur = std::make_unique<ZoomBlur>(device);

    //Sound
    bgmVolume = 0.25f;
    bgm = (Audio::Instance().LoadAudioSource("./resources/Sounds/BGM/SceneGame.wav"));
    bgm->Play(true, OptionSystem::Instance().GetBgmVolume() * bgmVolume);
    bgmBoss = (Audio::Instance().LoadAudioSource("./resources/Sounds/BGM/BossBattle.wav"));
    seAlert = (Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Enemy/AppearBoss.wav"));

    //--------------------<����>--------------------//
    decorationManager = std::make_unique<DecorationManager>(device);

    bossBattle = false;
    //test_effect = std::make_unique<Effect>("./resources/Effects/Mat_03_01/rennsyuu_3.efkefc");
}

void SceneGame::Update(float elapsedTime)
{
    //--------------------<�V�[���؂�ւ�>--------------------//
    if (IsChangeScene())return;

    //--------------------<�ŏ��̉��o>--------------------//
    if(player->GetIsPerforming())
    {
        
        //camera->f_Gui();
        camera->FirstPoseUpdate(elapsedTime);
        camera->SetTarget({ 0.0f,0.0f,-100.0f });
        if(player->GetPerformanceState()>=10)
        {
            camera->SetTarget({ 0.0f,0.0f,player->GetPosition().z });
            GameSystem::CAMERA_DIRECTION Dir = GameSystem::Instance().GetCameraDirection();
            Back_Ground->Update(elapsedTime, static_cast<int>(Dir));
        }

        player->FirstPose(elapsedTime);
        EffectManager::Instance().Update(elapsedTime);

        return;
    }

    //--------------------<�Ō�̉��o>--------------------//
    if (EnemyManager::Instance().GetIsLastPerformance())
    {
        EffectManager::Instance().Update(elapsedTime);

        EnemyManager::Instance().SetLastPerformanceState(player->GetPerformanceState());
        if(player->LastAttack(elapsedTime))
        {
            SceneManager::Instance().ChangeScene(new SceneLoading(new SceneClear));
            return;
        }


        ProjectileManager::Instance().Clear();
        // �J�����̒����_��ݒ�
        camera->UpdateLastPerformance(elapsedTime, player->GetPerformanceState());
        EnemyManager::Instance().Update(elapsedTime);

        return;
    }


	//--------------------<�I�v�V����>--------------------//
    closeOptionFrame = false;

    if(!EnemyManager::Instance().GetCameraFocusBoss())
    {
        IsOpenOption();
        option->GuiMenu();
    }
    option->Update(elapsedTime);
    if (openOption || closeOptionFrame)return;



    // �w�i�X�V
    Back_Ground->f_Gui();
    GameSystem::CAMERA_DIRECTION Dir = GameSystem::Instance().GetCameraDirection();
    Back_Ground->Update(elapsedTime, static_cast<int>(Dir));

    GameSystem::Instance().f_Gui();

    mZoomBlur->fSetZoomPower(EnemyManager::Instance().fGetZoomPower());

    //****************************************************************
     // 
     // ��������̓q�b�g�X�g�b�v�̎��ɓ���
     // 
     //****************************************************************
    player->HitSlow(elapsedTime);
    


    //****************************************************************
    // 
    //  �v���C���[�̕K�E�Z
    // 
    //****************************************************************

    // �J�������v���C���[�Ɍ�����
    camera->SetAttention(player->GetAttentionCamera());

    
    camera->SetPlayerPos(player->GetPosition());

    camera->f_Gui();
    camera->Update(elapsedTime);

    EffectManager::Instance().Update(elapsedTime);


    GameSystem::Instance().Update(elapsedTime);
   
    // �J�����ɒ��ڂ����Ă���Ƃ��͍X�V���Ȃ�
    if (!player->GetAttentionCamera() && !player->GetEnemiesStop())
    {
        //--------------------<�G�l�~�[�̍X�V����>--------------------//
        EnemyManager::Instance().Update(elapsedTime);

        //--------------------<�e�̍X�V����>--------------------//
        ProjectileManager::Instance().SetTarget(player->GetPosition());
        ProjectileManager::Instance().Update(elapsedTime);
    }


    
    camera->SetAttentionBoss(EnemyManager::Instance().GetCameraFocusBoss());

    DirectX::XMFLOAT3 BossPosition = EnemyManager::Instance().GetTargetPosition();
    BossPosition.z += 30.0f;
    camera->SetBossPosition(BossPosition);

    if (!EnemyManager::Instance().GetCameraFocusBoss())
    {

        if (camera->GetChangeCameraDir() && camera->GetChangeCameraDirState() == 1)
        {
            player->ChangeCameraDirCorrect();
        }
    }

    //�{�X�o�g����BGM�ς���
    ChangeBgm(elapsedTime);
    

    //--------------------<����>--------------------//
    decorationManager->Update(elapsedTime);
 


 

    //--------------------<�v���C���[�̍X�V����>--------------------//
    // �{�X�ɒ��ڂ��Ă���Ԃ̓v���C���[�𑀍�ł��Ȃ�
    if (!EnemyManager::Instance().GetCameraFocusBoss())
    {
        player->Update(elapsedTime);
    }

    else
    {
        DirectX::XMFLOAT3 player_pos = { 0.0f,0.0f,0.0f };
        player_pos.z = -20.0f;
        player->SetPosition(player_pos);
    }





    //�V�[���J�ڊm�F�p
    GuiMenu();
}

void SceneGame::Render(ID3D11DeviceContext* dc)
{
    //--------------------<�萔�o�b�t�@�̃X���b�g�P�̓J����>--------------------//
    camera->f_CameraUpdate(dc);

    //--------------------<�萔�o�b�t�@�̃X���b�g�Q�̓f�B���N�V�������C�g>--------------------//
    directionLight->Update(dc);


    //bloom->BeginBloom(dc);
    mZoomBlur->fBegin(dc);

    //--------------------<���f���̕`��>--------------------//
    Back_Ground->Render(dc);
    
    if (!EnemyManager::Instance().GetCameraFocusBoss())
    {
        player->Render(dc);
    }

    //--------------------<����>--------------------//
    decorationManager->Render(dc);



    EnemyManager::Instance().Render(dc);
    
    
    ProjectileManager::Instance().Render(dc);
    
    EffectManager::Instance().Render(camera->GetView(), camera->GetProjection());
    
    mZoomBlur->fEnd(dc);
   //bloom->EndBloom(dc);


    //--------------------<�X�v���C�g�̕`��>--------------------//
    bool draw{false};
    // �v���C���[�̍ŏ��̉��o���̘g�`��
    if(player->GetIsPerforming()||player->GetIsFinisher()|| EnemyManager::Instance().GetCameraFocusBoss()||EnemyManager::Instance().GetIsLastPerformance())
    {
        draw = true;
    }
    player->Render2D(dc,draw);
    GameSystem::Instance().SetFrameDraw(draw);
    EnemyManager::Instance().Render2D(dc);

    if(draw)
    {
        GameSystem::Instance().PerformRender(dc);
        return;
    }

    GameSystem::Instance().Render(dc);
    
    option->Render(dc);
    
}

void SceneGame::Finalize()
{
    //�J�������������������Ă���
    GameSystem::Instance().SetCameraDirection(GameSystem::CAMERA_DIRECTION::SIDE);

    GameSystem::Instance().Finalize();

    //audio���
    bgm->Release();
    bgmBoss->Release();
    seAlert->Release();

    player->Finalize();

    option->ReleaseSounds();

    //--------------------<�e�}�l�[�W���������>--------------------//
    EnemyManager::Instance().Finalize();
    ProjectileManager::Instance().Finalize();
    EffectManager::Instance().GetEffekseerManager()->StopAllEffects();
}


//--------------------<�I�v�V����>--------------------//
void SceneGame::IsOpenOption()
{
    auto gamepad = GamePad::Instance();

    if (openOption && !option->GetIsOpenOption())
    {
        openOption = false;
        closeOptionFrame = true;
        GameSystem::Instance().GamePadUpdate();
    }

    if (gamepad.GetButtonDown() & GamePad::BTN_START)//C
    {
        if (!(option->GetIsNowLerp()))
        {
            if (openOption)
            {
                openOption = false;
                closeOptionFrame = true;
                option->Finalize();
                GameSystem::Instance().GamePadUpdate();
            }
            else
            {
                openOption = true;
                option->Initialize();
            }
        }
    }

    bgm->SetVolume(OptionSystem::Instance().GetBgmVolume() * 0.25f);

}

//--------------------<�V�[���؂�ւ�>--------------------//
bool SceneGame::IsChangeScene()
{
    //�Q�[���N���A

    
    //�Q�[���I�[�o�[
	if(!(player->GetIsAlive()))
	{
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneOver()));
        return true;
	}
    //�^�C�g���ɖ߂�
    if(option->GetIsBackTitle())
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle()));
        return true;
    }
    //���g���C
    if(option->GetIsRetryGame())
    {
        if(EnemyManager::Instance().GetIsBossBattle())
        {
            GameSystem::Instance().SetRetry();
        }
        
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame()));
        return true;
    }
    return false;
}

//--------------------<BGM�؂�ւ�>--------------------//
void SceneGame::ChangeBgm(float elapsedTime)
{
    if (EnemyManager::Instance().GetStartBossBGM())
    {
        seAlert->Play(false, OptionSystem::Instance().GetSeVolume() * 0.5f);
        bossBattle = true;
    }

    if(bossBattle)
    {
        bgmVolume -= 0.001f;
        if (bgmVolume > 0.0f)
        {
            bgm->SetVolume(OptionSystem::Instance().GetBgmVolume() * bgmVolume);
        }
        else
        {
            bgm->Stop();
            bgmBoss->Play(true, OptionSystem::Instance().GetBgmVolume() * 0.3f);
        }
    }
}


//--------------------<IMGUI>--------------------//
void SceneGame::GuiMenu()
{
#if _DEBUG
    ImGui::Begin("SceneGame");

    if (ImGui::Button("GameClear"))
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneClear()));
    }
    if (ImGui::Button("GameOver"))
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneOver()));
    }

    if (ImGui::Button("play"))
    {
        test_effect->Play({ 0.0f,0.0f,0.0f });
    }
    ImGui::End();
#endif
}
