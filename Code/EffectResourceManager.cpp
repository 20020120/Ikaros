#include"EffectResoureceManaeger.h"

std::shared_ptr<Effect> EffectResourceManager::LoadEffect(const char* Name)
{


    //���f��������
    auto it = mEffectMap.find(Name);
    if (it != mEffectMap.end())
    {
        if (!it->second.expired())
        {
            return it->second.lock();
        }
    }
    //�V�K���f�����\�[�X�쐬���ǂݍ���
    // std::shared_ptr<ModelResource> model;
    auto effect = std::make_shared<Effect>(Name);

    //�}�b�v�ɓo�^
    mEffectMap[Name] = effect;

    return effect;
}
