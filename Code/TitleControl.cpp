
#include "TitleControl.h"
#include "gAudio.h"
#include "OptionSystem.h"

void TitleControl::Initialize()
{
    cool_timer.Initialize(nullptr, COUNT::DOWN, 0.2f);

    se_select = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/OperateSE.wav");

    is_demo = false;
}

void TitleControl::Finalize()
{
    se_select->Release();
}



void TitleControl::InputUpdate(float elapsedTime)
{
    // ���t���[�����Z�b�g����
    is_push_enter = false;


    // ���͏���
    Control(elapsedTime);
}


void TitleControl::Control(float elapsedTime)
{

    // �f�����͕ʏ�����,����return
    if (is_demo)
    {
        DecisionKey_NoCoolTime();
        return;
    }

    // �N�[���^�C�}�[�̍X�V
    cool_timer.Update(elapsedTime);

    // �N�[���^�C�����I����Ă��Ȃ���� return����
    if (cool_timer.NowTime() > 0.0f) return;


    //// �Ȃɂ����͂�������� return����
    //if (!gamepad.GetButton())return;

    // �I���L�[�������ꂽ�Ƃ��̏���
    CursorMove(elapsedTime);

    // ����L�[�������ꂽ�Ƃ��̏���
    DecisionKey();
}




void TitleControl::CursorMove(float elapsedTime)
{
    auto gamepad = GamePad::Instance();

    if ((gamepad.GetAxisLY() > 0.0f) || gamepad.GetButton() & GamePad::BTN_UP)//up
    {
        if (index > min_limit)
        {
            --index;

            cool_timer.Reset();

            se_select->Stop();
            se_select->Play(false, OptionSystem::Instance().GetSeVolume());
        }
    }
    if ((gamepad.GetAxisLY() < 0.0f) || gamepad.GetButton() & GamePad::BTN_DOWN)//down
    {
        if (index < max_limit)
        {
            ++index;

            cool_timer.Reset();

            se_select->Stop();
            se_select->Play(false, OptionSystem::Instance().GetSeVolume());
        }
    }


    // ����
    if(index < min_limit)
    {
        index = min_limit;
    }

    if(index > max_limit)
    {
        index = max_limit;
    }
    
}


void TitleControl::DecisionKey()
{
    // ����L�[�̓��͂������
    if (GamePad::Instance().GetButtonDown() & startButton)
    {
        is_push_enter = true;
    }
}

void TitleControl::DecisionKey_NoCoolTime()
{
    // ����L�[�̓��͂������
    if (/*gamepad.GetButton()*/GamePad::Instance().GetButtonDown() & startButton)
    {
        is_push_enter = true;
    }
}