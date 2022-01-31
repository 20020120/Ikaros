#include "Bullet.h"

#include "GameSystem.h"
#include "ProjectileManager.h"
#include "User_Function.h"
#include "EffectManager.h"

#include"MathSupport.h"
#include"User_Function.h"

Bullet::Bullet(ID3D11Device* d, Parent p, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 velocity, float speed)
{
    //--------------------<初期化>--------------------//
    //ModelRenderInitializer Ini;
    //Ini.SetALLPath(
    //    "./resources/Models/Characters/Players/PlayerModel.nk",
    //    "./resources/Shaders/SkinnedMesh_VS.cso",
    //    "./resources/Shaders/LambertPS.cso",
    //    "./resources/Shaders/GeometryShader.cso"
    //);

   

    //--------------------<位置を初期化>--------------------//
    t.Position = pos;

    t.Scale = {
      0.01f,
      0.01f,
      0.01f
    };

    //--------------------<速度を初期化>--------------------//
    Velocity = velocity;

    this->speed = defaultSpeed = speed;

    //--------------------<当たり判定用サイズを初期化>--------------------//
    hitAreaSize={1.5f,1.5f,1.5f};

    //--------------------<向きを初期化>--------------------//
    //正規化
    Velocity = Calcu3D::Normalize(Velocity);

    //自身の前方向を求める
    constexpr DirectX::XMFLOAT3 front = { 0,0,1 };

    //回転角を求める
    if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::SIDE)
    {
        const float dot = (front.y * Velocity.y) + (front.z * Velocity.z);

        const float ang = acosf(dot);
        r.Angle.z += (Velocity.y > 0) ? -ang : ang;
       // r.Angle.z += DirectX::XMConvertToRadians(45);
    }
    else if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
    {
        const float dot = (front.x * Velocity.x) + (front.z * Velocity.z);
        const float ang = acosf(dot);
        r.Angle.z += (Velocity.x > 0) ? ang : -ang;
        r.Angle.z += DirectX::XMConvertToRadians(-90);
    }
    //--------------------<自分の親と自分が何者か設定>--------------------//
    parent = p;
    

    switch (p) {
    case Parent::NONE:
        break;
    case Parent::REDENEMY:
        projtype = ProjType::REDBULLET; 
        bullet = std::make_unique<Effect>("./resources/Effects/Bullet/RedEnemyBullet.efk");
        break;
    case Parent::BLUEENEMY:
        projtype = ProjType::BLUEBULLET;
        bullet = std::make_unique<Effect>("./resources/Effects/Bullet/BlueEnemyBullet.efk");
        break;
    case Parent::REDPLAYER://拡散弾
        projtype = ProjType::REDBULLET;
        lifeTimer = ProjectileManager::Instance().GetRedPlayerBulletLife();
        hitCount = 1;
        bullet = std::make_unique<Effect>("./resources/Effects/Bullet/RedPlayerBullet.efk");
        break;

    case Parent::BLUEPLAYER ://貫通弾
        projtype = ProjType::BLUEBULLET;
        lifeTimer = ProjectileManager::Instance().GetBluePlayerBulletLife();
        hitCount = 3;
        bullet = std::make_unique<Effect>("./resources/Effects/Bullet/BluePlayerBullet.efk");
        break;
    default:;
    }

	efcHandle = bullet->Play(t.Position, r.Angle);
    

    // モデルをロード
    //LoadModel(d, Ini);

}


void Bullet::Initialize()
{

}

void Bullet::Update(float elapsedTime)
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

    bullet->SetPosition(efcHandle, t.Position);
    //bullet->SetAngle(efcHandle, { DirectX::XMConvertToRadians(30),DirectX::XMConvertToRadians(30),DirectX::XMConvertToRadians(30) });


    //Model->Update(elapsedTime);
}

void Bullet::Render(ID3D11DeviceContext* dc)
{
    //Model->Render(dc, t, r);

    //当たり判定用
#ifdef _DEBUG
    DirectX::XMFLOAT4X4 world{};
    const DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(1,1,1) };
    const DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(0,0,0) };
    const DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(t.Position.x,t.Position.y,t.Position.z) };
    DirectX::XMStoreFloat4x4(&world, S * R * T);
    //hitAreaBox->Render(dc, world, { 1,1,1,1 });
#endif
}


void Bullet::Finalize()
{
}

Bullet::~Bullet()
{
    bullet->Stop(efcHandle);
}


