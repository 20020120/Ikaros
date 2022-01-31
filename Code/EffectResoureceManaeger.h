#pragma once

#include"Effect.h"
#include<memory>
#include<map>
#include<string>

class EffectResourceManager final
{
public:
    //--------------------<ÉVÉìÉOÉãÉgÉì>--------------------//
    static EffectResourceManager& Instance()
    {
        static EffectResourceManager m;
        return m;
    }

    // ì«Ç›çûÇ›
    std::shared_ptr<Effect> LoadEffect(const char* Name);

private:
    EffectResourceManager() = default;
    ~EffectResourceManager() = default;

    //--------------------<äiî[>--------------------//
    using EffectMap = std::map<std::string, std::weak_ptr<Effect>>;
    EffectMap mEffectMap;
};
