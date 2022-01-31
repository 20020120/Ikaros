#pragma once

#include"ModelComponent.h"
#include"Leaf.h"

#include<memory>

class BaseObject
{
public:
    BaseObject() = default;
    virtual ~BaseObject() = default;

    virtual void Initialize(){}
    virtual void Update(float elapsedTime) = 0;
    virtual void Render(ID3D11DeviceContext* dc) = 0;
    virtual void Finalize(){}

    void LoadModel(ID3D11Device* d, ModelRenderInitializer Ini)
    {
        Model = std::make_unique<ModelComponent>(d, Ini);
    }

    //--------------------<getter,setter>--------------------//
    [[nodiscard]] const DirectX::XMFLOAT3& GetPosition() const { return t.Position; }
    [[nodiscard]] const DirectX::XMFLOAT3& GetScele() const { return t.Scale; }
    [[nodiscard]] DirectX::XMFLOAT3 GetHitAreaSize() const { return hitAreaSize; }
    [[nodiscard]] float GetInvTimer() const { return invTimer; }
    [[nodiscard]] float GetMaxInvTimer() const { return maxInvTimer; }

    [[nodiscard]] const auto GetFrontVec() const
    // ���f���̐��ʃx�N�g���擾�i���K���ρj
    {
        const DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z);
        DirectX::XMFLOAT3 front{};
        DirectX::XMVector3Normalize(transform.r[2]);
        DirectX::XMStoreFloat3(&front, transform.r[2]);
        return front;
    }

    [[nodiscard]] const auto GetUpVec() const
        // ���f���̐��ʃx�N�g���擾�i���K���ρj
    {
        const DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z);
        DirectX::XMFLOAT3 front{};
        DirectX::XMVector3Normalize(transform.r[1]);
        DirectX::XMStoreFloat3(&front, transform.r[1]);
        return front;
    }

    

    DirectX::XMFLOAT3 hitAreaSize{};
    float invTimer{ 1.0f };//���G����
    float maxInvTimer{ 1.0f };//�ő喳�G����

    std::unique_ptr<ModelComponent> Model;


    enum class Type
    {
        NONE, // �f�o�b�O�p
        RED,
        BLUE
    };

    Type type{ Type::NONE };


    //--------------------<���f���̎p���Ɋւ���ϐ�>--------------------//
    Transform t;
   /* struct Transform
    {
        DirectX::XMFLOAT3 Position{ 0.0f,0.0f,0.0f };
        DirectX::XMFLOAT3 Scale{ 1.0f,1.0f,1.0f };
    };*/


    Rotate r;
   /* struct Rotate
    {
        DirectX::XMFLOAT3 Angle{ 0.0f,0.0f,0.0f };
    };*/
};

