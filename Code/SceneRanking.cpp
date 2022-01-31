#include"SceneRanking.h"
#include"SceneManager.h"
#include"SceneTitle.h"
#include"SceneLoading.h"
#include<fstream>

#include <imgui.h>

#include "User_Function.h"
#include "gAudio.h"

SceneRanking::SceneRanking()
{
}

SceneRanking::~SceneRanking()
{
}

void SceneRanking::Initialize(ID3D11Device* device)
{
    // 初期化
    font = std::make_unique<Font>(device, "./resources/Sprite/Font/Font.fnt", 2048);

    //Ship = std::make_unique<ClearShip>(device);
    //Ship->RankingInitialize();

    //Dome = std::make_unique<ClearSkyDome>(device);
    //camera = std::make_unique<Camera>(device);

    sprBack.Initialize(device, L"./resources/Sprite/UIs/ClearScoreBack.png");
    sprBack.Position = { 640.0f,370.0f };
    sprBack.Scale = { 2.4f,2.25f };
    sprBack.TexSize = { 440.0f,290.0f };
    sprBack.Pivot = { 220.0f,145.0f };

    sprRankingFont.Initialize(device, L"./resources/Sprite/UIs/RankingScene.png");
    sprRankingFont.Position = { 640.0f,360.0f };
    sprRankingFont.Scale = { 1.0f,1.0f };
    sprRankingFont.TexSize = { 1280.0f,720.0f };
    sprRankingFont.Pivot = { 640.0f,360.0f };

    sprBlackOut.Initialize(device, L"./resources/Sprite/UIs/Black.png");
    sprBlackOut.Position = { 640.0f,360.0f };
    sprBlackOut.Scale = { 1.0f,1.0f };
    sprBlackOut.TexSize = { 1280.0f,720.0f };
    sprBlackOut.Pivot = { 640.0f,360.0f };


    sprCircle[0].Initialize(device, L"./resources/Sprite/CircleAnimation/Circle0.png");
    sprCircle[0].Position = { 925.0f,560.0f };
    sprCircle[0].Scale = { 1.0f,1.0f };
    sprCircle[0].TexSize = { 640.0f,640.0f };
    sprCircle[0].Pivot = { 320.0f,320.0f };
    sprCircle[1].Initialize(device, L"./resources/Sprite/CircleAnimation/Circle1.png");
    sprCircle[1].Position = { 640.0f,640.0f };
    sprCircle[1].Scale = { 1.0f,1.0f };
    sprCircle[1].TexSize = { 640.0f,640.0f };
    sprCircle[1].Pivot = { 320.0f,320.0f };
    sprCircle[2].Initialize(device, L"./resources/Sprite/CircleAnimation/Circle2.png");
    sprCircle[2].Position = { 640.0f,640.0f };
    sprCircle[2].Scale = { 1.0f,1.0f };
    sprCircle[2].TexSize = { 640.0f,640.0f };
    sprCircle[2].Pivot = { 320.0f,320.0f };

    sprCircle[3].Initialize(device, L"./resources/Sprite/CircleAnimation/Circle1.png");
    sprCircle[3].Position = { 210.0f,130.0f };
    sprCircle[3].Scale = { 0.5f,0.5f };
    sprCircle[3].TexSize = { 640.0f,640.0f };
    sprCircle[3].Pivot = { 320.0f,320.0f };
    sprCircle[4].Initialize(device, L"./resources/Sprite/CircleAnimation/Circle2.png");
    sprCircle[4].Position = { 640.0f,640.0f };
    sprCircle[4].Scale = { 0.5f,0.5f };
    sprCircle[4].TexSize = { 640.0f,640.0f };
    sprCircle[4].Pivot = { 320.0f,320.0f };

    //sound
    bgm = Audio::Instance().LoadAudioSource("./resources/Sounds/BGM/SceneRanking.wav");
    bgm->Play(true, OptionSystem::Instance().GetBgmVolume() * 0.5f);
    seSelect = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/OperateSE.wav");
    seDecision = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/StartSE.wav");


    //font初期設定
    fontPos= { 925,620 };

    fontScale = { 0.4f,0.4f };



    stageIndex = 0;
    backTitle = false;
    blackOutRatio = 1.0f;

    scoreAlpha = 0.0f;


    OpenRanking();
}

