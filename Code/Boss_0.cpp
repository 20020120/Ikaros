#include"GameSystem.h"
#include"EnemyManager.h"
#include "Boss_0.h"

#include "Bullet.h"
#include "Collision.h"
#include"Energy.h"
#include"ProjectileManager.h"
#include "SceneManager.h"

#include"MathSupport.h"
Boss_0::Boss_0(ID3D11Device* Device)
    :Enemy(Device, "./resources/Models/Characters/Boss/BossWhite.nk", "./resources/Shaders/PhisicsBase_PS.cso")
{
    Initialize();

    ModelRenderInitializer Ini;
    Ini.SetALLPath(
        "./resources/Models/Characters/Boss/Laser.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/PhisicsBase_PS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );
    mBeamEmitter.Model = std::make_unique<ModelComponent>(Device, Ini);
    mBeamEmitter.t = t;
    mBeamEmitter.r = r;

    //--------------------<イベントフラグの初期化>--------------------//
    laserFlag.Load("./resources/Models/Characters/Boss/BossLaserFlag.flg");

    //--------------------<ポイントライト初期化>--------------------//
    mPointLight = std::make_unique<GamePointLight>(Device,4);
    mPointLight->SetColor({ 1.0f,0.0f,0.0f });

    ZoomPower = 0.0f;
    ZoomUp = true;
    StackZoomPower = 0.0f;
    // 登場
    EnemyManager::Instance().SetStartBossAppear(true);
    EnemyManager::Instance().SetStartBossBGM(true);
}
Boss_0::~Boss_0()
{
    Finalize();
}


void Boss_0::Initialize()
{
    IsPerformance = false;
    // 位置を初期化
    t.Position = { 0.0f,0.0f,90.0f };
    // 角度を初期化
    r.Angle = { 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f };

    // 大きさを初期化
    t.Scale = { 0.04f,0.04f,0.04f };
    EnemyManager::Instance().SetCameraFocusBoss(true);
    EnemyManager::Instance().SetTargetPosition(t.Position);
    
    hitAreaSize = { 0.0f,0.0f,0.0f };

    status.hitPoint = MaxHitPoint;

    CompleteAttack = true;

    // 状態を設定
    CurrentMode = TOP;

    maxInvTimer = 0.3f;

    ShowBeamEmitter = false;

    LightRange = { 0.0f };
    LightColor = { 1.0f,0.0f,0.0f };
    
    type = Type::RED;

    IsLastPerformance = false;

    // 弾を一回全部消す
    ProjectileManager::Instance().Clear();

    //****************************************************************
    // 
    // 関数ポインタを初期化
    // 
    //****************************************************************

    MoveArray[0][0] = {&Boss_0::S_Stop}; // サイド
    MoveArray[0][1] = {&Boss_0::S_Move}; // サイド
    MoveArray[1][0] = {&Boss_0::T_Stop}; // トップ
    MoveArray[1][1] = {&Boss_0::T_Move}; // トップ

    //****************************************************************
    // 
    // サイドの攻撃
    // 
    //****************************************************************

    std::vector<pBehavior> vec{};

    vec.emplace_back(&Boss_0::T_MoveFirstPosition);
    vec.emplace_back(&Boss_0::T_NagiriEnergy);
    vec.emplace_back(&Boss_0::EndAttack);
    TopBehaviorVec.emplace_back(vec);
    vec.clear();

    //--------------------<薙ぎ払い->薙ぎ払い->薙ぎ払い>--------------------//
    vec.emplace_back(&Boss_0::T_NagiriAttack);
    vec.emplace_back(&Boss_0::T_MoveShot);
    vec.emplace_back(&Boss_0::T_NagiriEnergy);
    vec.emplace_back(&Boss_0::EndAttack);
    TopBehaviorVec.emplace_back(vec);
    vec.clear();


    //--------------------<薙ぎ払い->全体攻撃>--------------------//
    vec.emplace_back(&Boss_0::T_AllRangeAttack);
    vec.emplace_back(&Boss_0::T_NagiriAttack);
    vec.emplace_back(&Boss_0::EndAttack);
    TopBehaviorVec.emplace_back(vec);
    vec.clear();

    //--------------------<ホーミング弾->薙ぎ払いエネルギー>--------------------//
    vec.emplace_back(&Boss_0::T_Horming);
    vec.emplace_back(&Boss_0::T_NagiriAttack);
    vec.emplace_back(&Boss_0::EndAttack);
    TopBehaviorVec.emplace_back(vec);
    vec.clear();

    //--------------------<休憩タイム>--------------------//
    vec.emplace_back(&Boss_0::T_NagiriEnergy);
    vec.emplace_back(&Boss_0::EndAttack);
    TopBehaviorVec.emplace_back(vec);
    vec.clear();

    //****************************************************************
    // 
    // 発狂後のトップ
    // 
    //****************************************************************

    //--------------------<薙ぎ払い＆ホーミング->エネルギー>--------------------//
    vec.emplace_back(&Boss_0::T_V_Homing);
    vec.emplace_back(&Boss_0::T_V_NagiriEnergy);
    vec.emplace_back(&Boss_0::EndAttack);
    TopBehaviorVec.emplace_back(vec);
    vec.clear();

    //--------------------<全体攻撃＆薙ぎ払い->ホーミング>--------------------//
    vec.emplace_back(&Boss_0::T_V_AllRangeAttack);
    vec.emplace_back(&Boss_0::T_V_NagiriAttack);
    vec.emplace_back(&Boss_0::T_MoveShot);
    vec.emplace_back(&Boss_0::EndAttack);
    TopBehaviorVec.emplace_back(vec);
    vec.clear();


    //--------------------<ホーミング->ビーム>--------------------//
    vec.emplace_back(&Boss_0::T_V_Beam);
    vec.emplace_back(&Boss_0::T_V_NagiriAttack);
    vec.emplace_back(&Boss_0::EndAttack);
    TopBehaviorVec.emplace_back(vec);
    vec.clear();

    //--------------------<コンボ攻撃>--------------------//
    vec.emplace_back(&Boss_0::T_V_SpecialAttack);
    vec.emplace_back(&Boss_0::EndAttack);
    TopBehaviorVec.emplace_back(vec);
    vec.clear();
    

    //****************************************************************
    // 
    // サイドの攻撃
    // 
    //****************************************************************

    // 初期攻撃
    vec.emplace_back(&Boss_0::S_MoveFirstPosition);

    // 薙ぎ払い三連
    vec.emplace_back(&Boss_0::S_NagiriAttack);
    vec.emplace_back(&Boss_0::EndAttack);
    SideBehaviorVec.emplace_back(vec);
    vec.clear();


    // ホーミング弾
    vec.emplace_back(&Boss_0::S_Horming);
    vec.emplace_back(&Boss_0::S_Horming);
    vec.emplace_back(&Boss_0::EndAttack);
    SideBehaviorVec.emplace_back(vec);
    vec.clear();


    // 休憩タイム
    vec.emplace_back(&Boss_0::S_NagiriEnergy);
    vec.emplace_back(&Boss_0::S_NagiriEnergy);
    vec.emplace_back(&Boss_0::S_CrossNagiriAttack);
    vec.emplace_back(&Boss_0::EndAttack);

    SideBehaviorVec.emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_0::S_CrossNagiriAttack);
    vec.emplace_back(&Boss_0::S_Horming);
    vec.emplace_back(&Boss_0::S_NagiriEnergy);
    vec.emplace_back(&Boss_0::EndAttack);

    SideBehaviorVec.emplace_back(vec);
    vec.clear();

    // シンプルなビーム
    vec.emplace_back(&Boss_0::S_Beam);
    vec.emplace_back(&Boss_0::S_NagiriAttack);
    vec.emplace_back(&Boss_0::EndAttack);
    SideBehaviorVec.emplace_back(vec);
    vec.clear();

    vec.emplace_back(&Boss_0::S_CrossNagiriAttack);
    vec.emplace_back(&Boss_0::EndAttack);

    SideBehaviorVec.emplace_back(vec);
    vec.clear();

    //****************************************************************
    // 
    // 発狂後のサイド
    // 
    //****************************************************************

    // コンボ攻撃

    // 両方から薙ぎ払い攻撃->ビーム
    vec.emplace_back(&Boss_0::S_V_CrossNagiriAttack);
    vec.emplace_back(&Boss_0::S_Beam);
    vec.emplace_back(&Boss_0::EndAttack);
    SideBehaviorVec.emplace_back(vec);
    vec.clear();

    // 薙ぎ払いエネルギー
    vec.emplace_back(&Boss_0::S_V_NagiriEnergy);
    vec.emplace_back(&Boss_0::S_V_Horming);
    vec.emplace_back(&Boss_0::EndAttack);
    SideBehaviorVec.emplace_back(vec);
    vec.clear();


    vec.emplace_back(&Boss_0::S_Beam);
    vec.emplace_back(&Boss_0::S_V_CrossNagiriAttack);
    vec.emplace_back(&Boss_0::EndAttack);
    SideBehaviorVec.emplace_back(vec);
    vec.clear();

    



    CompleteChangeView = true;

    //--------------------<エフェクト>--------------------//
    //angryEffect = std::make_unique<Effect>("./resources/Effects/CircleSmoke.efk");
}