void Bullet::Move(float elapsedTime)
{
    t.Position.x += Velocity.x * speed * elapsedTime;
    t.Position.y += Velocity.y * speed * elapsedTime;
    t.Position.z += Velocity.z * speed * elapsedTime;

    if (t.Position.z < -60.0f)
    {
        ProjectileManager::Instance().Remove(this);
    }
}


//****************************************************************
// 
//  誘導弾
// 
//****************************************************************

// FirstVecは最初に進む向き（正か負で判定）
HomingBullet::HomingBullet(ID3D11Device* d, Parent p, DirectX::XMFLOAT3 pos,int FirstVec,float ratio) 
{
    //--------------------<初期化>--------------------//
    //ModelRenderInitializer Ini;
    //Ini.SetALLPath(
    //    "./resources/Models/Characters/Players/PlayerModel.nk",
    //    "./resources/Shaders/SkinnedMesh_VS.cso",
    //    "./resources/Shaders/LambertPS.cso",
    //    "./resources/Shaders/GeometryShader.cso"
    //);



    //--------------------<位置を初期化>--------------------//
    t.Position = pos;

    t.Scale = {
      0.01f,
      0.01f,
      0.01f
    };

    HomingRatio = ratio;
    //--------------------<速度を初期化>--------------------//

    const auto t = ProjectileManager::Instance().GetTarget();

    if(FirstVec>0)
    {
        switch (GameSystem::Instance().GetCameraDirection()) {
        case GameSystem::CAMERA_DIRECTION::SIDE: 
            Velocity = { 0.0f,5.0f,0.0f };
            break;
        case GameSystem::CAMERA_DIRECTION::TOP: 
            Velocity = { 5.0f,0.0f,0.0f };
            break;
        default: ;
        }
    }
    else
    {
        switch (GameSystem::Instance().GetCameraDirection()) {
        case GameSystem::CAMERA_DIRECTION::SIDE: 
            Velocity = { 0.0f,-5.0f,0.0f };
            break;
        case GameSystem::CAMERA_DIRECTION::TOP: 
            Velocity = { -5.0f,0.0f,0.0f };
            break;
        default:;
        }
    }
    Velocity.z = -0.2f;


    //--------------------<当たり判定用サイズを初期化>--------------------//
    hitAreaSize = { 1.5f,1.5f,1.5f };

    //--------------------<向きを初期化>--------------------//
    //正規化
    //Velocity = Calcu3D::Normalize(Velocity);

    //自身の前方向を求める
    constexpr DirectX::XMFLOAT3 front = { 0,0,1 };

    //回転角を求める
    if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::SIDE)
    {
        const float dot = (front.y * Velocity.y) + (front.z * Velocity.z);
        const float ang = acosf(dot);
        r.Angle.x += (Velocity.y > 0) ? -ang : ang;
    }
    else if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
    {
        const float dot = (front.x * Velocity.x) + (front.z * Velocity.z);
        const float ang = acosf(dot);
        r.Angle.y += (Velocity.x > 0) ? ang : -ang;
    }
    //--------------------<自分の親と自分が何者か設定>--------------------//
    parent = p;

    switch (p) {
    case Parent::NONE:
        break;
    case Parent::REDENEMY:
        projtype = ProjType::REDBULLET;
        bullet = std::make_unique<Effect>("./resources/Effects/Bullet/RedEnemyBullet.efk");
        break;
    case Parent::BLUEENEMY:
        projtype = ProjType::BLUEBULLET;
        bullet = std::make_unique<Effect>("./resources/Effects/Bullet/BlueEnemyBullet.efk");
        break;
 
    default:;
    }

    // モデルをロード
    //LoadModel(d, Ini);
    efcHandle = bullet->Play(pos, r.Angle);


}

void HomingBullet::Initialize()
{
    
}

void HomingBullet::Update(float elapsedTime)
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

    bullet->SetPosition(efcHandle, t.Position);

    //Model->Update(elapsedTime);
}

