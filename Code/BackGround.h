#pragma once

#include"ModelComponent.h"
#include"ShipMoveState.h"

#include"ConstantBuffer.h"


#include <memory>

#include"Texture.h"


// �w�i�̓V���N���X
class SkyDome
{
public:
    struct Data
    {
        DirectX::XMFLOAT4 ScrollSpeed; //UV�X�N���[���̑��x
    };


    SkyDome(ID3D11Device* d);
    ~SkyDome()=default;


    void Update(float elapsedTime, int View);
    void Render(ID3D11DeviceContext* dc);

private:
    std::unique_ptr<ModelComponent> model;

    Rotate r;

    DirectX::XMFLOAT4 orientation{ 0.0f,0.0f,0.0f,1.0f };  // ��]�s��
    DirectX::XMFLOAT3 RotateAxis{ 0.0f,-1.0001f,0.0f }; // ��]��
};


// �w�i�̑D
class BGShip
{
public:
    BGShip(ID3D11Device* d);
    ~BGShip() = default;


    void Update(float elapsedTime, int View);
    void Render(ID3D11DeviceContext* dc);
private:
    void GuiMenu();

    void MoveSideView(float elapsedTime);  // �����_�̓���
    void MoveUpView(float elapsedTime);    // �c���_�̓���

    [[nodiscard]] const auto& GetFrontVec() const; // ���f���̐��ʃx�N�g���擾�i���K���ρj


    using MoveBehavior = void(BGShip::*)(float); // �D�̓����J��
    MoveBehavior behaviorArray[2]{}; // �����̂͂����

    std::unique_ptr<ModelComponent> model;

    Transform t;
    Rotate r;

    float StackWaitTime = {}; // �ҋ@�o�ߎ���
    const float WaitTime = {20.0f}; // �ҋ@����

    int MoveState = 0;
    int CurrentView = 0; // ���݂̎��_
};


class BGColony
{
public:
    BGColony(ID3D11Device* d);
    ~BGColony();

    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);

private:
    //--------------------<�֐�>--------------------//
    void GuiMenu();

    //--------------------<�ϐ�>--------------------//
    std::unique_ptr<ModelComponent> Model;
    Transform t;
    Rotate r;

    Texture Tex_BaseColor{};
    Texture Tex_Metallic{};
    Texture Tex_Roughness{};
    Texture Tex_Normal{};

    
};

class BackGround
{
public:
    BackGround(ID3D11Device* d);
    ~BackGround() = default;


    void Initialize();
    void Update(float elapsedTime,int View);
    void Render(ID3D11DeviceContext* dc);
    void f_Gui();

public:
    std::unique_ptr<BGShip> ship;
    std::unique_ptr<SkyDome> dome;
    std::unique_ptr<BGColony> planet;

};

