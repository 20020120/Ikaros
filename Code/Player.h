#pragma once
#include "BaseObject.h"

#include"Controller.h"

#include"SpriteBatch.h"

#include "EventFlag.h"

#include "GameSystem.h"
//#include"ParticleEmitter.h"
#include"Effect.h"
#include"Light.h"
#include "gAudioSource.h"
#include"ConstantBuffer.h"
#include<functional>
class Player final : protected BaseObject
{
public:
    Player(ID3D11Device* d);
    ~Player() override = default;


    void Initialize() override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;
    void Render2D(ID3D11DeviceContext* dc, bool draw);
    void Finalize() override;

    //****************************************************************
    // 
    //  ゲッター
    // 
    //****************************************************************

    [[nodiscard]] const auto& GetPosition() const { return t.Position; }
    [[nodiscard]] auto* GetGameSystemData() { return &GameSystemDatas; }

    bool GetEndGame()const { return EndGame; }

    void SetPosition(DirectX::XMFLOAT3 pos) { t.Position = pos; }
    void SetLightPerformance(bool a) { IsLightPerformance = a; }
    //****************************************************************
    // 
    //  メンバ関数（クラス外からアクセスされる関数）
    // 
    //****************************************************************
    bool Damaged(const int damage); // 被ダメージ
    void GetEnergy(Type t, const int getEnergy); // エネルギーと接触
    [[nodiscard]] const auto GetIsInvincible() const { return IsInvincible; } // 無敵時間かどうか
    [[nodiscard]] const auto GetFrontVec() const { return BaseObject::GetFrontVec(); } // 無敵時間かどうか

    void SlowForFinisher(float& elapsedTime); // 必殺技中はスローにする

    bool GetIsAlive() const { return alive; }

    bool OnCameraTarget_FlingFinisher()const;

    // 必殺技かどうか
    bool GetIsFinisher() const { return IsFinisher; }
    bool GetIsBeamFinisher() { return  IsBeamFinisher; }

    //縦横切り替えの際の補正
    void ChangeCameraDirCorrect();

    // ヒットストップ
    void HitSlow(float& elapsedTime);
    void Slow(float& elapsedTime);

    auto GetIsPerforming() const { return IsPerforming; }
    bool GetAttentionCamera() const { return AttentionCamera; } // カメラを注目させる
    bool GetIsFinisher() { return IsFinisher; }
    int GetPerformanceState()const { return PerformState; }
    // プレイヤーの最初の決めポーズ
    void FirstPose(float elapsedTime);

    // プレイヤーの最後の一撃
    bool  LastAttack(float elapsedTime);

    bool GetEnemiesStop()const { return EnemiesStop; }
    //****************************************************************
    // 
    //   チュートリアル用の関数
    // 
    //****************************************************************

    bool GetIsMove(); // 移動しているかどうか
    bool GetTutorial_IsBreakCloseAttack() { return Tutorial_IsBreakCloseAttack; }
    bool GetCompleteDodgeTutorial() { return CompleteDodgeTutorial; }
    bool GetCompleteElement1Tutorial() const { return CompleteElement1Tutorial; }
    void SetIsTutorial(bool a) { IsTutorial = a; }
    void ReSetCompleteDodgeTutorial() { CompleteDodgeTutorial = false; }

    void PerformEndTutorial(float elapsedTime); // チュートリアルの終了演出

    // カメラシェイク関数を追加
    void fSetCameraShakeFunc(std::function<void()> Function_);

private:

    //****************************************************************
    // 
    // メンバ関数
    // 
    //****************************************************************
    //関数ポインタ
    using pFunc = void(Player::*)(float);

    // モードチェンジ
    void ChangeMode();

    // 移動
    pFunc Move[2] = {
        &Player::MoveHorizontal,
        &Player::MoveVertical,
    };
    void MoveVertical(float elapsedTime);
    void MoveHorizontal(float elapsedTime);

    // 移動入力
    [[nodiscard]] const auto& GetMoveVec() const;

    // 回転の更新
    void RotUpdate(float vecY);

