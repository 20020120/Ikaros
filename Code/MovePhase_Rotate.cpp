

#include "MovePhase_Rotate.h"

#include "User_Function.h"


void RotateXToTarget::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // �ڕW�܂ł̐��K�������x�N�g��
    DirectX::XMVECTOR to_target_direction = DirectX::XMVector3Normalize(Calcu3D::MakeVectorAtoB(position, *p_target));

    // ���K���������ʕ����̃x�N�g��
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    DirectX::XMVECTOR up = DirectX::XMVector3Normalize(M.r[1]);

    // ���K�����ꂽ��̃x�N�g���̓��ς��t�R�T�C������ -> �p�x�����܂�
    float dot_ = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, front));
    if (dot_ > 0.0f)dot_ += -FLT_EPSILON;
    const float angle_ = std::acosf(dot_);

    // ���ςō��E���� (�����ڂ͏㉺)
    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, up));
    constexpr float ratio = 1.0f;
    angle.x += dot > 0.0f ? -angle_ : angle_;

    // �J�E���g�𑝉�������
    ++*p_rush_count;
}



void RotateYToTarget::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{

    // �ڕW�܂ł̐��K�������x�N�g��
    DirectX::XMVECTOR to_target_direction = DirectX::XMVector3Normalize(Calcu3D::MakeVectorAtoB(position, *p_target));

    // ���K���������ʕ����̃x�N�g��
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(M.r[0]);

    // ���K�����ꂽ��̃x�N�g���̓��ς��t�R�T�C������ -> �p�x�����܂�
    float dot_ = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, front));
    if (dot_ > 0.0f)dot_ += -FLT_EPSILON;
    const float angle_ = std::acosf(dot_);

    // ���ςō��E����
    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, right));
    angle.y += dot > 0.0f ? -angle_ : angle_;

    // �J�E���g�𑝉�������
    ++* p_rush_count;
}






void RotateXToTargetLerp::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // �ڕW�܂ł̐��K�������x�N�g��
    DirectX::XMVECTOR to_target_direction = Calcu3D::MakeVectorAtoB(position, *p_target, true);

    // ���K���������ʕ����̃x�N�g��
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    DirectX::XMVECTOR up = DirectX::XMVector3Normalize(M.r[1]);

    // ���K�����ꂽ��̃x�N�g���̓��ς��t�R�T�C������ -> �p�x�����܂�
    float dot_ = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, front));
    if (dot_ > 0.0f)dot_ += -FLT_EPSILON;
    angle_ = std::acosf(dot_);

    constexpr float epsilon_radian = DirectX::XMConvertToRadians(0.1f);
    if (std::fabsf(angle_) <= epsilon_radian) return;

    // ���ςō��E���� (�����ڂ͏㉺)
    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, up));
    angle.x += dot > 0.0f ? Calcu3D::LerpFloat(0.0f, -angle_, ratio * elapsedTime) : Calcu3D::LerpFloat(0.0f, angle_, ratio * elapsedTime);
}



void RotateYToTargetLerp::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // �ڕW�܂ł̐��K�������x�N�g��
    DirectX::XMVECTOR to_target_direction = Calcu3D::MakeVectorAtoB(position, *p_target, true);

    // ���K���������ʕ����̃x�N�g��
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(M.r[0]);

    // ���K�����ꂽ��̃x�N�g���̓��ς��t�R�T�C������ -> �p�x�����܂�
    float dot_ = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, front));
    if (dot_ > 0.0f)dot_ += -FLT_EPSILON;
    angle_ = std::acosf(dot_);

    constexpr float epsilon_radian = DirectX::XMConvertToRadians(0.1f);
    if (std::fabsf(angle_) <= epsilon_radian) return;

    // ���ςō��E����
    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, right));
    angle.y += dot < 0.0f ? Calcu3D::LerpFloat(0.0f, -angle_, ratio * elapsedTime) : Calcu3D::LerpFloat(0.0f, angle_, ratio * elapsedTime);

}





void RotateXToTargetLerp_LimitTimer::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);

    //     ���݂̕⊮�� = �i�񂾎��� / ��������
    const float ratio = 1.0f - (timer.NowTime() / limit_time);


    // �ڕW�܂ł̐��K�������x�N�g��
    DirectX::XMVECTOR to_target_direction = Calcu3D::MakeVectorAtoB(position, *p_target, true);

    // ���K���������ʕ����̃x�N�g��
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    DirectX::XMVECTOR up = DirectX::XMVector3Normalize(M.r[1]);

    // ���K�����ꂽ��̃x�N�g���̓��ς��t�R�T�C������ -> �p�x�����܂�
    float dot_ = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, front));
    if (dot_ > 0.0f)dot_ += -FLT_EPSILON;
    angle_ = std::acosf(dot_);

    constexpr float epsilon_radian = DirectX::XMConvertToRadians(0.1f);
    if (std::fabsf(angle_) <= epsilon_radian) return;

    // ���ςō��E���� (�����ڂ͏㉺)
    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, up));
    angle.x += dot > 0.0f ? Calcu3D::LerpFloat(0.0f, -angle_, ratio/*   * elapsedTime  */) : Calcu3D::LerpFloat(0.0f, angle_, ratio/*   * elapsedTime  */);
}



