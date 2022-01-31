#pragma once
#include<DirectXMath.h>
#include<d3d11.h>
#include<wrl.h>

#include"Controller.h"
#include"Effect.h"
#include<functional>
class Camera 
{
public:
    Camera(ID3D11Device* d);
    ~Camera();

    void f_CameraUpdate(ID3D11DeviceContext* dc);
    void f_Gui();

    void Update(float elapsedTime);
    void FirstPoseUpdate(float elapsedTime);
    void TitleConstantBufferUpdate(ID3D11DeviceContext* dc);
    
    void f_UpdateCameraForMouse();
    void f_SettingMouse();

    void DemoCameraUpdate(float elapsedTime);

    void SetAttention(bool a) { attentionPlayer = a; }

    struct SceneConstance
    {
        DirectX::XMFLOAT4X4 ViewProjection{};
        DirectX::XMFLOAT4 CameraPosition{};
    };


    //--------------------<注視点を設定する>--------------------//
    void SetTarget(DirectX::XMFLOAT3 target);
    void SetTarget_FlyingFinisher(DirectX::XMFLOAT3 target);
    void SetLookAtLH(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);
    void SetAngle(DirectX::XMFLOAT3 angle);

    void SetIsTargetPlayer(bool Is) { IsTargetPlayer = Is; }
    //--------------------<ゲッター>--------------------//

    SceneConstance& GetData() { return data; }
    auto& GetFront() { return Forward; }
    auto& GetRight() { return Right; }
    auto& GetEye() { return Eye; }
    auto& GetView() { return View; }
    auto& GetProjection() { return Projection; }
    auto& GetChangeCameraDir() { return changeCameraDir; }
    auto& GetChangeCameraDirState() { return changeCameraDirState; }
    const auto& GetTarget() { return Target; }
    auto& GetFocus() { return Focus; }
    void ChangeView();
    auto GetFovDegree() const { return camera_fov_degree; }
    //--------------------<セッター>--------------------//
    void SetCameraShake() { camera_shake = true; }
    void SetDemoCameraShake() { demo_camera_shake = true; origin_demo_eye = Eye; }
    void SetPlayerPos(const DirectX::XMFLOAT3 pos)
    {
	    playerPos = pos;
    }

    void SetAttentionBoss(bool a) { attentionBoss = a; }
    void SetBossPosition(DirectX::XMFLOAT3 p) { BossPosition = p; }

    void UpdateLastPerformance(float elapsedTime, int State);
    void SetFovDegree(const float degree_) { camera_fov_degree = degree_; }
private:
    //--------------------<カメラシェイク用>--------------------//
    void Shake(float elapsedTime, DirectX::XMFLOAT3& eye);
    void ShakeZY();
    void ShakeZX();
    void ShakeInit();

    void DemoShake(float elapsedTime, DirectX::XMFLOAT3& eye);
    void ShakeXY(float elapsedTime);
    void DemoShakeInit();


    //--------------------<演出用>--------------------//
    void AttentionPlayer(float elapsedTime);

    void AttentionBoss(float elapsedTime);



//--------------------<縦シューティングと横シューティングか切り替える>--------------------//
    void ChangeCameraDir(float elasped);
    void ChangeCameraDirToUp(float elapsedTime);
    void ChangeCameraDirToSide(float elapsedTime);



private:
    //--------------------<カメラの姿勢>--------------------//

    DirectX::XMFLOAT4 LightDirection;
    DirectX::XMFLOAT4 CameraPosition;
    float LightPower;


    DirectX::XMFLOAT3 Eye;
    DirectX::XMFLOAT3 Focus;
    DirectX::XMFLOAT3 Up;
    
    DirectX::XMFLOAT3 Right;
    DirectX::XMFLOAT3 Forward;
    
    DirectX::XMFLOAT4X4 View;
    DirectX::XMFLOAT4X4 Projection;

    DirectX::XMFLOAT3 Target;

    DirectX::XMFLOAT3 Angle;

    SceneConstance data{};



    //--------------------<カメラシェイク用>--------------------//
    bool camera_shake = false;
    static constexpr float DEFAULT_SHAKE_TIMER = 0.1f;
    float shake_timer = DEFAULT_SHAKE_TIMER;
    DirectX::XMFLOAT3 shake_power = {};

    bool demo_camera_shake = false;
    static constexpr float DEFAULT_DEMO_SHAKE_TIMER = 2.5f;
    float demo_shake_timer = DEFAULT_DEMO_SHAKE_TIMER;
    DirectX::XMFLOAT2 demo_shake_range = {};
    DirectX::XMFLOAT3 demo_shake_power = {};
    DirectX::XMFLOAT3 origin_demo_eye = {};

    //--------------------<プレイヤーの必殺技で追従中かどうかの変数>--------------------//
    bool IsTargetPlayer{};
    bool IsPullPlayer{}; // 必殺技中に線形補完でプレイヤーから遠ざかって元の位置で待機

    //--------------------<縦シューティングか横シューティングか切り替える用>--------------------//
    bool changeCameraDir{ false };
    int changeCameraDirState{ 0 };
    const DirectX::XMFLOAT3 eyeTop{ 0.0f,0.0f,20.0f };
    const DirectX::XMFLOAT3 eyeSide{ 0.0f,20.0f,-10.0f };

    //--------------------<コントローラー>--------------------//


    //****************************************************************
    // 
    // 各種マウス設定に関する変数
    // 
    //****************************************************************
    
    bool OpenMenu{};
    //--------------------<反転>--------------------//
    bool ReverseX{};
    bool ReverseY{};

    //--------------------<マウス感度>--------------------//
    DirectX::XMFLOAT2 sensitivity{0.3f,0.3f};


    //--------------------<マウスを操作させない>--------------------//
    bool Locked{true};


    Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
public:
    std::function<void()> mCameraShakeFunc{ nullptr };
private:

    //--------------------<演出用>--------------------//
    bool attentionPlayer{ false };
    DirectX::XMFLOAT3 playerPos = {};
    float ratio = 0.02f;
    float targetX = -55.0f;

    bool attentionBoss{ false };
    DirectX::XMFLOAT3 BossPosition{};

    float camera_fov_degree = 45.0f;    // Projection行列を作る際の視野角 (使っているのは TitleConstantBufferUpdate() だけ)
    //                                  // GetterとSetterあり


    //****************************************************************
    // 
    // 定数
    // 
    //****************************************************************

    const float RollSpeed{ 3.0f };
    float MaxAngleX = DirectX::XMConvertToRadians(45);
    float MinAngleX = DirectX::XMConvertToRadians(-45);
};