void SceneRanking::Update(float elapsedTime)
{
    

    //camera->SetTarget({ 0.0f,10.0f,0.0f });
    //camera->SetAngle({ DirectX::XMConvertToRadians(-40.0f),0.0f,0.0f });
    //camera->Update(elapsedTime);
    //Ship->Update(elapsedTime);
    //Dome->Update(elapsedTime);


    inputInterval -= elapsedTime;
    InputUpdate();

    CircleAnimation(elapsedTime);
    CircleAnimationMini(elapsedTime);

    //ブラックアウト
    if (!backTitle)blackOutRatio -= elapsedTime;
    else blackOutRatio += elapsedTime;
	sprBlackOut.Color.w = Calcu3D::LerpFloat(sprBlackOut.Color.w, blackOutRatio, blackOutRatio);
    if (blackOutRatio > 1.0f)
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle()));
        return;
    }

   
    scoreAlpha += elapsedTime * 1.5f;
    scoreAlpha = std::min(scoreAlpha, 1.0f);


#ifdef _DEBUG
    ImGui::Begin("Ranking");

    ImGui::DragFloat2("sprBack.Scale", &sprBack.Scale.x, 0.01f);

    ImGui::DragFloat2("fontPos", &fontPos.x);

    ImGui::DragFloat2("fontScale", &fontScale.x, 0.01f);

    ImGui::DragFloat2("CirclePos", &sprCircle[0].Position.x);
    sprCircle[1].Position = sprCircle[0].Position;
    sprCircle[2].Position = sprCircle[0].Position;
    ImGui::DragFloat("CircleScale", &sprCircle[0].Scale.x,0.01f);
    sprCircle[0].Scale.y = sprCircle[0].Scale.x;

    ImGui::DragFloat2("CirclePosMini", &sprCircle[3].Position.x);
    sprCircle[4].Position = sprCircle[3].Position;
    ImGui::DragFloat("CircleScaleMini", &sprCircle[3].Scale.x,0.01f);
    sprCircle[3].Scale.y = sprCircle[3].Scale.x;



    ImGui::End();


#endif
}

void SceneRanking::Render(ID3D11DeviceContext* dc)
{
    
    //camera->f_CameraUpdate(dc);

    //Ship->Render(dc);
    //Dome->Render(dc);

    //****************************************************************
    // 
    // 以下２D描画
    // 
    //****************************************************************

    const std::wstring FirstScore = to_wstring(ScoreArray[stageIndex][0]);
    const wchar_t* StrFirstScore = FirstScore.c_str();

    const std::wstring SecondScore = to_wstring(ScoreArray[stageIndex][1]);
    const wchar_t* StrSecondScore = SecondScore.c_str();

    const std::wstring ThirdScore = to_wstring(ScoreArray[stageIndex][2]);
    const wchar_t* StrThirdScore = ThirdScore.c_str();

    const std::wstring FourthScore = to_wstring(ScoreArray[stageIndex][3]);
    const wchar_t* StrFourthScore = FourthScore.c_str();

    const std::wstring FifthScore = to_wstring(ScoreArray[stageIndex][4]);
    const wchar_t* StrFifthScore = FifthScore.c_str();

    const std::wstring StageIndex = to_wstring(stageIndex+1);
    const wchar_t* StrStageIndex = StageIndex.c_str();

    const wchar_t* StrBack = L"Qキー/Backボタン : タイトルに戻る";

    //sprBack.Render(dc);
    sprCircle[1].Position = sprCircle[0].Position;
    sprCircle[2].Position = sprCircle[0].Position;
    sprCircle[4].Position = sprCircle[3].Position;
    sprCircle[2].Scale = sprCircle[0].Scale;
    sprCircle[0].Render(dc);
    sprCircle[1].Render(dc);
    sprCircle[2].Render(dc);
    sprCircle[3].Render(dc);
    sprCircle[4].Render(dc);

    sprRankingFont.Render(dc);

    font->Begin(dc);

    font->Draw({ 770,215 }, { 1.3f,1.3f }, { 0.0f,0.88f, 0.8f, scoreAlpha }, StrFirstScore, wcslen(StrFirstScore), Font::TEXT_ALIGN::UPPER_RIGHT);
    font->Draw({ 770,310 }, { 1.2f,1.2f }, { 0.0f,0.88f, 0.8f, scoreAlpha }, StrSecondScore, wcslen(StrSecondScore), Font::TEXT_ALIGN::UPPER_RIGHT);
	font->Draw({ 770,405 }, { 1.1f,1.1f }, { 0.0f,0.88f, 0.8f, scoreAlpha }, StrThirdScore, wcslen(StrThirdScore), Font::TEXT_ALIGN::UPPER_RIGHT);
	font->Draw({ 770,475 }, { 1.0f,1.0f }, { 0.0f,0.88f, 0.8f, scoreAlpha }, StrFourthScore, wcslen(StrFourthScore), Font::TEXT_ALIGN::UPPER_RIGHT);
	font->Draw({ 770,540 }, { 1.0f,1.0f }, { 0.0f,0.88f, 0.8f, scoreAlpha }, StrFifthScore, wcslen(StrFifthScore), Font::TEXT_ALIGN::UPPER_RIGHT);


	font->Draw({ 810,115 }, { 0.8f,1.0f }, { 0.0f,0.88f,0.8f,scoreAlpha }, StrStageIndex, wcslen(StrStageIndex), Font::TEXT_ALIGN::UPPER_MIDDLE);

	font->Draw({ 925,620 }, {0.4f,0.4f}, { 0.0f,0.88f,0.8f,1.0f }, StrBack, wcslen(StrBack), Font::TEXT_ALIGN::UPPER_MIDDLE);

    font->End(dc);

    sprBlackOut.Render(dc);
}

