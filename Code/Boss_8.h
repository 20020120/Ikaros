#pragma once

#include"BaseBoss.h"
#include"Light.h"
#include"EventFlag.h"
class Boss_8 final :public BaseBoss
{
protected:
    void Behavior(float elapsedTime) override;
    void Move(float elapsedTime) override;
    void Collision() override;
public:
    Boss_8(ID3D11Device* d);
    ~Boss_8() override;
    void LastMotion(float elapsedTime);
    void Initialize() override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;
protected:
    void ChangeView() override;
    void AttackManager(float elapsedTime) override;
    void GuiMenu() override;
    void T_HighTempoShot(float elapsedTime);
    void T_Horming(float elapsedTime);
    void T_TargetShot(float elapsedTime);
    void T_MixEnergy(float elapsedTime);
    void T_StepAllRangeAttack(float elapsedTime);
    void T_Beam(float elapsedTime);
    void T_DiffuseShot(float elapsedTime);
    void S_DiffuseShot(float elapsedTime);
    void S_Energy(float elapsedTime);
    void S_TargetShot(float elapsedTime);
    void S_V_CrossNagiriAttack(float elapsedTime);
    void S_Beam(float elapsedTime);
    void S_Horming(float elapsedTime);
    void FirstMotion(float elapsedTime) override;
    void ChangeViewPerformance(float) override;
    void PlayEffects(float elapsedTime) override;


    enum  AnimationName {
        appear,
        angle_change,
        angry,
        beam_charge,
        bean_ready,
        beam_shoot,
        beam_end,
        allrange_attackhoming_attack,
        allrange_attack,
        nagiri_attack,
        concentrated_attack,
        die,
        dead,
        idle,
    };

    //--------------------<ライト>--------------------//
    std::unique_ptr<GamePointLight> mPointLight;

    //--------------------<イベントフラグ>--------------------//
    EventFlagSystem mEventFlag;

    float Speed{ 15.0f }; // 移動速度

       //--------------------<ライトに関する変数>--------------------//
    float LightRange{}; // 影響度
    DirectX::XMFLOAT3 LightColor{}; // 色

    int MaxHitPoint{ 150 };

    using pBehavior = void(Boss_8::*)(float);

    std::vector<std::vector<pBehavior>> BehaviorVec[3]{}; // 行動遷移の配列




};
