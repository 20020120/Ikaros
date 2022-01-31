
#include "MovePhase_Start_Side.h"



Start_FromBack_Side::Start_FromBack_Side(XMFLOAT3& scale_, XMFLOAT3 target_scale_) : scale(scale_), target_scale(target_scale_)
{
    constexpr float seconds = 3.5f;

    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    // ���x�����l
    speed = 50.0f;
}


void Start_FromBack_Side::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);


    const float time = timer.NowTime();

    // if���̏I���ő���return���Ă���
    if (time > 0.0f)
    {
        // �^�C�}�[���X�V���Ă�������s���鏈���Q

        // ���x�̕ύX
        SpeedRatio(elapsedTime,time);

        // �O���ֈړ�
        MoveFront(elapsedTime, position, angle);

        // Y��180�x��]
        RotationY180(elapsedTime, time, angle);

        // X = (0.0f)�܂ŕ��
        LerpPositionX(elapsedTime, time, position);

        // Z���ŉ�]
        RotateZ(elapsedTime, time, angle);

        // �X�P�[���l�̍X�V
        ScaleUp(elapsedTime, time);

        return;
    }

    else
    {
        // �^�C�}�[��0.0f�ɂȂ�����
        constexpr float pi = DirectX::XMConvertToRadians(180.0f);
        angle.y = -pi;
        angle.z = 0.0f;
        position.x = 0.0f;
        scale = target_scale;
        return;
    }
}


void Start_FromBack_Side::MoveFront(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // �O����(���f����z������)�ɐi�ޏ���
    DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z).r[2];
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));

    // �ʒu���X�V
    position.x += forward.x * speed * elapsedTime;
    position.y += forward.y * speed * elapsedTime;
    position.z += forward.z * speed * elapsedTime;
}


void Start_FromBack_Side::SpeedRatio(float elapsedTime, const float& now_time)
{

    if(now_time >= 1.0f)
    {
        speed = Calcu3D::LerpFloat(speed, 40.0f, 1.0f * elapsedTime);
    }

    else if(now_time >= 0.1f)
    {
        speed = Calcu3D::LerpFloat(speed, 0.0f, 5.0f * elapsedTime);
    }
}


void Start_FromBack_Side::RotationY180(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle)
{
    if (now_time >= 1.0f) return;

    // ���݃^�C�}�[��2.0�b���߂�����
    constexpr float pi = DirectX::XMConvertToRadians(180.0f);
    angle.y = Calcu3D::LerpFloat(angle.y, pi, 5.0f * elapsedTime);

}


void Start_FromBack_Side::LerpPositionX(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position)
{
    if (now_time >= 0.8f)return;

    // ���݃^�C�}�[��0.8�b���߂�����
    position.x = Calcu3D::LerpFloat(position.x, 0.0f, 5.0f * elapsedTime);
}


void Start_FromBack_Side::RotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle)
{
    if(now_time >= 1.0f)
    {
        constexpr float rotate_speed = DirectX::XMConvertToRadians(270.0f);
        angle.z += rotate_speed * elapsedTime;

        if (angle.z > 3.14f) angle.z = -3.14f;
    }
    else 
    {
        angle.z = Calcu3D::LerpFloat(angle.z, 0.0f, 5.0f * elapsedTime);
    }
}


void Start_FromBack_Side::ScaleUp(float elapsedTime, const float& now_time)
{
    if (now_time >= 1.0f) return;

    scale = Calcu3D::LerpFloat3(scale, target_scale, 5.0f * elapsedTime);
}





Start_FromFront_Side::Start_FromFront_Side(XMFLOAT3& scale_, XMFLOAT3 target_scale_) : scale(scale_), target_scale(target_scale_)
{
    constexpr float seconds = 3.5f;

    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    // ���x�����l
    speed = 50.0f;
}


