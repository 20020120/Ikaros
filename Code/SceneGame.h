#pragma once
#include"Scene.h"
#include"Camera.h"
#include<memory>

#include "BackGround.h"
#include "Player.h"

#include "Controller.h"
#include "Option.h"
#include"Bloom.h"
#include"Light.h"
#include"Decoration.h"
#include"ZoomBlur.h"
#include "gAudioSource.h"

class SceneGame :public SceneParent
{
public:
    SceneGame();
    ~SceneGame()override;

    void Initialize(ID3D11Device* device)override;
    void Update(float elapsedTime)override;
    void Render(ID3D11DeviceContext* dc)override;
    void Finalize() override;


    //--------------------<オプション>--------------------//
    void IsOpenOption();
    //--------------------<シーン切り替え>--------------------//
    bool IsChangeScene();
    //--------------------<BGMきりかえ>--------------------//
    void ChangeBgm(float elapsedTime);
    //--------------------<IMGUI>--------------------//
    void GuiMenu();

private:
    //--------------------<カメラ>--------------------//
    std::unique_ptr<Camera> camera;

    // プレイヤー
    std::unique_ptr<Player> player;

    std::unique_ptr<DirectionLight> directionLight{nullptr};
    
    std::unique_ptr<Effect> test_effect;
    
    std::unique_ptr<BackGround> Back_Ground;

    std::unique_ptr<Bloom> bloom;

    std::unique_ptr<AudioSource> bgm{ nullptr };
    std::unique_ptr<AudioSource> bgmBoss{ nullptr };
    std::unique_ptr<AudioSource> seAlert{ nullptr };

    std::unique_ptr<DecorationManager> decorationManager;  // 装飾クラス

    //--------------------<オプション>--------------------//
    bool openOption{ false };//オプションを開いているかどうか
    bool closeOptionFrame{ false };//オプションを閉じたフレームかどうか

    bool bossBattle{ false };
    float bgmVolume = 0.25f;//通常時のBGMの音量
    
    std::unique_ptr<Option> option;

    std::unique_ptr<ZoomBlur> mZoomBlur{};
};


