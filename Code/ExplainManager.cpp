#include "ExplainManager.h"

#include <algorithm>
#include<imgui.h>

#include "SceneGame.h"
#include"SceneManager.h"
#include "gAudio.h"
#include"OptionSystem.h"


void ExplainManager::Initialize(ID3D11Device* d)
{
    //****************************************************************
    // 
    //  ������
    // 
    //****************************************************************
    CurrentTutorialState = None;
    ShowTutorialText = true;
    CompleteShotTutorial = false;
    CompleteSwordTutorial = false;
    PreferGoElement2 = false;
    StackaEndGameSeconds = 0.0f;
    InputState = 1;
    InputRatio = 1.0f;

    //--------------------<�X�v���C�g�̏�����>--------------------//
    spr_CheckBox1.Initialize(d, L"./resources/Sprite/UIs/CheckBox.png");
    spr_CheckBox1.TexSize = { 256.0f,256.0f };
    spr_CheckBox1.Pivot = { 128.0f,128.0f };
    spr_CheckBox1.Scale = { 0.15f,0.15f };
    spr_CheckBox1.Position = { 850.0f,60.0f };

    spr_CheckBox2.Initialize(d, L"./resources/Sprite/UIs/CheckBox.png");
    spr_CheckBox2.TexSize = { 256.0f,256.0f };
    spr_CheckBox2.Pivot = { 128.0f,128.0f };
    spr_CheckBox2.Scale = { 0.15f,0.15f };
    spr_CheckBox2.Position = { 850.0f,120.0f };

    spr_CheckMark1.Initialize(d, L"./resources/Sprite/UIs/CheckMarkMask.png",L"./resources/Sprite/Shaders/dissolve2.png", L"./resources/Sprite/UIs/CheckMark.png");

    spr_CheckMark1.TexSize = { 256.0f,256.0f };
    spr_CheckMark1.Pivot = { 128.0f,128.0f };
    spr_CheckMark1.Scale = { 0.15f,0.15f };
    spr_CheckMark1.Position = { 850.0f,60.0f };

    spr_CheckMark2.Initialize(d, L"./resources/Sprite/UIs/CheckMarkMask.png", L"./resources/Sprite/Shaders/dissolve2.png", L"./resources/Sprite/UIs/CheckMark.png");
    spr_CheckMark2.TexSize = { 256.0f,256.0f };
    spr_CheckMark2.Pivot = { 128.0f,128.0f };
    spr_CheckMark2.Scale = { 0.15f,0.15f };
    spr_CheckMark2.Position = { 850.0f,120.0f };

    spr_TextBack.Initialize(d, L"./resources/Sprite/UIs/TutorialFrame.png");
    spr_TextBack.TexSize = { 600.0f,200.0f };
    spr_TextBack.Pivot = { 300.0f,100.0f };
    spr_TextBack.Position = { 1030.0f,90.0f };
    spr_TextBack.Scale = { 0.8f,0.8f };

    sprBackButton.Initialize(d, L"./resources/Sprite/UIs/Controller_BackBTN.png");
    sprBackButton.Position = { 920.0f,60.0f };
    sprBackButton.Scale = { 0.2f,0.2f };
    sprBackButton.Pivot = { 128.0f,128.0f };
    sprBackButton.TexSize = { 256.0f,256.0f };
    sprBackButton.Color = { 0.0f,1.0f,1.0f,1.0f };

    sprBackButtonFrame.Initialize(d, L"./resources/Sprite/UIs/Controller_BTNCircle.png");
    sprBackButtonFrame.Position = { 920.0f,60.0f };
    sprBackButtonFrame.Scale = { 0.2f,0.2f };
    sprBackButtonFrame.Pivot = { 128.0f,128.0f };
    sprBackButtonFrame.TexSize = { 256.0f,0.0f };
    sprBackButtonFrame.TexPos = { 0.0f,256.0f };

    sprStartButton.Initialize(d, L"./resources/Sprite/UIs/Controller_StartBTN.png");
    sprStartButton.Position = { 420.0f,65.0f };
    sprStartButton.Scale = { 0.2f,0.2f };
    sprStartButton.Pivot = { 128.0f,128.0f };
    sprStartButton.TexSize = { 256.0f,256.0f };

    //--------------------<�����摜�̏�����>--------------------//
    spr_MoveExp.Initialize(d, L"./resources/Sprite/Tutorial/Keyboard/MoveTutorial.png");
    spr_MoveExp.TexSize = { 1920.0f,1080.0f };
    spr_MoveExp.Pivot = { 1920.0f * 0.5f,1080.0f * 0.5f };
    spr_MoveExp.Position = { 640.0,360.0f };
    spr_MoveExp.Scale = { 0.0f,0.0f };

    spr_AttackExp.Initialize(d, L"./resources/Sprite/Tutorial/Keyboard/AttackTutorial.png");
    spr_AttackExp.TexSize = { 1920.0f,1080.0f };
    spr_AttackExp.Pivot = { 1920.0f * 0.5f,1080.0f * 0.5f };
    spr_AttackExp.Position = { 640.0,360.0f };
    spr_AttackExp.Scale = { 0.0f,0.0f };

    spr_DodgeExp.Initialize(d, L"./resources/Sprite/Tutorial/Keyboard/DodgeTutorial.png");
    spr_DodgeExp.TexSize = { 1920.0f,1080.0f };
    spr_DodgeExp.Pivot = { 1920.0f * 0.5f,1080.0f * 0.5f };
    spr_DodgeExp.Position = { 640.0,360.0f };
    spr_DodgeExp.Scale = { 0.0f,0.0f };

    spr_Element1Exp.Initialize(d, L"./resources/Sprite/Tutorial/Keyboard/Attribute1Tutorial.png");
    spr_Element1Exp.TexSize = { 1920.0f,1080.0f };
    spr_Element1Exp.Pivot = { 1920.0f * 0.5f,1080.0f * 0.5f };
    spr_Element1Exp.Position = { 640.0,360.0f };
    spr_Element1Exp.Scale = { 0.0f,0.0f };

    spr_Element2Exp.Initialize(d, L"./resources/Sprite/Tutorial/Keyboard/Attribute2Tutorial.png");
    spr_Element2Exp.TexSize = { 1920.0f,1080.0f };
    spr_Element2Exp.Pivot = { 1920.0f * 0.5f,1080.0f * 0.5f };
    spr_Element2Exp.Position = { 640.0,360.0f };
    spr_Element2Exp.Scale = { 0.0f,0.0f };

    //*******************************************************************//
    spr_MoveExp2.Initialize(d, L"./resources/Sprite/Tutorial/Controller/MoveTutorial.png");
    spr_MoveExp2.TexSize = { 1920.0f,1080.0f };
    spr_MoveExp2.Pivot = { 1920.0f * 0.5f,1080.0f * 0.5f };
    spr_MoveExp2.Position = { 640.0,360.0f };
    spr_MoveExp2.Scale = { 0.0f,0.0f };

    spr_AttackExp2.Initialize(d, L"./resources/Sprite/Tutorial/Controller/AttackTutorial.png");
    spr_AttackExp2.TexSize = { 1920.0f,1080.0f };
    spr_AttackExp2.Pivot = { 1920.0f * 0.5f,1080.0f * 0.5f };
    spr_AttackExp2.Position = { 640.0,360.0f };
    spr_AttackExp2.Scale = { 0.0f,0.0f };

    spr_DodgeExp2.Initialize(d, L"./resources/Sprite/Tutorial/Controller/DodgeTutorial.png");
    spr_DodgeExp2.TexSize = { 1920.0f,1080.0f };
    spr_DodgeExp2.Pivot = { 1920.0f * 0.5f,1080.0f * 0.5f };
    spr_DodgeExp2.Position = { 640.0,360.0f };
    spr_DodgeExp2.Scale = { 0.0f,0.0f };

    spr_Element1Exp2.Initialize(d, L"./resources/Sprite/Tutorial/Controller/Attribute1Tutorial.png");
    spr_Element1Exp2.TexSize = { 1920.0f,1080.0f };
    spr_Element1Exp2.Pivot = { 1920.0f * 0.5f,1080.0f * 0.5f };
    spr_Element1Exp2.Position = { 640.0,360.0f };
    spr_Element1Exp2.Scale = { 0.0f,0.0f };

    spr_Element2Exp2.Initialize(d, L"./resources/Sprite/Tutorial/Controller/Attribute2Tutorial.png");
    spr_Element2Exp2.TexSize = { 1920.0f,1080.0f };
    spr_Element2Exp2.Pivot = { 1920.0f * 0.5f,1080.0f * 0.5f };
    spr_Element2Exp2.Position = { 640.0,360.0f };
    spr_Element2Exp2.Scale = { 0.0f,0.0f };




    sprController.Initialize(d, L"./resources/Sprite/Option/ControllerIcon.png");
    sprController.Position = { 840.0f,360.0f };
    sprController.TexSize = { 190.0f,150.0f };
    sprController.Pivot = { 95.0f,75.0f };
    sprController.Scale = { 1.0f,1.0f };


    sprKeyBord.Initialize(d, L"./resources/Sprite/UIs/KeyboardIcon.png");
    sprKeyBord.Position = {400.0f,360.0f};
    sprKeyBord.TexSize = { 190.0f,150.0f };
    sprKeyBord.Pivot = { 95.0f,75.0f };
    sprKeyBord.Scale = { 1.0f,1.0f };

    sprBackFrame.Initialize(d, L"./resources/Sprite/Option/optionBack.png");
    sprBackFrame.Position = { 640.0f,360.0f };
    sprBackFrame.TexSize = { 1280.0f,720.0f };
    sprBackFrame.Pivot = { 640.0f,360.0f };
    sprBackFrame.Scale = { 1.0f,1.0f };

    sprChoiceFrame.Initialize(d, L"./resources/Sprite/Option/Frame.png");
    sprChoiceFrame.Position = { 400.0f,360.0f };
    sprChoiceFrame.TexSize = { 440.0f,260.0f };
    sprChoiceFrame.Pivot = { 220.0f,130.0f };
    sprChoiceFrame.Scale = { 1.0f,1.0f };

    se_Check = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/tutorial_clear.wav");

    //--------------------<�֐��|�C���^�̏�����>--------------------//

    // �`��֐��|�C���^�̏�����
    RenderArray[0] = &ExplainManager::RenderNoneExp;
    RenderArray[1] = &ExplainManager::RenderMoveExp;
    RenderArray[2] = &ExplainManager::RenderAttackExp;
    RenderArray[3] = &ExplainManager::RenderDodgeExp;
    RenderArray[4] = &ExplainManager::RenderElement1Exp;
    RenderArray[5] = &ExplainManager::RenderElement2Exp;
    RenderArray[6] = &ExplainManager::RenderLastExp;



    NextConditionFuncArray[0] = &ExplainManager::ConditionNoneToMove;
    NextConditionFuncArray[1] = &ExplainManager::ConditionMoveToAttack;
    NextConditionFuncArray[2] = &ExplainManager::ConditionAttackToDodge;
    NextConditionFuncArray[3] = &ExplainManager::ConditionDodgeToElement1;
    NextConditionFuncArray[4] = &ExplainManager::ConditionElement1ToElement2;
    NextConditionFuncArray[5] = &ExplainManager::ConditionElement2ToEnd;
    NextConditionFuncArray[6] = &ExplainManager::ConditionEndToGame;





    //--------------------<�t�H���g��������>--------------------//
    font = std::make_unique<Font>(d, "./resources/Sprite/Font/Font.fnt", 2048);

    DecideInputDevice = false;
    
}

