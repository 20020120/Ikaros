#include "Option.h"
#include<imgui.h>
#include "User_Function.h"
#include "gAudio.h"


Option::Option(ID3D11Device* device, MODE mode)
{
	//フォント
	font = std::make_unique<Font>(device, "./resources/Sprite/Font/Font.fnt", 2048);

	//画像のロード	
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/optionBack.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/Frame.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/Frame.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/Frame.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/Frame.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/ControllerIcon.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/SoundIcon.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/RetryIcon.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/DirectingIcon.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/SettingFrame.png", 1));

	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/Gauge.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/GaugeFrame.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/GaugeArrow.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/Gauge.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/GaugeFrame.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/GaugeArrow.png", 1));

	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Tutorial/Controller/MoveTutorial.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Tutorial/Controller/AttackTutorial.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Tutorial/Controller/DodgeTutorial.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Tutorial/Controller/Attribute1Tutorial.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Tutorial/Controller/Attribute2Tutorial.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Tutorial/Keyboard/MoveTutorial.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Tutorial/Keyboard/AttackTutorial.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Tutorial/Keyboard/DodgeTutorial.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Tutorial/Keyboard/Attribute1Tutorial.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Tutorial/Keyboard/Attribute2Tutorial.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Tutorial/Next.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Tutorial/Return.png", 1));

	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/ControllerIcon.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/UIs/KeyboardIcon.png", 1));
	sprites.emplace_back(new Sprite_Batch(device, L"./resources/Sprite/Option/Frame.png", 1));


	//パラメーターの追加
	for (int i = 0; i < sprites.size(); i++)
	{
		positions.emplace_back(DirectX::XMFLOAT2(1280 + 640, 0));
		targetPositions.emplace_back(DirectX::XMFLOAT2(0, 0));
		scales.emplace_back(DirectX::XMFLOAT2(1, 1));
		colors.emplace_back(DirectX::XMFLOAT4(1, 1, 1, 0));
	}

	//ボルト
	sprBolts[0] = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Option/Bolt1.png", 1);//内側
	sprBolts[1] = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Option/Bolt2.png", 1);//真ん中
	sprBolts[2] = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Option/Bolt3.png", 1);//外側
	boltPos[0] = { 1280 + 640, 0 };
	boltPos[1] = { 1280 + 640, 0 };


	openOption = true;

	backTitle = false;

	this->mode = mode;

	is_finalize_frame = false;

	FontsInitialize();

	optionSystem = OptionSystem::Instance().GetSystem();

	//sounds
	se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/OpenPause.wav"));
	se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/OperateSE.wav"));
	volumes.emplace_back(3.0f);
	volumes.emplace_back(1.0f);
}

Option::~Option()
{

}

