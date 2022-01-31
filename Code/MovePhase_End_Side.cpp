

#include "MovePhase_End_Side.h"


End_ToUp_Side::End_ToUp_Side()
{
    constexpr float seconds = 0.0f;

    timer.Initialize(nullptr, COUNT::UP, seconds);

    // ���x�����l
    speed = 5.0f;
}

void End_ToUp_Side::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);


    const float time = timer.NowTime();

    // �^�C�}�[���X�V���Ă�������s���鏈���Q

    // ���x�̕ύX
    SpeedRatio(elapsedTime, time);

    // �O���ֈړ�
    MoveFront(elapsedTime, time, position, angle);

    // Z��(0.0f�܂ŕ�Ԃ�)��]
    LerpRotateZ(elapsedTime, time, angle);
    
    // Z���ŉ�]
    RotateZ(elapsedTime, time, angle);

    // X��()
    LerpRotateX(elapsedTime, time, angle);

    // X�̈ʒu��(-10.0f�܂�)��Ԃ���
    LerpPositionX(elapsedTime, time, position);

}


void End_ToUp_Side::MoveFront(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    if (now_time < 1.0f) return;

    // ���݃^�C�}�[��1.0�b�ȓ��Ȃ�

    // �O����(���f����z������)�ɐi�ޏ���
    DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z).r[2];
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));

    // �ʒu���X�V
    position.x += forward.x * speed * elapsedTime;
    position.y += forward.y * speed * elapsedTime;
    position.z += forward.z * speed * elapsedTime;
}


void End_ToUp_Side::SpeedRatio(float elapsedTime, const float& now_time)
{
    if (now_time < 1.0f) return;

    // ���݃^�C�}�[��1.0�b�ȓ��Ȃ�
    speed = Calcu3D::LerpFloat(speed, 90.0f, 5.0f * elapsedTime);
}


void End_ToUp_Side::LerpRotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle)
{

    if (now_time < 1.0f)
    {
        // ���݃^�C�}�[��1.0�b�ȓ��Ȃ�
        constexpr float pi_2 = DirectX::XMConvertToRadians(360.0f);
        angle.z = Calcu3D::LerpFloat(angle.z, -pi_2, 5.0f * elapsedTime);
    }

    else if(now_time >= 1.0f)
    {
        // ���݃^�C�}�[��1.0�b�𒴂��Ă�����
        angle.z = 0.0f;
    }

}


void End_ToUp_Side::RotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle)
{
    if (now_time < 1.0f) return;

    // ���݃^�C�}�[��1.0�b�ȓ��Ȃ�
    constexpr float rotate_speed = DirectX::XMConvertToRadians(270.0f);
    angle.z += rotate_speed * elapsedTime;

    if (angle.z > 3.14f) angle.z = -3.14f;

}



void End_ToUp_Side::LerpRotateX(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle)
{
    if (now_time < 1.0f) return;

    // ���݃^�C�}�[��1.0�b�ȓ��Ȃ�
    constexpr float half_pi = DirectX::XMConvertToRadians(90.0f);
    angle.x = Calcu3D::LerpFloat(angle.x, -half_pi, 5.0f * elapsedTime);
}


void End_ToUp_Side::LerpPositionX(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position)
{
    if (now_time >= 1.0f) return;

    // ���݃^�C�}�[��1.0�b�𒴂�����
    position.x = Calcu3D::LerpFloat(position.x, -10.0f, 5.0f * elapsedTime);


}






End_ToDown_Side::End_ToDown_Side()
{
    constexpr float seconds = 0.0f;

    timer.Initialize(nullptr, COUNT::UP, seconds);

    // ���x�����l
    speed = 5.0f;
}

void End_ToDown_Side::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);


    const float time = timer.NowTime();

    // �^�C�}�[���X�V���Ă�������s���鏈���Q

    // ���x�̕ύX
    SpeedRatio(elapsedTime, time);

    // �O���ֈړ�
    MoveFront(elapsedTime, time, position, angle);

    // Z��(0.0f�܂ŕ�Ԃ�)��]
    LerpRotateZ(elapsedTime, time, angle);

    // Z���ŉ�]
    RotateZ(elapsedTime, time, angle);

    // X��()
    LerpRotateX(elapsedTime, time, angle);

    // X�̈ʒu��(-10.0f�܂�)��Ԃ���
    LerpPositionX(elapsedTime, time, position);

}


void End_ToDown_Side::MoveFront(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    if (now_time < 1.0f) return;

    // ���݃^�C�}�[��1.0�b�ȓ��Ȃ�

    // �O����(���f����z������)�ɐi�ޏ���
    DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z).r[2];
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));

    // �ʒu���X�V
    position.x += forward.x * speed * elapsedTime;
    position.y += forward.y * speed * elapsedTime;
    position.z += forward.z * speed * elapsedTime;
}


void End_ToDown_Side::SpeedRatio(float elapsedTime, const float& now_time)
{
    if (now_time < 1.0f) return;

    // ���݃^�C�}�[��1.0�b�ȓ��Ȃ�
    speed = Calcu3D::LerpFloat(speed, 90.0f, 5.0f * elapsedTime);
}


void End_ToDown_Side::LerpRotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle)
{

    if (now_time < 1.0f)
    {
        // ���݃^�C�}�[��1.0�b�ȓ��Ȃ�
        constexpr float pi_2 = DirectX::XMConvertToRadians(360.0f);
        angle.z = Calcu3D::LerpFloat(angle.z, -pi_2, 5.0f * elapsedTime);
    }

    else if (now_time >= 1.0f)
    {
        // ���݃^�C�}�[��1.0�b�𒴂��Ă�����
        angle.z = 0.0f;
    }

}


void End_ToDown_Side::RotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle)
{
    if (now_time < 1.0f) return;

    // ���݃^�C�}�[��1.0�b�ȓ��Ȃ�
    constexpr float rotate_speed = DirectX::XMConvertToRadians(270.0f);
    angle.z += rotate_speed * elapsedTime;

    if (angle.z > 3.14f) angle.z = -3.14f;

}



void End_ToDown_Side::LerpRotateX(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle)
{
    if (now_time < 1.0f) return;

    // ���݃^�C�}�[��1.0�b�ȓ��Ȃ�
    constexpr float half_pi = DirectX::XMConvertToRadians(90.0f);
    angle.x = Calcu3D::LerpFloat(angle.x, half_pi, 5.0f * elapsedTime);
}


void End_ToDown_Side::LerpPositionX(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position)
{
    if (now_time >= 1.0f) return;

    // ���݃^�C�}�[��1.0�b�𒴂�����
    position.x = Calcu3D::LerpFloat(position.x, -10.0f, 5.0f * elapsedTime);


}
