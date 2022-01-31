#pragma once
#include <d3d11.h>
class SceneParent
{
public:
    SceneParent()
        :Ready()
    {}
    virtual~SceneParent() = default;

    virtual void Initialize(ID3D11Device* device) = 0;
    virtual void Update(float elapsedTime) = 0;
    virtual void Render(ID3D11DeviceContext* dc) = 0;
    virtual void Finalize() = 0;

    bool IsReady() const { return Ready; }
    void SetReady() { Ready = true; }

private:
    bool Ready{};
};

