#pragma once

#include<DirectXMath.h>

//�q�b�g����
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 normal = { 0,0,0 };
	float distance = -1.0f;
	int materialIndex = -1;
	DirectX::XMFLOAT3 rotation = { 0,0,0 };//��]��
};

//�R���W����
class Collision
{
public:
	//���Ƌ��̌�������
	static bool SphereVsSphere(
		const DirectX::XMFLOAT3& posA,
		float radiusA,
		const DirectX::XMFLOAT3& posB,
		float radiusB );

	//���ʏ�̋�`���m�̓����蔻��
	static bool RectVsRect(const DirectX::XMFLOAT2& posA, const DirectX::XMFLOAT2& sizeA, const DirectX::XMFLOAT2& posB, const DirectX::XMFLOAT2& sizeB);

	//�����̂ƒ�����
	static bool CubeVsCube(const DirectX::XMFLOAT3& posA, const DirectX::XMFLOAT3& sizeA, const DirectX::XMFLOAT3& posB, const DirectX::XMFLOAT3& sizeB);

	//2�_�Ԃ̋�����Ԃ�
	static float Distance(const DirectX::XMFLOAT3& posA, const DirectX::XMFLOAT3& posB);
};

