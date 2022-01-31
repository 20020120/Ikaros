#include "SceneClear.h"

#include "EnemyManager.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTitle.h"
#include "SceneGame.h"

#include"MathSupport.h"
#include "User_Function.h"
#include "gAudio.h"
#include "OptionSystem.h"

SceneClear::SceneClear()
{
	
}
SceneClear::~SceneClear()
{
	
}

void SceneClear::Initialize(ID3D11Device* device)
{
	font = std::make_unique<Font>(device, "./resources/Sprite/Font/Font.fnt", 2048);

	sprChoiceFrame.Initialize(device, L"./resources/Sprite/Option/SettingFrame.png");
	sprChoiceFrame.Position = { 350.0f,650.0f };
	sprChoiceFrame.Angle = { 0.0f };
	sprChoiceFrame.Color = { 1.0f,1.0f,1.0f,1.0f };
	sprChoiceFrame.Scale = { 0.6f,0.5f };
	sprChoiceFrame.TexPos = { 0.0f,0.0f };
	sprChoiceFrame.TexSize = { 440.0f,260.0f };
	sprChoiceFrame.Pivot = sprChoiceFrame.TexSize * 0.5f;

	// スコア背景初期化
	sprScoreBack.Initialize(device, L"./resources/Sprite/UIs/ClearScoreBack.png");
	sprScoreBack.Position = { 10.0f,190.0f };
	sprScoreBack.Scale = { 1.5f,1.3f };
	sprScoreBack.TexSize = { 440.0f,290.0f };

	sprClearLogo.Initialize(device, L"./resources/Sprite/CutIn/clear_logo.png");
	sprClearLogo.TexSize = { 512.0f,64.0f };
	sprClearLogo.Position = { 150.0f, 80.0f };
	sprClearLogo.Scale = { 1.5f, 1.5f };

	sprScoreRank.Initialize(device, L"./resources/Sprite/UIs/Rank.png");
	sprScoreRank.Position = { 550.0f,340.0f };
	sprScoreRank.TexSize = { 256.0f,256.0f };
	sprScoreRank.Pivot = { 128.0f,128.0f };
	sprScoreRank.Scale = { 1.0f,1.0f };
	sprScoreRank.Color.w = 0.0f;

	sprLine.Initialize(device, L"./resources/Sprite/UIs/Line.png", 4);
	sprLine.TexSize = { 200,50 };
	sprLine.Pivot = { 100,25 };
	linePos[0] = { 110,270 };
	linePos[1] = { 115,330 };
	linePos[2] = { 115,390 };
	linePos[3] = { 125,490 };
	lineScale[0] = { 0.75f, 0.75f };
	lineScale[1] = { 0.85f, 0.75f };
	lineScale[2] = { 0.85f, 0.75f };
	lineScale[3] = { 0.8f, 0.75f };

	//****************************************************************
	// 
    // スコア初期化(今は仮)
	// 
	//****************************************************************
	ClearTime = static_cast<int>(GameSystem::Instance().GetClearTimer());

	MaxCombo = GameSystem::Instance().GetMaxCombo();
	BreakEnemyValue = GameSystem::Instance().GetNowKilledSize();

	// スコアを計算
	// 時間を計算
	int TimeCoe = (300 - ClearTime) / 10;
	TimeCoe = (std::max)(1, TimeCoe);
	// 撃破数を計算
	int EnemyCoe = BreakEnemyValue;
	// コンボ
	int ComboCoe = MaxCombo / 10;
	ComboCoe = (std::max)(1, ComboCoe);

	// スコアを計算
	Score = EnemyCoe * ComboCoe * TimeCoe;

	CalcRank();

	UpdateRanking();

	// スコアによって評価を変える
	sprScoreRank.TexPos.x = Rank * 256.0f;


	// 最小値が０になるようにする
	Score = (std::max)(Score, 0);


	ScoreRatio = 0.0f;
	CurrentScore = 0;

	ratio = 0.0f;

	//天球
	skydome = std::make_unique<ClearSkyDome>(device);

	camera = std::make_unique<Camera>(device);
	directionLight = std::make_unique<DirectionLight>(device);

	player = std::make_unique<ClearPlayer>(device);
	ship = std::make_unique<ClearShip>(device);



	//sounds
	bgm = Audio::Instance().LoadAudioSource("./resources/Sounds/BGM/SceneClear.wav");
	bgm->Play(true, OptionSystem::Instance().GetBgmVolume());
	seSelect = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/OperateSE.wav");
	seDicision = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/Drumroll.wav");
	seDrumroll = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/OperateSE.wav");
	seAnnounce[0] = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/OnePoint1.wav");
	seAnnounce[1] = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/OnePoint.wav");
	seRank[0] = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/RankS.wav");
	seRank[1] = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/RankS.wav");
	seRank[2] = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/RankB.wav");

	announcementState = 0;
	announcement = true;
	announcementTimer = 0.0f;

	breakEnemyScale = { 0.0f,0.0f };
	maxComboScale = { 0.0f,0.0f };
	clearTimeScale = { 0.0f,0.0f };
	scoreScale = { 0.0f,0.0f };

	highScoreAlpha = 0.0f;


	directionLight->SetDirection({ 0.454f,-0.870f,0.192f });
	// 向きの初期化
	GameSystem::Instance().SetCameraDirection(GameSystem::CAMERA_DIRECTION::SIDE);
}

