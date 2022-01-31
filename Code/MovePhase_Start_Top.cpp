
#include "MovePhase_Start_Top.h"


Start_FromBack_Top::Start_FromBack_Top(XMFLOAT3& scale_, XMFLOAT3 target_scale_) : scale(scale_), target_scale(target_scale_)
{
    constexpr float seconds = 2.8f;

    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    // 速度初期値
    speed = 50.0f;
}


void Start_FromBack_Top::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);


    const float time = timer.NowTime();

    // if文の終わりで早期returnしている
    if (time > 0.0f)
    {
        // タイマーが更新していたら実行する処理群

        // 速度の変更
        SpeedRatio(elapsedTime, time);

        // 前方へ移動
        MoveFront(elapsedTime, position, angle);

        // Y軸180度回転
        RotationX180(elapsedTime, time, angle);

        // X = (0.0f)まで補間
        LerpPositionY(elapsedTime, time, position);

        // Z軸で回転
        RotateZ(elapsedTime, time, angle);

        // スケール値の更新
        ScaleUp(elapsedTime, time);


        return;
    }

    else
    {
        // タイマーが0.0fになったら
        constexpr float pi = DirectX::XMConvertToRadians(180.0f);
        angle.x = -pi;
        angle.z = 3.14f;
        position.y = 0.0f;
        scale = target_scale;
        return;
    }
}


void Start_FromBack_Top::MoveFront(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // 前方向(モデルのz軸方向)に進む処理
    DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z).r[2];
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));

    // 位置を更新
    position.x += forward.x * speed * elapsedTime;
    position.y += forward.y * speed * elapsedTime;
    position.z += forward.z * speed * elapsedTime;
}


void Start_FromBack_Top::SpeedRatio(float elapsedTime, const float& now_time)
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


void Start_FromBack_Top::RotationX180(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle)
{
    if (now_time >= 1.0f) return;

    // 現在タイマーが2.0秒を過ぎたら
    constexpr float pi = DirectX::XMConvertToRadians(180.0f);
    angle.x = Calcu3D::LerpFloat(angle.x, -pi, 5.0f * elapsedTime);
}


void Start_FromBack_Top::LerpPositionY(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position)
{
    if (now_time >= 0.8f)return;

    // 現在タイマーが0.8秒を過ぎたら
    position.y = Calcu3D::LerpFloat(position.y, 0.0f, 6.0f * elapsedTime);
}


void Start_FromBack_Top::RotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle)
{
    if (now_time >= 1.0f)
    {
        constexpr float rotate_speed = DirectX::XMConvertToRadians(270.0f);
        angle.z += rotate_speed * elapsedTime;

        if (angle.z > 3.14f) angle.z = -3.14f;
    }
    else
    {
        angle.z = Calcu3D::LerpFloat(angle.z, 3.14f, 6.0f * elapsedTime);
    }
}


void Start_FromBack_Top::ScaleUp(float elapsedTime, const float& now_time)
{
    if (now_time >= 1.0f) return;

    scale = Calcu3D::LerpFloat3(scale, target_scale, 5.0f * elapsedTime);
}






Start_FromFront_Top::Start_FromFront_Top(XMFLOAT3& scale_, XMFLOAT3 target_scale_) : scale(scale_), target_scale(target_scale_)
{
    constexpr float seconds = 2.8f;

    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    // 速度初期値
    speed = 50.0f;
}


void Start_FromFront_Top::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);


    const float time = timer.NowTime();

    // if文の終わりで早期returnしている
    if (time > 0.0f)
    {
        // タイマーが更新していたら実行する処理群

        // 速度の変更
        SpeedRatio(elapsedTime, time);

        // 前方へ移動
        MoveFront(elapsedTime, position, angle);

        // Y軸180度回転
        RotationX180(elapsedTime, time, angle);

        // X = (0.0f)まで補間
        LerpPositionY(elapsedTime, time, position);

        // Z軸で回転
        RotateZ(elapsedTime, time, angle);

        // スケール値の更新
        ScaleUp(elapsedTime, time);


        return;
    }

    else
    {
        // タイマーが0.0fになったら
        constexpr float pi = DirectX::XMConvertToRadians(180.0f);
        angle.x = -pi;
        angle.z = 3.14f;
        position.y = 0.0f;
        scale = target_scale;
        return;
    }
}


void Start_FromFront_Top::MoveFront(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    // 前方向(モデルのz軸方向)に進む処理
    DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z).r[2];
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));

    // 位置を更新
    position.x += forward.x * speed * elapsedTime;
    position.y += forward.y * speed * elapsedTime;
    position.z += forward.z * speed * elapsedTime;
}


void Start_FromFront_Top::SpeedRatio(float elapsedTime, const float& now_time)
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
  

void Start_FromFront_Top::RotationX180(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle)
{
    if (now_time >= 1.0f) return;

    // 現在タイマーが2.0秒を過ぎたら
    constexpr float pi = DirectX::XMConvertToRadians(180.0f);
    angle.x = Calcu3D::LerpFloat(angle.x, -pi, 5.0f * elapsedTime);
}


void Start_FromFront_Top::LerpPositionY(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& position)
{
    if (now_time >= 0.8f)return;

    // 現在タイマーが0.8秒を過ぎたら
    position.y = Calcu3D::LerpFloat(position.y, 0.0f, 6.0f * elapsedTime);
}