//****************************************************************
// 
// �X�V����
// 
//****************************************************************
void ExplainManager::Update(float elapsedTime)
{
    // ���얳�����Ԃ��X�V
    StackNoInputSeconds += elapsedTime;
    if (StackNoInputSeconds >= NoInputSeconds)
    {
        StackNoInputSeconds = NoInputSeconds;
    }



  



    // �����ɓK�����Ă����玟��
    if((this->*NextConditionFuncArray[CurrentTutorialState])(elapsedTime))
    {
        
        InitializeCondition();
    }


    GuiMenu();

}
//****************************************************************
// 
// �`�揈��
// 
//****************************************************************
void ExplainManager::Render(ID3D11DeviceContext* dc)
{

    // �����ɉ������`��
    (this->*RenderArray[CurrentTutorialState])(dc);

}
//****************************************************************
// 
// �I������
// 
//****************************************************************
void ExplainManager::Finalize()
{
    
}


//--------------------<�v���C���[�������Ă��鎞�Ԃ��X�V>--------------------//
void ExplainManager::UpdateMoveTime(float time)
{
    PlayerMoveTime += time;
}


//--------------------<�e�������Ƃɏ�����>--------------------//
void ExplainManager::InitializeCondition()
{
    CurrentTutorialState++;
    ShowTutorialText = true;
    StackNoInputSeconds = 0.0f;
    Checkmark1DissolveRate = 0.0f;
    Checkmark2DissolveRate = 0.0f;
    IsPlaySound = false;
}



