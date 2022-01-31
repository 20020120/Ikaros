#pragma once

#include <vector>
#include <memory>
#include "BaseProjectile.h"

class ProjectileManager
{
private:
    ProjectileManager() {}
    ~ProjectileManager() {}
public:
    static ProjectileManager& Instance()
    {
        static ProjectileManager instance;
        return instance;
    }
    //--------< ä÷êî >--------//
    void Initialize(ID3D11Device* device) { projectiles.clear(); }
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);
    void Finalize();

    //íeÇÃë¨ìxçXêV
    void UpdateSpeed();

    //getter,setter
    const int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }//íeêîéÊìæ
    BaseProjectile& GetProjectile(int index) const { return *projectiles.at(index); }// íeéÊìæ
    float GetRedPlayerBulletLife() { return RedPlayerBulletLife; }
    float GetBluePlayerBulletLife() { return BluePlayerBulletLife; }

    void SetTarget(DirectX::XMFLOAT3 p) { Target = p; }
    const auto& GetTarget() { return Target; }
    // íeìoò^
    void RegisterProjectile(BaseProjectile* projectile);
    // íeçÌèú
    void Clear();
    // íeçÌèú
    void Remove(BaseProjectile* projectile);
    //IMGUI
    void GuiMenu();

private:
    //--------< ïœêî >--------//
    std::vector<std::unique_ptr<BaseProjectile>> projectiles;
    std::vector<std::unique_ptr<BaseProjectile>> removes;

    float RedPlayerBulletLife = 1.0f;
    float BluePlayerBulletLife = 10.0f;

    float redPlayerAddSpeed = 1.0f;
    float bluePlayerAddSpeed = 1.0f;
    float redEnemyAddSpeed = 1.0f;
    float blueEnemyAddSpeed = 1.0f;

    DirectX::XMFLOAT3 Target{};
};