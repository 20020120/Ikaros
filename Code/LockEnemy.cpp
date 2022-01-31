
#include "LockEnemy.h"

#include "GameSystem.h"
#include "MovePhase_End_Side.h"
#include "MovePhase_End_Top.h"
#include "MovePhase_Idle.h"
#include "MovePhase_Rotate.h"
#include "MovePhase_Rush.h"


#include "SceneManager.h"
#include "ProjectileManager.h"
#include "Bullet.h"
#include "Energy.h"
#include "MathSupport.h"


EnemyLock_Shot_B::EnemyLock_Shot_B(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
    EnemyPSFilePath::Lambert
)
{
    // �����̐ݒ�
    type = type_;

    // �ڕW�̐ݒ�
    target = target_;

    // ���x�̐ݒ�
    status.speed = 8.0f;
}

void EnemyLock_Shot_B::Initialize()
{
    move_state = std::make_unique<MoveState>();
    BaseMove* rotate_to_target = nullptr;

    const GameSystem::CAMERA_DIRECTION& camera_direction = GameSystem::Instance().GetCameraDirection();
    if (camera_direction == GameSystem::CAMERA_DIRECTION::SIDE)
    {
        origin_pos = t.Position.y >= 0.0f ? origin_pos_up : origin_pos_down;

        rotate_to_target = new RotateXToTargetLerp(target, 0.4f);
    }
    else if (camera_direction == GameSystem::CAMERA_DIRECTION::TOP)
    {
        origin_pos = t.Position.x >= 0.0f ? origin_pos_right : origin_pos_left;

        rotate_to_target = new RotateYToTargetLerp(target, 0.4f);
    }

    BaseMove* return_origin_pos = new ReturnOriginPosition(origin_pos);

    move_state->SetMove(return_origin_pos);
    move_state->AddTransition(return_origin_pos, rotate_to_target, [&](float) {return IsInOriginPos(origin_pos); });
}

void EnemyLock_Shot_B::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);
}

void EnemyLock_Shot_B::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    invTimer -= elapsedTime;

    // �N�[���_�E��
    if (CanShot(elapsedTime) == false) return;


    if (move_state->GetNowMove() <= 0) return;
    //�e����
    BulletShot(elapsedTime);
}

bool EnemyLock_Shot_B::IsInOriginPos(DirectX::XMFLOAT3 origin)
{
    constexpr float epsilon_size = 2.0f;

    if (t.Position.z < origin.z - epsilon_size) return false;
    if (t.Position.y < origin.y - epsilon_size) return false;
    if (t.Position.z > origin.z + epsilon_size) return false;
    if (t.Position.y > origin.y + epsilon_size) return false;

    return true;
}



EnemyLock_Shot_E::EnemyLock_Shot_E(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
    EnemyPSFilePath::Lambert
)
{
    // �����̐ݒ�
    type = type_;

    // �ڕW�̐ݒ�
    target = target_;

    // ���x�̐ݒ�
    status.speed = 8.0f;
}

void EnemyLock_Shot_E::Initialize()
{
    move_state = std::make_unique<MoveState>();
    BaseMove* rotate_to_target = nullptr;

    const GameSystem::CAMERA_DIRECTION& camera_direction = GameSystem::Instance().GetCameraDirection();
    if (camera_direction == GameSystem::CAMERA_DIRECTION::SIDE)
    {
        origin_pos = t.Position.y >= 0.0f ? origin_pos_up : origin_pos_down;

        rotate_to_target = new RotateXToTargetLerp(target, 0.4f);

    }
    else if (camera_direction == GameSystem::CAMERA_DIRECTION::TOP)
    {
        origin_pos = t.Position.x >= 0.0f ? origin_pos_right : origin_pos_left;

        rotate_to_target = new RotateYToTargetLerp(target, 0.4f);
    }

    BaseMove* return_origin_pos = new ReturnOriginPosition(origin_pos);

    move_state->SetMove(return_origin_pos);
    move_state->AddTransition(return_origin_pos, rotate_to_target, [&](float) {return IsInOriginPos(origin_pos); });

}

void EnemyLock_Shot_E::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);
}

void EnemyLock_Shot_E::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    invTimer -= elapsedTime;

    // �N�[���_�E��
    if (CanShot(elapsedTime) == false) return;

    if (move_state->GetNowMove() <= 0) return;
    //�e����
    EnergyShot(elapsedTime);
}

bool EnemyLock_Shot_E::IsInOriginPos(DirectX::XMFLOAT3 origin)
{
    constexpr float epsilon_size = 2.0f;

    if (t.Position.z < origin.z - epsilon_size) return false;
    if (t.Position.y < origin.y - epsilon_size) return false;
    if (t.Position.z > origin.z + epsilon_size) return false;
    if (t.Position.y > origin.y + epsilon_size) return false;

    return true;
}




EnemyLock_Straight_Target::EnemyLock_Straight_Target(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, bool hard_mode_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Utubo : EnemyModelFilePath::Red_Utubo,
    EnemyPSFilePath::Lambert
)
{
    // �����̐ݒ�
    type = type_;

    // �ڕW�̐ݒ�
    target = target_;

    // ���x�̐ݒ�
    status.speed = 45.0f;

    // �̗͂̐ݒ�
    status.hitPoint = EnemyKindsHP::Utubo;

    // �X�P�[���̐ݒ�
    t.Scale = EnemyKindsScale::Utubo;

    // �����蔻��̐ݒ�
    hitAreaSize.x = hitAreaSize.x + 3.0f;
    hitAreaSize.y = hitAreaSize.y + 3.0f;
    hitAreaSize.z = hitAreaSize.z + 8.0f;

    // �o�����o����
    is_appear = true;

    // �n�[�h���[�h��
    hard_mode = hard_mode_;

    // �����A�j���[�V����
    Model->f_PlayAnimation(EnemyAnim::Utubo::AnimationName::move, true);
}

