#pragma once

#include <vector>
#include <memory>
#include "Enemy.h"
#include "Timer.h"
#include"Renderer.h"

#include "gAudioSource.h"

// EnemyManager     ※シングルトン

// ------------------------ 説明 ------------------------ //
// SceneGame内で動作しているPlayer,Projectile,Enemyで使用されている
// 例外でSceneTitleのInitialize()でResetSpawnTimer()が呼ばれている
// ----------------------------------------------------- //




class EnemyManager
{
public:    // 生成設定用
    enum class STAGE
    {
        STAGE1,
        STAGE2,
        STAGE3,
        STAGE4,
        STAGE5,

        //EXTRA1,
        //EXTRA2,
        //EXTRA3,
    };

public:   // 敵の生成関係
    enum class ENEMY_KIND
    {
        Straight_None,                          // 直進するだけ
        Straight_None_Start_Back,               // 直進のみ           (出現イベント後ろから)
        Straight_None_Start_Front,              // 直進のみ           (出現イベント前から)
        Straight_None_Start_Side,               // 直進のみ           (出現イベント横から)
        Straight_None_Bezier_Top,               // カーブのみ            (縦シューのみ対応)
        Straight_None_Bezier_Side,              // カーブのみ            (横シューのみ対応)
        Straight_None_Horizon_Top,              // 横直進のみ

        Straight_Shot_Start_Back,               // 直進射撃           (出現イベント後ろから)
        Straight_Shot_Start_Front,              // 直進射撃           (出現イベント前から)
        Straight_Shot_Start_Side,               // 直進射撃           (出現イベント横から)
        Straight_Shot_Bezier_Top,               // カーブ射撃            (縦シューのみ対応)
        Straight_Shot_Bezier_Side,              // カーブ射撃            (横シューのみ対応)

        Straight_TargetShot_Start_Back,         // 直進目標射撃        (出現イベント後ろから)
        Straight_TargetShot_Start_Front,        // 直進目標射撃        (出現イベント前から)
        Straight_TargetShot_Start_Side,         // 直進目標射撃        (出現イベント横から)
        Straight_TargetShot_Bezier_Top,         // カーブ目標射撃         (縦シューのみ対応)
        Straight_TargetShot_Bezier_Side,        // カーブ目標射撃         (横シューのみ対応)

        
        
        //Wave_None,                              // 波上に動くだけ
        //Wave_Shot_B,                            // 波上に動きながら撃つ (弾)
        //Wave_Shot_E,                            // 波上に動きながら撃つ (エネルギー)
        //Lock_Shot_B,                            // 固定砲台           (弾)
        //Lock_Shot_E,                            // 固定砲台           (エネルギー)
        Lock_MultiShot_B,                       // 拡散射撃する       （弾）　　　　　　　　             クジラ型の敵予定
        Lock_MultiShot_E,                       // 拡散射撃する       （エネルギー）　　　　             クジラ型の敵予定
        Lock_TargetMultiShot_B,                 // 拡散目標射撃する     (弾)
        Lock_TargetMultiShot_B_Hard,            // 拡散目標射撃する     (弾)
        Lock_TargetSingleShot,                  // 射撃する           （弾）
        Lock_TargetSingleShot_Hard,             // 射撃する           （弾）
        Lock_Straight_Target,                   // 狙いをつけてから目標に突撃する                       ウツボ型の敵予定
        Lock_Straight_Target_Hard,              // 狙いをつけてから目標に突撃する                       ウツボ型の敵予定
        Translate_Shot_B_Top,                   // x方向に射撃する    （弾）(縦シューのみ対応)
        Translate_Shot_B_Top_Hard,              // x方向に射撃する    （弾）(縦シューのみ対応)
        Translate_Shot_B_Side,                  // y方向に射撃する    （弾）(横シューのみ対応)
        Translate_Shot_B_Side_Hard,             // y方向に射撃する    （弾）(横シューのみ対応)
        Rotate_AroundShot_E,                    // 回転しながら、全方向にエネルギーを撃つ                 ヒトデ型の予定
        Move_Round_TargetShot_Side,             // 周遊しながら目標に射撃する（横シューのみ対応）
        Move_Round_TargetShot_Side_Hard,        // 周遊しながら目標に射撃する（横シューのみ対応）
        //Back_Front_Straight_None,               // 後ろから来て前方でターンした後、前進し続ける

