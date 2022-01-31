#pragma once
#include"RenderSystem.h"
#include"Leaf.h"
class ModelComponent
{
public:
    ModelComponent(ID3D11Device* device, ModelRenderInitializer Ini);
    ~ModelComponent();

    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************
    void f_PlayAnimation(int AnimeNumber, bool isLoop = false);

    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc, Transform t, Rotate r);
    void Render(ID3D11DeviceContext* dc, Transform t, DirectX::XMFLOAT4 orientation);

    //--------------------<ゲッター>--------------------//

    const auto& GetCurrentAnimationNumber() const { return CurrentAnimationNumber; }
    const auto& GetCurrentAnimationFrame() const { return CurrentAnimationFrame; }
    const auto& GetIsPlayAnimation()const { return IsPlayAnimation; }
    void SetMaterialColor(DirectX::XMFLOAT4 col) { MaterialColor = col; }
private:
    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************

    std::shared_ptr<ModelRenderSystem> System;   
    //色
    DirectX::XMFLOAT4 MaterialColor;

    //--------------------<アニメーション関係>--------------------//
    //キーフレーム
    ModelAnimationResource::KeyFrame KeyFrame{};
    //現在のアニメーションの番号
    int CurrentAnimationNumber;
    int OldAnimationNumber{};

    //アニメーションが再生中かどうか
    bool IsPlayAnimation;

    //アニメーションをループさせるかどうか
    bool IsLoopAnimation;

    //現在のアニメーションのフレーム
    float CurrentAnimationFrame;
    
    //アニメーションのサイズ
    float AnimationSize;
    float animation_tick{ 0.0f };
    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************

    //アニメーションの更新
    void f_UpdateAnimation();
    //キーフレームの算出
    void GetKeyFrame(float elapsedTime);
    // アニメーションのブレンド
    void f_BlendAnimation(const ModelAnimationResource::KeyFrame* keyframes[2], float factor,
        ModelAnimationResource::KeyFrame& keyframe);
};
