#include "Player.h"
#include <imgui.h>

#include"Bullet.h"
#include "SceneManager.h"
#include"ProjectileManager.h"
#include "EnemyManager.h"
#include "Collision.h"
#include"User_Function.h"
#include"MathSupport.h"
#include "gAudio.h"
#include "OptionSystem.h"

Player::Player(ID3D11Device* d)
    :BaseObject()
{
    ModelRenderInitializer Ini;
    Ini.SetALLPath(
        "./resources/Models/Characters/Players/Player.nk",
        //"./resources/Models/Characters/Players/PlayerModel.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/PhisicsBase_PS.cso",
        "./resources/Shaders/None_Gs.cso"
    );

    // モデルの初期化
    LoadModel(d, Ini);
    Ini.SetModelPath("./resources/Models/Characters/Players/AfterIMageModel.nk");
    Ini.SetPsPath("./resources/Shaders/AfterImage_PS.cso");
    mAfterImage.Model = std::make_unique<ModelComponent>(d, Ini);

    //--------------------<スプライトを初期化>--------------------//
    spr_HitArea = std::make_unique<Sprite_Batch>(d, L"./resources/Sprite/UIs/HitArea.png", 1);
    
    //--------------------<当たり判定可視化用を初期化>--------------------//
    hitAreaSize = { 3,3,3 };

    dodgeSize = { 25.0f,25.0f,30.0f };

    attackArea = { 25.0f,20.0f, 20.0f };

    //particleEmitter.Initialize(d);

    

    Light = std::make_unique<GamePointLight>(d, 3);
    DirectX::XMFLOAT3 p = t.Position;
    p.y += 6.0f;
    Light->SetPosition(p);
    Light->SetRange(100.0f);
    Light->SetColor({ 0.1f,0.5f,0.8f });


    spr_CutIn0.Initialize(d, L"./resources/Sprite/CutIn/view_1_red.png");
    spr_CutIn0.Position = { 1280.0f,360.0f };
    spr_CutIn0.Pivot = { 0.0f,360.0f };
    spr_CutIn0.TexSize = { 1280.0f,720.0f };

    spr_CutIn1.Initialize(d, L"./resources/Sprite/CutIn/CutInFrame.png");
    spr_CutIn1.Position = { 1280.0f,360.0f };
    spr_CutIn1.Pivot = { 0.0f,360.0f };
    spr_CutIn1.TexSize = { 1280.0f,720.0f };

    sprBlackCurtain.Initialize(d, L"./resources/Sprite/UIs/Black.png");
    sprBlackCurtain.Position = {};
    sprBlackCurtain.TexSize = { 1280.0f,720.0f };
    sprBlackCurtain.Color = { 1.0f,1.0f,1.0f,0.0f };
    sprBlackCurtain.Scale = { 1.0f,1.0f };

    ClearMask = std::make_unique<Sprite_Batch>(d,
        L"./resources/Sprite/Shaders/AlphaMask.png", 1,
        L"./resources/Sprite/Shaders/dissolve.png",
        L"./resources/Sprite/Shaders/load_screen.png");
    mClearDissolvePower = 0.0f;
    //SE
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/ModeChangeSE.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/Hit_SameColor.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/Hit_DifferentColor.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/HitInvincible.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/CloseAttack.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/GetEnergy.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/RedBulletShot.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/BlueBulletShot.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/Dodge.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/CriticalDodge.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Player/Heal.wav"));

    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/LastSlash.wav"));

    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/BossBomb.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/BossBomb2.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/BossBomb3.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/BossBomb4.wav"));

    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Enemy/LaserCharge.wav"));
    se.emplace_back(Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Enemy/LaserBeam.wav"));
    

    volumes.emplace_back(0.9f);//ModeChange
    volumes.emplace_back(0.75f);//HitSameColor
    volumes.emplace_back(0.75f);//HitDifferentColor
    volumes.emplace_back(0.5f);//HitInvincible
    volumes.emplace_back(0.5f);//CloseAttack
    volumes.emplace_back(1.0f);//GetEnergy
    volumes.emplace_back(0.4f);//RedBulletShot
    volumes.emplace_back(0.2f);//BlueBulletShot
    volumes.emplace_back(0.75f);//Dodge
    volumes.emplace_back(0.75f);//CriticalDodge
    volumes.emplace_back(1.0f);//Heal

    //****************************************************************
    // 
    // エフェクトのロード
    // 
    //****************************************************************
    efk_MuzzleFlash = std::make_unique<Effect>("./resources/Effects/muzleflash.efk");
    efk_RedDodgeSuccess = std::make_unique<Effect>("./resources/Effects/dodge_success_red.efk");
    //efk_RedDodgeSuccess = std::make_unique<Effect>("./resources/Effects/dodge_success.efk");
    efk_BlueDodgeSuccess = std::make_unique<Effect>("./resources/Effects/dodge_success_blue.efk");
    //efk_BlueDodgeSuccess = std::make_unique<Effect>("./resources/Effects/dodge_success.efk");

    efk_WeakDamaged = std::make_unique<Effect>("./resources/Effects/player_hit.efk");
    efk_UnWeakDamaged = std::make_unique<Effect>("./resources/Effects/player_hit_block.efk");
    efk_SonicBoom = std::make_unique<Effect>("./resources/Effects/sonic_boom.efk");
    efk_RedSlash = std::make_unique<Effect>("./resources/Effects/slash_red.efk");
    efk_BlueSlash = std::make_unique<Effect>("./resources/Effects/slash_blue.efk");
    efk_Heal = std::make_unique<Effect>("./resources/Effects/player_heal.efk");
    efk_SlashFinisher = std::make_unique<Effect>("./resources/Effects/white_out.efk");
    efk_Charge = std::make_unique<Effect>("./resources/Effects/laser_charge.efk");

    // じおめとりしぇーだ
    Gs_CBuffer.Init(d, &mGsData);
    Gs_CBuffer.Set_GSSlot(4);
}

void Player::Initialize()
{
    mClearDissolvePower = 0.0f;
    //--------------------<初期化>--------------------//
    StackPerformanceTime = 0.0f;
    EndGame = false;
    TutorialPerformState = 0;
    // スケール
    t.Scale = {
        0.035f,
        0.035f,
        0.035f
    };

    IsSlash = false;

    RotRatio = 0.0f;
    
    //--------------------<プレイヤーのタイプを初期化>--------------------//
    type = Type::BLUE;
    Model->f_PlayAnimation(AnimationName::wing,true);
    Model->Update(0.1f);

    mAfterImage.Model->f_PlayAnimation(AnimationName::wing, true);
    mAfterImage.Model->Update(0.1f);

    //--------------------<速度を初期化>--------------------//
    CalcSpeedFromHP();

    //--------------------<体力を初期化>--------------------//
    HitPoint = MaxHitPoint;
    alive = true;

    //--------------------<エネルギーを初期化>--------------------//
    EnergyGauge = 0;

    // 各待ち時間を初期化
    StackShootInterval = 0.0f;
    StackInvincibleTime = 0.0f;

    // 無敵判定を初期化
    IsInvincible = false;

    // 弾の向きを初期化
    BulletVelocity = { 0.0f,0.0f,3.0f };

    BulletVelocityUp = { 0.0f,2.0f,3.0f };
    BulletVelocityDown = { 0.0f,-2.0f,3.0f };

    BulletVelocityRight = { 2.0f,0.0f,3.0f };
    BulletVelocityLeft = { -2.0f,0.0f,3.0f };

    //--------------------<回避回数を初期化>--------------------//
    DodgeCount = MaxDodgeCount;

    PerformanceAngleRatio = 0.0f;

    //--------------------<GameSystemに送信するデータ>--------------------//

    GameSystemDatas.HP = &HitPoint;
    GameSystemDatas.Energy = &EnergyGauge;
    GameSystemDatas.Dodge = &DodgeCount;
    GameSystemDatas.position = &t.Position;

    // イベントフラグを読み込み
    Flags.Load("./resources/Models/Characters/Players/PlayerEventFlag.flg");

    //--------------------<演出のための初期化>--------------------//
    t.Position = { -170.0f,100.0f,-100.0f };
    IsPerforming = true;

    // チュートリアル用
    Tutorial_IsBreakCloseAttack = false;
    CompleteDodgeTutorial = false;
    IsTutorial = false;
    CompleteElement1Tutorial = false;

    //--------------------<必殺技関係の初期化>--------------------//
    IsFinisher = false;
    IsLightPerformance = false;

    // 残像を初期化
    mAfterImage.t = t;
    mAfterImage.r = r;

    SlashDir = Side;

    //--------------------<ライトの色>--------------------//
    PlayerColor = { 0.1f,0.5f,0.8f };
    Light->SetColor(PlayerColor);

    //演出設定を初期化
    IsLightPerformance = OptionSystem::Instance().GetDirectingSetting();
    PreferSkipPerformance = false;
    StackSkipTimer = 0.0f;
    EnemiesStop = false;
}

void Player::Update(float elapsedTime)
{
    CompleteDodgeTutorial = false;
    if(IsTutorial)
    {
        HitPoint = MaxHitPoint;
    }

    // ライト
    DirectX::XMFLOAT3 p = t.Position;
    p.y += 6.0f;
    p.x += 6.0f;
    Light->SetPosition(p);
    switch (type)
    {
    case Type::NONE: break;
    case Type::RED:
        PlayerColor = Calcu3D::LerpFloat3(PlayerColor, { 0.9f,0.2f,0.2f }, 2.0f * elapsedTime);
        break;
    case Type::BLUE:
        PlayerColor = Calcu3D::LerpFloat3(PlayerColor, { 0.1f,0.5f,0.8f }, 2.0f * elapsedTime);
        break;
    default: ;
    }

    Light->SetColor(PlayerColor);

    //--------------------<死亡時の演出>--------------------//

    if(HitPoint<=0)
    {
        PerformanceDead(elapsedTime);
        return ;
    }


    //--------------------<ImGuiのデバッグウインドウ>--------------------//
    GuiMenu();

    //--------------------<パーティクルの更新>--------------------//
    auto frontVec = GetFrontVec();
    frontVec *= -1.0f;
    //particleEmitter.SetVelocity(frontVec);
    //particleEmitter.SetPosition(t.Position);
    //particleEmitter.Update(elapsedTime);

    CalcParticle();

    Model->Update(elapsedTime);
    mAfterImage.Model->Update(elapsedTime);
    // 何もしていなければ待機アニメーションを流す
    if(!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(AnimationName::wait, true);
        mAfterImage.Model->f_PlayAnimation(AnimationName::wait, true);
    }

    //--------------------<必殺技中は全ての更新を受け付けない>--------------------//
    if (IsFinisher)
    {
        ShowAfterImage = false;
        StackInvincibleTime = 10.0f;

        // 必殺技の軽量設定
        if(IsLightPerformance)
        {
            // 必殺技の更新処理
            if (type == Type::BLUE)
            {
                Finisher_SlashLight(elapsedTime);
            }

            else if (type == Type::RED)
            {
                Finisher_BeamLight(elapsedTime);
            }
        }
        else
        {
            // 必殺技の更新処理
            if (type == Type::BLUE)
            {
                Finisher_Slash(elapsedTime);
            }
            else if (type == Type::RED)
            {
                Finisher_Beam(elapsedTime);
            }
        }

        return;
    }

    // 演出を更新
    IsLightPerformance = OptionSystem::Instance().GetDirectingSetting();

    // 移動を更新
    (this->*Move[static_cast<int>(GameSystem::Instance().GetCameraDirection())])(elapsedTime);

    // 視点変更中はさせない処理
    if (!GameSystem::Instance().GetChangingView())
    {

    }

    // 速度を更新
    CalcSpeedFromHP();

    // 回避
    inputDodge(elapsedTime);
    

    //射撃
    InputShooting(elapsedTime);

    //近距離攻撃
    InputCloseAttack(elapsedTime);

    // エネルギーを更新（変形）
    UpdateEnergy();

    // 無敵を更新
    UpdateInvTime(elapsedTime);

    if (!IsFinisher)
    {

        if (!IsDebug)
        {
            //衝突判定
            VsProjectile(elapsedTime);
            VsEnemy(elapsedTime);
        }
    }

    // 残像を更新
    UpdateAfterImage(elapsedTime);

    // 行動制限を更新
    LimitMoveArea();

    // スキップの黒画像を更新
    if (!IsPerforming)
    {
        sprBlackCurtain.Color.w -= elapsedTime * 5.0f;
    }
    if(sprBlackCurtain.Color.w <=0.0f )
    {
        sprBlackCurtain.Color.w = 0.0f;
    }
    
    

}

//--------------------<モデルの描画>--------------------//
void Player::Render(ID3D11DeviceContext* dc)
{

    Gs_CBuffer.SetBuffer(dc, &mGsData,GEOMETRY);

    // ライトを更新
    Light->Update(dc);

    //particleEmitter.Render(dc);
    // 無敵時間の表現

    if (IsInvincible)
    {
        const int IntTime = static_cast<int>((StackInvincibleTime) * 10.0f);
        switch (IntTime % 3)
        {
        case 0:
            Model->Render(dc, t, r);
           // AfterImageModel->Render(dc, afImage_t, afImage_r);
            break;
        default:
            break;
        }
    }
    else
    {
        Model->Render(dc, t, r);
    }

    //  残像
    if (ShowAfterImage)
    {
        mAfterImage.Model->Render(dc, mAfterImage.t, mAfterImage.r);
    }
    

}

//--------------------<スプライトの描画>--------------------//
void Player::Render2D(ID3D11DeviceContext* dc,bool draw)
{
    if (!draw)
    {
        using namespace  DirectX;
        // 当たり判定円の描画
        const XMFLOAT2 pos = {
            t.Position.z,
               t.Position.y
        };
        constexpr XMFLOAT2 scale = { 10.0f,10.0f };
        constexpr XMFLOAT2 txPos = { 0.0f,0.0f };
        constexpr XMFLOAT2 cutSize = { 25.0f,25.0f };
        constexpr XMFLOAT2 pivot = { 0.5f,0.5f };
        constexpr XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
    }
    
    spr_CutIn0.Render(dc);
    spr_CutIn1.Render(dc);

    sprBlackCurtain.Render(dc);
    ClearMask->SetDissolvePower(mClearDissolvePower);
    ClearMask->Begin(dc);
    ClearMask->Render(dc, { 0.0f,0.0f }, { 1.0f,1.0f }, { 0.0f,0.0f }, { 1280.0f,720.0f }, { 0.0f,0.0f }, 0.0f, { 1.0f,1.0f,1.0f,1.0f });
    ClearMask->End(dc);
}



void Player::Finalize()
{
    //particleEmitter.Finalize();

    //サウンド解放
    for(const auto& s : se)
    {
        s->Release();
    }
}

//****************************************************************
// 
//  メンバ関数（クラス外からアクセスされる関数）
// 
//****************************************************************

// 被ダメージ
bool Player::Damaged(const int damage)
{
    if (!IsInvincible)
    {
        //  体力を減らす
        HitPoint -= damage;
        GamePad::Instance().SetVibration(100.0f, 100.0f, 0.5f);
        mCameraShakeFunc();
        // 無敵時間開始
        StackInvincibleTime = 0.0f;
        IsInvincible = true;
        return true;
    }
    return false;
}

// エネルギー取得
void Player::GetEnergy(const Type t, const int getEnergy)
{
    switch (t)
	{
    case Type::NONE:

        break;
    case Type::RED:

        // 現在の属性から次に集めたい属性の色で加算にしたいので
        (type == Type::BLUE) ? EnergyGauge += getEnergy     // モードが青色の時、次の変形は赤色なので　赤エネルギーを取った時に加算　＋
                            : EnergyGauge -= getEnergy;     // 　　　　赤色　　　　　　　　青色　　　　　　　　　　　　　　　　減算　－

        break;
    case Type::BLUE:

        // 現在の属性から次に集めたい属性の色で加算にしたいので
        (type == Type::BLUE) ? EnergyGauge -= getEnergy      // モードが青色の時、次の変形は赤色なので　赤エネルギーを取った時に減算　－
                             : EnergyGauge += getEnergy;     // 　　　　赤色　　　　　　　　青色　　　　　　　　　　　　　　　　加算　＋

        break;
    default:

        break;;
    }
    se.at(SE::GET_ENERGY)->Stop();
    se.at(SE::GET_ENERGY)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::GET_ENERGY));
}



