#pragma once
#include <d3d11.h>
#include <memory>

#include "TitleControl.h"

#include "SpriteBatch.h"
#include "Font.h"
#include "Light.h"
#include "ModelComponent.h"
#include "TitleCamera.h"
#include "Option.h"
#include "Scene.h"
#include "gAudioSource.h"

class TitleManager final
{
public:
    TitleManager() = default;
    ~TitleManager() = default;
public:
    void Initialize(ID3D11Device* device);
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);
    void Finalize();
    void GuiMenu();

private:
    void Input(float elapsedTime);
    void ChangeScene();
    void RenderFonts(ID3D11DeviceContext* dc);
    void SkipMovie(float elapsedTime);
    void SpeedRatio(float elapsedTime, const int& index);

private:
    using pFunc = void(TitleManager::*)(float);

private: // 関数ポインタ　タイトルの選択肢の関数
    pFunc states[6] =
    {
        &TitleManager::sTitle,
        &TitleManager::sStart,
        &TitleManager::sStageSelect,
        &TitleManager::sOption,
        &TitleManager::sEnd,
        &TitleManager::sNone,
    };

    void sTitle(float elapsedTime);
    void sOption(float elapsedTime);
    void sStart(float elapsedTime);
    void sStageSelect(float elapsedTime);
    void sEnd(float elapsedTime);
    void sNone(float elapsedTime);

private:
    static constexpr float lerp_timer_max = 0.5f;
    static constexpr float lerp_timer_min = 0.0f;

private:
    // タイトル用入力処理
    std::unique_ptr<TitleControl> input;
    // タイトル用カメラ処理
    std::unique_ptr<TitleCamera> camera;

    // オプション
    std::unique_ptr<Option> option;
    bool openOption{ false };//オプションを開いているかどうか
    bool closeOption{ false };//オプションを閉じているかどうか

    // スプライト
    std::unique_ptr<Sprite_Batch> SprTitle;
    std::unique_ptr<Font> font;
    std::unique_ptr<Sprite_Batch> SprSelect[4];

    // モデル関係
    std::unique_ptr<DirectionLight> directionLight{ nullptr };
    struct Model final
    {
        std::unique_ptr<ModelComponent> model;
        Transform transform;
        Rotate rotate;
        float speed_ratio;
    } mdl_player, mdl_garage, mdl_launch_pad,mdl_SkyDome;


    // スキップ用のタイマー
    Timer skip_timer;

    // 補間用のタイマー
    float lerp_timer = 0.0f;

    // シーン切り替えのフラグ
    SceneParent* next_scene = nullptr;

    //sound
    std::unique_ptr<AudioSource> bgm{ nullptr };
    std::unique_ptr<AudioSource> bgm_sand_storm{ nullptr };
    std::unique_ptr<AudioSource> se_decision;
    std::unique_ptr<AudioSource> se_lunch;
    std::unique_ptr<AudioSource> se_take_off;

    std::unique_ptr<AudioSource> se_doom;
    std::unique_ptr<AudioSource> se_sonic_boom;


    enum STATE
    {
        TITLE,          // タイトル画面中
        START,          // スタート画面中
        STAGE_SELECT,   // ステージ選択画面中
        OPTION,         // オプション画面中
        END,            // ゲーム終了
        NONE,           // ムービー処理中
    };

    STATE current_state_index;

    bool imgui_now_titled;

    // 演出用ライト
    // ポイントライト
    std::unique_ptr<PointLight> PointLights{}; 

    // スポットライト
    std::unique_ptr<SpotLight> mSpotLight;
    float StackLightManagerTimer{};
    int LightNum = 0;

    DirectX::XMFLOAT2 LogoPos{};

    std::unique_ptr<Effect> efk_smoke;
    Effekseer::Handle hdl_smoke = -1;
    std::unique_ptr<Effect> efk_sparkle;
    Effekseer::Handle hdl_sparkle = -1;

    // スタート時の演出
    int StartPerformanceState{}; // 遷移を管理する
    float StartPerformanceTime{}; // 時間管理

    void CameraShake(float elapsedTime,
                     bool IsShake                  /*カメラを振るか*/,
                     DirectX::XMFLOAT3 SwingWidth, /*振り幅*/
                     float OneTermSeconds,         /*一回振る秒数*/
                     int ShakeTimes                /*何回振るか*/
                     );

    int CameraShakeState{};
};