void SceneRanking::Finalize()
{
    bgm->Release();
    seSelect->Release();
    seDecision->Release();
}

void SceneRanking::OpenRanking()
{
    int index = -1;
    // ファイルを開く
    ifstream ifs;

    //stage1
    index = 0;
    {
        ifs.open("./resources/Files/Score0.dat");
        if (ifs)
        {
            ifs.read(reinterpret_cast<char*>(&ScoreArray[index]), sizeof(int) * ARRAYSIZE(ScoreArray[index]));
            ifs.close();
        }
        else
        {
            for (int k = 0; k < 6; k++)
            {
                ScoreArray[index][k] = { 0 };
            }
        }
        ofstream ofs;
        ofs.open("./resources/Files/Score0.dat");
        if (ofs)
        {
            ofs.write(reinterpret_cast<char*>(&ScoreArray[index]), sizeof(int) * ARRAYSIZE(ScoreArray[index]));
            ofs.close();
        }

    }

    //stage2
    index = 1;
    {
        ifs.open("./resources/Files/Score1.dat");
        if (ifs)
        {
            ifs.read(reinterpret_cast<char*>(&ScoreArray[index]), sizeof(int) * ARRAYSIZE(ScoreArray[index]));
            ifs.close();
        }
        else
        {
            for (int k = 0; k < 6; k++)
            {
                ScoreArray[index][k] = { 0 };
            }
        }
        ofstream ofs;
        ofs.open("./resources/Files/Score1.dat");
        if (ofs)
        {
            ofs.write(reinterpret_cast<char*>(&ScoreArray[index]), sizeof(int) * ARRAYSIZE(ScoreArray[index]));
            ofs.close();
        }

    }

    //stage3
    index = 2;
    {
        ifs.open("./resources/Files/Score2.dat");
        if (ifs)
        {
            ifs.read(reinterpret_cast<char*>(&ScoreArray[index]), sizeof(int) * ARRAYSIZE(ScoreArray[index]));
            ifs.close();
        }
        else
        {
            for (int k = 0; k < 6; k++)
            {
                ScoreArray[index][k] = { 0 };
            }
        }
        ofstream ofs;
        ofs.open("./resources/Files/Score2.dat");
        if (ofs)
        {
            ofs.write(reinterpret_cast<char*>(&ScoreArray[index]), sizeof(int) * ARRAYSIZE(ScoreArray[index]));
            ofs.close();
        }

    }

    //stage4
    index = 3;
    {
        ifs.open("./resources/Files/Score3.dat");
        if (ifs)
        {
            ifs.read(reinterpret_cast<char*>(&ScoreArray[index]), sizeof(int) * ARRAYSIZE(ScoreArray[index]));
            ifs.close();
        }
        else
        {
            for (int k = 0; k < 6; k++)
            {
                ScoreArray[index][k] = { 0 };
            }
        }
        ofstream ofs;
        ofs.open("./resources/Files/Score3.dat");
        if (ofs)
        {
            ofs.write(reinterpret_cast<char*>(&ScoreArray[index]), sizeof(int) * ARRAYSIZE(ScoreArray[index]));
            ofs.close();
        }

    }

    //stage5
    index = 4;
    {
        ifs.open("./resources/Files/Score4.dat");
        if (ifs)
        {
            ifs.read(reinterpret_cast<char*>(&ScoreArray[index]), sizeof(int) * ARRAYSIZE(ScoreArray[index]));
            ifs.close();
        }
        else
        {
            for (int k = 0; k < 6; k++)
            {
                ScoreArray[index][k] = { 0 };
            }
        }
        ofstream ofs;
        ofs.open("./resources/Files/Score4.dat");
        if (ofs)
        {
            ofs.write(reinterpret_cast<char*>(&ScoreArray[index]), sizeof(int) * ARRAYSIZE(ScoreArray[index]));
            ofs.close();
        }

    }

}