void SceneClear::Finalize()
{
	//sound解放
	bgm->Release();
	seDicision->Release();
	seSelect->Release();
	seDrumroll->Release();
	seAnnounce[0]->Release();
	seAnnounce[1]->Release();
	seRank[0]->Release();
	seRank[1]->Release();
	seRank[2]->Release();

	EnemyManager::Instance().ResetSpawnTimer();
}

void SceneClear::Update(float elapsedTime)
{

	//bgm->SetVolume(OptionSystem::Instance().GetBgmVolume());

	camera->SetTarget(player->t.Position);
	//camera->SetAngle({ 0.0f,0.0f,0.0f });

	player->Update(elapsedTime);
	ship->Update(elapsedTime);
	skydome->Update(elapsedTime);


	//スコア発表
	ScoreAnnouncement(elapsedTime);

	//ハイスコアのフォント色変える
	UpdateHighScoreFont(elapsedTime);

	ChoiceScene(elapsedTime);

	//シーン遷移
	if (announcement)return;

	const GamePadButton startButton = GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y;
	if (GamePad::Instance().GetButtonDown() & startButton)
	{
		seDicision->Play(false, OptionSystem::Instance().GetSeVolume() * 1.5f);
		switch (nextMenu)
		{
		case RETRY:

		{
			EnemyManager::STAGE stage = EnemyManager::Instance().GetStageIndex();

			if (stage != EnemyManager::STAGE::STAGE5)
			{
				// 一つ上のステージへ
				stage = static_cast<EnemyManager::STAGE>(static_cast<int>(stage) + 1);

				EnemyManager::Instance().SetStageIndex(stage);
			}
		}

		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame()));

		break;
		case TITLE:
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle()));
			break;
		default:;
		}
	}


#ifdef _DEBUG
	ImGui::Begin("Clear");
	ImGui::DragFloat2("rankPosition", &sprScoreRank.Position.x);
	ImGui::DragFloat("announcementTimer", &announcementTimer);

	ImGui::DragFloat2("linePos0", &linePos[0].x);
	ImGui::DragFloat2("linePos1", &linePos[1].x);
	ImGui::DragFloat2("linePos2", &linePos[2].x);
	ImGui::DragFloat2("linePos3", &linePos[3].x);
	ImGui::DragFloat2("lineScale0", &lineScale[0].x, 0.01f);
	ImGui::DragFloat2("lineScale1", &lineScale[1].x, 0.01f);
	ImGui::DragFloat2("lineScale2", &lineScale[2].x, 0.01f);
	ImGui::DragFloat2("lineScale3", &lineScale[3].x, 0.01f);

	int s = static_cast<int>(EnemyManager::Instance().GetStageIndex());
	ImGui::DragInt("enemy", &s);

	ImGui::End();


#endif

}

