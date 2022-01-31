#pragma once

#include"Font.h"
#include"Renderer.h"
#include<d3d11.h>
#include "Controller.h"
#include "Option.h"
//****************************************************************
// 
//  �`���[�g���A���̐����N���X
// 
//****************************************************************

class ExplainManager final
{
public:
    ExplainManager(){}
    ~ExplainManager() {}

    void Initialize(ID3D11Device* d);
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);
    void Finalize();


    //****************************************************************
    // 
    // �N���X�O����A�N�Z�X����郁���o�֐�
    // 
    //****************************************************************

    void UpdateMoveTime(float time); // �v���C���[�������Ă��鎞��
    bool GetShowTutorialText() const { return ShowTutorialText; }// �`���[�g���A���̃e�L�X�g���\������Ă��邩�ǂ���

    void SetCompleteShotTutorial(bool a) { CompleteShotTutorial = a; }// �ˌ��œG��|������
    void SetCompleteCloseTutorial(bool a) { CompleteSwordTutorial = a; }// �ߐڍU���œG��|������
    void SetCompleteDodgeTutorial(bool a) { CompleteDodge = a; }        // ����̃`���[�g���A�����I������
    void SetCompleteElement1Tutorial(bool a) { CompleteModeChange = a; }        // �ό`�̃`���[�g���A�����I������
    void AddDodgeCounts() { DodgeCounts++; }

    bool GetPermissionPerformancePlayer()const { return  PermissionPerformancePlayer; }

    int GetCurrentTutorialState() const { return CurrentTutorialState; }
    void PadUpdate();
