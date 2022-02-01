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
// ������
// 
//****************************************************************
void SceneManager::Initialize()
{
	CurrentScene->Initialize(Device);
}
//****************************************************************
// 
// �X�V����
// 
//****************************************************************
void SceneManager::Update(float elapsedTime)
{
	CurrentScene->Update(elapsedTime);
}
//****************************************************************
// 
// �`�揈��
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

	//���������̏ꍇ�͏���������
	if (!CurrentScene->IsReady())
	{
		Initialize();
	}
}