//****************************************************************
// 
//  メンバ関数（クラス内アクセス）
// 
//****************************************************************

// プレイヤーのモードを変換させる関数
void Player::ChangeMode()
{
#if 0
    // 赤->青 
    if (type == Type::RED && EnergyGauge <= LimitBlueEnergy)
    {
        // アニメーション
        Model->f_PlayAnimation(AnimationName::transform);
        EndTransform = false;
        // タイプ
        type = Type::BLUE;
    }
    // 青->赤
    else if (type == Type::BLUE && EnergyGauge >= LimitRedEnergy)
    {
        Model->f_PlayAnimation(AnimationName::inv_transform);
        EndTransform = false;
        type = Type::RED;
    }
    // 体力回復
    HitPoint++;
    // 回避可能回数回復
    DodgeCount = MaxDodgeCount;


    constexpr float mulValue = { 0.7f };
    //エネルギー設定
    if (type == Type::RED)
    {
        EnergyGauge = static_cast<int>(mulValue * static_cast<float>(LimitRedEnergy));
    }
    else if (type == Type::BLUE)
    {
        EnergyGauge = static_cast<int>(mulValue * static_cast<float>(LimitBlueEnergy));
    }
#else

    // 属性の変化
    if(type == Type::BLUE)
    {
        type = Type::RED;

        // アニメーション
        Model->f_PlayAnimation(AnimationName::inv_transform);
        mAfterImage.Model->f_PlayAnimation(AnimationName::inv_transform);
        EndTransform = false;

    }
    else if(type == Type::RED)
    {
        type = Type::BLUE;

        // アニメーション
        Model->f_PlayAnimation(AnimationName::transform);
        mAfterImage.Model->f_PlayAnimation(AnimationName::transform);
        EndTransform = false;

    }

    // 体力回復
    HitPoint += HealHpCount;
    // 回避可能回数回復
    DodgeCount = MaxDodgeCount;


    //エネルギー設定
    EnergyGauge = MinEnergyCount;


    se.at(SE::MODE_CHANGE)->Stop();
    se.at(SE::MODE_CHANGE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::MODE_CHANGE));

    // エフェクト再生
    efk_Heal->Play(t.Position, {}, 0.10f);

    //--------------------<体力の上限設定>--------------------//
    HitPoint = min(MaxHitPoint, HitPoint);

    //--------------------<体力の下限設定>--------------------//
    HitPoint = max(HitPoint, 0);
#endif

}

//--------------------<移動入力処理>--------------------//
const auto& Player::GetMoveVec() const
{
    auto gamepad = GamePad::Instance();
    // 入力値を取り出し
    const float ax = gamepad.GetAxisLX();
    const float ay = gamepad.GetAxisLY();

    return XMFLOAT2(ax, ay);
}

//--------------------<回転の更新>--------------------//
void Player::RotUpdate(float vecY)
{
    // 移動中のプレイヤーの角度を計算する
        // 回転の補完率
    const float RotRatio = { 0.1f };
    float End = 0.0f;

    if (vecY > 0.1f)   // 上向き
    {
        End = DirectX::XMConvertToRadians(30.0f);
    }
    else if (vecY < -0.1f) // 下向き
    {
        End = DirectX::XMConvertToRadians(-30.0f);
    }
    r.Angle.z = Calcu3D::LerpFloat(r.Angle.z, End, RotRatio);

}


//--------------------<速度を体力から計算>--------------------//
void Player::CalcSpeedFromHP()
{
    // 体力が最大値の何％か求める
    const int rate = 100 / MaxHitPoint * HitPoint;

    //rateを0.0f~1.0fに変換
    const float clampRate = static_cast<float>(rate) * 0.01f;

    // clampRateから速度を算出
    const float res = (MinSpeed * clampRate) + (MaxSpeed * (1.0f - clampRate));
    Speed = res;
}

