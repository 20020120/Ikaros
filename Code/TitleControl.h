#pragma once
#include "Controller.h"
#include "Timer.h"

#include "gAudioSource.h"


class TitleControl final
{
public:
    TitleControl() = default;
    ~TitleControl() = default;

public:
    void Initialize();
    void InputUpdate(float elapsedTime);
    void Finalize();

public: // Get�֐�
    const short GetStateIndex() { return index; }
    const bool PushEnterNow() { return is_push_enter; }

public: // Set�֐�
    void SetStateIndexMax(const short& max_) { max_limit = max_; }
    void IndexToZero() { index = min_limit; }
    void SetIsDemo() { is_demo = true; }
    void IndexToOne() { index = 1; }    // index���P�ɂ���   (�V�[���J�ڗp�̃t���O�ŁA0���Q�[��, 1���`���[�g���A���Ƃ�������)�@(�ڍׂ�,TitleManager��sNone�Ō�s)

private:
    void Control(float elapsedTime);
    void CursorMove(float elapsedTime);
    void DecisionKey();
    void DecisionKey_NoCoolTime();

private:
    static constexpr GamePadButton startButton = GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y;
    static constexpr short min_limit = 0;

private:
    Timer cool_timer;

    short index;
    bool is_push_enter;

    bool is_demo = false;

    short max_limit;

    //sound
    std::unique_ptr<AudioSource> se_select;
};
