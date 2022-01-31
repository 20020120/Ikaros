#include "Bullet.h"

#include "GameSystem.h"
#include "ProjectileManager.h"
#include "User_Function.h"
#include "EffectManager.h"

#include"MathSupport.h"
#include"User_Function.h"

Bullet::Bullet(ID3D11Device* d, Parent p, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 velocity, float speed)
{
    //--------------------<������>--------------------//
    //ModelRenderInitializer Ini;
    //Ini.SetALLPath(
    //    "./resources/Models/Characters/Players/PlayerModel.nk",
    //    "./resources/Shaders/SkinnedMesh_VS.cso",
    //    "./resources/Shaders/LambertPS.cso",
    //    "./resources/Shaders/GeometryShader.cso"
    //);

   

    //--------------------<�ʒu��������>--------------------//
    t.Position = pos;

    t.Scale = {
      0.01f,
      0.01f,
      0.01f
    };

    //--------------------<���x��������>--------------------//
    Velocity = velocity;

    this->speed = defaultSpeed = speed;

    //--------------------<�����蔻��p�T�C�Y��������>--------------------//
    hitAreaSize={1.5f,1.5f,1.5f};

    //--------------------<������������>--------------------//
    //���K��
    Velocity = Calcu3D::Normalize(Velocity);

    //���g�̑O���������߂�
    constexpr DirectX::XMFLOAT3 front = { 0,0,1 };

    //��]�p�����߂�
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
    //--------------------<�����̐e�Ǝ��������҂��ݒ�>--------------------//
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
    case Parent::REDPLAYER://�g�U�e
        projtype = ProjType::REDBULLET;
        lifeTimer = ProjectileManager::Instance().GetRedPlayerBulletLife();
        hitCount = 1;
        bullet = std::make_unique<Effect>("./resources/Effects/Bullet/RedPlayerBullet.efk");
        break;

    case Parent::BLUEPLAYER ://�ђʒe
        projtype = ProjType::BLUEBULLET;
        lifeTimer = ProjectileManager::Instance().GetBluePlayerBulletLife();
        hitCount = 3;
        bullet = std::make_unique<Effect>("./resources/Effects/Bullet/BluePlayerBullet.efk");
        break;
    default:;
    }

	efcHandle = bullet->Play(t.Position, r.Angle);
    

    // ���f�������[�h
    //LoadModel(d, Ini);

}


void Bullet::Initialize()
{

}