//--------------------<回避入力処理>--------------------//
void Player::inputDodge(float elapsedTime)
{
    // 回避可能回数を更新
    if (DodgeCount < 1)
    {
        CanDodge = false;
    }
    else
    {
        CanDodge = true;
    }

    // 回避可能
    if (CanDodge && !ReadyDodge&& !IsSlash)
    {
        //  回避する
        if(GamePad::Instance().GetButtonDown() &GamePad::BTN_Y)
        {
            DodgeVecX = GetMoveVec().x;
            DodgeVecY = GetMoveVec().y;
            // ジャスト回避の受付時間をリセット
            StackDodgeTime = 0.0f;
            BeforeDodgeAngle = r.Angle.z; // 角度を初期化
            ReadyDodge = true;
            RotRatio = 0.0f;
            if (IsTutorial==false)
            {
                DodgeCount--;
            }

            //ジャスト回避成功かどうか
            const int pIndex = IsHitProjectile(dodgeSize);
            const int eIndex = IsHitEnemy(dodgeSize);
            if (pIndex >= 0 || eIndex >= 0)
            {
                dodgeSuccess = true;
                StackHitStopSeconds = 0.0f;
                StackInvincibleTime = 0.0f;

                //エネルギー取得
                DodgeSuccessEnergy(pIndex, eIndex);
                CompleteDodgeTutorial = true;
                StackAfterImageTime = 0.7f;
                ShowAfterImage = true;

                se.at(CRITICAL_DODGE)->Stop();
                se.at(CRITICAL_DODGE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(CRITICAL_DODGE));
            }
            else
            {
                se.at(DODGE)->Stop();
                se.at(DODGE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(DODGE));
            }

            // 残像の位置を更新する
            mAfterImage.t.Position = t.Position;
            mAfterImage.r = r;
            
        }
    }

    // ジャスト回避の受付時間を更新
    StackDodgeTime += elapsedTime;
    if (StackDodgeTime >= DodgeTime && ReadyDodge)
    {
        ReadyDodge = false; // ジャスト回避の受付時間開始から一定時間経過で受付終了
        dodgeSuccess = false;
    }
    else if(StackDodgeTime >= DodgeTime)
    {
        StackDodgeTime = DodgeTime;
    }

}
void Player::DodgeSuccessEnergy(const int pIndex, const int eIndex)
{
    bool dodgeProj = false;
    bool dodgeEnemy = false;

    if (pIndex >= 0 && eIndex < 0)dodgeProj = true;
    else if (pIndex < 0 && eIndex >= 0)dodgeEnemy = true;
    else
    {
        BaseProjectile& proj = ProjectileManager::Instance().GetProjectile(pIndex);
        Enemy& enemy = EnemyManager::Instance().GetEnemy(eIndex);

        float projLength = Collision::Distance(t.Position, proj.GetPosition());
        float eneLength = Collision::Distance(t.Position, enemy.GetPosition());

        if (projLength < eneLength)dodgeProj = true;
        else if (projLength > eneLength)dodgeEnemy = true;
    }

    const int parryOtherRatio = 2;
    const int parryEnergyRatio = 3;
    if(dodgeProj)
    {
        BaseProjectile& proj = ProjectileManager::Instance().GetProjectile(pIndex);

        switch (proj.GetProjType())
        {
        case BaseProjectile::ProjType::REDBULLET:
            GetEnergy(Type::RED, increaseEnergy * parryOtherRatio);
            hdl_RedDodgeSuccess = efk_RedDodgeSuccess->Play(t.Position, {}, 0.15f);
            break;
        case BaseProjectile::ProjType::BLUEBULLET:
            GetEnergy(Type::BLUE, increaseEnergy * parryOtherRatio);
            hdl_BlueDodgeSuccess = efk_BlueDodgeSuccess->Play(t.Position, {}, 0.15f);
            break;
        case BaseProjectile::ProjType::REDENERGY:
            hdl_RedDodgeSuccess = efk_RedDodgeSuccess->Play(t.Position, {}, 0.15f);
            GetEnergy(Type::RED, increaseEnergy * parryEnergyRatio);
            break;
        case BaseProjectile::ProjType::BLUEENERGY:
            GetEnergy(Type::BLUE, increaseEnergy * parryEnergyRatio);
            hdl_BlueDodgeSuccess = efk_BlueDodgeSuccess->Play(t.Position, {}, 0.15f);
            break;
	    }
    }
    else if(dodgeEnemy)
    {
        Enemy& enemy = EnemyManager::Instance().GetEnemy(eIndex);

	    switch(enemy.GetType())
	    {
        case Type::RED:
            GetEnergy(Type::RED, increaseEnergy * parryOtherRatio);
            hdl_RedDodgeSuccess = efk_RedDodgeSuccess->Play(t.Position, {}, 0.15f);
            break;
        case Type::BLUE:
            GetEnergy(Type::BLUE, increaseEnergy * parryOtherRatio);
            hdl_BlueDodgeSuccess = efk_BlueDodgeSuccess->Play(t.Position, {}, 0.15f);
            break;
        default: ;
        }
    }
}


//--------------------<回避中の回転処理>--------------------//
void Player::RotDodge(float elapsedTime)
{
    const float AfterDodgeAngle = BeforeDodgeAngle + DirectX::XMConvertToRadians(360.0f);

    RotRatio += elapsedTime * 1.2f;
    r.Angle.z = Calcu3D::LerpFloat(BeforeDodgeAngle, DirectX::XMConvertToRadians(720.0f), RotRatio);

    if (RotRatio>=0.95f)
    {
        r.Angle.z = BeforeDodgeAngle;
    }
}

//--------------------<エネルギ更新処理>--------------------//

void Player::UpdateEnergy()
{
    //energy補正
    if (EnergyGauge > MaxEnergyCount)EnergyGauge = MaxEnergyCount;
    else if (EnergyGauge < MinEnergyCount)EnergyGauge = MinEnergyCount;

    const GamePadButton transformationButton = GamePad::BTN_RIGHT_TRIGGER | GamePad::BTN_RIGHT_SHOULDER;

    if (GamePad::Instance().GetButtonDown() & transformationButton&&EnemyManager::Instance().IsPlayerTransformable())
    {
        if(EnergyGauge >= MaxEnergyCount)
        {
            // 変形
            IsFinisher = true;
        }
#if 0
        else if (EnergyGauge <= LimitBlueEnergy)
        {
            // 変形せずバフと必殺技のみ


            //エネルギー設定
            constexpr float mulValue = { 0.7f };
            EnergyGauge = static_cast<int>(mulValue * static_cast<float>(LimitRedEnergy));


            // 体力回復
            HitPoint++;
            // 回避可能回数回復
            DodgeCount = MaxDodgeCount;
            // エフェクト再生
            efk_Heal->Play(t.Position, {}, 0.10f);

            //--------------------<体力の上限設定>--------------------//
            HitPoint = min(MaxHitPoint, HitPoint);

            //--------------------<体力の下限設定>--------------------//
            HitPoint = max(HitPoint, 0);
        }
#endif

    }

    if(!EndTransform&&Flags.Check("EndChangeToWing",Model->GetCurrentAnimationNumber(),Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(AnimationName::wing, true);
        mAfterImage.Model->f_PlayAnimation(AnimationName::wing, true);
        EndTransform = true;
        CompleteElement1Tutorial = true;
        AttentionCamera = false;\
    }
    else if(!EndTransform && Flags.Check("EndChangeToHuman", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(AnimationName::wait, true);
        mAfterImage.Model->f_PlayAnimation(AnimationName::wait, true);
        EndTransform = true;
        AttentionCamera = false;
        CompleteElement1Tutorial = true;
    }
    else if(!EndTransform&&Flags.Check("EndBeamMotion",Model->GetCurrentAnimationNumber(),Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(AnimationName::wing, true);
        mAfterImage.Model->f_PlayAnimation(AnimationName::wing, true);
        EndTransform = true;
        CompleteElement1Tutorial = true;
        AttentionCamera = false;
    }
         

}

//--------------------<無敵時間更新>--------------------//
void Player::UpdateInvTime(float elapsedTime)
{
    StackInvincibleTime += elapsedTime;

    // 無敵になってからが一定時間を超えたら
    if (StackInvincibleTime > LimitInvincibleTime)
    {
        IsInvincible = false;
    }
}



//--------------------<射撃入力処理>--------------------//
void Player::InputShooting(float elapsedTime)
{
    // 射撃可能
    if(CanShoot&&!IsFinisher&&!IsSlash)
    {
        //  射撃する
        if (GamePad::Instance().GetButton() & GamePad::BTN_B)
        {
            // デバイスを取得
            const auto& device = SceneManager::Instance().GetDevice();

            Bullet* bullet0{ nullptr };
            Bullet* bullet1{ nullptr };
            Bullet* bullet2{ nullptr };

            // 弾を生成（モードによって射撃される弾を変える）
            switch (type) {
            case Type::NONE:

                break;
            case Type::RED:   // 飛行機
                if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::SIDE)
                {
                    DirectX::XMFLOAT3 launchPosition{ t.Position };
                    launchPosition.z += 7.0f;
                    launchPosition.y += 3.0f;
                    bullet0 = new Bullet(device, BaseProjectile::Parent::REDPLAYER, launchPosition, BulletVelocity      , 30.0f * 1.2f);
                    bullet1 = new Bullet(device, BaseProjectile::Parent::REDPLAYER, launchPosition, BulletVelocityUp    , 30.0f * 1.2f);
                    bullet2 = new Bullet(device, BaseProjectile::Parent::REDPLAYER, launchPosition, BulletVelocityDown  , 30.0f * 1.2f);
                }
                else if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
                {
                    DirectX::XMFLOAT3 launchPosition{ t.Position };
                    launchPosition.z += 7.0f;
                    bullet0 = new Bullet(device, BaseProjectile::Parent::REDPLAYER, launchPosition, BulletVelocity      , 30.0f * 1.2f);
                    launchPosition.x = t.Position.x + 3.0f;
                    bullet1 = new Bullet(device, BaseProjectile::Parent::REDPLAYER, launchPosition, BulletVelocityRight , 30.0f * 1.2f);
                    launchPosition.x = t.Position.x - 3.0f;
                    bullet2 = new Bullet(device, BaseProjectile::Parent::REDPLAYER, launchPosition, BulletVelocityLeft  , 30.0f * 1.2f);
                }

                ProjectileManager::Instance().RegisterProjectile(bullet0);
                ProjectileManager::Instance().RegisterProjectile(bullet1);
                ProjectileManager::Instance().RegisterProjectile(bullet2);

                se.at(SE::RED_BULLET_SHOT)->Stop();
                se.at(SE::RED_BULLET_SHOT)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(RED_BULLET_SHOT));

                Model->f_PlayAnimation(AnimationName::Shot_Recoil);

                break;
            case Type::BLUE:  // 人型
                bullet0 = new Bullet(device, BaseProjectile::Parent::BLUEPLAYER, t.Position, BulletVelocity,50.0f);
                ProjectileManager::Instance().RegisterProjectile(bullet0);

                se.at(SE::BLUE_BULLET_SHOT)->Stop();
                se.at(SE::BLUE_BULLET_SHOT)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(BLUE_BULLET_SHOT));

                DirectX::XMFLOAT3 LaunchPosition{ t.Position };
                LaunchPosition.z += 3.0f;
                LaunchPosition.y -= 1.0f;

                // エフェクトを再生
                hdl_MuzzleFlash = efk_MuzzleFlash->Play(LaunchPosition, {}, 0.1f);


                break;
            default:
                assert("Player Type Is Broken");
                break;
            }

            // 射撃時間をリセット
            StackShootInterval = 0.0f;
            CanShoot = false;

          
        }
    }
    else
    {
        // 射撃可能でなければ
        if(type==Type::BLUE)
        {
            StackShootInterval += elapsedTime * 2.0f;    // 射撃後一定時間経過まで
        }
        else
        {
            StackShootInterval += elapsedTime;     // 射撃後一定時間経過まで
        }

        if(StackShootInterval>=ShootInterval)  // 次の弾を撃てないようにする
        {
            CanShoot = true;
        }
    }

}



