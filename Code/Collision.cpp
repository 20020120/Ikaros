#include "Collision.h"

#include <cmath>

//球と球の交差判定
bool Collision::SphereVsSphere(
	const DirectX::XMFLOAT3& posA, float radiusA,
	const DirectX::XMFLOAT3& posB, float radiusB )
{
	//a->bの単位ベクトルを算出
	DirectX::XMVECTOR PosA = DirectX::XMLoadFloat3(&posA);
	DirectX::XMVECTOR PosB = DirectX::XMLoadFloat3(&posB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PosB, PosA);
	//using namespace DirectX;
	//DirectX::XMVECTOR Vec = PosB - PosA;
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	//距離判定
	float range = radiusA + radiusB;
	if (range * range < lengthSq)
	{
		return false;
	}
	
	return true;
}

bool Collision::RectVsRect(const DirectX::XMFLOAT2& posA, const DirectX::XMFLOAT2& sizeA, const DirectX::XMFLOAT2& posB, const DirectX::XMFLOAT2& sizeB)
{
	const float leftA = posA.x - sizeA.x;
	const float rightA = posA.x + sizeA.x;
	const float topA = posA.y - sizeA.y;
	const float bottomA = posA.y + sizeA.y;

	const float leftB = posB.x - sizeB.x;
	const float rightB = posB.x + sizeB.x;
	const float topB = posB.y - sizeB.y;
	const float bottomB = posB.y + sizeB.y;


	if ((rightA > leftB) && (leftA < rightB) && (topA < bottomB) && (bottomA > topB))
	{
		return true;
	}
	else
		return false;
}

bool Collision::CubeVsCube(const DirectX::XMFLOAT3& posA, const DirectX::XMFLOAT3& sizeA, const DirectX::XMFLOAT3& posB, const DirectX::XMFLOAT3& sizeB)
{
	bool resultX = false, resultY = false, resultZ = false;
	const float cubesSizeX = sizeA.x / 2.0f + sizeB.x / 2.0f;
	const float cubesSizeY = sizeA.y / 2.0f + sizeB.y / 2.0f;
	const float cubesSizeZ = sizeA.z / 2.0f + sizeB.z / 2.0f;

	//x
	const float distanceX = posA.x - posB.x;
	if (std::fabs(distanceX) < cubesSizeX)
	{
		resultX = true;
	}
	//y
	const float distanceY = posA.y - posB.y;
	if (fabs(distanceY) < cubesSizeY)
	{
		resultY = true;
	}
	//z
	const float distanceZ = posA.z - posB.z;
	if (fabs(distanceZ) < cubesSizeZ)
	{
		resultZ = true;
	}
	if (resultX && resultY && resultZ)
	{
		return true;
	}
	return false;
}


//2点間の距離を返す
float Collision::Distance(const DirectX::XMFLOAT3& posA, const DirectX::XMFLOAT3& posB)
{
	return ((posA.x - posB.x) * (posA.x - posB.x) + (posA.y - posB.y) * (posA.y - posB.y) + (posA.z - posB.z) * (posA.z - posB.z));
}