#pragma once


#include "BaseProjectile.h"
#include"Enemy.h"
#include"Light.h"
#include "EventFlag.h"
#include"BaseBoss.h"
//****************************************************************
// 
//  ボス２
// 
//****************************************************************
class Boss_2 final :public BaseBoss
{
public:
    Boss_2(ID3D11Device* d);
    ~Boss_2() = default;

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
    void GuiMenu();
 
private:
    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************


  
    

    //--------------------<ビームのモデル>--------------------//
    struct BeamEmitter
    {
        std::unique_ptr<ModelComponent> Model;
        Transform t;
        Rotate r;
    }mBeamEmitter[2]{};


   
    bool ShowBeamEmitter{}; // 可視化

    //--------------------<ライト>--------------------//
    std::unique_ptr<GamePointLight> mPointLight;

    //--------------------<イベントフラグ>--------------------//
    EventFlagSystem laserFlag;

    float Speed{ 15.0f }; // 移動速度

       //--------------------<ライトに関する変数>--------------------//
    float LightRange{}; // 影響度
    DirectX::XMFLOAT3 LightColor{}; // 色

private:
    using pBehavior = void(Boss_2::*)(float);

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



    //--------------------<topの攻撃>--------------------//
    void T_Beam(float elapsedTime);
    void T_Beam2(float elapsedTime);
    void T_MoveBeam(float elapsedTime);
    void T_Nagiri(float elapsedTime);
    void T_AllRangeAttack(float elapsedTime);
    void T_Horming(float elapsedTime);
    void S_Horming(float elapsedTime);


    //--------------------<Sideの攻撃>--------------------//
    void S_CrossAttack0(float elapsedTime);
    void S_CrossAttack1(float elapsedTime);
    void S_RandamAttack(float elapsedTime);
    void S_Beam(float elapsedTime);
    void T_RandamSpeedAttack(float elapsedTime);
    void S_NagiriSlow(float elasedTime);
    void S_NagiriSlow2(float elapsedTime);
    void S_NagiriFast(float elasedTime);
    void S_NagiriFast2(float elapsedTime);
protected:
    void AttackManager(float elapsedTime) override;
};