void EnemyLock_Straight_Target::Initialize()
{
    t.Position.z = turn_z_axis_size;


    // ���E����܂ł̎��ԃt���O
    constexpr float seconds = 7.0f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);

    

    move_state = std::make_unique<MoveState>();
    BaseMove* rotate_to_target = nullptr;
    BaseMove* idle = new Idle(0.5f, nullptr, 1);
    BaseMove* rush_idle = new Idle(0.5f, nullptr, 1);
    BaseMove* move_front = new RushFront(status.speed, 5.0f);
    
    BaseMove* death = new Death();



    // �����_�� ���n�_�̌���֐�
    auto SetOriginPos = [](DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 origin_plus_est, DirectX::XMFLOAT3 origin_minus_est, DirectX::XMFLOAT3 origin_plus, DirectX::XMFLOAT3 origin_minus) {

        // �ʒu���狗�����v�Z����
        const float length_plus_est = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_plus_est))));
        const float length_minus_est = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_minus_est))));

        DirectX::XMFLOAT3 origin_pos = {};

        // �ʒu�����̕����߂��ꍇ
        if (length_plus_est < length_minus_est)
        {
            const float length_plus = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_plus))));

            if (length_plus_est < length_plus)
            {
                origin_pos = origin_plus_est;
            }
            else
            {
                origin_pos = origin_plus;
            }
            

            return origin_pos;

        }

        // �ʒu�����̕����߂��ꍇ
        else
        {
            const float length_down = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_minus))));

            if (length_minus_est < length_down)
            {
                origin_pos = origin_minus_est;
            }
            else
            {
                origin_pos = origin_minus;
            }


            return origin_pos;
        }
    };


    // �ڕW�Ɍ����܂ł̎���
    const float limit_time = (hard_mode) ? 1.0f : 2.0f;

    const float shift_z = (hard_mode) ? 10.0f : 0.0f;

    const GameSystem::CAMERA_DIRECTION& camera_direction = GameSystem::Instance().GetCameraDirection();
    if (camera_direction == GameSystem::CAMERA_DIRECTION::SIDE)
    {
        // (MultiShot�Ɣ��Ȃ��悤��origin_������Œl��ύX���Ă���)
        origin_pos = SetOriginPos(t.Position,
            { origin_pos_upest.x,       origin_pos_upest.y + 5.0f,         origin_pos_upest.z + shift_z },
            { origin_pos_downest.x,     origin_pos_downest.y - 5.0f,       origin_pos_downest.z + shift_z },
            { origin_pos_up.x,              origin_pos_up.y - 5.0f,            origin_pos_up.z + shift_z },
            { origin_pos_down.x,            origin_pos_down.y + 5.0f,          origin_pos_down.z + shift_z });


        RotateXToTargetLerp_LimitTimer* rotate_x_to_target = new RotateXToTargetLerp_LimitTimer(target, limit_time);
        rotate_to_target = rotate_x_to_target;
    }
    else if (camera_direction == GameSystem::CAMERA_DIRECTION::TOP)
    {
        // (MultiShot�Ɣ��Ȃ��悤��origin_������Œl��ύX���Ă���)
        origin_pos = SetOriginPos(t.Position,
            {origin_pos_rightest.x + 5.0f,     origin_pos_rightest.y,      origin_pos_rightest.z},
            {origin_pos_leftest.x - 5.0f,      origin_pos_leftest.y,       origin_pos_leftest.z},
            { origin_pos_right.x - 5.0f,           origin_pos_right.y,         origin_pos_right.z},
            {origin_pos_left.x + 5.0f,             origin_pos_left.y,          origin_pos_left.z});


        RotateYToTargetLerp_LimitTimer* rotate_y_to_target = new RotateYToTargetLerp_LimitTimer(target, limit_time);
        rotate_to_target = rotate_y_to_target;
    }


    BaseMove* return_origin_pos = new ReturnOriginPosition(origin_pos,1.2f);

    move_state->SetMove(return_origin_pos)
        .AddTransition(return_origin_pos, idle, [&](float) {return IsInOriginPos(origin_pos); })
        .SetFinalize(return_origin_pos, [&]() {is_appear = false; Model->f_PlayAnimation(EnemyAnim::Utubo::AnimationName::stand, true); })
        .AddTransition(idle, rotate_to_target, [&](float) {return (timer.NowTime() <= 7.0f - 1.5f);/* �^�C�}�[�̎c�莞�Ԃ�5.5�b�ȉ��̎�(1.5�b�i�񂾂Ƃ�) = true */ })
        .SetFinalize(idle, [&]() {timer.Reset(); })
        .AddTransition(rotate_to_target, rush_idle, [&](float) {return (timer.NowTime() <= 7.0f - ((hard_mode) ? 1.0f : 2.0f));/* �^�C�}�[�̎c�莞�Ԃ�5.0�b�ȉ��̎�(2.0�b�i�񂾂Ƃ�) = true */ })
        .SetInitialize(rush_idle, [&]() {timer.Reset(); Model->f_PlayAnimation(EnemyAnim::Utubo::AnimationName::attack_begin); })
        .AddTransition(rush_idle, move_front, [&](float) {return (timer.NowTime() <= 7.0f - ((hard_mode) ? 0.5f : 1.0f));/* �^�C�}�[�̎c�莞�Ԃ�6.5�b�ȉ��̎�(0.5�b�i�񂾂Ƃ�) = true */ })
        .SetInitialize(move_front, [&]() {timer.Reset(); Model->f_PlayAnimation(EnemyAnim::Utubo::AnimationName::attack_loop, true); })
        .AddTransition(move_front, death, [&](float) {return (timer.NowTime() <= 7.0f - 4.0f); })
        .SetInitialize(death, [&]()
        {
            Destroy();
        });

    ;
}

