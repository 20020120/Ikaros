#include"ResourceManager.h"

//--------------------<アニメーションありモデル>--------------------//
std::shared_ptr<ModelRenderSystem> ResourceManager::LoadModelResource(ID3D11Device* device, ModelRenderInitializer Ini)
{
    //モデルを検索
    ModelMap::iterator it = models.find(Ini.GetPath(0));
    if (it != models.end())
    {
        if (!it->second.expired())
        {
            return it->second.lock();
        }
    }

    //新規モデルリソース作成＆読み込み
    // std::shared_ptr<ModelResource> model;
    auto model = std::make_shared<ModelRenderSystem>(device, Ini);

    //マップに登録
    models[Ini.GetPath(0)] = model;

    return model;
}

//--------------------<アニメーションなしモデル>--------------------//

std::shared_ptr<StaticModelRenderSystem> ResourceManager::LoadStaticModelResource(ID3D11Device* device, ModelRenderInitializer Ini)
{
    //モデルを検索
    StaticModelMap::iterator it = staticModels.find(Ini.GetPath(0));
    if (it != staticModels.end())
    {
        if (!it->second.expired())
        {
            return it->second.lock();
        }
    }

    //新規モデルリソース作成＆読み込み
    // std::shared_ptr<ModelResource> model;
    auto model = std::make_shared<StaticModelRenderSystem>(device, Ini);

    //マップに登録
    staticModels[Ini.GetPath(0)] = model;

    return model;
}




