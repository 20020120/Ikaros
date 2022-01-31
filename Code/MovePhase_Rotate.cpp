

#include "MovePhase_Rotate.h"

#include "User_Function.h"


void RotateXToTarget::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // 目標までの正規化したベクトル
    DirectX::XMVECTOR to_target_direction = DirectX::XMVector3Normalize(Calcu3D::MakeVectorAtoB(position, *p_target));

    // 正規化した正面方向のベクトル
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    DirectX::XMVECTOR up = DirectX::XMVector3Normalize(M.r[1]);

    // 正規化された二つのベクトルの内積を逆コサインする -> 角度が求まる
    float dot_ = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, front));
    if (dot_ > 0.0f)dot_ += -FLT_EPSILON;
    const float angle_ = std::acosf(dot_);

    // 内積で左右判定 (見た目は上下)
    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, up));
    constexpr float ratio = 1.0f;
    angle.x += dot > 0.0f ? -angle_ : angle_;

    // カウントを増加させる
    ++*p_rush_count;
}



void RotateYToTarget::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{

    // 目標までの正規化したベクトル
    DirectX::XMVECTOR to_target_direction = DirectX::XMVector3Normalize(Calcu3D::MakeVectorAtoB(position, *p_target));

    // 正規化した正面方向のベクトル
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(M.r[0]);

    // 正規化された二つのベクトルの内積を逆コサインする -> 角度が求まる
    float dot_ = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, front));
    if (dot_ > 0.0f)dot_ += -FLT_EPSILON;
    const float angle_ = std::acosf(dot_);

    // 内積で左右判定
    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, right));
    angle.y += dot > 0.0f ? -angle_ : angle_;

    // カウントを増加させる
    ++* p_rush_count;
}






void RotateXToTargetLerp::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // 目標までの正規化したベクトル
    DirectX::XMVECTOR to_target_direction = Calcu3D::MakeVectorAtoB(position, *p_target, true);

    // 正規化した正面方向のベクトル
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    DirectX::XMVECTOR up = DirectX::XMVector3Normalize(M.r[1]);

    // 正規化された二つのベクトルの内積を逆コサインする -> 角度が求まる
    float dot_ = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, front));
    if (dot_ > 0.0f)dot_ += -FLT_EPSILON;
    angle_ = std::acosf(dot_);

    constexpr float epsilon_radian = DirectX::XMConvertToRadians(0.1f);
    if (std::fabsf(angle_) <= epsilon_radian) return;

    // 内積で左右判定 (見た目は上下)
    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, up));
    angle.x += dot > 0.0f ? Calcu3D::LerpFloat(0.0f, -angle_, ratio * elapsedTime) : Calcu3D::LerpFloat(0.0f, angle_, ratio * elapsedTime);
}



void RotateYToTargetLerp::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // 目標までの正規化したベクトル
    DirectX::XMVECTOR to_target_direction = Calcu3D::MakeVectorAtoB(position, *p_target, true);

    // 正規化した正面方向のベクトル
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(M.r[0]);

    // 正規化された二つのベクトルの内積を逆コサインする -> 角度が求まる
    float dot_ = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, front));
    if (dot_ > 0.0f)dot_ += -FLT_EPSILON;
    angle_ = std::acosf(dot_);

    constexpr float epsilon_radian = DirectX::XMConvertToRadians(0.1f);
    if (std::fabsf(angle_) <= epsilon_radian) return;

    // 内積で左右判定
    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, right));
    angle.y += dot < 0.0f ? Calcu3D::LerpFloat(0.0f, -angle_, ratio * elapsedTime) : Calcu3D::LerpFloat(0.0f, angle_, ratio * elapsedTime);

}





