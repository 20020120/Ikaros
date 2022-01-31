#include"Decoration.h"
#include"MathSupport.h"
#include"User_Function.h"
//****************************************************************
//
//  隕石
// 
//****************************************************************


Meteorite::Meteorite(ID3D11Device* d, int ModelType, DirectX::XMFLOAT3 IniPosition)
{

    ModelRenderInitializer Ini;
    Ini.SetALLPath(
        "./resources/Models/Stages/Meteorite1.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/LambertPS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );



    if (ModelType == 0)
    {
        Ini.SetModelPath("./resources/Models/Stages/Meteorite2.nk");
    }

    Model = std::make_unique<ModelComponent>(d, Ini);

    // ランダムで回転を決定
    RotSpeed = {
        static_cast<float>(rand() / RAND_MAX)*7.0f,
        static_cast<float>(rand() / RAND_MAX)*7.0f,
        static_cast<float>(rand() / RAND_MAX)*7.0f
    };


    t.Position = IniPosition;

    t.Scale = { 0.7f,0.7f,0.7f };

    Speed = (static_cast<float>(rand() % 3)+2) * 5.0f;
}

Meteorite::~Meteorite()
{
}

void Meteorite::Update(float elapsedTime)
{
    // 一定ペースで回転
    r.Angle += RotSpeed * elapsedTime;

    // 回転値を補正
    r.Angle.x = Calcu3D::ClampRotate(r.Angle.x);
    r.Angle.y = Calcu3D::ClampRotate(r.Angle.y);
    r.Angle.z = Calcu3D::ClampRotate(r.Angle.z);


    t.Position.z -= Speed * elapsedTime;

    if(t.Position.z<-400.0f)
    {
        t.Position.z = 400.0f;
    }
}

void Meteorite::Render(ID3D11DeviceContext* dc)
{
    Model->Render(dc, t, r);
}

DecorationManager::DecorationManager(ID3D11Device* d)
{
    Meteorite* m0 = new Meteorite(d, 0, { -160.0f,-5.0f,200.0f });
    MeteoriteVec.emplace_back(m0);

    Meteorite* m1 = new Meteorite(d, 1, { 0.0f,-70.0f,110.0f });
    MeteoriteVec.emplace_back(m1);

    Meteorite* m2 = new Meteorite(d, 1, { -200.0f,20.0f ,320.0f});
    MeteoriteVec.emplace_back(m2);

    Meteorite* m3 = new Meteorite(d, 0, { -400.0f,10.0f ,320.0f });
    MeteoriteVec.emplace_back(m3);
}

void DecorationManager::Update(float elapsedTime)
{
    for (int i=0;i< static_cast<int>(MeteoriteVec.size());i++)
    {
        MeteoriteVec[i]->Update(elapsedTime);
    }
}

void DecorationManager::Render(ID3D11DeviceContext* dc)
{
    for (int i = 0; i < static_cast<int>(MeteoriteVec.size()); i++)
    {
        MeteoriteVec[i]->Render(dc);
    }
}

