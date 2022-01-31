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
    // �֐�
    // 
    //****************************************************************

    // �e�`���[�g���A���̏���
    void MoveTutorial(float elapsedTime);
    void Attacktutorial(float elapsedTime);
    void DodgeTutorial(float elapsedTime);
    void Energy1Tutorial(float elapsedTime);

    //�I�v�V�����֘A�̃A�b�v�f�[�g
    bool OptionUpdate(float elapsedTime);
    //�I�v�V�����̊J��
    void IsOpenOption();

    //--------------------<�J����>--------------------//
    std::unique_ptr<Camera> camera;

    //--------------------<�w�i>--------------------//
    std::unique_ptr<BackGround> backGround{ nullptr };

    //--------------------<�v���C���[>--------------------//
    std::unique_ptr<Player> player;

    //--------------------<�I�v�V����>--------------------//
    std::unique_ptr<Option> option;
    bool openOption{ false };
    float closeOptionInterval{ 0.0f };


    int CurrentTutorial{};

    // ���������Ǘ�����
    ExplainManager explainManager;

    std::unique_ptr<AudioSource> bgm{ nullptr };
};