void SceneRanking::InputUpdate()
{
    if (inputInterval > 0.0f || backTitle)return;
    auto gamepad = GamePad::Instance();
    if (gamepad.GetAxisLX() > 0.5f || gamepad.GetButton() & GamePad::BTN_RIGHT)//right
    {
        if (stageIndex < 4)
        {
            stageIndex++;
            scoreAlpha = 0.0f;

            seSelect->Stop();
            seSelect->Play(false, OptionSystem::Instance().GetSeVolume());
        }
        inputInterval = MaxInputInterval;
    }
    if (gamepad.GetAxisLX() < -0.5f || gamepad.GetButton() & GamePad::BTN_LEFT)//left
    {
        if (stageIndex > 0)
        {
            stageIndex--;
            scoreAlpha = 0.0f;

            seSelect->Stop();
            seSelect->Play(false, OptionSystem::Instance().GetSeVolume());
        }
        inputInterval = MaxInputInterval;
    }


    if (gamepad.GetButtonDown() & GamePad::BTN_BACK)
    {
        blackOutRatio = 0.0f;
        backTitle = true;

        seDecision->Stop();
        seDecision->Play(false, OptionSystem::Instance().GetSeVolume());
    }
}

void SceneRanking::CircleAnimation(float elapsedTime)
{
    for (int i = 0;i < 3;i++)
    {
        AnimParam& param = Circles[i];

        param.timer += elapsedTime;

        if(param.state == 0)
        {
            param.maxTimer = 3.0f + (static_cast<float>(rand()) / 32767.0f * 3.0f);
            param.beginAngle = sprCircle[i].Angle;
            param.targetAngle = (static_cast<float>(rand() % 60)) * 12.0f - 360.0f;//degree

            if (i == 0)
            {
                param.targetScale = static_cast<float>(rand()) / 32767.0f * 2.0f / 10.0f + 1.0f;
            }

            param.timer = 0.0f;
            param.state++;
        }
        else
        {
            param.ratio = param.timer / param.maxTimer;
            sprCircle[i].Angle = Calcu3D::LerpFloat(param.beginAngle, param.targetAngle, param.ratio);
            sprCircle[i].Scale = Calcu3D::LerpFloat2(sprCircle[i].Scale, { param.targetScale,param.targetScale }, param.ratio * 0.75f);

            if(param.timer > param.maxTimer)
            {
                param.state = 0;
            }
        }
    }
}
void SceneRanking::CircleAnimationMini(float elapsedTime)
{
    for (int i = 0; i < 2; i++)
    {
        AnimParam& param = CirclesMini[i];

        param.timer += elapsedTime;

        if (param.state == 0)
        {
            param.maxTimer = 5.0f + (static_cast<float>(rand()) / 32767.0f * 5.0f);
            param.beginAngle = sprCircle[i+3].Angle;
            param.targetAngle = (static_cast<float>(rand() % 60)) * 12.0f - 360.0f;//degree

            param.targetScale = static_cast<float>(rand()) / 32767.0f * 2.5f / 10.0f + 0.5f;
            

            param.timer = 0.0f;
            param.state++;
        }
        else
        {
            param.ratio = param.timer / param.maxTimer;
            sprCircle[i+3].Angle = Calcu3D::LerpFloat(param.beginAngle, param.targetAngle, param.ratio);
            sprCircle[i+3].Scale = Calcu3D::LerpFloat2(sprCircle[i+3].Scale, { param.targetScale,param.targetScale }, param.ratio * 0.75f);

            if (param.timer > param.maxTimer)
            {
                param.state = 0;
            }
        }
    }
}
