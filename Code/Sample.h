#pragma once

#include"ModelComponent.h"
#include"EventFlag.h"
#include"Leaf.h"

#include<d3d11.h>
#include<memory>

#include <imgui.h>

//****************************************************************
// 
//  キャラクターのサンプル
// 
//****************************************************************
class SampleCharacter
{
public:
    SampleCharacter(ID3D11Device* d);
    ~SampleCharacter();

    void Initialize();
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);
    void Finalize();
private:


    std::unique_ptr<ModelComponent> model;  //  include "ModelComponent.h"
    EventFlagSystem EventFlag;              //  include "EventFlag.h"
    Transform t{};                          //  include "Leaf.h"
    Rotate r{};                             //  include "Leaf.h"


    enum class EventKeys {
        testFlag1,
        testFlag2,
    };

};


//****************************************************************
// 
//  関数定義
// 
//****************************************************************


inline SampleCharacter::SampleCharacter(ID3D11Device* d)
{
    //--------------------<モデルの初期化>--------------------//
    ModelRenderInitializer Ini{};
    Ini.SetALLPath(
        "./resources/Models/SiotaTest/SiotaTest.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/LambertPS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );

    model = std::make_unique<ModelComponent>(d, Ini);


    //--------------------<姿勢に関する構造体の初期化>--------------------//
    t.Position = {
        0.0f,
        0.0f,
        0.0f
    };

    t.Scale = {
        0.3f,
        0.3f,
        0.3f
    };

    r.Angle = {
        0.0f,
        0.0f,
        0.0f
    };


    //--------------------<イベントフラグの初期化>--------------------//
    EventFlag.Load("./resources/Models/SiotaTest/SampleFlag.flg");
}

inline void SampleCharacter::Initialize()
{
    
}

inline void SampleCharacter::Update(float elapsedTime)
{
    //--------------------<モデルの更新処理>--------------------//
    model->Update(elapsedTime);

    ImGui::Begin("SampleCharacter");
    if (ImGui::Button("PlayAnimation"))
    {
        // アニメーション
        model->f_PlayAnimation(0, false);
    }

    if (ImGui::CollapsingHeader("EventFlag"))
    {
        // testFlag1
        bool a = EventFlag.Check("testFlag1", model->GetCurrentAnimationNumber(), model->GetCurrentAnimationFrame());
        ImGui::Checkbox("testFlag1", &a);

        a= EventFlag.Check("testFlag2", model->GetCurrentAnimationNumber(), model->GetCurrentAnimationFrame());
        ImGui::Checkbox("testFlag2", &a);
    }
    ImGui::End();

}

inline void SampleCharacter::Render(ID3D11DeviceContext* dc)
{
    model->Render(dc, t, r);
}

inline void SampleCharacter::Finalize()
{
    
}


inline SampleCharacter::~SampleCharacter()
{
    
}