private:
    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
    void GuiMenu();
    void InitializeCondition(); // �e�������Ƃɏ�����

    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************

    // �I�v�V����
    std::unique_ptr<Option> option;
    bool openOption{ false };//�I�v�V�������J���Ă��邩�ǂ���
    bool closeOption{ false };//�I�v�V��������Ă��邩�ǂ���

    bool ShowTutorialText; // �`���[�g���A���̉摜��\�����邩�ǂ���

    float Checkmark1DissolveRate{};
    float Checkmark2DissolveRate{};

    enum Device
    {
        KeyBoard,
        Controller,
    }InputDevice{};
    float InputRatio{};
    int InputState{};

    // �X�v���C�g
    Renderer sprController;
    Renderer sprKeyBord;
    Renderer sprBackFrame;
    Renderer sprChoiceFrame;

    enum TutorialState // �`���[�g���A���̎��
    {
        None,     // �Ȃ�
        Move,     // �ړ�
        Attack,   // �U��
        Dodge,    // ���
        Element1, // �����P 
        Element2, // �����Q
        End,      // �I��
        Value,    // ��
    };

    int CurrentTutorialState{ None }; // ���݂̃`���[�g���A���̎��

    //--------------------<�e�`���[�g���A���̏����Ɋւ���ϐ�>--------------------//

    float PlayerMoveTime{};       // �v���C���[���ړ����Ă��鎞��
    bool CompleteShotTutorial{};  // �ˌ��œG��|�������ǂ���
    bool CompleteSwordTutorial{}; // �ߐڍU���œG��|�������ǂ���
    bool CompleteDodge{};         // ��𐬌�
    bool PreferGoElement2{};      // ���̃`���[�g���A���ɍs�����Ƃ�����
    bool CompleteModeChange{};    // �ό`����
    bool EndTutorial{};           // �`���[�g���A���I��

    int DodgeCounts{}; // �`���[�g���A���̉����

    bool PermissionPerformancePlayer{}; // �v���C���[���ޏ�̉��o�����邱�Ƃ�������
    bool DecideInputDevice{};

    float StackNextTutorialWaitSecond{}; // ���̃`���[�g���A���ֈڍs����܂ł̎���
    const float NextTutorialWaitSeconds{ 1.0f }; // ���̃`���[�g���A���ւ̈ȍ~���ԁi�b�j

    //--------------------<�`��Ɋւ���ϐ�>--------------------//

    std::unique_ptr<Font> font{};  // �t�H���g
    Renderer spr_CheckBox1{};   //�`�F�b�N�{�b�N�X
    Renderer spr_CheckBox2{};
    Renderer spr_CheckMark1{};  // �`�F�b�N�}�[�N
    Renderer spr_CheckMark2{};
    Renderer spr_TextBack{}; // �e�L�X�g�̔w�i

    //--------------------<�L�[�{�[�h>--------------------//
    Renderer spr_MoveExp{};     // �ړ��̃`���[�g���A���̉摜
    Renderer spr_AttackExp{};   // �U���̃`���[�g���A���̉摜
    Renderer spr_DodgeExp{};    // ����̃`���[�g���A���̉摜
    Renderer spr_Element1Exp{}; // �����P�̃`���[�g���A���̉摜
    Renderer spr_Element2Exp{}; // �����Q�̃`���[�g���A���̉摜


    //--------------------<�R���g���[���[>--------------------//
    Renderer spr_MoveExp2{};     // �ړ��̃`���[�g���A���̉摜
    Renderer spr_AttackExp2{};   // �U���̃`���[�g���A���̉摜
    Renderer spr_DodgeExp2{};    // ����̃`���[�g���A���̉摜
    Renderer spr_Element1Exp2{}; // �����P�̃`���[�g���A���̉摜
    Renderer spr_Element2Exp2{}; // �����Q�̃`���[�g���A���̉摜


    Renderer sprBackButton{};      // �R���g���[���[�̃o�b�N�{�^��
    Renderer sprBackButtonFrame{}; // �R���g���[���[�̃o�b�N�{�^��
    Renderer sprStartButton{};     // �X�^�[�g�{�^��

    //--------------------<�f�o�b�O�p>--------------------//
    DirectX::XMFLOAT2 DebugPosition;
    DirectX::XMFLOAT2 DebugScale;

    //--------------------<�Q�[���p�b�h����>--------------------//
    float StackNoInputSeconds; // �A�łōs���߂��Ȃ��悤���΂炭����𖳌�������
    const float NoInputSeconds = 0.5f; // �i�b�j

    // �Q�[���I���t���O
    float StackaEndGameSeconds{};  // �Q�[���I���܂ł̎���

    // �T�E���h
    bool IsPlaySound{};
    bool IsPlaySound2{};
    std::unique_ptr<AudioSource> se_Check{};



    //****************************************************************
    // 
    // �֐��|�C���^
    // 
    //****************************************************************

    // ���̃`���[�g���A���ɑJ�ڂ���������Ǘ�����֐��|�C���^
    using pCondition = bool(ExplainManager::*)(float);

    // ���̃`���[�g���A���ɑJ�ڂ�������̊֐�
    pCondition NextConditionFuncArray[Value]{};

    bool ConditionNoneToMove(float elapsedTime);
    bool ConditionMoveToAttack(float elapsedTime);       // �ړ��̃`���[�g���A������U���̃`���[�g���A��
    bool ConditionAttackToDodge(float elapsedTime);      // �U���̃`���[�g���A���������̃`���[�g���A��
    bool ConditionDodgeToElement1(float elapsedTime);    // ����̃`���[�g���A�����瑮���P�̃`���[�g���A��
    bool ConditionElement1ToElement2(float elapsedTime); //�����P�̃`���[�g���A�����瑮���Q�̃`���[�g���A��
    bool ConditionElement2ToEnd(float elapsedTime);      //����2�̃`���[�g���A������I��
    bool ConditionEndToGame(float elapsedTime);          
        
    // ���݂̃`���[�g���A���ɉ����ĕ\����ς���
    using pRenderExp = void(ExplainManager::*)(ID3D11DeviceContext* dc);

    pRenderExp RenderArray[Value]{};

    void RenderNoneExp(ID3D11DeviceContext* dc);   
    void RenderMoveExp(ID3D11DeviceContext* dc);     // �ړ��̃`���[�g���A��
    void RenderAttackExp(ID3D11DeviceContext* dc);   // �U���̃`���[�g���A��
    void RenderDodgeExp(ID3D11DeviceContext* dc);    // ����̃`���[�g���A��
    void RenderElement1Exp(ID3D11DeviceContext* dc); // �����P�̃`���[�g���A��
    void RenderElement2Exp(ID3D11DeviceContext* dc); // �����Q�̃`���[�g���A��
    void RenderLastExp(ID3D11DeviceContext* dc);     // �Ō�̃`���[�g���A��
    

};