void Option::Initialize()
{
	openOption = false;
	closeOption = false;
	openSetting = false;
	lookTutorial = false;

	nowLerp = true;
	inputOpen = true;

	settingFontAppear = false;
	settingFontDisappear = false;

	choiceOptionNum = 0;
	choiceSettingNum = 0;
	lookTutorialNum = 0;
	choiceOperateNum = 0;

	setSoundNow = false;

	changePictureInterval = 0.0f;

	//パラメーターの初期化
	targetPositions.at(BACK) = { 640,360 };
	targetPositions.at(FRAME_LEFTUP) = { 415,230 };
	targetPositions.at(FRAME_RIGHTUP) = { 885,230 };
	targetPositions.at(FRAME_LEFTDOWN) = { 415,515 };
	targetPositions.at(FRAME_RIGHTDOWN) = { 885,515 };

	targetPositions.at(CONTROLLER) = { 415,250 };
	targetPositions.at(SOUND) = { 885,250 };
	targetPositions.at(RETRY) = { 415,540 };
	targetPositions.at(DIRECTING) = { 885,540 };

	targetPositions.at(FRAME_SETTING) = { 640,150 };

	const float gaugeSizeY = sprites.at(BGM_GAUGE_CONTENTS)->GetTextureWidth() * scales.at(BGM_GAUGE_CONTENTS).x;
	targetPositions.at(BGM_GAUGE_FRAME) = { 720,175 };
	targetPositions.at(BGM_GAUGE_CONTENTS) = { 407,175 };
	targetPositions.at(BGM_GAUGE_ARROW) = { targetPositions.at(BGM_GAUGE_CONTENTS).x + optionSystem.bgmVolume * gaugeSizeY - 2.0f,135 };
	targetPositions.at(SE_GAUGE_FRAME) = { 720,305 };
	targetPositions.at(SE_GAUGE_CONTENTS) = { 407,305 };
	targetPositions.at(SE_GAUGE_ARROW) = { targetPositions.at(SE_GAUGE_CONTENTS).x + optionSystem.seVolume * gaugeSizeY - 2.0f,265 };

	for (int i = C_MOVE_TUTORIAL; i < K_ATTRIBUTE2_TUTORIAL + 1; i++)
	{
		targetPositions.at(i) = { 640,348 };
	}
	targetPositions.at(NEXT) = { 965,550};
	targetPositions.at(RETURN) = { 520,550 };

	targetPositions.at(CONTROLLER_ICON) = { 400.0f,360.0f };
	targetPositions.at(KEYBOARD_ICON) = { 840.0f,360.0f };


	for (int i = 0; i < sprites.size(); i++)
	{
		positions.at(i) = { targetPositions.at(i).x + 1280,targetPositions.at(i).y };
		scales.at(i) = { 1,1 };
		colors.at(i) = { 1,1,1,1 };
	}

	scales.at(FRAME_SETTING) = { 2.0f,0.35f };


	scales.at(BGM_GAUGE_FRAME) = { 1.5f,0.6f };
	scales.at(BGM_GAUGE_CONTENTS) = { 1.5f,0.6f };
	scales.at(BGM_GAUGE_ARROW) = { 0.7f,0.35f };
	scales.at(SE_GAUGE_FRAME) = { 1.5f,0.6f };
	scales.at(SE_GAUGE_CONTENTS) = { 1.5f,0.6f };
	scales.at(SE_GAUGE_ARROW) = { 0.7f,0.35f };

	for (int i = C_MOVE_TUTORIAL; i < K_ATTRIBUTE2_TUTORIAL + 1; i++)
	{
		scales.at(i) = { 0.45f,0.44f };
	}
	scales.at(NEXT) = { 0.35f,0.35f };
	scales.at(RETURN) = { 0.35f,0.35f };

	//----カラー----//
	colors.at(BACK) = { 1,1,1,1.0f };
	colors.at(FRAME_LEFTUP) = { 1,1,1,1 };
	colors.at(FRAME_RIGHTUP) = { 0.8f,0.8f,0.8f,1 };
	colors.at(FRAME_LEFTDOWN) = { 0.8f,0.8f,0.8f,1 };
	colors.at(FRAME_RIGHTDOWN) = {0.8f,0.8f,0.8f,1 };
	colors.at(FRAME_SETTING) = {0,1.0f,1.0f,0 };

	for (int i = BGM_GAUGE_CONTENTS; i < sprites.size(); i++)
	{
		colors.at(i) = { 1,1,1,0 };
	}

	//----------フォントの初期位置設定----------//
	fonts.at(SOUSA_SETUMEI).targetPosition = { 415,120 };
	fonts.at(ONRYOU_SETTEI).targetPosition = { 885,120 };
	fonts.at(RITORAINADO).targetPosition = { 415,405 };
	fonts.at(ENSHUTU_SETTEI).targetPosition = { 885,405 };

	//----操作説明のフォント----//
	fonts.at(KONTORORA).targetPosition = { 415,150 };
	fonts.at(KIBODO).targetPosition = { 840,150 };
	fonts.at(MODORU_CONTROLLER).targetPosition = { 640,595 };

	//----音量設定のフォント----//
	fonts.at(BGM).targetPosition = { 285,140 };
	fonts.at(SE).targetPosition = { 285,270 };
	fonts.at(SETTEINORISETTO).targetPosition = { 650,400 };
	fonts.at(MODORU_SOUND).targetPosition = { 640,530 };

	//----リトライなどのフォント----//
	fonts.at(RITORAI).targetPosition = { 640,140 };
	fonts.at(TAITORUHE).targetPosition = { 640,270 };
	fonts.at(OPUSHONWO_TOJIRU).targetPosition = { 640,400 };
	fonts.at(MODORU_RETRY).targetPosition = { 640,530 };

	//----演出設定のフォント----//
	fonts.at(TUJO).targetPosition = { 300,160 };//603
	fonts.at(TUJO_SETUMEI).targetPosition = { 350,230 };//620or646
	fonts.at(KEIRYO).targetPosition = { 300,330 };//580
	fonts.at(KEIRYO_SETUMEI).targetPosition = { 350,400 };//580
	fonts.at(MODORU_DIRECTING).targetPosition = { 640,510 };
	const float checkBoxPosY[2] = { 160,330 };
	fonts.at(CHECK_BOX).targetPosition = { 326,checkBoxPosY[optionSystem.directingSetting] };

	for (auto& f : fonts)
	{
		f.position = {f.targetPosition.x + 1280, f.targetPosition.y };
		f.color = { 0,1,1,1 };
		f.scale = { defaultFontScale,defaultFontScale };
	}
	for (int i = ENSHUTU_SETTEI+1; i < fonts.size(); i++)
	{
		fonts.at(i).color = { 0,1,1,0 };
		fonts.at(i).scale = { defaultFontScale+0.4f,defaultFontScale+0.4f };
	}

	constexpr float scaleRatio = 0.9f;
	fonts.at(TUJO_SETUMEI).scale = { defaultFontScale* scaleRatio,defaultFontScale*scaleRatio };
	fonts.at(KEIRYO_SETUMEI).scale = { defaultFontScale* scaleRatio,defaultFontScale* scaleRatio };

	//-----ボルト-----//
	for (int i = 0; i < 2; i++)
		boltPos[i] = { targetBoltPos[i].x + 1280.0f,targetBoltPos[i].y };

	se.at(SE::OPEN_PAUSE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(OPEN_PAUSE));
}

bool Option::Finalize()
{
	closeOption = true;
	inputOpen = false;

	OptionSystem::Instance().SetSystemResources(optionSystem);

	se.at(SE::OPEN_PAUSE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(OPEN_PAUSE));

	return true; 
}

void Option::Update(float elapsedTime)
{
	// タイトル時専用
	is_finalize_frame = false;


	//入力のインターバル
	interval -= elapsedTime;
	//tutorial画像をスクロールするインターバル
	changePictureInterval -= elapsedTime;

	//線形補完のratio更新
	ratio = 10.0f * elapsedTime;
	
	//-----開閉の処理-----//
 	OpenAndCloseOption();

	//-----どの設定を選んでいるか-----//
	if (!nowLerp && inputOpen)
	{
		GetMoveKey();
	}

	//-----選択中の色変える-----//
	ChangeColor();

	//-----決定キーが押されたら-----//
	if (!nowLerp && openOption)
	{
		GetDecisionKey();
	}

	//-----設定画面の開閉-----//
	OpenAndCloseSetting();

	//設定画面の更新処理
	if ((!nowLerp && openSetting)||(openSetting && lookTutorial))
	{
		if (SettingUpdate[choiceOptionNum] != nullptr)
			(this->*SettingUpdate[choiceOptionNum])(elapsedTime);
	}
	if (settingFontDisappear)
	{
		DisappearSettingFont(ENSHUTU_SETTEI+1, static_cast<int>(fonts.size()));
	}

	//position補正
	//PositionCorrect();

	//ボルトの更新処理
	BoltUpdate(elapsedTime);

	//resource更新
	OptionSystem::Instance().SetSystemResources(optionSystem);
}