void Start_FromFront_Top::RotateZ(float elapsedTime, const float& now_time, DirectX::XMFLOAT3& angle)
{
    if (now_time >= 1.0f)
    {
        constexpr float rotate_speed = DirectX::XMConvertToRadians(270.0f);
        angle.z += rotate_speed * elapsedTime;

        if (angle.z > 3.14f) angle.z = -3.14f;
    }
    else
    {
        angle.z = Calcu3D::LerpFloat(angle.z, 3.14f, 6.0f * elapsedTime);
    }
}


void Start_FromFront_Top::ScaleUp(float elapsedTime, const float& now_time)
{
    if (now_time >= 1.0f) return;

    scale = Calcu3D::LerpFloat3(scale, target_scale, 5.0f * elapsedTime);
}







Start_FromLeft_Top::Start_FromLeft_Top(XMFLOAT3 start)
{
    constexpr float seconds = all_time;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    control_point =
    {
        {start.x,start.y,start.z},
        {start.x + 10.0f * 1.0f,start.y,start.z + 17.0f},
        {start.x + 10.0f * 5.0f,start.y,start.z + 17.0f},
        {start.x + 10.0f * 9.0f,0.0f,start.z + 0.0f}
    };

}


void Start_FromLeft_Top::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);

    const float ratio = 1.0f - (timer.NowTime() / all_time);


    position = Calcu3D::BezierCurve(control_point, ratio);


    if (ratio <= 0.99f) return;
    LerpPositionY(elapsedTime, position);

}


void Start_FromLeft_Top::LerpPositionY(float elapsedTime, DirectX::XMFLOAT3& position)
{
    position.y = Calcu3D::LerpFloat(position.y, 0.0f, 5.0f * elapsedTime);
}





Start_FromRight_Top::Start_FromRight_Top(XMFLOAT3 start)
{
    constexpr float seconds = all_time;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    control_point =
    {
        {start.x,start.y,start.z},
        {start.x - 10.0f * 1.0f,start.y,start.z + 17.0f},
        {start.x - 10.0f * 5.0f,start.y,start.z + 17.0f},
        {start.x - 10.0f * 9.0f,0.0f,start.z + 0.0f}
    };

}


void Start_FromRight_Top::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);

    const float ratio = 1.0f - (timer.NowTime() / all_time);


    position = Calcu3D::BezierCurve(control_point, ratio);


    if (ratio <= 0.99f) return;
    LerpPositionY(elapsedTime, position);

}


void Start_FromRight_Top::LerpPositionY(float elapsedTime, DirectX::XMFLOAT3& position)
{
    position.y = Calcu3D::LerpFloat(position.y, 0.0f, 5.0f * elapsedTime);
}














Start_FromLeft_RotateY_Top::Start_FromLeft_RotateY_Top(XMFLOAT3 start)
{
    constexpr float seconds = all_time;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    control_point =
    {
        {start.x,start.y,start.z},
        {start.x + 10.0f * 1.0f,start.y,start.z + 17.0f},
        {start.x + 10.0f * 5.0f,start.y,start.z + 17.0f},
        {start.x + 10.0f * 9.0f,0.0f,start.z + 0.0f}
    };

}


void Start_FromLeft_RotateY_Top::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);

    const float ratio = 1.0f - (timer.NowTime() / all_time);


    position = Calcu3D::BezierCurve(control_point, ratio);

    LerpRotateY360(ratio, angle);


    if (ratio <= 0.99f) return;
    LerpPositionY(elapsedTime, position);

}


void Start_FromLeft_RotateY_Top::LerpPositionY(float elapsedTime, DirectX::XMFLOAT3& position)
{
    position.y = Calcu3D::LerpFloat(position.y, 0.0f, 5.0f * elapsedTime);
}


void Start_FromLeft_RotateY_Top::LerpRotateY360(const float ratio, DirectX::XMFLOAT3& angle)
{
    constexpr float pi_2 = DirectX::XMConvertToRadians(360.0f);

    angle.y = Calcu3D::LerpFloat(-3.14f, 2.0f * pi_2 - 3.14f, ratio);
}





Start_FromRight_RotateY_Top::Start_FromRight_RotateY_Top(XMFLOAT3 start)
{
    constexpr float seconds = all_time;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    control_point =
    {
        {start.x,start.y,start.z},
        {start.x - 10.0f * 1.0f,start.y,start.z + 17.0f},
        {start.x - 10.0f * 5.0f,start.y,start.z + 17.0f},
        {start.x - 10.0f * 9.0f,0.0f,start.z + 0.0f}
    };

}


void Start_FromRight_RotateY_Top::Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
{
    timer.Update(elapsedTime);

    const float ratio = 1.0f - (timer.NowTime() / all_time);


    position = Calcu3D::BezierCurve(control_point, ratio);

    LerpRotateY360(ratio, angle);


    if (ratio <= 0.99f) return;
    LerpPositionY(elapsedTime, position);
}


void Start_FromRight_RotateY_Top::LerpPositionY(float elapsedTime, DirectX::XMFLOAT3& position)
{
    position.y = Calcu3D::LerpFloat(position.y, 0.0f, 5.0f * elapsedTime);
}


void Start_FromRight_RotateY_Top::LerpRotateY360(const float ratio, DirectX::XMFLOAT3& angle)
{
    constexpr float pi_2 = DirectX::XMConvertToRadians(360.0f);

    angle.y = Calcu3D::LerpFloat(-3.14f, -2.0f * pi_2 - 3.14f, ratio);
}
