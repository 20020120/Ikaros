#pragma once


#include "BaseProjectile.h"
#include"Enemy.h"
#include"Light.h"
#include "EventFlag.h"
#include"BaseBoss.h"
//****************************************************************
// 
//  �{�X�Q
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
    // �֐�
    // 
    //****************************************************************
    void GuiMenu();
 
private:
    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************


  
    

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

    float Speed{ 15.0f }; // �ړ����x

       //--------------------<���C�g�Ɋւ���ϐ�>--------------------//
    float LightRange{}; // �e���x
    DirectX::XMFLOAT3 LightColor{}; // �F

private:
    using pBehavior = void(Boss_2::*)(float);

    std::vector<std::vector<pBehavior>> BehaviorVec[4]{}; // �s���J�ڂ̔z��

    //****************************************************************
    // 
    // �֐��|�C���^
    // 
    //****************************************************************

    // �����ʒu�Ɉړ�
    void T_MoveFirstPosition(float elapsedTime);
    void S_MoveFirstPosition(float elapsedTime);
    
    // �U���I������
    void EndAttack(float elapsedTime);



    //--------------------<top�̍U��>--------------------//
    void T_Beam(float elapsedTime);
    void T_Beam2(float elapsedTime);
    void T_MoveBeam(float elapsedTime);
    void T_Nagiri(float elapsedTime);
    void T_AllRangeAttack(float elapsedTime);
    void T_Horming(float elapsedTime);
    void S_Horming(float elapsedTime);


    //--------------------<Side�̍U��>--------------------//
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