void Option::Render(ID3D11DeviceContext* dc)
{
	for (int i = 0; i < sprites.size(); i++)
	{
		DirectX::XMFLOAT2 texSize = { sprites.at(i)->GetTextureWidth(),sprites.at(i)->GetTextureHeight() };
		DirectX::XMFLOAT2 pivot = { texSize.x / 2.0f,texSize.y / 2.0f };
		if (i == BGM_GAUGE_CONTENTS || i == SE_GAUGE_CONTENTS)
		{
			pivot.x = 0.0f;
			const float gaugeSizeY = sprites.at(BGM_GAUGE_CONTENTS)->GetTextureWidth();
			texSize.x = (i == BGM_GAUGE_CONTENTS) ? optionSystem.bgmVolume * gaugeSizeY : optionSystem.seVolume * gaugeSizeY;
		}

		sprites.at(i)->Begin(dc);
		sprites.at(i)->render(dc, positions.at(i), scales.at(i), { 0,0 }, texSize, pivot, 0.0f, colors.at(i));
		sprites.at(i)->End(dc);
	}
	//ボルト
	for (int b = 0; b < 2; b++)
	{
		for (int i = 0; i < 3; i++)
		{
			const DirectX::XMFLOAT2 texSize = { sprBolts[i]->GetTextureWidth(),sprBolts[i]->GetTextureHeight() };
			DirectX::XMFLOAT2 pivot = { texSize.x / 2.0f,texSize.y / 2.0f };

			sprBolts[i]->Begin(dc);
			sprBolts[i]->render(dc, boltPos[b], { 1.0f,1.0f }, { 0,0 }, texSize, pivot, boltAngle[i], { 1,1,1,1 });
			sprBolts[i]->End(dc);
		}
	}

	//フォント
	font->Begin(dc);
	for (int i = 0; i < fonts.size(); i++)
	{
		if (i >= TUJO && i <= KEIRYO_SETUMEI)
		{
			font->Draw(fonts.at(i).position, fonts.at(i).scale, fonts.at(i).color, fonts.at(i).string, ::wcslen(fonts.at(i).string), Font::TEXT_ALIGN::UPPER_LEFT);
		}
		else
		{
			font->Draw(fonts.at(i).position, fonts.at(i).scale, fonts.at(i).color, fonts.at(i).string, ::wcslen(fonts.at(i).string), Font::TEXT_ALIGN::UPPER_MIDDLE);
		}
	}
	font->End(dc);
}

void Option::GuiMenu()
{
#if _DEBUG
	ImGui::Begin("Option");
	
	ImGui::DragFloat2("TUJO_pos", &fonts.at(TUJO).position.x, 0.5f,0.0f,1280);
	ImGui::DragFloat2("CHECK_BOX", &fonts.at(CHECK_BOX).position.x, 0.5f,0.0f,1280);
	

	ImGui::End();
#endif
}


void Option::OpenAndCloseOption()
{
	if (!openOption)
	{
		for (int i = 0; i < sprites.size(); i++)
		{
			positions.at(i).x = Calcu3D::LerpFloat(positions.at(i).x, targetPositions.at(i).x, ratio);
		}
		for (auto& f : fonts)
		{
			f.position.x = Calcu3D::LerpFloat(f.position.x, f.targetPosition.x, ratio);
		}

		if (positions.at(BACK).x <= 640.1f)
		{
			openOption = true;
			nowLerp = false;
			PositionCorrect();
		}
		for (int i = 0; i < 2; i++)
		{
			boltPos[i].x = Calcu3D::LerpFloat(boltPos[i].x, targetBoltPos[i].x, ratio);
		}
	}
	if (closeOption)
	{
		openSetting = false;
		for (int i = 0; i < sprites.size(); i++)
		{
			positions.at(i).x = Calcu3D::LerpFloat(positions.at(i).x, targetPositions.at(i).x + 1280.0f, ratio);
		}
		for (auto& f : fonts)
		{
			f.position.x = Calcu3D::LerpFloat(f.position.x, f.targetPosition.x + 1280.0f, ratio);
		}
		if (positions.at(BACK).x >= 640.0f + 1280.1f)
		{
			nowLerp = false;
			closeOption = false;
		}
		for (int i = 0; i < 2; i++)
		{
			boltPos[i].x = Calcu3D::LerpFloat(boltPos[i].x, targetBoltPos[i].x + 1280.0f, ratio);
		}
	}
}

void Option::OpenAndCloseSetting()
{
	//open
	if (openOption && openSetting && nowLerp && inputOpen)
	{
		if (choiceOptionNum == SOUSA_SETUMEI && openSetting && colors.at(1).w <= 0.01f)return;

		//sprite
		for (int i = FRAME_LEFTUP; i < FRAME_RIGHTDOWN + 1; i++)
		{
			colors.at(i).w = Calcu3D::LerpFloat(colors.at(i).w, 0.0f, ratio);
			colors.at(4 + i).w = Calcu3D::LerpFloat(colors.at(i).w, 0.0f, ratio);

			if (choiceOptionNum == i - 1)
			{
				scales.at(i).x = Calcu3D::LerpFloat(scales.at(i).x, 2.5f, ratio);
				scales.at(i).y = Calcu3D::LerpFloat(scales.at(i).y, 2.5f, ratio);

				positions.at(i).x = Calcu3D::LerpFloat(positions.at(i).x, 640, ratio);
				positions.at(i).y = Calcu3D::LerpFloat(positions.at(i).y, 360, ratio);
				positions.at(4 + i) = { positions.at(i).x,positions.at(i).y + 35.0f };

				if (colors.at(i).w <= 0.01f)
				{
					nowLerp = false;
					choiceSettingNum = 0;
				}
			}
		}
		//font
		for (int i = SOUSA_SETUMEI; i < ENSHUTU_SETTEI + 1; i++)
		{
			if (choiceOptionNum == i)
			{
				fonts.at(i).scale.x = Calcu3D::LerpFloat(fonts.at(i).scale.x, 2.0f, ratio);
				fonts.at(i).scale.y = Calcu3D::LerpFloat(fonts.at(i).scale.y, 2.0f, ratio);

				fonts.at(i).position.x = Calcu3D::LerpFloat(fonts.at(i).position.x, 640, ratio);
				fonts.at(i).position.y = Calcu3D::LerpFloat(fonts.at(i).position.y, 360 - 100, ratio);
			}
			fonts.at(i).color.w = Calcu3D::LerpFloat(fonts.at(i).color.w, 0.0f, ratio);
		}
	}
	//close
	if (openOption && !openSetting && nowLerp && inputOpen)
	{
		//sprite
		for (int i = FRAME_LEFTUP; i < FRAME_RIGHTDOWN + 1; i++)
		{
			colors.at(i).w = Calcu3D::LerpFloat(colors.at(i).w, 1.0f, ratio);
			colors.at(i + 4).w = Calcu3D::LerpFloat(colors.at(i).w, 1.0f, ratio);

			if (choiceOptionNum == i - 1)
			{
				scales.at(i).x = Calcu3D::LerpFloat(scales.at(i).x, 1.0f, ratio);
				scales.at(i).y = Calcu3D::LerpFloat(scales.at(i).y, 1.0f, ratio);

				positions.at(i).x = Calcu3D::LerpFloat(positions.at(i).x, targetPositions.at(i).x, ratio);
				positions.at(i).y = Calcu3D::LerpFloat(positions.at(i).y, targetPositions.at(i).y, ratio);
				positions.at(i + 4) = { positions.at(i).x,positions.at(i).y + 35.0f };

				if (colors.at(i).w >= 0.99f)
				{
					nowLerp = false;

					settingFontAppear = false;
					settingFontDisappear = false;
				}
			}
		}
		//font
		for (int i = SOUSA_SETUMEI; i < ENSHUTU_SETTEI + 1; i++)
		{
			if (choiceOptionNum == i)
			{
				fonts.at(i).scale.x = Calcu3D::LerpFloat(fonts.at(i).scale.x, defaultFontScale, ratio);
				fonts.at(i).scale.y = Calcu3D::LerpFloat(fonts.at(i).scale.y, defaultFontScale, ratio);

				fonts.at(i).position.x = Calcu3D::LerpFloat(fonts.at(i).position.x, fonts.at(i).targetPosition.x, ratio);
				fonts.at(i).position.y = Calcu3D::LerpFloat(fonts.at(i).position.y, fonts.at(i).targetPosition.y, ratio);
			}
			fonts.at(i).color.w = Calcu3D::LerpFloat(fonts.at(i).color.w, 1.0f, ratio);
		}
	}
}