void HomingBullet::Move(float elapsedTime)
{
    //--------------------< ホーミング>--------------------//

     // ターゲット位置を取得
    const DirectX::XMFLOAT3 tar = ProjectileManager::Instance().GetTarget();

    DirectX::XMFLOAT3 toTargetVec = tar - t.Position;
    toTargetVec = Calcu3D::Normalize(toTargetVec);

    const DirectX::XMFLOAT3 v = Calcu3D::LerpFloat3(Velocity, toTargetVec, HomingRatio*elapsedTime);
    Velocity.x = v.x;
    Velocity.y = v.y;
    Velocity.z = std::min(Velocity.z, v.z);

    t.Position.x += Velocity.x * speed * elapsedTime;
    t.Position.y += Velocity.y * speed * elapsedTime;
    t.Position.z += Velocity.z * speed * elapsedTime;

    if (t.Position.z < -60.0f)
    {
        ProjectileManager::Instance().Remove(this);
    }
}

void HomingBullet::Render(ID3D11DeviceContext* dc)
{

    //Model->Render(dc, t, r);

}

void HomingBullet::Finalize()
{
    bullet->Stop(efcHandle);
}

HomingBullet::~HomingBullet()
{
	HomingBullet::Finalize();
}

//****************************************************************
// 
//  ビーム
// 
//****************************************************************
Beam::Beam(ID3D11Device* d, Parent p, DirectX::XMFLOAT3 pos)
{
    //--------------------< 初期化>--------------------//
    // 位置
    t.Position = pos;
    // 当たり判定
    hitAreaSize.x = 15.0f;
    hitAreaSize.y = 15.0f;
    hitAreaSize.z = 0.0f;

    // 生存時間を設定
    lifeTimer = 3.0f;

    //--------------------<自分の親と自分が何者か設定>--------------------//
    parent = p;

    switch (p) {
    case Parent::NONE:
        break;
    case Parent::REDENEMY:
        projtype = ProjType::REDBULLET;
        break;
    case Parent::BLUEENEMY:
        projtype = ProjType::BLUEBULLET;
        break;
    default:;
    }


    beam = std::make_unique<Effect>("./resources/Effects/laser_beam.efk");
    DirectX::XMFLOAT3 angle{};

    switch (p) {
    case Parent::NONE:
        break;
    case Parent::REDENEMY:
        break;
    case Parent::BLUEENEMY:
        break;
    case Parent::REDPLAYER:

        angle.y = DirectX::XMConvertToRadians(180.0f);
        break;
    case Parent::BLUEPLAYER:
        angle.y = DirectX::XMConvertToRadians(180.0f);
        break;
    default:
        break;
    }

    efcHandle = beam->Play(pos, angle, 0.1f);
}

void Beam::Initialize()
{
    
}

void Beam::Update(float elapsedTime)
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

    Move(elapsedTime);
}

void Beam::Move(float elapsedTime)
{
    // ビームの位置と大きさを更新
    hitAreaSize.z += BeamSpeed * elapsedTime;
}

void Beam::Render(ID3D11DeviceContext* dc)
{

  
}

void Beam::Finalize()
{
}

Beam::~Beam()
{
    beam->Stop(efcHandle);
}


MoveBeam::MoveBeam(ID3D11Device* d, Parent p, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 speed)
{
    //--------------------< 初期化>--------------------//
    // 位置
    t.Position = pos;
    // 当たり判定
    hitAreaSize.x = 15.0f;
    hitAreaSize.y = 15.0f;
    hitAreaSize.z = 0.0f;

    Speed = speed;

    // 生存時間を設定
    lifeTimer = 3.0f;

    //--------------------<自分の親と自分が何者か設定>--------------------//
    parent = p;

    switch (p) {
    case Parent::NONE:
        break;
    case Parent::REDENEMY:
        projtype = ProjType::REDBULLET;
        break;
    case Parent::BLUEENEMY:
        projtype = ProjType::BLUEBULLET;
        break;
    default:;
    }


    beam = std::make_unique<Effect>("./resources/Effects/Beam/BeamBlue.efk");
    DirectX::XMFLOAT3 angle{};

    switch (p) {
    case Parent::NONE:
        break;
    case Parent::REDENEMY:
        break;
    case Parent::BLUEENEMY:
        break;
    case Parent::REDPLAYER:

        angle.y = DirectX::XMConvertToRadians(180.0f);
        break;
    case Parent::BLUEPLAYER:
        angle.y = DirectX::XMConvertToRadians(180.0f);
        break;
    default:
        break;
    }

    efcHandle = beam->Play(pos, angle);
}

MoveBeam::~MoveBeam()
{
    Finalize();
}

void ::MoveBeam::Initialize()
{
    BaseProjectile::Initialize();
}

