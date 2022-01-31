#include"BackGround.h"
#include <imgui.h>

#include "ShipMoveFunctions.h"
#include"MathSupport.h"
#include "User_Function.h"



BackGround::BackGround(ID3D11Device* d)
{
    ship = std::make_unique<BGShip>(d);
    dome = std::make_unique<SkyDome>(d);
    planet = std::make_unique<BGColony>(d);
    ship->Update(0.0f, 0);
}

void BackGround::Initialize()
{
 
}


void BackGround::Update(float elapsedTime,int View)
{
    ship->Update(elapsedTime, View);
    dome->Update(elapsedTime, View);
    planet->Update(elapsedTime);
}


void BackGround::Render(ID3D11DeviceContext* dc)
{
    ship->Render(dc);
    dome->Render(dc);
    planet->Render(dc);

}




void BackGround::f_Gui()
{
#if _DEBUG

    
#endif
}



//****************************************************************
// 
//  天球クラス
// 
//****************************************************************

SkyDome::SkyDome(ID3D11Device* d)
{
    // モデルのロード

    ModelRenderInitializer Ini{ };
    Ini.SetALLPath(
        "./resources/Models/Stages/SkyDome3.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/LambertPS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );

    model = std::make_unique<ModelComponent>(d, Ini);
    r.Angle={0.0f,0.0f,0.0f};
}

void SkyDome::Update(float elapsedTime,int View)
{

    constexpr float ScrollSpeed = 100.0f;
    constexpr DirectX::XMFLOAT3 SideViewAxis{ 0.0f,-1.00001f,0.0f }; // 横画面の時の回転軸
    constexpr DirectX::XMFLOAT3 TopViewAxis{ 1.000001f,0.0f,0.0f }; // 縦画面の時の回転軸

    DirectX::XMVECTOR V_Front = { DirectX::XMVectorSet(0.0f,0.0f,1.00001f,0.0f) };

    DirectX::XMVECTOR V_RotAxis = DirectX::XMLoadFloat3(&RotateAxis);
    V_RotAxis = DirectX::XMVector3Normalize(V_RotAxis);
    DirectX::XMVECTOR OrienVec = DirectX::XMLoadFloat4(&orientation);
    DirectX::XMVECTOR V_EndAxis{};

    // 軸を補完
    if (View==0)  // 横回転
    {
        V_EndAxis = DirectX::XMLoadFloat3(&SideViewAxis);
    }
    else if(View==1) // 縦回転
    {
        V_EndAxis = DirectX::XMLoadFloat3(&TopViewAxis);
    }
    V_RotAxis = V_EndAxis;

      

    // 任意軸で回転
    DirectX::XMVECTOR RQ = DirectX::XMQuaternionRotationAxis(V_RotAxis, ScrollSpeed * elapsedTime);
    DirectX::XMVECTOR VO = DirectX::XMQuaternionMultiply(OrienVec, RQ);

    OrienVec= DirectX::XMQuaternionSlerp(OrienVec, VO, 10.0f * elapsedTime);
    DirectX::XMStoreFloat4(&orientation, OrienVec);

}

void SkyDome::Render(ID3D11DeviceContext* dc)
{
    // 動かさないのでローカル
    Transform t;
    t.Position = { 0.0f,0.0f,0.0f };
    t.Scale = { 0.5f,0.5f,0.5f };


    model->Render(dc, t, orientation);
}


//****************************************************************
// 
//  宇宙船の背景
// 
//****************************************************************

BGShip::BGShip(ID3D11Device* d)
{
    // モデルのロード
    ModelRenderInitializer Ini{ };
    Ini.SetALLPath(
        "./resources/Models/Stages/Ship.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/LambertPS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );

    model = std::make_unique<ModelComponent>(d, Ini);

    t.Position = { -100.0f,0.0f,0.0f };
    t.Scale = { 0.03f,0.03f,0.03f };
    r.Angle.y = DirectX::XMConvertToRadians(-90.0f);

    MoveState = 0;

    // 動きの配列を初期化
    behaviorArray[0] = &BGShip::MoveSideView;
    behaviorArray[1] = &BGShip::MoveUpView;


}

const auto& BGShip::GetFrontVec() const
{
    const DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z);
    DirectX::XMFLOAT3 front{};
    DirectX::XMVector3Normalize(transform.r[0]);
    DirectX::XMStoreFloat3(&front, transform.r[0]);
    return front;
}

void BGShip::Update(float elapsedTime, int View)
{
    if(CurrentView!=View)
    {
        CurrentView = View;
        MoveState = 0;
    }

    (this->*behaviorArray[CurrentView])(elapsedTime);
    GuiMenu();
}

void BGShip::Render(ID3D11DeviceContext* dc)
{
  
    model->Render(dc, t, r);
}



//--------------------<よこしてんの動き>--------------------//
void BGShip::MoveSideView(float elapsedTime)
{
    constexpr float Speed = 10.0f;

    switch (MoveState)
    {
    case 0:
        // 位置の初期化
        t.Position = { -250.0f,0.0f,-270.0f };
        t.Scale = { 0.03f,0.03f,0.03f };
        MoveState++;
        /*FallThrough*/

    case 1:
        // 左奥から中央付近に傾きながら移動
        t.Position += GetFrontVec() * Speed * elapsedTime;

        r.Angle.x = Calcu3D::LerpFloat(r.Angle.x, DirectX::XMConvertToRadians(20.0f),  1.0f*elapsedTime);
        r.Angle.y = Calcu3D::LerpFloat(r.Angle.y, DirectX::XMConvertToRadians(-45.0f), 0.3f*elapsedTime);

        if(r.Angle.y>DirectX::XMConvertToRadians(-45.003f))
        {
            MoveState++;
        }
        break;

    case 2:
        r.Angle.y = Calcu3D::LerpFloat(r.Angle.y, DirectX::XMConvertToRadians(-180.0f), 1.0f*elapsedTime);
        r.Angle.x = Calcu3D::LerpFloat(r.Angle.x, DirectX::XMConvertToRadians(0.0f),    1.0f*elapsedTime);

        if (r.Angle.y < DirectX::XMConvertToRadians(-170.003f))
        {
            MoveState++;
        }
        break;

    case 3:
        t.Position += GetFrontVec() * Speed * elapsedTime * 50.0f;
        if(t.Position.x<-600.0f)
        {
            MoveState++;
        }
        break;
    case 4:
       // エフェクトを出して消える
        StackWaitTime = 0.0f;
         t.Scale = { 0.0f,0.0f,0.0f };
         MoveState++;
        
        break;
    case 5:
        StackWaitTime += elapsedTime;
        if(StackWaitTime>WaitTime)
        {
            MoveState = 0;
        }
        break;
        default: ;
    }   
}


//--------------------<たてしてんの動き>--------------------//
void BGShip::MoveUpView(float elapsedTime)
{
    constexpr float Speed = 20.0f;

    switch (MoveState)
    {
    case 0:
        // 位置の初期化
        t.Position = { 0.0f,-80.0f,-170.0f };
        t.Scale = { 0.03f,0.03f,0.03f };
        r.Angle = { 0.0f,DirectX::XMConvertToRadians(-90.0f),0.0f };
        MoveState++;
        /*FallThrough*/

    case 1:
        // 左奥から中央付近に傾きながら移動
        t.Position.z += Speed * elapsedTime;

       if(t.Position.z>0.0f)
       {
           MoveState++;
           StackWaitTime = 0.0f;
       }
        break;

    case 2:
        StackWaitTime += elapsedTime;
        if(StackWaitTime>WaitTime*0.2f)
        {
            MoveState++;
        }

        break;
    case 3:
        t.Position += GetFrontVec() * Speed * elapsedTime * 50.0f;
        if (t.Position.z >600.0f)
        {
            MoveState++;
            StackWaitTime = 0.0f;
        }
        break;
    case 4:
        StackWaitTime += elapsedTime;
        if (StackWaitTime > WaitTime)
        {
            MoveState = 0;
        }
        break;
    default:;
    }
}



void BGShip::GuiMenu()
{
    return;
#ifdef _DEBUG


    ImGui::Begin("BGShip");
    DirectX::XMFLOAT3 deg = {
        DirectX::XMConvertToDegrees(r.Angle.x),
           DirectX::XMConvertToDegrees(r.Angle.y),
           DirectX::XMConvertToDegrees(r.Angle.z)
    };
    ImGui::DragFloat3("Angle", &deg.x);

    r.Angle = {
        DirectX::XMConvertToRadians(deg.x),
        DirectX::XMConvertToRadians(deg.y),
        DirectX::XMConvertToRadians(deg.z)
    };

    ImGui::End();
#endif
}



//****************************************************************
// 
//  背景の惑星
// 
//****************************************************************

BGColony::BGColony(ID3D11Device* d)
{
    // モデルをロード
    ModelRenderInitializer Ini{ };
    Ini.SetALLPath(
        "./resources/Models/Stages/Colony2.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/Colony_PBR_Ps.cso",
        "./resources/Shaders/GeometryShader.cso"
    );

    Model = std::make_unique<ModelComponent>(d, Ini);

    t.Position = { 54.0f,-412.0f,0.0f };
    t.Scale = { 40.0f,40.0f,40.0f };
    r.Angle = { 0.0f,DirectX::XMConvertToRadians(90.0f),0.0f };

    // テクスチャの初期化
    

}

BGColony::~BGColony()
{
}

void BGColony::Update(float elapsedTime)
{

    r.Angle.z -= DirectX::XMConvertToRadians(5.0f) * elapsedTime;
    r.Angle.z = Calcu3D::ClampRotate(r.Angle.z);

#ifdef _DEBUG
    GuiMenu();
#endif

}

void BGColony::Render(ID3D11DeviceContext* dc)
{
    Model->Render(dc, t, r);
}

void BGColony::GuiMenu()
{
    ImGui::Begin("Planet");
    ImGui::DragFloat3("Position", &t.Position.x);
    ImGui::DragFloat3("Scale", &t.Scale.x);

    DirectX::XMFLOAT3 deg = {
        DirectX::XMConvertToDegrees(r.Angle.x),
       DirectX::XMConvertToDegrees(r.Angle.y),
       DirectX::XMConvertToDegrees(r.Angle.z)
    };
    ImGui::DragFloat3("Angle", &deg.x);

    r.Angle = {
        DirectX::XMConvertToRadians(deg.x),
        DirectX::XMConvertToRadians(deg.y),
        DirectX::XMConvertToRadians(deg.z)
    };


    ImGui::End();

}