void EnemyLock_Straight_Target::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);


    if (hard_mode)
    {
        Model->Update(elapsedTime);
    }
}

bool EnemyLock_Straight_Target::IsInOriginPos(DirectX::XMFLOAT3 origin)
{
    constexpr float epsilon_size = 2.0f;

    if (t.Position.z < origin.z - epsilon_size) return false;
    if (t.Position.y < origin.y - epsilon_size) return false;
    if (t.Position.z > origin.z + epsilon_size) return false;
    if (t.Position.y > origin.y + epsilon_size) return false;

    return true;
}










EnemyLock_MultiShot_B::EnemyLock_MultiShot_B(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{
    // �����̐ݒ�
    type = type_;

    // �ˌ����[�g
    status.shotInterval = 1.2f;

    // �̗͂̐ݒ�
    status.hitPoint = EnemyKindsHP::Lock_Shark;

    // �X�P�[���̐ݒ�
    t.Scale = EnemyKindsScale::Lock_Shark;

    // �����A�j���[�V����
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyLock_MultiShot_B::Initialize()
{
    // �����n�߂Ă���A���E����܂ł̎��ԃt���O
    constexpr float seconds = 8.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);


    // �s���J�ڂ̐ݒ�
    move_state = std::make_unique<MoveState>();
    BaseMove* idle = new Idle(0.5f, nullptr, 1);

    // �J�����̌����̎擾
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    // �����_�� ���n�_�̌���֐� (���E���������肷��)
    BaseMove* end = nullptr;
    auto SetOriginPos = [&](DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 origin_plus_est, DirectX::XMFLOAT3 origin_minus_est, DirectX::XMFLOAT3 origin_plus, DirectX::XMFLOAT3 origin_minus){

        // �ʒu���狗�����v�Z����
        const float length_plus_est = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_plus_est))));
        const float length_minus_est = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_minus_est))));

        DirectX::XMFLOAT3 origin_pos = {};

        // �ʒu�����̕����߂��ꍇ
        if (length_plus_est < length_minus_est)
        {
            const float length_plus = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_plus))));

            if (length_plus_est < length_plus)
            {
                origin_pos = origin_plus_est;
            }
            else
            {
                origin_pos = origin_plus;
            }

            // ���E����
            if (camera_direction == c_d::SIDE)
            {
                end = new End_ToUp_Side();
            }
            else
            {
                end = new End_ToRight_Top();
            }

            return origin_pos;

        }

        // �ʒu�����̕����߂��ꍇ
        else
        {
            const float length_down = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_minus))));

            if (length_minus_est < length_down)
            {
                origin_pos = origin_minus_est;
            }
            else
            {
                origin_pos = origin_minus;
            }

            // ���E����
            if (camera_direction == c_d::SIDE)
            {
                end = new End_ToDown_Side();
            }
            else
            {
                end = new End_ToLeft_Top();
            }


            return origin_pos;
        }
    };

    if (camera_direction == c_d::SIDE)
    {
        origin_pos = SetOriginPos(t.Position,
            { origin_pos_upest.x,       origin_pos_upest.y,         origin_pos_upest.z + 5.0f },
            { origin_pos_downest.x,     origin_pos_downest.y,       origin_pos_downest.z + 5.0f },
            { origin_pos_up.x,              origin_pos_up.y,            origin_pos_up.z + 5.0f },
            { origin_pos_down.x,            origin_pos_down.y,          origin_pos_down.z + 5.0f });
        SelectedShot = &EnemyLock_MultiShot_B::TripleShot;
    }
    else if (camera_direction == c_d::TOP)
    {
        origin_pos = SetOriginPos(t.Position,
            { origin_pos_rightest.x,     origin_pos_rightest.y,      origin_pos_rightest.z },
            { origin_pos_leftest.x,      origin_pos_leftest.y,       origin_pos_leftest.z },
            { origin_pos_right.x,           origin_pos_right.y,         origin_pos_right.z },
            { origin_pos_left.x,             origin_pos_left.y,          origin_pos_left.z });
        SelectedShot = &EnemyLock_MultiShot_B::PentaShot;
    }


    BaseMove* return_origin_pos = new ReturnOriginPosition(origin_pos,1.2f);

    BaseMove* death = new Death();

    move_state->SetMove(return_origin_pos)
        .AddTransition(return_origin_pos, idle, [&](float) {return IsInOriginPos(origin_pos); })
        .AddTransition(idle, end, [&](float) {return (timer.NowTime() <= 0.0f); })
        .SetInitialize(idle,[&](){Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin); })
        .SetFinalize(idle, [&]() {timer.Reset(); is_appear = true; })
        .SetFinalize(end, [&]() {timer.Reset(); })
        .AddTransition(end, death, [&](float) {return (timer.NowTime() <= 8.5f - 2.5f); })
        .SetInitialize(death, [&]() {Destroy(); });

}

void EnemyLock_MultiShot_B::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);


    if (!is_appear)return;
    if (timer.NowTime() > 8.5f - 0.2f)return;
    if (Model->GetCurrentAnimationNumber() == EnemyAnim::Shark::AnimationName::move)return;
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyLock_MultiShot_B::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //���G���ԍX�V
    invTimer -= elapsedTime;

    if(move_state->GetNowMove() >= STATE::sIdle)
    {
        timer.Update(elapsedTime);
    }

    Shot(elapsedTime);
}

bool EnemyLock_MultiShot_B::IsInOriginPos(DirectX::XMFLOAT3 origin)
{
    constexpr float epsilon_size = 2.0f;

    if (t.Position.z < origin.z - epsilon_size) return false;
    if (t.Position.y < origin.y - epsilon_size) return false;
    if (t.Position.z > origin.z + epsilon_size) return false;
    if (t.Position.y > origin.y + epsilon_size) return false;

    return true;
}

