#pragma once

#include"Font.h"
#include"Renderer.h"
#include<d3d11.h>
#include "Controller.h"
#include "Option.h"
//****************************************************************
// 
//  チュートリアルの説明クラス
// 
//****************************************************************

class ExplainManager final
{
public:
    ExplainManager(){}
    ~ExplainManager() {}

    void Initialize(ID3D11Device* d);
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);
    void Finalize();


    //****************************************************************
    // 
    // クラス外からアクセスされるメンバ関数
    // 
    //****************************************************************

    void UpdateMoveTime(float time); // プレイヤーが動いている時間
    bool GetShowTutorialText() const { return ShowTutorialText; }// チュートリアルのテキストが表示されているかどうか

    void SetCompleteShotTutorial(bool a) { CompleteShotTutorial = a; }// 射撃で敵を倒したか
    void SetCompleteCloseTutorial(bool a) { CompleteSwordTutorial = a; }// 近接攻撃で敵を倒したか
    void SetCompleteDodgeTutorial(bool a) { CompleteDodge = a; }        // 回避のチュートリアルを終えたか
    void SetCompleteElement1Tutorial(bool a) { CompleteModeChange = a; }        // 変形のチュートリアルを終えたか
    void AddDodgeCounts() { DodgeCounts++; }

    bool GetPermissionPerformancePlayer()const { return  PermissionPerformancePlayer; }

    int GetCurrentTutorialState() const { return CurrentTutorialState; }
    void PadUpdate();
private:
    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************
    void GuiMenu();
    void InitializeCondition(); // 各条件ごとに初期化

    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************

    // オプション
    std::unique_ptr<Option> option;
    bool openOption{ false };//オプションを開いているかどうか
    bool closeOption{ false };//オプションを閉じているかどうか

    bool ShowTutorialText; // チュートリアルの画像を表示するかどうか

    float Checkmark1DissolveRate{};
    float Checkmark2DissolveRate{};

    enum Device
    {
        KeyBoard,
        Controller,
    }InputDevice{};
    float InputRatio{};
    int InputState{};

    // スプライト
    Renderer sprController;
    Renderer sprKeyBord;
    Renderer sprBackFrame;
    Renderer sprChoiceFrame;

    enum TutorialState // チュートリアルの種類
    {
        None,     // なし
        Move,     // 移動
        Attack,   // 攻撃
        Dodge,    // 回避
        Element1, // 属性１ 
        Element2, // 属性２
        End,      // 終了
        Value,    // 量
    };

    int CurrentTutorialState{ None }; // 現在のチュートリアルの種類

    //--------------------<各チュートリアルの条件に関する変数>--------------------//

    float PlayerMoveTime{};       // プレイヤーが移動している時間
    bool CompleteShotTutorial{};  // 射撃で敵を倒したかどうか
    bool CompleteSwordTutorial{}; // 近接攻撃で敵を倒したかどうか
    bool CompleteDodge{};         // 回避成功
    bool PreferGoElement2{};      // 次のチュートリアルに行くことを許可
    bool CompleteModeChange{};    // 変形完了
    bool EndTutorial{};           // チュートリアル終了

    int DodgeCounts{}; // チュートリアルの回避回数

    bool PermissionPerformancePlayer{}; // プレイヤーが退場の演出をすることを許可する
    bool DecideInputDevice{};

    float StackNextTutorialWaitSecond{}; // 次のチュートリアルへ移行するまでの時間
    const float NextTutorialWaitSeconds{ 1.0f }; // 次のチュートリアルへの以降時間（秒）

    //--------------------<描画に関する変数>--------------------//

    std::unique_ptr<Font> font{};  // フォント
    Renderer spr_CheckBox1{};   //チェックボックス
    Renderer spr_CheckBox2{};
    Renderer spr_CheckMark1{};  // チェックマーク
    Renderer spr_CheckMark2{};
    Renderer spr_TextBack{}; // テキストの背景

    //--------------------<キーボード>--------------------//
    Renderer spr_MoveExp{};     // 移動のチュートリアルの画像
    Renderer spr_AttackExp{};   // 攻撃のチュートリアルの画像
    Renderer spr_DodgeExp{};    // 回避のチュートリアルの画像
    Renderer spr_Element1Exp{}; // 属性１のチュートリアルの画像
    Renderer spr_Element2Exp{}; // 属性２のチュートリアルの画像


    //--------------------<コントローラー>--------------------//
    Renderer spr_MoveExp2{};     // 移動のチュートリアルの画像
    Renderer spr_AttackExp2{};   // 攻撃のチュートリアルの画像
    Renderer spr_DodgeExp2{};    // 回避のチュートリアルの画像
    Renderer spr_Element1Exp2{}; // 属性１のチュートリアルの画像
    Renderer spr_Element2Exp2{}; // 属性２のチュートリアルの画像


    Renderer sprBackButton{};      // コントローラーのバックボタン
    Renderer sprBackButtonFrame{}; // コントローラーのバックボタン
    Renderer sprStartButton{};     // スタートボタン

    //--------------------<デバッグ用>--------------------//
    DirectX::XMFLOAT2 DebugPosition;
    DirectX::XMFLOAT2 DebugScale;

    //--------------------<ゲームパッド入力>--------------------//
    float StackNoInputSeconds; // 連打で行き過ぎないようしばらく操作を無効化する
    const float NoInputSeconds = 0.5f; // （秒）

    // ゲーム終了フラグ
    float StackaEndGameSeconds{};  // ゲーム終了までの時間

    // サウンド
    bool IsPlaySound{};
    bool IsPlaySound2{};
    std::unique_ptr<AudioSource> se_Check{};



    //****************************************************************
    // 
    // 関数ポインタ
    // 
    //****************************************************************

    // 次のチュートリアルに遷移する条件を管理する関数ポインタ
    using pCondition = bool(ExplainManager::*)(float);

    // 次のチュートリアルに遷移する条件の関数
    pCondition NextConditionFuncArray[Value]{};

    bool ConditionNoneToMove(float elapsedTime);
    bool ConditionMoveToAttack(float elapsedTime);       // 移動のチュートリアルから攻撃のチュートリアル
    bool ConditionAttackToDodge(float elapsedTime);      // 攻撃のチュートリアルから回避のチュートリアル
    bool ConditionDodgeToElement1(float elapsedTime);    // 回避のチュートリアルから属性１のチュートリアル
    bool ConditionElement1ToElement2(float elapsedTime); //属性１のチュートリアルから属性２のチュートリアル
    bool ConditionElement2ToEnd(float elapsedTime);      //属性2のチュートリアルから終了
    bool ConditionEndToGame(float elapsedTime);          
        
    // 現在のチュートリアルに応じて表示を変える
    using pRenderExp = void(ExplainManager::*)(ID3D11DeviceContext* dc);

    pRenderExp RenderArray[Value]{};

    void RenderNoneExp(ID3D11DeviceContext* dc);   
    void RenderMoveExp(ID3D11DeviceContext* dc);     // 移動のチュートリアル
    void RenderAttackExp(ID3D11DeviceContext* dc);   // 攻撃のチュートリアル
    void RenderDodgeExp(ID3D11DeviceContext* dc);    // 回避のチュートリアル
    void RenderElement1Exp(ID3D11DeviceContext* dc); // 属性１のチュートリアル
    void RenderElement2Exp(ID3D11DeviceContext* dc); // 属性２のチュートリアル
    void RenderLastExp(ID3D11DeviceContext* dc);     // 最後のチュートリアル
    

};


