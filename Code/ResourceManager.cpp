#include"ResourceManager.h"

//--------------------<�A�j���[�V�������胂�f��>--------------------//
std::shared_ptr<ModelRenderSystem> ResourceManager::LoadModelResource(ID3D11Device* device, ModelRenderInitializer Ini)
{
    //���f��������
    ModelMap::iterator it = models.find(Ini.GetPath(0));
    if (it != models.end())
    {
        if (!it->second.expired())
        {
            return it->second.lock();
        }
    }

    //�V�K���f�����\�[�X�쐬���ǂݍ���
    // std::shared_ptr<ModelResource> model;
    auto model = std::make_shared<ModelRenderSystem>(device, Ini);

    //�}�b�v�ɓo�^
    models[Ini.GetPath(0)] = model;

    return model;
}

//--------------------<�A�j���[�V�����Ȃ����f��>--------------------//

std::shared_ptr<StaticModelRenderSystem> ResourceManager::LoadStaticModelResource(ID3D11Device* device, ModelRenderInitializer Ini)
{
    //���f��������
    StaticModelMap::iterator it = staticModels.find(Ini.GetPath(0));
    if (it != staticModels.end())
    {
        if (!it->second.expired())
        {
            return it->second.lock();
        }
    }

    //�V�K���f�����\�[�X�쐬���ǂݍ���
    // std::shared_ptr<ModelResource> model;
    auto model = std::make_shared<StaticModelRenderSystem>(device, Ini);

    //�}�b�v�ɓo�^
    staticModels[Ini.GetPath(0)] = model;

    return model;
}