void SceneClear::Render(ID3D11DeviceContext* dc)
{
	//****************************************************************
	// 
	// ３D描画
	// 
	//****************************************************************
	camera->f_CameraUpdate(dc);
	directionLight->Update(dc);

	player->Render(dc);
	ship->Render(dc);

	skydome->Render(dc);

	//****************************************************************
	// 
	// 2D描画
	// 
	//****************************************************************

	// スコアの背景描画
	sprScoreBack.Render(dc);

	//font描画
	const wchar_t* StrNext = L"次へ";
	const wchar_t* StrReTry = L"リトライ";
	const wchar_t* StrGoTitle = L"タイトルへ";
	const wchar_t* StrClear = L"Game Clear!";
	const wchar_t* StrScore = L"スコア";
	const wchar_t* StrBreakEnemy = L"敵撃破数";
	const wchar_t* StrMaxComb = L"最大コンボ";
	const wchar_t* StrClearTime = L"クリア時間";
	const wchar_t* StrHighScore = L"-High Score!-";

    const std::wstring EnemyValue = to_wstring(BreakEnemyValue);
	const wchar_t* StrBreakEnemyValue = EnemyValue.c_str();

	const std::wstring ComboValue = to_wstring(MaxCombo);
	const wchar_t* StrMaxComboValue = ComboValue.c_str();

	const std::wstring ClearTimeValue = to_wstring(ClearTime);
	const wchar_t* StrClearTimeValue = ClearTimeValue.c_str();

	const std::wstring ScoreValue = to_wstring(CurrentScore);
	const wchar_t* StrScoreValue = ScoreValue.c_str();

	font->Begin(dc);
	// 撃破数
	font->Draw({ 70.0f,250.0f }, {0.6f,0.6f}, { 1.0f,1.0f,1.0f,1.0f }, StrBreakEnemy, wcslen(StrBreakEnemy), Font::TEXT_ALIGN::UPPER_LEFT);
	font->Draw({ 355.0f,245.0f }, breakEnemyScale, { 1.0f,1.0f,1.0f,1.0f }, StrBreakEnemyValue, wcslen(StrBreakEnemyValue), Font::TEXT_ALIGN::UPPER_RIGHT);

	// 最大コンボ
	font->Draw({ 70.0f,310.0f }, { 0.6f,0.6f }, { 1.0f,1.0f,1.0f,1.0f }, StrMaxComb, wcslen(StrMaxComb), Font::TEXT_ALIGN::UPPER_LEFT);
	font->Draw({ 355.0f,305.0f }, maxComboScale, { 1.0f,1.0f,1.0f,1.0f }, StrMaxComboValue, wcslen(StrMaxComboValue), Font::TEXT_ALIGN::UPPER_RIGHT);

	// クリアタイム
	font->Draw({ 70.0f,370.0f }, { 0.6f,0.6f }, { 1.0f,1.0f,1.0f,1.0f }, StrClearTime, wcslen(StrClearTime), Font::TEXT_ALIGN::UPPER_LEFT);
	font->Draw({ 355.0f,365.00f }, clearTimeScale, { 1.0f,1.0f,1.0f,1.0f }, StrClearTimeValue, wcslen(StrClearTimeValue), Font::TEXT_ALIGN::UPPER_RIGHT);

	// スコア
	font->Draw({ 140.0f,460.0f }, { 0.8f,0.8f }, { 1.0f,1.0f,1.0f,1.0f }, StrScore, wcslen(StrScore), Font::TEXT_ALIGN::UPPER_MIDDLE);
	font->Draw({ 375.0f,455.0f }, scoreScale, { 1.0f,1.0f,1.0f,1.0f }, StrScoreValue, wcslen(StrScoreValue), Font::TEXT_ALIGN::UPPER_RIGHT);

	//ハイスコア!
	font->Draw({ 380.0f,435.0f }, {0.5f,0.5f}, { highScoreColorR,1.0f,1.0f,highScoreAlpha }, StrHighScore, wcslen(StrHighScore), Font::TEXT_ALIGN::UPPER_RIGHT);

	// リトライか次へか
	if(EnemyManager::Instance().GetStageIndex() != EnemyManager::STAGE::STAGE5)
	{
		font->Draw({ 160.0f,620.0f }, reTryScale, { 1.0f,1.0f,1.0f,1.0f }, StrNext, wcslen(StrNext), Font::TEXT_ALIGN::UPPER_MIDDLE);
	}
	else
	{
		font->Draw({ 160.0f,620.0f }, reTryScale, { 1.0f,1.0f,1.0f,1.0f }, StrReTry, wcslen(StrReTry), Font::TEXT_ALIGN::UPPER_MIDDLE);
	}
	

    font->Draw({ 510.0f,620.0f }, goTitleScale, { 1.0f,1.0f,1.0f,1.0f }, StrGoTitle, wcslen(StrGoTitle), Font::TEXT_ALIGN::UPPER_MIDDLE);

	font->End(dc);

	sprClearLogo.Render(dc);
	sprChoiceFrame.Render(dc);
	sprScoreRank.Render(dc);

	for (int i = 0; i < 4; i++)
	{
		sprLine.Position = linePos[i];
		sprLine.Scale = lineScale[i];
		sprLine.Render(dc);
	}
	
}

