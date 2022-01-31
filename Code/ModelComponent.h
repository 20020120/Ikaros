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
    // �֐�
    // 
    //****************************************************************
    void f_PlayAnimation(int AnimeNumber, bool isLoop = false);

    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc, Transform t, Rotate r);
    void Render(ID3D11DeviceContext* dc, Transform t, DirectX::XMFLOAT4 orientation);

    //--------------------<�Q�b�^�[>--------------------//

    const auto& GetCurrentAnimationNumber() const { return CurrentAnimationNumber; }
    const auto& GetCurrentAnimationFrame() const { return CurrentAnimationFrame; }
    const auto& GetIsPlayAnimation()const { return IsPlayAnimation; }
    void SetMaterialColor(DirectX::XMFLOAT4 col) { MaterialColor = col; }
private:
    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************

    std::shared_ptr<ModelRenderSystem> System;   
    //�F
    DirectX::XMFLOAT4 MaterialColor;

    //--------------------<�A�j���[�V�����֌W>--------------------//
    //�L�[�t���[��
    ModelAnimationResource::KeyFrame KeyFrame{};
    //���݂̃A�j���[�V�����̔ԍ�
    int CurrentAnimationNumber;
    int OldAnimationNumber{};

    //�A�j���[�V�������Đ������ǂ���
    bool IsPlayAnimation;

    //�A�j���[�V���������[�v�����邩�ǂ���
    bool IsLoopAnimation;

    //���݂̃A�j���[�V�����̃t���[��
    float CurrentAnimationFrame;
    
    //�A�j���[�V�����̃T�C�Y
    float AnimationSize;
    float animation_tick{ 0.0f };
    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************

    //�A�j���[�V�����̍X�V
    void f_UpdateAnimation();
    //�L�[�t���[���̎Z�o
    void GetKeyFrame(float elapsedTime);
    // �A�j���[�V�����̃u�����h
    void f_BlendAnimation(const ModelAnimationResource::KeyFrame* keyframes[2], float factor,
        ModelAnimationResource::KeyFrame& keyframe);
};
