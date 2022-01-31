
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
    // 毎フレームリセットする
    is_push_enter = false;


    // 入力処理
    Control(elapsedTime);
}


void TitleControl::Control(float elapsedTime)
{

    // デモ中は別処理後,早期return
    if (is_demo)
    {
        DecisionKey_NoCoolTime();
        return;
    }

    // クールタイマーの更新
    cool_timer.Update(elapsedTime);

    // クールタイムが終わっていなければ returnする
    if (cool_timer.NowTime() > 0.0f) return;


    //// なにも入力が無ければ returnする
    //if (!gamepad.GetButton())return;

    // 選択キーが押されたときの処理
    CursorMove(elapsedTime);

    // 決定キーが押されたときの処理
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


    // 制限
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
    // 決定キーの入力があれば
    if (GamePad::Instance().GetButtonDown() & startButton)
    {
        is_push_enter = true;
    }
}

void TitleControl::DecisionKey_NoCoolTime()
{
    // 決定キーの入力があれば
    if (/*gamepad.GetButton()*/GamePad::Instance().GetButtonDown() & startButton)
    {
        is_push_enter = true;
    }
}