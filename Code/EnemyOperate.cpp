
#include "Enemy.h"
#include "SceneManager.h"
#include "ProjectileManager.h"

EnemyDummy::EnemyDummy(ID3D11Device* Device, const char* model_filepath) : Enemy(
	Device,
	model_filepath,
	EnemyPSFilePath::Lambert
)
{
	type = Type::NONE;
	status.speed = 0.0f;
}

EnemyDummy::EnemyDummy(ID3D11Device* Device, const char* model_filepath, const char* PsPath)
	: Enemy(
		Device,
		model_filepath,
		PsPath)
{
	type = Type::NONE;
	status.speed = 0.0f;
}

//���i
EnemyMoveStraight::EnemyMoveStraight(ID3D11Device* d,const Type& type_) : Enemy(
	d,
	type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
	EnemyPSFilePath::Lambert)
{
	type = type_;
	status.speed = 8.0f;

}

void EnemyMoveStraight::Initialize()
{
	t.Position = { 0.0f,0.0f,60.0f };
}



void EnemyMoveStraight::Behavior(float elapsedTime)
{
	Move(elapsedTime);

	invTimer -= elapsedTime;

	// �N�[���_�E��
	if (CanShot(elapsedTime) == false) return;

	//�e����
	BulletShot(elapsedTime);
}

void EnemyMoveStraight::Move(float elapsedTime)
{
	// �O����(���f����z������)�ɐi�ޏ���
	DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z) };
	DirectX::XMFLOAT4X4 rotate;
	DirectX::XMStoreFloat4x4(&rotate, R);
	DirectX::XMFLOAT3 forward = { rotate._31,rotate._32,rotate._33 };
	DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forward));
	DirectX::XMStoreFloat3(&forward, Forward);

	// y��z�̂�
	t.Position.y += forward.y * status.speed * elapsedTime;
	t.Position.z += forward.z * status.speed * elapsedTime;
}

//�E�F�[�u
EnemyMoveWave::EnemyMoveWave(ID3D11Device* d, const Type& type_) : Enemy(
	d,
	type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
	EnemyPSFilePath::Lambert)
{
	type = type_;
	status.speed = 10.0f;

}


void EnemyMoveWave::Initialize()
{
	t.Position = { 0.0f,0.0f,60.0f };
}



void EnemyMoveWave::Behavior(float elapsedTime)
{
	curveAng += DirectX::XMConvertToRadians(curveSpeed) * elapsedTime;

	if(curveAng < DirectX::XMConvertToRadians(-180))
	{
		curveAng += DirectX::XMConvertToRadians(360);
	}
	if (curveAng > DirectX::XMConvertToRadians(180))
	{
		curveAng -= DirectX::XMConvertToRadians(360);
	}

	r.Angle.x = cosf(curveAng) * DirectX::XMConvertToRadians(45);

	Move(elapsedTime);

	invTimer -= elapsedTime;

	// �N�[���_�E��
	if (CanShot(elapsedTime) == false) return;

	//�e����
	BulletShot(elapsedTime);
}

void EnemyMoveWave::Move(float elapsedTime)
{
	// �O����(���f����z������)�ɐi�ޏ���
	DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z) };
	DirectX::XMFLOAT4X4 rotate;
	DirectX::XMStoreFloat4x4(&rotate, R);
	DirectX::XMFLOAT3 forward = { rotate._31,rotate._32,rotate._33 };
	DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forward));
	DirectX::XMStoreFloat3(&forward, Forward);

	// y��z�̂�
	t.Position.y += forward.y * status.speed * elapsedTime;
	t.Position.z += forward.z * status.speed * elapsedTime;
}
