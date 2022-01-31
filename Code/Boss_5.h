#pragma once
#include "BaseBoss.h"
#include "EventFlag.h"
#include"Light.h"
class Boss_5 :public BaseBoss
{
protected:
    void Behavior(float elapsedTime) override;
    void Move(float elapsedTime) override;
    void Collision() override;
public:
    Boss_5(ID3D11Device* d);
    ~Boss_5() override;
    void Initialize() override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;
    void LastMotion(float elapsedTime);
protected:
    void AttackManager(float elapsedTime) override;
    void GuiMenu() override;
    void ChangeView(float elapsedTime) override;

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


    enum AnimationName {
        stand,
        change_camera,
        angry,
        beam_begin,
        beam_loop,
        beam_end,
        homing,
        overrall,
        get_rib_of_begein,
        get_rib_of_loop,
        get_rib_of_end,
        defeat,
        Aperemce,
        Aperemce2,

    };

    //--------------------<イベントフラグ>--------------------//
    EventFlagSystem laserFlag;
    EventFlagSystem mAnimationFlag;

    float Speed{ 15.0f }; // 移動速度

       //--------------------<ライトに関する変数>--------------------//
    float LightRange{}; // 影響度
    DirectX::XMFLOAT3 LightColor{}; // 色

    int MaxHitPoint{ 150 };

    using pBehavior = void(Boss_5::*)(float);

    std::vector<std::vector<pBehavior>> BehaviorVec[2]{}; // 行動遷移の配列



    //****************************************************************
    // 
    // 関数ポインタ
    // 
    //****************************************************************
    void T_Shot(float elapsedTime);
    void T_DiffuseBullet(float elapsedTime);
    void T_V_Homing(float elapsedTime);
    void S_Shot(float elapsedTime);
    void T_Energy(float elapsedTime);
    void S_Energy(float elapsedTime);
    void S_DiffuseShot(float elapsedTime);
    void S_ChaicePlayerAttack(float elapsedTime);
    void FirstMotion(float elapsedTime) override;
    void PlayEffects(float elapsedTime) override;
};