void EnemyLock_MultiShot_B::Shot(float elapsedTime)
{
    // �U���J�n�A�j�����I��������
    if (!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
    }

    if (CanShot(elapsedTime) == false)return;

    const size_t index = move_state->GetNowMove();


    if (timer.NowTime() <= 5.5f) return;
    if (index != STATE::sIdle) return;

    // �J���������ɂ������ˌ����@���X�V
    (this->*SelectedShot)(elapsedTime);
    
}

void EnemyLock_MultiShot_B::BulletRegister(ID3D11Device* p_device, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 front)
{
    if(type == BaseObject::Type::BLUE)
    {
        ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device, BaseProjectile::Parent::BLUEENEMY,
        t.Position + front * 4.5f, direction));
    }
    else if(type == BaseObject::Type::RED)
    {
        ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device, BaseProjectile::Parent::REDENEMY,
            t.Position + front * 4.5f, direction));
    }
}

void EnemyLock_MultiShot_B::TripleShot(float elapsedTime)
{
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();

    DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(transform.r[2]);
    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(transform.r[0]);

    DirectX::XMFLOAT3 direction, front_;
    DirectX::XMStoreFloat3(&direction, front);
    DirectX::XMStoreFloat3(&front_, front);


    // �O����
    BulletRegister(p_device, direction, front_);



    constexpr float half_half_pi = DirectX::XMConvertToRadians(45.0f);

    // �O�����x�N�g����+45�xx����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(right, half_half_pi)));

    // �O�̎΂ߏ����
    BulletRegister(p_device, direction, front_);




    // �O�����x�N�g����-45�xx����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(right, -half_half_pi)));

    // �O�̎΂߉�����
    BulletRegister(p_device, direction, front_);
}

void EnemyLock_MultiShot_B::PentaShot(float elapsedTime)
{
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();

    DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(transform.r[2]);
    DirectX::XMVECTOR up = DirectX::XMVector3Normalize(transform.r[1]);

    DirectX::XMFLOAT3 direction, front_;
    DirectX::XMStoreFloat3(&direction, front);
    DirectX::XMStoreFloat3(&front_, front);


    // �O����
    BulletRegister(p_device, direction, front_);



    constexpr float radian = DirectX::XMConvertToRadians(15.0f);

    // �O�����x�N�g����+15�xy����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(up, radian)));

    // �O�̎΂߂��傢�����
    BulletRegister(p_device, direction, front_);

    // �O�����x�N�g����+30�xy����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(up, radian + radian)));

    // �O�̎΂ߏ����
    BulletRegister(p_device, direction, front_);




    // �O�����x�N�g����-15�xy����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(up, -radian)));

    // �O�̎΂߂��傢������
    BulletRegister(p_device, direction, front_);

    // �O�����x�N�g����-30�xy����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(up, -radian + -radian)));

    // �O�̎΂߉�����
    BulletRegister(p_device, direction, front_);
}
















EnemyLock_MultiShot_E::EnemyLock_MultiShot_E(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{
    // �����̐ݒ�
    type = type_;

    // �ˌ����[�g
    status.shotInterval = 1.2f;

    // �̗͂̐ݒ�
    status.hitPoint = EnemyKindsHP::Lock_Shark;

    // �X�P�[���̐ݒ�
    t.Scale = EnemyKindsScale::Lock_Shark * 0.8f;

    // �����A�j���[�V����
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyLock_MultiShot_E::Initialize()
{
    // �����n�߂Ă���A���E����܂ł̎��ԃt���O
    constexpr float seconds = 5.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);


    // �s���J�ڂ̐ݒ�
    move_state = std::make_unique<MoveState>();
    BaseMove* idle = new Idle(9999.0f, nullptr, 1);

    // �J�����̌����̎擾
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    // �����_�� ���n�_�̌���֐� (���E���������肷��)
    BaseMove* end = nullptr;
    auto SetOriginPos = [&](DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 origin_plus_est, DirectX::XMFLOAT3 origin_minus_est, DirectX::XMFLOAT3 origin_plus, DirectX::XMFLOAT3 origin_minus) {

        // �ʒu���狗�����v�Z����
        const float length_plus_est = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_plus_est))));
        const float length_minus_est = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_minus_est))));

        DirectX::XMFLOAT3 origin_pos = {};

        // �ʒu�����̕����߂��ꍇ
        if (length_plus_est < length_minus_est)
        {
            const float length_plus = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_plus))));

            if (length_plus_est < length_plus)
            {
                origin_pos = origin_plus_est;
            }
            else
            {
                origin_pos = origin_plus;
            }

            // ���E����
            if (camera_direction == c_d::SIDE)
            {
                end = new End_ToUp_Side();
            }
            else
            {
                end = new End_ToRight_Top();
            }

            return origin_pos;

        }

        // �ʒu�����̕����߂��ꍇ
        else
        {
            const float length_down = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_minus))));

            if (length_minus_est < length_down)
            {
                origin_pos = origin_minus_est;
            }
            else
            {
                origin_pos = origin_minus;
            }

            // ���E����
            if (camera_direction == c_d::SIDE)
            {
                end = new End_ToDown_Side();
            }
            else
            {
                end = new End_ToLeft_Top();
            }


            return origin_pos;
        }
    };

    if (camera_direction == c_d::SIDE)
    {
        origin_pos = SetOriginPos(t.Position, origin_pos_upest, origin_pos_downest, origin_pos_up, origin_pos_down);
        SelectedShot = &EnemyLock_MultiShot_E::TripleShot;
    }
    else if (camera_direction == c_d::TOP)
    {
        origin_pos = SetOriginPos(t.Position, origin_pos_rightest, origin_pos_leftest, origin_pos_right, origin_pos_left);
        SelectedShot = &EnemyLock_MultiShot_E::PentaShot;
    }


    BaseMove* return_origin_pos = new ReturnOriginPosition(origin_pos, 1.2f);

    BaseMove* death = new Death();

    move_state->SetMove(return_origin_pos)
        .AddTransition(return_origin_pos, idle, [&](float) {return IsInOriginPos(origin_pos); })
        .AddTransition(idle, end, [&](float) {return (timer.NowTime() <= 0.0f); })
        .SetInitialize(idle, [&]() {Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin); })
        .SetFinalize(idle, [&]() {timer.Reset(); is_appear = true; })
        .SetFinalize(end, [&]() {timer.Reset(); })
        .AddTransition(end, death, [&](float) {return (timer.NowTime() <= 0.0f); })
        .SetInitialize(death, [&]() {Destroy(); });

}

