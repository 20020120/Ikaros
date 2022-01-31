#pragma once

#include "StaticModelComponent.h"

#include <memory>
#include <string>
#include <map>

#include<d3d11.h>

class ResourceManager
{
private:
    ResourceManager() = default;
    ~ResourceManager() = default;

public:
    //シングルトンにする
    static ResourceManager& Instance()
    {
        static ResourceManager i;
        return i;
    }

    //モデルのリソース読み込み
    std::shared_ptr<ModelRenderSystem> LoadModelResource(ID3D11Device* device,ModelRenderInitializer Ini);

    // アニメーションなしモデルの読み込み
    std::shared_ptr<StaticModelRenderSystem> LoadStaticModelResource(ID3D11Device* device, ModelRenderInitializer Ini);

private:
    using ModelMap = std::map<std::string, std::weak_ptr<ModelRenderSystem>>;
    using StaticModelMap = std::map<std::string, std::weak_ptr<StaticModelRenderSystem>>;

    ModelMap models;
    StaticModelMap staticModels;
};
