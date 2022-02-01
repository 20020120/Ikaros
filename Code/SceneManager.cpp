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
// ‰Šú‰»
// 
//****************************************************************
void SceneManager::Initialize()
{
	CurrentScene->Initialize(Device);
}
//****************************************************************
// 
// XVˆ—
// 
//****************************************************************
void SceneManager::Update(float elapsedTime)
{
	CurrentScene->Update(elapsedTime);
}
//****************************************************************
// 
// •`‰æˆ—
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

	//–¢‰Šú‰»‚Ìê‡‚Í‰Šú‰»‚·‚é
	if (!CurrentScene->IsReady())
	{
		Initialize();
	}
}
