#include "BaseBoss.h"
#include "EventFlag.h"
#include "Light.h"
class Boss_4 :public BaseBoss
{
protected:
    void Behavior(float elapsedTime) override;
    void Move(float elapsedTime) override;
    void Collision() override;
public:
    Boss_4(ID3D11Device* d);
    ~Boss_4() override;
    void Initialize() override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;
protected:
    void AttackManager(float elapsedTime) override;
    void PlayEffects(float elapsedTime);
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
    //--------------------<イベントフラグ>--------------------//
    EventFlagSystem mEventFlag;
    float Speed{ 15.0f }; // 移動速度
    float MaxHitPoint = 200.0f;
       //--------------------<ライトに関する変数>--------------------//
    float LightRange{}; // 影響度
    DirectX::XMFLOAT3 LightColor{}; // 色


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

private:
    using pBehavior = void(Boss_4::*)(float);

    std::vector<std::vector<pBehavior>> BehaviorVec[4]{}; // 行動遷移の配列

    //****************************************************************
    // 
    // 関数ポインタ
    // 
    //****************************************************************

    void T_CurveShot(float elapsedTime);
    void T_RotateAttack(float elapsedTime);
    void T_RotateAttack2(float elapsedTime);
    void T_Energy(float elapsedTime);
    void S_Energy(float elapsedTime);
    void T_DiffuseBullet(float elapsedTime);
    void S_Beam(float elapsedTime);
    void T_Horming(float elapsedTime);
    void S_RandSpeedAttack(float elapsedTime);
    void S_DiffuseShot(float elapsedTime);
    void S_CurveBullet(float elapsedTime);
    void S_CurveBullet2(float elapsedTime);
    void S_DiffuseShot2(float elapsedTime);
protected:
    void LastMotion(float elapsedTime);
    void ChangeView() override;
    //void ChangeViewPerformance(float) override;
    void FirstMotion(float elapsedTime) override;
};
