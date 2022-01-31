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
    //--------------------<初期化>--------------------//

    backGround = std::make_unique<BackGround>(device);

    camera = std::make_unique<Camera>(device);

    player = std::make_unique<Player>(device);
    player->Initialize();
    ProjectileManager::Instance().Initialize(device);

    explainManager.Initialize(device);

    option = std::make_unique<Option>(device, Option::MODE::TUTORIAL);
    openOption = false;
    closeOptionInterval = 0.0f;

    // 敵の管理クラスの初期化
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
    
    //何かボタンを押したらゲーム画面へ
#ifdef _DEBUG
    if (GetAsyncKeyState('Z') & 0x8000)
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame()));
        return;
    }

#endif



    //オプションの更新
    if (OptionUpdate(elapsedTime))return;

    explainManager.Update(elapsedTime);
    //****************************************************************
    // 
    // チュートリアルのテキストが表示されているときこれより下は更新されない
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
        // なし
        break;
    case 1:   // Move
         MoveTutorial(elapsedTime);    // 移動のチュートリアル
        break;
    case 2:   // Attack
        Attacktutorial(elapsedTime);  // 攻撃のチュートリアル
        player->ReSetCompleteDodgeTutorial();
        break;
    case 3:  // Dodge
        DodgeTutorial(elapsedTime);   // 回避のチュートリアル
        break;
    case 4:  // Element1
        break;
    case 5:  // Element2
        Energy1Tutorial(elapsedTime); // 変形のチュートリアル
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
    // ここより下は退場演出の時更新されない
    // 
    //****************************************************************


    // 回避成功時にスローになる
    player->HitSlow(elapsedTime);


    // 敵の更新
    EnemyManager::Instance().TutorialUpdate(elapsedTime);
    EnemyManager::Instance().TutorialSpownEnemy(explainManager.GetCurrentTutorialState());
    // カメラをプレイヤーに向ける
 // カメラをプレイヤーに向ける
    camera->SetAttention(player->GetAttentionCamera());
    camera->SetPlayerPos(player->GetPosition());
    camera->f_Gui();
    camera->Update(elapsedTime);
    // プレイヤーの更新
    player->Update(elapsedTime);


    GameSystem::Instance().Update(elapsedTime);

    // 弾の更新
    ProjectileManager::Instance().Update(elapsedTime);
    // エフェクトの更新
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

    // オプション表示
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
    // 射撃で敵を倒したとき
    if (EnemyManager::Instance().GetIsAliveShotTutorialEnemy())
    {
        explainManager.SetCompleteShotTutorial(true);
    }
    // 近接攻撃で敵を倒したとき
    if (player->GetTutorial_IsBreakCloseAttack())
    {
        explainManager.SetCompleteCloseTutorial(true);
    }



}

//--------------------<回避のチュートリアル>--------------------//
void SceneTutorial::DodgeTutorial(float elapsedTime)
{
    // 回避成功
    if(player->GetCompleteDodgeTutorial())
    {
        explainManager.AddDodgeCounts();
    }

}

//--------------------<属性１のチュートリアル>--------------------//
void SceneTutorial::Energy1Tutorial(float elapsedTime)
{
    // モードを変更
    if(player->GetCompleteElement1Tutorial())
    {
        explainManager.SetCompleteElement1Tutorial(true);
    }
}




void SceneTutorial::MoveTutorial(float elapsedTime)
{
    // もしプレイヤーが動いていたら
    if (player->GetIsMove())
    {
        explainManager.UpdateMoveTime(elapsedTime);
    }


}


//--------------------<オプション>--------------------------//
bool SceneTutorial::OptionUpdate(float elapsedTime)
{
    closeOptionInterval -= elapsedTime;
    option->GuiMenu();
    option->Update(elapsedTime);
    IsOpenOption();

    //タイトルに戻る
    if (option->GetIsBackTitle())
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle()));
        return true;
    }
    //リトライ
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