void EnemyLock_MultiShot_E::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);


    if (!is_appear)return;
    if (timer.NowTime() > 5.5f - 0.2f)return;
    if (Model->GetCurrentAnimationNumber() == EnemyAnim::Shark::AnimationName::move)return;
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move,true);
        
}

void EnemyLock_MultiShot_E::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //���G���ԍX�V
    invTimer -= elapsedTime;

    if (move_state->GetNowMove() >= STATE::sIdle)
    {
        timer.Update(elapsedTime);
    }

    Shot(elapsedTime);
}

bool EnemyLock_MultiShot_E::IsInOriginPos(DirectX::XMFLOAT3 origin)
{
    constexpr float epsilon_size = 2.0f;

    if (t.Position.z < origin.z - epsilon_size) return false;
    if (t.Position.y < origin.y - epsilon_size) return false;
    if (t.Position.z > origin.z + epsilon_size) return false;
    if (t.Position.y > origin.y + epsilon_size) return false;

    return true;
}

void EnemyLock_MultiShot_E::Shot(float elapsedTime)
{
    // �U���J�n�A�j�����I��������
    if (!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
    }

    if (CanShot(elapsedTime) == false)return;

    const size_t index = move_state->GetNowMove();


    if (timer.NowTime() <= 2.0f) return;
    if (index != STATE::sIdle) return;

    // �J���������ɂ������ˌ����@���X�V
    (this->*SelectedShot)(elapsedTime);

}

void EnemyLock_MultiShot_E::EnergyRegister(ID3D11Device* p_device, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 front)
{
    if (type == BaseObject::Type::BLUE)
    {
        ProjectileManager::Instance().RegisterProjectile(new Energy(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position + front * 4.5f, direction));
    }
    else if (type == BaseObject::Type::RED)
    {
        ProjectileManager::Instance().RegisterProjectile(new Energy(p_device, BaseProjectile::Parent::REDENEMY,
            t.Position + front * 4.5f, direction));
    }
}

void EnemyLock_MultiShot_E::TripleShot(float elapsedTime)
{
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();

    DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(transform.r[2]);
    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(transform.r[0]);

    DirectX::XMFLOAT3 direction, front_;
    DirectX::XMStoreFloat3(&direction, front);
    DirectX::XMStoreFloat3(&front_, front);


    // �O����
    EnergyRegister(p_device, direction, front_);



    constexpr float half_half_pi = DirectX::XMConvertToRadians(45.0f);

    // �O�����x�N�g����+45�xx����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(right, half_half_pi)));

    // �O�̎΂ߏ����
    EnergyRegister(p_device, direction, front_);




    // �O�����x�N�g����-45�xx����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(right, -half_half_pi)));

    // �O�̎΂߉�����
    EnergyRegister(p_device, direction, front_);
}

void EnemyLock_MultiShot_E::PentaShot(float elapsedTime)
{
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();

    DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(transform.r[2]);
    DirectX::XMVECTOR up = DirectX::XMVector3Normalize(transform.r[1]);

    DirectX::XMFLOAT3 direction, front_;
    DirectX::XMStoreFloat3(&direction, front);
    DirectX::XMStoreFloat3(&front_, front);


    // �O����
    EnergyRegister(p_device, direction, front_);



    constexpr float radian = DirectX::XMConvertToRadians(15.0f);

    // �O�����x�N�g����+15�xy����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(up, radian)));

    // �O�̎΂߂��傢�����
    EnergyRegister(p_device, direction, front_);

    // �O�����x�N�g����+30�xy����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(up, radian + radian)));

    // �O�̎΂ߏ����
    EnergyRegister(p_device, direction, front_);




    // �O�����x�N�g����-15�xy����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(up, -radian)));

    // �O�̎΂߂��傢������
    EnergyRegister(p_device, direction, front_);

    // �O�����x�N�g����-30�xy����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(up, -radian + -radian)));

    // �O�̎΂߉�����
    EnergyRegister(p_device, direction, front_);
}