        Boss1,                                   // ボス1
        Boss2,                                   // ボス2
        Boss3,                                   // ボス3
        Boss4,                                   // ボス4
        Boss5,                                   // ボス5
        Boss_Ex1,                                // エクストラボス1
        Boss_Ex2,                                // エクストラボス2
        Boss_Ex3,                                // エクストラボス3

    };
private:
    // 敵の生成用データ
    struct SpawnData final
    {
        float spawn_time;                                       // 生成時間
        std::vector<ENEMY_KIND> kinds = {};                     // 敵の種類
        std::vector<uint8_t> sizes = {};                        // 敵の量
        std::vector<std::vector<float>> pos_widthss = {};       // 敵の生成位置 float値 (横画面なら縦幅、　縦画面なら横幅) (z軸の位置が y = 0.0f)
        std::vector<std::vector<BaseObject::Type>> typess = {}; // 敵の属性
        std::vector<std::vector<float>> z_shiftss = {};         // 重ならない用の z値 のずらす量
    };


private:
    EnemyManager()
    {
        spawn_timer.Initialize(nullptr, COUNT::UP, 0.0f);
    }
    ~EnemyManager()
    {

    }
public:
    static EnemyManager& Instance()
    {
        static EnemyManager instance;
        return instance;
    }
    //--------< 関数 >--------//
    void Initialize(ID3D11Device* d, STAGE stage = STAGE::STAGE1);
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);
    void Finalize();
    void UpdateBossAppear(float elapsedTime);
    void Render2D(ID3D11DeviceContext* dc);

    // エネミー数取得
    [[nodiscard]] int GetEnemyCount() const { return static_cast<int>(enemies.size()); }
    // エネミー取得
    [[nodiscard]] Enemy& GetEnemy(int index) const { return *enemies.at(index); }
    // エネミー登録
    void RegisterEnemy(Enemy* enemy);
    // エネミー削除
    void Clear();
    // エネミー削除
    void Remove(Enemy* enemy);
    //エネミー死亡
    void Death(Enemy* enemy);
	// 目標地点の登録
    void RegisterTargetPosition(const DirectX::XMFLOAT3* target_);

    const auto& GetCameraFocusBoss() const { return CameraFocusBoss; }
    void SetCameraFocusBoss(const bool t) { CameraFocusBoss = t; }

    const auto& GetTargetPosition() const { return targetPosition; }
    void SetTargetPosition(DirectX::XMFLOAT3 p) { targetPosition = p; }

    //--------------------<ボスの登場フラグ>--------------------//
    void SetStartBossAppear(bool a)
    {
        IsStartBossAppear = a;
        BossAppearState = 0;
    }
    bool GetEndBossAppear()const { return IsEndBossAppear; }

    // ボス戦まで行った後タイトルに戻っても、最初からにならないので
    // タイトルのInitialize()でこの関数を呼び出す。
    void ResetSpawnTimer() { spawn_timer.Reset(); }

    void SetGameEnd(bool a) { GameEnd = a; }
    bool GetGameEnd()const  { return GameEnd; }

    void SetIsLastPerformance(bool a) { IsLastPerformance = a; }
    bool GetIsLastPerformance()const { return IsLastPerformance; }

    // ステージの変更は、このSetStageIndex()を使用している、
    // TitleManagerのsStageSelect()でしか変更できない
    void SetStageIndex(STAGE stage_) { stage = stage_; }
    STAGE GetStageIndex() { return stage; }

    // プレイヤーの変形(ゲージ使用)の可否 ( 変形可　= return true, 変形不可 = return false )
    bool IsPlayerTransformable() { return !is_player_not_transfrom_time; /* この変数は変形ができないときにtrueになるので,  ! 演算子でbool値を反対にした */ }

    void SetLastPerformanceState(int a) { LastPerformanceState = a; }
    int GetLastPerformanceState()const { return LastPerformanceState; }

    // ボス戦のBGMを流すかどうか
    bool GetStartBossBGM();
    void SetStartBossBGM(bool a) { StartBossBGM = true; }

    // ボス戦中かどうか

    bool GetIsBossBattle() { return is_boss_battle; }

    [[nodiscard]] float fGetZoomPower() const { return ZoomPower; }
    void fSetZoomPower(float ZoomPower_) { ZoomPower = ZoomPower_; }

    //****************************************************************
    // 
    //  チュートリアル
    // 
    //****************************************************************

    void TutorialSpownEnemy(int CurrentTutorialState); // チュートリアル用の敵の生成
    void TutorialUpdate(float elapsedTime);
    void TutorialSetIsAliveShotTutorialEnemy(bool a) { IsAliveShotTutorialEnemy = a; }
    auto GetIsAliveShotTutorialEnemy() const { return IsAliveShotTutorialEnemy; }

    void TutorialSetIsAliveAttackTutorialEnemy1(bool a) { IsAliveAttackTutorialEnemy1 = a; }
    auto GetIsAliveAttackTutorialEnemy1() const { return IsAliveAttackTutorialEnemy1; }
    void TutorialSetIsAliveAttackTutorialEnemy2(bool a) { IsAliveAttackTutorialEnemy2 = a; }
    auto GetIsAliveAttackTutorialEnemy2() const { return IsAliveAttackTutorialEnemy2; }
    void Tutorial_SetIsAliveElementTutorialEnemy1(bool a) { IsAliveElementTutorialEnemy1 = a; }
    void Tutorial_SetIsAliveElementTutorialEnemy2(bool a) { IsAliveElementTutorialEnemy2 = a; }
    void ResetIsClearEnemies() { IsClearEnemies = false; }
