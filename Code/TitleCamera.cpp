

#include "TitleCamera.h"

#include "User_Function.h"


void TitleCamera::Initialize(ID3D11Device* device)
{

    camera = std::make_unique<Camera>(device);
    constexpr DirectX::XMFLOAT3 focus = { 0.0f,0.0f,0.0f };
    constexpr DirectX::XMFLOAT3 up = { 0.0f,1.0f,0.0f };
    camera->SetLookAtLH(left_front_pos, focus, up);

    timer.Initialize(nullptr, COUNT::UP, 0.0f);

    next_camera_position = left_front_pos;


    phase_index = 0;
}


void TitleCamera::Update(float elapsedTime)
{
    if (CameraPosLerp(elapsedTime) == false)
    {
        timer.Update(elapsedTime);

        (this->*phase[phase_index])(elapsedTime);
    }

    camera->DemoCameraUpdate(elapsedTime);

    camera->f_Gui();
}


void TitleCamera::ConstantBufferUpdate(ID3D11DeviceContext* dc)
{
    camera->TitleConstantBufferUpdate(dc);
}

bool TitleCamera::CameraPosLerp(float elapsedTime)
{
    // カメラ位置の更新があれば更新する
    // ※ 更新予定位置が定数と同じ場合のみ更新させる
    if (next_camera_position.x != back_pos.x && next_camera_position.x != left_front_pos.x) return false;
    if (next_camera_position.y != back_pos.y && next_camera_position.y != left_front_pos.y) return false;
    if (next_camera_position.z != back_pos.z && next_camera_position.z != left_front_pos.z) return false;


    DirectX::XMFLOAT3& eye = camera->GetEye();
    eye.x = Calcu3D::LerpFloat(eye.x, next_camera_position.x, 5.0f * elapsedTime);
    eye.y = Calcu3D::LerpFloat(eye.y, next_camera_position.y, 5.0f * elapsedTime);
    eye.z = Calcu3D::LerpFloat(eye.z, next_camera_position.z, 5.0f * elapsedTime);


    if (fabsf(eye.x - next_camera_position.x) < 0.02f &&
        fabsf(eye.y - next_camera_position.y) < 0.02f &&
        fabsf(eye.z - next_camera_position.z) < 0.02f)
    {
        eye = next_camera_position;
    }

    return true;
}


void TitleCamera::Phase0(float elapsedTime)
{
    // 更新内容
    timer.Reset();
    phase_index = 1;

    constexpr DirectX::XMFLOAT3 right_front_target = { -5.0f,8.0f,7.0f };
    camera->SetTarget(right_front_target);

    constexpr DirectX::XMFLOAT3 right_back_eye = { -5.0f,0.0f,5.0f };
    next_camera_position = right_back_eye;
}


void TitleCamera::Phase1(float elapsedTime)
{
    // 更新内容

    DirectX::XMFLOAT3& eye = camera->GetEye();
    eye.x = Calcu3D::LerpFloat(eye.x, next_camera_position.x, 5.0f * elapsedTime);
    eye.y = Calcu3D::LerpFloat(eye.y, next_camera_position.y, 5.0f * elapsedTime);
    eye.z = Calcu3D::LerpFloat(eye.z, next_camera_position.z, 5.0f * elapsedTime);



    constexpr float seconds = 1.0f;
    if (timer.NowTime() < seconds) return;

    timer.Reset();
    phase_index = 2;
}


void TitleCamera::Phase2(float elapsedTime)
{
    // 更新内容
    DirectX::XMFLOAT3& target_ = camera->GetFocus();
    const DirectX::XMFLOAT3& player_pos = *target;
    target_.x = Calcu3D::LerpFloat(target_.x, player_pos.x, 5.0f * elapsedTime);
    target_.y = Calcu3D::LerpFloat(target_.y, player_pos.y, 5.0f * elapsedTime);
    target_.z = Calcu3D::LerpFloat(target_.z, player_pos.z, 5.0f * elapsedTime);



    constexpr float seconds = 1.5f;
    if (timer.NowTime() < seconds) return;

    timer.Reset();
    phase_index = 3;
}


void TitleCamera::Phase3(float elapsedTime)
{
    // 更新内容
    DirectX::XMFLOAT3& target_ = camera->GetFocus();
    const DirectX::XMFLOAT3& player_pos = *target;
    target_.x = Calcu3D::LerpFloat(target_.x, player_pos.x, 5.0f * elapsedTime);
    target_.y = Calcu3D::LerpFloat(target_.y, player_pos.y, 5.0f * elapsedTime);
    target_.z = Calcu3D::LerpFloat(target_.z, player_pos.z, 5.0f * elapsedTime);



    constexpr float seconds = 2.0f;
    if (timer.NowTime() < seconds) return;

    timer.Reset();
    phase_index = 4;
}


void TitleCamera::Phase4(float elapsedTime)
{
    constexpr float seconds = 1.5f;
    if (timer.NowTime() < seconds) return;


    timer.Reset();
    phase_index = 5;
}


void TitleCamera::Phase5(float elapsedTime)
{
    
}
