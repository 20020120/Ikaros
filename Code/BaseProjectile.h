#pragma once

#include"BaseObject.h"

class BaseProjectile :public BaseObject
{
public:
    BaseProjectile()=default;
    ~BaseProjectile()=default;

    virtual void Initialize() override {}
    virtual void Update(float elapsedTime) {}
    virtual void Render(ID3D11DeviceContext* dc) override {}
    virtual void Finalize() override {}

    // 誰から発射されたか
    enum class Parent
    {
        NONE ,
        REDENEMY,
        BLUEENEMY,
        REDPLAYER,
        BLUEPLAYER
    };

    Parent parent{ Parent::NONE };

    // エネルギーか弾か
    enum class ProjType
    {
        NONE,
        REDBULLET,//0
        BLUEBULLET,//1
        REDENERGY,//2
        BLUEENERGY,//3
    };

    ProjType projtype{ProjType::NONE};

    DirectX::XMFLOAT3 Velocity{};

    float lifeTimer{ 15.0f };
    int hitCount = 1;
    float speed = 30.0f;//弾のスピード
    float defaultSpeed = 30.0f;
    
    //getter,setter
    Parent& GetParent() { return parent; }
    ProjType& GetProjType() { return projtype; }
    [[nodiscard]] float GetLifeTimer() const { return lifeTimer; }
    [[nodiscard]] int GetHitCount() const { return hitCount; }

    void AddSpeed(const float addRatio) { speed = defaultSpeed * addRatio; }


    void Hit()
    {
        if (invTimer <= 0.0f)
        {
            hitCount--;
            invTimer = maxInvTimer;
        }
    }

};