void Boss_0::Move(float elapsedTime)
{

    // 現在の視点ｎ基準点に移動
    switch (CurrentView)
    {
        case 0:  // サイド
            
            break;
        case 1:  // トップ
            
            break;
        default:
            break;
    }

    // 現在の視点に応じて移動方法を切り替え
    (this->*MoveArray[static_cast<int>(GameSystem::Instance().GetCameraDirection())][movetype])(elapsedTime);

}

void Boss_0::Behavior(float elapsedTime)
{

    //タイプを決定する
    if(type==Type::RED)
    {
        ShotRadian.y = DirectX::XMConvertToRadians(-180.0f);
        LightColor = Calcu3D::LerpFloat3(LightColor, { 1.0f,0.0f,0.0f }, 10.0f * elapsedTime);
    }
    else if (type == Type::BLUE)
    {
        ShotRadian.y = DirectX::XMConvertToRadians(-180.0f);
        LightColor = Calcu3D::LerpFloat3(LightColor, { 0.0f,0.0f,1.0f }, 10.0f * elapsedTime);
    }
    

    // 最初の動き
    if(CompleteFirstMotion==false)
    {
        if (EnemyManager::Instance().GetEndBossAppear() == false) return;
        FirstMotion(elapsedTime);
        EnemyManager::Instance().SetCameraFocusBoss(true);
        return;
    }

    if(IsLastPerformance)
    {
        
        return;
    }


    // 移動
    Move(elapsedTime);
    ChangeView(elapsedTime);

    if (CompleteAttack)
    {
        Performancemanager(elapsedTime);
    }


    // 攻撃
    AttackManager(elapsedTime);



    //無敵時間更新
    invTimer -= elapsedTime;

    

    //--------------------<ビームモデルの更新>--------------------//
    mBeamEmitter.t = t;
    mBeamEmitter.t.Position.z -= 10.0f;
    mBeamEmitter.r = r;
    mBeamEmitter.Model->Update(elapsedTime);

    //--------------------<ポイントライトを更新>--------------------//
    mPointLight->SetPosition(mBeamEmitter.t.Position);
    mPointLight->SetColor(LightColor);
    mPointLight->SetRange(LightRange);

    GuiMenu();
}

void Boss_0::Collision()
{
    if (!CompleteChangeView) return;

    //****************************************************************
    // 
    // モーション待ち中の体力管理
    // 
    //****************************************************************
    if(CurrentMode == TOP && status.hitPoint < MaxHitPoint * 0.5f)
    {
        return;
    }
   

    VsProjectile();
}

const auto& Boss_0::GetShotVec(const DirectX::XMFLOAT3 rad)const
{
    const DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(rad.x, rad.y, rad.z);
    DirectX::XMFLOAT3 front{};
    DirectX::XMVector3Normalize(transform.r[2]);
    DirectX::XMStoreFloat3(&front, transform.r[2]);
    return front;
}


void Boss_0::Finalize()
{
    EnemyManager::Instance().SetGameEnd(true);
    GameSystem::Instance().TimerStop();
}

