

#include "MovePhase_Rush.h"
#include "User_Function.h"



void RushFront::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // �O����(���f����z������)�ɐi�ޏ���
    DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z).r[2];
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));

    now_ratio = Calcu3D::LerpFloat(now_ratio, 1.0f, lerp_ratio * elapsedTime);

    // �ʒu���X�V
    position.x += forward.x * speed * now_ratio * elapsedTime;
    position.y += forward.y * speed * now_ratio * elapsedTime;
    position.z += forward.z * speed * now_ratio * elapsedTime;
}



void RushZ::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // z������
    constexpr DirectX::XMFLOAT3 forward = { 0.0f,0.0f,-1.0f };

    now_ratio = Calcu3D::LerpFloat(now_ratio, 1.0f, lerp_ratio * elapsedTime);

    // �ʒu���X�V
    position.x += forward.x * speed * now_ratio * elapsedTime;
    position.y += forward.y * speed * now_ratio * elapsedTime;
    position.z += forward.z * speed * now_ratio * elapsedTime;
}



void RushRound::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);

    Rotate(elapsedTime, angle);


    // �O����(���f����z������)�ɐi�ޏ���
    DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z).r[2];
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));
    
    // �ʒu���X�V
    position.x += forward.x * speed * elapsedTime;
    position.y += forward.y * speed * elapsedTime;
    position.z += forward.z * speed * elapsedTime;

}


void RushRound::Rotate(float elapsedTime, DirectX::XMFLOAT3& angle)
{
    const float time = timer.NowTime();
    const float ratio = 5.0f * elapsedTime;

    constexpr float half_pi = DirectX::XMConvertToRadians(90.0f);


    constexpr DirectX::XMFLOAT4 point = { 4.0f - 0.5f,5.6f - 0.5f,8.3f - 0.5f,9.9f - 0.5f };


    if(ope < 0.0f)
    {
        if ((time >= point.x && time < point.y) && angle.x >= -half_pi * 1.0f)
        {
            angle.x = Calcu3D::LerpFloat(angle.x, -half_pi * 1.0f, ratio);
        }

        else if ((time >= point.y && time < point.z) && angle.x >= -half_pi * 2.0f)
        {
            angle.x = Calcu3D::LerpFloat(angle.x, -half_pi * 2.0f, ratio);
        }

        else if ((time >= point.z && time < point.w) && angle.x >= -half_pi * 3.0f)
        {
            angle.x = Calcu3D::LerpFloat(angle.x, -half_pi * 3.0f, ratio);
        }

        else if (time >= point.w && angle.x >= -half_pi * 4.0f)
        {
            angle.x = Calcu3D::LerpFloat(angle.x, -half_pi * 4.0f, ratio);
        }
    }

    else
    {
        if ((time >= point.x && time < point.y) && angle.x <= half_pi * 1.0f)
        {
            angle.x = Calcu3D::LerpFloat(angle.x, half_pi * 1.0f, ratio);
        }

        else if ((time >= point.y && time < point.z) && angle.x <= half_pi * 2.0f)
        {
            angle.x = Calcu3D::LerpFloat(angle.x, half_pi * 2.0f, ratio);
        }

        else if ((time >= point.z && time < point.w) && angle.x <= half_pi * 3.0f)
        {
            angle.x = Calcu3D::LerpFloat(angle.x, half_pi * 3.0f, ratio);
        }

        else if (time >= point.w && angle.x <= half_pi * 4.0f)
        {
            angle.x = Calcu3D::LerpFloat(angle.x, half_pi * 4.0f, ratio);
        }
    }
    
}




void RushTarget::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // �O����(���f����z������)�ɐi�ޏ���
    DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z).r[2];
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));

    // �ʒu���X�V
    position.x += forward.x * speed * rush_speed_ratio * elapsedTime;
    position.y += forward.y * speed * rush_speed_ratio * elapsedTime;
    position.z += forward.z * speed * rush_speed_ratio * elapsedTime;
}


void RushFront_LimitTimer::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);

    if (timer.NowTime() <= 0.0f) return;


    // �O����(���f����z������)�ɐi�ޏ���
    DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z).r[2];
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));
    

    // �ʒu���X�V
    position.x += forward.x * speed * elapsedTime;
    position.y += forward.y * speed * elapsedTime;
    position.z += forward.z * speed * elapsedTime;
}




RushBezier_Top::RushBezier_Top(DirectX::XMFLOAT3 start)
{
    timer.Initialize(nullptr, COUNT::UP, 0.0f);

    // x�����ɂ��炷��
    constexpr float shift_x = 54.0f;

    // �����ʒu��x�l������������
    if(start.x > 0.0f)
    {

        // �E���獶��        x (+ => -), z (+ => -)
        control_point =
        {
            {start.x +  0.0f,   0.0f,   start.z +  0.0f},
            {start.x - 30.0f,   0.0f,   start.z + 10.0f},
            {start.x - 50.0f,   0.0f,   start.z - 20.0f},
            {start.x - 10.0f,   0.0f,   start.z - 90.0f}
        };

        // �E����
        start_angle_y = DirectX::XMConvertToRadians(-90.0f);
        end_angle_y = DirectX::XMConvertToRadians(-180.0f + -20.0f);
    }

    // x�l������������
    else
    {

        // ������E��        x (- => +), z (+ => -)
        control_point =
        {
            {start.x -  0.0f,   0.0f,   start.z +  0.0f},
            {start.x + 30.0f,   0.0f,   start.z + 10.0f},
            {start.x + 50.0f,   0.0f,   start.z - 20.0f},
            {start.x + 10.0f,   0.0f,   start.z - 90.0f}
        };

        // ������
        start_angle_y = DirectX::XMConvertToRadians(-270.0f);
        end_angle_y = DirectX::XMConvertToRadians(-180.0f + 20.0f);
    }
    
}