    // 移動速度を残り体力の割合から算出
    void CalcSpeedFromHP();

    // 回避入力
    void inputDodge(float elapsedTime);

    //ジャスト回避成功時にエネルギーを増やす
    void DodgeSuccessEnergy(const int pIndex, const int eIndex);

    // 回避の回転
    void RotDodge(float elapsedTime);

    // 射撃入力
    void InputShooting(float elapsedTime);

    // エネルギー更新
    void UpdateEnergy();

    // 無敵時間更新
    void UpdateInvTime(float elapsedTime);

    // 行動制限
    void LimitMoveArea();

    //近距離攻撃
    void InputCloseAttack(float elapsedTime);

    // 必殺技
    void FlyModeFinisher(float elapsedTime);    // 飛行機形態の必殺技（突進）

    void FlyModeFinisherMini(float elapsedTime);// 飛行機形態の必殺技（簡易版）

    void Finisher_Beam(float elapsedTime); // 人型の必殺技
    void Finisher_Slash(float elapsedTime); // 人型の必殺技

    // 軽量設定のフィニッシャー
    void Finisher_BeamLight(float elapsedTime); // 人型の必殺技
    void Finisher_SlashLight(float elapsedTime); // 人型の必殺技

    // パーティクルの計算
    void CalcParticle();

    // 死亡時の演出
    void PerformanceDead(float elapsedTime);

    // 残像の位置を計算する
    void UpdateAfterImage(float elapsedTime);

    //衝突判定
    void VsProjectile(float elapsedTime);
    [[nodiscard]] int IsHitProjectile(const DirectX::XMFLOAT3 HitAreaSize) const;//当たっていたらindexあたってなかったら-1
    void VsEnemy(float elapsedTime);
    [[nodiscard]] int IsHitEnemy(const DirectX::XMFLOAT3 HitAreaSize) const;//当たっていたらindexあたってなかったら-1
    //全ての敵にダメージを与える
    void AttackAllEnemy(float damage);



    // 残像の計算
    void CalcAfterImage(float elapsedTime);


    //--------------------<デバッグ用>--------------------//
    void GuiMenu();

    //****************************************************************
    // 
    // メンバ変数
    // 
    //****************************************************************
    enum AnimationName {
        wait,
        FinisherBeamBegin,
        FinisherBeamLoop,
        FinisherBeamEnd,
        Shot_Recoil,
        Slash_side,
        Slash_top,
        FinisherSwordBegin,
        FinisherSwordSlash,
        GameOver,
        GameClearBegin,
        GameClearLoop,
        transform,
        inv_transform,
        wing,
        finisher_begin,
        finisher_end,
        finisher_end_pose,
    };

    //--------------------<エネルギー>--------------------//
    int EnergyGauge{};  // モードチェンジに使うエネルギーのゲージ(-20~20)
    static constexpr int MaxEnergyCount = { 40 };
    static constexpr int MinEnergyCount = { 0 };
    bool canTransform{ false };
    int increaseEnergy = 5;

    //--------------------<変形>--------------------//

    bool EndTransform{};
    static constexpr int HealHpCount = { 4 };

    //--------------------<体力>--------------------//
    static constexpr int MaxHitPoint{ 15 }; // 体力の最大値
    static constexpr int MinHitPoint{ 0 }; // 体力の最大値
    int HitPoint{};   // 体力
    bool alive{ true };

    int damageRatio = 2;

    //--------------------<スピード>--------------------//
    static constexpr float MaxSpeed{ 60.0f };  // 最大速度
    static constexpr float MinSpeed{ 30.0f };  // 最小速度
    float Speed{};    // 速度


    //--------------------<回避>--------------------//
    bool ReadyDodge{}; // 回避待機
    int DodgeCount{}; // 回避可能回数
    static constexpr int MaxDodgeCount = { 5 }; // 最大回避回数
    bool CanDodge{}; // 回避可能かどうか
    float StackDodgeTime{}; // ジャスト回避の受付時間
    static constexpr float DodgeTime = { 1.0f }; // ジャスト回避の受付時間
    float RotRatio{};  // 回転制御用の変数

