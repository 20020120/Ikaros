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
// マウスの左ボタン長押しを検知
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
// マウスカーソルの位置を取得
// 
//****************************************************************

//--------------------<今>--------------------//
DirectX::XMFLOAT2 UserMouse::f_GetCursolPosition()
{
    return CursolPosition;
}
//--------------------<１フレーム前>--------------------//
DirectX::XMFLOAT2 UserMouse::f_GetOldCursolPosition()
{
    return OldCursolPosition;
}

//****************************************************************
// 
// マウスカーソルの速度
// 
//****************************************************************
DirectX::XMFLOAT2 UserMouse::f_GetCursolSpeed()
{
    return CursolSpeed;
}



//****************************************************************
// 
// 更新処理
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
