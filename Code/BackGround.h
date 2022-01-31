#pragma once

#include"ModelComponent.h"
#include"ShipMoveState.h"

#include"ConstantBuffer.h"


#include <memory>

#include"Texture.h"


// 背景の天球クラス
class SkyDome
{
public:
    struct Data
    {
        DirectX::XMFLOAT4 ScrollSpeed; //UVスクロールの速度
    };


    SkyDome(ID3D11Device* d);
    ~SkyDome()=default;


    void Update(float elapsedTime, int View);
    void Render(ID3D11DeviceContext* dc);

private:
    std::unique_ptr<ModelComponent> model;

    Rotate r;

    DirectX::XMFLOAT4 orientation{ 0.0f,0.0f,0.0f,1.0f };  // 回転行列
    DirectX::XMFLOAT3 RotateAxis{ 0.0f,-1.0001f,0.0f }; // 回転軸
};


// 背景の船
class BGShip
{
public:
    BGShip(ID3D11Device* d);
    ~BGShip() = default;


    void Update(float elapsedTime, int View);
    void Render(ID3D11DeviceContext* dc);
private:
    void GuiMenu();

    void MoveSideView(float elapsedTime);  // 横視点の動き
    void MoveUpView(float elapsedTime);    // 縦視点の動き

    [[nodiscard]] const auto& GetFrontVec() const; // モデルの正面ベクトル取得（正規化済）


    using MoveBehavior = void(BGShip::*)(float); // 船の動き遷移
    MoveBehavior behaviorArray[2]{}; // 動きのはいれつ

    std::unique_ptr<ModelComponent> model;

    Transform t;
    Rotate r;

    float StackWaitTime = {}; // 待機経過時間
    const float WaitTime = {20.0f}; // 待機時間

    int MoveState = 0;
    int CurrentView = 0; // 現在の視点
};


class BGColony
{
public:
    BGColony(ID3D11Device* d);
    ~BGColony();

    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);

private:
    //--------------------<関数>--------------------//
    void GuiMenu();

    //--------------------<変数>--------------------//
    std::unique_ptr<ModelComponent> Model;
    Transform t;
    Rotate r;

    Texture Tex_BaseColor{};
    Texture Tex_Metallic{};
    Texture Tex_Roughness{};
    Texture Tex_Normal{};

    
};

class BackGround
{
public:
    BackGround(ID3D11Device* d);
    ~BackGround() = default;


    void Initialize();
    void Update(float elapsedTime,int View);
    void Render(ID3D11DeviceContext* dc);
    void f_Gui();

public:
    std::unique_ptr<BGShip> ship;
    std::unique_ptr<SkyDome> dome;
    std::unique_ptr<BGColony> planet;

};