    float DodgeVecX{}; // どの方向に回避するか
    float DodgeVecY{}; // どの方向に回避するか
    static constexpr float DodgeSpeed = 40.0f; // 回避速度
    bool dodgeSuccess{ false };
    DirectX::XMFLOAT3 dodgeSize{};
    float BeforeDodgeAngle{}; // 回避前のプレイヤーの角度
    DirectX::XMFLOAT2 dodgeAreaScale{ 1,1 };//当たり判定用

    //--------------------<射撃>--------------------//
    static constexpr float ShootInterval = { 0.2f }; //射撃してから次に撃てるまでの間隔（秒）
    float StackShootInterval{}; //射撃してからの経過時間（射撃できるかの判定に使う）
    bool CanShoot{}; // 

    DirectX::XMFLOAT3 BulletVelocity{}; // 弾の速度
    DirectX::XMFLOAT3 BulletVelocityUp{}; // 弾の速度
    DirectX::XMFLOAT3 BulletVelocityDown{}; // 弾の速度
    DirectX::XMFLOAT3 BulletVelocityRight{}; // 弾の速度
    DirectX::XMFLOAT3 BulletVelocityLeft{}; // 弾の速度

    //--------------------<無敵時間>--------------------//
    float StackInvincibleTime{}; //ダメージを受けてからの経過時間
    float LimitInvincibleTime = { 2.0f }; //無敵時間の長さ（秒）
    bool IsInvincible{};  // 無敵かどうか

    //--------------------<プレイヤーの行動範囲制限>--------------------//
    static constexpr  DirectX::XMFLOAT2 LeftTop = { -47.0f,26.0f };
    static constexpr  DirectX::XMFLOAT2 RightBottom = { 47.0f,-26.0f };

    //--------------------<近距離攻撃>--------------------//
    float attackInterval{ 0.0f };
    static constexpr float MaxAttackInterval{ 0.5f };
    DirectX::XMFLOAT3 attackArea{};
    float StackCloseAttackInterval{};
    const float CloseAttackInterval{ 1.3f };
    bool IsSlash{}; // 斬ってるかどうか


    //--------------------<必殺技>--------------------//

    bool IsFinisher{};  // 必殺技してるかどうか
    int FinisherState{}; // 必殺技の遷移管理
    float FinisherRatio{}; // 必殺技の関数内で使われる線形補完の割合（初期化注意）
    float FinisherDamage = { 4.0f };
    bool EnemiesStop{}; // 敵の動きを止める

    bool IsBeamFinisher{};
    //--------------------<ヒットストップ>--------------------//
    float StackHitStopSeconds{}; // ヒットストップの経過時間
    const float HitStopSecond{ 0.2f };

    //--------------------<演出に関する変数>--------------------//
    int PerformState{}; // 演出を管理する変数
    bool IsPerforming{}; //演出中かどうか
    float PerformRate{}; // 演出の補完率
    float PerformanceAngleRatio{ 0.0f };
    bool IsLightPerformance{};
    Renderer spr_CutIn0{}; // カットインの画像
    Renderer spr_CutIn1{}; // カットインの画像
    bool IsCutIn{};
    float StackSkipTimer{}; // スキップの長押し時間に関する変数
    bool PreferSkipPerformance{}; // スキップを許可

    bool IsLastPerformance{};

    enum CameraDirection
    {
        Side,
        Top,
    }SlashDir{};


    float StackAfterImageTime{};

    bool ShowAfterImage{};

    int AfterImageState{};

    float StackPerformanceTime = 0.0f;
    bool EndGame = false;

    //--------------------<チュートリアル>--------------------//
    bool Tutorial_IsBreakCloseAttack{ false }; //近接攻撃のチュートリアル完了かどうか
    bool CompleteDodgeTutorial{ false };       // 回避のチュートリアル完了かどうか
    bool IsTutorial{ false }; // チュートリアルかどうか
    bool CompleteElement1Tutorial{ false };   //  エネルギーのチュートリアルを終えたか
    int TutorialPerformState{}; // チュートリアルの演出管理変数

