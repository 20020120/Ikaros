#include "Energy.h"
#include "ProjectileManager.h"
#include"User_Function.h"
Energy::Energy(ID3D11Device* d, Parent p, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 velocity)
{
    //--------------------<初期化>--------------------//
    ModelRenderInitializer Ini;
    Ini.SetALLPath(
        "./resources/Models/Characters/Energies/EnargyRed.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/LambertPS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );


    //--------------------<位置を初期化>--------------------//
    t.Position = pos;
    t.Scale = { 2.0f,2.0f,2.0f };

    //--------------------<速度を初期化>--------------------//
    Velocity = velocity;

    //--------------------<当たり判定>--------------------//
    hitAreaSize = { 6.0f,6.0f,6.0f };


    
    //--------------------<自分の親と自分が何者か設定>--------------------//
    parent = p;

    switch (p) {
    case Parent::NONE:
        break;
    case Parent::REDENEMY:
        projtype = ProjType::REDENERGY;
        Ini.SetModelPath("./resources/Models/Characters/Energies/EnargyRed.nk"); // モデルのパスを設定
        //mEmisiveTexture.Register(d, 5, L"./resources/Models/Characters/Energies/M_ene_akaaka_anime.fbm/lambert7_Emissive.png");
        break;
    case Parent::BLUEENEMY:
        projtype = ProjType::BLUEENERGY;
        Ini.SetModelPath("./resources/Models/Characters/Energies/EnergyBlue.nk"); // モデルのパスを設定
        break;
    default:;
    }

    // モデルをロード
    LoadModel(d, Ini);

    hitCount = 1;

    lifeTimer = 7.0f;
}

Energy::~Energy()
{
    
}


void Energy::Initialize()
{
}

void Energy::Update(float elapsedTime)
{
    //無敵時間更新
    invTimer -= elapsedTime;

    //生存時間更新
    lifeTimer -= elapsedTime;
    if (lifeTimer < 0.0f)
    {
        ProjectileManager::Instance().Remove(this);
        return;
    }

    //Hit回数値が0になってたら破棄
    if (hitCount <= 0)
    {
        ProjectileManager::Instance().Remove(this);
        return;
    }

    Move(elapsedTime);
    Model->Update(elapsedTime);

    // 回転
    r.Angle.z += DirectX::XMConvertToRadians(360.0f) * elapsedTime;
    r.Angle.z = Calcu3D::ClampRotate(r.Angle.z);

    r.Angle.y += DirectX::XMConvertToRadians(180.0f) * elapsedTime;
    r.Angle.y = Calcu3D::ClampRotate(r.Angle.z);

    r.Angle.x += DirectX::XMConvertToRadians(90.0f) * elapsedTime;
    r.Angle.x = Calcu3D::ClampRotate(r.Angle.z);
}

void Energy::Render(ID3D11DeviceContext* dc)
{

    if(parent==Parent::REDENEMY)
    {
        //mEmisiveTexture.SetTexture(dc);
    }

    if(lifeTimer >= 2.0f)
    {
        Model->Render(dc, t, r);
    }
    else if(static_cast<int>(lifeTimer * 10.0f) % 3 == 0)
    {
        Model->Render(dc, t, r);
    }

    //当たり判定用
#ifdef _DEBUG
    DirectX::XMFLOAT4X4 world{};
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(1,1,1) };
    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(0,0,0) };
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(t.Position.x,t.Position.y,t.Position.z) };
    DirectX::XMStoreFloat4x4(&world, S * R * T);
    //hitAreaBox->Render(dc, world, { 1,1,1,1 });
#endif

}

void Energy::Finalize()
{
    
}

void Energy::Move(float elapsedTime)
{

    t.Position.x += Velocity.x * speed * elapsedTime;
    t.Position.y += Velocity.y * speed * elapsedTime;
    t.Position.z += Velocity.z * speed * elapsedTime;

    if (t.Position.z < -60.0f)
    {
        ProjectileManager::Instance().Remove(this);
    }
}




