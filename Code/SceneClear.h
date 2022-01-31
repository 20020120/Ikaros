#pragma once
#include"Scene.h"
#include "Font.h"
#include "Controller.h"

#include "Camera.h"
#include "Light.h"
#include "ClearPlayer.h"
#include "ClearBack.h"
#include "Renderer.h"

#include<memory>
#include "gAudioSource.h"

class SceneClear :public SceneParent
{
public:
    SceneClear();
    ~SceneClear()override;

    void Initialize(ID3D11Device* device)override;
    void Update(float elapsedTime)override;
    void Render(ID3D11DeviceContext* dc)override;
    void Finalize()override;

    void ChoiceScene(float elapsedTime);
    void ScoreAnnouncement(float elapsedTime);
private:

    void UpdateRanking();
    void CalcRank();

    void UpdateHighScoreFont(float elapsedTime);

    enum NextMenu
    {
        RETRY,
        TITLE
    };

    NextMenu nextMenu;

    enum RANK
    {
	    S_RANK,
        A_RANK,
        B_RANK,
    };

    std::unique_ptr<Font> font;


    Renderer sprChoiceFrame;
    Renderer sprClearLogo{};
    float ratio{};

    DirectX::XMFLOAT2 reTryScale{ 1.0f,1.0f };
    DirectX::XMFLOAT2 goTitleScale{ 1.0f,1.0f };

    DirectX::XMFLOAT2 breakEnemyScale{ 1.0f,1.0f };
    DirectX::XMFLOAT2 maxComboScale{ 1.0f,1.0f };
    DirectX::XMFLOAT2 clearTimeScale{ 1.0f,1.0f };
    DirectX::XMFLOAT2 scoreScale{ 1.0f,1.0f };

    //--------------------<�J����>--------------------//
    std::unique_ptr<Camera> camera;

    // ���C�g
    std::unique_ptr<DirectionLight> directionLight{ nullptr };

    //player
    std::unique_ptr<ClearPlayer> player{ nullptr };
    //ship
    std::unique_ptr<ClearShip> ship{ nullptr };
    //�V��
    std::unique_ptr<ClearSkyDome> skydome;

    //sounds
    std::unique_ptr<AudioSource> bgm;
    std::unique_ptr<AudioSource> seSelect;
    std::unique_ptr<AudioSource> seDicision;
    std::unique_ptr<AudioSource> seDrumroll;
    std::unique_ptr<AudioSource> seAnnounce[2];
    std::unique_ptr<AudioSource> seRank[3];


    // �X�R�A�֘A
    Renderer sprScoreBack{};
    int Score{};           // �X�R�A 
    int ClearTime{};       // �N���A����
    int BreakEnemyValue{}; // �G���j
    int MaxCombo{};        // �ő�R���{
    float Rank{};
    int CurrentScore{}; 
    float ScoreRatio{}; // ���`�⊮�̃��[�g

    int ScoreArray[6];//[�ۑ�����X�R�A��+1]
    Renderer sprScoreRank{};  // �X�R�A�̃A���t�@�x�b�g

    Renderer sprLine{};//�����̉��̐�
    DirectX::XMFLOAT2 linePos[4] = {};
    DirectX::XMFLOAT2 lineScale[4] = {};

    int announcementState = 0;
    bool announcement{ false };
    float announcementTimer = 0.0f;

    bool highScore{ false };//�n�C�X�R�A�X�V�������ǂ���
    float highScoreAlpha{ 0.0f };
    float highScoreColorR{ 0.0f };
};