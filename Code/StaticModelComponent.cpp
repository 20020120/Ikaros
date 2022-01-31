#include "StaticModelComponent.h"
#include"ResourceManager.h"
#include "User_Function.h"


StaticModelComponent::StaticModelComponent(ID3D11Device* device, ModelRenderInitializer Ini)
{
    System = ResourceManager::Instance().LoadStaticModelResource(device, Ini);
    MaterialColor = { 1.0f,1.0f,1.0f,1.0f };
}

StaticModelComponent::~StaticModelComponent()
{
    
}


void StaticModelComponent::Update(float elapsedTime)
{
    
}

void StaticModelComponent::Render(ID3D11DeviceContext* dc)
{

    System->Render(dc);
}