EnemyLock_TargetSingleShot::EnemyLock_TargetSingleShot(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, bool hard_mode) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{
    // �����̐ݒ�
    type = type_;

    // �ڕW�̐ݒ�
    target = target_;

    // �ˌ����[�g
    status.shotInterval = 1.2f;

    // ���x�̐ݒ� (�e���˂̂���)
    status.speed = (hard_mode) ? 38.0f : 21.0f;

    // �̗͂̐ݒ�
    status.hitPoint = EnemyKindsHP::Lock_Shark;

    // �X�P�[���̐ݒ�
    t.Scale = EnemyKindsScale::Lock_Shark;

    // �����A�j���[�V����
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyLock_TargetSingleShot::Initialize()
{
    // �����n�߂Ă���A���E����܂ł̎��ԃt���O
    constexpr float seconds = 8.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);


    // �s���J�ڂ̐ݒ�
    move_state = std::make_unique<MoveState>();
    BaseMove* rotate_to_target = nullptr;

    // �J�����̌����̎擾
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    // �����_�� ���n�_�̌���֐� (���E���������肷��)
    BaseMove* end = nullptr;
    auto SetOriginPos = [&](DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 origin_plus_est, DirectX::XMFLOAT3 origin_minus_est, DirectX::XMFLOAT3 origin_plus, DirectX::XMFLOAT3 origin_minus) {

        // �ʒu���狗�����v�Z����
        const float length_plus_est = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_plus_est))));
        const float length_minus_est = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_minus_est))));

        DirectX::XMFLOAT3 origin_pos = {};

        // �ʒu�����̕����߂��ꍇ
        if (length_plus_est < length_minus_est)
        {
            const float length_plus = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_plus))));

            if (length_plus_est < length_plus)
            {
                origin_pos = origin_plus_est;
            }
            else
            {
                origin_pos = origin_plus;
            }

            // ���E����
            if (camera_direction == c_d::SIDE)
            {
                end = new End_ToUp_Side();
            }
            else
            {
                end = new End_ToRight_Top();
            }

            return origin_pos;

        }

        // �ʒu�����̕����߂��ꍇ
        else
        {
            const float length_down = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_minus))));

            if (length_minus_est < length_down)
            {
                origin_pos = origin_minus_est;
            }
            else
            {
                origin_pos = origin_minus;
            }

            // ���E����
            if (camera_direction == c_d::SIDE)
            {
                end = new End_ToDown_Side();
            }
            else
            {
                end = new End_ToLeft_Top();
            }


            return origin_pos;
        }
    };

    if (camera_direction == c_d::SIDE)
    {
        origin_pos = SetOriginPos(t.Position,
            { origin_pos_upest.x,       origin_pos_upest.y + 5.0f,         origin_pos_upest.z + -5.0f },
            { origin_pos_downest.x,     origin_pos_downest.y - 5.0f,       origin_pos_downest.z + -5.0f },
            { origin_pos_up.x,              origin_pos_up.y - 5.0f,            origin_pos_up.z + -5.0f },
            { origin_pos_down.x,            origin_pos_down.y + 5.0f,          origin_pos_down.z + -5.0f });

        rotate_to_target = new RotateXToTargetLerp(target, 0.8f);
    }
    else if (camera_direction == c_d::TOP)
    {
        origin_pos = SetOriginPos(t.Position,
            { origin_pos_rightest.x + 5.0f,     origin_pos_rightest.y,      origin_pos_rightest.z + -5.0f },
            { origin_pos_leftest.x - 5.0f,      origin_pos_leftest.y,       origin_pos_leftest.z + -5.0f },
            { origin_pos_right.x - 5.0f,           origin_pos_right.y,         origin_pos_right.z + -5.0f },
            { origin_pos_left.x + 5.0f,             origin_pos_left.y,          origin_pos_left.z + -5.0f });

        rotate_to_target = new RotateYToTargetLerp(target, 0.8f);
    }


    BaseMove* return_origin_pos = new ReturnOriginPosition(origin_pos, 1.2f);

    BaseMove* death = new Death();

    move_state->SetMove(return_origin_pos)
        .AddTransition(return_origin_pos, rotate_to_target, [&](float) {return IsInOriginPos(origin_pos); })
        .AddTransition(rotate_to_target, end, [&](float) {return (timer.NowTime() <= 0.0f); })
        .SetFinalize(return_origin_pos, [&]() {Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin); })
        .SetFinalize(rotate_to_target, [&]() {timer.Reset(); is_appear = true; })
        .SetFinalize(end, [&]() {timer.Reset(); })
        .AddTransition(end, death, [&](float) {return (timer.NowTime() <= 8.5f - 3.5f); })
        .SetInitialize(death, [&]() {Destroy(); });
}

void EnemyLock_TargetSingleShot::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);


    if (!is_appear)return;
    if (timer.NowTime() > 8.5f - 0.2f)return;
    if (Model->GetCurrentAnimationNumber() == EnemyAnim::Shark::AnimationName::move)return;
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyLock_TargetSingleShot::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //���G���ԍX�V
    invTimer -= elapsedTime;

    if (move_state->GetNowMove() >= STATE::sIdle)
    {
        timer.Update(elapsedTime);
    }

    Shot(elapsedTime);
}

bool EnemyLock_TargetSingleShot::IsInOriginPos(DirectX::XMFLOAT3 origin)
{
    constexpr float epsilon_size = 2.0f;

    if (t.Position.z < origin.z - epsilon_size) return false;
    if (t.Position.y < origin.y - epsilon_size) return false;
    if (t.Position.z > origin.z + epsilon_size) return false;
    if (t.Position.y > origin.y + epsilon_size) return false;

    return true;
}

void EnemyLock_TargetSingleShot::Shot(float elapsedTime)
{
    // �U���J�n�A�j�����I��������
    if (!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
    }

    if (CanShot(elapsedTime) == false)return;

    const size_t index = move_state->GetNowMove();

    
    if (index != STATE::sIdle) return;

    // �J���������ɂ������ˌ����@���X�V
    BulletTargetShot(elapsedTime, *target);

}