void Boss_0::Render(ID3D11DeviceContext* dc)
{

    mPointLight->Update(dc);
    Model->Render(dc, t, r);
    if (ShowBeamEmitter)
    {
        mBeamEmitter.Model->Render(dc, mBeamEmitter.t, mBeamEmitter.r);
    }
}


void Boss_0::FirstMotion(float elapsedTime)
{
    

    // ボスのほうにカメラを向けてボス登場モーション
    StackTimer += elapsedTime;
    ProjectileManager::Instance().Clear();

    ZoomPower += elapsedTime;
    if (ZoomPower >= 1.5f)
    {
        if (!ZoomUp)
        {
            ZoomPower -= elapsedTime * 100.0f;
            ZoomPower = std::max(ZoomPower, 0.0f);
        }
        else
        {
            ZoomPower += elapsedTime * 100.0f;
            ZoomPower = std::min(ZoomPower, 100.0f);
            if (ZoomPower >= 100.0f)
            {
                ZoomUp = false;
            }
        }
    }
    EnemyManager::Instance().fSetZoomPower(ZoomPower);

    // それが終わったら
    if (StackTimer > Timer)
    {
        t.Position.z -= 100.0f * elapsedTime;
        r.Angle = { 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f };
    }

    if(t.Position.z<20.0f)
    {
        CompleteFirstMotion = true;
        hitAreaSize = { 10.0f,10.0f,10.0f };
        CompleteAttack = false;
        CurrentAttackComboNumber = 4;
    }
}

//--------------------< ボスの攻撃を管理する>--------------------//
void Boss_0::AttackManager(float elapsedTime)
{
    if (!CompleteChangeView|| IsPerformance)
    {
        ProjectileManager::Instance().Clear();
        return;
    }

    switch (CurrentMode) // 現在の状態から乱数で攻撃を決定
    {
    case TOP:  // トップ
        //--------------------<攻撃終了なら次の攻撃を抽選>--------------------//
        if (CompleteAttack&&!GameSystem::Instance().GetChangingView()&& CompleteChangeView&& !EndPerformance)
        {

            CurrentAttackComboNumber = static_cast<int>(rand() % 3) + 1; // ０は最初の動きなので抽選対象外
            CompleteAttack = false;
            AttackState = 0;
            
        }
        (this->*TopBehaviorVec[CurrentAttackComboNumber][CurrentAttackNumber])(elapsedTime);


        break;
    case SIDE:  // サイド
        //--------------------<攻撃終了なら次の攻撃を抽選>--------------------//
        if (CompleteAttack && !GameSystem::Instance().GetChangingView() && CompleteChangeView && !EndPerformance)
        {
            CurrentAttackComboNumber = static_cast<int>(rand() % 5) + 1;// ０は最初の動きなので抽選対象外
            CompleteAttack = false;
            AttackState = 0;
        }
        (this->*SideBehaviorVec[CurrentAttackComboNumber][CurrentAttackNumber])(elapsedTime);
        break;
    case ANGRY_TOP: // 発狂トップ
        //--------------------<攻撃終了なら次の攻撃を抽選>--------------------//
        if (CompleteAttack && !GameSystem::Instance().GetChangingView() && CompleteChangeView && !EndPerformance)
        {
            CurrentAttackComboNumber = static_cast<int>(rand() % 4) + AddIndexTop;// ０は最初の動きなので抽選対象外
            CompleteAttack = false;
            AttackState = 0;
        }
        (this->*TopBehaviorVec[CurrentAttackComboNumber][CurrentAttackNumber])(elapsedTime);

        break;
    case ANGRY_SIDE: // 発狂サイド
        //--------------------<攻撃終了なら次の攻撃を抽選>--------------------//
        if (CompleteAttack && !GameSystem::Instance().GetChangingView() && CompleteChangeView && !EndPerformance)
        {
            CurrentAttackComboNumber = static_cast<int>(rand() % 3) + AddIndexSide;// ０は最初の動きなので抽選対象外
            CompleteAttack = false;
            AttackState = 0;
        }
        (this->*SideBehaviorVec[CurrentAttackComboNumber][CurrentAttackNumber])(elapsedTime);

        break;
    default: ;
    }
}


//--------------------<フロント方向に攻撃>--------------------//

void Boss_0::T_ShotFront() const
{

    const auto F = GetFrontVec();
    constexpr float BulletSpeed = 1.0f;
    
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    auto b0 = new Bullet(p_device, BaseProjectile::Parent::REDENEMY,
        t.Position, F* BulletSpeed);
    ProjectileManager::Instance().RegisterProjectile(b0);


}


void Boss_0::Shot_ShotRadian(const BaseProjectile::Parent p,const XMFLOAT3 Radian)
{
    const DirectX::XMFLOAT3 front = GetShotVec(Radian);

    constexpr float BulletSpeed = 1.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    auto b0 = new Bullet(p_device, p,
        t.Position, front * BulletSpeed, 20.0f);
    ProjectileManager::Instance().RegisterProjectile(b0);

}

void Boss_0::T_ShotEnergyFront() const
{
    const auto F = GetFrontVec();
    constexpr float BulletSpeed = 6.0f;

    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    auto b0 = new Energy(p_device, BaseProjectile::Parent::REDENEMY,
        t.Position, F );
    ProjectileManager::Instance().RegisterProjectile(b0);
}


void Boss_0::ShotEnergyShotRadian(BaseProjectile::Parent p,DirectX::XMFLOAT3 Radian) const
{
    const DirectX::XMFLOAT3 front = GetShotVec(Radian);

    constexpr float BulletSpeed = 1.0f;
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    auto b0 = new Energy(p_device, p,
        t.Position, front * BulletSpeed);
    ProjectileManager::Instance().RegisterProjectile(b0);
}




void Boss_0::S_ShotEnergyFront() const
{
    const auto F = GetFrontVec();
    constexpr float BulletSpeed = 6.0f;

    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    auto b0 = new Energy(p_device, BaseProjectile::Parent::BLUEENEMY,
        t.Position, F);
    ProjectileManager::Instance().RegisterProjectile(b0);
}



