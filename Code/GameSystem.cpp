

#include "GameSystem.h"
#include <imgui.h>

// 追加
#include "SpriteBatch.h"
#include "Camera.h"


void GameSystem::Initialize(ID3D11Device* Device, GameSystemData* Data)
{

    //                       //
    //          重要          //
    //                       //

    /* SceneGameが生成される度にGameSystemは初期化されるので、Retryなどには基本対応していません。*/
    /* 例外で、タイマーだけはクリアタイムとして正常動作します                                  */
    
    // リトライされていたら
    if(retry)
    {
        InitializeInSceneGame(Device, Data);

        retry = false;

        return;
    }




    PlayerDatas = Data;

    //if (defined) return;

    HP = std::make_unique<HealthPoint>();
    HP->Initialize(Device, PlayerDatas->HP);

    DodgeCounter = std::make_unique<DodgeCount>();
    DodgeCounter->Initialize(Device,PlayerDatas->Dodge);

    Energy_Gauge = std::make_unique<EnergyGauge>();
    Energy_Gauge->Initialize(Device,PlayerDatas->Energy);

    ScoreCounters[0] = std::make_unique<Score>();
    ScoreCounters[0]->Initialize(Device);
    ScoreCounters[1] = std::make_unique<Score>();
    ScoreCounters[1]->Initialize(Device, 7);

    Time = std::make_unique<Timer>();
    Time->Initialize(Device, COUNT::UP, 0, L"./resources/Sprite/UIs/yone.png", 3);
    Time->UnlockStop();

    ComboManager = std::make_unique<Combo>();
    ComboManager->Initialize(Device);

    spr_HitArea = std::make_unique<Sprite_Batch>(Device, L"./resources/Sprite/UIs/HitArea.png", 1);

    cameraDir = CAMERA_DIRECTION::SIDE;

    // 枠初期化
    spr_PerformanceFrame.Initialize(Device, L"./resources/Sprite/UIs/PerformanceFrame.png");
    spr_PerformanceFrame.TexSize={ 1280.0f,720.0f };

    // カメラ方向の初期化　(横)
    cameraDir = CAMERA_DIRECTION::SIDE;
    FrameDraw = false;

    // 撃破数 (SceneGame毎にリセット)
    killed_size = 0;

    //defined = true;

    retry = false;
}


void GameSystem::InitializeInSceneGame(ID3D11Device* Device, GameSystemData* Data)
{

    // リトライ時の処理

    // コンボ数,撃破数は初期化しない
    // 時間は、別関数でちゃんと処理されるので気にしなくてよい

    PlayerDatas = Data;
    

    HP = std::make_unique<HealthPoint>();
    HP->Initialize(Device, PlayerDatas->HP);

    DodgeCounter = std::make_unique<DodgeCount>();
    DodgeCounter->Initialize(Device, PlayerDatas->Dodge);

    Energy_Gauge = std::make_unique<EnergyGauge>();
    Energy_Gauge->Initialize(Device, PlayerDatas->Energy);

    //ScoreCounters[0] = std::make_unique<Score>();
    //ScoreCounters[0]->Initialize(Device);
    //ScoreCounters[1] = std::make_unique<Score>();
    //ScoreCounters[1]->Initialize(Device, 7);

    Time = std::make_unique<Timer>();
    Time->Initialize(Device, COUNT::UP, 0, L"./resources/Sprite/UIs/yone.png", 3);
    Time->UnlockStop();

    //ComboManager = std::make_unique<Combo>();
    //ComboManager->Initialize(Device);

    //spr_HitArea = std::make_unique<Sprite_Batch>(Device, L"./resources/Sprite/UIs/HitArea.png", 1);

    cameraDir = CAMERA_DIRECTION::SIDE;

    // 枠初期化
    spr_PerformanceFrame.Initialize(Device, L"./resources/Sprite/UIs/PerformanceFrame.png");
    spr_PerformanceFrame.TexSize = { 1280.0f,720.0f };

    // カメラ方向の初期化　(横)
    cameraDir = CAMERA_DIRECTION::SIDE;
    FrameDraw = false;

    // 撃破数 (SceneGame毎にリセット)
    //killed_size = 0;
}


