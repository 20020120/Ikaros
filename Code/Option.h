#pragma once

#include<d3d11.h>
//#include <filesystem>
#include <memory>

#include "Font.h"
#include "SpriteBatch.h"
#include "Controller.h"
#include "OptionSystem.h"
#include "gAudioSource.h"

class Option
{
public:
	enum class MODE
	{
		GAME,
		TITLE,
		TUTORIAL,
	};

public:
	Option(ID3D11Device* device, MODE mode = MODE::GAME);
	~Option();

	void Initialize();
	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc);
	bool Finalize();
	void ReleaseSounds();
	void GuiMenu();


	[[nodiscard]] bool GetIsNowLerp() const { return nowLerp; }
	[[nodiscard]] bool GetIsCloseOption() const { return closeOption; }
	[[nodiscard]] bool GetIsOpenOption() const { return inputOpen; }
	[[nodiscard]] bool GetIsBackTitle() const { return backTitle; }
	[[nodiscard]] bool GetIsRetryGame() const { return retryGame; }
	[[nodiscard]] bool GetIsFinalizeFrame() const { return is_finalize_frame; }

private:
	//----オプションの開閉----//
	void OpenAndCloseOption();
	//----設定画面を開く----//
	void OpenAndCloseSetting();
	//----移動キーが押されたら(設定画面以外)----//
	void GetMoveKey();
	//----決定キーが押されたら(設定画面以外)----//
	void GetDecisionKey();
	//-----選択中の色変える-----//
	void ChangeColor();
	//-----position補正-----//
	void PositionCorrect();
	
	//----設定画面----//
	void ChoiceOperate();
	void ControllerSetting(float elapsedTime);
	void SoundSetting(float elapsedTime);
	void RetrySetting(float elapsedTime);
	void DirectingSetting(float elapsedTime);

	//static void (Option::* SettingUpdate[])(float elapsedTime);
	using pFunc = void(Option::*)(float);

	pFunc SettingUpdate[4] = {
		&Option::ControllerSetting,
		&Option::SoundSetting,
		&Option::RetrySetting,
		&Option::DirectingSetting,
	};

	//----文字初期設定----//
	void FontsInitialize();
	//----使わない文字のalpha値0にする----//
	void EraseFonts(const int min, const int max);
	//----設定画面のフォント出す----//
	void AppearSettingFont(const int min, const int max);
	//----設定画面のフォント消す----//
	void DisappearSettingFont(const int min, const int max);


	//----ボルト----//
	void BoltUpdate(float elapsedTime);

private:
	OptionSystemResource optionSystem;


private:
	struct FontParam
	{
		wchar_t* string = nullptr;
		DirectX::XMFLOAT2 position = { 0,0 };
		DirectX::XMFLOAT2 targetPosition = { 0,0 };
		DirectX::XMFLOAT2 scale = { 0.7f,0.7f };
		DirectX::XMFLOAT4 color = { 0.0f,0.0f,0.0f,1.0f };
	};


	enum SPRITE
	{
		BACK,
		FRAME_LEFTUP,
		FRAME_RIGHTUP,
		FRAME_LEFTDOWN,
		FRAME_RIGHTDOWN,

		CONTROLLER,
		SOUND,
		RETRY,
		DIRECTING,
		FRAME_SETTING,

		BGM_GAUGE_CONTENTS,
		BGM_GAUGE_FRAME,
		BGM_GAUGE_ARROW,
		SE_GAUGE_CONTENTS,
		SE_GAUGE_FRAME,
		SE_GAUGE_ARROW,

		//操作説明(C_:コントローラー,K_:キーボード)
		C_MOVE_TUTORIAL,
		C_ATTACK_TUTORIAL,
		C_DODGE_TUTORIAL,
		C_ATTRIBUTE1_TUTORIAL,
		C_ATTRIBUTE2_TUTORIAL,
		K_MOVE_TUTORIAL,
		K_ATTACK_TUTORIAL,
		K_DODGE_TUTORIAL,
		K_ATTRIBUTE1_TUTORIAL,
		K_ATTRIBUTE2_TUTORIAL,

		NEXT,
		RETURN,

		CONTROLLER_ICON,
		KEYBOARD_ICON,
	};

	enum FONTS
	{
		SOUSA_SETUMEI,//操作説明
		ONRYOU_SETTEI,//音量設定
		RITORAINADO,//リトライなど
		ENSHUTU_SETTEI,//演出設定

		//----操作説明のフォント----//
		KONTORORA,//コントローラー
		KIBODO,//キーボード
		MODORU_CONTROLLER,//戻る

		//----音量設定のフォント----//
		BGM,
		SE,
		SETTEINORISETTO,//設定のリセット
		MODORU_SOUND,//戻る

		//----リトライなどのフォント----//
		RITORAI,//リトライ(ゲームへ)
		TAITORUHE,//タイトルへ
		OPUSHONWO_TOJIRU,//オプションを閉じる
		MODORU_RETRY,//戻る
		
		//----演出設定のフォント----//
		TUJO,//通常
		TUJO_SETUMEI,//通常の説明
		KEIRYO,//軽量
		KEIRYO_SETUMEI,//軽量の説明
		MODORU_DIRECTING,//戻る
		CHECK_BOX,
	};

	enum SE
	{
		OPEN_PAUSE,
		SELECT,
	};
private:
	std::unique_ptr<Font> font;
	std::vector<std::unique_ptr<Sprite_Batch>> sprites;
	std::vector<DirectX::XMFLOAT2> positions;
	std::vector<DirectX::XMFLOAT2> targetPositions;
	std::vector<DirectX::XMFLOAT2> scales;
	std::vector<DirectX::XMFLOAT4> colors;

	std::vector<FontParam> fonts;

	//-----ボルト-----//
	std::unique_ptr<Sprite_Batch> sprBolts[3];
	DirectX::XMFLOAT2 boltPos[2];
	DirectX::XMFLOAT2 targetBoltPos[2] = { { 150,125 },{1135,620} };
	float boltAngle[3] = { 0.0f,0.0f,0.0f };

	const GamePadButton startButton = GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y;

	//bool is_title{ false }; // 追加した (タイトルシーンかどうか) // FontInitializeとRetrySettingで干渉
	bool is_finalize_frame{ false }; // 追加した (そのフレーム内でFinalizeが呼ばれたか) // GetDecisionKeyとUpdateで干渉

	bool backTitle{ false };
	bool retryGame{ false };

	bool openOption{ false };
	bool closeOption{ false };
	bool openSetting{ false };
	bool inputOpen{ false };
	bool lookTutorial{ false };

	bool nowLerp{ false };

	bool settingFontAppear{ false };
	bool settingFontDisappear{ false };

	float ratio = 0.0f;

	const float defaultFontScale = 0.8f;

	int choiceOptionNum = 0;
	int choiceSettingNum = 0;
	int lookTutorialNum = 0;
	int choiceOperateNum = 0;

	float interval = 0.0f;
	const float maxInterval = 0.5f;

	float changePictureInterval = 0.0f;

	float changeVolumeSpeed = 0.5f;

	//setting画面の選択できる数
	int articleMax = 0;
	int articleMin = 0;

	bool setSoundNow = false;//音量調節中かどうか

	std::vector<std::unique_ptr<AudioSource>> se;
	std::vector<float> volumes;

	MODE mode = MODE::GAME;
};