void Option::GetMoveKey()
{
    const auto& gamepad = GamePad::Instance();
	if (interval <= 0.0f)
	{
		bool playSE = false;
		if (!openSetting)
		{
			if ((gamepad.GetAxisLY() > 0.3f)|| gamepad.GetButton() & GamePad::BTN_UP)//up
			{
				if(choiceOptionNum >= 2)
				{
					choiceOptionNum -= 2;
					interval = maxInterval;
					playSE = true;
				}
			}
			if (gamepad.GetAxisLY() < -0.3f || gamepad.GetButton() & GamePad::BTN_DOWN)//down
			{
				if(choiceOptionNum <= 1)
				{
					choiceOptionNum += 2;
					interval = maxInterval;
					playSE = true;
				}
			}
			if (gamepad.GetAxisLX() > 0.3f || gamepad.GetButton() & GamePad::BTN_RIGHT)//right
			{
				if (choiceOptionNum % 2 == 0)
				{
					choiceOptionNum++;
					interval = maxInterval;
					playSE = true;
				}
			}
			if (gamepad.GetAxisLX() < -0.3f || gamepad.GetButton() & GamePad::BTN_LEFT)//left
			{
				if (choiceOptionNum % 2 == 1)
				{
					choiceOptionNum--;
					interval = maxInterval;
					playSE = true;
				}
			}
		}
		else
		{
			if (gamepad.GetAxisLY() > 0.3f || gamepad.GetButton() & GamePad::BTN_UP)//up
			{
				if(choiceSettingNum > articleMin)
				{
					choiceSettingNum--;
					interval = maxInterval;
					playSE = true;
				}
				if(choiceOperateNum == 2 && !lookTutorial)
				{
					choiceOperateNum = 0;
					playSE = true;
				}
			}
			if (gamepad.GetAxisLY() < -0.3f || gamepad.GetButton() & GamePad::BTN_DOWN)//down
			{
				if (choiceSettingNum < articleMax)
				{
					choiceSettingNum++;
					interval = maxInterval;
					playSE = true;
				}
				if (choiceOperateNum != 2 && !lookTutorial)
				{
					choiceOperateNum = 2;
					playSE = true;
				}
			}
			if (choiceOptionNum == 0)
			{
				if (gamepad.GetAxisLX() > 0.3f || gamepad.GetButton() & GamePad::BTN_RIGHT)//right
				{
					if (lookTutorial && choiceSettingNum == 0)
					{
						if (lookTutorialNum < 4)
						{
							lookTutorialNum++;
							interval = maxInterval;
							changePictureInterval = maxInterval * 0.1f;
							playSE = true;
						}
					}
					else
					{
						if (choiceOperateNum == 0 && !lookTutorial)
						{
							choiceOperateNum = 1;
							playSE = true;
						}
					}
				}
				if (gamepad.GetAxisLX() < -0.3f || gamepad.GetButton() & GamePad::BTN_LEFT)//left
				{
					if (lookTutorial && choiceSettingNum == 0)
					{
						if (lookTutorialNum > 0)
						{
							lookTutorialNum--;
							interval = maxInterval;
							changePictureInterval = maxInterval * 0.1f;
							playSE = true;
						}
					}
					else
					{
						if (choiceOperateNum == 1 && !lookTutorial)
						{
							choiceOperateNum = 0;
							playSE = true;
						}
					}
				}
			}
		}
		if (playSE)
		{
			se.at(SE::SELECT)->Stop();
			se.at(SE::SELECT)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SELECT));
		}
	}
}

void Option::GetDecisionKey()
{
	if (!inputOpen || openSetting)return;

	if (GamePad::Instance().GetButtonDown() & startButton)
	{
		se.at(SE::SELECT)->Stop();
		se.at(SE::SELECT)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SELECT));

		// タイトル時専用
		if (mode == MODE::TITLE)
		{
			if (choiceOptionNum == RITORAINADO)
			{
				is_finalize_frame = true;

				return;
			}
		}


		if (choiceOptionNum == SOUSA_SETUMEI)
		{
			if (!openSetting)
			{
				openSetting = true;
				choiceOperateNum = 0;
			}
		}
		else
		{
			openSetting = !openSetting;
		}

		nowLerp = true;

		if(settingFontAppear)
		{
			settingFontDisappear = true;
		}
		else
		{
			settingFontDisappear = false;
		}

		if(openSetting)
		{
			//カーソル合わせる
			const float gaugeSizeY = sprites.at(BGM_GAUGE_CONTENTS)->GetTextureWidth() * scales.at(BGM_GAUGE_CONTENTS).x;
			positions.at(BGM_GAUGE_ARROW).x = targetPositions.at(BGM_GAUGE_CONTENTS).x + optionSystem.bgmVolume * gaugeSizeY - 2.0f;
			positions.at(SE_GAUGE_ARROW).x = targetPositions.at(SE_GAUGE_CONTENTS).x + optionSystem.seVolume * gaugeSizeY - 2.0f;
		}
	}
}

