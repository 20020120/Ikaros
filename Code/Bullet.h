#pragma once

#include"BaseProjectile.h"
#include"Effect.h"

class Bullet :public BaseProjectile
{
public:
    Bullet(ID3D11Device* d,
        Parent p,
        DirectX::XMFLOAT3 pos,
        DirectX::XMFLOAT3 velocity,
		float speed = 30.0f);

    ~Bullet();

    void Initialize() override;
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;

private:
    //�֐�
    void Move(float elapsedTime);

    std::unique_ptr<Effect> bullet;
    Effekseer::Handle efcHandle = -1;

};

 // �v���C���[�ɗU������e
class HomingBullet :public BaseProjectile
{
public:
    
    HomingBullet(ID3D11Device* d, Parent p, DirectX::XMFLOAT3 pos, int FirstVec, float ratio = 4.0f);

    ~HomingBullet();

    void Initialize() override;
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;



private:
    //�֐�
    void Move(float elapsedTime);

    //�ϐ�
    float speed = 40.0f;
    float HomingRatio = {};

    //effect
    std::unique_ptr<Effect> bullet;
    Effekseer::Handle efcHandle = -1;

};


//****************************************************************
// 
//  �r�[��
// 
//****************************************************************
class Beam final :public BaseProjectile
{
public:
    Beam(ID3D11Device* d, Parent p, DirectX::XMFLOAT3 pos);
    ~Beam();

    void Initialize() override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;


private:
    //--------------------<�֐�> --------------------//
    void Move(float elapsedTime);

    float BeamSpeed{120.0f}; // �r�[���̑���
    std::unique_ptr<Effect> beam;
    Effekseer::Handle efcHandle = -1;
};

//****************************************************************
// 
//  �����r�[��
// 
//****************************************************************
class MoveBeam final :public BaseProjectile
{
public:
    MoveBeam(ID3D11Device* d, Parent p, DirectX::XMFLOAT3 pos,DirectX::XMFLOAT3 Speed);
    ~MoveBeam();

    void Initialize() override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;


private:
    //--------------------<�֐�> --------------------//
    void Move(float elapsedTime);
    DirectX::XMFLOAT3 Speed{};
    float BeamSpeed{ 120.0f }; // �r�[���̑���
    std::unique_ptr<Effect> beam;
    Effekseer::Handle efcHandle = -1;
};


class CurveBullet :public BaseProjectile
{
public:
    CurveBullet( Parent p, DirectX::XMFLOAT3 pos, int FirstVec, int view);
    ~CurveBullet() override;
    void Initialize() override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;

private:
    //�֐�
    void Move(float elapsedTime);

    //�ϐ�
    float speed = 20.0f;
    float WidthSpeed{ 40.0f };

    int View{}; // ���_�i�O�|��Side�A�P�|��Top�j

    //effect
    std::unique_ptr<Effect> bullet;
    Effekseer::Handle efcHandle = -1;
};


class StayToTargetBullet:public BaseProjectile
{
public:
    StayToTargetBullet(Parent p, DirectX::XMFLOAT3 pos, int view, DirectX::XMFLOAT3 MiddleTargetPos_, float WaitSecond = 1.0f);
    ~StayToTargetBullet() override;
    void Initialize() override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;

    //�֐�
    void Move(float elapsedTime);

    //�ϐ�
    float speed = 30.0f;

    int View{}; // ���_�i�O�|��Side�A�P�|��Top�j

    //effect
    std::unique_ptr<Effect> bullet;
    Effekseer::Handle efcHandle = -1;

    float StackWaitSeconds{};
    DirectX::XMFLOAT3 MiddleTargetPos{};
    DirectX::XMFLOAT3 TargetPos{};
    DirectX::XMFLOAT3 ParentPos{};
    float Ratio{};
    int State{};

};

class BoomerangBullet:public BaseProjectile
{
public:
    BoomerangBullet(Parent p, DirectX::XMFLOAT3 pos,  DirectX::XMFLOAT3 FirstSpeed);
    ~BoomerangBullet() override;
    void Initialize() override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;

    //�֐�
    void Move(float elapsedTime);

    //�ϐ�
    DirectX::XMFLOAT3 speed {};
    float Deccel{};

    int View{}; // ���_�i�O�|��Side�A�P�|��Top�j

    //effect
    std::unique_ptr<Effect> bullet;
    Effekseer::Handle efcHandle = -1;

    float StackWaitSeconds{};
    float Ratio{};
    int State{};

};