void MoveBeam::Update(float elapsed_time)
{
    //無敵時間更新
    invTimer -= elapsed_time;

    //生存時間更新
    lifeTimer -= elapsed_time;

    Move(elapsed_time);

    if (lifeTimer < 0.0f)
    {
        ProjectileManager::Instance().Remove(this);

        return;
    }

    
}

void MoveBeam::Render(ID3D11DeviceContext* dc)
{
    BaseProjectile::Render(dc);
}

void MoveBeam::Finalize()
{
    beam->Stop(efcHandle);
}

void MoveBeam::Move(float elapsedTime)
{
    // ビームの位置と大きさを更新
    hitAreaSize.z += BeamSpeed * elapsedTime;

    t.Position += Speed * elapsedTime;
    beam->SetPosition(efcHandle, t.Position);
}



//****************************************************************
// 
//  曲がる弾
// 
//****************************************************************
CurveBullet::CurveBullet( Parent p, DirectX::XMFLOAT3 pos, int FirstVec, int view)
{
    // 初期化
    View = view;
    t.Position = pos;
    WidthSpeed *= FirstVec;
    lifeTimer = 7.0f;
    parent = p;
    switch (p) {
    case Parent::NONE:
        break;
    case Parent::REDENEMY:
        projtype = ProjType::REDBULLET;
        bullet = std::make_unique<Effect>("./resources/Effects/Bullet/RedEnemyBullet.efk");
        break;
    case Parent::BLUEENEMY:
        projtype = ProjType::BLUEBULLET;
        bullet = std::make_unique<Effect>("./resources/Effects/Bullet/BlueEnemyBullet.efk");
        break;

    default:;
    }

    //--------------------<当たり判定用サイズを初期化>--------------------//
    hitAreaSize = { 1.5f,1.5f,1.5f };

    efcHandle = bullet->Play(pos, r.Angle);

    // 速度はランダム
    float pow = static_cast<float>(rand()) / RAND_MAX + 0.5f;
    speed *= pow;

}

CurveBullet::~CurveBullet()
{
    bullet->Stop(efcHandle);
}

void CurveBullet::Initialize()
{
    BaseProjectile::Initialize();
}

void CurveBullet::Update(float elapsed_time)
{

    //無敵時間更新
    invTimer -= elapsed_time;

    //生存時間更新
    lifeTimer -= elapsed_time;
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

    Move(elapsed_time);

    bullet->SetPosition(efcHandle, t.Position);
}

void CurveBullet::Render(ID3D11DeviceContext* dc)
{

}

void CurveBullet::Finalize()
{
    BaseProjectile::Finalize();
}

void CurveBullet::Move(float elapsedTime)
{
    t.Position.z -= speed * elapsedTime;  // 前進

    if(t.Position.z>=30.0f|| t.Position.z <= -30.0f)
    {
        speed *= -1.0f;
    }

    switch (GameSystem::Instance().GetCameraDirection())
    {
    case GameSystem::CAMERA_DIRECTION::SIDE:
        if (t.Position.y >= 40.0f || t.Position.y <= -40.0f)
        {
            WidthSpeed *= -1.0f;
        }

        t.Position.y += WidthSpeed * elapsedTime;
        break;
    case GameSystem::CAMERA_DIRECTION::TOP:
        if (t.Position.x >= 40.0f || t.Position.x <= -40.0f)
        {
            WidthSpeed *= -1.0f;
        }
        t.Position.x += WidthSpeed * elapsedTime;
        break;
    default: ;
    }

    
   



}


//****************************************************************
// 
//  停止からの追尾弾
// 
//****************************************************************
StayToTargetBullet::StayToTargetBullet(Parent p, DirectX::XMFLOAT3 pos, int view, DirectX::XMFLOAT3 MiddleTargetPos_,
    float WaitSecond)
{
    // 初期化
    View = view;
    t.Position = pos;
    lifeTimer = 7.0f;
    parent = p;
    MiddleTargetPos = MiddleTargetPos_;
    State = 0;
    ParentPos = t.Position;
    StackWaitSeconds = WaitSecond;
    switch (p) {
    case Parent::NONE:
        break;
    case Parent::REDENEMY:
        projtype = ProjType::REDBULLET;
        bullet = std::make_unique<Effect>("./resources/Effects/Bullet/RedEnemyBullet.efk");
        break;
    case Parent::BLUEENEMY:
        projtype = ProjType::BLUEBULLET;
        bullet = std::make_unique<Effect>("./resources/Effects/Bullet/BlueEnemyBullet.efk");
        break;

    default:;
    }

    //--------------------<当たり判定用サイズを初期化>--------------------//
    hitAreaSize = { 1.5f,1.5f,1.5f };

    efcHandle = bullet->Play(pos, r.Angle);

}

