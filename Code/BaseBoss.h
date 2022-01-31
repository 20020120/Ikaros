#pragma once

#include "BaseProjectile.h"
#include"Enemy.h"
#include"ConstantBuffer.h"

class BaseBoss : public Enemy
{
public:
    BaseBoss(ID3D11Device* d, const char* ModelFilePath, const char* PSFilePath);
    ~BaseBoss() override;
    void T_MoveFirstPosition(float elapsedTime);
    void S_MoveFirstPosition(float elapsedTime);

    void Initialize() override;
    virtual void Render(ID3D11DeviceContext* dc) override = 0;
    void Finalize() override;


protected:

    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************

    virtual void ChangeView(float elapsedTime);
    virtual void ChangeViewPerformance(float);
    virtual void AttackManager(float elapsedTime) = 0;
    virtual void GuiMenu(){};
    void Shot_ShotRadian(BaseProjectile::Parent p, DirectX::XMFLOAT3 Radian, DirectX::XMFLOAT3 pos);
    void Shot_ShotRadianSlow(BaseProjectile::Parent p, DirectX::XMFLOAT3 Radian, DirectX::XMFLOAT3 pos);
    void Shot_ShotRadianFast(BaseProjectile::Parent p, DirectX::XMFLOAT3 Radian, DirectX::XMFLOAT3 pos);
    DirectX::XMFLOAT3 GetShotVec(DirectX::XMFLOAT3 rad) const;

    void T_ShotHoming(int FirstVec, float Ratio = 5.0f);

    void T_ShotHoming2(int FirstVec);
    virtual void Shot_ShotVector(BaseProjectile::Parent p, DirectX::XMFLOAT3 Vec, DirectX::XMFLOAT3 pos);
    virtual void Shot_ShotVectorFast(BaseProjectile::Parent p, DirectX::XMFLOAT3 Vec, DirectX::XMFLOAT3 pos);
    virtual void Shot_ShotVectorSlow(BaseProjectile::Parent p, DirectX::XMFLOAT3 Vec, DirectX::XMFLOAT3 pos);
    void EndAttack(float elapsedTime);
    virtual void FirstMotion(float elapsedTime);
    void ShotEnergyShotRadian(BaseProjectile::Parent p, DirectX::XMFLOAT3 Radian) const;

    void AppealTime(float elapsedTime);
    void AppealTime2(float elapsedTime);

    int HitPoint{};  // 体力
    float Speed{20.0f}; // 速度

      //--------------------<演出>--------------------//
    bool IsFirstMotion{}; //最初の動き
    float StackTimer{};  // タイマー
    bool IsPerformance{};
    float ZoomPower{};
    bool ZoomUp{};
    bool IsRoar{};
    //--------------------<攻撃>--------------------//
    bool CompleteAttack{};
    int CurrentAttackComboNumber{};
    int CurrentAttackNumber{};
    int AttackState = 0;
    float StackAttackInterval{};
    DirectX::XMFLOAT3 ShotRadian{};
    float Ratio{};
    bool shotRight{};

    //--------------------<体力>--------------------//
    const float MaxHp = 400.0f;

    enum Mode
    {
        TOP,         // 縦
        SIDE,        // 横
        ANGRY_TOP,   // 発狂縦
        ANGRY_SIDE,  // 発狂横

    }CurrentMode{}; // 現在のボスの強さ

    bool IsInputChangeView{};
    bool CompleteChangeView{};
    bool IsDraw{};


    // エフェクト
    std::unique_ptr<Effect> efk_Angry{};     // 登場
    std::unique_ptr<Effect> efk_Smoke{};     // 怒り
    std::unique_ptr<Effect> efk_Spark{};     // 火花
    std::unique_ptr<Effect> efk_LastBomb{};  // 最後の爆発

    Effekseer::Handle hdl_Angry{};  // 登場
    Effekseer::Handle hdl_Smoke{};  // 怒り
    Effekseer::Handle hdl_Spark{};  // 火花
    Effekseer::Handle hdl_LastBomb{};  // 最後の爆発

    enum class EffectState
    {
        None,      // 無し
        Min,       // ちょっと
        Middle,    // まぁまぁ
        Max,       // いっぱい
    }mEffectState{ EffectState::None };

    float StackSmokeEffectInterVal{};
    float StackSmokeEffectSeconds{};
    float StackSparkEffectInterVal{};
    float StackSparkEffectSeconds{};
    
    virtual void PlayEffects(float elapsedTime) {}
    bool LastBomb{};

    struct GeometryData
    {
        float Power{};
        DirectX::XMFLOAT3 Pad0{};
    }mGeometryData{};

    ConstantBuffer Geometry_CBuffer{};

    //--------------------<オーディオ>--------------------//

    std::vector<std::unique_ptr<AudioSource>> se;
    std::vector<float> volumes;
    enum SE
    {
        Shout,
        RED_BULLET_SHOT,
        BLUE_BULLET_SHOT,
        LaserCharge,
        LaserShot,
        Shout2,
    };
    //SE
    const float  ShotVolume = 0.5f;
    const float  ShotVolume2 = 1.0f;

};
