#include "SceneManager.h"
#include"SceneTitle.h"
#include"SceneGame.h"
#include"SceneCredit.h"
#include"SceneLoading.h"
SceneManager::SceneManager()
{
	CurrentScene = new SceneCredit();
}


SceneManager::~SceneManager()
{

	//CurrentScene->Finalize();
	delete CurrentScene;
}

//****************************************************************
// 
// 初期化
// 
//****************************************************************
void SceneManager::Initialize()
{
	CurrentScene->Initialize(Device);
}
//****************************************************************
// 
// 更新処理
// 
//****************************************************************
void SceneManager::Update(float elapsedTime)
{
	CurrentScene->Update(elapsedTime);
}
//****************************************************************
// 
// 描画処理
// 
//****************************************************************
void SceneManager::Render(ID3D11DeviceContext* dc)
{
	CurrentScene->Render(dc);
}

void SceneManager::Finalize()
{
	CurrentScene->Finalize();
}


void SceneManager::ChangeScene(SceneParent* s)
{
	if (CurrentScene != nullptr)
	{
		CurrentScene->Finalize();
		delete CurrentScene;
		CurrentScene = nullptr;
	}
	CurrentScene = s;

	//未初期化の場合は初期化する
	if (!CurrentScene->IsReady())
	{
		Initialize();
	}
}