EnemyLock_TargetMultiShot_B::EnemyLock_TargetMultiShot_B(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, bool hard_mode) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
), limit_shot_time(hard_mode ? 2.5f : 5.5f)
{
    // �����̐ݒ�
    type = type_;

    // �ڕW�̐ݒ�
    target = target_;

    // �ˌ����[�g
    status.shotInterval = 1.2f;

    // �̗͂̐ݒ�
    status.hitPoint = EnemyKindsHP::Lock_Shark;

    // �X�P�[���̐ݒ�
    t.Scale = EnemyKindsScale::Lock_Shark;

    // �����A�j���[�V����
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyLock_TargetMultiShot_B::Initialize()
{
    // �����n�߂Ă���A���E����܂ł̎��ԃt���O
    constexpr float seconds = 8.5f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);


    // �s���J�ڂ̐ݒ�
    move_state = std::make_unique<MoveState>();
    BaseMove* rotate_to_target = nullptr;

    // �J�����̌����̎擾
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    // �����_�� ���n�_�̌���֐� (���E���������肷��)
    BaseMove* end = nullptr;
    auto SetOriginPos = [&](DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 origin_plus_est, DirectX::XMFLOAT3 origin_minus_est, DirectX::XMFLOAT3 origin_plus, DirectX::XMFLOAT3 origin_minus) {

        // �ʒu���狗�����v�Z����
        const float length_plus_est = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_plus_est))));
        const float length_minus_est = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_minus_est))));

        DirectX::XMFLOAT3 origin_pos = {};

        // �ʒu�����̕����߂��ꍇ
        if (length_plus_est < length_minus_est)
        {
            const float length_plus = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_plus))));

            if (length_plus_est < length_plus)
            {
                origin_pos = origin_plus_est;
            }
            else
            {
                origin_pos = origin_plus;
            }

            // ���E����
            if (camera_direction == c_d::SIDE)
            {
                end = new End_ToUp_Side();
            }
            else
            {
                end = new End_ToRight_Top();
            }

            return origin_pos;

        }

        // �ʒu�����̕����߂��ꍇ
        else
        {
            const float length_down = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&origin_minus))));

            if (length_minus_est < length_down)
            {
                origin_pos = origin_minus_est;
            }
            else
            {
                origin_pos = origin_minus;
            }

            // ���E����
            if (camera_direction == c_d::SIDE)
            {
                end = new End_ToDown_Side();
            }
            else
            {
                end = new End_ToLeft_Top();
            }


            return origin_pos;
        }
    };

    if (camera_direction == c_d::SIDE)
    {
        origin_pos = SetOriginPos(t.Position, origin_pos_upest, origin_pos_downest, origin_pos_up, origin_pos_down);
        SelectedShot = &EnemyLock_TargetMultiShot_B::TripleShot;
        rotate_to_target = new RotateXToTargetLerp(target, 0.8f);
    }
    else if (camera_direction == c_d::TOP)
    {
        origin_pos = SetOriginPos(t.Position, origin_pos_rightest, origin_pos_leftest, origin_pos_right, origin_pos_left);
        SelectedShot = &EnemyLock_TargetMultiShot_B::PentaShot;
        rotate_to_target = new RotateYToTargetLerp(target, 0.8f);
    }


    BaseMove* return_origin_pos = new ReturnOriginPosition(origin_pos, 1.2f);

    BaseMove* death = new Death();

    move_state->SetMove(return_origin_pos)
        .AddTransition(return_origin_pos, rotate_to_target, [&](float) {return IsInOriginPos(origin_pos); })
        .AddTransition(rotate_to_target, end, [&](float) {return (timer.NowTime() <= 0.0f); })
        .SetFinalize(return_origin_pos, [&]() {Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_begin); })
        .SetFinalize(rotate_to_target, [&]() {timer.Reset(); is_appear = true; })
        .SetFinalize(end, [&]() {timer.Reset(); })
        .AddTransition(end, death, [&](float) {return (timer.NowTime() <= 8.5f - 1.5f); })
        .SetInitialize(death, [&]() {Destroy(); });

}

void EnemyLock_TargetMultiShot_B::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);


    if (!is_appear)return;
    if (timer.NowTime() > 8.5f - 0.2f)return;
    if (Model->GetCurrentAnimationNumber() == EnemyAnim::Shark::AnimationName::move)return;
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::move, true);
}

void EnemyLock_TargetMultiShot_B::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //���G���ԍX�V
    invTimer -= elapsedTime;

    if (move_state->GetNowMove() >= STATE::sIdle)
    {
        timer.Update(elapsedTime);
    }

    Shot(elapsedTime);
}

bool EnemyLock_TargetMultiShot_B::IsInOriginPos(DirectX::XMFLOAT3 origin)
{
    constexpr float epsilon_size = 2.0f;

    if (t.Position.z < origin.z - epsilon_size) return false;
    if (t.Position.y < origin.y - epsilon_size) return false;
    if (t.Position.z > origin.z + epsilon_size) return false;
    if (t.Position.y > origin.y + epsilon_size) return false;

    return true;
}

void EnemyLock_TargetMultiShot_B::Shot(float elapsedTime)
{
    // �U���J�n�A�j�����I��������
    if (!Model->GetIsPlayAnimation())
    {
        Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
    }

    if (CanShot(elapsedTime) == false)return;

    const size_t index = move_state->GetNowMove();


    if (timer.NowTime() <= limit_shot_time) return;
    if (index != STATE::sIdle) return;

    // �J���������ɂ������ˌ����@���X�V
    (this->*SelectedShot)(elapsedTime);

}

void EnemyLock_TargetMultiShot_B::BulletRegister(ID3D11Device* p_device, DirectX::XMFLOAT3 direction)
{
    if (type == BaseObject::Type::BLUE)
    {
        ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, direction));
    }
    else if (type == BaseObject::Type::RED)
    {
        ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device, BaseProjectile::Parent::REDENEMY,
            t.Position, direction));
    }
}

void EnemyLock_TargetMultiShot_B::TripleShot(float elapsedTime)
{
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();

    DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(transform.r[2]);
    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(transform.r[0]);

    DirectX::XMFLOAT3 direction;
    DirectX::XMStoreFloat3(&direction, front);


    // �O����
    BulletRegister(p_device, direction);



    constexpr float half_half_pi = DirectX::XMConvertToRadians(30.0f);

    // �O�����x�N�g����+45�xx����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(right, half_half_pi)));

    // �O�̎΂ߏ����
    BulletRegister(p_device, direction);




    // �O�����x�N�g����-45�xx����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(right, -half_half_pi)));

    // �O�̎΂߉�����
    BulletRegister(p_device, direction);
}