void RotateYToTargetLerp_LimitTimer::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);

    //     ���݂̕⊮�� = �i�񂾎��� / ��������
    const float ratio = 1.0f - (timer.NowTime() / limit_time);


    // �ڕW�܂ł̐��K�������x�N�g��
    DirectX::XMVECTOR to_target_direction = Calcu3D::MakeVectorAtoB(position, *p_target, true);

    // ���K���������ʕ����̃x�N�g��
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(M.r[0]);

    // ���K�����ꂽ��̃x�N�g���̓��ς��t�R�T�C������ -> �p�x�����܂�
    float dot_ = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, front));
    if (dot_ > 0.0f)dot_ += -FLT_EPSILON;
    angle_ = std::acosf(dot_);

    constexpr float epsilon_radian = DirectX::XMConvertToRadians(0.1f);
    if (std::fabsf(angle_) <= epsilon_radian) return;

    // ���ςō��E����
    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, right));
    angle.y += dot < 0.0f ? Calcu3D::LerpFloat(0.0f, -angle_, ratio/*   * elapsedTime  */) : Calcu3D::LerpFloat(0.0f, angle_, ratio/*   * elapsedTime  */);

}






void RotateX::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);


    // �o�ߎ��Ԃɍ��킹���⊮�����Z�o����
    //     ���݂̕⊮�� = �o�ߎ��� / ��������
    const float ratio = 1.0f - (timer.NowTime() / all_time);


    constexpr float angle_speed_max = DirectX::XMConvertToRadians(810.0f);


    // �c�莞�Ԃ��ݒ莞�Ԃ̔����ȏ�̂Ƃ�
    if (timer.NowTime() >= half_time)
    {
        // ���X�ɑ��x���グ��
        angle_speed = Calcu3D::LerpFloat(0.0f, angle_speed_max, ratio);

        angle.x += angle_speed * elapsedTime;


        constexpr float pi = DirectX::XMConvertToRadians(180.0f);
        if (angle.x < 2.0f * pi) return;

        angle.x += -2.0f * pi;
    }



    // �c�莞�Ԃ��ݒ莞�Ԃ̔�������������Ƃ�
    else
    {
        // ���X�ɑ��x�𗎂Ƃ�
        angle_speed = Calcu3D::LerpFloat(angle_speed_max, 0.0f, ratio);


        // �p���x��angle_speed_max �́@0.1�{���傫��������
        if (angle_speed > angle_speed_max * 0.1f)
        {
            angle.x += angle_speed * elapsedTime;

            constexpr float pi = DirectX::XMConvertToRadians(180.0f);
            if (angle.x < 2.0f * pi) return;

            angle.x += -2.0f * pi;
        }

        // �p���x��0�ɂȂ�����
        else
        {

            constexpr float pi = DirectX::XMConvertToRadians(180.0f);

            if (angle.x < pi)
            {
                // x���p�x��0.0f�ɂȂ�悤�ɕ⊮����
                angle.x = Calcu3D::LerpFloat(angle.x, 0.0f, 3.0f * elapsedTime);
            }

            else
            {
                // x���p�x��6.28f�ɂȂ�悤�ɕ⊮����
                angle.x = Calcu3D::LerpFloat(angle.x, 2.0f * pi, 3.0f * elapsedTime);
            }
        }
    }



}