//--------------------<移動>--------------------//
void Player::MoveHorizontal(float elapsedTime)
{
    // 回避行動中は専用の動きをする
    const auto moveVec = GetMoveVec();

    if (!ReadyDodge)
    {
        RotUpdate(moveVec.y);

        const float AddSpeedY = Speed * moveVec.y * elapsedTime;
        const float AddSpeedZ = Speed * moveVec.x * elapsedTime;
        t.Position.y += AddSpeedY;
        t.Position.z += AddSpeedZ;
    }
    else
    {
        RotDodge(elapsedTime);
        const float AddSpeedY = Speed * moveVec.y * elapsedTime*1.4f;
        const float AddSpeedZ = Speed * moveVec.x * elapsedTime*1.4f;
        t.Position.y += AddSpeedY;
        t.Position.z += AddSpeedZ;
    }
}
void Player::MoveVertical(float elapsedTime)
{
    // 回避行動中は専用の動きをする
    const auto moveVec = GetMoveVec();

    if (!ReadyDodge)
    {
        RotUpdate(-moveVec.x);

        const float AddSpeedX = Speed * moveVec.x * elapsedTime;
        const float AddSpeedZ = Speed * moveVec.y * elapsedTime;
        t.Position.x += AddSpeedX;
        t.Position.z += AddSpeedZ;
    }
    else
    {
        RotDodge(elapsedTime);
        const float AddSpeedX = Speed * moveVec.x * elapsedTime;
        const float AddSpeedZ = Speed * moveVec.y * elapsedTime;
        t.Position.x += AddSpeedX;
        t.Position.z += AddSpeedZ;
    }
}

//--------------------<プレイヤーの行動範囲制限>--------------------//
void Player::LimitMoveArea()
{
    GameSystem& gameSystem = GameSystem::Instance();

    //衝突判定
    if (gameSystem.GetCameraDirection() == GameSystem::CAMERA_DIRECTION::SIDE)//横スクロール
    {
        // 左の行動制限
        if (LeftTop.x > t.Position.z)
        {
            t.Position.z = LeftTop.x;
        }
        // 右の行動制限
        else if (RightBottom.x < t.Position.z)
        {
            t.Position.z = RightBottom.x;
        }
        // 上の行動制限
        if (LeftTop.y < t.Position.y)
        {
            t.Position.y = LeftTop.y;
        }
        else if (RightBottom.y > t.Position.y)
        {
            t.Position.y = RightBottom.y;
        }
    }
    else if (gameSystem.GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)//縦スクロール
    {
        // 左の行動制限
        if (LeftTop.x > t.Position.x)
        {
            t.Position.x = LeftTop.x;
        }
        // 右の行動制限
        else if (RightBottom.x < t.Position.x)
        {
            t.Position.x = RightBottom.x;
        }
        // 上の行動制限
        if (LeftTop.y < t.Position.z)
        {
            t.Position.z = LeftTop.y;
        }
        else if (RightBottom.y > t.Position.z)
        {
            t.Position.z = RightBottom.y;
        }
    }


    //縦横切り替えの補正
    if(GameSystem::Instance().GetCameraDirection()==GameSystem::CAMERA_DIRECTION::SIDE)
    {
        t.Position.x = 0.0f;
    }
    else if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
    {
        t.Position.y = 0.0f;
    }


}

//--------------------<近距離攻撃>--------------------//
void Player::InputCloseAttack(float elapsedTime)
{
   // // 画面切り替え中は処理しない
   // if (Ga0meSystem::Instance().GetChangingView()) return;

    attackInterval -= elapsedTime;
    StackCloseAttackInterval += elapsedTime;

    if (!ReadyDodge&&StackCloseAttackInterval > CloseAttackInterval)
    {
        if (GamePad::Instance().GetButton() & GamePad::BTN_A)
        {
            if (attackInterval < 0.0f)
            {
                DirectX::XMFLOAT3 efkAngle = { 0.0f,0.0f,0.0f };
                // 攻撃の回転
                if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
                {
                    efkAngle = { 0.0f,0.0f,DirectX::XMConvertToRadians(90.0f) };
                }

                attackInterval = MaxAttackInterval;

                se.at(SE::CLOSE_ATTACK)->Stop();
                se.at(SE::CLOSE_ATTACK)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::CLOSE_ATTACK));

                switch (type)
                {
                case Type::NONE:
                    break;
                case Type::RED:

                    switch (SlashDir)
                    {
                    case Side:
                        Model->f_PlayAnimation(AnimationName::Slash_side);
                        break;
                    case Top:
                        Model->f_PlayAnimation(AnimationName::Slash_top);
                        break;
                    default:;
                    }

                    hdl_RedSlash = efk_RedSlash->Play(t.Position, efkAngle, 0.15f);
                    break;
                case Type::BLUE:
                    hdl_BlueSlash = efk_BlueSlash->Play(t.Position, efkAngle, 0.15f);
                    break;
                default:;
                }

                switch (GameSystem::Instance().GetCameraDirection())
                {
                case GameSystem::CAMERA_DIRECTION::SIDE:
                    SlashDir = Side;
                    break;
                case GameSystem::CAMERA_DIRECTION::TOP:
                    SlashDir = Top;
                    break;
                default:;
                }

                IsSlash = true;
                StackCloseAttackInterval = 0.0f;
            }
        }
    }

    //攻撃中は常に判定を取る
    if (attackInterval > 0.0f)
    {
        const int index = IsHitEnemy(attackArea);
        if (index >= 0)
        {
            Enemy& enemy = EnemyManager::Instance().GetEnemy(index);

			constexpr float attackPower = 1.5f;
            if (enemy.GetType() == type)
            {
                enemy.Damage(attackPower, enemy.GetMaxInvTimer());
            }
            else
            {
                enemy.Damage(attackPower * static_cast<float>(damageRatio), enemy.GetMaxInvTimer());
            }
            Tutorial_IsBreakCloseAttack = true; // チュートリアル用
        }
    }
    else
    {
        IsSlash = false;
    }

    // 攻撃の回転

    if (type == Type::BLUE)
    {
        switch (SlashDir)
        {
        case Side:
            r.Angle.x = Calcu3D::LerpFloat(0.0f, DirectX::XMConvertToRadians(360.0f), 1.0f - attackInterval * 2.0f);
            r.Angle.x = Calcu3D::ClampRotate(r.Angle.x);
            break;
        case Top:
            r.Angle.y = Calcu3D::LerpFloat(0.0f, DirectX::XMConvertToRadians(360.0f), 1.0f - attackInterval * 2.0f);
            r.Angle.y = Calcu3D::ClampRotate(r.Angle.y);
            break;
        default:;
        }

    }
    
    


   
}


//--------------------<必殺技>--------------------//

void Player::SlowForFinisher(float& elapsedTime)
{
    if(IsFinisher)
    {
        elapsedTime *= 0.5f;
    }
}

//--------------------<カメラが追従するかどうか>--------------------//
bool Player::OnCameraTarget_FlingFinisher() const
{
    if (!IsFinisher) return false;
    if (type != Type::RED) return false;
    if (FinisherState != 9) return false;
    return true;
}

void Player::FlyModeFinisher(float elapsedTime)
{
    const float Time= elapsedTime *= 2.0f; // 通常速度に戻す
    DirectX::XMFLOAT3 front = GetFrontVec();
    // この関数内だけの定数
    constexpr float BackSpeed = 40.0f;

    switch (FinisherState)
    {
    case 0:  // 初期化
        FinisherRatio = 1.0f;
        FinisherState++;
        /*FallThrough*/
        break;
    
    case 1:
    {
        const float L_Speed = Calcu3D::LerpFloat(BackSpeed, 0.0f, FinisherRatio);
        t.Position.z -= L_Speed * Time;
        t.Position.y += L_Speed * Time;

        FinisherRatio -= 4.0f * Time;

        r.Angle.x += DirectX::XMConvertToRadians(80.0f) * Time;

        if (r.Angle.x >= DirectX::XMConvertToRadians(20.0f))
        {
            FinisherState++;
        }
        break;
    }
    case 2: // 初期化
        FinisherRatio = 0.0f;
        FinisherState++;
        /*FallThrough*/
        break;
    case 3:
    {
        // 前方方向に進む
        constexpr float EndAngleY = DirectX::XMConvertToRadians(-90.0f);
        constexpr float EndAngleX = DirectX::XMConvertToRadians(0.0f);
        constexpr float BeginSpeed = 50.0f;
        constexpr float EndSpeed = 500.0f;
        FinisherRatio += 2.0f * Time;

        const float speed = Calcu3D::LerpFloat(BeginSpeed, EndSpeed, FinisherRatio);
        r.Angle.y = Calcu3D::LerpFloat(r.Angle.y, EndAngleY, 2.0f * Time);
        r.Angle.x = Calcu3D::LerpFloat(r.Angle.x, EndAngleX, 2.0f * Time);
        t.Position += front * speed * Time;

        if (t.Position.x <= -300.0f)
        {
            FinisherState++;
        }
    }
        break;

    case 4: // 初期化
        FinisherRatio = 0.0f;
        FinisherState++;
        /*FallThrough*/
        break;

    case 5:
    {
        // 上方向に進む
        constexpr float speed = 1500.0f;
        constexpr float EndAngleX = DirectX::XMConvertToRadians(-90.0f);
        r.Angle.x = Calcu3D::LerpFloat(r.Angle.x, EndAngleX, 2.0f * Time);
        t.Position += front * speed * Time;

            if(t.Position.y>=300.0f)
            {
                FinisherState++;
            }
    }
        break;
    case 6: // 初期化
        FinisherRatio = 0.0f;
        FinisherState++;
        /*FallThrough*/
        break;
    case 7:
    {

            // 下りながら加速
        constexpr float BeginSpeed = 1500.0f;
        constexpr float EndSpeed = 500.0f;

        FinisherRatio += 2.0f * Time;
        const float speed = Calcu3D::LerpFloat(BeginSpeed, EndSpeed, FinisherRatio);

        constexpr float EndAngleY = DirectX::XMConvertToRadians(-270.0f);
        constexpr float EndAngleX = DirectX::XMConvertToRadians(90.0f);
        r.Angle.y = Calcu3D::LerpFloat(r.Angle.y, EndAngleY, 3.0f*Time);
        r.Angle.x = Calcu3D::LerpFloat(r.Angle.x, EndAngleX, 3.0f*Time);

        t.Position += front * speed * Time;
        if(t.Position.z<0.0f)
        {
            t.Position.z = 0.0f;
        }

            if(t.Position.y<=100.0f)
            {
                r.Angle.x = XMConvertToRadians(90.0f);
                r.Angle.y = XMConvertToRadians(90.0f);
                t.Position.z = 0.0f;
                FinisherState++;
            }
    }
        break;

    case 8: // 初期化
        FinisherState++;
        FinisherRatio = 0.0f;
        /*FallThrough*/
    case 9:
        //  追従ように0.5秒ストップする
        FinisherRatio += Time;

        if(FinisherRatio>=0.5f)
        {
            FinisherState++;
        }

        break;
    case 10:
        FinisherRatio = 0.0f;
        FinisherState++;
        /*FallThrough*/

    case 11 :
    {
        constexpr float speed = 1000.0f;
        t.Position += front * speed * Time;
        if (t.Position.y <= 5.0f)
        {
            t.Position.y = 5.0f;
            FinisherState++;
        }
    }
        break;
    case 12:
        FinisherState++;
        FinisherRatio = 0.0f;
        r.Angle.x = 0.0f;
        /*FallThrough*/
    case 13:
    {
        constexpr float speed = 1000.0f;
        t.Position += front * speed * Time;
            if(t.Position.x>-2.0f)
            {
                FinisherState++;
            }
            break;
    }

    case 14:
        FinisherState++;
        FinisherRatio = 0.0f;
        r.Angle.x = 0.0f;
        /*FallThrough*/
    case 15:
        t.Position.x += 15.0f * Time;
        if(t.Position.x>7.0f)
        {
            FinisherState++;
        }
        break;
    case 16:
        FinisherState++;
        FinisherRatio = 0.0f;
    case 17:
        t.Position.x += 1000.0f * Time;

        if (t.Position.x > 100.0f)
        {
            FinisherState++;
        }
        break;

    case 18:
        t.Position = {
        0.0f,
        0.0f,
        -100.0f
        };
        r.Angle = {
            0.0f,
            0.0f,
            0.0f
        };
        FinisherState++;
        /*FallThrough*/
    case 19:
        t.Position.z += 50.0f * Time;

        if(t.Position.z>-30.0f)
        {
            FinisherState++;
        }
        break;

    case 20:
        FinisherState = 0;
        IsFinisher = false;
        //enemyにダメージを与える
        AttackAllEnemy(FinisherDamage);
        StackInvincibleTime = 0.0f;
        ChangeMode();
    default:;
    }

}

