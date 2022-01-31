#pragma once

#include"BaseBoss.h"
#include"Light.h"
#include"EventFlag.h"
class Boss_7 final :public BaseBoss
{
protected:
    void Behavior(float elapsedTime) override;
    void Move(float elapsedTime) override;
    void Collision() override;
public:
    Boss_7(ID3D11Device* d);
    ~Boss_7() override;
    void Initialize() override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;
protected:
    void ChangeView(float elapsedTime) override;
    void AttackManager(float elapsedTime) override;
    void GuiMenu() override;
    void T_DiffuseShot(float elapsedTime);
    void T_Homing(float elapsedTime);
    void T_ShotToPlayer(float elapsedTime);
    void T_Energy(float elapsedTime);
    void T_Energy2(float elapsedTime);
    void FirstMotion(float elapsedTime) override;
    void LastMotion(float elapsedTime);
    void PlayEffects(float elapsedTime) override;


    //--------------------<���C�g>--------------------//
    std::unique_ptr<GamePointLight> mPointLight;

    //--------------------<�C�x���g�t���O>--------------------//
    EventFlagSystem laserFlag;
    EventFlagSystem mAnimationFlag;

    float Speed{ 15.0f }; // �ړ����x

       //--------------------<���C�g�Ɋւ���ϐ�>--------------------//
    float LightRange{}; // �e���x
    DirectX::XMFLOAT3 LightColor{}; // �F

    int MaxHitPoint{ 75 };

    using pBehavior = void(Boss_7::*)(float);

    std::vector<std::vector<pBehavior>> BehaviorVec{}; // �s���J�ڂ̔z��

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
        ApereRance,
        ApereRance2,
    };


  


};