void Option::ChangeColor()
{
	//sprite
	for (int i = FRAME_LEFTUP; i < FRAME_RIGHTDOWN + 1; i++)
	{
		if (!openSetting && !nowLerp)
		{
			if (choiceOptionNum == i - 1)
			{
				scales.at(i) = { 1.1f,1.1f };
				scales.at(i + 4) = { 1.1f,1.1f };


				colors.at(i) = { 1,1,1,1 };
				colors.at(i+4) = { 1,1,1,1 };
			}
			else
			{
				scales.at(i) = { 1,1 };
				scales.at(i + 4) = { 1,1 };


				colors.at(i) = { 0.7f,0.7f,0.7f,1 };
				colors.at(i+4) = { 0.7f,0.7f,0.7f,1 };
			}
		}
	}
	//font
	for (int i = SOUSA_SETUMEI; i < ENSHUTU_SETTEI + 1; i++)
	{
		if (!openSetting && !nowLerp)
		{
			if(choiceOptionNum == i)
			{
				fonts.at(i).scale = { defaultFontScale + 0.1f,defaultFontScale + 0.1f };
				fonts.at(i).color = { 0,1,1,1 };
			}
			else
			{
				fonts.at(i).scale = { defaultFontScale,defaultFontScale };
				fonts.at(i).color = { 0,0.7f,0.7f,1 };
			}
		}
	}
}

