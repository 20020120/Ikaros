#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

// 追加
class Sprite_Batch;
class Camera;
#include "HealthPoint.h"
#include "DodgeCount.h"
#include "EnergyGauge.h"
#include "Score.h"
#include "Timer.h"
#include "Combo.h"
#include"Renderer.h"
//#include "Player.h"

// playerから送られてくるデータ用構造体
struct GameSystemData
{
    int* HP;
    int* Energy;
    int* Dodge;
    DirectX::XMFLOAT3* position;
};

// ゲームシステム ※シングルトンパターン

class GameSystem
{
private:
    GameSystem() {};
    ~GameSystem() {};

public:
    static GameSystem& Instance()
    {
        static GameSystem instance;
        return instance;
    }

public:
    void Initialize(ID3D11Device* Device, GameSystemData* Data);
    void Update(float ElapsedTime);
    void Render(ID3D11DeviceContext* Immediate_Context);
    void PerformRender(ID3D11DeviceContext* Immediate_Context);// 演出中の枠描画
    void Finalize();

    void f_Gui();
    void SetFrameDraw(bool a) { FrameDraw = a; }

    void SetIsChangeColor(bool frag) const { Energy_Gauge->SetChangeColor(frag); }

public://定数(Getter,Setterの関係で)
    enum class CAMERA_DIRECTION
    {
        SIDE,//（サイドビュー）
        TOP,//(トップビュー）
    };


public: // Get関数
    bool CanFinisher();     // 必殺技ができるか
    bool CanDodge();        // 回避できるか
    int GetHealthPoint();   // 体力の取得
    [[nodiscard]] CAMERA_DIRECTION GetCameraDirection() const { return cameraDir; }
    DirectX::XMFLOAT3* GetPlayerPos() { return PlayerDatas->position; }
    const auto& GetNowCombo() { return ComboManager->GetCombo(); }
    const auto& GetMaxCombo() { return ComboManager->GetMaxCombo(); }
    const auto& GetNowKilledSize() { return killed_size; }
    const auto& GetClearTimer() { return Time->NowTime(); }

public: // Set関数
    void Reset();           // 値の初期化のみ
    void SetCamera(Camera* camera) { pCamera = camera; }
    void SetCameraDirection(CAMERA_DIRECTION dir) { cameraDir = dir; }//カメラの向き
    void TimerStop() { Time->Stop(); }
    void SetRetry() { retry = true; }

    void ChangeView();
    const auto& GetChangingView() { return ChangingView; }
    void SetChangingView(bool s) { ChangingView = s; }
    void RetryBoss(const float& add_time) { Time->Add(add_time); }
    void KilledEnemy()
    {
        killed_size++;
        ComboManager->AddCombo();
    }

private:
    void RenderHitCircle(ID3D11DeviceContext* Immediate_Context,
        const DirectX::XMFLOAT4X4& view,
        const DirectX::XMFLOAT4X4& projection);

    // リトライ時専用関数
    void InitializeInSceneGame(ID3D11Device* Device, GameSystemData* Data);

public:
    //ゲームパッドの更新
    void GamePadUpdate() const{};


private: // 定数の定義 

private: // 変数の定義
    bool defined;
    std::unique_ptr<HealthPoint> HP;
    std::unique_ptr<DodgeCount> DodgeCounter;
    std::unique_ptr<EnergyGauge> Energy_Gauge;
    std::unique_ptr<Score> ScoreCounters[2];
    std::unique_ptr<Timer> Time;
    std::unique_ptr<Combo> ComboManager;
    std::unique_ptr<Sprite_Batch> spr_HitArea;

    Renderer spr_PerformanceFrame;

    // 撃破数 (SceneGame毎にリセット)
    int killed_size = 0;

    GameSystemData* PlayerDatas;

    Renderer spr_CutIn0{}; // カットインの画像
    Renderer spr_CutIn1{}; // カットインの画像
    int CutInState = {};

    Camera* pCamera;

    bool FrameDraw = false;
    //視点
	CAMERA_DIRECTION cameraDir{ CAMERA_DIRECTION::SIDE };
    bool ChangingView{};

    // Initializeの最後でfalseにする
    bool retry = false;
};