void Player::FlyModeFinisherMini(float elapsedTime)
{
    const float Time = elapsedTime *= 2.0f; // 通常速度に戻す
    switch (FinisherState)
    {
    case 0:
        // 初期化
        // エフェクトと共に瞬間移動
        t.Position = { -200.0f,100.0f,0.0f };
        r.Angle.x = XMConvertToRadians(90.0f);
        r.Angle.y = XMConvertToRadians(90.0f);
        FinisherRatio = 0.0f;
        FinisherState++;
        /*FallThrough*/
    case 1:
        //  追従ように0.5秒ストップする
        FinisherRatio += Time;

        if (FinisherRatio >= 0.5f)
        {
            FinisherState++;
            FinisherRatio = 0.0f;
        }
        break;
    case 2:
    {
        constexpr float speed = 1000.0f;
        t.Position += GetFrontVec() * speed * Time;
        if (t.Position.y <= 5.0f)
        {
            t.Position.y = 5.0f;
            FinisherState++;
        }
    }
        break;
    case 3:

        break;


    }
}

//--------------------<残像を計算>--------------------//
void Player::UpdateAfterImage(float elapsedTime)
{

    if (ShowAfterImage)
    {
        StackAfterImageTime -= elapsedTime;

        if(StackAfterImageTime<=0.0f)
        {
            ShowAfterImage = false;
        }
    }


    switch (type)
    {
    case Type::NONE: break;
    case Type::RED:
        mAfterImage.Model->SetMaterialColor({ 0.8f,0.8f,0.8f,StackAfterImageTime });
        break;
    case Type::BLUE:
        mAfterImage.Model->SetMaterialColor({ 0.8f,0.8f,0.8f,StackAfterImageTime });
        break;
    default: ;
    }
    

}


void Player::CalcParticle()
{
  


}

void Player::PerformanceDead(float elapsedTime)
{


    StackPerformanceTime += elapsedTime;
    switch (GameSystem::Instance().GetCameraDirection())
    {
    case GameSystem::CAMERA_DIRECTION::SIDE:

        r.Angle.x = Calcu3D::LerpFloat(r.Angle.x, DirectX::XMConvertToRadians(30.0f), 10.0f * elapsedTime);
        t.Position.y -= 30.0f * elapsedTime;
        t.Position.z -= 30.0f * elapsedTime;
        r.Angle.z += DirectX::XMConvertToRadians(360.0f) * elapsedTime;

        if(StackPerformanceTime>=2.0f)
        {
            alive = false;
        }

        break;
    case GameSystem::CAMERA_DIRECTION::TOP: 
        r.Angle.z += DirectX::XMConvertToRadians(360.0f) * elapsedTime;
        t.Position.x-= 30.0f * elapsedTime;
        if (StackPerformanceTime >= 2.0f)
        {
            alive = false;
        }

        break;
    default: ;
    }


}


//--------------------<プレイヤーの最初のポーズ>--------------------//
void Player::FirstPose(float elapsedTime)
{
    

    // 演出設定
    if(IsLightPerformance)
    {

        PerformRate = 0.0f;
        PerformState = 0;
        IsPerforming = false;
        r.Angle.y = 0.0f;
        t.Position = { 0.0f,0.0f,0.0f };
        sprBlackCurtain.Color.w = 0.0f;
        return;
    }


    // アニメーションの更新
    Model->Update(elapsedTime);
    static const auto FirstPosition = t.Position;
    static const DirectX::XMFLOAT3 FirstRadian = {DirectX::XMConvertToRadians(54.0f),DirectX::XMConvertToRadians(120.0f),DirectX::XMConvertToRadians(20.0f), };

    // 演出を飛ばす
    const auto DecideButton = GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_Y | GamePad::BTN_X;
    if(GamePad::Instance().GetButton()&DecideButton)
    {
        // 暗転
        StackSkipTimer += elapsedTime;
    }
    if(StackSkipTimer>=0.3f)
    {
        PreferSkipPerformance = true;
    }

    if(PreferSkipPerformance)
    {
        sprBlackCurtain.Color.w += elapsedTime * 5.0f;
    }
    if(sprBlackCurtain.Color.w>=1.0f)
    {
        sprBlackCurtain.Color.w = 1.0f;
        PerformRate = 0.0f;
        PerformState = 0;
        IsPerforming = false;
        r.Angle.y = 0.0f;
        t.Position = { 0.0f,0.0f,0.0f };
        return;
    }

    switch(PerformState)
    {
    case 0:
        // 初期化
        // 位置
        //t.Position = { -100.0f,50.0f,-100.0f };
        hdl_SonicBoom = efk_SonicBoom->Play(t.Position, {}, 0.2f);
        PerformState++;
        /*FallThrough*/
    case 1:
    {
        // ベジエ曲線を使って位置をほかん
        PerformRate += elapsedTime * 0.75f;
        std::vector<DirectX::XMFLOAT3> vec{};
        vec.emplace_back(FirstPosition);
        vec.emplace_back(DirectX::XMFLOAT3{ -150.0f,40.0f,-300.0f });
        vec.emplace_back(DirectX::XMFLOAT3{ -100.0f,10.0f, -180.0f });
        vec.emplace_back(DirectX::XMFLOAT3{ 0.0f,0.0f,-100.0f });
        t.Position = Calcu3D::BezierCurve(vec, PerformRate);

        PerformanceAngleRatio += elapsedTime * 1.2f;
        const DirectX::XMFLOAT3 EndRad = { Calcu3D::ConvertDegreeToRadian(30.0f,100.0f,60.0f) };
        r.Angle = Calcu3D::LerpFloat3(FirstRadian, EndRad, PerformanceAngleRatio);
        if(PerformanceAngleRatio>=1.0f)
        {
            PerformState++;
            PerformanceAngleRatio = 0.0f;
            DirectX::XMFLOAT3 rad = r.Angle;
            rad = {
                -40.0f,
                -180.0f,
                0.0f
            };
            hdl_SonicBoom = efk_SonicBoom->Play(t.Position, rad, 0.15f);
        }

    }
        break;

    case 2:
    {
        const DirectX::XMFLOAT3 FirstRad2 = { Calcu3D::ConvertDegreeToRadian(30.0f,100.0f,60.0f) };

        // ベジエ曲線を使って位置をほかん
        PerformRate += elapsedTime * 0.5f;
        std::vector<DirectX::XMFLOAT3> vec{};
        vec.emplace_back(FirstPosition);
        vec.emplace_back(DirectX::XMFLOAT3{ -150.0f,40.0f,-300.0f });
        vec.emplace_back(DirectX::XMFLOAT3{ -100.0f,10.0f, -180.0f });
        vec.emplace_back(DirectX::XMFLOAT3{ 0.0f,0.0f,-100.0f });
        t.Position = Calcu3D::BezierCurve(vec, PerformRate);

        PerformanceAngleRatio += elapsedTime * 2.2f;
        const DirectX::XMFLOAT3 EndRad = { Calcu3D::ConvertDegreeToRadian(21.0f,60.0f,310.0f) };
        r.Angle = Calcu3D::LerpFloat3(FirstRad2, EndRad, PerformanceAngleRatio);
        if (PerformanceAngleRatio >= 1.0f)
        {
            PerformState++;
            PerformanceAngleRatio = 0.0f;
            hdl_SonicBoom = efk_SonicBoom->Play(t.Position, EndRad, 0.15f);
        }
    }
        break;
    case 3:
    {
        const DirectX::XMFLOAT3 FirstRad2 = { Calcu3D::ConvertDegreeToRadian(21.0f,60.0f,310.0f) };
        // ベジエ曲線を使って位置をほかん
        PerformRate += elapsedTime * 0.5f;
        std::vector<DirectX::XMFLOAT3> vec{};
        vec.emplace_back(FirstPosition);
        vec.emplace_back(DirectX::XMFLOAT3{ -150.0f,40.0f,-300.0f });
        vec.emplace_back(DirectX::XMFLOAT3{ -100.0f,10.0f, -180.0f });
        vec.emplace_back(DirectX::XMFLOAT3{ 25.0f,0.0f,-100.0f });
        t.Position = Calcu3D::BezierCurve(vec, PerformRate);

        PerformanceAngleRatio += elapsedTime * 2.2f;
        const DirectX::XMFLOAT3 EndRad = { Calcu3D::ConvertDegreeToRadian(0.0f,90.0f,0.0f) };
        r.Angle = Calcu3D::LerpFloat3(FirstRad2, EndRad, PerformanceAngleRatio);
        if (PerformanceAngleRatio >= 1.0f)
        {
            PerformState++;
            PerformanceAngleRatio = 0.0f;
            hdl_SonicBoom = efk_SonicBoom->Play(t.Position, EndRad, 0.15f);
        }
    }
        break;
    case 4:
        // 一瞬スローに
        t.Position.x += 10.0f * elapsedTime;
        PerformRate += elapsedTime;

        if(PerformRate>=0.5f)
        {
            PerformState++;
            PerformRate = 0.0f;
        }
        
        break;
    case 5:
        {
            PerformState++;
        }
        break;
    case 6:
        //      po-zu
        
        PerformState++;
        
    case 7:
        // 更新
        PerformState++;
        break;
    case 8:
    {
            // hennkei
        PerformState++;
    }
        
    case 9:
        // kousinn
        PerformState++;
        break;
    case 10:
       //  syokiitiniidou
    {
        r.Angle.y = Calcu3D::LerpFloat(r.Angle.y, DirectX::XMConvertToRadians(-20.0f), 10.0f * elapsedTime);
        r.Angle.z = Calcu3D::LerpFloat(r.Angle.z, DirectX::XMConvertToRadians(40.0f), 8.0f * elapsedTime);

        DirectX::XMFLOAT3 move = GetFrontVec();
        t.Position += move * 120.0f * elapsedTime;

        if (r.Angle.y <= DirectX::XMConvertToRadians(-19.0f)&&t.Position.x<=0.0f)
        {
            PerformState++;
            r.Angle.y = DirectX::XMConvertToRadians(-20.0f);
            t.Position.x = 0.0f;
        }
    }
        break;
    case 11:
    {
        r.Angle.y = Calcu3D::LerpFloat(r.Angle.y, DirectX::XMConvertToRadians(0.0f), 8.0f * elapsedTime);
        r.Angle.z = Calcu3D::LerpFloat(r.Angle.z, DirectX::XMConvertToRadians(0.0f), 8.0f * elapsedTime);
        DirectX::XMFLOAT3 move = GetFrontVec();
        t.Position += move * 80.0f * elapsedTime;
        if(t.Position.z>0.0f)
        {
            PerformState++;
        }
    }
        break;
    case 12:

        // 最初期化

        PerformRate = 0.0f;
        PerformState = 0;
        IsPerforming = false;
        r.Angle.y = 0.0f;
        break;
        default:
            break;
    }

        
    GuiMenu();
    
}