void Boss_0::T_ShotHoming(int FirstVec)
{
    // ホーミング

    ID3D11Device* p_device = SceneManager::Instance().GetDevice();
    // 前・左右斜めに発射
    HomingBullet* b0;
    if (GameSystem::Instance().GetCameraDirection()==GameSystem::CAMERA_DIRECTION::TOP)
    {
        b0 = new HomingBullet(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, FirstVec);
    }
    else
    {
        b0 = new HomingBullet(p_device, BaseProjectile::Parent::REDENEMY,
            t.Position, FirstVec);
    }

    ProjectileManager::Instance().RegisterProjectile(b0);

}


//****************************************************************
// 
//  視点変更時の演出
// 
//****************************************************************

void Boss_0::PerformanceView(float elapsedTime)
{
    // カメラをボスに注目させる

    switch (PerformanceState)
    {
    case 0:
        EnemyManager::Instance().SetCameraFocusBoss(true);
        EnemyManager::Instance().SetTargetPosition(t.Position);
        IsPerformance = true;
        PerformanceState++;
        /*FallThrough*/
        break;
    case 1:
        // アニメーション
        StackPerformanceSecond += elapsedTime;

        EnemyManager::Instance().SetCameraFocusBoss(true);
        EnemyManager::Instance().SetTargetPosition(t.Position);

        StackZoomPower += elapsedTime;
        StackZoomPower = std::min(StackZoomPower, 1.0f);
        EnemyManager::Instance().fSetZoomPower(StackZoomPower);

        if(StackPerformanceSecond>PerformanceSeconds)
        {
            StackPerformanceSecond = 0.0f;
            PerformanceState++;
        }

        break;

    case 2:
        EnemyManager::Instance().SetCameraFocusBoss(false);
        EndPerformance = true;
        PerformanceState = 0;
        IsPerformance = false;
        InputChangeView = true;
        CompleteChangeView = false;
        EnemyManager::Instance().fSetZoomPower(0.0f);
        break;
    default: 
        break;
    }


}

void Boss_0::Performancemanager(float elapsedTime)
{
    if (IsPerformance)
    {
        PerformanceView(elapsedTime);
    }
}



void Boss_0::ChangeView(float elapsedTime)
{
    // 視点変更完了後のⅠフレーム処理
    if (!CompleteChangeView && !GameSystem::Instance().GetChangingView())
    {
        CurrentAttackComboNumber = 0;
        CurrentAttackNumber = 0;
        CompleteChangeView = true;

        CompleteAttack = false;
        movetype = 0;
        EndPerformance = false;
        AttackState = 0;
        //--------------------<体力を補正>--------------------//
        switch (CurrentMode) {
            case TOP: 
                break;
        case SIDE:
            status.hitPoint = MaxHitPoint * 0.5f;
            type = Type::RED;
            break;
        case ANGRY_TOP:
            type = Type::BLUE;
            status.hitPoint = MaxHitPoint * 0.5f;
            break;
        case ANGRY_SIDE:
            status.hitPoint = MaxHitPoint * 0.25f;
            type = Type::RED;
            break;
        default: ;
        }
    }

    // ボスの体力が残り75%で縦画面の時横画面に変更
    if (CurrentMode==TOP && status.hitPoint < MaxHitPoint * 0.5f&& CompleteAttack&& CompleteChangeView&& !IsPerformance)
    {
        IsPerformance = true;
        r.Angle = { 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f };
        CurrentMode = SIDE;
        ZoomPower = 0.0f;
        
    }

    // ボスの体力が１の時演出開始
    if (status.hitPoint <= 1.0f)
    {
        status.hitPoint = 1.0f;
        if (CompleteAttack && CompleteChangeView && !IsPerformance)
        {
            IsLastPerformance = true;
        }
    }

    ZoomPower += elapsedTime;
    if (ZoomPower >= 1.5f)
    {
        if (!ZoomUp)
        {
            ZoomPower -= elapsedTime * 100.0f;
            ZoomPower = std::max(ZoomPower, 0.0f);
        }
        else
        {
            ZoomPower += elapsedTime * 100.0f;
            ZoomPower = std::min(ZoomPower, 100.0f);
            if (ZoomPower >= 100.0f)
            {
                ZoomUp = false;
            }
        }
    }
    EnemyManager::Instance().fSetZoomPower(ZoomPower);

    if(InputChangeView)
    {
        // 切り替え処理
        GameSystem::Instance().ChangeView();
        InputChangeView = false;
        CompleteChangeView = false;
    }


}

