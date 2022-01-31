#pragma once
#include"Enemy.h"
#include<vector>

//****************************************************************
// 
//  �`���[�g���A���̓G�̓������Ǘ�����N���X
// 
//****************************************************************
class EnemyBehaviorManager final
{
public:
    EnemyBehaviorManager() = default;
    ~EnemyBehaviorManager() = default;

    void AddBehavior(std::function<void(float)> pFunction);  // �U�镑����ǉ�
    void Step(); // ���̂ӂ�܂��ɑJ�ڂ���
    void Update(float elapsedTime); // �X�V����

    auto GetCurrentBehavior() const { return CurrentBehavior; } // ���݉��Ԗڂ̐U��܂���������

private:


    //  �U�镑�����Ǘ�����x�N�^�[
    std::vector<std::function<void(float)>> BehaviorVec;
    // ���݂̐U�镑��
    int CurrentBehavior{ 0 };

};






//****************************************************************
// 
//  �`���[�g���A���̓G����
// 
//****************************************************************

//--------------------<�U���̃`���[�g���A���ŏo�Ă���G>--------------------//
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
    // �ϐ�
    // 
    //****************************************************************

    DirectX::XMFLOAT3 WaitPosition;    // �ҋ@�ʒu
    DirectX::XMFLOAT3 InitialPosition; // �����ʒu
    float MoveRatio{}; // �ړ��̕⊮��
    int Index{}; // �����̔ԍ�

     //  �U�镑�����Ǘ�����x�N�^�[
    using pFunc=  void(AttackTutorialEnemy::*)(float);
    std::vector<pFunc> BehaviorVec;
    // ���݂̐U�镑��
    int CurrentBehavior{ 0 };

    //****************************************************************
    // 
    // �ӂ�܂������肷��֐�
    // 
    //****************************************************************
    void Entry(float elaspedTime); // �����ʒu���猈�߂�ꂽ�ꏊ�܂ňړ�����
    void Wait(float elapsedTime);  // �ҋ@
protected:
    void Collision() override;
};


//--------------------<����̃`���[�g���A���̓G>--------------------//

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
    //  �֐�
    // 
    //****************************************************************

    void Behavior(float elapsedTime) override;
    void Move(float elapsedTime) override;


private:
    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************

    DirectX::XMFLOAT3 WaitPosition;    // �ҋ@�ʒu
    DirectX::XMFLOAT3 InitialPosition; // �����ʒu
    float MoveRatio{}; // �ړ��̕⊮��
    int Index{}; // �����̔ԍ�

     //  �U�镑�����Ǘ�����x�N�^�[
    using pFunc = void(DodgeTutorialEnemy::*)(float);
    std::vector<pFunc> BehaviorVec;
    // ���݂̐U�镑��
    int CurrentBehavior{ 0 };

    float StackShotInterval{};            // ���ˊԊu���X�V
    const float ShotInterval = { 0.8f };  // �i�b�j

    //****************************************************************
    // 
    //  �ӂ�܂��̊֐�
    // 
    //****************************************************************

    void Entry(float elapsedTime);  // ���߂�ꂽ�ꏊ�܂ňړ�����
    void Shot(float elapsedTime);   // ���Ԋu�Ŏˌ�����
protected:
    void Collision() override{};
};

//****************************************************************
// 
//  �G�l���M�[�̃`���[�g���A��
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
   // �ϐ�
   // 
   //****************************************************************

    DirectX::XMFLOAT3 WaitPosition;    // �ҋ@�ʒu
    DirectX::XMFLOAT3 InitialPosition; // �����ʒu
    float MoveRatio{}; // �ړ��̕⊮��
    int Index{}; // �����̔ԍ�

    //  �U�镑�����Ǘ�����x�N�^�[
    using pFunc = void(EnergyTutorialEnemy::*)(float);
    std::vector<pFunc> BehaviorVec;
    // ���݂̐U�镑��
    int CurrentBehavior{ 0 };

    float StackShotInterval{};            // ���ˊԊu���X�V
    const float ShotInterval = { 0.8f };  // �i�b�j

    //****************************************************************
    // 
    //  �ӂ�܂��̊֐�
    // 
    //****************************************************************

    void Entry(float elapsedTime);  // ���߂�ꂽ�ꏊ�܂ňړ�����
    void Shot(float elapsedTime);   // ���Ԋu�Ŏˌ�����
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
   // �ϐ�
   // 
   //****************************************************************

    DirectX::XMFLOAT3 WaitPosition;    // �ҋ@�ʒu
    DirectX::XMFLOAT3 InitialPosition; // �����ʒu
    float MoveRatio{}; // �ړ��̕⊮��
    int Index{}; // �����̔ԍ�

    //  �U�镑�����Ǘ�����x�N�^�[
    using pFunc = void(EnergyTutorialEnemy2::*)(float);
    std::vector<pFunc> BehaviorVec;
    // ���݂̐U�镑��
    int CurrentBehavior{ 0 };

    float StackShotInterval{};            // ���ˊԊu���X�V
    const float ShotInterval = { 0.8f };  // �i�b�j

    //****************************************************************
    // 
    //  �ӂ�܂��̊֐�
    // 
    //****************************************************************

    void Entry(float elapsedTime);  // ���߂�ꂽ�ꏊ�܂ňړ�����
    void Shot(float elapsedTime);   // ���Ԋu�Ŏˌ�����
};