void RushBezier_Top::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // �J�E���g�A�b�v�̃^�C�}�[
    timer.Update(elapsedTime);

    // 0.15f�̔{�� => 6.6�b��ratio��1.0f
    const float ratio = timer.NowTime() * 0.15f;


    position = Calcu3D::BezierCurve(control_point, ratio);


    // �����̒���
    const float calc_ratio = 1.5f * ratio;
    angle.y = Calcu3D::LerpFloat(start_angle_y, end_angle_y, calc_ratio);
}



RushBezier_Side::RushBezier_Side(DirectX::XMFLOAT3 start)
{
    timer.Initialize(nullptr, COUNT::UP, 0.0f);

    // �����ʒu��y�l������������
    if(start.y > 0.0f)
    {

        // �ォ�牺��        y (+ => -), z (+ => -)
        control_point =
        {
            {0.0f,   start.y + 0.0f,   start.z + 0.0f},
            {0.0f,   start.y - 5.0f,   start.z - 90.0f},
            {0.0f,   start.y - 40.0f,   start.z - 10.0f},
            {0.0f,   start.y - 60.0f,   start.z - 140.0f}
        };

        // ������
        start_angle_x = DirectX::XMConvertToRadians(0.0f);
        end_angle_x = DirectX::XMConvertToRadians(20.0f);
    }

    // y�l������������
    else
    {

        // ��������        y (- => +), z (+ => -)
        control_point =
        {
            {0.0f,   start.y - 0.0f,   start.z + 0.0f},
            {0.0f,   start.y + 5.0f,   start.z - 90.0f},
            {0.0f,   start.y + 40.0f,   start.z - 10.0f},
            {0.0f,   start.y + 60.0f,   start.z - 140.0f}
        };

        // �����
        start_angle_x = DirectX::XMConvertToRadians(0.0f);
        end_angle_x = DirectX::XMConvertToRadians(-20.0f);
        
    }
    
}


void RushBezier_Side::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // �J�E���g�A�b�v�̃^�C�}�[
    timer.Update(elapsedTime);

    // 0.15f�̔{�� => 6.6�b��ratio��1.0f
    const float ratio = timer.NowTime() * 0.15f;


    position = Calcu3D::BezierCurve(control_point, ratio);


    // �����̒���
    const float calc_ratio = 1.5f * ratio;
    angle.x = Calcu3D::LerpFloat(start_angle_x, end_angle_x, calc_ratio);

}






void RushWaveVertical::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    constexpr float pi = DirectX::XMConvertToRadians(180);

    curveAng += DirectX::XMConvertToRadians(curveSpeed) * elapsedTime;

    if (curveAng < -pi)
    {
        curveAng += 2.0f * pi;
    }
    if (curveAng > pi)
    {
        curveAng -= 2.0f * pi;
    }

    angle.x = cosf(curveAng) * DirectX::XMConvertToRadians(45);



    // �O����(���f����z������)�ɐi�ޏ���
    DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z).r[2];
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));

    // �ʒu���X�V
    position.x += forward.x * speed * rush_speed_ratio * elapsedTime;
    position.y += forward.y * speed * rush_speed_ratio * elapsedTime;
    position.z += forward.z * speed * rush_speed_ratio * elapsedTime;
}



void RushWaveHorizon::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    constexpr float pi = DirectX::XMConvertToRadians(180);

    curveAng += DirectX::XMConvertToRadians(2 * curveSpeed) * elapsedTime;

    if (curveAng < -pi)
    {
        curveAng += 2.0f * pi;
    }
    if (curveAng > pi)
    {
        curveAng -= 2.0f * pi;
    }

    angle.y = cosf(curveAng) * DirectX::XMConvertToRadians(45) + -pi;



    // �O����(���f����z������)�ɐi�ޏ���
    DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z).r[2];
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));

    // �ʒu���X�V
    position.x += forward.x * speed * rush_speed_ratio * elapsedTime;
    position.y += forward.y * speed * rush_speed_ratio * elapsedTime;
    position.z += forward.z * speed * rush_speed_ratio * elapsedTime;
}






void NextPositionRushHorizon::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // �����𔽓]����
    constexpr float pi = DirectX::XMConvertToRadians(180.0f);
    angle.y = angle.y < 0.0f ? 0.0f : -pi;


    // z�̈ʒu����
    if (position.z < -turn_z_axis_size)
    {
        position.z = -turn_z_axis_size;
    }
    if (position.z > turn_z_axis_size)
    {
        position.z = turn_z_axis_size;
    }

    // �J�E���g�𑝉�������
    ++*p_rush_count;

    constexpr float teleport_y_size = 20.0f;
    if (*p_rush_count == 1)
        position.y += teleport_y_size;
    if (*p_rush_count == 2)
        position.y += -2.0f * teleport_y_size;
    if (*p_rush_count == 3) position = origin_position;
}



void NextPositionRushVertical::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // �����𔽓]����
    angle.x *= -1.0f;

    // y�̈ʒu����
    if (position.y < -turn_y_axis_size)
    {
        position.y = -turn_y_axis_size;
    }
    if (position.y > turn_y_axis_size)
    {
        position.y = turn_y_axis_size;
    }

    // �J�E���g�𑝉�������
    *p_rush_count++;


    constexpr float teleport_z_size = 20.0f;
    position.z += -teleport_z_size;
}