void Boss_0::GuiMenu()
{
#ifdef _DEBUG
    ImGui::Begin("Boss0");
    
    ImGui::InputInt("Move", &movetype);
    movetype = std::max(0, std::min(1, movetype));
    ImGui::InputFloat("HP", &status.hitPoint);

    if (ImGui::Button("Next"))
    {
        status.hitPoint -= MaxHitPoint * 0.25f;
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

    if (ImGui::Button("T_Homing＋"))
    {
        T_ShotHoming(1);
    }
    if (ImGui::Button("T_Homingー"))
    {
        T_ShotHoming(-1);
    }
    
    ImGui::InputInt("AttackState", &AttackState);

    float a = mBeamEmitter.Model->GetCurrentAnimationFrame();
    ImGui::InputFloat("AnimeFrame", &a);
    if (ImGui::Button("BreakPoint"))
    {
        int a = 0;
    }

    ImGui::Checkbox("CompleteAttack", &CompleteAttack);

    ImGui::InputFloat("LightRange", &LightRange);
    ImGui::InputFloat3("LightColor", &LightColor.x);

    int i = (int)type;
    ImGui::InputInt("Type", &i);


    ImGui::End();

#endif

}


//******************************************************************************************************************************************************//
//                                                                                                                                                      //
//                                                            関数ポインタに詰める関数達                                                                //
//                                                                                                                                                      //
//******************************************************************************************************************************************************//


//****************************************************************
// 
//   移動
// 
//****************************************************************


void Boss_0::T_Stop(float elaspedTime)
{
  // 動かない   
}

void Boss_0::T_Move(float elapsedTime)
{
    // 左右に移動
    t.Position.x += Speed * elapsedTime;

    // 制限にひっかかったら反転
    if(t.Position.x>LimitRight|| t.Position.x < LimitLeft)
    {
        Speed *= -1.0f;
    }
}

void Boss_0::S_Stop(float elaspedTime)
{
    
}

void Boss_0::S_Move(float elapsedTime)
{
    // 左右に移動
    t.Position.y += Speed * elapsedTime;

    // 制限にひっかかったら反転
    if (t.Position.x > LimitRight || t.Position.x < LimitLeft)
    {
        Speed *= -1.0f;
    }
}





//****************************************************************
// 
//  攻撃の関数
// 
//****************************************************************

//--------------------<攻撃終了>--------------------//

void Boss_0::EndAttack(float elapsedTime)
{
    CompleteAttack = true;
    CurrentAttackNumber = 0;
    AttackState = 0;
}


void Boss_0::T_MoveFirstPosition(float elapsedTime)
{
    t.Position.z -= Speed * elapsedTime;
    t.Position.y = 0.0f;
    if (t.Position.z < 20.0f)
    {
        CurrentAttackNumber++;
    }
}

void Boss_0::S_MoveFirstPosition(float elapsedTime)
{
    t.Position.z += Speed * elapsedTime;
    t.Position.x = 0.0f;
    if(t.Position.z>40.0f)
    {
        CurrentAttackNumber++;
    }

}

void Boss_0::T_NormalAttack(float elapsedTime)
{
    CompleteAttack = true;
}


void Boss_0::T_NagiriAttack(float elapsedTime)
{
    constexpr float Begin = { DirectX::XMConvertToRadians(100.0f) };
    constexpr float End = { DirectX::XMConvertToRadians(250.0f) };

    constexpr float s = 1.2f;


    StackAttackInterval += elapsedTime;
    const bool Attack = StackAttackInterval > NagiriInterval;

    // 薙ぎ払い攻撃
    switch (AttackState)
    {
    case 0:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 1:
        // 薙ぎ払い攻撃
        Ratio -= elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio < 0.0f)
        {
            AttackState++;
        }
        break;
    case 2:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);

        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 3:
        // 終了
        CurrentAttackNumber++;
        AttackState = 0;
        Ratio = 0.0f;
        StackAttackInterval = 0.0f;
        ShotRadian = { 0.0f,0.0f,0.0f };
        break;
    }
}

