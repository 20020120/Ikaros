
#include "ShipMoveFunctions.h"
#include "User_Function.h"
#include "MathSupport.h"


inline DirectX::XMVECTOR& ToFront(DirectX::XMFLOAT3& angle)
{
    return DirectX::XMVector3Normalize(DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z).r[2]);
}

inline DirectX::XMVECTOR& ToRight(DirectX::XMFLOAT3& angle)
{
    return DirectX::XMVector3Normalize(DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z).r[0]);
}





// 現在位置から船首"右側"までの移動
void ToHead_RightSide::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle, const float speed_ratio)
{

}

// 現在位置から船首"左側"までの移動
void ToHead_LeftSide::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle, const float speed_ratio)
{
    const DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    DirectX::XMVECTOR left_vec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(transform.r[0])/* X方向 */, SMC::to_head_length);
    DirectX::XMVECTOR head_vec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(transform.r[2])/* Z方向 */, SMC::to_right_length);
    

    // 最終目標地点
    DirectX::XMVECTOR End_Pos = DirectX::XMVectorSubtract(DirectX::XMVectorZero(), DirectX::XMVectorAdd(head_vec, left_vec));

    // 目標地点までのベクトル
    DirectX::XMVECTOR To_End_Vec = DirectX::XMVectorSubtract(End_Pos, DirectX::XMLoadFloat3(&position));

    // 位置の更新
    DirectX::XMVECTOR Vec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(To_End_Vec), speed_ratio * elapsedTime);
    DirectX::XMFLOAT3 displacement;
    DirectX::XMStoreFloat3(&displacement, Vec);
    position.x += displacement.x;
    position.y += displacement.y;
    position.z += displacement.z;

    // 現在位置と最終地点の距離
    float different_length = DirectX::XMVectorGetX(DirectX::XMVector3Length(To_End_Vec));
    // 現在位置と最終地点がほぼ同じなら位置を最終地点にする
    if (different_length > 0.1f) return;

    DirectX::XMStoreFloat3(&position, End_Pos);
}




// 現在位置から船尾"右側"までの移動
void ToBack_RightSide::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle, const float speed_ratio)
    {
        
    }

// 現在位置から船尾"左側"までの移動
void ToBack_LeftSide::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle, const float speed_ratio)
{
    const DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    
    DirectX::XMVECTOR left_vec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(transform.r[0])/* X方向 */, SMC::to_back_length);
    DirectX::XMVECTOR back_vec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(transform.r[2])/* Z方向 */, SMC::to_right_length);

    // 最終目標地点
    DirectX::XMVECTOR End_Pos = DirectX::XMVectorSubtract(DirectX::XMVectorZero(), DirectX::XMVectorAdd(back_vec, left_vec));

    // 目標地点までのベクトル
    DirectX::XMVECTOR To_End_Vec = DirectX::XMVectorSubtract(End_Pos, DirectX::XMLoadFloat3(&position));

    // 位置の更新
    DirectX::XMVECTOR Vec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(To_End_Vec), speed_ratio * elapsedTime);
    DirectX::XMFLOAT3 displacement;
    DirectX::XMStoreFloat3(&displacement, Vec);
    position.x += displacement.x;
    position.y += displacement.y;
    position.z += displacement.z;

    // 現在位置と最終地点の距離
    float different_length = DirectX::XMVectorGetX(DirectX::XMVector3Length(To_End_Vec));
    // 現在位置と最終地点がほぼ同じなら位置を最終地点にする
    if (different_length > FLT_MIN) return;

    DirectX::XMStoreFloat3(&position, End_Pos);
}


void ToRightSide::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle, const float speed_ratio)
{
    constexpr float angle_speed = DirectX::XMConvertToRadians(3.0f);
    angle.y += angle_speed * elapsedTime;

    constexpr float pi_and_halfpi = DirectX::XMConvertToRadians(270.0f);
    if(angle.y >= pi_and_halfpi)
    {
        angle.y = pi_and_halfpi;
    }
}


void Rotating::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle, const float speed_ratio)
{
    constexpr float angle_speed = DirectX::XMConvertToRadians(3.0f);
    angle.y += angle_speed * elapsedTime;

    constexpr float two_pi = DirectX::XMConvertToRadians(360);
    if (angle.y >= two_pi)
    {
        angle.y = 0.0f;
    }
}