void RotateXToTargetLerp_LimitTimer::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);

    //     現在の補完率 = 進んだ時間 / 制限時間
    const float ratio = 1.0f - (timer.NowTime() / limit_time);


    // 目標までの正規化したベクトル
    DirectX::XMVECTOR to_target_direction = Calcu3D::MakeVectorAtoB(position, *p_target, true);

    // 正規化した正面方向のベクトル
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    DirectX::XMVECTOR up = DirectX::XMVector3Normalize(M.r[1]);

    // 正規化された二つのベクトルの内積を逆コサインする -> 角度が求まる
    float dot_ = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, front));
    if (dot_ > 0.0f)dot_ += -FLT_EPSILON;
    angle_ = std::acosf(dot_);

    constexpr float epsilon_radian = DirectX::XMConvertToRadians(0.1f);
    if (std::fabsf(angle_) <= epsilon_radian) return;

    // 内積で左右判定 (見た目は上下)
    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, up));
    angle.x += dot > 0.0f ? Calcu3D::LerpFloat(0.0f, -angle_, ratio/*   * elapsedTime  */) : Calcu3D::LerpFloat(0.0f, angle_, ratio/*   * elapsedTime  */);
}



void RotateYToTargetLerp_LimitTimer::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);

    //     現在の補完率 = 進んだ時間 / 制限時間
    const float ratio = 1.0f - (timer.NowTime() / limit_time);


    // 目標までの正規化したベクトル
    DirectX::XMVECTOR to_target_direction = Calcu3D::MakeVectorAtoB(position, *p_target, true);

    // 正規化した正面方向のベクトル
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(M.r[0]);

    // 正規化された二つのベクトルの内積を逆コサインする -> 角度が求まる
    float dot_ = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, front));
    if (dot_ > 0.0f)dot_ += -FLT_EPSILON;
    angle_ = std::acosf(dot_);

    constexpr float epsilon_radian = DirectX::XMConvertToRadians(0.1f);
    if (std::fabsf(angle_) <= epsilon_radian) return;

    // 内積で左右判定
    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, right));
    angle.y += dot < 0.0f ? Calcu3D::LerpFloat(0.0f, -angle_, ratio/*   * elapsedTime  */) : Calcu3D::LerpFloat(0.0f, angle_, ratio/*   * elapsedTime  */);

}






void RotateX::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);


    // 経過時間に合わせた補完率を算出する
    //     現在の補完率 = 経過時間 / 制限時間
    const float ratio = 1.0f - (timer.NowTime() / all_time);


    constexpr float angle_speed_max = DirectX::XMConvertToRadians(810.0f);


    // 残り時間が設定時間の半分以上のとき
    if (timer.NowTime() >= half_time)
    {
        // 徐々に速度を上げる
        angle_speed = Calcu3D::LerpFloat(0.0f, angle_speed_max, ratio);

        angle.x += angle_speed * elapsedTime;


        constexpr float pi = DirectX::XMConvertToRadians(180.0f);
        if (angle.x < 2.0f * pi) return;

        angle.x += -2.0f * pi;
    }



    // 残り時間が設定時間の半分を下回ったとき
    else
    {
        // 徐々に速度を落とす
        angle_speed = Calcu3D::LerpFloat(angle_speed_max, 0.0f, ratio);


        // 角速度がangle_speed_max の　0.1倍より大きかったら
        if (angle_speed > angle_speed_max * 0.1f)
        {
            angle.x += angle_speed * elapsedTime;

            constexpr float pi = DirectX::XMConvertToRadians(180.0f);
            if (angle.x < 2.0f * pi) return;

            angle.x += -2.0f * pi;
        }

        // 角速度が0になったら
        else
        {

            constexpr float pi = DirectX::XMConvertToRadians(180.0f);

            if (angle.x < pi)
            {
                // x軸角度が0.0fになるように補完する
                angle.x = Calcu3D::LerpFloat(angle.x, 0.0f, 3.0f * elapsedTime);
            }

            else
            {
                // x軸角度が6.28fになるように補完する
                angle.x = Calcu3D::LerpFloat(angle.x, 2.0f * pi, 3.0f * elapsedTime);
            }
        }
    }



}



