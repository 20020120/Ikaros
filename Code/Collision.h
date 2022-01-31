#pragma once

#include<DirectXMath.h>

//ヒット結果
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 normal = { 0,0,0 };
	float distance = -1.0f;
	int materialIndex = -1;
	DirectX::XMFLOAT3 rotation = { 0,0,0 };//回転量
};

//コリジョン
class Collision
{
public:
	//球と球の交差判定
	static bool SphereVsSphere(
		const DirectX::XMFLOAT3& posA,
		float radiusA,
		const DirectX::XMFLOAT3& posB,
		float radiusB );

	//平面上の矩形同士の当たり判定
	static bool RectVsRect(const DirectX::XMFLOAT2& posA, const DirectX::XMFLOAT2& sizeA, const DirectX::XMFLOAT2& posB, const DirectX::XMFLOAT2& sizeB);

	//直方体と直方体
	static bool CubeVsCube(const DirectX::XMFLOAT3& posA, const DirectX::XMFLOAT3& sizeA, const DirectX::XMFLOAT3& posB, const DirectX::XMFLOAT3& sizeB);

	//2点間の距離を返す
	static float Distance(const DirectX::XMFLOAT3& posA, const DirectX::XMFLOAT3& posB);
};