void ExplainManager::GuiMenu()
{
#ifdef _DEBUG
    ImGui::Begin("Explainmanaegr");
    ImGui::DragFloat2("CheckBoxScale", &spr_CheckBox1.Scale.x);
    ImGui::DragFloat2("CheckBoxPosition", &spr_CheckBox1.Position.x);

    ImGui::DragFloat2("BackGroundPosition", &spr_TextBack.Position.x);
    ImGui::DragFloat2("BackGroundScale", &spr_TextBack.Scale.x);

    ImGui::SliderFloat("mark1power", &Checkmark1DissolveRate, 0.0f, 1.0f);

    ImGui::DragFloat2("MovexplaPosition", &spr_MoveExp.Position.x);
    ImGui::DragFloat2("MovexplaScale", &spr_MoveExp.Scale.x);

    ImGui::DragFloat2("DebugPosition", &DebugPosition.x);
    ImGui::DragFloat2("DebugScale", &DebugScale.x);

    ImGui::Checkbox("ShotTutorial", &CompleteShotTutorial);
    ImGui::Checkbox("SwordTutorial", &CompleteSwordTutorial);
    ImGui::Checkbox("Dodge", &CompleteDodge);
    ImGui::Checkbox("ChangeMode", &CompleteModeChange);
    ImGui::Checkbox("EndTutorial", &EndTutorial);

    ImGui::SliderFloat("StackEndGameSeconds", &StackaEndGameSeconds, 0.0f, 5.0f);

    switch (InputDevice) {
        case KeyBoard:
            ImGui::Text("KeyBoard");
            break;
    case Controller:
        ImGui::Text("Controller");
        break;
    default: ; }

    ImGui::DragFloat2("keypos", &sprKeyBord.Position.x);
    ImGui::DragFloat2("Conpos", &sprController.Position.x);
    ImGui::DragFloat2("choiceframepos", &sprChoiceFrame.Position.x);
    ImGui::DragFloat2("backframepos", &sprBackFrame.Position.x);
    ImGui::DragFloat2("backButton", &sprBackButton.Position.x);
    ImGui::End();
    

#endif
}