void SceneClear::ChoiceScene(float elapsedTime)
{
	if (!announcement)
	{
		if (GamePad::Instance().GetAxisLX() < -0.6f) // 右
		{
			if (nextMenu != RETRY)
			{
				seSelect->Stop();
				seSelect->Play(false, OptionSystem::Instance().GetSeVolume());
				nextMenu = RETRY;
			}
		}
		else if (GamePad::Instance().GetAxisLX() > 0.6f) // 左
		{
			if (nextMenu != TITLE)
			{
				seSelect->Stop();
				seSelect->Play(false, OptionSystem::Instance().GetSeVolume());
				nextMenu = TITLE;
			}
		}
	}

	constexpr float retryPosition{ 160.0f };
	constexpr float goTitlePosition{ 510.0f };
	const float Speed{ 10.0f * elapsedTime };

	switch (nextMenu) {
	case RETRY:
		ratio -= Speed;
		ratio = std::max(0.0f, ratio);
		break;
	case TITLE:
		ratio += Speed;
		ratio = std::min(1.0f, ratio);
		break;
	default:;
	}

	sprChoiceFrame.Position.x = Calcu3D::LerpFloat(retryPosition, goTitlePosition, ratio);
	sprChoiceFrame.Scale.x = Calcu3D::LerpFloat(0.75f, 0.9f, ratio);
	sprChoiceFrame.Scale.y = Calcu3D::LerpFloat(0.4f, 0.4f, ratio);
	reTryScale.x = Calcu3D::LerpFloat(0.8f, 1.2f, 1.0f - ratio);
	reTryScale.y = Calcu3D::LerpFloat(0.8f, 1.2f, 1.0f - ratio);
	goTitleScale.x = Calcu3D::LerpFloat(0.8f, 1.2f, ratio);
	goTitleScale.y = Calcu3D::LerpFloat(0.8f, 1.2f, ratio);
}

