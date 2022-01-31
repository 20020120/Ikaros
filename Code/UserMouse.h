#pragma once
#include<Windows.h>
#include<DirectXMath.h>
class UserMouse
{
public:
    UserMouse(HWND h);
    ~UserMouse();



    //--------------------<�}�E�X�̍��{�^����������Ă��邩�ǂ���>--------------------//
    bool f_StateLeftButton();
    //--------------------<�}�E�X�|�C���^�\�̈ʒu���擾>--------------------//
    DirectX::XMFLOAT2 f_GetCursolPosition();

    //--------------------<1�t���[���O�̃J�[�\���̈ʒu���擾>--------------------//
    DirectX::XMFLOAT2 f_GetOldCursolPosition();
    
    //--------------------<1�t���[�����Ƃ̑��x>--------------------//
    DirectX::XMFLOAT2 f_GetCursolSpeed();

    //--------------------<�A�b�v�f�[�g�֐�>--------------------//
    void f_Update();
private:
    HWND hwnd;
    DirectX::XMFLOAT2 OldCursolPosition{};
    DirectX::XMFLOAT2 CursolPosition{};
    DirectX::XMFLOAT2 CursolSpeed{};
};
