#pragma once
#include"Scene.h"
#include"ModelComponent.h"
#include"Camera.h"
#include"Light.h"
#include<memory>
#include"Renderer.h"
class SceneLoading :public SceneParent
{
public:
    SceneLoading(SceneParent* NextScene);
    ~SceneLoading();

    void Initialize(ID3D11Device* device)override;
    void Update(float elapsedTime)override;
    void Render(ID3D11DeviceContext* dc)override;
    void Finalize()override;

private:
    static void LoadingThread(SceneLoading* scene);
    SceneParent* NextScene;

    std::unique_ptr<ModelComponent> Model;
    std::unique_ptr<Camera> mCamera;
    std::unique_ptr < DirectionLight> mDirectionLight;

    Renderer sprLoading{};

    float animTimerY = 0.0f;
};