void Start_FromFront_Side::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);


    const float time = timer.NowTime();

    // if���̏I���ő���return���Ă���
    if (time > 0.0f)
    {
        // �^�C�}�[���X�V���Ă�������s���鏈���Q

        // ���x�̕ύX
        SpeedRatio(elapsedTime, time);

        // �O���ֈړ�
        MoveFront(elapsedTime, position, angle);

        // Y��180�x��]
        RotationY180(elapsedTime, time, angle);

        // X = (0.0f)�܂ŕ��
        LerpPositionX(elapsedTime, time, position);

        // Z���ŉ�]
        RotateZ(elapsedTime, time, angle);

        // �X�P�[���l�̍X�V
        ScaleUp(elapsedTime, time);

        return;
    }

    else
    {
        // �^�C�}�[��0.0f�ɂȂ�����
        constexpr float pi = DirectX::XMConvertToRadians(180.0f);
        angle.y = 0.0f;
        angle.z = 0.0f;
        position.x = 0.0f;
        scale = target_scale;
        return;
    }
}


void Start_FromFront_Side::MoveFront(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // �O����(���f����z������)�ɐi�ޏ���
    DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z).r[2];
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));

    // �ʒu���X�V
    position.x += forward.x * speed * elapsedTime;
    position.y += forward.y * speed * elapsedTime;
    position.z += forward.z * speed * elapsedTime;
}


void Start_FromFront_Side::SpeedRatio(float elapsedTime, const float& now_time)
{

    if (now_time >= 1.0f)
    {
        speed = Calcu3D::LerpFloat(speed, 40.0f, 1.0f * elapsedTime);
    }

    else if (now_time >= 0.1f)
    {
        speed = Calcu3D::LerpFloat(speed, 0.0f, 5.0f * elapsedTime);
    }
}


void Start_FromFront_Side::RotationY180(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle)
{
    if (now_time >= 1.0f) return;

    // ���݃^�C�}�[��2.0�b���߂�����
    //constexpr float pi = DirectX::XMConvertToRadians(-360.0f);
    angle.y = Calcu3D::LerpFloat(angle.y, 0.0f, 5.0f * elapsedTime);

}


void Start_FromFront_Side::LerpPositionX(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position)
{
    if (now_time >= 0.8f)return;

    // ���݃^�C�}�[��0.8�b���߂�����
    position.x = Calcu3D::LerpFloat(position.x, 0.0f, 5.0f * elapsedTime);
}


void Start_FromFront_Side::RotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle)
{
    if (now_time >= 1.0f)
    {
        constexpr float rotate_speed = DirectX::XMConvertToRadians(270.0f);
        angle.z += rotate_speed * elapsedTime;

        if (angle.z > 3.14f) angle.z = -3.14f;
    }
    else
    {
        angle.z = Calcu3D::LerpFloat(angle.z, 0.0f, 5.0f * elapsedTime);
    }
}


void Start_FromFront_Side::ScaleUp(float elapsedTime, const float& now_time)
{
    if (now_time >= 1.0f) return;

    scale = Calcu3D::LerpFloat3(scale, target_scale, 5.0f * elapsedTime);
}










Start_FromYDown_Side::Start_FromYDown_Side(XMFLOAT3 start)
{
    constexpr float seconds = all_time;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    control_point =
    {
        {start.x,start.y,start.z},
        {start.x,start.y + 5.0f * 1.0f,start.z + 17.0f},
        {start.x,start.y + 5.0f * 5.0f,start.z + 17.0f},
        {0.0f,start.y + 5.0f * 9.0f,start.z + 0.0f}
    };

}


void Start_FromYDown_Side::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);

    const float ratio = 1.0f - (timer.NowTime() / all_time);


    position = Calcu3D::BezierCurve(control_point, ratio);


    if (ratio <= 0.99f) return;
    LerpPositionX(elapsedTime, position);

}


void Start_FromYDown_Side::LerpPositionX(float elapsedTime, DirectX::XMFLOAT3& position)
{
    position.x = Calcu3D::LerpFloat(position.x, 0.0f, 5.0f * elapsedTime);
}






