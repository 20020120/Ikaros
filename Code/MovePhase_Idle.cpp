
#include "MovePhase_Idle.h"



void Idle::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    if (Is(elapsedTime) == false) return;

    if (p_random_min_1 == nullptr) return;

    *p_random_min_1 = rand() % range + 1;
}

bool Idle::Is(const float elapsedTime)
{
    if (inited == false) { timer.Initialize(nullptr, COUNT::DOWN, time); inited = true; }

    // �ҋ@��Ԃ̍X�V
    timer.Update(elapsedTime);

    
    if (timer.NowTime() > 0.0f) return false;

    //�@���Ԃ��O�ɂȂ��true��Ԃ�
    timer.Reset();
    return true;
}



void ShakeBody::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    
}




void ReturnOriginPosition::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    position.x = Calcu3D::LerpFloat(position.x, origin_position.x, lerp_ratio * elapsedTime);
    position.y = Calcu3D::LerpFloat(position.y, origin_position.y, lerp_ratio * elapsedTime);
    position.z = Calcu3D::LerpFloat(position.z, origin_position.z, lerp_ratio * elapsedTime);
}



void ReturnTeleport::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    position = origin_position;
}
