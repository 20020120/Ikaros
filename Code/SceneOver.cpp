#include "SceneOver.h"

#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTitle.h"

#include"MathSupport.h"
#include "User_Function.h"

#include"SceneManager.h"
#include"SceneTitle.h"
#include"SceneGame.h"
#include "gAudio.h"
#include "OptionSystem.h"
#include <imgui.h>

#include "EffectManager.h"
#include "EnemyManager.h"

SceneOver::SceneOver()
{

}
SceneOver::~SceneOver()
{

}

void SceneOver::Initialize(ID3D11Device* device)
{
	font = std::make_unique<Font>(device, "./resources/Sprite/Font/Font.fnt", 2048);

	SprChoiceFrame.Initialize(device, L"./resources/Sprite/UIs/SettingFrame.png");
	SprChoiceFrame.Position = { 380.0f,560.0f };
	SprChoiceFrame.Angle = { 0.0f };
	SprChoiceFrame.Color = { 0.85f,0.0f,0.0f,1.0f };
	SprChoiceFrame.Scale = { 0.6f,0.4f };
	SprChoiceFrame.TexPos = { 0.0f,0.0f };
	SprChoiceFrame.TexSize = { 440.0f,260.0f };
	SprChoiceFrame.Pivot = SprChoiceFrame.TexSize * 0.5f;

	SprLogo.Initialize(device, L"./resources/Sprite/CutIn/OverLogo.png");
	SprLogo.Position = { 330.0f,320.0f };
	SprLogo.Angle = 0.0f;
	SprLogo.Color = { 1.0f,1.0f,1.0f,1.0f };
	SprLogo.Scale = { 1.4f,1.4f };
	SprLogo.TexPos = { 0.0f,0.0f };
	SprLogo.TexSize = { 382.0f,64.0f };
	SprLogo.Pivot = SprChoiceFrame.TexSize * 0.5f;

	SprBlackOut.Initialize(device, L"./resources/Sprite/UIs/Black.png");
	SprBlackOut.Position = { 0.0f,0.0f };
	SprBlackOut.Angle = 0.0f;
	SprBlackOut.Color = { 1.0f,1.0f,1.0f,0.0f };
	SprBlackOut.Scale = { 1.0f,1.0f };
	SprBlackOut.TexPos = { 0.0f,0.0f };
	SprBlackOut.TexSize = { 1280.0f,720.0f };
	SprBlackOut.Pivot = {0,0};


	Ratio = 0.0f;

	camera = std::make_unique<Camera>(device);
	directionLight = std::make_unique<DirectionLight>(device);
	DirectX::XMFLOAT3 dir = { 0.655f,-0.574f,-0.492f };
	directionLight->SetDirection(dir);

	//Boss = std::make_unique<GameOverBoss>(device);
	player = std::make_unique<OverPlayer>(device);

	StackTime = 0.0f;
	scrollTime = 0.0f;

	seSelect = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/OperateSE.wav");
	seSandstorm = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/Sandstorm.wav");
	bgm = Audio::Instance().LoadAudioSource("./resources/Sounds/BGM/SceneOver.wav");
	bgm->Play(true, OptionSystem::Instance().GetBgmVolume() * 0.5f);

	sandstormVolume = 0.1f;
	playSE = false;

	blackOut = false;
	backTitle = false;
}

void SceneOver::Finalize()
{
	bgm->Release();
	seSelect->Release();
	seSandstorm->Release();

	player->Finalize();
	EffectManager::Instance().GetEffekseerManager()->StopAllEffects();
}

void SceneOver::Update(float elapsedTime)
{
	StackTime += elapsedTime;

	if (StackTime > 2.0f)
	{
		
	}

	//bgm->SetVolume(OptionSystem::Instance().GetBgmVolume());
	//camera->SetTarget(player->t.Position);

	player->Update(elapsedTime);

	LogoUpdate(elapsedTime);

	if (!(player->GetTransformAnim()))
		ChoiceScene(elapsedTime);

	//playerの処理が終わったらブラックアウト
	if(player->GetFinishPerform() || backTitle)
	{
		SprBlackOut.Color.w += 1.0f * elapsedTime;
		if(SprBlackOut.Color.w > 1.0f)
		{
			SprBlackOut.Color.w = 1.0f;
			blackOut = true;
		}
	}
	//ブラックアウト終わったらシーン遷移
	if(blackOut)
	{
		if (player->GetFinishPerform())
		{
			if (EnemyManager::Instance().GetIsBossBattle())
			{
				GameSystem::Instance().SetRetry();
			}
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame()));
			return;
		}
		if(backTitle)
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle()));
			return;
		}
	}

	camera->Update(elapsedTime);


	EffectManager::Instance().Update(elapsedTime);

	//****************************************************************
	// 
	//  シーン遷移
	// 
	//****************************************************************

	const GamePadButton startButton = GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y;
	if (GamePad::Instance().GetButtonDown() & startButton && (Ratio > 0.8f || Ratio < 0.2f))
	{
		switch (nextMenu)
		{
		case RETRY:
			player->SetStartTransform(true);
			break;
		case TITLE:
			backTitle = true;
			break;
		default:;
		}
	}