void SceneClear::ScoreAnnouncement(float elapsedTime)
{
	announcementTimer += elapsedTime;
	float seVolume = 1.0f;

	switch(announcementState)
	{
	case 0://待ち
	{
		if(announcementTimer > 0.5f)
		{
			announcementTimer = 0.0f;
			announcementState++;
		}
		break;
	}
	case 1:// 撃破数
	{
		const DirectX::XMFLOAT2 scale{ 0.7f,0.7f };
		breakEnemyScale = Calcu3D::LerpFloat2(breakEnemyScale, scale, announcementTimer);

		if(announcementTimer> 0.2f)
		{
			breakEnemyScale = scale;

			announcementTimer = 0.0f;
			announcementState++;
			seAnnounce[0]->Stop();
			seAnnounce[0]->Play(false, OptionSystem::Instance().GetSeVolume() * seVolume);
		}

		break;
	}
	case 2://待ち
	{
		if (announcementTimer > 0.5f)
		{
			announcementTimer = 0.0f;
			announcementState++;
		}
		break;
	}
	case 3://最大コンボ
	{
		const DirectX::XMFLOAT2 scale{ 0.7f,0.7f };
		maxComboScale = Calcu3D::LerpFloat2(maxComboScale, scale, announcementTimer);

		if (announcementTimer > 0.0f)
		{
			maxComboScale = scale;

			announcementTimer = 0.0f;
			announcementState++;

			seAnnounce[0]->Stop();
			seAnnounce[0]->Play(false, OptionSystem::Instance().GetSeVolume() * seVolume);
		}

		break;
	}
	case 4://待ち
	{
		if (announcementTimer > 0.5f)
		{
			announcementTimer = 0.0f;
			announcementState++;
		}
		break;
	}
	case 5://クリア時間
	{
		const DirectX::XMFLOAT2 scale{ 0.7f,0.7f };
		clearTimeScale = Calcu3D::LerpFloat2(clearTimeScale, scale, announcementTimer);

		if (announcementTimer > 0.2f)
		{
			clearTimeScale = scale;

			announcementTimer = 0.0f;
			announcementState++;

			seAnnounce[0]->Stop();
			seAnnounce[0]->Play(false, OptionSystem::Instance().GetSeVolume() * seVolume);

			//seDrumroll->Play(false, OptionSystem::Instance().GetSeVolume() * 3.0f);
		}

		break;
	}
	case 6://待ち
	{
		if (announcementTimer > 0.5f)
		{
			announcementTimer = 0.0f;
			announcementState++;
		}
		break;
	}
	case 7://スコア
	{
		const DirectX::XMFLOAT2 scale{ 0.8f,0.8f };
		scoreScale = Calcu3D::LerpFloat2(scoreScale, scale, announcementTimer * 5.0f);

		// スコアを補完
		ScoreRatio += elapsedTime * 0.5f;
		ScoreRatio = (std::min)(1.0f, ScoreRatio);
		CurrentScore = static_cast<int>(Calcu3D::LerpFloat(0, static_cast<float>(Score), ScoreRatio));

		if (announcementTimer > 0.9f)
		{
			scoreScale = { 0.9f,0.9f };

			CurrentScore = Score;

			announcementTimer = 0.0f;
			announcementState++;

			//seDrumroll->Stop();
			seAnnounce[1]->Stop();
			seAnnounce[1]->Play(false, OptionSystem::Instance().GetSeVolume() * 0.75f);
		}

		break;
	}
	case 8://待ち
	{
		if (highScore)
			highScoreAlpha = Calcu3D::LerpFloat(highScoreAlpha, 1.0f, announcementTimer * 10.0f);

		if (announcementTimer > 0.5f)
		{
			announcementTimer = 0.0f;
			announcementState++;
		}
		break;
	}
	case 9://ランク表示
	{
		constexpr DirectX::XMFLOAT2 pos = { 500.0f,370.0f };

		sprScoreRank.Position = Calcu3D::LerpFloat2(sprScoreRank.Position, pos, announcementTimer );
		sprScoreRank.Color.w = Calcu3D::LerpFloat(sprScoreRank.Color.w, 1.0f, announcementTimer);

		if (announcementTimer > 1.0f)
		{
			sprScoreRank.Position = pos;
			sprScoreRank.Color.w = 1.0f;

			announcementTimer = 0.0f;
			announcementState++;

			announcement = false;

			seRank[static_cast<int>(Rank)]->Stop();
			seRank[static_cast<int>(Rank)]->Play(false, OptionSystem::Instance().GetSeVolume());

		}

		break;
	}
	}

	

	//スキップ
	const GamePadButton startButton = GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y;
	if (GamePad::Instance().GetButtonDown() & startButton)
	{
		breakEnemyScale = { 0.7f,0.7f };
		maxComboScale = { 0.7f,0.7f };
		clearTimeScale = { 0.7f,0.7f };

		if(announcementState < 6)
		{
			announcementState = 6;
			announcementTimer = 0.0f;

			seAnnounce[1]->Stop();
			seAnnounce[1]->Play(false, OptionSystem::Instance().GetSeVolume());
		}
		else if(announcementState < 8)//スコア発表スキップ
		{
			CurrentScore = Score;

			announcementState = 8;
			announcementTimer = 0.0f;

			seAnnounce[1]->Stop();
			seAnnounce[1]->Play(false, OptionSystem::Instance().GetSeVolume());

			scoreScale = { 1.0f,1.0f };

		}
		else if(announcementState == 9)
		{
			sprScoreRank.Position = { 500.0f,370.0f };
			sprScoreRank.Color.w = 1.0f;

			announcementTimer = 0.0f;
			announcement = false;

			seRank[static_cast<int>(Rank)]->Stop();
			seRank[static_cast<int>(Rank)]->Play(false, OptionSystem::Instance().GetSeVolume());

		}
	}
	

}


void SceneClear::UpdateHighScoreFont(float elapsedTime)
{
	if (announcement)return;

	switch(announcementState)
	{
	case 10:
	{
		if (announcementTimer > 3.0f)
		{
			announcementTimer = 0.0f;
			announcementState++;
		}
		break;
	}
	case 11:
	{
		highScoreColorR = Calcu3D::LerpFloat(highScoreColorR, 0.75f, announcementTimer * 0.5f);
		if (announcementTimer > 0.75)
		{
			announcementTimer = 0.0f;
			announcementState++;
		}
		break;
	}
	case 12:
	{
		highScoreColorR = Calcu3D::LerpFloat(highScoreColorR, 0.0f, announcementTimer * 0.5f);
		if (announcementTimer > 0.75f)
		{
			announcementTimer = 0.0f;
			announcementState = 10;
		}
		break;
	}
	}
}