bool ExplainManager::ConditionNoneToMove(float elapsedTime)
{
    // �R���g���[���[���L�[�{�[�h��I������
    auto gamepad = GamePad::Instance();
    if (!DecideInputDevice)
    {
        if (gamepad.GetAxisLX() >= 0.5f)
        {
            InputState = 1;
        }
        else if (gamepad.GetAxisLX() <= -0.5f)
        {
            InputState = 0;
        }

        if (gamepad.GetAxisRX() >= 0.5f)
        {
            InputState = 1;
        }
        else if (gamepad.GetAxisRX() <= -0.5f)
        {
            InputState = 0;
        }

        switch (InputState)
        {
        case 0:
            InputRatio -= elapsedTime * 5.0f;
            InputRatio = (std::max)(0.0f, InputRatio);
            break;
        case 1:
            InputRatio += elapsedTime * 5.0f;
            InputRatio = (std::min)(1.0f, InputRatio);
            break;
        default:
            break;
        }

        if (InputRatio >= 1.0f)
        {
            InputDevice = Device::Controller;
        }
        else if (InputRatio <= 0.0f)
        {
            InputDevice = Device::KeyBoard;
        }

        sprChoiceFrame.Position.x = Calcu3D::LerpFloat(400.0f, 840.0f, InputRatio);


        const auto DecideButton = GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_Y | GamePad::BTN_X;
        if (gamepad.GetButtonDown() & DecideButton)
        {
            DecideInputDevice = true;
        }
    }
    else
    {
        spr_MoveExp.Scale.x += elapsedTime * 4.0f;
        spr_MoveExp.Scale.y += elapsedTime * 4.0f;

        spr_MoveExp2.Scale.x += elapsedTime * 4.0f;
        spr_MoveExp2.Scale.y += elapsedTime * 4.0f;

        if (spr_MoveExp.Scale.x >= 0.5f|| spr_MoveExp2.Scale.x >= 0.5f)
        {
            spr_MoveExp.Scale = { 0.5f,0.5f };
            spr_MoveExp2.Scale = { 0.5f,0.5f };

            const auto DecideButton = gamepad.BTN_A | gamepad.BTN_B | gamepad.BTN_Y | gamepad.BTN_X;
            if (gamepad.GetButtonDown() & DecideButton)
            {
                return true;
            }

        }
    }

    return false;
}

// �ړ��̃`���[�g���A������U���̃`���[�g���A����
bool ExplainManager::ConditionMoveToAttack(float elapsedTime)
{
    auto gamepad = GamePad::Instance();
    //--------------------<�����摜�̏���>--------------------//
    if(ShowTutorialText)
    {
        const auto DecideButton = gamepad.BTN_A | gamepad.BTN_B | gamepad.BTN_Y | gamepad.BTN_X;
        if(gamepad.GetButtonDown()&DecideButton)
        {
            ShowTutorialText = false;
            spr_MoveExp.Scale = { 0.0f,0.0f };
            spr_MoveExp2.Scale = { 0.0f,0.0f };
        }

        return false;
    }



    //--------------------<�����̏���>--------------------//
    //
    // �v���C���[������̎��Ԉȏ㓮������`�F�b�N
    if(PlayerMoveTime>1.5f)
    {
        Checkmark1DissolveRate += elapsedTime * 3.0f;
        Checkmark1DissolveRate = (std::min)(Checkmark1DissolveRate, 1.0f);

        if(!IsPlaySound)
        {
            se_Check->Play(false, OptionSystem::Instance().GetSeVolume());
            IsPlaySound = true;
        }
    }
    if(Checkmark1DissolveRate>=1.0f)
    {
        StackNextTutorialWaitSecond += elapsedTime;
    }

    if (StackNextTutorialWaitSecond > NextTutorialWaitSeconds)
    {
        spr_AttackExp.Scale.x += elapsedTime * 4.0f;
        spr_AttackExp.Scale.y += elapsedTime * 4.0f;

        spr_AttackExp2.Scale.x += elapsedTime * 4.0f;
        spr_AttackExp2.Scale.y += elapsedTime * 4.0f;

        if (spr_AttackExp.Scale.x >= 0.5f|| spr_AttackExp2.Scale.x >= 0.5f)
        {
            spr_AttackExp.Scale = { 0.5f,0.5f };
            spr_AttackExp2.Scale = { 0.5f,0.5f };

            StackNextTutorialWaitSecond = 0.0f;
            return true;
        }
    }

    return false;
}

//****************************************************************
// 
//  �U���̃`���[�g���A���������̃`���[�g���A��
// 
//****************************************************************
bool ExplainManager::ConditionAttackToDodge(float elapsedTime)
{
    auto gamepad = GamePad::Instance();
    //--------------------<�����摜�̏���>--------------------//
    if (ShowTutorialText)
    {
        const auto DecideButton = gamepad.BTN_A | gamepad.BTN_B | gamepad.BTN_Y | gamepad.BTN_X;
        if (gamepad.GetButtonDown() & DecideButton)
        {
            ShowTutorialText = false;
            spr_AttackExp.Scale = { 0.0f,0.0f };
            spr_AttackExp2.Scale = { 0.0f,0.0f };
        }

        return false;
    }

    //--------------------<�����̏���>--------------------//
    // �ˌ��œG��|������`�F�b�N
    if(CompleteShotTutorial)
    {
        Checkmark1DissolveRate += elapsedTime * 3.0f;
        Checkmark1DissolveRate = (std::min)(Checkmark1DissolveRate, 1.0f);
        if (!IsPlaySound)
        {
            se_Check->Play(false, OptionSystem::Instance().GetSeVolume());
            IsPlaySound = true;
        }

    }
    // �ߐڍU���œG��|������`�F�b�N
    if (CompleteSwordTutorial)
    {
        Checkmark2DissolveRate += elapsedTime * 3.0f;
        Checkmark2DissolveRate = (std::min)(Checkmark2DissolveRate, 1.0f);
        if (!IsPlaySound2)
        {
            se_Check->Play(false, OptionSystem::Instance().GetSeVolume());
            IsPlaySound2 = true;
        }
    }


    if(Checkmark1DissolveRate>=1.0f&& Checkmark2DissolveRate>=1.0f)
    {
        StackNextTutorialWaitSecond += elapsedTime;
    }

    if(StackNextTutorialWaitSecond>NextTutorialWaitSeconds)
    {
        spr_DodgeExp.Scale.x += elapsedTime * 4.0f;
        spr_DodgeExp.Scale.y += elapsedTime * 4.0f;


        spr_DodgeExp2.Scale.x += elapsedTime * 4.0f;
        spr_DodgeExp2.Scale.y += elapsedTime * 4.0f;

        if (spr_DodgeExp.Scale.x >= 0.5f|| spr_DodgeExp2.Scale.x >= 0.5f)
        {
            spr_DodgeExp.Scale = { 0.5f,0.5f };
            spr_DodgeExp2.Scale = { 0.5f,0.5f };

            StackNextTutorialWaitSecond = 0.0f;
            return true;
        }
    }


    return false;
}