bool  Player::LastAttack(float elapsedTime)
{
    Model->Update(elapsedTime);

    switch (PerformState)
    {
    case 0:
        PerformState++;
        break;
    case 1:
        // 暗転
        sprBlackCurtain.Color.w += elapsedTime * 5.0f;
        if (sprBlackCurtain.Color.w >= 1.0f)
        {
            PerformState++;
        }
        break;
    case 2:
        // 初期化
        t.Position = { 0.0f,-5.0f,0.0f };
        r.Angle = { 0.0f,0.0f,0.0f };

        PerformState++;
        /*FallThrough*/

    case 3:
        // 暗転明け
        sprBlackCurtain.Color.w -= elapsedTime * 5.0f;
        if (sprBlackCurtain.Color.w <= 0.0f)
        {
            // アニメーションを再生
            Model->f_PlayAnimation(AnimationName::finisher_begin);
            sprBlackCurtain.Color.w = 0.0f;
            PerformState++;
        }
        break;
    case 4:
        if(Flags.Check("LastAttack1",Model->GetCurrentAnimationNumber(),Model->GetCurrentAnimationFrame()))
        {
            PerformState++;
            StackPerformanceTime = 0.0f;
        }

        break;
    case 5:
        t.Position.z += 20.0f * elapsedTime;

        if(Flags.Check("LastAttack0",Model->GetCurrentAnimationNumber(),Model->GetCurrentAnimationFrame()))
        {
            PerformState++;
            StackPerformanceTime = 0.0f;
        }
        break;
    case 6:
        // 暗転
        sprBlackCurtain.Color.w += elapsedTime * 5.0f;
        if (sprBlackCurtain.Color.w >= 1.0f)
        {
            // 初期化
            t.Position = { 0.0f,0.0f,85.0f };
            r.Angle = { 0.0f,0.0f,0.0f };
            se[LastSlash0]->Play(false);
            PerformState++;
        }
        break;
    case 7:
        // 暗転明け
        sprBlackCurtain.Color.w -= elapsedTime * 5.0f;
        if (sprBlackCurtain.Color.w <= 0.0f)
        {
            sprBlackCurtain.Color.w = 0.0f;
            Model->f_PlayAnimation(AnimationName::finisher_end);
            PerformState++;
        }
        break;
    case 8:
        // 待機

        if (Flags.Check("LastAttack2", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            Model->f_PlayAnimation(AnimationName::finisher_end_pose, true);
            PerformState++;
            StackPerformanceTime = 0.0f;
            se[BossBomb0]->Play(false);
            se[BossBomb2]->Play(false);
            se[BossBomb3]->Play(false);
            se[BossBomb4]->Play(false);
            
            
        }
        break;

    case 9:
        // 爆発

        StackPerformanceTime += elapsedTime;
        if(StackPerformanceTime>=1.2f)
        {
            StackPerformanceTime = 0.0f;
            PerformState++;
        }
        
        break;
    case 10:
         // 暗転
        mClearDissolvePower += elapsedTime * 0.8f;
        if (mClearDissolvePower >= 1.0f)
        {
            PerformState++;
        }
        break;
    case 11:
        // 初期化


        return true;
        break;


    default: 
        break;
    }
    
    return  false;
}

//****************************************************************
// 
//  チュートリアル用の関数
// 
//****************************************************************
bool Player::GetIsMove() // 移動しているかどうか
{
    auto mvec = GetMoveVec();
    if(mvec.x>0.01f|| mvec.x < -0.01f || mvec.y>0.01f || mvec.y < -0.01f )
    {
        return true;
    }
    return false;
}


//****************************************************************
    





//--------------------<プレイヤーと弾の衝突判定>--------------------//
void Player::VsProjectile(float elapsedTime)
{
	const int index = IsHitProjectile(hitAreaSize);

    if (index >= 0 && StackDodgeTime >= DodgeTime)
    {

        BaseProjectile& proj = ProjectileManager::Instance().GetProjectile(index);
        const int myType = static_cast<int>(type);
        const int projType = static_cast<int>(proj.GetProjType());

        //弾なら
        if(projType == static_cast<int>(BaseProjectile::ProjType::REDBULLET) || projType == static_cast<int>(BaseProjectile::ProjType::BLUEBULLET))
        {
            const int damage = 1;

            //プレイヤーの色と弾の色が同じ場合
            if (myType == projType)
            {
                if(Damaged(damage))
                {
                    se.at(SE::HIT_SAME_COLOR)->Stop();
                    se.at(SE::HIT_SAME_COLOR)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_SAME_COLOR));
                    hdl_UnWeakDamaged = efk_UnWeakDamaged->Play(proj.GetPosition(), {}, 0.155f);
                    efk_UnWeakDamaged->SetAngle(hdl_UnWeakDamaged, { 0.0f,DirectX::XMConvertToRadians(90.0f),0.0f });

                }
                else
                {
                    se.at(SE::HIT_INVINCIBLE)->Stop();
                    se.at(SE::HIT_INVINCIBLE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_INVINCIBLE));
                }
                
            }
            //違う場合
            else
            {
                if(Damaged(damage * damageRatio))                                     
                {
                    se.at(SE::HIT_DIFFERENT_COLOR)->Stop();
                    se.at(SE::HIT_DIFFERENT_COLOR)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_DIFFERENT_COLOR));
                    efk_WeakDamaged->Play(proj.GetPosition(), {}, 0.04f);
                }
                else
                {
                    se.at(SE::HIT_INVINCIBLE)->Stop();
                    se.at(SE::HIT_INVINCIBLE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_INVINCIBLE));
                }
                
           
            }

            LimitInvincibleTime = 0.5f;

            //弾のHit回数を減らす
            proj.Hit();
        }
        //エネルギーなら
        else if (projType == static_cast<int>(BaseProjectile::ProjType::REDENERGY))
        {
            //赤エネルギー取得
            GetEnergy(Type::RED, increaseEnergy);
            //弾削除
            ProjectileManager::Instance().Remove(&proj);
        }
        else if (projType == static_cast<int>(BaseProjectile::ProjType::BLUEENERGY))
        {
            //青エネルギー取得
            GetEnergy(Type::BLUE, increaseEnergy);
            //弾削除
            ProjectileManager::Instance().Remove(&proj);
        }
    }
}
int Player::IsHitProjectile(const DirectX::XMFLOAT3 HitAreaSize) const
{
    if (dodgeSuccess)return -1;

    int hitProjIndex = -1;

	const int projCount = ProjectileManager::Instance().GetProjectileCount();
    for (int i = 0; i < projCount; i++)
    {
        BaseProjectile& proj = ProjectileManager::Instance().GetProjectile(i);

        const int myType = static_cast<int>(type);
        const int projType = static_cast<int>(proj.GetProjType());

        //弾の親がプレイヤーならcontinue
        if (proj.GetParent() == BaseProjectile::Parent::BLUEPLAYER || proj.GetParent() == BaseProjectile::Parent::REDPLAYER)continue;

        GameSystem& gameSystem = GameSystem::Instance();

        //衝突判定
        if (Collision::CubeVsCube(t.Position, HitAreaSize, proj.GetPosition(), proj.GetHitAreaSize()))
        {
            if (hitProjIndex < 0)hitProjIndex = i;
            else
            {
                if (Collision::Distance(t.Position, proj.GetPosition()) < Collision::Distance(t.Position, ProjectileManager::Instance().GetProjectile(hitProjIndex).GetPosition()))
                {
                    hitProjIndex = i;
                }
            }
        }
	        
    }
    return hitProjIndex;
}


//--------------------<プレイヤーと敵の衝突判定>--------------------//
void Player::VsEnemy(float elapsedTime)
{
    const int index = IsHitEnemy(hitAreaSize);

    if (index >= 0 && StackDodgeTime >= DodgeTime)
    {
	    const Enemy& enemy = EnemyManager::Instance().GetEnemy(index);

        const int damage = 1;

    	//プレイヤーの色と敵の色が同じなら
        if (type == enemy.GetType())
        {
            if (Damaged(damage))
            {
                se.at(SE::HIT_SAME_COLOR)->Stop();
                se.at(SE::HIT_SAME_COLOR)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_SAME_COLOR));
            }
            else
            {
                se.at(SE::HIT_INVINCIBLE)->Stop();
                se.at(SE::HIT_INVINCIBLE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_INVINCIBLE));
            }
        }
        //違う場合
        else
        {
            if (Damaged(damage * damageRatio))
            {
                se.at(SE::HIT_DIFFERENT_COLOR)->Stop();
                se.at(SE::HIT_DIFFERENT_COLOR)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_DIFFERENT_COLOR));
            }
            else
            {
                se.at(SE::HIT_INVINCIBLE)->Stop();
                se.at(SE::HIT_INVINCIBLE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::HIT_INVINCIBLE));
            }
        }

        //ダメージ処理
        LimitInvincibleTime = 2.0f;
    }
    
}
int Player::IsHitEnemy(const DirectX::XMFLOAT3 HitAreaSize) const
{
    if (dodgeSuccess)return -1;

    int hitEneIndex = -1;

    const int eneCount = EnemyManager::Instance().GetEnemyCount();
    for (int i = 0; i < eneCount; i++)
    {
        Enemy& enemy = EnemyManager::Instance().GetEnemy(i);

        GameSystem& gameSystem = GameSystem::Instance();

        //登場中なら当たらない
        if (enemy.IsAppear())continue;

        //衝突判定
        if (Collision::CubeVsCube(t.Position, HitAreaSize, enemy.GetPosition(), enemy.GetHitAreaSize() ))
        {
            if (hitEneIndex < 0) hitEneIndex = i;
            else
            {
                if (Collision::Distance(t.Position, enemy.GetPosition()) < Collision::Distance(t.Position, EnemyManager::Instance().GetEnemy(hitEneIndex).GetPosition()))
                {
                    hitEneIndex = i;
                }
            }
        }
    }

    return hitEneIndex;
}
void Player::AttackAllEnemy(float damage)
{
	for(int i = 0;i<EnemyManager::Instance().GetEnemyCount();i++)
	{
        Enemy& enemy = EnemyManager::Instance().GetEnemy(i);
        enemy.Damage(damage, enemy.GetMaxInvTimer(), true);
	}
}