void RotateY::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);


    // �o�ߎ��Ԃɍ��킹���⊮�����Z�o����
    //     ���݂̕⊮�� = �o�ߎ��� / ��������
    const float ratio = 1.0f - (timer.NowTime() / all_time);


    constexpr float angle_speed_max = DirectX::XMConvertToRadians(810.0f);


    // �c�莞�Ԃ��ݒ莞�Ԃ̔����ȏ�̂Ƃ�
    if (timer.NowTime() >= half_time)
    {
        // ���X�ɑ��x���グ��
        angle_speed = Calcu3D::LerpFloat(0.0f, angle_speed_max, ratio);

        angle.y += angle_speed * elapsedTime;


        constexpr float pi = DirectX::XMConvertToRadians(180.0f);
        if (angle.y < pi) return;

        angle.y += -2.0f * pi;
    }



    // �c�莞�Ԃ��ݒ莞�Ԃ̔�������������Ƃ�
    else
    {
        // ���X�ɑ��x�𗎂Ƃ�
        angle_speed = Calcu3D::LerpFloat(angle_speed_max, 0.0f, ratio);


        // �p���x��angle_speed_max �́@0.1�{���傫��������
        if (angle_speed > angle_speed_max * 0.1f)
        {
            angle.y += angle_speed * elapsedTime;

            constexpr float pi = DirectX::XMConvertToRadians(180.0f);
            if (angle.y < pi) return;

            angle.y += -2.0f * pi;
        }

        // �p���x��0�ɂȂ�����
        else
        {
            constexpr float pi = DirectX::XMConvertToRadians(180.0f);

            if(angle.y < 0.0f)
            {
                // y���p�x��-3.14f�ɂȂ�悤�ɕ⊮����
                angle.y = Calcu3D::LerpFloat(angle.y, -pi, 3.0f * elapsedTime);
            }

            else
            {
                // y���p�x��3.14f�ɂȂ�悤�ɕ⊮����
                angle.y = Calcu3D::LerpFloat(angle.y, pi, 3.0f * elapsedTime);
            }
            
        }
    }



}



void RotateZ::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);


    // �o�ߎ��Ԃɍ��킹���⊮�����Z�o����
    //     ���݂̕⊮�� = �o�ߎ��� / ��������
    const float ratio = 1.0f - (timer.NowTime() / all_time);


    constexpr float angle_speed_max = DirectX::XMConvertToRadians(810.0f);


    // �c�莞�Ԃ��ݒ莞�Ԃ̔����ȏ�̂Ƃ�
    if (timer.NowTime() >= half_time)
    {
        // ���X�ɑ��x���グ��
        angle_speed = Calcu3D::LerpFloat(0.0f, angle_speed_max, ratio);

        angle.z += angle_speed * elapsedTime;


        constexpr float pi = DirectX::XMConvertToRadians(180.0f);
        if (angle.z < pi) return;

        angle.z += -2.0f * pi;
    }



    // �c�莞�Ԃ��ݒ莞�Ԃ̔�������������Ƃ�
    else
    {
        // ���X�ɑ��x�𗎂Ƃ�
        angle_speed = Calcu3D::LerpFloat(angle_speed_max, 0.0f, ratio);


        // �p���x��angle_speed_max �́@0.1�{���傫��������
        if (angle_speed > angle_speed_max * 0.1f)
        {
            angle.z += angle_speed * elapsedTime;

            constexpr float pi = DirectX::XMConvertToRadians(180.0f);
            if (angle.z < pi) return;

            angle.z += -2.0f * pi;
        }

        // �p���x��0�ɂȂ�����
        else
        {

            constexpr float pi = DirectX::XMConvertToRadians(180.0f);

            if (angle.z < 0.0f)
            {
                // y���p�x��-3.14f�ɂȂ�悤�ɕ⊮����
                angle.z = Calcu3D::LerpFloat(angle.z, -pi, 3.0f * elapsedTime);
            }

            else
            {
                // y���p�x��3.14f�ɂȂ�悤�ɕ⊮����
                angle.z = Calcu3D::LerpFloat(angle.z, pi, 3.0f * elapsedTime);
            }
        }
    }



}










void RotateToUpLerp::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // ���K���������ʕ����̃x�N�g��
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    // ���K������y��"��"�����̃x�N�g��
    DirectX::XMVECTOR y_up = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

    // ���K�����ꂽ��̃x�N�g���̓��ς��t�R�T�C������ -> �p�x�����܂�
    const float angle_ = std::acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(y_up, front)));

    constexpr float ratio = 1.0f;
    angle.x += -Calcu3D::LerpFloat(0.0f, angle_, ratio * elapsedTime);
}



void RotateToDownLerp::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // ���K���������ʕ����̃x�N�g��
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    // ���K������y��"��"�����̃x�N�g��
    DirectX::XMVECTOR y_down = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));

    // ���K�����ꂽ��̃x�N�g���̓��ς��t�R�T�C������ -> �p�x�����܂�
    const float angle_ = std::acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(y_down, front)));

    constexpr float ratio = 1.0f;
    angle.x += Calcu3D::LerpFloat(0.0f, angle_, ratio * elapsedTime);
}



void RotateInverseY::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    angle.y += Calcu3D::LerpFloat(0.0f, half_pi, 0.5f * elapsedTime);

}
