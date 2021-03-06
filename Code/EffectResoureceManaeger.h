#pragma once

#include"Effect.h"
#include<memory>
#include<map>
#include<string>

class EffectResourceManager final
{
public:
    //--------------------<シングルトン>--------------------//
    static EffectResourceManager& Instance()
    {
        static EffectResourceManager m;
        return m;
    }

    // 読み込み
    std::shared_ptr<Effect> LoadEffect(const char* Name);

private:
    EffectResourceManager() = default;
    ~EffectResourceManager() = default;

    //--------------------<格納>--------------------//
    using EffectMap = std::map<std::string, std::weak_ptr<Effect>>;
    EffectMap mEffectMap;
};
