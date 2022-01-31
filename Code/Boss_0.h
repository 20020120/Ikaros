#pragma once
#include"Enemy.h"
#include<vector>
#include "BaseProjectile.h"
#include "Effect.h"
#include"EventFlag.h"
#include "Light.h"


class Boss_0 final :public Enemy
{
public:
    Boss_0(ID3D11Device* Device);
    ~Boss_0()override;

    void Initialize() override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;

protected:


    void Behavior(float elapsedTime) override;
    void Move(float elapsedTime) override;
    void FirstMotion(float elapsedTime);
private:
    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************
    void GuiMenu();

    // ボスから視点を変更
    void ChangeView();

    void Shot_ShotRadian(BaseProjectile::Parent p,DirectX::XMFLOAT3 Radian); // 射撃角度を使って射撃

    void AttackManager(float elapsedTime);
    void T_ShotFront() const;

    void T_ShotEnergyFront()const;
    void ShotEnergyShotRadian(BaseProjectile::Parent p, DirectX::XMFLOAT3 Radian) const;
    

    void S_ShotEnergyFront()const;

    void T_ShotHoming(int FirstVec);

    // 視点変更時に演出
    void PerformanceView(float elapsedTime);
    void Performancemanager(float elapsedTime);

    //--------------------<ダメージ処理>--------------------//
    void Collision() override;

    //--------------------<発射角度からベクトルを取得>--------------------//
    const auto& GetShotVec(DirectX::XMFLOAT3 rad) const;
    

    

    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************
    int CurrentView{}; // 現在の視点
    int NextView{ -1 }; // 次の視点

    //--------------------<発狂に関する変数>--------------------//
    bool IsAngry{};

    //--------------------<登場時の動きに関する変数>--------------------//
    bool CompleteFirstMotion{}; // 最初の動きが完了したかどうか
    float StackTimer{}; // タイマー
    const float Timer{ 5.0f };
    float StackZoomPower{};
    

    //--------------------<移動に関する変数>--------------------//
    float Speed{10.0f}; // 移動速度
    enum MoveType
    {
        STOP,
        MOVE,
    };
    int movetype{};

    // 移動制限
    const float LimitTop = {};
    const float LimitBottom = {};
    const float LimitRight = {45.0f};
    const float LimitLeft = {-45.0f};


    //--------------------<攻撃に関する変数>--------------------//

    // どの角度で発射するか
    DirectX::XMFLOAT3 ShotRadian{};
    
   
    bool CompleteAttack{}; // 攻撃終了

    //--------------------<ライトに関する変数>--------------------//
    float LightRange{}; // 影響度
    DirectX::XMFLOAT3 LightColor{}; // 色



    enum Mode
    {
        TOP,         // 縦
        SIDE,        // 横
        ANGRY_TOP,   // 発狂縦
        ANGRY_SIDE,  // 発狂横

    };
    Mode CurrentMode{}; // 現在のボスの強さ

    const float NormaAttackInterval = { 1.5f }; // 射撃間隔（秒）
    float StackNormalAttackInterval{};


    float Ratio{}; // 線形補完に使うレート
    int AttackState{}; // 薙ぎ払い攻撃の状態
    float StackAttackInterval{}; // 薙ぎ払い攻撃の経過時間
    const float NagiriInterval{0.17f}; // 薙ぎ払い攻撃の経過時間(秒)

    //--------------------<体力に関する>--------------------//
    const float MaxHitPoint = 100.0f;

    //--------------------<軸変更の時の>--------------------//
    bool CompleteChangeView{}; // 変換完了か
    bool InputChangeView{};    // 変換入力完了か

    //--------------------<攻撃管理変数>--------------------//
    int CurrentAttackNumber{};
    int CurrentAttackComboNumber{}; // どの攻撃の配列を選択するか


    //--------------------<演出に関する>--------------------//
    int PerformanceState{};
    float StackPerformanceSecond = 0.0f;
    const float PerformanceSeconds = 5.5f;
    bool EndPerformance = false;
    bool IsPerformance{ false };
    bool IsLastPerformance{ false };

    //関数ポインタ
    using pBehavior = void(Boss_0::*)(float);
    std::vector<std::vector<pBehavior>> SideBehaviorVec;  // 攻撃パターンが詰まった配列
    std::vector<std::vector<pBehavior>> TopBehaviorVec;

