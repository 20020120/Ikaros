#pragma once
#include<Windows.h>
#include<DirectXMath.h>
class UserMouse
{
public:
    UserMouse(HWND h);
    ~UserMouse();



    //--------------------<マウスの左ボタンが押されているかどうか>--------------------//
    bool f_StateLeftButton();
    //--------------------<マウスポインタ―の位置を取得>--------------------//
    DirectX::XMFLOAT2 f_GetCursolPosition();

    //--------------------<1フレーム前のカーソルの位置を取得>--------------------//
    DirectX::XMFLOAT2 f_GetOldCursolPosition();
    
    //--------------------<1フレームごとの速度>--------------------//
    DirectX::XMFLOAT2 f_GetCursolSpeed();

    //--------------------<アップデート関数>--------------------//
    void f_Update();
private:
    HWND hwnd;
    DirectX::XMFLOAT2 OldCursolPosition{};
    DirectX::XMFLOAT2 CursolPosition{};
    DirectX::XMFLOAT2 CursolSpeed{};
};
