#pragma once

#include "BaseObject.h"

#include<d3d11.h>
#include <imgui.h>
#include "Effect.h"
#include "EventFlag.h"

#include "gAudioSource.h"

class Enemy : public BaseObject
{
public:
    Enemy(ID3D11Device* d, const char* ModelFilePath, const char* PSFilePath);
    ~Enemy()override = default;

public:
    virtual void Initialize()override {}
    void Update(float elapsedTime) final;
    virtual void Render(ID3D11DeviceContext* dc)override;
    virtual void Finalize()override {}

    //getter
    const Type& GetType() const { return type; }
    const bool& IsAppear() const { return is_appear; }

protected:
    // 仮想関数
    virtual void Behavior(float elapsedTime);

    virtual void Move(float elapsedTime) {};

    virtual void Collision();

    // 関数
    bool CanShot(float elapsedTime);
    void BulletShot(float elapsedTime);
    void BulletTargetShot(float elapsedTime, const DirectX::XMFLOAT3 target);
    void EnergyShot(float elapsedTime);
    void EnergyTargetShot(float elapsedTime, const DirectX::XMFLOAT3 target);
    void Destroy();
    void VsProjectile();
    bool HitProcessing(int projType);

public:
    //関数
    bool Damage(float damage, float invTimer, bool Finisehr = false);

protected:
    // 定数
    static constexpr float turn_z_axis_size = 68.0f;
    static constexpr float turn_y_axis_size = 30.0f;
    static constexpr DirectX::XMFLOAT3 origin_pos_up        = { 0.0f,10.0f,30.0f };
    static constexpr DirectX::XMFLOAT3 origin_pos_upest     = { 0.0f,20.0f,30.0f };
    static constexpr DirectX::XMFLOAT3 origin_pos_down      = { 0.0f,-10.0f,30.0f };
    static constexpr DirectX::XMFLOAT3 origin_pos_downest   = { 0.0f,-20.0f,30.0f };

    static constexpr DirectX::XMFLOAT3 origin_pos_right     = { 12.0f,0.0f,20.0f };
    static constexpr DirectX::XMFLOAT3 origin_pos_rightest  = { 30.0f,0.0f,20.0f };
    static constexpr DirectX::XMFLOAT3 origin_pos_left      = { -12.0f,0.0f,20.0f };
    static constexpr DirectX::XMFLOAT3 origin_pos_leftest   = { -30.0f,0.0f,20.0f };

protected:
    //変数
    struct Status
    {
        float speed = { 0.0f };
        float shotInterval = { 1.0f };//弾を打つ間隔
        float shotTimer = { 0.0f };
        float hitPoint{ 2.0f };
    };

    Status status;

    bool is_appear = false;
    // イベントフラグ
    EventFlagSystem Flags;

    std::shared_ptr<Effect> efk_WeakDamaged;    // 被ダメージ（弱点）
    Effekseer::Handle hdl_WeakDamaged = -1;
    std::shared_ptr<Effect> efk_UnDamaged;      // 被ダメージ（同じ属性）
    Effekseer::Handle hdl_UnDamaged = -1;
    std::shared_ptr<Effect> efk_Bomb;           // 爆発
    Effekseer::Handle hdl_Bomb = -1;
    std::shared_ptr<Effect> efk_MoreBomb;           // 爆発
    Effekseer::Handle hdl_MoreBomb = -1;

    //SE
    std::unique_ptr<AudioSource> se_WeakDamaged;
    std::unique_ptr<AudioSource> se_UnDamaged;

};

class EnemyDummy final : public Enemy
{
public:
    EnemyDummy(ID3D11Device* Device, const char* model_filepath);
    EnemyDummy(ID3D11Device* Device, const char* model_filepath,const char* PsPath);
};


//----------行動パターン----------//
//直進
class EnemyMoveStraight final : public Enemy
{
public:
    EnemyMoveStraight(ID3D11Device* d, const Type& type);
    void Initialize() override;
    void Behavior(float elapsedTime)override;
    void Move(float elapsedTime) override;
};
//カーブ
class EnemyMoveWave final : public Enemy
{
public:

    EnemyMoveWave(ID3D11Device* d, const Type& type);
    void Initialize() override;
    void Behavior(float elapsedTime)override;
    void Move(float elapsedTime) override;
private:
    float curveAng{};
    float curveSpeed{ 30.0f };

};

namespace EnemyModelFilePath
{
    inline constexpr char* Blue_Eye     = "./resources/Models/Characters/Enemies/EnemyBlue.nk";
    inline constexpr char* Red_Eye      = "./resources/Models/Characters/Enemies/EnemyRed.nk";
    inline constexpr char* Red_Boss0    = "./resources/Models/Characters/Enemies/EnemyBlue.nk";

    inline constexpr char* Blue_Utubo   = "./resources/Models/Characters/Enemies/Utubo_Blue.nk";
    inline constexpr char* Blue_Shark   = "./resources/Models/Characters/Enemies/Shark_Blue.nk";
    inline constexpr char* Blue_Hitode  = "./resources/Models/Characters/Enemies/Hitode_Blue.nk";
    inline constexpr char* Blue_Ankou   = "./resources/Models/Characters/Enemies/Ankou_Blue.nk";

    inline constexpr char* Red_Utubo    = "./resources/Models/Characters/Enemies/Utubo_Red.nk";
    inline constexpr char* Red_Shark    = "./resources/Models/Characters/Enemies/Shark_Red.nk";
    inline constexpr char* Red_Hitode   = "./resources/Models/Characters/Enemies/Hitode_Red.nk";
    inline constexpr char* Red_Ankou    = "./resources/Models/Characters/Enemies/Ankou_Red.nk";

    inline constexpr char* Boss         = "./resources/Models/Characters/Boss/BossWhite.nk";
    inline constexpr char* Boss2        = "./resources/Models/Characters/Boss/Boss2.nk";

}

namespace EnemyPSFilePath
{
    inline constexpr char* Lambert = "./resources/Shaders/PhisicsBase_PS.cso";
    inline constexpr char* Dissolve = "./resources/Shaders/Dissolve_PS.cso";
}


namespace EnemyKindsScale
{
    inline constexpr DirectX::XMFLOAT3 Utubo         = { 0.04f,0.04f,0.04f };
    inline constexpr DirectX::XMFLOAT3 Lock_Shark    = { 0.03f,0.03f,0.03f };
    inline constexpr DirectX::XMFLOAT3 Move_Shark    = { 0.02f,0.02f,0.02f };
    inline constexpr DirectX::XMFLOAT3 Hitode        = { 0.02f,0.02f,0.02f };
    inline constexpr DirectX::XMFLOAT3 Ankou         = { 0.02f,0.02f,0.02f };
}

namespace EnemyKindsHP
{
    inline constexpr float Utubo         = 4.0f;
    inline constexpr float Lock_Shark    = 6.0f;
    inline constexpr float Move_Shark    = 2.0f;
    inline constexpr float Hitode        = 4.0f;
    inline constexpr float Ankou         = 2.0f;
}

namespace EnemyAnim
{
    namespace Ankou
    {
        enum AnimationName {
            move,
        };
    }

    namespace Hitode
    {
        enum AnimationName {
            spin_loop,
            spin_begin,
            spin_end,
            no_spin,
        };
    }

    namespace Shark
    {
        enum AnimationName {
            move,
            stand,
            attack_begin,
            attack_loop,
        };
    }

    namespace Utubo
    {
        enum AnimationName {
            move,
            stand,
            attack_begin,
            attack_loop,
        };
    }
}