    //--------------------<カメラ>--------------------//
    bool AttentionCamera{}; // カメラを注目させる
    std::function<void()> mCameraShakeFunc{ nullptr };
    // デバッグ用
    bool IsDebug{ false };

    GameSystemData GameSystemDatas;

    // スプライト
    std::unique_ptr<Sprite_Batch> spr_HitArea;
    Renderer sprBlackCurtain{}; // 暗転用の幕


    // イベントフラグ
    EventFlagSystem Flags;

    //--------------------<軌跡>--------------------//
    //ParticleEmitter particleEmitter;

    struct GsData
    {
        float pow{};
        DirectX::XMFLOAT3 pad0;
    }mGsData{};
    ConstantBuffer Gs_CBuffer{};

    std::unique_ptr<GamePointLight> Light;
    DirectX::XMFLOAT3 PlayerColor{};
    //****************************************************************
    // 
    // サウンド
    // 
    //****************************************************************
    std::vector<std::unique_ptr<AudioSource>> se;
    std::vector<float> volumes;
    enum SE
    {
        MODE_CHANGE,
        HIT_SAME_COLOR,
        HIT_DIFFERENT_COLOR,
        HIT_INVINCIBLE,
        CLOSE_ATTACK,
        GET_ENERGY,
        RED_BULLET_SHOT,
        BLUE_BULLET_SHOT,
        DODGE,
        CRITICAL_DODGE,
        HEAL,
        LastSlash0,
        BossBomb0,
        BossBomb2,
        BossBomb3,
        BossBomb4,
        BEAMCHARGE,
        BEAM,

    };

    //****************************************************************
    // 
    // エフェクト
    // 
    //****************************************************************

    std::unique_ptr<Effect> efk_Charge;             // 溜め
    Effekseer::Handle hdl_Charge = -1;
    std::unique_ptr<Effect> efk_Beam;             // レーザー
    Effekseer::Handle hdl_Beam = -1;
    std::unique_ptr<Effect> efk_MuzzleFlash;        // マズルフラッシュ
    Effekseer::Handle hdl_MuzzleFlash = -1;
    std::unique_ptr<Effect> efk_RedDodgeSuccess;    // 回避成功時（赤）
    Effekseer::Handle hdl_RedDodgeSuccess = -1;
    std::unique_ptr<Effect> efk_BlueDodgeSuccess;   // 回避成功時（青）
    Effekseer::Handle hdl_BlueDodgeSuccess = -1;
    std::unique_ptr<Effect> efk_WeakDamaged;        // 被ダメージ(弱点)
    Effekseer::Handle hdl_WeakDamaged = -1;
    std::unique_ptr<Effect> efk_UnWeakDamaged;      // 被ダメージ(弱点じゃない)
    Effekseer::Handle hdl_UnWeakDamaged = -1;
    std::unique_ptr<Effect> efk_InvisibleDamaged;   // 被ダメージ(無敵)
    Effekseer::Handle hdl_InvisibleDamaged = -1;
    std::unique_ptr<Effect> efk_SonicBoom;          // ソニックブーム
    Effekseer::Handle hdl_SonicBoom = -1;
    std::unique_ptr<Effect> efk_RedSlash;           // 斬撃（赤）
    Effekseer::Handle hdl_RedSlash = -1;
    std::unique_ptr<Effect> efk_BlueSlash;          // 斬撃（青）
    Effekseer::Handle hdl_BlueSlash = -1;
    std::unique_ptr<Effect> efk_Heal;               // 回復
    Effekseer::Handle hdl_Heal = -1;
    std::unique_ptr<Effect> efk_SlashFinisher;      // 剣の必殺技
    Effekseer::Handle hdl_SlashFinisher = -1;

    std::unique_ptr<Sprite_Batch> ClearMask{ nullptr };
    float mClearDissolvePower{ 0.0f };
    //****************************************************************
    // 
    // 残像
    // 
    //****************************************************************
    struct AfterImage
    {
        Transform t{};
        Rotate r{};
        std::unique_ptr<ModelComponent> Model{};
    }mAfterImage;
 };