bool ExplainManager::ConditionDodgeToElement1(float elapsedTime)
{
    auto gamepad = GamePad::Instance();
    //--------------------<�����摜�̏���>--------------------//
    if (ShowTutorialText)
    {
        const auto DecideButton = gamepad.BTN_A | gamepad.BTN_B | gamepad.BTN_Y | gamepad.BTN_X;
        if (gamepad.GetButtonDown() & DecideButton)
        {
            ShowTutorialText = false;
            spr_DodgeExp.Scale = { 0.0f,0.0f };
            spr_DodgeExp2.Scale = { 0.0f,0.0f };
        }

        return false;
    }

    //--------------------<�����̏���>--------------------//


    // ��𐬌�������
    if(DodgeCounts>=3)
    {
        Checkmark1DissolveRate += elapsedTime;
        if (!IsPlaySound)
        {
            se_Check->Play(false, OptionSystem::Instance().GetSeVolume());
            IsPlaySound = true;
        }
    }

    if (Checkmark1DissolveRate >= 1.0f)
    {
        StackNextTutorialWaitSecond += elapsedTime;
    }

    if (StackNextTutorialWaitSecond > NextTutorialWaitSeconds)
    {
        spr_Element1Exp.Scale.x += elapsedTime * 4.0f;
        spr_Element1Exp.Scale.y += elapsedTime * 4.0f;
        spr_Element1Exp2.Scale.x += elapsedTime * 4.0f;
        spr_Element1Exp2.Scale.y += elapsedTime * 4.0f;

        if (spr_Element1Exp.Scale.x >= 0.5f)
        {
            spr_Element1Exp.Scale = { 0.5f,0.5f };
            spr_Element1Exp2.Scale = { 0.5f,0.5f };
            return true;
        }
    }

    return false;
}

bool ExplainManager::ConditionElement1ToElement2(float elapsedTime)
{
    auto gamepad = GamePad::Instance();
    //--------------------<�����摜�̏���>--------------------//
    if (ShowTutorialText)
    {
        const auto DecideButton = gamepad.BTN_A | gamepad.BTN_B | gamepad.BTN_Y | gamepad.BTN_X;
        if (gamepad.GetButtonDown() & DecideButton)
        {
            ShowTutorialText = false;
            spr_Element1Exp.Scale = { 0.0f,0.0f };
            spr_Element1Exp2.Scale = { 0.0f,0.0f };
            PreferGoElement2 = true;
        }

        return false;
    }


    //--------------------<�����̏���>--------------------//

    if (PreferGoElement2)
    {
        spr_Element2Exp.Scale.x += elapsedTime * 4.0f;
        spr_Element2Exp.Scale.y += elapsedTime * 4.0f;

        spr_Element2Exp2.Scale.x += elapsedTime * 4.0f;
        spr_Element2Exp2.Scale.y += elapsedTime * 4.0f;

        if (spr_Element2Exp.Scale.x >= 0.5f|| spr_Element2Exp2.Scale.x >= 0.5f)
        {
            spr_Element2Exp.Scale = { 0.5f,0.5f };
            spr_Element2Exp2.Scale = { 0.5f,0.5f };
            PreferGoElement2 = false;
            return true;
        }
    }
    

    return false;
}
bool ExplainManager::ConditionElement2ToEnd(float elapsedTime)
{
    auto gamepad = GamePad::Instance();
    //--------------------<�����摜�̏���>--------------------//
    if (ShowTutorialText)
    {
        const auto DecideButton = gamepad.BTN_A | gamepad.BTN_B | gamepad.BTN_Y | gamepad.BTN_X;
        if (gamepad.GetButtonDown() & DecideButton)
        {
            ShowTutorialText = false;
            spr_Element2Exp.Scale = { 0.0f,0.0f };
            spr_Element2Exp2.Scale = { 0.0f,0.0f };
        }

        return false;
    }
    //--------------------<�����̏���>--------------------//
    

    
    if(CompleteModeChange)
    {
        Checkmark1DissolveRate += elapsedTime * 3.0f;
        if (!IsPlaySound)
        {
            se_Check->Play(false, OptionSystem::Instance().GetSeVolume());
            IsPlaySound = true;
        }
    }
    if (Checkmark1DissolveRate >= 1.0f)
    {
        return true;
    }

    return false;
}