void GameSystem::Update(float ElapsedTime)
{
    ScoreCounters[0]->Update(ElapsedTime);
    ScoreCounters[1]->Update(ElapsedTime);

    Time->Update(ElapsedTime);

    ComboManager->Update(ElapsedTime);

    Energy_Gauge->Update(ElapsedTime);

    DodgeCounter->Update(ElapsedTime);

    HP->Update(ElapsedTime);

    if(FrameDraw)
    {
        spr_PerformanceFrame.Scale.y = Calcu3D::LerpFloat(spr_PerformanceFrame.Scale.y, 1.0f, 10.0f * ElapsedTime);
    }
    else
    {
        spr_PerformanceFrame.Scale.y = Calcu3D::LerpFloat(spr_PerformanceFrame.Scale.y, 1.5f, 10.0f * ElapsedTime);
    }
}


void GameSystem::Render(ID3D11DeviceContext* Immediate_Context)
{
    // プレイヤーの当たり判定の表示
    //if (pCamera)
        //RenderHitCircle(Immediate_Context, pCamera->GetView(), pCamera->GetProjection());

    // 体力の表示
    HP->SpriteRender(Immediate_Context);

    // 回避の表示
    DodgeCounter->Render(Immediate_Context);


    Energy_Gauge->SpriteRender(Immediate_Context);

#ifdef _DEBUG
   // ScoreCounters[0]->SpriteRender(Immediate_Context, {300.0f, 0.0f}
   // );
   // ScoreCounters[1]->SpriteRender(Immediate_Context, { 300.0f, 60.0f });
   //
   // Time->SpriteRender(Immediate_Context, { 700.0f,0.0f });
   //
   // ComboManager->SpriteRender(Immediate_Context, { 800,500 });
#endif
}

//--------------------<演出中の枠描画>--------------------//
void GameSystem::PerformRender(ID3D11DeviceContext* Immediate_Context)
{
    
    spr_PerformanceFrame.Render(Immediate_Context);
}


void GameSystem::Finalize()
{
    Energy_Gauge->Finalize();
}


void GameSystem::f_Gui()
{
#if _DEBUG
    ImGui::Begin("GameSystem");

    Energy_Gauge->f_Gui();

    ImGui::Text("HP : %d", HP->NowHealthPoint());
    if (ImGui::Button("HP +1"))
    {
        HP->AddHP(1);
    }
    if (ImGui::Button("HP -1"))
    {
        HP->SubtractHP(-1);
    }

    if (ImGui::Button("Add Score 1"))
    {
        ScoreCounters[0]->Add(1);
        ScoreCounters[1]->Add(1);
    }

    if (ImGui::Button("Add Score 10"))
    {
        ScoreCounters[0]->Add(10);
        ScoreCounters[1]->Add(10);
    }


    ImGui::Text("NowCombo : %d", ComboManager->GetCombo());
    ImGui::Text("MaxCombo : %d", ComboManager->GetMaxCombo());
    if (ImGui::Button("Add Combo"))
    {
        ComboManager->AddCombo();
    }
    ImGui::Checkbox("ChangingView", &ChangingView);

    ImGui::End();
#endif
}



//****************************************************************
// 
//  Get関数
// 
//****************************************************************


bool GameSystem::CanFinisher()
{
    // ゲージの割合をみて片方が~割以上であればtrueを返す

    return false;
}


bool GameSystem::CanDodge()
{
    return DodgeCounter->CanDodge();
}


int GameSystem::GetHealthPoint()
{
    return HP->NowHealthPoint();
}


//****************************************************************
// 
//  Set関数
// 
//****************************************************************


void GameSystem::Reset()
{
    Energy_Gauge->Reset();
    ScoreCounters[0]->Reset();
}



void GameSystem::ChangeView()
{
    pCamera->ChangeView();
}



//****************************************************************
// 
//  Private関数
// 
//****************************************************************


void GameSystem::RenderHitCircle(
	ID3D11DeviceContext* Immediate_Context,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection
)
{
	// ビューポートの取得
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	Immediate_Context->RSGetViewports(&numViewports, &viewport);

	// プレイヤーの位置をスクリーン座標系に変換
	DirectX::XMVECTOR screenPos = DirectX::XMVector3Project(DirectX::XMLoadFloat3(&*PlayerDatas->position),
		viewport.TopLeftX, viewport.TopLeftY,
		viewport.Width, viewport.Height,
		viewport.MinDepth, viewport.MaxDepth,
        DirectX::XMLoadFloat4x4(&projection), DirectX::XMLoadFloat4x4(&view), DirectX::XMMatrixIdentity());

	DirectX::XMFLOAT2 pos;
	DirectX::XMStoreFloat2(&pos, screenPos);
    
    spr_HitArea->Begin(Immediate_Context);
    spr_HitArea->Render(Immediate_Context, pos, { 1,1 });
    spr_HitArea->End(Immediate_Context);
}
