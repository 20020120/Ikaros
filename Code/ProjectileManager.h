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
    //--------< �֐� >--------//
    void Initialize(ID3D11Device* device) { projectiles.clear(); }
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);
    void Finalize();

    //�e�̑��x�X�V
    void UpdateSpeed();

    //getter,setter
    const int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }//�e���擾
    BaseProjectile& GetProjectile(int index) const { return *projectiles.at(index); }// �e�擾
    float GetRedPlayerBulletLife() { return RedPlayerBulletLife; }
    float GetBluePlayerBulletLife() { return BluePlayerBulletLife; }

    void SetTarget(DirectX::XMFLOAT3 p) { Target = p; }
    const auto& GetTarget() { return Target; }
    // �e�o�^
    void RegisterProjectile(BaseProjectile* projectile);
    // �e�폜
    void Clear();
    // �e�폜
    void Remove(BaseProjectile* projectile);
    //IMGUI
    void GuiMenu();

private:
    //--------< �ϐ� >--------//
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