bool ExplainManager::ConditionEndToGame(float elapsedTime)
{
    ShowTutorialText = false;
    auto gamepad = GamePad::Instance();
    if (gamepad.GetButton() & GamePad::BTN_BACK)
    {
        StackaEndGameSeconds += elapsedTime;
        if (StackaEndGameSeconds >= 1.0f)
        {
            EndTutorial = true;
        }
    }
    else
    {
        StackaEndGameSeconds -= elapsedTime * 0.5f;
        StackaEndGameSeconds = std::max(0.0f, StackaEndGameSeconds);
    }

    sprBackButtonFrame.TexPos.y = Calcu3D::LerpFloat(0.0f, 256.0f, StackaEndGameSeconds);
    sprBackButtonFrame.TexSize.y = Calcu3D::LerpFloat(0.0f, 256.0f, StackaEndGameSeconds);

    if (EndTutorial)
    {
        Checkmark1DissolveRate += elapsedTime * 3.0f;
    }
    if (Checkmark1DissolveRate >= 1.0f)
    {
        PermissionPerformancePlayer = true;
        if (!IsPlaySound)
        {
            se_Check->Play(false, OptionSystem::Instance().GetSeVolume());
            IsPlaySound = true;
        }
    }

    return false;
}





//****************************************************************
// 
//  �e�`���[�g���A���̕`��
// 
//****************************************************************
void ExplainManager::RenderNoneExp(ID3D11DeviceContext* dc)
{

    if (!DecideInputDevice)
    {

        sprBackFrame.Render(dc);
        sprController.Render(dc);
        sprKeyBord.Render(dc);
        sprChoiceFrame.Render(dc);

        // �`���[�g���A���̏�����
        font->Begin(dc);
        font->Draw({ 640.0f,100.0f }, { 1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f }, L"�ǂ���Ńv���C���邩�I��ł�������", wcslen(L"�ǂ���Ńv���C���邩�I��ł�������"), Font::TEXT_ALIGN::UPPER_MIDDLE);
        switch (InputDevice)
        {
        case KeyBoard:
            font->Draw({ 640.0f,600.0f }, { 1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f }, L"�L�[�{�[�h", wcslen(L"�L�[�{�[�h"), Font::TEXT_ALIGN::UPPER_MIDDLE);
            break;
        case Controller:
            font->Draw({ 640.0f,600.0f }, { 1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f }, L"XBox�R���g���[���[", wcslen(L"XBox�R���g���[���["), Font::TEXT_ALIGN::UPPER_MIDDLE);
            font->Draw({ 640.0f,540.0f }, { 1.0f,1.0f }, { 1.0f,1.0f,0.0f,1.0f }, L"�i�����j", wcslen(L"�i�����j"), Font::TEXT_ALIGN::UPPER_MIDDLE);
            break;
        default:;
        }

        font->End(dc);
    }
    else
    {
          spr_CheckMark1.SetDissolvePower(Checkmark1DissolveRate);

          spr_TextBack.Render(dc);
          spr_CheckBox1.Render(dc);
          spr_CheckMark1.Render(dc);

          switch (InputDevice) {
          case KeyBoard:
              spr_MoveExp.Render(dc);
              break;
          case Controller:
              spr_MoveExp2.Render(dc);
              break;
          default:;
          }
    }
}

 void ExplainManager::RenderMoveExp(ID3D11DeviceContext* dc)
{
     spr_CheckMark1.SetDissolvePower(Checkmark1DissolveRate);

     spr_TextBack.Render(dc);
     spr_CheckBox1.Render(dc);
     spr_CheckMark1.Render(dc);

    


    
     switch (InputDevice) {
     case KeyBoard:

         // �`���[�g���A���̏�����
         font->Begin(dc);
         // �@�̂𓮂���
         font->Draw({ 890.0f,45.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�@�̂𓮂���(W/A/S/D)", wcslen(L"�@�̂𓮂���(W/A/S/D)"), Font::TEXT_ALIGN::UPPER_LEFT);

         font->Draw({ 460.0f, 60.0f }, { 0.4f,0.4f }, { 1.0f,1.0f,1.0f,1.0f }, L"TAB�L�[�ŃI�v�V�������J��", wcslen(L"TAB�L�[�ŃI�v�V�������J��"), Font::TEXT_ALIGN::UPPER_LEFT);
         
         


         font->End(dc);

         spr_MoveExp.Render(dc);
         spr_AttackExp.Render(dc);


         break;
     case Controller:

         sprStartButton.Render(dc);

         font->Begin(dc);
         font->Draw({ 460.0f, 60.0f }, { 0.4f,0.4f }, { 1.0f,1.0f,1.0f,1.0f }, L"�{�^���ŃI�v�V�������J��", wcslen(L"�{�^���ŃI�v�V�������J��"), Font::TEXT_ALIGN::UPPER_LEFT);
         // �ߐڍU���œG��|��
         font->End(dc);

         // �`���[�g���A���̏�����
         font->Begin(dc);
         // �@�̂𓮂���
         font->Draw({ 890.0f,45.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�@�̂𓮂���(L�X�e�B�b�N)", wcslen(L"�@�̂𓮂���(L�X�e�B�b�N)"), Font::TEXT_ALIGN::UPPER_LEFT);

         font->End(dc);

         spr_MoveExp2.Render(dc);
         spr_AttackExp2.Render(dc);

         break;
     default:;
     }
     
}

 //****************************************************************
// 
//  �U���̃`���[�g���A���̕`��
// 
//****************************************************************
 void ExplainManager::RenderAttackExp(ID3D11DeviceContext* dc)
{

     spr_CheckMark1.SetDissolvePower(Checkmark1DissolveRate);
     spr_CheckMark2.SetDissolvePower(Checkmark2DissolveRate);

     spr_TextBack.Render(dc);
     spr_CheckBox1.Render(dc);
     spr_CheckMark1.Render(dc);
     spr_CheckBox2.Render(dc);
     spr_CheckMark2.Render(dc);


   

     switch (InputDevice) {
     case KeyBoard:
         // �`���[�g���A���̏�����
         font->Begin(dc);
         // �ˌ��œG��|��
         font->Draw({ 890.0f,45.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�ˌ��œG��|��(L�L�[)", wcslen(L"�ˌ��œG��|��(L�L�[)"), Font::TEXT_ALIGN::UPPER_LEFT);

         font->Draw({ 890.0f,105.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�ߐڍU���œG��|��(K�L�[)", wcslen(L"�ߐڍU���œG��|��(K�L�[)"), Font::TEXT_ALIGN::UPPER_LEFT);

         font->Draw({ 460.0f, 60.0f }, { 0.4f,0.4f }, { 1.0f,1.0f,1.0f,1.0f }, L"TAB�L�[�ŃI�v�V�������J��", wcslen(L"TAB�L�[�ŃI�v�V�������J��"), Font::TEXT_ALIGN::UPPER_LEFT);

         font->End(dc);

         spr_AttackExp.Render(dc);
         spr_DodgeExp.Render(dc);
         break;
     case Controller:


         sprStartButton.Render(dc);

         font->Begin(dc);
         font->Draw({ 460.0f, 60.0f }, { 0.4f,0.4f }, { 1.0f,1.0f,1.0f,1.0f }, L"�{�^���ŃI�v�V�������J��", wcslen(L"�{�^���ŃI�v�V�������J��"), Font::TEXT_ALIGN::UPPER_LEFT);
         // �ߐڍU���œG��|��
         font->End(dc);

         // �`���[�g���A���̏�����
         font->Begin(dc);
         // �ˌ��œG��|��
         font->Draw({ 890.0f,45.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�ˌ��œG��|��(X�{�^��)", wcslen(L"�ˌ��œG��|��(X�{�^��)"), Font::TEXT_ALIGN::UPPER_LEFT);

         font->Draw({ 890.0f,105.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�ߐڍU���œG��|��(A�{�^��)", wcslen(L"�ߐڍU���œG��|��(A�{�^��)"), Font::TEXT_ALIGN::UPPER_LEFT);
         // �ߐڍU���œG��|��
         font->End(dc);

         spr_AttackExp2.Render(dc);
         spr_DodgeExp2.Render(dc);
         break;
     default:;
     }
     
     
}


//****************************************************************
// 
//  ����̃`���[�g���A���̕`��
// 
//****************************************************************
 void ExplainManager::RenderDodgeExp(ID3D11DeviceContext* dc)
{
     spr_CheckMark1.SetDissolvePower(Checkmark1DissolveRate);
     

     spr_TextBack.Render(dc);
     spr_CheckBox1.Render(dc);
     spr_CheckMark1.Render(dc);
     



     switch (InputDevice) {
     case KeyBoard:

     {
         // �`���[�g���A���̏�����
         font->Begin(dc);
         // �U�����M���M���ŉ������
         font->Draw({ 890.0f,45.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�e���M���M���ŉ������(J�L�[)", wcslen(L"�e���M���M���ŉ������(J�L�[)"), Font::TEXT_ALIGN::UPPER_LEFT);

         std::wstring a = std::to_wstring(std::max(3 - DodgeCounts, 0));

         font->Draw({ 880.0f,85.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�c��", wcslen(L"�c��"), Font::TEXT_ALIGN::UPPER_LEFT);

         font->Draw({ 925.0f,85.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f },
             a.c_str(), wcslen(a.c_str()), Font::TEXT_ALIGN::UPPER_LEFT);

         font->Draw({ 940.0f,85.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"��", wcslen(L"��"), Font::TEXT_ALIGN::UPPER_LEFT);

         font->Draw({ 460.0f, 60.0f }, { 0.4f,0.4f }, { 1.0f,1.0f,1.0f,1.0f }, L"TAB�L�[�ŃI�v�V�������J��", wcslen(L"TAB�L�[�ŃI�v�V�������J��"), Font::TEXT_ALIGN::UPPER_LEFT);

         font->End(dc);

         spr_DodgeExp.Render(dc);
         spr_Element1Exp.Render(dc);
     }
         break;
     case Controller:
     {
         sprStartButton.Render(dc);

         font->Begin(dc);
         font->Draw({ 460.0f, 60.0f }, { 0.4f,0.4f }, { 1.0f,1.0f,1.0f,1.0f }, L"�{�^���ŃI�v�V�������J��", wcslen(L"�{�^���ŃI�v�V�������J��"), Font::TEXT_ALIGN::UPPER_LEFT);
         // �ߐڍU���œG��|��
         font->End(dc);

         // �`���[�g���A���̏�����
         font->Begin(dc);
         // �U�����M���M���ŉ������
         font->Draw({ 890.0f,45.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�e���M���M���ŉ������(R�g���K�[)", wcslen(L"�e���M���M���ŉ������(R�g���K�[)"), Font::TEXT_ALIGN::UPPER_LEFT);

         std::wstring a = std::to_wstring(std::max(3 - DodgeCounts, 0));
         font->Draw({ 880.0f,85.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�c��", wcslen(L"�c��"), Font::TEXT_ALIGN::UPPER_LEFT);

         font->Draw({ 925.0f,85.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f },
             a.c_str(), wcslen(a.c_str()), Font::TEXT_ALIGN::UPPER_LEFT);

         font->Draw({ 940.0f,85.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"��", wcslen(L"��"), Font::TEXT_ALIGN::UPPER_LEFT);

         font->End(dc);

         spr_DodgeExp2.Render(dc);
         spr_Element1Exp2.Render(dc);
     }
         break;
     default:;
     }

     

}

 void ExplainManager::RenderElement1Exp(ID3D11DeviceContext* dc)
{
     spr_CheckMark1.SetDissolvePower(Checkmark1DissolveRate);


     spr_TextBack.Render(dc);
     spr_CheckBox1.Render(dc);
     spr_CheckMark1.Render(dc);

     switch (InputDevice) {
     case KeyBoard:

         spr_Element1Exp.Render(dc);
         spr_Element2Exp.Render(dc);
         break;
     case Controller:

         spr_Element1Exp2.Render(dc);
         spr_Element2Exp2.Render(dc);
         break;
     default:;
     }


    
}

 void ExplainManager::RenderElement2Exp(ID3D11DeviceContext* dc)
{
     spr_CheckMark1.SetDissolvePower(Checkmark1DissolveRate);


     spr_TextBack.Render(dc);
     spr_CheckBox1.Render(dc);
     spr_CheckMark1.Render(dc);

     switch (InputDevice) {
     case KeyBoard:

         // �`���[�g���A���̏�����
         font->Begin(dc);
         font->Draw({ 890.0f,45.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�Q�[�W�𗭂߂�", wcslen(L"�Q�[�W�𗭂߂�"), Font::TEXT_ALIGN::UPPER_LEFT);
         font->Draw({ 890.0f,85.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�X�y�[�X�L�[�ő�����ς���", wcslen(L"�X�y�[�X�L�[�ő�����ς���"), Font::TEXT_ALIGN::UPPER_LEFT);

         font->Draw({ 460.0f, 60.0f }, { 0.4f,0.4f }, { 1.0f,1.0f,1.0f,1.0f }, L"TAB�L�[�ŃI�v�V�������J��", wcslen(L"TAB�L�[�ŃI�v�V�������J��"), Font::TEXT_ALIGN::UPPER_LEFT);

         font->End(dc);

         spr_Element1Exp.Render(dc);
         spr_Element2Exp.Render(dc);
         break;
     case Controller:

         sprStartButton.Render(dc);

         font->Begin(dc);
         font->Draw({ 460.0f, 60.0f }, { 0.4f,0.4f }, { 1.0f,1.0f,1.0f,1.0f }, L"�{�^���ŃI�v�V�������J��", wcslen(L"�{�^���ŃI�v�V�������J��"), Font::TEXT_ALIGN::UPPER_LEFT);
         // �ߐڍU���œG��|��
         font->End(dc);

         // �`���[�g���A���̏�����
         font->Begin(dc);
         font->Draw({ 890.0f,45.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�Q�[�W�𗭂߂�", wcslen(L"�Q�[�W�𗭂߂�"), Font::TEXT_ALIGN::UPPER_LEFT);
         font->Draw({ 890.0f,85.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"L�{�^��/�g���K�[�ő�����ς���", wcslen(L"L�{�^��/�g���K�[�ő�����ς���"), Font::TEXT_ALIGN::UPPER_LEFT);
         font->End(dc);

         spr_Element1Exp2.Render(dc);
         spr_Element2Exp2.Render(dc);
         break;
     default:;
     }

    
}

void ExplainManager::RenderLastExp(ID3D11DeviceContext* dc)
{
    spr_CheckMark1.SetDissolvePower(Checkmark1DissolveRate);


    spr_TextBack.Render(dc);
    spr_CheckBox1.Render(dc);
    spr_CheckMark1.Render(dc);



    switch (InputDevice) {
    case KeyBoard:

        // �`���[�g���A���̏�����
        font->Begin(dc);
        font->Draw({ 890.0f,45.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"Q�L�[�𒷉�������", wcslen(L"Q�L�[�𒷉�������"), Font::TEXT_ALIGN::UPPER_LEFT);
        font->Draw({ 890.0f,85.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�Q�[���ɐi��", wcslen(L"�Q�[���ɐi��"), Font::TEXT_ALIGN::UPPER_LEFT);

        font->Draw({ 460.0f, 60.0f }, { 0.4f,0.4f }, { 1.0f,1.0f,1.0f,1.0f }, L"TAB�L�[�ŃI�v�V�������J��", wcslen(L"TAB�L�[�ŃI�v�V�������J��"), Font::TEXT_ALIGN::UPPER_LEFT);

        font->End(dc);

        break;
    case Controller:

        sprStartButton.Render(dc);

        font->Begin(dc);
        font->Draw({ 460.0f, 60.0f }, { 0.4f,0.4f }, { 1.0f,1.0f,1.0f,1.0f }, L"�{�^���ŃI�v�V�������J��", wcslen(L"�{�^���ŃI�v�V�������J��"), Font::TEXT_ALIGN::UPPER_LEFT);
        // �ߐڍU���œG��|��
        font->End(dc);

        sprBackButton.Render(dc);
        // �`���[�g���A���̏�����
        font->Begin(dc);
        font->Draw({ 950.0f,45.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�{�^���𒷉�������", wcslen(L"�{�^���𒷉�������"), Font::TEXT_ALIGN::UPPER_LEFT);
        font->Draw({ 950.0f,85.0f }, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }, L"�Q�[���ɐi��", wcslen(L"�Q�[���ɐi��"), Font::TEXT_ALIGN::UPPER_LEFT);
        font->End(dc);

        break;
    default:;
    }

}

void ExplainManager::PadUpdate()
{
}