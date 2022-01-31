#include"ModelComponent.h"
#include"User_Function.h"

#include"ResourceManager.h"
ModelComponent::ModelComponent(ID3D11Device* device, ModelRenderInitializer Ini)
    :CurrentAnimationNumber(0)
    ,OldAnimationNumber(0)
    ,IsPlayAnimation(false)
    ,MaterialColor(1.0f,1.0f,1.0f,1.0f)
{
    animation_tick = 0.0f;
    //System = std::make_shared<ModelRenderSystem>(device, Ini);
    System = ResourceManager::Instance().LoadModelResource(device, Ini);
}

ModelComponent::~ModelComponent()
{

}

void ModelComponent::f_PlayAnimation(int AnimeNumber,bool isLoop)
{
    CurrentAnimationNumber = AnimeNumber;
    IsPlayAnimation = true;
    IsLoopAnimation = isLoop;
    animation_tick = 0.0f;
}

void ModelComponent::Update(float elapsedTime)
{

    //auto& kf = System->GetAnimeResources().at(CurrentAnimationNumber).Sequence.at(0);
   // CurrentAnimationFrame = 0.0f;


    GetKeyFrame(elapsedTime);
}

void ModelComponent::Render(ID3D11DeviceContext* dc, Transform t, Rotate r)
{
    //各情報からワールド変換行列を作成する
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(t.Scale.x,t.Scale.y,t.Scale.z) };
    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z) };
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(t.Position.x, t.Position.y, t.Position.z) };

    
    DirectX::XMFLOAT4X4 World = f_SetWorldCoordinate(WorldCoordinate::LHS_YUP, S, R, T);

    System->Render(dc, World, MaterialColor, &KeyFrame);
}

void ModelComponent::Render(ID3D11DeviceContext* dc, Transform t, DirectX::XMFLOAT4 orientation)
{
    //各情報からワールド変換行列を作成する
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(t.Scale.x,t.Scale.y,t.Scale.z) };
    
    DirectX::XMVECTOR O = DirectX::XMLoadFloat4(&orientation);
    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(O) };

    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(t.Position.x, t.Position.y, t.Position.z) };


    DirectX::XMFLOAT4X4 World = f_SetWorldCoordinate(WorldCoordinate::LHS_YUP, S, R, T);

    System->Render(dc, World, MaterialColor, &KeyFrame);
}

void ModelComponent::GetKeyFrame(float elapsedTime)
{
    float frame_index{ 0 };

    ModelAnimationResource& animation{ System->GetAnimeResources().at(CurrentAnimationNumber) };
    //--------------------<もしアニメーションのデータがNONEなら>--------------------//
    if (animation.Name == "None")
    {
        KeyFrame = { animation.Sequence.at(0) };

    }

    if (IsPlayAnimation)
    {
        int animvalue{ static_cast<int>(System->GetAnimeResources().size()) };
        frame_index = animation_tick * animation.SamplingRate;

        CurrentAnimationFrame = frame_index;
        AnimationSize = static_cast<float>(animation.Sequence.size() - 1);

        if (CurrentAnimationFrame > AnimationSize)
        {
            CurrentAnimationFrame = 0;
            animation_tick = 0;
            if (!IsLoopAnimation) IsPlayAnimation = false;

        }
        else
        {
            animation_tick += elapsedTime;
        }
    }
    unsigned int currentanimframe = static_cast<unsigned int>(CurrentAnimationFrame);
    KeyFrame = { animation.Sequence.at(currentanimframe) };
}

void ModelComponent::f_BlendAnimation(const ModelAnimationResource::KeyFrame* keyframes[2], float factor, ModelAnimationResource::KeyFrame& keyframe)
{
    using namespace DirectX;

    size_t node_count{ keyframes[0]->NodeVec.size() };
    keyframe.NodeVec.resize(node_count);
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        XMVECTOR S[2]{
            XMLoadFloat3(&keyframes[0]->NodeVec.at(node_index).Scaling),
            XMLoadFloat3(&keyframes[1]->NodeVec.at(node_index).Scaling) };
        XMStoreFloat3(&keyframe.NodeVec.at(node_index).Scaling, XMVectorLerp(S[0], S[1], factor));

        XMVECTOR R[2]{
            XMLoadFloat4(&keyframes[0]->NodeVec.at(node_index).Rotation),
            XMLoadFloat4(&keyframes[1]->NodeVec.at(node_index).Rotation) };
        XMStoreFloat4(&keyframe.NodeVec.at(node_index).Rotation, XMQuaternionSlerp(R[0], R[1], factor));

        XMVECTOR T[2]{ XMLoadFloat3(&keyframes[0]->NodeVec.at(node_index).Translation),
            XMLoadFloat3(&keyframes[1]->NodeVec.at(node_index).Translation) };
        XMStoreFloat3(&keyframe.NodeVec.at(node_index).Translation, XMVectorLerp(T[0], T[1], factor));
    }

    //****************************************************************
    // 
    //  補完中の更新処理
    // 
    //****************************************************************

    size_t node_count1{ keyframe.NodeVec.size() };
    for (size_t node_index = 0; node_index < node_count1; ++node_index)
    {
        ModelAnimationResource::KeyFrame::Node& node{ keyframe.NodeVec.at(node_index) };
        XMMATRIX S{ XMMatrixScaling(node.Scaling.x, node.Scaling.y, node.Scaling.z) };
        XMMATRIX R{ XMMatrixRotationQuaternion(XMLoadFloat4(&node.Rotation)) };
        XMMATRIX T{ XMMatrixTranslation(node.Translation.x, node.Translation.y, node.Translation.z) };
   
        int64_t parent_index{System->GetRenderResource().SceneView.NodeVec.at(node_index).ParentIndex };
        XMMATRIX P{ parent_index < 0 ? XMMatrixIdentity() :
            XMLoadFloat4x4(&keyframe.NodeVec.at(parent_index).GlobalTransform) };
   
        XMStoreFloat4x4(&node.GlobalTransform, S * R * T * P);
    }

}


