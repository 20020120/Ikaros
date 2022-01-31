#pragma once
#include"Scene.h"
#include <memory>
#include "Font.h"
#include"ClearBack.h"
#include"Camera.h"
#include"Renderer.h"
#include "gAudioSource.h"

class SceneRanking final :public SceneParent
{
public:
    SceneRanking();
    ~SceneRanking() override;
    void Initialize(ID3D11Device* device) override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;

private:
    void InputUpdate();
    void OpenRanking();
    void CircleAnimation(float elapsedTime);
    void CircleAnimationMini(float elapsedTime);

    //--------------------<構造体>--------------------//
    struct AnimParam
    {
        float timer = 0.0f;
        float maxTimer = 1.0f;
        int state = 0;
        float targetScale = 1.0f;
        float beginAngle = 0.0f;
        float targetAngle = 0.0f;
        float ratio = 0.0f;
    };
    AnimParam Circles[3];
    AnimParam CirclesMini[2];

    int ScoreArray[5][6]; // スコアの配列

    int stageIndex = 0;

    std::unique_ptr<Font> font;
    //std::unique_ptr<ClearShip> Ship{};
    //std::unique_ptr<ClearSkyDome>Dome{};
    //std::unique_ptr<Camera> camera{};


    //--------------------<スプライト>--------------------//
    Renderer sprBack{};
    Renderer sprRankingFont{};
    Renderer sprBlackOut{};
    Renderer sprCircle[5]{};

    DirectX::XMFLOAT2 fontPos;
    DirectX::XMFLOAT2 fontScale;
    //--------------------<スプライト>--------------------//
    std::unique_ptr<AudioSource> bgm;
    std::unique_ptr<AudioSource> seSelect;
    std::unique_ptr<AudioSource> seDecision;



    float inputInterval{ 0.0f };
    const float MaxInputInterval = 0.5f;

    bool backTitle{ false };
    float blackOutRatio = { 1.0f };

    float scoreAlpha = 0.0f;

    
};
