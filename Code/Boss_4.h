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

    

    //--------------------<�r�[���̃��f��>--------------------//
    struct BeamEmitter
    {
        std::unique_ptr<ModelComponent> Model;
        Transform t;
        Rotate r;
    }mBeamEmitter[2]{};



    bool ShowBeamEmitter{}; // ����

    //--------------------<���C�g>--------------------//
    std::unique_ptr<GamePointLight> mPointLight;

    //--------------------<�C�x���g�t���O>--------------------//
    EventFlagSystem laserFlag;
    //--------------------<�C�x���g�t���O>--------------------//
    EventFlagSystem mEventFlag;
    float Speed{ 15.0f }; // �ړ����x
    float MaxHitPoint = 200.0f;
       //--------------------<���C�g�Ɋւ���ϐ�>--------------------//
    float LightRange{}; // �e���x
    DirectX::XMFLOAT3 LightColor{}; // �F


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

    std::vector<std::vector<pBehavior>> BehaviorVec[4]{}; // �s���J�ڂ̔z��

    //****************************************************************
    // 
    // �֐��|�C���^
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
