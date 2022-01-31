#pragma once
#include"Scene.h"
#include"Camera.h"
#include<memory>

#include"BackGround.h"

#include "ExplainManager.h"
#include"Player.h"
#include"Option.h"

#include "gAudioSource.h"

class SceneTutorial final :public SceneParent
{
public:
    SceneTutorial();
    ~SceneTutorial()override;

    void Initialize(ID3D11Device* device)override;
    void Update(float elapsedTime)override;
    void Render(ID3D11DeviceContext* dc)override;
    void Finalize() override;


private:
    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************

    // 各チュートリアルの条件
    void MoveTutorial(float elapsedTime);
    void Attacktutorial(float elapsedTime);
    void DodgeTutorial(float elapsedTime);
    void Energy1Tutorial(float elapsedTime);

    //オプション関連のアップデート
    bool OptionUpdate(float elapsedTime);
    //オプションの開閉
    void IsOpenOption();

    //--------------------<カメラ>--------------------//
    std::unique_ptr<Camera> camera;

    //--------------------<背景>--------------------//
    std::unique_ptr<BackGround> backGround{ nullptr };

    //--------------------<プレイヤー>--------------------//
    std::unique_ptr<Player> player;

    //--------------------<オプション>--------------------//
    std::unique_ptr<Option> option;
    bool openOption{ false };
    float closeOptionInterval{ 0.0f };


    int CurrentTutorial{};

    // 説明文を管理する
    ExplainManager explainManager;

    std::unique_ptr<AudioSource> bgm{ nullptr };
};