void SceneClear::UpdateRanking()
{
	const int stageNum = static_cast<int>(EnemyManager::Instance().GetStageIndex());
	
	// ファイルを開く
	ifstream ifs;
	switch(stageNum)
	{
	case 0:
		ifs.open("./resources/Files/Score0.dat");
		break;
	case 1:
		ifs.open("./resources/Files/Score1.dat");
		break;
	case 2:
		ifs.open("./resources/Files/Score2.dat");
		break;
	case 3:
		ifs.open("./resources/Files/Score3.dat");
		break;
	case 4:
		ifs.open("./resources/Files/Score4.dat");
		break;
	}

	if (ifs)
	{
		ifs.read(reinterpret_cast<char*>(&ScoreArray), ARRAYSIZE(ScoreArray) * sizeof(int));
		ifs.close();
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			ScoreArray[i] = { 0 };
		}
	}

	//ハイスコア更新したかどうか
	if (ScoreArray[0] < Score)
	{
		highScore = true;
	}

	// 最下位のスコアを更新する
	ScoreArray[5] = Score;

	// 大きい順にソート
	std::sort(ScoreArray, std::end(ScoreArray), std::greater<int>());

	ofstream ofs;
	switch (stageNum)
	{
	case 0:
		ofs.open("./resources/Files/Score0.dat");
		break;
	case 1:
		ofs.open("./resources/Files/Score1.dat");
		break;
	case 2:
		ofs.open("./resources/Files/Score2.dat");
		break;
	case 3:
		ofs.open("./resources/Files/Score3.dat");
		break;
	case 4:
		ofs.open("./resources/Files/Score4.dat");
		break;
	}

	if(ofs)
	{
		ofs.write(reinterpret_cast<char*>(&ScoreArray), sizeof(int) * ARRAYSIZE(ScoreArray));
		ofs.close();
	}

}

void SceneClear::CalcRank()
{
	const int stageNum = static_cast<int>(EnemyManager::Instance().GetStageIndex());
	switch (stageNum)
	{
	case 0:
	{
		if (Score >= 2500)//S
		{
			Rank = static_cast<float>(RANK::S_RANK);
		}
		else if (Score >= 1000)//A
		{
			Rank = static_cast<float>(RANK::A_RANK);
		}
		else//B
		{
			Rank = (float)RANK::B_RANK;
		}
		Score = 5000 + Score * 2;
		break;
	}
	case 1:
	{
		if (Score >= 800)//S
		{
			Rank = static_cast<float>(RANK::S_RANK);
		}
		else if (Score >= 400)//A
		{
			Rank = static_cast<float>(RANK::A_RANK);
		}
		else//B
		{
			Rank = (float)RANK::B_RANK;
		}
		Score = 3000 + Score * 10;
		break;
	}
	case 2:
	{
		if (Score >= 3000)//S
		{
			Rank = static_cast<float>(RANK::S_RANK);
		}
		else if (Score >= 600)//A
		{
			Rank = static_cast<float>(RANK::A_RANK);
		}
		else//B
		{
			Rank = (float)RANK::B_RANK;
		}
		Score = 3000 + Score * 2;
		break;
	}
	case 3:
	{
		if (Score >= 5500)//S
		{
			Rank = static_cast<float>(RANK::S_RANK);
		}
		else if (Score >= 1000)//A
		{
			Rank = static_cast<float>(RANK::A_RANK);
		}
		else//B
		{
			Rank = (float)RANK::B_RANK;
		}
		Score = 1500 + Score * 2;
		break;
	}
	case 4:
	{
		if (Score >= 1000)//S
		{
			Rank = static_cast<float>(RANK::S_RANK);
		}
		else if (Score >= 500)//A
		{
			Rank = static_cast<float>(RANK::A_RANK);
		}
		else//B
		{
			Rank = (float)RANK::B_RANK;
		}
		Score = 8000 + Score * 10;
		break;
	}
	}
}