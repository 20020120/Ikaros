#pragma once
#include "BaseBoss.h"
#include "EventFlag.h"
#include"Light.h"
class Boss_6 final :public BaseBoss
{
protected:
    void Behavior(float elapsedTime) override;
    void Move(float elapsedTime) override;
    void Collision() override;
public:
     Boss_6(ID3D11Device* d);
    ~Boss_6() override;
    void Initialize() override;
    void PlayEffects(float elapsedTime);
    void LastMotion(float elapsedTime);
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;
protected:
    void ChangeView() override;
    void AttackManager(float elapsedTime) override;
    void GuiMenu() override;


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
    EventFlagSystem mAnimationFlag;

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
    };

    float Speed{ 15.0f }; // 移動速度

       //--------------------<ライトに関する変数>--------------------//
    float LightRange{}; // 影響度
    DirectX::XMFLOAT3 LightColor{}; // 色

    float MaxHitPoint = 200.0f;

private:
    using pBehavior = void(Boss_6::*)(float);

    std::vector<std::vector<pBehavior>> BehaviorVec[4]{}; // 行動遷移の配列


    //****************************************************************
    // 
    // 関数ポインタ
    // 
    //****************************************************************


    void T_Beam(float elapsedTime); // ビーム
    void T_StepAllRangeAttack(float elapsedTime);
    void T_Energy(float elapsedTime);
    void T_Horming(float elapsedTime);
    void T_StepToPlayerShot(float elapsedTime);

    void S_Horming(float elaspedTime);
    void S_Boonmerang(float elapsedTime);
    void S_Energy(float elapsedTime);
    void S_SummonEnemy(float elapsedTime);
    void S_DiffuseShot(float elapsedTime);
protected:
    void FirstMotion(float elapsedTime) override;
};