void Option::ChoiceOperate()
{
	const int useFontMin = KONTORORA;
	const int useFontMax = MODORU_CONTROLLER;
	EraseFonts(useFontMin, useFontMax);

	if (nowLerp)return;
	settingFontDisappear = false;

	if(!settingFontAppear)
	{
		AppearSettingFont(useFontMin, useFontMax);

		colors.at(CONTROLLER_ICON).w = Calcu3D::LerpFloat(colors.at(CONTROLLER_ICON).w, 1.0f, ratio);
		colors.at(KEYBOARD_ICON).w = Calcu3D::LerpFloat(colors.at(KEYBOARD_ICON).w, 1.0f, ratio);
		scales.at(CONTROLLER_ICON) = { 1.0f,1.0f };
		scales.at(KEYBOARD_ICON) = { 1.0f,1.0f };
		positions.at(FRAME_SETTING) = { 400.0f,360.0f };
		scales.at(FRAME_SETTING) = { 1.0f,1.0f };

		positions.at(CONTROLLER_ICON) = { 400.0f,360.0f };
		positions.at(KEYBOARD_ICON) = { 840.0f,360.0f };


		fonts.at(MODORU_CONTROLLER).position.y = 500.0f;
	}
	else
	{
		const DirectX::XMFLOAT2 pos[3] = { {400.0f,360.0f},{840.0f,360.0f},{640,530} };
		const DirectX::XMFLOAT2 sca[3] = { {1,1},{1,1},{0.7f,0.35f} };

		positions.at(FRAME_SETTING) = Calcu3D::LerpFloat2(positions.at(FRAME_SETTING), pos[choiceOperateNum], ratio);
		scales.at(FRAME_SETTING) = Calcu3D::LerpFloat2(scales.at(FRAME_SETTING), sca[choiceOperateNum], ratio);

		if (GamePad::Instance().GetButtonDown() & startButton)
		{
			if(choiceOperateNum == 2)//戻る
			{
				openSetting = false;
				nowLerp = true;
				settingFontDisappear = true;

				se.at(SE::SELECT)->Stop();
				se.at(SE::SELECT)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SELECT));
			}
			else
			{
				lookTutorial = true;

				nowLerp = true;

				settingFontAppear = false;
				settingFontDisappear = true;

				choiceSettingNum = 0;

				se.at(SE::SELECT)->Stop();
				se.at(SE::SELECT)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SELECT));
			}
		}
	}
}
void Option::ControllerSetting(float elapsedTime)
{
	if(!lookTutorial)
	{
		ChoiceOperate();
	}
	else if(nowLerp && lookTutorial)
	{
		colors.at(CONTROLLER_ICON).w = Calcu3D::LerpFloat(colors.at(CONTROLLER_ICON).w, 0.0f, ratio);
		colors.at(KEYBOARD_ICON).w = Calcu3D::LerpFloat(colors.at(KEYBOARD_ICON).w, 0.0f, ratio);

		if (choiceOperateNum == 0 || choiceOperateNum == 1)
		{
			scales.at(28 + choiceOperateNum) = Calcu3D::LerpFloat2(scales.at(28 + choiceOperateNum), { 2.5f,2.5f }, ratio);

			positions.at(28 + choiceOperateNum) = Calcu3D::LerpFloat2(positions.at(28 + choiceOperateNum), { 640,360 }, ratio);

			positions.at(FRAME_SETTING) = { positions.at(28 + choiceOperateNum).x,positions.at(28 + choiceOperateNum).y + 35.0f };

			if (colors.at(28 + choiceOperateNum).w <= 0.01f)
			{
				nowLerp = false;
				choiceSettingNum = 0;
				settingFontDisappear = false;
			}
		}
		//font
		for (int i = KONTORORA; i < MODORU_CONTROLLER + 1; i++)
		{
			if (choiceOperateNum == i + 4)
			{
				fonts.at(i).scale.x = Calcu3D::LerpFloat(fonts.at(i).scale.x, 2.0f, ratio);
				fonts.at(i).scale.y = Calcu3D::LerpFloat(fonts.at(i).scale.y, 2.0f, ratio);

				fonts.at(i).position.x = Calcu3D::LerpFloat(fonts.at(i).position.x, 640, ratio);
				fonts.at(i).position.y = Calcu3D::LerpFloat(fonts.at(i).position.y, 360 - 100, ratio);
			}
			fonts.at(i).color.w = Calcu3D::LerpFloat(fonts.at(i).color.w, 0.0f, ratio);
		}
	}
	else if (lookTutorial)
	{

		static DirectX::XMFLOAT2 framePosition[2] = { { 640,350 }, {640,630} };
		static DirectX::XMFLOAT2 framescale[2] = { { 2.1f,2.0f },{ 0.5f,0.3f } };

		const int useFontMin = MODORU_CONTROLLER;
		const int useFontMax = MODORU_CONTROLLER;

		EraseFonts(useFontMin, useFontMax);

		fonts.at(MODORU_CONTROLLER).position.y = 595.0f;

		articleMax = 1;
		articleMin = 0;
		choiceSettingNum = (choiceSettingNum > articleMax) ? articleMax : ((choiceSettingNum < articleMin) ? articleMin : choiceSettingNum);

		if (!settingFontAppear)
		{
			AppearSettingFont(useFontMin, useFontMax);

			////説明画像出す
			colors.at(C_MOVE_TUTORIAL + choiceOperateNum*5).w = Calcu3D::LerpFloat(colors.at(C_MOVE_TUTORIAL + choiceOperateNum * 5).w, 1.0f, ratio);
			colors.at(NEXT).w = Calcu3D::LerpFloat(colors.at(NEXT).w, 1.0f, ratio);
		}
		else
		{
			//チュートリアル画像の切り替え
			for (int i = C_MOVE_TUTORIAL + choiceOperateNum * 5; i < C_ATTRIBUTE2_TUTORIAL + 1 + choiceOperateNum * 5; i++)
			{
				float end = 0.0f;
				if (i == C_MOVE_TUTORIAL + choiceOperateNum * 5 + lookTutorialNum && changePictureInterval <= 0.0f)end = 1.0f;
				colors.at(i).w = Calcu3D::LerpFloat(colors.at(i).w, end, ratio * 0.5f);
			}

			if (lookTutorialNum == 0)
				colors.at(RETURN).w = Calcu3D::LerpFloat(colors.at(RETURN).w, 0.0f, ratio * 0.4f);
			else
				colors.at(RETURN).w = Calcu3D::LerpFloat(colors.at(RETURN).w, 1.0f, ratio * 0.4f);
			if (lookTutorialNum == 4)
				colors.at(NEXT).w = Calcu3D::LerpFloat(colors.at(NEXT).w, 0.0f, ratio * 0.4f);
			else
				colors.at(NEXT).w = Calcu3D::LerpFloat(colors.at(NEXT).w, 1.0f, ratio * 0.4f);


			//設定画面を閉じる
			if ((GamePad::Instance().GetButtonDown() & startButton) && choiceSettingNum == 1)
			{
				lookTutorial = false;

				nowLerp = true;

				settingFontAppear = false;
				settingFontDisappear = true;

				lookTutorialNum = 0;
				choiceOperateNum = 0;



				se.at(SE::SELECT)->Stop();
				se.at(SE::SELECT)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SELECT));

			}
		}

		//-----枠-----//
		positions.at(FRAME_SETTING) = Calcu3D::LerpFloat2(positions.at(FRAME_SETTING), framePosition[choiceSettingNum], ratio);
		scales.at(FRAME_SETTING) = Calcu3D::LerpFloat2(scales.at(FRAME_SETTING), framescale[choiceSettingNum], ratio);
	}
}
void Option::SoundSetting(float elapsedTime)
{
	constexpr DirectX::XMFLOAT2 framePosition[4] = { {290,170},{290,300},{640,430}, {640, 560} };
	constexpr float framescaleX[4] = { 0.4f,0.35f,1.0f, 0.5f };

	const int useFontMin = BGM;
	const int useFontMax = MODORU_SOUND;

	EraseFonts(useFontMin, useFontMax);

	articleMin = 0;
	articleMax = 3;
	choiceSettingNum = (choiceSettingNum > articleMax) ? articleMax : ((choiceSettingNum < articleMin) ? articleMin : choiceSettingNum);
	auto gamepad = GamePad::Instance();

	if(!settingFontAppear)
	{
		AppearSettingFont(useFontMin, useFontMax);

		//調節バー出す
		for(int i= BGM_GAUGE_CONTENTS;i<= SE_GAUGE_ARROW;i++)
		{
			colors.at(i).w = Calcu3D::LerpFloat(colors.at(i).w, 1.0f, ratio);
		}
	}
	else
	{
		if (choiceSettingNum < 2)//音量調節
		{
			if (gamepad.GetAxisLX() > 0.3f)//right
			{
				if (choiceSettingNum == 0)optionSystem.bgmVolume += changeVolumeSpeed * elapsedTime;
				if (choiceSettingNum == 1)optionSystem.seVolume += changeVolumeSpeed * elapsedTime;

				if (optionSystem.bgmVolume > 1.0f)optionSystem.bgmVolume = 1.0f;
				if (optionSystem.seVolume > 1.0f)optionSystem.seVolume = 1.0f;

				//カーソル合わせる
				const float gaugeSizeY = sprites.at(BGM_GAUGE_CONTENTS)->GetTextureWidth() * scales.at(BGM_GAUGE_CONTENTS).x;
				positions.at(BGM_GAUGE_ARROW).x = targetPositions.at(BGM_GAUGE_CONTENTS).x + optionSystem.bgmVolume * gaugeSizeY - 2.0f;
				positions.at(SE_GAUGE_ARROW).x = targetPositions.at(SE_GAUGE_CONTENTS).x + optionSystem.seVolume * gaugeSizeY - 2.0f;

				setSoundNow = true;
			}
			if (gamepad.GetAxisLX() < -0.3f)//left
			{
				if (choiceSettingNum == 0)optionSystem.bgmVolume -= changeVolumeSpeed * elapsedTime;
				if (choiceSettingNum == 1)optionSystem.seVolume -= changeVolumeSpeed * elapsedTime;

				if (optionSystem.bgmVolume < 0.0f)optionSystem.bgmVolume = 0.0f;
				if (optionSystem.seVolume < 0.0f)optionSystem.seVolume = 0.0f;

				//カーソル合わせる
				const float gaugeSizeY = sprites.at(BGM_GAUGE_CONTENTS)->GetTextureWidth() * scales.at(BGM_GAUGE_CONTENTS).x;
				positions.at(BGM_GAUGE_ARROW).x = targetPositions.at(BGM_GAUGE_CONTENTS).x + optionSystem.bgmVolume * gaugeSizeY - 2.0f;
				positions.at(SE_GAUGE_ARROW).x = targetPositions.at(SE_GAUGE_CONTENTS).x + optionSystem.seVolume * gaugeSizeY - 2.0f;

				setSoundNow = true;
			}

			if((!(gamepad.GetAxisLX() > 0.3f) && !(gamepad.GetAxisLX() < -0.3f)) && setSoundNow)
			{
				se.at(SE::SELECT)->Stop();
				se.at(SE::SELECT)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SELECT));
				setSoundNow = false;
			}
		}
		else//設定のリセット
		{
			if (gamepad.GetButtonDown() & startButton)
			{
				if (choiceSettingNum == 2)//設定のリセット
				{
					optionSystem.bgmVolume = optionSystem.seVolume = 0.5f;
					//カーソル合わせる
					const float gaugeSizeY = sprites.at(BGM_GAUGE_CONTENTS)->GetTextureWidth() * scales.at(BGM_GAUGE_CONTENTS).x;
					positions.at(BGM_GAUGE_ARROW).x = targetPositions.at(BGM_GAUGE_CONTENTS).x + optionSystem.bgmVolume * gaugeSizeY - 2.0f;
					positions.at(SE_GAUGE_ARROW).x = targetPositions.at(SE_GAUGE_CONTENTS).x + optionSystem.seVolume * gaugeSizeY - 2.0f;

					se.at(SE::SELECT)->Stop();
					se.at(SE::SELECT)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SELECT));

				}
				else if(choiceSettingNum == 3)//もどる
				{
					openSetting = false;

					nowLerp = true;

					settingFontDisappear = true;

					se.at(SE::SELECT)->Stop();
					se.at(SE::SELECT)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SELECT));

				}
			}
		}
	}

	//-----枠-----//
	positions.at(FRAME_SETTING) = Calcu3D::LerpFloat2(positions.at(FRAME_SETTING), framePosition[choiceSettingNum], ratio);
	scales.at(FRAME_SETTING) = Calcu3D::LerpFloat2(scales.at(FRAME_SETTING), { framescaleX[choiceSettingNum], 0.35f }, ratio);
}
void Option::RetrySetting(float elapsedTime)
{
	// タイトル時専用
	if (mode == MODE::TITLE)
	{
		openOption = false;
		closeOption = true;
		return;
	}


	constexpr DirectX::XMFLOAT2 framePosition[4] = { {640,165},{640,295},{640,430}, {640, 560} };
	constexpr float framescaleX[4] = { 0.8f,0.9f,1.5f, 0.5f };

	const int useFontMin = RITORAI;
	const int useFontMax = MODORU_RETRY;

	EraseFonts(useFontMin, useFontMax);

	articleMin = 0;
	articleMax = 3;
	choiceSettingNum = (choiceSettingNum > articleMax) ? articleMax : ((choiceSettingNum < articleMin) ? articleMin : choiceSettingNum);

	if (!settingFontAppear)
	{
		AppearSettingFont(useFontMin, useFontMax);
	}
	else
	{
		if (GamePad::Instance().GetButtonDown() & startButton)
		{
			switch(choiceSettingNum)
			{
			case 0://リトライ

				retryGame = true;

				break;
			case 1://タイトルへ

				backTitle = true;

				break;
			case 2://オプションを閉じる
				openSetting = false;
				//openOption = false;
				closeOption = true;
				Finalize();

				break;
			case 3://戻る
				openSetting = false;

				nowLerp = true;

				settingFontDisappear = true;
				break;
			}

			se.at(SE::SELECT)->Stop();
			se.at(SE::SELECT)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SELECT));

		}
	}

	//-----枠-----//
	positions.at(FRAME_SETTING) = Calcu3D::LerpFloat2(positions.at(FRAME_SETTING), framePosition[choiceSettingNum], ratio);
	scales.at(FRAME_SETTING) = Calcu3D::LerpFloat2(scales.at(FRAME_SETTING), { framescaleX[choiceSettingNum],0.35f }, ratio);

}
void Option::DirectingSetting(float elapsedTime)
{
	constexpr DirectX::XMFLOAT2 framePosition[3] = { {420,190},{420,360}, {640, 540} };
	constexpr float framescaleX[3] = { 0.8f,0.8f, 0.5f };

	const int useFontMin = TUJO;
	const int useFontMax = CHECK_BOX;

	EraseFonts(useFontMin, useFontMax);

	articleMin = 0;
	articleMax = 2;
	choiceSettingNum = (choiceSettingNum > articleMax) ? articleMax : ((choiceSettingNum < articleMin) ? articleMin : choiceSettingNum);

	if (!settingFontAppear)
	{
		AppearSettingFont(useFontMin, useFontMax);
	}
	else
	{
		if (GamePad::Instance().GetButtonDown() & startButton)
		{
			if(choiceSettingNum < articleMax)//設定の変更
			{
				optionSystem.directingSetting = choiceSettingNum;
				const float checkBoxPosY[2] = { 160,330 };
				fonts.at(CHECK_BOX).position.y = checkBoxPosY[optionSystem.directingSetting];
			}
			else//戻る
			{
				openSetting = false;

				nowLerp = true;

				settingFontDisappear = true;
			}

			se.at(SE::SELECT)->Stop();
			se.at(SE::SELECT)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SELECT));

		}
	}
	
	//-----枠-----//
	positions.at(FRAME_SETTING) = Calcu3D::LerpFloat2(positions.at(FRAME_SETTING), framePosition[choiceSettingNum], ratio);
	scales.at(FRAME_SETTING) = Calcu3D::LerpFloat2(scales.at(FRAME_SETTING), { framescaleX[choiceSettingNum],0.35f }, ratio);

}

