#include "ProjectileManager.h"

#include<imgui.h>

#include "GameSystem.h"

void ProjectileManager::Update(float elapsedTime)
{
    // éãì_ïœçXââèoíÜÇÕÇ∑Ç◊Çƒîjä¸
    if(GameSystem::Instance().GetChangingView())
    {
        for (const auto& projectile : projectiles)
        {
            Remove(projectile.get());
        }
    }


    for (const auto& projectile : projectiles)
    {
        projectile->Update(elapsedTime);
    }

    // îjä¸èàóù
    for (std::unique_ptr<BaseProjectile>& projectile : removes)
    {
        std::vector<std::unique_ptr<BaseProjectile>>::iterator it =
            std::find(projectiles.begin(), projectiles.end(), projectile);
        if (it != projectiles.end()) { projectiles.erase(it); }
        if (projectile != nullptr) { projectile.release(); }
    }
    removes.clear();

    UpdateSpeed();
}

void ProjectileManager::Render(ID3D11DeviceContext* dc)
{
    for (const auto& projectile : projectiles)
    {
        projectile->Render(dc);
    }

    //Gui
    GuiMenu();
}


void ProjectileManager::Finalize()
{
    for (std::unique_ptr<BaseProjectile>& projectile : removes)
    {
        std::vector<std::unique_ptr<BaseProjectile>>::iterator it =
            std::find(projectiles.begin(), projectiles.end(), projectile);
        if (it != projectiles.end()) { projectiles.erase(it); }
        if (projectile != nullptr) { projectile.release(); }
    }
    removes.clear();

    projectiles.clear();

}



void ProjectileManager::Clear()
{
    projectiles.clear();
}

void ProjectileManager::RegisterProjectile(BaseProjectile* projectile)
{
    projectiles.emplace_back(projectile);
}

void ProjectileManager::Remove(BaseProjectile* projectile)
{
    removes.emplace_back(projectile);
}

void ProjectileManager::UpdateSpeed()
{
    for (const auto& projectile : projectiles)
    {
        switch(projectile->GetParent())
        {
        case BaseProjectile::Parent::REDENEMY:
            projectile->AddSpeed(redEnemyAddSpeed);
            break;
        case BaseProjectile::Parent::BLUEENEMY:
            projectile->AddSpeed(blueEnemyAddSpeed);
            break;
        case BaseProjectile::Parent::REDPLAYER:
            projectile->AddSpeed(redPlayerAddSpeed);
            break;
        case BaseProjectile::Parent::BLUEPLAYER:
            projectile->AddSpeed(bluePlayerAddSpeed);
            break;
        }
    }
}


void ProjectileManager::GuiMenu()
{
#ifdef _DEBUG
    ImGui::Begin("ProjectileManager");
    ImGui::SliderFloat("RedPlayerBulletLife", &RedPlayerBulletLife, 0.0f, 25.0f, "%.5f");
    ImGui::SliderFloat("BluePlayerBulletLife", &BluePlayerBulletLife, 0.0f, 25.0f, "%.5f");

    ImGui::DragFloat("redPlayerAddSpeed", &redPlayerAddSpeed, 0.01f, 0.0f, 3.0f);
    ImGui::DragFloat("bluePlayerAddSpeed", &bluePlayerAddSpeed, 0.01f, 0.0f, 3.0f);
    ImGui::DragFloat("redEnemyAddSpeed",  &redEnemyAddSpeed, 0.01f, 0.0f, 3.0f);
    ImGui::DragFloat("blueEnemyAddSpeed", &blueEnemyAddSpeed, 0.01f, 0.0f, 3.0f);
    ImGui::End();
#endif
}
