#include "SceneLoading.h"
#include"SceneManager.h"

#include<thread>
SceneLoading::SceneLoading(SceneParent* nextscene)
{
    NextScene = nextscene;
}

SceneLoading::~SceneLoading()
{

}

void SceneLoading::Initialize(ID3D11Device* device)
{
    // モデルをロード
    ModelRenderInitializer Ini;
    Ini.SetALLPath(
        "./resources/Models/Load/LoadIcon.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/LambertPS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );
    Model = std::make_unique<ModelComponent>(device, Ini);

    sprLoading.Initialize(device, L"./resources/Sprite/LoadScene/loading.png");
    sprLoading.Position = {590.0f,550.0f};
    sprLoading.Scale = { 0.4f,0.4f };
    sprLoading.TexSize = { 285,64 };
    sprLoading.Color.w = 0.75f;
    animTimerY = 0.0f;

    // かめらを初期化
    mCamera = std::make_unique<Camera>(device);

    Model->f_PlayAnimation(0, true);

    mDirectionLight = std::make_unique<DirectionLight>(device);

    //スレッド開始
    std::thread Thread(LoadingThread, this);
    //スレッドの管理を放棄
    Thread.detach();
}

void SceneLoading::Update(float elapsedTime)
{
    mCamera->SetTarget({ 0.0f,0.0f,0.0f });
    mCamera->Update(elapsedTime);

    Model->Update(elapsedTime);

    animTimerY += elapsedTime;
    if (animTimerY > 0.1f)
    {
        int r = rand() % 20;
        if (r >= 18)sprLoading.TexPos.y = 128.0f;
        else if (r >= 13)sprLoading.TexPos.y = 64.0f;
        else sprLoading.TexPos.y = 0.0f;
        animTimerY = 0.0f;
    }

    if (NextScene->IsReady())
    {
        SceneManager::Instance().ChangeScene(NextScene);
    }

}

void SceneLoading::Render(ID3D11DeviceContext* dc)
{
    // かめら更新
    mCamera->f_CameraUpdate(dc);

    mDirectionLight->Update(dc);

    Transform t;
    t.Position = { 30.0f,0.0f,0.0f };
    t.Scale = { 1.0f,1.0f,1.0f };
    Rotate r;
    r.Angle.x = DirectX::XMConvertToRadians(-90.0f);
    r.Angle.y = DirectX::XMConvertToRadians(-90.0f);

    Model->Render(dc, t, r);

    sprLoading.Render(dc);
}

void SceneLoading::Finalize()
{
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
    CoInitialize(nullptr);

    auto device = SceneManager::Instance().GetDevice();

    //次のシーンの初期化を行う
    scene->NextScene->Initialize(device);

    //スレッドが終わる前にCOM関連の終了化
    CoUninitialize();

    //次のシーンの準備完了設定
    scene->NextScene->SetReady();
}
