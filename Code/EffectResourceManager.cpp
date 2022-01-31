#include"EffectResoureceManaeger.h"

std::shared_ptr<Effect> EffectResourceManager::LoadEffect(const char* Name)
{


    //モデルを検索
    auto it = mEffectMap.find(Name);
    if (it != mEffectMap.end())
    {
        if (!it->second.expired())
        {
            return it->second.lock();
        }
    }
    //新規モデルリソース作成＆読み込み
    // std::shared_ptr<ModelResource> model;
    auto effect = std::make_shared<Effect>(Name);

    //マップに登録
    mEffectMap[Name] = effect;

    return effect;
}
