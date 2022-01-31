#include"Light.h"
#include<imgui.h>

#include "User_Function.h"
#include"MathSupport.h"
//****************************************************************
// 
// ディレクションライト
// 
//****************************************************************
DirectionLight::DirectionLight(ID3D11Device* d)
{
    
    data.direction = { -1.0f,-0.5f,1.0f };

    // 正規化
    DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&data.direction);
    dir = DirectX::XMVector3Normalize(dir);
    DirectX::XMStoreFloat3(&data.direction, dir);

    data.Color = { 1.0f,1.0f,1.0f,0.0f };
    data.Power = 1.0f;

    cb.Init(d, &data);
    cb.Set_PSSlot(2); // 定数バッファの２は必ずディレクションライト

}

DirectionLight::~DirectionLight()
{
}

void DirectionLight::Update(ID3D11DeviceContext* dc)
{
    data.Power = std::min(1.0f, std::max(0.0f, data.Power));
    data.direction = Calcu3D::Normalize(data.direction);
    cb.SetBuffer(dc, &data, SetBufferType::PIXEL);
    GuiMenu();
}

void DirectionLight::GuiMenu()
{
#ifdef _DEBUG
    ImGui::Begin("DirectionLight");
    ImGui::DragFloat3("Direction", &data.direction.x, 0.01f);
    ImGui::SliderFloat("Power", &data.Power, 0.0f, 1.0f);
    ImGui::End();
#endif

}


//****************************************************************
// 
// ポイントライト
// 
//****************************************************************
PointLight::PointLight(ID3D11Device* d,int Ps_Slot)
{
    this->Ps_Slot = Ps_Slot;
    ModelRenderInitializer Ini;
    Ini.SetALLPath(
        "./resources/Models/QuadParticle.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/LambertPS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );

    for (int i=0; i < Value; i++)
    {
        data.Position[i] = { 0.0f,-1.5f,0.0f,0.0f };
        data.Color[i] = { 1.0f,0.0f,0.0f,0.0f };
        data.InfluenceRange[i] = { 10.0f,0.0f,0.0f,0.0f };
        //Model[i] = std::make_unique<ModelComponent>(d, Ini);
    }
    c_buf.Init(d, &data);
    c_buf.Set_PSSlot(this->Ps_Slot);

}


PointLight::~PointLight()
{
}

void PointLight::Update(ID3D11DeviceContext* dc)
{
#ifdef _DEBUG


    const std::string Name = "PointLight" + std::to_string(Ps_Slot);
    ImGui::Begin(Name.c_str());

    static float r = 0;
    ImGui::DragFloat("Range_All", &r);
    for (int i = 0; i < Value; i++)
    {
        ImGui::DragFloat3(("position"+ std::to_string(i)).c_str(), &data.Position[i].x, 0.1f);
        ImGui::DragFloat3(("color" + std::to_string(i)).c_str(), &data.Color[i].x, 0.1f);
       /* ImGui::DragFloat(("Range" + std::to_string(i)).c_str(), &data.InfluenceRange[i].x, 1.0f);
        data.InfluenceRange[i].x = r;*/

    }




    ImGui::End();
#endif

    for (int i = 0; i < Value; i++)
    {
        Transform t;
        t.Position={
            data.Position[i].x,
            data.Position[i].y,
            data.Position[i].z
        };

        t.Scale = { 1.0f,1.0f,1.0f };
        Rotate r;
        //Model[i]->Render(dc, t, r);
    }

    c_buf.SetBuffer(dc, &data, SetBufferType::PIXEL);
 
}

void PointLight::SetPosition(DirectX::XMFLOAT3 pos,int Index)
{
    data.Position[Index] = {
        pos.x,
        pos.y,
        pos.z,
        0.0f
    };
}

void PointLight::SetColor(DirectX::XMFLOAT3 col, int Index)
{
    data.Color[Index] = {
        col.x,
        col.y,
        col.z,
        1.0f
    };

}

void PointLight::SetRange(float range, int Index)
{
    data.InfluenceRange[Index].x = range;
}



//****************************************************************
// 
//  ゲーム用ポイントライト
// 
//****************************************************************

GamePointLight::GamePointLight(ID3D11Device*d,int Index)
{
    this->Ps_Slot = Index;
    ModelRenderInitializer Ini;
    Ini.SetALLPath(
        "./resources/Models/QuadParticle.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/LambertPS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );

    
        data.Position = { 0.0f,-1.5f,0.0f,0.0f };
        data.Color = { 1.0f,0.0f,0.0f,0.0f };
        data.InfluenceRange = { 10.0f,0.0f,0.0f,0.0f };
        //Model = std::make_unique<ModelComponent>(d, Ini);
    
    c_buf.Init(d, &data);
    c_buf.Set_PSSlot(this->Ps_Slot);
}

GamePointLight::~GamePointLight()
{
}

void GamePointLight::SetPosition(DirectX::XMFLOAT3 pos)
{
    data.Position = {
        pos.x,
        pos.y,
        pos.z,
        0.0f
    };

}

void GamePointLight::SetColor(DirectX::XMFLOAT3 col)
{
    data.Color = {
        col.x,
        col.y,
        col.z,
        1.0f
    };
}

void GamePointLight::SetRange(float range)
{
    data.InfluenceRange.x = range;
}

void GamePointLight::Update(ID3D11DeviceContext* dc)
{

#ifdef _DEBUG
    ImGui::DragFloat3(("Pos"+std::to_string(Ps_Slot)).c_str(), &data.Position.x);
    ImGui::DragFloat(("range" + std::to_string(Ps_Slot)).c_str(), &data.InfluenceRange.x);
    ImGui::SliderFloat3(("Color" + std::to_string(Ps_Slot)).c_str(), &data.Color.x, 0.0f, 1.0f, " % .01f");
#endif


    Transform t;
    t.Position = {
        data.Position.x,
        data.Position.y,
        data.Position.z
    };

    t.Scale = { 1.0f,1.0f,1.0f };
    Rotate r;
   // Model->Render(dc, t, r);

    c_buf.SetBuffer(dc, &data, SetBufferType::PIXEL);
}





//****************************************************************
// 
//  スポットライト
// 
//****************************************************************

SpotLight::SpotLight(ID3D11Device* d,int Slot)
{
    data.Position = { 0.0f,5.0f,0.0f,0.0f };
    data.Angle = DirectX::XMConvertToRadians(0.0f);
    data.Color = { 10.0f,10.0f,10.0f,0.0f };
    data.Direction = { 1.0f,-1.0f,1.0f };
    DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&data.Direction);
    dir = DirectX::XMVector3Normalize(dir);
    DirectX::XMStoreFloat3(&data.Direction, dir);

    data.Range = { 80.0f };
    

    c_buf.Init(d, &data);
    c_buf.Set_PSSlot(Slot);
}

SpotLight::~SpotLight()
{
}

void SpotLight::Update(ID3D11DeviceContext*dc)
{

#ifdef _DEBUG
    ImGui::Begin("SpotLight");
    ImGui::DragFloat3("Position", &data.Position.x, 0.1f);
    ImGui::DragFloat3("Direction", &data.Direction.x, 0.1f);
    float deg = DirectX::XMConvertToDegrees(data.Angle);
    ImGui::DragFloat("Angle", &deg, 5.0f);
    data.Angle = DirectX::XMConvertToRadians(deg);
    ImGui::DragFloat("Range", &data.Range, 1.0f);
    ImGui::DragFloat3("Color", &data.Color.x, 0.1f);
    ImGui::End();
#endif


    c_buf.SetBuffer(dc, &data, PIXEL);

}