void RotateY::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);


    // 経過時間に合わせた補完率を算出する
    //     現在の補完率 = 経過時間 / 制限時間
    const float ratio = 1.0f - (timer.NowTime() / all_time);


    constexpr float angle_speed_max = DirectX::XMConvertToRadians(810.0f);


    // 残り時間が設定時間の半分以上のとき
    if (timer.NowTime() >= half_time)
    {
        // 徐々に速度を上げる
        angle_speed = Calcu3D::LerpFloat(0.0f, angle_speed_max, ratio);

        angle.y += angle_speed * elapsedTime;


        constexpr float pi = DirectX::XMConvertToRadians(180.0f);
        if (angle.y < pi) return;

        angle.y += -2.0f * pi;
    }



    // 残り時間が設定時間の半分を下回ったとき
    else
    {
        // 徐々に速度を落とす
        angle_speed = Calcu3D::LerpFloat(angle_speed_max, 0.0f, ratio);


        // 角速度がangle_speed_max の　0.1倍より大きかったら
        if (angle_speed > angle_speed_max * 0.1f)
        {
            angle.y += angle_speed * elapsedTime;

            constexpr float pi = DirectX::XMConvertToRadians(180.0f);
            if (angle.y < pi) return;

            angle.y += -2.0f * pi;
        }

        // 角速度が0になったら
        else
        {
            constexpr float pi = DirectX::XMConvertToRadians(180.0f);

            if(angle.y < 0.0f)
            {
                // y軸角度が-3.14fになるように補完する
                angle.y = Calcu3D::LerpFloat(angle.y, -pi, 3.0f * elapsedTime);
            }

            else
            {
                // y軸角度が3.14fになるように補完する
                angle.y = Calcu3D::LerpFloat(angle.y, pi, 3.0f * elapsedTime);
            }
            
        }
    }



}



void RotateZ::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);


    // 経過時間に合わせた補完率を算出する
    //     現在の補完率 = 経過時間 / 制限時間
    const float ratio = 1.0f - (timer.NowTime() / all_time);


    constexpr float angle_speed_max = DirectX::XMConvertToRadians(810.0f);


    // 残り時間が設定時間の半分以上のとき
    if (timer.NowTime() >= half_time)
    {
        // 徐々に速度を上げる
        angle_speed = Calcu3D::LerpFloat(0.0f, angle_speed_max, ratio);

        angle.z += angle_speed * elapsedTime;


        constexpr float pi = DirectX::XMConvertToRadians(180.0f);
        if (angle.z < pi) return;

        angle.z += -2.0f * pi;
    }



    // 残り時間が設定時間の半分を下回ったとき
    else
    {
        // 徐々に速度を落とす
        angle_speed = Calcu3D::LerpFloat(angle_speed_max, 0.0f, ratio);


        // 角速度がangle_speed_max の　0.1倍より大きかったら
        if (angle_speed > angle_speed_max * 0.1f)
        {
            angle.z += angle_speed * elapsedTime;

            constexpr float pi = DirectX::XMConvertToRadians(180.0f);
            if (angle.z < pi) return;

            angle.z += -2.0f * pi;
        }

        // 角速度が0になったら
        else
        {

            constexpr float pi = DirectX::XMConvertToRadians(180.0f);

            if (angle.z < 0.0f)
            {
                // y軸角度が-3.14fになるように補完する
                angle.z = Calcu3D::LerpFloat(angle.z, -pi, 3.0f * elapsedTime);
            }

            else
            {
                // y軸角度が3.14fになるように補完する
                angle.z = Calcu3D::LerpFloat(angle.z, pi, 3.0f * elapsedTime);
            }
        }
    }



}










void RotateToUpLerp::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // 正規化した正面方向のベクトル
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    // 正規化したy軸"正"方向のベクトル
    DirectX::XMVECTOR y_up = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

    // 正規化された二つのベクトルの内積を逆コサインする -> 角度が求まる
    const float angle_ = std::acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(y_up, front)));

    constexpr float ratio = 1.0f;
    angle.x += -Calcu3D::LerpFloat(0.0f, angle_, ratio * elapsedTime);
}



void RotateToDownLerp::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // 正規化した正面方向のベクトル
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
    // 正規化したy軸"負"方向のベクトル
    DirectX::XMVECTOR y_down = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));

    // 正規化された二つのベクトルの内積を逆コサインする -> 角度が求まる
    const float angle_ = std::acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(y_down, front)));

    constexpr float ratio = 1.0f;
    angle.x += Calcu3D::LerpFloat(0.0f, angle_, ratio * elapsedTime);
}



void RotateInverseY::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    angle.y += Calcu3D::LerpFloat(0.0f, half_pi, 0.5f * elapsedTime);

}