void Player::fSetCameraShakeFunc(std::function<void()> Function_)
{
    mCameraShakeFunc = Function_;
}


//--------------------<縦横切り替えの際の補正>--------------------//
void Player::ChangeCameraDirCorrect()
{
	if(GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::SIDE)
	{
		//side->top
        t.Position.y = Calcu3D::LerpFloat(t.Position.y, 0.0f, 0.1f);
    }
    else if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
    {
        //top->side
        t.Position.x = Calcu3D::LerpFloat(t.Position.x, 0.0f, 0.1f);
    }
}

//--------------------<ヒットストップ>--------------------//
void Player::HitSlow(float& elapsedTime)
{
    StackHitStopSeconds += elapsedTime;

    if (StackHitStopSeconds < HitStopSecond)
    {
        elapsedTime *= 0.2f;
    }
}

void Player::Slow(float &elapsedTime)
{
    elapsedTime *= 0.3f;
}

//****************************************************************
// 
//  ビームの必殺技
// 
//****************************************************************
void Player::Finisher_Beam(float elapsedTime)
{
    switch (FinisherState)
    {
    case 0:
        // カメラを注目させる
        AttentionCamera = true;
        // 演出中の枠を表示する
        ProjectileManager::Instance().Clear();
        // 角度を変える
        r.Angle.y = DirectX::XMConvertToRadians(60.0f);
        r.Angle.x = 0.0f;
        r.Angle.z = 0.0f;

        // カットイン開始
        spr_CutIn0.Position.x = Calcu3D::LerpFloat(spr_CutIn0.Position.x, 0.0f, 50.0f * elapsedTime);
        if(spr_CutIn0.Position.x<2.0f)
        {
            spr_CutIn0.Position.x = 0.0f;
            FinisherState++;
        }
        break;
    case 1:
        // 待機時間
        FinisherRatio += elapsedTime;
        if(FinisherRatio>=0.4f)
        {
            FinisherRatio = 0.0f;
            FinisherState++;
        }
        break;
    case 2:
        // カットイン開始
        spr_CutIn0.Position.x = Calcu3D::LerpFloat(spr_CutIn0.Position.x, -1280.0f, 50.0f * elapsedTime);
        if (spr_CutIn0.Position.x < -1275.0f)
        {
            spr_CutIn0.Position.x = -1275.0f;
            FinisherState++;
        }
        break;
    case 3:
        // 初期化
        // カメラを注目させる
        AttentionCamera = true;
        // 演出中の枠を表示する

        // 角度を変える
        r.Angle.y = DirectX::XMConvertToRadians(60.0f);
        r.Angle.x = 0.0f;
        r.Angle.z = 0.0f;

        // アニメーションを再生
        Model->f_PlayAnimation(AnimationName::FinisherBeamBegin, false);
        FinisherState++;

        /*FallThrough*/
        break;
    case 4:
        // 変形終了と共にため動作
        if(Flags.Check("EndStartBeamMotion", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            FinisherState++;
        }

        break;
    case 5:
        // ため動作
        Model->f_PlayAnimation(AnimationName::FinisherBeamLoop, true);
        se[BEAMCHARGE]->Play(false, OptionSystem::Instance().GetSeVolume());
        hdl_Charge = efk_Charge->Play(t.Position, {}, 0.2f);
        FinisherState++;
        /*Fallthrough*/

    case 6:
        // 溜め終わり
        FinisherRatio += elapsedTime;
        if (FinisherRatio >= 1.0f)
        {
            FinisherState++;
            FinisherRatio = 0.0f;
        }
        break;
    case 7:
    {
        // ビーム打つ
        se[BEAMCHARGE]->Stop();
        se[BEAM]->Play(false, OptionSystem::Instance().GetSeVolume());

        ID3D11Device* p_device = SceneManager::Instance().GetDevice();

        Light->SetRange(-4.0f);
        auto b0 = new Beam(p_device, BaseProjectile::Parent::REDPLAYER,
            t.Position);

        ProjectileManager::Instance().RegisterProjectile(b0);

        // カメラを初期化
        AttentionCamera = false;

        // 角度を初期化
        r.Angle.y = 0.0f;
        FinisherState++;
    }
        break;
    case 8:
        // ビーム撃ち終わり
        FinisherRatio += elapsedTime;
        if (FinisherRatio >= 3.0f)
        {
            Model->f_PlayAnimation(AnimationName::FinisherBeamEnd);
            FinisherState++;
            FinisherRatio = 0.0f;
            Light->SetRange(30.0f);
        }
        break;
    case 9:
        AttentionCamera = true;
        // 最初期化
        FinisherState = 0;
        IsFinisher = false;
        StackInvincibleTime = 0.0f;
        type = Type::BLUE;
        // アニメーション
        EndTransform = false;
        // 体力回復
        HitPoint += HealHpCount;
        // 回避可能回数回復
        DodgeCount = MaxDodgeCount;
        se[BEAM]->Stop();

        //エネルギー設定
        EnergyGauge = MinEnergyCount;

        se.at(SE::MODE_CHANGE)->Stop();
        se.at(SE::MODE_CHANGE)->Play(false, OptionSystem::Instance().GetSeVolume()* volumes.at(SE::MODE_CHANGE));

        // エフェクト再生
        efk_Heal->Play(t.Position, {}, 0.10f);

        //UIに変わったことを知らせる
        GameSystem::Instance().SetIsChangeColor(true);

        //se再生
        se.at(HEAL)->Stop();
        se.at(HEAL)->Play(false, OptionSystem::Instance().GetSeVolume()* volumes.at(HEAL));

        //--------------------<体力の上限設定>--------------------//
        HitPoint = min(MaxHitPoint, HitPoint);

        //--------------------<体力の下限設定>--------------------//
        HitPoint = max(HitPoint, 0);
        // 弾をすべて消す
        ProjectileManager::Instance().Clear();

        break;
    default: 
        break;
    }


}

void Player::Finisher_BeamLight(float elapsedTime)
{
    switch (FinisherState)
    {
    case 0:
        // カットイン開始
        spr_CutIn0.Position.x = Calcu3D::LerpFloat(spr_CutIn0.Position.x, 0.0f, 50.0f * elapsedTime);
        ProjectileManager::Instance().Clear();
        if (spr_CutIn0.Position.x < 2.0f)
        {
            spr_CutIn0.Position.x = 0.0f;
            FinisherState++;
        }
        break;
    case 1:
        // 待機時間
        FinisherRatio += elapsedTime;
        if (FinisherRatio >= 0.2f)
        {
            Model->f_PlayAnimation(AnimationName::FinisherBeamLoop, true);
            FinisherRatio = 0.0f;
            FinisherState++;
        }
        break;
    case 2:
        // カットイン開始
        spr_CutIn0.Position.x = Calcu3D::LerpFloat(spr_CutIn0.Position.x, -1280.0f, 50.0f * elapsedTime);
        if (spr_CutIn0.Position.x < -1275.0f)
        {
            spr_CutIn0.Position.x = -1275.0f;
            FinisherState++;
        }
        break;
    case 3:
    
        FinisherState++;
        /*FallThrough*/
        break;
    case 4:
        // ビーム打つ
    {
        se[BEAM]->Play(false, OptionSystem::Instance().GetSeVolume());
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        Light->SetRange(-4.0f);
        auto b0 = new Beam(p_device, BaseProjectile::Parent::REDPLAYER,
            t.Position);
        ProjectileManager::Instance().RegisterProjectile(b0);
        FinisherRatio = 0.0f;
        FinisherState++;
    }
        /*FallThrough*/
    case 5:
       // ビーム撃ち終わり
        FinisherRatio += elapsedTime;
        if (FinisherRatio >= 3.0f)
        {
            FinisherState++;
            FinisherRatio = 0.0f;
            Light->SetRange(30.0f);
            Model->f_PlayAnimation(AnimationName::FinisherBeamEnd);
        }
        break;

    case 6:
      
        // 最初期化
        FinisherState = 0;
        IsFinisher = false;
        StackInvincibleTime = 0.0f;
        type = Type::BLUE;
        // アニメーション
        EndTransform = false;
        // 体力回復
        HitPoint += HealHpCount;
        // 回避可能回数回復
        DodgeCount = MaxDodgeCount;

        //UIに変わったことを知らせる
        GameSystem::Instance().SetIsChangeColor(true);


        //エネルギー設定
        EnergyGauge = MinEnergyCount;

        se.at(SE::MODE_CHANGE)->Stop();
        se.at(SE::MODE_CHANGE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::MODE_CHANGE));

        // エフェクト再生
        efk_Heal->Play(t.Position, {}, 0.10f);

        //--------------------<体力の上限設定>--------------------//
        HitPoint = min(MaxHitPoint, HitPoint);

        //--------------------<体力の下限設定>--------------------//
        HitPoint = max(HitPoint, 0);
        // 弾をすべて消す
        ProjectileManager::Instance().Clear();
        break;
    default:
        break;
    }

}