#if _DEBUG
	ImGui::Begin("GameOver");

	ImGui::DragFloat2("pos", &SprLogo.Position.x, 0.1f);
	ImGui::DragFloat2("scale", &SprLogo.Scale.x, 0.01f);

	ImGui::DragFloat3("color", &SprChoiceFrame.Color.x, 0.01f);

	ImGui::DragFloat("volume", &sandstormVolume, 0.01f);

	ImGui::End();
#endif
}

void SceneOver::Render(ID3D11DeviceContext* dc)
{
	//****************************************************************
	// 
	// ３D描画
	// 
	//****************************************************************
	camera->f_CameraUpdate(dc);
	directionLight->Update(dc);

	player->Render(dc);


	//****************************************************************
	// 
	// ２D描画
	// 
	//****************************************************************
	//font描画
	const wchar_t* Str_ReTry = L"リトライ";
	const wchar_t* Str_GoTitle = L"タイトルへ";

	const DirectX::XMFLOAT4 fontColor = { 0.5f,0.0f,0.0f,1.0f };

	font->Begin(dc);
	font->Draw({ 380.0f,410.0f }, StrReTryScale, fontColor, Str_ReTry, wcslen(Str_ReTry),Font::TEXT_ALIGN::UPPER_MIDDLE);
	font->Draw({ 380.0f,560.0f }, StrGoTitleScale, fontColor, Str_GoTitle, wcslen(Str_GoTitle), Font::TEXT_ALIGN::UPPER_MIDDLE);
	font->End(dc);


	SprChoiceFrame.Render(dc);
	SprLogo.Render(dc);

	EffectManager::Instance().Render(camera->GetView(), camera->GetProjection());

	SprBlackOut.Render(dc);
}



void SceneOver::ChoiceScene(float elapsedTime)
{
	//****************************************************************
	// 
	//  左右入力でシーンを選択
	// 
	//****************************************************************
	if (GamePad::Instance().GetAxisLY() < -0.6f) 
	{
		if(nextMenu != TITLE)
		{
			seSelect->Stop();
			seSelect->Play(false, OptionSystem::Instance().GetSeVolume());
			nextMenu = TITLE;
		}
	}
	else if (GamePad::Instance().GetAxisLY() > 0.6f) 
	{
		if(nextMenu != RETRY)
		{
			seSelect->Stop();
			seSelect->Play(false, OptionSystem::Instance().GetSeVolume());
			nextMenu = RETRY;
		}
	}

	constexpr float ReTryPosition{ 410.0f + 30.0f };
	constexpr float GoTitlePosition{ 560.0f + 30.0f };
	const float Speed{ 10.0f * elapsedTime };

	switch (nextMenu) {
	case RETRY:
		Ratio -= Speed;
		Ratio = std::max(0.0f, Ratio);
		break;
	case TITLE:
		Ratio += Speed;
		Ratio = std::min(1.0f, Ratio);
		break;
	default:;
	}

	SprChoiceFrame.Position.y = Calcu3D::LerpFloat(ReTryPosition, GoTitlePosition, Ratio);
	SprChoiceFrame.Scale.x = Calcu3D::LerpFloat(0.7f, 0.9f, Ratio);
	SprChoiceFrame.Scale.y = Calcu3D::LerpFloat(0.4f, 0.5f, Ratio);
	StrReTryScale.x = Calcu3D::LerpFloat(0.8f, 1.2f, 1.0f - Ratio);
	StrReTryScale.y = Calcu3D::LerpFloat(0.8f, 1.2f, 1.0f - Ratio);
	StrGoTitleScale.x = Calcu3D::LerpFloat(0.8f, 1.2f, Ratio);
	StrGoTitleScale.y = Calcu3D::LerpFloat(0.8f, 1.2f, Ratio);
}

void SceneOver::LogoUpdate(float elapsedTime)
{
	scrollTime += elapsedTime;
	if (scrollTime > 0.1f)
	{
		int r = rand() % 20;
		if (r >= 18)
		{
			SprLogo.TexPos.y = 128.0f;
			if(!playSE)
			{
				seSandstorm->Play(true, OptionSystem::Instance().GetSeVolume() * sandstormVolume);
				playSE = true;
			}
		}
		else if (r >= 15)
		{
			SprLogo.TexPos.y = 64.0f;
			if (!playSE)
			{
				seSandstorm->Play(true, OptionSystem::Instance().GetSeVolume() * sandstormVolume);
				playSE = true;
			}
		}
		else
		{
			SprLogo.TexPos.y = 0.0f;
			if (playSE)
			{
				seSandstorm->Stop();
				playSE = false;
			}
		}
		scrollTime = 0.0f;
	}
}
