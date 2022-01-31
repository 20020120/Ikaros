#include "UserMouse.h"


UserMouse::UserMouse(HWND h)
{
    hwnd = h;
    f_GetCursolPosition();
    f_GetOldCursolPosition();
}

UserMouse::~UserMouse()
{
}

//****************************************************************
// 
// �}�E�X�̍��{�^�������������m
// 
//****************************************************************
bool UserMouse::f_StateLeftButton()
{
    bool state{};
    if (GetKeyState(VK_LBUTTON) & 0x80)
    {
        state = true;
    }
    return state;
}


//****************************************************************
// 
// �}�E�X�J�[�\���̈ʒu���擾
// 
//****************************************************************

//--------------------<��>--------------------//
DirectX::XMFLOAT2 UserMouse::f_GetCursolPosition()
{
    return CursolPosition;
}
//--------------------<�P�t���[���O>--------------------//
DirectX::XMFLOAT2 UserMouse::f_GetOldCursolPosition()
{
    return OldCursolPosition;
}

//****************************************************************
// 
// �}�E�X�J�[�\���̑��x
// 
//****************************************************************
DirectX::XMFLOAT2 UserMouse::f_GetCursolSpeed()
{
    return CursolSpeed;
}



//****************************************************************
// 
// �X�V����
// 
//****************************************************************
void UserMouse::f_Update()
{
    POINT p{};
    ::GetCursorPos(&p);
    ::ScreenToClient(hwnd, &p);

    OldCursolPosition = CursolPosition;
    CursolPosition = { static_cast<float>(p.x),static_cast<float>(p.y) };
    CursolSpeed = { OldCursolPosition.x - CursolPosition.x,OldCursolPosition.y - CursolPosition.y };

  
}
