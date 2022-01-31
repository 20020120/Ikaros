#pragma once
#include"Scene.h"
#include "Font.h"

#include<memory>

#include "Controller.h"
#include"Renderer.h"
#include "Light.h"
#include "Camera.h"

#include"GameOverBoss.h"
#include "OverPlayer.h"
#include "gAudioSource.h"

class SceneOver :public SceneParent
{
public:
    SceneOver();
    ~SceneOver()override;

    void Initialize(ID3D11Device* device)override;
    void Update(float elapsedTime)override;
    void Render(ID3D11DeviceContext* dc)override;
    void Finalize()override;

private:
    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************
    void ChoiceScene(float elapsedTime);
    void LogoUpdate(float elapsedTime);


    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************

    enum NextMenu
    {
        RETRY,
        TITLE
    };

    NextMenu nextMenu;
    std::unique_ptr<Font> font;

    Renderer SprChoiceFrame;
    Renderer SprLogo;
    Renderer SprBlackOut;
    bool blackOut{ false };
    bool backTitle{ false };

    float Ratio{};

    DirectX::XMFLOAT2 StrReTryScale{ 1.0f,1.0f };
    DirectX::XMFLOAT2 StrGoTitleScale{ 1.0f,1.0f };


    float StackTime = 0.0f;
    float scrollTime = 0.0f;

    //--------------------<カメラ>--------------------//
    std::unique_ptr<Camera> camera;

    // ライト
    std::unique_ptr<DirectionLight> directionLight{ nullptr };

    // ゲームオーバーのボス
    //std::unique_ptr<GameOverBoss> Boss;

    //player
    std::unique_ptr<OverPlayer> player;

    std::unique_ptr<AudioSource> bgm{ nullptr };
    std::unique_ptr<AudioSource> seSelect{ nullptr };
    std::unique_ptr<AudioSource> seSandstorm{ nullptr };
    float sandstormVolume = 0.1f;
    bool playSE = false;

};