void Option::EraseFonts(const int min, const int max)
{
	for (int i = ENSHUTU_SETTEI+1; i < fonts.size(); i++)
	{
		if ((min > i) && (max < i))
		{
			fonts.at(i).color.w = 0.0f;
		}
	}
}

void Option::AppearSettingFont(const int min, const int max)
{
	//frame出す
	colors.at(FRAME_SETTING).w = Calcu3D::LerpFloat(colors.at(FRAME_SETTING).w, 1.0f, ratio);

	for (int i = ENSHUTU_SETTEI + 1; i < fonts.size(); i++)
	{
		if ((min <= i) && (max >= i))
		{
			fonts.at(i).color.w = Calcu3D::LerpFloat(fonts.at(i).color.w, 1.0f, ratio);
		}
	}

	if (fonts.at(min).color.w >= 0.99f)
	{
		settingFontAppear = true;
	}
}
void Option::DisappearSettingFont(const int min, const int max)
{
	//frame消す
	colors.at(FRAME_SETTING).w = Calcu3D::LerpFloat(colors.at(FRAME_SETTING).w, 0.0f, ratio);

	for (int i = ENSHUTU_SETTEI + 1; i < fonts.size(); i++)
	{
		if ((min <= i) && (max >= i))
		{
			fonts.at(i).color.w = Calcu3D::LerpFloat(fonts.at(i).color.w, 0.0f, ratio);
		}
	}

	//設定画面のsprite消す
	for (int i = BGM_GAUGE_CONTENTS; i <= KEYBOARD_ICON; i++)
	{
		colors.at(i).w = Calcu3D::LerpFloat(colors.at(i).w, 0.0f, ratio);
	}

	if (choiceOptionNum == SOUSA_SETUMEI && openSetting && !lookTutorial && colors.at(NEXT).w < 0.01f && colors.at(RETURN).w < 0.01f)
	{
		nowLerp = false;
	}
}