void EnemyLock_TargetMultiShot_B::PentaShot(float elapsedTime)
{
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();

    DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(transform.r[2]);
    DirectX::XMVECTOR up = DirectX::XMVector3Normalize(transform.r[1]);

    DirectX::XMFLOAT3 direction;
    DirectX::XMStoreFloat3(&direction, front);


    // �O����
    BulletRegister(p_device, direction);



    constexpr float radian = DirectX::XMConvertToRadians(15.0f);

    // �O�����x�N�g����+15�xy����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(up, radian)));

    // �O�̎΂߂��傢�����
    BulletRegister(p_device, direction);

    // �O�����x�N�g����+30�xy����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(up, radian + radian)));

    // �O�̎΂ߏ����
    BulletRegister(p_device, direction);




    // �O�����x�N�g����-15�xy����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(up, -radian)));

    // �O�̎΂߂��傢������
    BulletRegister(p_device, direction);

    // �O�����x�N�g����-30�xy����]������
    DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(up, -radian + -radian)));

    // �O�̎΂߉�����
    BulletRegister(p_device, direction);
}







EnemyTranslate_Shot_B_Top::EnemyTranslate_Shot_B_Top(ID3D11Device* Device, const Type& type_, bool hard_mode) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{

    // �����̐ݒ�
    type = type_;

    // ���x�̐ݒ�
    status.speed = 13.0f;

    // �ˌ����[�g
    status.shotInterval = (hard_mode == false) ? 0.8f : 0.3f;

    // �̗͂̐ݒ�
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // �X�P�[���̐ݒ�
    t.Scale = EnemyKindsScale::Move_Shark;

    // �����A�j���[�V����
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
}

void EnemyTranslate_Shot_B_Top::Initialize()
{
    constexpr float half_pi = DirectX::XMConvertToRadians(90.0f);

    // �����̕ύX
    if(t.Position.x > 0.0f)
    {
        r.Angle.y += half_pi;
    }
    else
    {
        r.Angle.y += -half_pi;
    }



    // �s���J�ڂ̐ݒ�
    move_state = std::make_unique<MoveState>();

    BaseMove* move_dir_z = new RushZ(status.speed, 5.0f);

    move_state->SetMove(move_dir_z);

}

void EnemyTranslate_Shot_B_Top::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);
}

void EnemyTranslate_Shot_B_Top::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //���G���ԍX�V
    invTimer -= elapsedTime;

    Shot(elapsedTime);
}

void EnemyTranslate_Shot_B_Top::Shot(float elapsedTime)
{
    if (CanShot(elapsedTime) == false)return;



    ID3D11Device* p_device = SceneManager::Instance().GetDevice();

    DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(transform.r[2]);

    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, front);

    const DirectX::XMFLOAT3 z_direction = {0.0f,0.0f,-1.0f * status.speed};

    DirectX::XMFLOAT3 direction;
    DirectX::XMStoreFloat3(&direction, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&z_direction), DirectX::XMVectorScale(front,2.0f * status.speed)));

    const BaseProjectile::Parent parent_type = (type == Type::RED) ? BaseProjectile::Parent::REDENEMY : BaseProjectile::Parent::BLUEENEMY;

    ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device,
        parent_type,
        t.Position + forward * 1.5f,
        direction,
        status.speed * 2.0f));

}






EnemyTranslate_Shot_B_Side::EnemyTranslate_Shot_B_Side(ID3D11Device* Device, const Type& type_, bool hard_mode) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Shark : EnemyModelFilePath::Red_Shark,
    EnemyPSFilePath::Lambert
)
{

    // �����̐ݒ�
    type = type_;

    // ���x�̐ݒ�
    status.speed = 13.0f;

    // �ˌ����[�g
    status.shotInterval = (hard_mode == false) ? 0.8f : 0.3f;

    // �̗͂̐ݒ�
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // �X�P�[���̐ݒ�
    t.Scale = EnemyKindsScale::Move_Shark;

    // �����A�j���[�V����
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
}

void EnemyTranslate_Shot_B_Side::Initialize()
{
    constexpr float half_pi = DirectX::XMConvertToRadians(90.0f);

    // �����̕ύX
    if (t.Position.y > 0.0f)
    {
        r.Angle.x += half_pi;
    }
    else
    {
        r.Angle.x += -half_pi;
    }



    // �s���J�ڂ̐ݒ�
    move_state = std::make_unique<MoveState>();

    BaseMove* move_dir_z = new RushZ(status.speed, 5.0f);

    move_state->SetMove(move_dir_z);

}

void EnemyTranslate_Shot_B_Side::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);
}

void EnemyTranslate_Shot_B_Side::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //���G���ԍX�V
    invTimer -= elapsedTime;

    Shot(elapsedTime);
}

void EnemyTranslate_Shot_B_Side::Shot(float elapsedTime)
{
    if (CanShot(elapsedTime) == false)return;



    ID3D11Device* p_device = SceneManager::Instance().GetDevice();

    DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z);
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(transform.r[2]);

    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, front);

    const DirectX::XMFLOAT3 z_direction = { 0.0f,0.0f,-1.0f * status.speed };

    DirectX::XMFLOAT3 direction;
    DirectX::XMStoreFloat3(&direction, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&z_direction), DirectX::XMVectorScale(front, 2.0f * status.speed)));

    const BaseProjectile::Parent parent_type = (type == Type::RED) ? BaseProjectile::Parent::REDENEMY : BaseProjectile::Parent::BLUEENEMY;

    ProjectileManager::Instance().RegisterProjectile(new Bullet(p_device,
        parent_type,
        t.Position + forward * 1.5f,
        direction,
        status.speed * 2.0f));

}