    const int AddIndexTop = 5; // 発狂後の攻撃パターン配列を指す
    const int AddIndexSide = 5; // 発狂後の攻撃パターン配列を指す

    std::vector<int> numVec{};
    pBehavior MoveArray[2][2]{};

    //--------------------<エフェクト>--------------------//
    //std::unique_ptr<Effect> angryEffect{ nullptr };

    //--------------------<ビームのモデル>--------------------//
    struct BeamEmitter
    {
        std::unique_ptr<ModelComponent> Model;
        Transform t;
        Rotate r;
    }mBeamEmitter;
    bool ShowBeamEmitter{};


    //--------------------<ライト>--------------------//
    std::unique_ptr<GamePointLight> mPointLight;

    //--------------------<イベントフラグ>--------------------//
    EventFlagSystem laserFlag;
    float StackInsurance = 0.0f;



    //******************************************************************************************************************************************************//
    //                                                                                                                                                      //
    //                                                            関数ポインタに詰める関数達                                                                //
    //                                                                                                                                                      //
    //******************************************************************************************************************************************************//


    //****************************************************************
    // 
    //   移動
    // 
    //****************************************************************

    void T_Stop(float elaspedTime);  // 止まる
    void T_Move(float elaspedTime);  // 動く

    void S_Stop(float elaspedTime);  // 止まる
    void S_Move(float elaspedTime);  // 動く

    //****************************************************************
    // 
    //  攻撃
    // 
    //****************************************************************

    // 初期の移動
    void T_MoveFirstPosition(float elapsedTime);
    void S_MoveFirstPosition(float elapsedTime);
    
    // 攻撃終了処理
    void EndAttack(float elapsedTime);

    //--------------------<縦シューティング>--------------------//

    void T_NormalAttack(float elapsedTime); // 常に発射している弾（３発）
    void T_NagiriAttack(float elapsedTime); // 薙ぎ払い攻撃（2回）
    void T_NagiriEnergy(float elapsedTime); // 薙ぎ払いエネルギー（2回）
    void T_Irradiation(float elapsedTime); // 照射ビーム
    void T_AllRangeAttack(float elapsedTime); // 全体に攻撃
    void T_Horming(float elapsedTime); // ホーミング弾発射
    void T_MoveShot(float elapsedTime); // 移動撃ち

    //--------------------<横シューティング>--------------------//

    void S_NormalAttack(float elapsedTime); // 常に発射している弾（３発）
    void S_NagiriAttack(float elapsedTime); // 薙ぎ払い攻撃（2回）
    void S_NagiriEnergy(float elapsedTime); // 薙ぎ払いエネルギー（2回）
    void S_Irradiation(float elapsedTime); // 照射ビーム
    void S_AllRangeAttack(float elapsedTime); // 全体に攻撃
    void S_Horming(float elapsedTime); // ホーミング弾発射
    void S_Beam(float elapsedTimr); // ビーム
    void S_CrossNagiriAttack(float elpasedTime); // 両サイドから薙ぎ払い発射
    

    //------------------------------------------------<発狂後>------------------------------------------------//
    
    //--------------------<縦シューティング>--------------------//

    void T_V_NagiriAttack(float elapsedTime);  // 薙ぎ払い攻撃（４回）
    void T_V_NagiriEnergy(float elapsedTime);  // 薙ぎ払いエネルギー（3回）
    void T_V_Beam(float elapsedTime);   // 照射ビーム
    void T_V_SpecialAttack(float elapsedTime); // コンボ攻撃
    void T_V_Homing(float elapsedTime);        // ホーミング
    void T_V_AllRangeAttack(float elapsedTime); // 全体攻撃


    //--------------------<横シューティング>--------------------//

    void S_V_NagiriAttack(float elapsedTime); // 薙ぎ払い攻撃（４回）
    void S_V_NagiriEnergy(float elapsedTime); // 薙ぎ払いエネルギー（4回）
    void S_V_Beam(float elapsedTime); // 照射ビーム
    void S_V_SpecialAttack(float elapsedTime); // コンボ攻撃
    void S_V_CrossNagiriAttack(float elpasedTime); // 両サイドから薙ぎ払い発射
    void S_V_Horming(float elapsedTime); // ホーミング弾発射
};


//****************************************************************
// 
//  ボス用ポイントライト
// 
//****************************************************************
