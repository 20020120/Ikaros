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
    // プレイヤーの初期化
    player = std::make_unique<Player>(device);
    player->Initialize();
    player->fSetCameraShakeFunc(camera->mCameraShakeFunc);

    GameSystem::Instance().Initialize(device, player->GetGameSystemData());
    //エネミーマネージャーの初期化
    EnemyManager::Instance().Initialize(device, EnemyManager::Instance().GetStageIndex());
    EnemyManager::Instance().RegisterTargetPosition(player->GetGameSystemData()->position);
    
    // 背景
    Back_Ground = std::make_unique<BackGround>(device);
    Back_Ground->Initialize();

    ProjectileManager::Instance().Clear();

    //オプション
    option = std::make_unique<Option>(device);

    bloom = std::make_unique<Bloom>(device);

    // ディレクションライトを初期化
    directionLight = std::make_unique<DirectionLight>(device);

    // ズームブラーを初期化
    mZoomBlur = std::make_unique<ZoomBlur>(device);

    //Sound
    bgmVolume = 0.25f;
    bgm = (Audio::Instance().LoadAudioSource("./resources/Sounds/BGM/SceneGame.wav"));
    bgm->Play(true, OptionSystem::Instance().GetBgmVolume() * bgmVolume);
    bgmBoss = (Audio::Instance().LoadAudioSource("./resources/Sounds/BGM/BossBattle.wav"));
    seAlert = (Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Enemy/AppearBoss.wav"));

    //--------------------<装飾>--------------------//
    decorationManager = std::make_unique<DecorationManager>(device);

    bossBattle = false;
    //test_effect = std::make_unique<Effect>("./resources/Effects/Mat_03_01/rennsyuu_3.efkefc");
}

void SceneGame::Update(float elapsedTime)
{
    //--------------------<シーン切り替え>--------------------//
    if (IsChangeScene())return;

    //--------------------<最初の演出>--------------------//
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

    //--------------------<最後の演出>--------------------//
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
        // カメラの注視点を設定
        camera->UpdateLastPerformance(elapsedTime, player->GetPerformanceState());
        EnemyManager::Instance().Update(elapsedTime);

        return;
    }


	//--------------------<オプション>--------------------//
    closeOptionFrame = false;

    if(!EnemyManager::Instance().GetCameraFocusBoss())
    {
        IsOpenOption();
        option->GuiMenu();
    }
    option->Update(elapsedTime);
    if (openOption || closeOptionFrame)return;



    // 背景更新
    Back_Ground->f_Gui();
    GameSystem::CAMERA_DIRECTION Dir = GameSystem::Instance().GetCameraDirection();
    Back_Ground->Update(elapsedTime, static_cast<int>(Dir));

    GameSystem::Instance().f_Gui();

    mZoomBlur->fSetZoomPower(EnemyManager::Instance().fGetZoomPower());

    //****************************************************************
     // 
     // ここより上はヒットストップの時に動く
     // 
     //****************************************************************
    player->HitSlow(elapsedTime);
    


    //****************************************************************
    // 
    //  プレイヤーの必殺技
    // 
    //****************************************************************

    // カメラをプレイヤーに向ける
    camera->SetAttention(player->GetAttentionCamera());

    
    camera->SetPlayerPos(player->GetPosition());

    camera->f_Gui();
    camera->Update(elapsedTime);

    EffectManager::Instance().Update(elapsedTime);


    GameSystem::Instance().Update(elapsedTime);
   
    // カメラに注目させているときは更新しない
    if (!player->GetAttentionCamera() && !player->GetEnemiesStop())
    {
        //--------------------<エネミーの更新処理>--------------------//
        EnemyManager::Instance().Update(elapsedTime);

        //--------------------<弾の更新処理>--------------------//
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

    //ボスバトルはBGM変える
    ChangeBgm(elapsedTime);
    

    //--------------------<装飾>--------------------//
    decorationManager->Update(elapsedTime);
 


 

    //--------------------<プレイヤーの更新処理>--------------------//
    // ボスに注目している間はプレイヤーを操作できない
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





    //シーン遷移確認用
    GuiMenu();
}

void SceneGame::Render(ID3D11DeviceContext* dc)
{
    //--------------------<定数バッファのスロット１はカメラ>--------------------//
    camera->f_CameraUpdate(dc);

    //--------------------<定数バッファのスロット２はディレクションライト>--------------------//
    directionLight->Update(dc);


    //bloom->BeginBloom(dc);
    mZoomBlur->fBegin(dc);

    //--------------------<モデルの描画>--------------------//
    Back_Ground->Render(dc);
    
    if (!EnemyManager::Instance().GetCameraFocusBoss())
    {
        player->Render(dc);
    }

    //--------------------<装飾>--------------------//
    decorationManager->Render(dc);



    EnemyManager::Instance().Render(dc);
    
    
    ProjectileManager::Instance().Render(dc);
    
    EffectManager::Instance().Render(camera->GetView(), camera->GetProjection());
    
    mZoomBlur->fEnd(dc);
   //bloom->EndBloom(dc);


    //--------------------<スプライトの描画>--------------------//
    bool draw{false};
    // プレイヤーの最初の演出中の枠描画
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
    //カメラ向きを初期化しておく
    GameSystem::Instance().SetCameraDirection(GameSystem::CAMERA_DIRECTION::SIDE);

    GameSystem::Instance().Finalize();

    //audio解放
    bgm->Release();
    bgmBoss->Release();
    seAlert->Release();

    player->Finalize();

    option->ReleaseSounds();

    //--------------------<各マネージャ解放処理>--------------------//
    EnemyManager::Instance().Finalize();
    ProjectileManager::Instance().Finalize();
    EffectManager::Instance().GetEffekseerManager()->StopAllEffects();
}


//--------------------<オプション>--------------------//
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

//--------------------<シーン切り替え>--------------------//
bool SceneGame::IsChangeScene()
{
    //ゲームクリア

    
    //ゲームオーバー
	if(!(player->GetIsAlive()))
	{
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneOver()));
        return true;
	}
    //タイトルに戻る
    if(option->GetIsBackTitle())
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle()));
        return true;
    }
    //リトライ
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

//--------------------<BGM切り替え>--------------------//
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