void Bullet::Update(float elapsedTime)
{
    //���G���ԍX�V
    invTimer -= elapsedTime;

    //�������ԍX�V
    lifeTimer -= elapsedTime;
    if (lifeTimer < 0.0f)
    {
        ProjectileManager::Instance().Remove(this);
        return;
    }

    //Hit�񐔒l��0�ɂȂ��Ă���j��
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

    //�����蔻��p
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
//  �U���e
// 
//****************************************************************

// FirstVec�͍ŏ��ɐi�ތ����i�������Ŕ���j
HomingBullet::HomingBullet(ID3D11Device* d, Parent p, DirectX::XMFLOAT3 pos,int FirstVec,float ratio) 
{
    //--------------------<������>--------------------//
    //ModelRenderInitializer Ini;
    //Ini.SetALLPath(
    //    "./resources/Models/Characters/Players/PlayerModel.nk",
    //    "./resources/Shaders/SkinnedMesh_VS.cso",
    //    "./resources/Shaders/LambertPS.cso",
    //    "./resources/Shaders/GeometryShader.cso"
    //);



    //--------------------<�ʒu��������>--------------------//
    t.Position = pos;

    t.Scale = {
      0.01f,
      0.01f,
      0.01f
    };

    HomingRatio = ratio;
    //--------------------<���x��������>--------------------//

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


    //--------------------<�����蔻��p�T�C�Y��������>--------------------//
    hitAreaSize = { 1.5f,1.5f,1.5f };

    //--------------------<������������>--------------------//
    //���K��
    //Velocity = Calcu3D::Normalize(Velocity);

    //���g�̑O���������߂�
    constexpr DirectX::XMFLOAT3 front = { 0,0,1 };

    //��]�p�����߂�
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
    //--------------------<�����̐e�Ǝ��������҂��ݒ�>--------------------//
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

    // ���f�������[�h
    //LoadModel(d, Ini);
    efcHandle = bullet->Play(pos, r.Angle);


}

void HomingBullet::Initialize()
{
    
}

void HomingBullet::Update(float elapsedTime)
{
    //���G���ԍX�V
    invTimer -= elapsedTime;

    //�������ԍX�V
    lifeTimer -= elapsedTime;
    if (lifeTimer < 0.0f)
    {
        ProjectileManager::Instance().Remove(this);
        return;
    }

    //Hit�񐔒l��0�ɂȂ��Ă���j��
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
    //--------------------< �z�[�~���O>--------------------//

     // �^�[�Q�b�g�ʒu���擾
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
//  �r�[��
// 
//****************************************************************
Beam::Beam(ID3D11Device* d, Parent p, DirectX::XMFLOAT3 pos)
{
    //--------------------< ������>--------------------//
    // �ʒu
    t.Position = pos;
    // �����蔻��
    hitAreaSize.x = 15.0f;
    hitAreaSize.y = 15.0f;
    hitAreaSize.z = 0.0f;

    // �������Ԃ�ݒ�
    lifeTimer = 3.0f;

    //--------------------<�����̐e�Ǝ��������҂��ݒ�>--------------------//
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
    //���G���ԍX�V
    invTimer -= elapsedTime;

    //�������ԍX�V
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
    // �r�[���̈ʒu�Ƒ傫�����X�V
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
    //--------------------< ������>--------------------//
    // �ʒu
    t.Position = pos;
    // �����蔻��
    hitAreaSize.x = 15.0f;
    hitAreaSize.y = 15.0f;
    hitAreaSize.z = 0.0f;

    Speed = speed;

    // �������Ԃ�ݒ�
    lifeTimer = 3.0f;

    //--------------------<�����̐e�Ǝ��������҂��ݒ�>--------------------//
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
    //���G���ԍX�V
    invTimer -= elapsed_time;

    //�������ԍX�V
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
    // �r�[���̈ʒu�Ƒ傫�����X�V
    hitAreaSize.z += BeamSpeed * elapsedTime;

    t.Position += Speed * elapsedTime;
    beam->SetPosition(efcHandle, t.Position);
}



//****************************************************************
// 
//  �Ȃ���e
// 
//****************************************************************
CurveBullet::CurveBullet( Parent p, DirectX::XMFLOAT3 pos, int FirstVec, int view)
{
    // ������
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

    //--------------------<�����蔻��p�T�C�Y��������>--------------------//
    hitAreaSize = { 1.5f,1.5f,1.5f };

    efcHandle = bullet->Play(pos, r.Angle);

    // ���x�̓����_��
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

    //���G���ԍX�V
    invTimer -= elapsed_time;

    //�������ԍX�V
    lifeTimer -= elapsed_time;
    if (lifeTimer < 0.0f)
    {
        ProjectileManager::Instance().Remove(this);
        return;
    }

    //Hit�񐔒l��0�ɂȂ��Ă���j��
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
    t.Position.z -= speed * elapsedTime;  // �O�i

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
//  ��~����̒ǔ��e
// 
//****************************************************************
StayToTargetBullet::StayToTargetBullet(Parent p, DirectX::XMFLOAT3 pos, int view, DirectX::XMFLOAT3 MiddleTargetPos_,
    float WaitSecond)
{
    // ������
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

    //--------------------<�����蔻��p�T�C�Y��������>--------------------//
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
    //���G���ԍX�V
    invTimer -= elapsed_time;

    //�������ԍX�V
    lifeTimer -= elapsed_time;
    if (lifeTimer < 0.0f)
    {
        ProjectileManager::Instance().Remove(this);
        return;
    }

    //Hit�񐔒l��0�ɂȂ��Ă���j��
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
        // ���Ԓn�_�Ɉړ�
        Ratio += elapsedTime * 2.0f;
        t.Position = Calcu3D::LerpFloat3(ParentPos, MiddleTargetPos, Ratio);

        if(Ratio>=1.0f)
        {
            Ratio = 0.0f;
            State++;
        }
        break;
    case 1:
        // ������Ƃ�����

        Ratio += elapsedTime;
        if (Ratio >= StackWaitSeconds)
        {
            Ratio = 0.0f;
            State++;
        }
        break;
    case 2:
        // �^�[�Q�b�g��������
        TargetPos = ProjectileManager::Instance().GetTarget();
        State++;
        /*FallThrough*/
    case 3:
        // �ړ��x�N�g�����Z�o���ړ�
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
//  �u�[�������̒e
// 
//****************************************************************

BoomerangBullet::BoomerangBullet(Parent p, DirectX::XMFLOAT3 pos,  DirectX::XMFLOAT3 FirstSpeed)
{
    // ������
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

    //--------------------<�����蔻��p�T�C�Y��������>--------------------//
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
    //���G���ԍX�V
    invTimer -= elapsed_time;

    //�������ԍX�V
    lifeTimer -= elapsed_time;
    if (lifeTimer < 0.0f)
    {
        ProjectileManager::Instance().Remove(this);
        return;
    }

    //Hit�񐔒l��0�ɂȂ��Ă���j��
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
        // Y�����X�V
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