void Player::Finisher_Slash(float elapsedTime)
{
    static DirectX::XMFLOAT3 StartPosition{};  // 斬る位置

    switch (FinisherState)
    {
    case 0:
        // カットイン開始
        spr_CutIn1.Position.x = Calcu3D::LerpFloat(spr_CutIn1.Position.x, 0.0f, 50.0f * elapsedTime);
        ProjectileManager::Instance().Clear();
        EnemiesStop = true;
        if (spr_CutIn1.Position.x < 2.0f)
        {
            spr_CutIn1.Position.x = -20.0f;
            FinisherState++;
        }
        break;
    case 1:
        // 待機時間
        FinisherRatio += elapsedTime;
        if (FinisherRatio >= 0.4f)
        {
            FinisherRatio = 0.0f;
            FinisherState++;
        }
        break;
    case 2:
        // カットイン終了
        spr_CutIn1.Position.x = Calcu3D::LerpFloat(spr_CutIn1.Position.x, -1280.0f, 50.0f * elapsedTime);
        if (spr_CutIn1.Position.x < -1275.0f)
        {
            spr_CutIn1.Position.x = -1275.0f;
            FinisherState++;
        }
        break;
    case 3:
        // 初期化
        StartPosition = t.Position;
        FinisherRatio = 0.0f;
        FinisherState++;
    case 4:
        FinisherRatio += elapsedTime * 2.0f;

        // 決まった位置に移動
        t.Position = Calcu3D::LerpFloat3(StartPosition, { 0.0f,0.0f,-15.0f }, FinisherRatio);
        if(FinisherRatio>=1.0f)
        {
            Model->f_PlayAnimation(AnimationName::inv_transform);
            mAfterImage.Model->f_PlayAnimation(AnimationName::inv_transform);
            FinisherState++;
            FinisherRatio = 0.0f;
        }
        break;
    case 5:
        if(Flags.Check("EndChangeToHuman", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            FinisherState++;
            FinisherRatio = 0.0f;
        }
        break;
    case 6:
        // 初期化
        // カメラを注目させる
        AttentionCamera = true;
        // 演出中の枠を表示する
        // 角度を変える
        r.Angle.y = DirectX::XMConvertToRadians(60.0f);
        r.Angle.x = 0.0f;
        r.Angle.z = 0.0f;

        // アニメーションを再生
        Model->f_PlayAnimation(AnimationName::FinisherSwordBegin, false);
        mAfterImage.Model->f_PlayAnimation(AnimationName::FinisherSwordBegin, false);
        FinisherState++;

        /*FallThrough*/
        break;
    case 7:
        // 変形終了と共にため動作
        if (Flags.Check("a", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            Model->f_PlayAnimation(AnimationName::FinisherSwordSlash, false);
            mAfterImage.Model->f_PlayAnimation(AnimationName::FinisherSwordSlash, false);
            FinisherState++;
        }

        break;
    case 8:
            FinisherState++;
        break;

    case 9:
        // 溜め終わり
        if (Flags.Check("PlaySlashEffect", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            // エフェクト再生
            efk_SlashFinisher->Play(t.Position, { 0.0f,DirectX::XMConvertToRadians(-90.0f),DirectX::XMConvertToRadians(160.0f) }, 0.25f);
            se[BEAM]->Play(false, OptionSystem::Instance().GetSeVolume());
            FinisherState++;
        }
        break;
    case 10:
    {
      
        // カメラを初期化
        AttentionCamera = false;
        // 角度を初期化
        r.Angle.y = 0.0f;
        EnemiesStop = false;
        FinisherState++;
    }
    break;
    case 11:
        FinisherRatio += elapsedTime;
        if (Flags.Check("EndSlashFinisher", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            //enemyにダメージを与える
            AttackAllEnemy(FinisherDamage);
            FinisherState++;
            FinisherRatio = 0.0f;
            Light->SetRange(30.0f);
            Model->f_PlayAnimation(AnimationName::wait, true);
            mAfterImage.Model->f_PlayAnimation(AnimationName::wait, true);
        }
        break;
    case 12:
        // 最初期化
        IsFinisher = false;
        FinisherState = 0;
        StackInvincibleTime = 0.0f;
        // 体力回復
        HitPoint += HealHpCount;
        // 回避可能回数回復
        DodgeCount = MaxDodgeCount;
        CompleteElement1Tutorial = true;

        //エネルギー設定
        EnergyGauge = MinEnergyCount;

        se.at(SE::MODE_CHANGE)->Stop();
        se.at(SE::MODE_CHANGE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::MODE_CHANGE));

        // エフェクト再生
        efk_Heal->Play(t.Position, {}, 0.10f);

        //UIに変わったことを知らせる
        GameSystem::Instance().SetIsChangeColor(true);


        //--------------------<体力の上限設定>--------------------//
        HitPoint = min(MaxHitPoint, HitPoint);

        //--------------------<体力の下限設定>--------------------//
        HitPoint = max(HitPoint, 0);
        // 弾をすべて消す
        ProjectileManager::Instance().Clear();
        type = Type::RED;
        break;
    default:
        break;
    }


}


void Player::Finisher_SlashLight(float elapsedTime)
{
    static DirectX::XMFLOAT3 StartPosition{};  // 斬る位置

    switch (FinisherState)
    {
    case 0:
        // カットイン開始
        spr_CutIn1.Position.x = Calcu3D::LerpFloat(spr_CutIn1.Position.x, 0.0f, 50.0f * elapsedTime);
        ProjectileManager::Instance().Clear();
        EnemiesStop = true;
        if (spr_CutIn1.Position.x < 2.0f)
        {
            spr_CutIn1.Position.x = -20.0f;
            FinisherState++;
        }
        break;
    case 1:
        // 待機時間
        FinisherRatio += elapsedTime;
        if (FinisherRatio >= 0.2f)
        {
            FinisherRatio = 0.0f;
            FinisherState++;
        }
        break;
    case 2:
        // カットイン終了
        spr_CutIn1.Position.x = Calcu3D::LerpFloat(spr_CutIn1.Position.x, -1280.0f, 50.0f * elapsedTime);
        if (spr_CutIn1.Position.x < -1275.0f)
        {
            spr_CutIn1.Position.x = -1275.0f;
            FinisherState++;
        }
        break;
    case 3:
        // 初期化
        StartPosition = t.Position;
        FinisherRatio = 0.0f;
        FinisherState++;
    case 4:
        FinisherRatio += elapsedTime * 4.0f;

        // 決まった位置に移動
        t.Position = Calcu3D::LerpFloat3(StartPosition, { 0.0f,0.0f,-15.0f }, FinisherRatio);
        if (FinisherRatio >= 1.0f)
        {
            se[BEAM]->Play(false, OptionSystem::Instance().GetSeVolume());
            Model->f_PlayAnimation(AnimationName::inv_transform);
            mAfterImage.Model->f_PlayAnimation(AnimationName::inv_transform);
            FinisherState++;
            FinisherRatio = 0.0f;
        }
        break;
    case 5:
        if (Flags.Check("EndChangeToHuman", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            FinisherState++;
            FinisherRatio = 0.0f;
        }
        break;
    case 6:
        // 初期化
        // 演出中の枠を表示する
        // 角度を変える
        r.Angle.y = DirectX::XMConvertToRadians(0.0f);
        r.Angle.x = 0.0f;
        r.Angle.z = 0.0f;

        // アニメーションを再生
        Model->f_PlayAnimation(AnimationName::FinisherSwordSlash, false);
        mAfterImage.Model->f_PlayAnimation(AnimationName::FinisherSwordSlash, false);
        FinisherState++;

        /*FallThrough*/
        break;
    case 7:
      
            FinisherState++;

        break;
    case 8:
        
        FinisherState++;
        break;

    case 9:
        // 溜め終わり
        if (Flags.Check("PlaySlashEffect", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            // エフェクト再生
            efk_SlashFinisher->Play(t.Position, { 0.0f,DirectX::XMConvertToRadians(-90.0f),DirectX::XMConvertToRadians(160.0f) }, 0.25f);
            FinisherState++;
        }
        break;
    case 10:
    {

        // カメラを初期化
        AttentionCamera = false;
        // 角度を初期化
        r.Angle.y = 0.0f;
        EnemiesStop = false;
        FinisherState++;
    }
    break;
    case 11:
        FinisherRatio += elapsedTime;
        if (Flags.Check("EndSlashFinisher", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
        {
            //enemyにダメージを与える
            AttackAllEnemy(FinisherDamage);
            FinisherState++;
            FinisherRatio = 0.0f;
            Light->SetRange(30.0f);
            Model->f_PlayAnimation(AnimationName::wait, true);
            mAfterImage.Model->f_PlayAnimation(AnimationName::wait, true);
        }
        break;
    case 12:
        // 最初期化
        IsFinisher = false;
        FinisherState = 0;
        StackInvincibleTime = 0.0f;
        // 体力回復
        HitPoint += HealHpCount;
        // 回避可能回数回復
        DodgeCount = MaxDodgeCount;
        CompleteElement1Tutorial = true;

        //エネルギー設定
        EnergyGauge = MinEnergyCount;

        se.at(SE::MODE_CHANGE)->Stop();
        se.at(SE::MODE_CHANGE)->Play(false, OptionSystem::Instance().GetSeVolume() * volumes.at(SE::MODE_CHANGE));

        // エフェクト再生
        efk_Heal->Play(t.Position, {}, 0.10f);


        //UIに変わったことを知らせる
        GameSystem::Instance().SetIsChangeColor(true);


        //--------------------<体力の上限設定>--------------------//
        HitPoint = min(MaxHitPoint, HitPoint);

        //--------------------<体力の下限設定>--------------------//
        HitPoint = max(HitPoint, 0);
        // 弾をすべて消す
        ProjectileManager::Instance().Clear();
        type = Type::RED;
        break;
    default:
        break;
    }

}


//--------------------<ImGuiのデバッグ>--------------------//
void Player::GuiMenu()
{

#ifdef _DEBUG
    ImGui::Begin("Player");

    ImGui::Checkbox("Debug", &IsDebug);

    ImGui::SliderFloat("Ratio", &PerformRate, 0.0f, 1.0f);

    if (ImGui::Button("ChangeModel"))
    {
        ChangeMode();
    }
    //位置
    ImGui::DragFloat3("Position", &t.Position.x);

    DirectX::XMFLOAT3 deg = {
        XMConvertToDegrees(r.Angle.x),
        XMConvertToDegrees(r.Angle.y),
        XMConvertToDegrees(r.Angle.z)
    };
    ImGui::DragFloat3("Angle", &deg.x);

    r.Angle = {
        XMConvertToRadians(deg.x),
        XMConvertToRadians(deg.y),
        XMConvertToRadians(deg.z)
    };




    // 体力と速度の相関
    ImGui::SliderInt("HitPoint", &HitPoint,0,15);
    ImGui::SliderFloat("Speed", &Speed, MinSpeed, MaxSpeed, "%.1f");

    // 射撃
    ImGui::Checkbox("CanShoot", &CanShoot); //射撃可能
    ImGui::InputFloat("StackShootInterval", &StackShootInterval);

    //エネルギー
    ImGui::SliderInt("EnergyGauge", &EnergyGauge, MinEnergyCount, MaxEnergyCount);
    if (ImGui::Button("GetRedEnergy"))
    {
        GetEnergy(Type::RED, increaseEnergy);
    }
    if (ImGui::Button("GetBlueEnergy"))
    {
        GetEnergy(Type::BLUE, increaseEnergy);
    }
    ImGui::SliderInt("increaseEnergy", &increaseEnergy, 0, 10);

    // ダメージ
    if (ImGui::Button("Damage"))
    {
        Damaged(1);
    }

    // 無敵時間
    ImGui::Checkbox("IsInvincible", &IsInvincible);

    // 回避
    ImGui::Checkbox("ReadyDodge", &ReadyDodge);
    ImGui::InputInt("DodgeCount", &DodgeCount);
    ImGui::Checkbox("DodgeSuccess", &dodgeSuccess);

    ImGui::Checkbox("Finisher", &IsFinisher);
    ImGui::InputInt("FinisherState", &FinisherState);
    if (ImGui::Button("Laser"))
    {
        //Model->f_PlayAnimation(AnimationName::laser);
    }

    if (ImGui::Button("BeamFinisher"))
    {
        IsFinisher = true;
    }

    ImGui::Checkbox("LightFinisher", &IsLightPerformance);

    ImGui::InputFloat("StackDodgeTime", &StackDodgeTime);

    ImGui::End();
#endif

}