Start_FromYUp_Side::Start_FromYUp_Side(XMFLOAT3 start)
{
    constexpr float seconds = all_time;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    control_point =
    {
        {start.x,start.y,start.z},
        {start.x,start.y - 5.0f * 1.0f,start.z + 17.0f},
        {start.x,start.y - 5.0f * 5.0f,start.z + 17.0f},
        {0.0f,start.y - 5.0f * 9.0f,start.z + 0.0f}
    };

}


void Start_FromYUp_Side::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);

    const float ratio = 1.0f - (timer.NowTime() / all_time);


    position = Calcu3D::BezierCurve(control_point, ratio);


    if (ratio <= 0.99f) return;
    LerpPositionX(elapsedTime, position);

}


void Start_FromYUp_Side::LerpPositionX(float elapsedTime, DirectX::XMFLOAT3& position)
{
    position.x = Calcu3D::LerpFloat(position.x, 0.0f, 5.0f * elapsedTime);
}










Start_FromYDown_RotateX_Side::Start_FromYDown_RotateX_Side(XMFLOAT3 start)
{
    constexpr float seconds = all_time;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    control_point =
    {
        {start.x,start.y,start.z},
        {start.x,start.y + 5.0f * 1.0f,start.z + 17.0f},
        {start.x,start.y + 5.0f * 5.0f,start.z + 17.0f},
        {0.0f,start.y + 5.0f * 11.0f,start.z + 0.0f}
    };

}


void Start_FromYDown_RotateX_Side::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);

    const float ratio = 1.0f - (timer.NowTime() / all_time);


    position = Calcu3D::BezierCurve(control_point, ratio);

    LerpRotateX360(ratio, angle);


    if (ratio <= 0.99f) return;
    LerpPositionX(elapsedTime, position);

}


void Start_FromYDown_RotateX_Side::LerpPositionX(float elapsedTime, DirectX::XMFLOAT3& position)
{
    position.x = Calcu3D::LerpFloat(position.x, 0.0f, 5.0f * elapsedTime);
}


void Start_FromYDown_RotateX_Side::LerpRotateX360(const float ratio, DirectX::XMFLOAT3& angle)
{
    constexpr float pi_2 = DirectX::XMConvertToRadians(360.0f);

    angle.x = Calcu3D::LerpFloat(0.0f, -2.0f * pi_2, ratio);


    //// 360�x�𒴂�����
    //if (angle.x < pi_2) return;
    //angle.x = 0.0f;

}









Start_FromYUp_RotateX_Side::Start_FromYUp_RotateX_Side(XMFLOAT3 start)
{
    constexpr float seconds = all_time;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    control_point =
    {
        {start.x,start.y,start.z},
        {start.x,start.y - 5.0f * 1.0f,start.z + 17.0f},
        {start.x,start.y - 5.0f * 5.0f,start.z + 17.0f},
        {0.0f,start.y - 5.0f * 11.0f,start.z + 0.0f}
    };

}


void Start_FromYUp_RotateX_Side::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);

    const float ratio = 1.0f - (timer.NowTime() / all_time);


    position = Calcu3D::BezierCurve(control_point, ratio);

    LerpRotateX360(ratio, angle);


    if (ratio <= 0.99f) return;
    LerpPositionX(elapsedTime, position);

}


void Start_FromYUp_RotateX_Side::LerpPositionX(float elapsedTime, DirectX::XMFLOAT3& position)
{
    position.x = Calcu3D::LerpFloat(position.x, 0.0f, 5.0f * elapsedTime);
}


void Start_FromYUp_RotateX_Side::LerpRotateX360(const float ratio, DirectX::XMFLOAT3& angle)
{
    constexpr float pi_2 = DirectX::XMConvertToRadians(360.0f);

    angle.x = Calcu3D::LerpFloat(0.0f, 2.0f * pi_2, ratio);


    //// 360�x�𒴂�����
    //if (angle.x < pi_2) return;
    //angle.x = 0.0f;
    
}