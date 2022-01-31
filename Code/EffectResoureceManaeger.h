#pragma once

#include"Effect.h"
#include<memory>
#include<map>
#include<string>

class EffectResourceManager final
{
public:
    //--------------------<�V���O���g��>--------------------//
    static EffectResourceManager& Instance()
    {
        static EffectResourceManager m;
        return m;
    }

    // �ǂݍ���
    std::shared_ptr<Effect> LoadEffect(const char* Name);

private:
    EffectResourceManager() = default;
    ~EffectResourceManager() = default;

    //--------------------<�i�[>--------------------//
    using EffectMap = std::map<std::string, std::weak_ptr<Effect>>;
    EffectMap mEffectMap;
};