void Option::FontsInitialize()
{
	fonts.emplace_back(FontParam{ L"操作説明", { 0,-100 },{ 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,1 } });
	fonts.emplace_back(FontParam{ L"音量設定", { 0,-100 },{ 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,1 } });
	switch(mode)
	{
	case MODE::GAME:
		fonts.emplace_back(FontParam{ L"あきらめる", { 0,-100 },{ 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,1 } });
		break;
	case MODE::TITLE:
		fonts.emplace_back(FontParam{ L"タイトルへ", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,1 } });
		break;
	case MODE::TUTORIAL:
		fonts.emplace_back(FontParam{ L"チュートリアル終了", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,1 } });
		break;
	}
	fonts.emplace_back(FontParam{ L"演出設定", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,1 } });

	//----操作説明のフォント----//
	fonts.emplace_back(FontParam{ L"コントローラー", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
	fonts.emplace_back(FontParam{ L"キーボード", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
	fonts.emplace_back(FontParam{ L"戻る", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });

	//----音量設定のフォント----//
	fonts.emplace_back(FontParam{ L"BGM", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
	fonts.emplace_back(FontParam{ L"SE", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
	fonts.emplace_back(FontParam{ L"設定のリセット", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
	fonts.emplace_back(FontParam{ L"戻る", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });

	//----リトライなどのフォント----//
	if(mode==MODE::TUTORIAL)
	{
		fonts.emplace_back(FontParam{ L"ゲームへ", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
		fonts.emplace_back(FontParam{ L"タイトルへ", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
		fonts.emplace_back(FontParam{ L"オプションを閉じる", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
		fonts.emplace_back(FontParam{ L"戻る", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
	}
	else
	{
		fonts.emplace_back(FontParam{ L"リトライ", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
		fonts.emplace_back(FontParam{ L"タイトルへ", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
		fonts.emplace_back(FontParam{ L"オプションを閉じる", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
		fonts.emplace_back(FontParam{ L"戻る", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
	}

	//----演出設定のフォント----//
	fonts.emplace_back(FontParam{ L"□　通常", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
	fonts.emplace_back(FontParam{ L"各演出を重視して遊ぶことができます", { 0,-100 }, { 0,0 }, { defaultFontScale*0.8f,defaultFontScale * 0.8f }, { 0,0,0,0 } });
	fonts.emplace_back(FontParam{ L"□　軽量", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
	fonts.emplace_back(FontParam{ L"ゲームのテンポを重視して遊ぶことができます", { 0,-100 }, { 0,0 }, { defaultFontScale * 0.8f,defaultFontScale * 0.8f}, { 0,0,0,0 } });
	fonts.emplace_back(FontParam{ L"戻る", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
	fonts.emplace_back(FontParam{ L"■", { 0,-100 }, { 0,0 }, { defaultFontScale,defaultFontScale }, { 0,0,0,0 } });
	
}


void Option::BoltUpdate(float elapsedTime)
{
	boltAngle[0] += 90.0f * elapsedTime;
	boltAngle[1] -= 90.0f * elapsedTime;

	if (boltAngle[0] >= 180.0f)boltAngle[0] -= XMConvertToRadians(360);
	if (boltAngle[1] <= 180.0f)boltAngle[0] += XMConvertToRadians(360);
}

void Option::ReleaseSounds()
{
	for (const auto& s : se)
	{
		s->Release();
	}
}

void Option::PositionCorrect()
{
	if(inputOpen && openOption && !nowLerp)
	{
		for (int i = 0; i < sprites.size(); i++)
		{
			if (i != FRAME_SETTING && i != BGM_GAUGE_ARROW && i != SE_GAUGE_ARROW)
				positions.at(i).x = targetPositions.at(i).x;
		}
		for (auto& f : fonts)
		{
			f.position.x = f.targetPosition.x;
		}
	}
}


//-----------------ファイル-----------------//
//void FileOption::LoadData()
//{
//	inputFile.open(bgmFilename);
//	if(inputFile)
//	{
//		inputFile.read(reinterpret_cast<char*>(&bgmVolume), sizeof(float) * 1);
//		inputFile.close();
//	}
//	else
//	{
//		bgmVolume = 0.5f;
//	}
//
//	inputFile.open(seFilename);
//	if(inputFile)
//	{
//		inputFile.read(reinterpret_cast<char*>(&seVolume), sizeof(float) * 1);
//		inputFile.close();
//	}
//	else
//	{
//		seVolume = 0.5f;
//	}
//
//	inputFile.open(directingFilename);
//	if(inputFile)
//	{
//		inputFile.read(reinterpret_cast<char*>(&directingSetting), sizeof(int) * 1);
//		inputFile.close();
//	}
//	else
//	{
//		directingSetting = 0;
//	}
//}
//void FileOption::SaveData()
//{
//	outputFile.open(bgmFilename);
//	if (outputFile)
//	{
//		outputFile.write(reinterpret_cast<char*>(&bgmVolume), sizeof(float) * 1);
//		outputFile.close();
//	}
//
//	outputFile.open(seFilename);
//	if (outputFile)
//	{
//		outputFile.write(reinterpret_cast<char*>(&seVolume), sizeof(float) * 1);
//		outputFile.close();
//	}
//
//	outputFile.open(directingFilename);
//	if (outputFile)
//	{
//		outputFile.write(reinterpret_cast<char*>(&directingSetting), sizeof(int) * 1);
//		outputFile.close();
//	}
//}
