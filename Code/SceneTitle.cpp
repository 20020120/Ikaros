#include "SceneTitle.h"
#include<imgui.h>

#include "EnemyManager.h"


SceneTitle::SceneTitle()
{
}

SceneTitle::~SceneTitle()
{
}

void SceneTitle::Initialize(ID3D11Device* device)
{
    title_manager = std::make_unique<TitleManager>();
    title_manager->Initialize(device);

    // ボス戦まで行った後タイトルに戻っても、処理上最初からにならないので
    // タイトルのInitialize()でこの関数を呼び出す。
    EnemyManager::Instance().ResetSpawnTimer();
}

void SceneTitle::Update(float elapsedTime)
{
    GuiMenu();
    title_manager->Update(elapsedTime);
}

void SceneTitle::Render(ID3D11DeviceContext* dc)
{
    title_manager->Render(dc);
}

void SceneTitle::Finalize()
{
    title_manager->Finalize();
    
}

void SceneTitle::GuiMenu()
{
#ifdef _DEBUG
    
    title_manager->GuiMenu();
    ImGui::Begin("title");

    //ImGui::Text("pad_connect is %d", game_pad.IsPadConnecting());

    ImGui::End();
    

#endif

}