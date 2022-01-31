#pragma once
#include"BaseProjectile.h"
#include"SpriteBatch.h"
#include"Texture.h"

class Energy : public BaseProjectile
{
public:
    Energy(ID3D11Device* d,
        Parent p,
        DirectX::XMFLOAT3 pos,
        DirectX::XMFLOAT3 velocity
        );
    ~Energy();

    void Initialize() override;
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc) override;
    void Finalize() override;

private:
    //ä÷êî
    void Move(float elapsedTime);

    //ïœêî
    float speed = 15.0f;

    Texture mEmisiveTexture{};
    
};