StayToTargetBullet::~StayToTargetBullet()
{
    bullet->Stop(efcHandle);
}

void StayToTargetBullet::Initialize()
{
    BaseProjectile::Initialize();
}

void StayToTargetBullet::Update(float elapsed_time)
{
    //無敵時間更新
    invTimer -= elapsed_time;

    //生存時間更新
    lifeTimer -= elapsed_time;
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

    Move(elapsed_time);

    bullet->SetPosition(efcHandle, t.Position);
}

void StayToTargetBullet::Render(ID3D11DeviceContext* dc)
{
}

void StayToTargetBullet::Finalize()
{
    
}

void StayToTargetBullet::Move(float elapsedTime)
{
    switch (State)
    {
    case 0:
        // 中間地点に移動
        Ratio += elapsedTime * 2.0f;
        t.Position = Calcu3D::LerpFloat3(ParentPos, MiddleTargetPos, Ratio);

        if(Ratio>=1.0f)
        {
            Ratio = 0.0f;
            State++;
        }
        break;
    case 1:
        // ちょっとたいき

        Ratio += elapsedTime;
        if (Ratio >= StackWaitSeconds)
        {
            Ratio = 0.0f;
            State++;
        }
        break;
    case 2:
        // ターゲットを初期化
        TargetPos = ProjectileManager::Instance().GetTarget();
        State++;
        /*FallThrough*/
    case 3:
        // 移動ベクトルを算出し移動
        DirectX::XMFLOAT3 moveVec{};
        moveVec = TargetPos - MiddleTargetPos;
        moveVec = Calcu3D::Normalize(moveVec);

        t.Position += speed * moveVec * elapsedTime;
        break;
    default: ;
    }

}

//****************************************************************
// 
//  ブーメランの弾
// 
//****************************************************************

BoomerangBullet::BoomerangBullet(Parent p, DirectX::XMFLOAT3 pos,  DirectX::XMFLOAT3 FirstSpeed)
{
    // 初期化
    speed = FirstSpeed;
    t.Position = pos;
    lifeTimer = 7.0f;
    parent = p;
    State = 0;
    switch (p) {
    case Parent::NONE:
        break;
    case Parent::REDENEMY:
        projtype = ProjType::REDBULLET;
        bullet = std::make_unique<Effect>("./resources/Effects/Bullet/RedEnemyBullet.efk");
        break;
    case Parent::BLUEENEMY:
        projtype = ProjType::BLUEBULLET;
        bullet = std::make_unique<Effect>("./resources/Effects/Bullet/BlueEnemyBullet.efk");
        break;

    default:;
    }

    //--------------------<当たり判定用サイズを初期化>--------------------//
    hitAreaSize = { 1.5f,1.5f,1.5f };

    efcHandle = bullet->Play(pos, r.Angle);
}

BoomerangBullet::~BoomerangBullet()
{
    bullet->Stop(efcHandle);
}

void BoomerangBullet::Initialize()
{
    BaseProjectile::Initialize();
}

void BoomerangBullet::Update(float elapsed_time)
{
    //無敵時間更新
    invTimer -= elapsed_time;

    //生存時間更新
    lifeTimer -= elapsed_time;
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

    Move(elapsed_time);

    bullet->SetPosition(efcHandle, t.Position);
}

void BoomerangBullet::Render(ID3D11DeviceContext* dc)
{
}

void BoomerangBullet::Finalize()
{
    BaseProjectile::Finalize();
}

void BoomerangBullet::Move(float elapsedTime)
{
    switch (GameSystem::Instance().GetCameraDirection()) {

    case GameSystem::CAMERA_DIRECTION::SIDE:
        // Y軸を更新
        t.Position.y += speed.y * elapsedTime;
        t.Position.z += speed.z * elapsedTime;

        speed.z += 70.0f * elapsedTime;
        speed.y -= 30.0f * elapsedTime;
        break;
    case GameSystem::CAMERA_DIRECTION::TOP:

        t.Position.x += speed.y * elapsedTime;
        t.Position.z += speed.z * elapsedTime;

        speed.z -= -20.0f * elapsedTime;
        speed.x -= 20.0f * elapsedTime;

        break;
    default:;
    }
}
