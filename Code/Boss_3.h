#pragma once


#include "BaseProjectile.h"
#include"Enemy.h"
#include"Light.h"
#include "EventFlag.h"

//****************************************************************
// 
//  ボス3
// 
//****************************************************************
class Boss_3 final :public Enemy
{
public:
    Boss_3(ID3D11Device* d);
    ~Boss_3() = default;

    void Initialize() override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;
protected:
    void Behavior(float elapsedTime) override;
    void Move(float elapsedTime) override;
    void Collision() override;

    void FirstMotion(float elapsedTime);

private:
    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************

    void ChangeView();
    void ChangeViewPerformance(float);

    void GuiMenu();
private:
    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************


    //--------------------<演出>--------------------//
    bool IsFirstMotion{}; //最初の動き
    float StackTimer{};  // タイマー
    bool IsPerformance{};

    //--------------------<攻撃>--------------------//
    bool CompleteAttack{};
    int CurrentAttackComboNumber{};
    int CurrentAttackNumber{};

    int AttackState = 0;
    float StackAttackInterval{};
    DirectX::XMFLOAT3 ShotRadian{};
    float Ratio{};
    bool shotRight{};

    DirectX::XMFLOAT3 target{};

    //--------------------<体力>--------------------//
    const float MaxHp = 200.0f;

    enum Mode
    {
        TOP,         // 縦
        SIDE,        // 横
        ANGRY_TOP,   // 発狂縦
        ANGRY_SIDE,  // 発狂横

    }CurrentMode{}; // 現在のボスの強さ


    //--------------------<ビームのモデル>--------------------//
    struct BeamEmitter
    {
        std::unique_ptr<ModelComponent> Model;
        Transform t;
        Rotate r;
    }mBeamEmitter[2]{};


    bool IsInputChangeView{};
    bool CompleteChangeView{};
    bool ShowBeamEmitter{}; // 可視化

    //--------------------<ライト>--------------------//
    std::unique_ptr<GamePointLight> mPointLight;

    //--------------------<ライトに関する変数>--------------------//
    float LightRange{}; // 影響度
    DirectX::XMFLOAT3 LightColor{}; // 色

    //--------------------<イベントフラグ>--------------------//
    EventFlagSystem laserFlag;

    //--------------------<移動に関する変数>--------------------//
    float Speed = 20.0f;

private:

    void  AttackManager(float elapsedTime);

    using pBehavior = void(Boss_3::*)(float);

    std::vector<std::vector<pBehavior>> BehaviorVec[4]{}; // 行動遷移の配列

    //****************************************************************
    // 
    // 関数ポインタ
    // 
    //****************************************************************

       // 初期位置に移動
    void T_MoveFirstPosition(float elapsedTime);
    void S_MoveFirstPosition(float elapsedTime);

    // 攻撃終了処理
    void EndAttack(float elapsedTime);


    // メモ：ボスのコンセプト
    // ボスと同じ属性のエネルギーを出してプレイヤーの属性を不利にする
    // そして敵を召喚して保身しながらプレイヤーを攻撃する
    // 全体攻撃の必殺技は青ー＞赤(積極的に青に変えてあげる)

    // 全部にかかわる攻撃
    void Shot_ShotRadian(BaseProjectile::Parent p, DirectX::XMFLOAT3 Radian, DirectX::XMFLOAT3 pos);
    void Shot_ShotRadianSlow(BaseProjectile::Parent p, DirectX::XMFLOAT3 Radian, DirectX::XMFLOAT3 pos);
    void Shot_ShotRadianFast(BaseProjectile::Parent p, DirectX::XMFLOAT3 Radian, DirectX::XMFLOAT3 pos);
    void Shot_EnergyRadian(const BaseProjectile::Parent p, const DirectX::XMFLOAT3 Radian, DirectX::XMFLOAT3 pos);
    void Shot_EnergyVector(const BaseProjectile::Parent p, const DirectX::XMFLOAT3 Vec, DirectX::XMFLOAT3 pos);
    void Shot_ShotVector(const BaseProjectile::Parent p, const DirectX::XMFLOAT3 Vec, DirectX::XMFLOAT3 pos);
    void Shot_ShotVectorFast(const BaseProjectile::Parent p, const DirectX::XMFLOAT3 Vec, DirectX::XMFLOAT3 pos);
    void Shot_ShotVectorSlow(const BaseProjectile::Parent p, const DirectX::XMFLOAT3 Vec, DirectX::XMFLOAT3 pos);

    void AppealTime(float elapsedTime);

    // Mode：TOPの関数
    void T_ShotEnergyToPlayer(float elapsedTime);
    void T_ShotBulletToPlayer(float elapsedTime);
    void T_MutualAttack(float elapsedTime);
    void S_WallBullet(float elapsedTime);
    void S_Beam(float elapsedTime);
    void T_SummonEnemies(float elasedTime);
    void T_SummonEnemiesGreat(float elasedTime);
    void T_RandomAllRangeAttack(float elapedTime);
    void T_Energy(float elapsedTime);
    void T_SlowShotBullet(float elapseTime);
    void T_SummonEnemiesMin(float elapsedTime);


    // Mode:SideFunction
    void S_FastShot(float elapsedTime);
    void S_Cross(float elapsedTime);
    void S_SlowShot(float elapsedTime);
    void S_SummonEnemyWall(float elapsedTime);
    void S_NagiriShot(float elapsdTime);
    void S_ShotEnergyToPlayer(float elapsedTime);
    void S_MoveShot(float elaspedTime);

};