void Boss_0::T_NagiriEnergy(float elapsedTime)
{
    constexpr float Begin = { DirectX::XMConvertToRadians(90.0f) };
    constexpr float End = { DirectX::XMConvertToRadians(270.0f) };

    constexpr float s = 1.2f;


    StackAttackInterval += elapsedTime;
    const bool Attack = StackAttackInterval > NagiriInterval;

    // 薙ぎ払い攻撃
    switch (AttackState)
    {
    case 0:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            ShotEnergyShotRadian(BaseProjectile::Parent::REDENEMY,ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 1:
        // 薙ぎ払い攻撃
        Ratio -= elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            ShotEnergyShotRadian(BaseProjectile::Parent::BLUEENEMY,ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio < 0.0f)
        {
            AttackState++;
        }
        break;
    case 2:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);

        if (Attack)
        {
            ShotEnergyShotRadian(BaseProjectile::Parent::REDENEMY,ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 3:
        // 終了
        CurrentAttackNumber++;
        AttackState = 0;
        Ratio = 0.0f;
        StackAttackInterval = 0.0f;
        ShotRadian = { 0.0f,0.0f,0.0f };
        break;
    }


}

void Boss_0::T_AllRangeAttack(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
    {
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        const int Max = 12;
        // 全体に攻撃
        for (int i = 0; i < Max; i++)
        {
            // 合計が 360どになるように

            float Deg = static_cast<float>(i) * (360.0f / static_cast<float>(Max)); // Y軸の角度（ディグリー）
            if (Deg > 360.0f) Deg -= 360.0f;

            Deg = DirectX::XMConvertToRadians(Deg);
            const DirectX::XMFLOAT3 Vel = { sinf(Deg),0.0f,cosf(Deg) }; // 進行方向

            auto b = new Bullet(p_device, BaseProjectile::Parent::REDENEMY, t.Position, Vel);
            ProjectileManager::Instance().RegisterProjectile(b);
        }
        AttackState++;
    }

    case 1:
    {
        // 終了
        AttackState = 0;
        CurrentAttackNumber++;
    }
        break;
   
     default: 
         break;;
    }
}

void Boss_0::T_Horming(float elapsedTime)
{
    static int Count = 0;
    StackAttackInterval += elapsedTime;
    if (StackAttackInterval > 0.3f)
    {
        T_ShotHoming(1);
        T_ShotHoming(-1);
        StackAttackInterval = 0.0f;
        Count++;
    }
    if(Count>3)
    {
        Count = 0;
        StackAttackInterval = 0.0f;
        CurrentAttackNumber++;
    }


}

void Boss_0::T_MoveShot(float elapsedTime)//
{
    switch (AttackState)
    {
    case 0:
        // 向かって右に移動
        Ratio += elapsedTime * 3.0f;

        t.Position.x = Calcu3D::LerpFloat(0.0f, 20.0f, Ratio);  // 移動
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-180.0f), DirectX::XMConvertToRadians(-540.0f), Ratio);
        if(Ratio>=1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 1:
        // 撃つ
        Ratio += elapsedTime;
        if(Ratio>=0.2f)
        {
            DirectX::XMFLOAT3 launchAngle{ r.Angle };
            launchAngle.y -= DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            launchAngle.y = DirectX::XMConvertToRadians(-180.0f) + DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);

            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 2:
        // 撃つ
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {
            DirectX::XMFLOAT3 launchAngle{ r.Angle };
            launchAngle.y -= DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            launchAngle.y = DirectX::XMConvertToRadians(-180.0f) + DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 3:
        // 撃つ
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {
            DirectX::XMFLOAT3 launchAngle{ r.Angle };
            launchAngle.y -= DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            launchAngle.y = DirectX::XMConvertToRadians(-180.0f) + DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 4:
        // 向かって左に移動
        Ratio += elapsedTime * 2.0f;
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-540.0f), DirectX::XMConvertToRadians(-180.0f), Ratio);
        t.Position.x = Calcu3D::LerpFloat(20.0f, -20.0f, Ratio);  // 移動
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 5:
        // 撃つ
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {
            DirectX::XMFLOAT3 launchAngle{ r.Angle };
            launchAngle.y -= DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            launchAngle.y = DirectX::XMConvertToRadians(-180.0f) + DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 6:
        // 撃つ
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {
            DirectX::XMFLOAT3 launchAngle{ r.Angle };
            launchAngle.y -= DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            launchAngle.y = DirectX::XMConvertToRadians(-180.0f) + DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 7:
        // 撃つ
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {
            DirectX::XMFLOAT3 launchAngle{ r.Angle };
            launchAngle.y -= DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            launchAngle.y = DirectX::XMConvertToRadians(-180.0f) + DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 8:
        // 戻る
        Ratio += elapsedTime*2.0f;
        t.Position.x = Calcu3D::LerpFloat(-20.0f, 0.0f, Ratio);  // 移動
        r.Angle.y = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(-180.0f), DirectX::XMConvertToRadians(-540.0f), Ratio);
        if (Ratio >= 1.0f)
        {
            Ratio = 0.0f;
            AttackState++;
            r.Angle.y = DirectX::XMConvertToRadians(-180.0f);
        }
        break;
    case 9:
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {
            DirectX::XMFLOAT3 launchAngle{ r.Angle };
            launchAngle.y -= DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            launchAngle.y = DirectX::XMConvertToRadians(-180.0f) + DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 10:
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {
            DirectX::XMFLOAT3 launchAngle{ r.Angle };
            launchAngle.y -= DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            launchAngle.y = DirectX::XMConvertToRadians(-180.0f) +
                
                DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 11:
        Ratio += elapsedTime;
        if (Ratio >= 0.2f)
        {
            DirectX::XMFLOAT3 launchAngle{ r.Angle };
            launchAngle.y -= DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, r.Angle);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            launchAngle.y = DirectX::XMConvertToRadians(-180.0f) + DirectX::XMConvertToRadians(45.0f);
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, launchAngle);
            Ratio = 0.0f;
            AttackState++;
        }
        break;
    case 12:
        /// <summary>
       /// 最初期化
       /// </summary>
       /// <param name="elapsedTime"></param>
        AttackState = 0;
        Ratio = 0.0f;
        t.Position.x = 0.0f;
        r.Angle.y = DirectX::XMConvertToRadians(-180.0f);
        CurrentAttackNumber++;
        break;
    default: ;
    }
}


void Boss_0::T_Irradiation(float elapsedTime)
{
    
}

void Boss_0::S_NormalAttack(float elapsedTime)
{



}

void Boss_0::S_NagiriAttack(float elapsedTime)
{
    constexpr float Begin = { DirectX::XMConvertToRadians(-100.0f) };
    constexpr float End = { DirectX::XMConvertToRadians(100.0f) };

    constexpr float s = 1.0f;


    StackAttackInterval += elapsedTime;
    const bool Attack = StackAttackInterval > NagiriInterval*0.7f;

    // 薙ぎ払い攻撃
    switch (AttackState)
    {
    case 0:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 1:
        // 薙ぎ払い攻撃
        Ratio -= elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio < 0.0f)
        {
            AttackState++;
        }
        break;
    case 2:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);

        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 3:
        // 終了
        CurrentAttackNumber++;
        AttackState = 0;
        Ratio = 0.0f;
        StackAttackInterval = 0.0f;
        ShotRadian = { 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        break;
    }
}

void Boss_0::S_NagiriEnergy(float elapsedTime)
{
    constexpr float Begin = { DirectX::XMConvertToRadians(-100.0f) };
    constexpr float End = { DirectX::XMConvertToRadians(100.0f) };

    constexpr float s = 1.2f;


    StackAttackInterval += elapsedTime;
    const bool Attack = StackAttackInterval > NagiriInterval;

    // 薙ぎ払い攻撃
    switch (AttackState)
    {
    case 0:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            ShotEnergyShotRadian(BaseProjectile::Parent::BLUEENEMY,ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 1:
        // 薙ぎ払い攻撃
        Ratio -= elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            ShotEnergyShotRadian(BaseProjectile::Parent::REDENEMY,ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio < 0.0f)
        {
            AttackState++;
        }
        break;
    case 2:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);

        if (Attack)
        {
            ShotEnergyShotRadian(BaseProjectile::Parent::BLUEENEMY,ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 3:
        // 終了
        CurrentAttackNumber++;
        AttackState = 0;
        Ratio = 0.0f;
        StackAttackInterval = 0.0f;
        ShotRadian = { 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        break;
    }
}

void Boss_0::S_AllRangeAttack(float elapsedTime)
{
    
}

void Boss_0::S_Horming(float elapsedTime)
{
    static int Count = 0;
    StackAttackInterval += elapsedTime;
    if (StackAttackInterval > 0.4f)
    {
        T_ShotHoming(1);
        T_ShotHoming(-1);
        StackAttackInterval = 0.0f;
        Count++;
    }
    if (Count > 3)
    {
        Count = 0;
        StackAttackInterval = 0.0f;
        CurrentAttackNumber++;
    }
}

void Boss_0::S_Beam(float elapsedTime)
{
    //--------------------<ビーム発射>--------------------//
    switch (AttackState)
    {
    case 0:
    {
        ShowBeamEmitter = true;
        mBeamEmitter.Model->f_PlayAnimation(0);
        AttackState++;

        /*FallThrough*/


    }
    case 1:
        if (laserFlag.Check("ShotBeam", mBeamEmitter.Model->GetCurrentAnimationNumber(), mBeamEmitter.Model->GetCurrentAnimationFrame()))
        {
            AttackState++;
        }
        break;
    case 2:
    {
        StackAttackInterval = 0.0f;
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        auto b0 = new Beam(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position);
        ProjectileManager::Instance().RegisterProjectile(b0);
        AttackState++;
    }
    case 3:
        
        StackAttackInterval += elapsedTime;



        if (StackAttackInterval > 4.0f)
        {
            LightRange = Calcu3D::LerpFloat(LightRange, 1.0f, 20.0f * elapsedTime);
        }
        else
        {
            LightRange = Calcu3D::LerpFloat(LightRange, 10000.0f, 20.0f * elapsedTime);
        }
        


        if(StackAttackInterval>5.0f)
        {
            AttackState++;
        }
        break;
    case 4:
        CurrentAttackNumber++;
        AttackState = 0;
        StackAttackInterval = 0.0f;
        StackInsurance = 0.0f;
        ShowBeamEmitter = false;
        break;

    }


}



void Boss_0::S_CrossNagiriAttack(float elapsedTime)
{
    const float Begin = DirectX::XMConvertToRadians(90.0f);
    const float Begin2 = DirectX::XMConvertToRadians(-90.0f);

    const float End = 0.0f;

    Ratio += elapsedTime * 1.0f;
    StackAttackInterval += elapsedTime;

    if (StackAttackInterval > 0.3f)
    {
        DirectX::XMFLOAT3 Radian{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        DirectX::XMFLOAT3 Radian2{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        Radian.x= Calcu3D::LerpFloat(Begin, End, Ratio);
        Radian2.x= Calcu3D::LerpFloat(Begin2, End, Ratio);

        Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, Radian);
        Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, Radian2);
        
        StackAttackInterval = 0.0f;
    }

    if(Ratio>1.0f)
    {
        Ratio = 0.0f;
        CurrentAttackNumber++;
        StackAttackInterval++;
    }

}



//*********************************************************************************************************************************
//                                                             
//   発狂後の攻撃
//                                                             
//*********************************************************************************************************************************

// ホーミング
void Boss_0::T_V_Homing(float elapsedTime)
{
    static int Count = 0;
    StackAttackInterval += elapsedTime;
    if (StackAttackInterval > 0.15f)
    {
        T_ShotHoming(1);
        T_ShotHoming(-1);
        StackAttackInterval = 0.0f;
        Count++;
    }
    if (Count > 3)
    {
        Count = 0;
        StackAttackInterval = 0.0f;
        CurrentAttackNumber++;
    }
}

void Boss_0::T_V_AllRangeAttack(float elapsedTime)
{
    switch (AttackState)
    {
    case 0:
    {
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        const int Max =  12;
        // 全体に攻撃
        for (int i = 0; i < Max; i++)
        {
            // 合計が 360どになるように

            float Deg = static_cast<float>(i) * (360.0f / static_cast<float>(Max)); // Y軸の角度（ディグリー）
            if (Deg > 360.0f) Deg -= 360.0f;

            Deg = DirectX::XMConvertToRadians(Deg);
            const DirectX::XMFLOAT3 Vel = { sinf(Deg),0.0f,cosf(Deg) }; // 進行方向

            auto b = new Bullet(p_device, BaseProjectile::Parent::REDENEMY, t.Position, Vel);
            ProjectileManager::Instance().RegisterProjectile(b);
        }
        AttackState++;
    }
    case 1:
    {
        // 終了
        AttackState = 0;
        CurrentAttackNumber++;
    }
    break;

    default:
        break;;
    }
}


// 薙ぎ払い攻撃
void Boss_0::T_V_NagiriAttack(float elapsedTime)
{

    constexpr float Begin = { DirectX::XMConvertToRadians(100.0f) };
    constexpr float End = { DirectX::XMConvertToRadians(250.0f) };

    constexpr float s = 1.0f;


    StackAttackInterval += elapsedTime;
    const bool Attack = StackAttackInterval > NagiriInterval*0.8f;

    // 薙ぎ払い攻撃
    switch (AttackState)
    {
    case 0:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 1:
        // 薙ぎ払い攻撃
        Ratio -= elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio < 0.0f)
        {
            AttackState++;
        }
        break;
    case 2:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);

        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 3:
        // 終了
        CurrentAttackNumber++;
        AttackState = 0;
        Ratio = 0.0f;
        StackAttackInterval = 0.0f;
        ShotRadian = { 0.0f,0.0f,0.0f };
        break;
    }
}

void Boss_0::T_V_NagiriEnergy(float elapsedTime)
{
    constexpr float Begin = { DirectX::XMConvertToRadians(90.0f) };
    constexpr float End = { DirectX::XMConvertToRadians(270.0f) };

    constexpr float s = 1.0f;


    StackAttackInterval += elapsedTime;
    const bool Attack = StackAttackInterval > NagiriInterval;

    // 薙ぎ払い攻撃
    switch (AttackState)
    {
    case 0:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            ShotEnergyShotRadian(BaseProjectile::Parent::REDENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 1:
        // 薙ぎ払い攻撃
        Ratio -= elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            ShotEnergyShotRadian(BaseProjectile::Parent::BLUEENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio < 0.0f)
        {
            AttackState++;
        }
        break;
    case 2:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.y = Calcu3D::LerpFloat(Begin, End, Ratio);

        if (Attack)
        {
            ShotEnergyShotRadian(BaseProjectile::Parent::REDENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 3:
        // 終了
        CurrentAttackNumber++;
        AttackState = 0;
        Ratio = 0.0f;
        StackAttackInterval = 0.0f;
        ShotRadian = { 0.0f,0.0f,0.0f };
        break;
    }


}

void Boss_0::T_V_Beam(float elapsedTime)
{
    //--------------------<ビーム発射>--------------------//
    switch (AttackState)
    {
    case 0:
    {
        ShowBeamEmitter = true;
        mBeamEmitter.Model->f_PlayAnimation(0);
        AttackState++;
        CompleteAttack = false;
        /*FallThrough*/


    }
    case 1:
        if (laserFlag.Check("ShotBeam", mBeamEmitter.Model->GetCurrentAnimationNumber(), mBeamEmitter.Model->GetCurrentAnimationFrame()))
        {
            AttackState++;
        }
        break;
    case 2:
    {
        StackAttackInterval = 0.0f;
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();
        mPointLight->SetRange(-4.0f);
        auto b0 = new Beam(p_device, BaseProjectile::Parent::REDENEMY,
            t.Position);
        ProjectileManager::Instance().RegisterProjectile(b0);
        AttackState++;
    }
    case 3:

        if (laserFlag.Check("EndBeam", mBeamEmitter.Model->GetCurrentAnimationNumber(), mBeamEmitter.Model->GetCurrentAnimationFrame()))
        {
            mPointLight->SetRange(0.0f);
        }

        StackAttackInterval += elapsedTime;
        if (StackAttackInterval > 5.0f)
        {
            AttackState++;
            mPointLight->SetRange(0.0f);
        }
        break;
    case 4:
        CurrentAttackNumber++;
        AttackState = 0;
        StackAttackInterval = 0.0f;
        StackInsurance = 0.0f;
        ShowBeamEmitter = false;
        break;

    }


}

void Boss_0::T_V_SpecialAttack(float elapsedTime)
{

    constexpr float Begin =  { DirectX::XMConvertToRadians(90.0f) };
    constexpr float Begin2 = { DirectX::XMConvertToRadians(270.0f) };

    const float End = 180.0f;

    Ratio += elapsedTime * 1.0f;
    StackAttackInterval += elapsedTime;

    if (StackAttackInterval > 0.6f)
    {
        DirectX::XMFLOAT3 Radian{ 0.0f,0.0f,0.0f };
        DirectX::XMFLOAT3 Radian2{ 0.0f,0.0f,0.0f };
        Radian.y = Calcu3D::LerpFloat(Begin, End, Ratio);
        Radian2.y = Calcu3D::LerpFloat(Begin2, End, Ratio);

        Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, Radian);
        Shot_ShotRadian(BaseProjectile::Parent::REDENEMY, Radian2);

        T_ShotHoming(1);
        T_ShotHoming(-1);

        StackAttackInterval = 0.0f;
    }

    if (Ratio > 1.0f)
    {
        Ratio = 0.0f;
        CurrentAttackNumber++;
        StackAttackInterval++;
    }

}

void Boss_0::S_V_NagiriAttack(float elapsedTime)
{
    constexpr float Begin = { DirectX::XMConvertToRadians(-100.0f) };
    constexpr float End = { DirectX::XMConvertToRadians(100.0f) };

    constexpr float s = 1.2f;


    StackAttackInterval += elapsedTime;
    const bool Attack = StackAttackInterval > NagiriInterval * 0.5f;

    // 薙ぎ払い攻撃
    switch (AttackState)
    {
    case 0:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 1:
        // 薙ぎ払い攻撃
        Ratio -= elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio < 0.0f)
        {
            AttackState++;
        }
        break;
    case 2:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);

        if (Attack)
        {
            Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 3:
        // 終了
        CurrentAttackNumber++;
        AttackState = 0;
        Ratio = 0.0f;
        StackAttackInterval = 0.0f;
        ShotRadian = { 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        break;
    }
}


void Boss_0::S_V_CrossNagiriAttack(float elapsedTime)
{
    const float Begin = DirectX::XMConvertToRadians(90.0f);
    const float Begin2 = DirectX::XMConvertToRadians(-90.0f);

    const float End = 0.0f;

    Ratio += elapsedTime * 1.0f;
    StackAttackInterval += elapsedTime;

    if (StackAttackInterval > 0.07f)
    {
        DirectX::XMFLOAT3 Radian{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        DirectX::XMFLOAT3 Radian2{ 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        Radian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        Radian2.x = Calcu3D::LerpFloat(Begin2, End, Ratio);

        Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, Radian);
        Shot_ShotRadian(BaseProjectile::Parent::BLUEENEMY, Radian2);

        StackAttackInterval = 0.0f;
    }

    if (Ratio > 1.0f)
    {
        Ratio = 0.0f;
        CurrentAttackNumber++;
        StackAttackInterval++;
    }

}

void Boss_0::S_V_NagiriEnergy(float elapsedTime)
{
    constexpr float Begin = { DirectX::XMConvertToRadians(-100.0f) };
    constexpr float End = { DirectX::XMConvertToRadians(100.0f) };

    constexpr float s = 1.2f;


    StackAttackInterval += elapsedTime;
    const bool Attack = StackAttackInterval > NagiriInterval * 0.5f;

    // 薙ぎ払い攻撃
    switch (AttackState)
    {
    case 0:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            ShotEnergyShotRadian(BaseProjectile::Parent::BLUEENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 1:
        // 薙ぎ払い攻撃
        Ratio -= elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);
        if (Attack)
        {
            ShotEnergyShotRadian(BaseProjectile::Parent::REDENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio < 0.0f)
        {
            AttackState++;
        }
        break;
    case 2:
        // 薙ぎ払い攻撃
        Ratio += elapsedTime * s;
        ShotRadian.x = Calcu3D::LerpFloat(Begin, End, Ratio);

        if (Attack)
        {
            ShotEnergyShotRadian(BaseProjectile::Parent::BLUEENEMY, ShotRadian);
            StackAttackInterval = 0.0f;
        }
        if (Ratio > 1.0f)
        {
            AttackState++;
        }
        break;
    case 3:
        // 終了
        CurrentAttackNumber++;
        AttackState = 0;
        Ratio = 0.0f;
        StackAttackInterval = 0.0f;
        ShotRadian = { 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
        break;
    }
}

void Boss_0::S_V_Beam(float elapsedTime)
{
    //--------------------<ビーム発射>--------------------//
    switch (AttackState)
    {
    case 0:
    {
        StackAttackInterval = 0.0f;
        ID3D11Device* p_device = SceneManager::Instance().GetDevice();

        auto b0 = new Beam(p_device, BaseProjectile::Parent::REDENEMY,
            t.Position);
        ProjectileManager::Instance().RegisterProjectile(b0);
        AttackState++;
    }
    /*FallThrough*/
    case 1:
        StackAttackInterval += elapsedTime;
        if (StackAttackInterval > 5.0f)
        {
            AttackState++;
        }
        break;
    case 2:
        CurrentAttackNumber++;
        AttackState = 0;
        StackAttackInterval = 0.0f;
        Ratio = 0.0f;
        break;

    }
}


void Boss_0::S_V_Horming(float elapsedTime)
{
    static int Count = 0;
    StackAttackInterval += elapsedTime;
    if (StackAttackInterval > 0.4f)
    {
        T_ShotHoming(1);
        T_ShotHoming(-1);
        StackAttackInterval = 0.0f;
        Count++;
    }
    if (Count > 7)
    {
        Count = 0;
        StackAttackInterval = 0.0f;
        CurrentAttackNumber++;
    }
}