private:
    // モデル領域確保用のダミー生成
    void MakeDummyEnemy();

    // 敵の生成処理
    void EnemyAppear(float elapsedTime);

    // 敵の設定
    void SetSpawnData(STAGE stage);
    // 生成用データの設定
    void SetSpawnDatas_Stage1();
    void SetSpawnDatas_Stage2();
    void SetSpawnDatas_Stage3();
    void SetSpawnDatas_Stage4();
    void SetSpawnDatas_Stage5();
    void SetSpawnDatas_Extra1(){};
    //void SetSpawnDatas_Extra2();
    //void SetSpawnDatas_Extra3();

public:
    // 敵の生成関数
    Enemy* SpawnEnemy(const ENEMY_KIND kind, BaseObject::Type enemy_type);
private:
    // 生成済みのデータの削除処理
    void EraseSpawnData();

private:
    // カメラ方向切り替え (一回だけ実行 timerが47.0f ~ 49.0fのときだけ)
    void ChangeCameraOnce();

    // ボス前の変形禁止
    void BossJustThen();

    //--------------------<デバッグ用>--------------------//
    void GuiMenu();

private:
    static constexpr float easy_boss_time   = 86.0f;
    static constexpr float normal_boss_time = 125.0f;


private:
    //--------< 変数 >--------//
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Enemy>> removes;

    //ダミーエネミー
    std::vector<std::unique_ptr<Enemy>> dummies;

    // 現在のステージ状態
    STAGE stage = STAGE::STAGE1;
    

    // 敵出現用のフラグタイマー
    Timer spawn_timer;

    // 敵の出現データ
    std::vector<SpawnData> spawn_datas;

    // 目標地点
    const DirectX::XMFLOAT3* target;

    // カメラ注視設定
    bool CameraFocusBoss{ false };
    DirectX::XMFLOAT3 targetPosition{};

	//enemy破壊sound
    std::unique_ptr<AudioSource> se_Bomb;


    // プレイヤーが変形(ゲージを使用)できるかのフラグ     ※ trueだったらできない
    //
    //  カメラの遷移(side~top)中にできないようにしたかった
    //
    bool is_player_not_transfrom_time = false;


    //--------------------<ボスの登場>--------------------//
    int BossAppearState{};    // ボスの登場エフェクト
    Renderer sprWarning{};
    bool IsStartBossAppear{ false };
    bool IsEndBossAppear{ false };

    bool StartBossBGM{ false };  // ボスのBGM再生開始
    bool PlayingBossBGM{ false }; // ボスのBGM再生中かどうか
    bool GameEnd{};

    bool IsLastPerformance{};
    int LastPerformanceState{}; // 最後の演出

    bool is_boss_battle = false; // ボス戦中か
    float ZoomPower{ 0.0f };
    //****************************************************************
    // 
    // チュートリアル用の変数
    // 
    //****************************************************************

    bool IsAliveShotTutorialEnemy;
    bool IsAliveAttackTutorialEnemy1;
    bool IsAliveAttackTutorialEnemy2;
    bool IsAliveElementTutorialEnemy1;
    bool IsAliveElementTutorialEnemy2;
    bool IsClearEnemies; // 敵を一度全部クリア
};