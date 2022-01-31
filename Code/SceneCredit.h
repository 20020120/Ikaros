#pragma once
#include"Scene.h"
#include"SpriteBatch.h"


#include<memory>
class SceneCredit final : public SceneParent
{
public:
    SceneCredit();
    ~SceneCredit();
    void Initialize(ID3D11Device* device) override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;

private:
    std::unique_ptr<Sprite_Batch> sprCredit{ nullptr };
    std::unique_ptr<Sprite_Batch> sprTeamLogo{ nullptr };
    std::unique_ptr<Sprite_Batch> sprFirst{ nullptr };
    
    int SceneState = 0;
    float DissolvePower{};
    float StackTimer{};
};
