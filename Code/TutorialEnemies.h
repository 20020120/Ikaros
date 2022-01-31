#pragma once
#include"Enemy.h"
#include<vector>

//****************************************************************
// 
//  チュートリアルの敵の動きを管理するクラス
// 
//****************************************************************
class EnemyBehaviorManager final
{
public:
    EnemyBehaviorManager() = default;
    ~EnemyBehaviorManager() = default;

    void AddBehavior(std::function<void(float)> pFunction);  // 振る舞いを追加
    void Step(); // 次のふるまいに遷移する
    void Update(float elapsedTime); // 更新処理

    auto GetCurrentBehavior() const { return CurrentBehavior; } // 現在何番目の振るまいかかえす

private:


    //  振る舞いを管理するベクター
    std::vector<std::function<void(float)>> BehaviorVec;
    // 現在の振る舞い
    int CurrentBehavior{ 0 };

};






//****************************************************************
// 
//  チュートリアルの敵たち
// 
//****************************************************************

//--------------------<攻撃のチュートリアルで出てくる敵>--------------------//
class AttackTutorialEnemy final:public Enemy
{
public:
    AttackTutorialEnemy(ID3D11Device* d, const Type& type,DirectX::XMFLOAT3 WaitPosition,int Index_);
    ~AttackTutorialEnemy() override;

    void Initialize() override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;
protected:
    void Behavior(float elapsedTime) override;
    void Move(float elapsedTime) override;

private:

    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************

    DirectX::XMFLOAT3 WaitPosition;    // 待機位置
    DirectX::XMFLOAT3 InitialPosition; // 初期位置
    float MoveRatio{}; // 移動の補完率
    int Index{}; // 自分の番号

     //  振る舞いを管理するベクター
    using pFunc=  void(AttackTutorialEnemy::*)(float);
    std::vector<pFunc> BehaviorVec;
    // 現在の振る舞い
    int CurrentBehavior{ 0 };

    //****************************************************************
    // 
    // ふるまいを決定する関数
    // 
    //****************************************************************
    void Entry(float elaspedTime); // 初期位置から決められた場所まで移動する
    void Wait(float elapsedTime);  // 待機
protected:
    void Collision() override;
};


//--------------------<回避のチュートリアルの敵>--------------------//

class DodgeTutorialEnemy final:public Enemy
{
public:
    DodgeTutorialEnemy(ID3D11Device* d, const Type& type, DirectX::XMFLOAT3 WaitPosition);
    ~DodgeTutorialEnemy()override;

    
    void Initialize() override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;

protected:
    //****************************************************************
    // 
    //  関数
    // 
    //****************************************************************

    void Behavior(float elapsedTime) override;
    void Move(float elapsedTime) override;


private:
    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************

    DirectX::XMFLOAT3 WaitPosition;    // 待機位置
    DirectX::XMFLOAT3 InitialPosition; // 初期位置
    float MoveRatio{}; // 移動の補完率
    int Index{}; // 自分の番号

     //  振る舞いを管理するベクター
    using pFunc = void(DodgeTutorialEnemy::*)(float);
    std::vector<pFunc> BehaviorVec;
    // 現在の振る舞い
    int CurrentBehavior{ 0 };

    float StackShotInterval{};            // 発射間隔を更新
    const float ShotInterval = { 0.8f };  // （秒）

    //****************************************************************
    // 
    //  ふるまいの関数
    // 
    //****************************************************************

    void Entry(float elapsedTime);  // 決められた場所まで移動する
    void Shot(float elapsedTime);   // 一定間隔で射撃する
protected:
    void Collision() override{};
};

//****************************************************************
// 
//  エネルギーのチュートリアル
// 
//****************************************************************

class EnergyTutorialEnemy final :public Enemy
{
public:
    EnergyTutorialEnemy(ID3D11Device* d, const Type& type, DirectX::XMFLOAT3 WaitPosition);
    ~EnergyTutorialEnemy();


    void Initialize() override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;
protected:
    void Behavior(float elapsedTime) override;
    void Move(float elapsedTime) override;
    void Collision() override {};
private:
    //****************************************************************
   // 
   // 変数
   // 
   //****************************************************************

    DirectX::XMFLOAT3 WaitPosition;    // 待機位置
    DirectX::XMFLOAT3 InitialPosition; // 初期位置
    float MoveRatio{}; // 移動の補完率
    int Index{}; // 自分の番号

    //  振る舞いを管理するベクター
    using pFunc = void(EnergyTutorialEnemy::*)(float);
    std::vector<pFunc> BehaviorVec;
    // 現在の振る舞い
    int CurrentBehavior{ 0 };

    float StackShotInterval{};            // 発射間隔を更新
    const float ShotInterval = { 0.8f };  // （秒）

    //****************************************************************
    // 
    //  ふるまいの関数
    // 
    //****************************************************************

    void Entry(float elapsedTime);  // 決められた場所まで移動する
    void Shot(float elapsedTime);   // 一定間隔で射撃する
};


class EnergyTutorialEnemy2 final :public Enemy
{
public:
    EnergyTutorialEnemy2(ID3D11Device* d, const Type& type, DirectX::XMFLOAT3 WaitPosition);
    ~EnergyTutorialEnemy2();


    void Initialize() override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;
protected:
    void Behavior(float elapsedTime) override;
    void Move(float elapsedTime) override;
    void Collision() override {};
private:
    //****************************************************************
   // 
   // 変数
   // 
   //****************************************************************

    DirectX::XMFLOAT3 WaitPosition;    // 待機位置
    DirectX::XMFLOAT3 InitialPosition; // 初期位置
    float MoveRatio{}; // 移動の補完率
    int Index{}; // 自分の番号

    //  振る舞いを管理するベクター
    using pFunc = void(EnergyTutorialEnemy2::*)(float);
    std::vector<pFunc> BehaviorVec;
    // 現在の振る舞い
    int CurrentBehavior{ 0 };

    float StackShotInterval{};            // 発射間隔を更新
    const float ShotInterval = { 0.8f };  // （秒）

    //****************************************************************
    // 
    //  ふるまいの関数
    // 
    //****************************************************************

    void Entry(float elapsedTime);  // 決められた場所まで移動する
    void Shot(float elapsedTime);   // 一定間隔で射撃する
};