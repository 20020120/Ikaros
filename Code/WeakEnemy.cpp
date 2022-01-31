

#include "WeakEnemy.h"

#include "BaseProjectile.h"
#include "Bullet.h"
#include "Energy.h"
#include "MovePhase_Start_Side.h"
#include "MovePhase_Start_Top.h"
#include "MovePhase_Idle.h"
#include "MovePhase_Rotate.h"
#include "MovePhase_Rush.h"

#include "GameSystem.h"
#include "MovePhase_End_Side.h"
#include "MovePhase_End_Top.h"
#include "ProjectileManager.h"
#include "SceneManager.h"

//EnemyStraightFromBackToFront::EnemyStraightFromBackToFront(ID3D11Device* Device, const Type& type_) : Enemy(
//    Device,
//    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
//    EnemyPSFilePath::Lambert
//)
//{
//    // �����̐ݒ�
//    type = type_;
//
//    // �֐��|�C���^�̐ݒ�
//    MovePhase[0] = &EnemyStraightFromBackToFront::MoveToFrontPhase;
//    MovePhase[1] = &EnemyStraightFromBackToFront::MoveRotatePhase;
//    MovePhase[2] = &EnemyStraightFromBackToFront::MoveToBackPhase;
//    move_state = 0;
//
//    // ���x�̐ݒ�
//    status.speed = 26.0f;
//
//    // �����p���̐ݒ�
//    r.Angle = {};
//}
//
//void EnemyStraightFromBackToFront::Initialize()
//{
//    t.Position.z = -50.0f;
//}
//
//
//
//void EnemyStraightFromBackToFront::Move(float elapsedTime)
//{
//    (this->*MovePhase[move_state])(elapsedTime);
//
//    if(move_state > 1)
//        Shot(elapsedTime);
//}
//
//
//void EnemyStraightFromBackToFront::MoveToFrontPhase(float elapsedTime)
//{
//    // �O����(���f����z������)�ɐi�ޏ���
//    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z) };
//    DirectX::XMFLOAT4X4 rotate;
//    DirectX::XMStoreFloat4x4(&rotate, R);
//    DirectX::XMFLOAT3 forward = { rotate._31,rotate._32,rotate._33 };
//    DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forward));
//    DirectX::XMStoreFloat3(&forward, Forward);
//
//    // y��z�݈̂ʒu���X�V
//    t.Position.y += forward.y * status.speed * elapsedTime;
//    t.Position.z += forward.z * status.speed * elapsedTime;
//
//
//    // ���̃t�F�[�Y�ɍs������
//    if (t.Position.z < turn_z_axis_size) return;
//
//    move_state++;
//}
//
//
//void EnemyStraightFromBackToFront::MoveRotatePhase(float elapsedTime)
//{
//    constexpr float half_pi = DirectX::XMConvertToRadians(180);
//
//    r.Angle.y += Calcu3D::LerpFloat(0.0f, half_pi, 0.5f * elapsedTime);
//
//
//
//    constexpr float epsilon_radian = DirectX::XMConvertToRadians(5);
//
//    if (r.Angle.y < half_pi + epsilon_radian) return;
//
//    r.Angle.y = half_pi;
//    status.speed = 8.0f;
//    move_state++;
//}
//
//
//void EnemyStraightFromBackToFront::MoveToBackPhase(float elapsedTime)
//{
//    // �O����(���f����z������)�ɐi�ޏ���
//    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z) };
//    DirectX::XMFLOAT4X4 rotate;
//    DirectX::XMStoreFloat4x4(&rotate, R);
//    DirectX::XMFLOAT3 forward = { rotate._31,rotate._32,rotate._33 };
//    DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forward));
//    DirectX::XMStoreFloat3(&forward, Forward);
//
//    // y��z�݈̂ʒu���X�V
//    t.Position.y += forward.y * status.speed * elapsedTime;
//    t.Position.z += forward.z * status.speed * elapsedTime;
//}
//
//
//
//EnemyStraightToTarget::EnemyStraightToTarget(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_) : Enemy(
//    Device,
//    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
//    EnemyPSFilePath::Lambert
//)
//{
//    // �����̐ݒ�
//    type = type_;
//    // �ڕW�n�_�̐ݒ�(�|�C���^)
//    target = target_;
//
//    // �֐��|�C���^�̐ݒ�
//    MovePhase[0] = &EnemyStraightToTarget::MoveFromTopPhase;
//    MovePhase[1] = &EnemyStraightToTarget::MoveRotateToTargetPhase;
//    MovePhase[2] = &EnemyStraightToTarget::MoveToTargetPhase;
//    move_state = 0;
//
//    // �������x�̐ݒ�
//    status.speed = 7.0f;
//}
//
//
//void EnemyStraightToTarget::Initialize()
//{
//    t.Position.y = 30.0f;
//    t.Position.z = 30.0f;
//}
//
//
//void EnemyStraightToTarget::Move(float elapsedTime)
//{
//    (this->*MovePhase[move_state])(elapsedTime);
//
//    if (move_state > 0)
//        Shot(elapsedTime);
//}
//
//
//void EnemyStraightToTarget::MoveFromTopPhase(float elapsedTime)
//{
//    // �㕔���牺�Ɉړ�
//
//    constexpr DirectX::XMFLOAT3 direction = { 0.0f,-1.0f,0.0f };
//
//    // y�݈̂ʒu���X�V
//    t.Position.y += direction.y * status.speed * elapsedTime;
//
//    // ���܂ňړ������玟�̃t�F�[�Y��
//    if (t.Position.y > 20.0f) return;
//    t.Position.y = 20.0f;
//
//    move_state++;
//}
//
//
//void EnemyStraightToTarget::MoveRotateToTargetPhase(float elapsedTime)
//{
//    // �ڕW�܂ł̐��K�������x�N�g��
//    DirectX::XMVECTOR to_target_direction = DirectX::XMVector3Normalize(Calcu3D::MakeVectorAtoB(t.Position, *target));
//
//    // ���K���������ʕ����̃x�N�g��
//    DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z);
//    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(M.r[2]);
//    DirectX::XMVECTOR up = DirectX::XMVector3Normalize(M.r[1]);
//
//    // ���K�����ꂽ��̃x�N�g���̓��ς��t�R�T�C������ -> �p�x�����܂�
//    const float angle = std::acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, front)));
//
//    // ���ςō��E���� (�����ڂ͏㉺)
//    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(to_target_direction, up));
//    constexpr float ratio = 0.98f;
//    r.Angle.x += dot > 0.0f ? Calcu3D::LerpFloat(0.0f, -angle, ratio * elapsedTime) : Calcu3D::LerpFloat(0.0f, angle, ratio * elapsedTime);
//
//
//    // �����Ă���x�N�g���ƖڕW�܂ł̃x�N�g�����P�O�x�ȉ��Ȃ玟�̃t�F�[�Y��
//    constexpr float epsilon_radian = DirectX::XMConvertToRadians(1.0f);
//    if (angle > 0.0f + epsilon_radian) return;
//    move_state++;
//}
//
//
//void EnemyStraightToTarget::MoveToTargetPhase(float elapsedTime)
//{
//    // �O����(���f����z������)�ɐi�ޏ���
//    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(r.Angle.x, r.Angle.y, r.Angle.z) };
//    DirectX::XMFLOAT4X4 rotate;
//    DirectX::XMStoreFloat4x4(&rotate, R);
//    DirectX::XMFLOAT3 forward = { rotate._31,rotate._32,rotate._33 };
//    DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forward));
//    DirectX::XMStoreFloat3(&forward, Forward);
//
//    // y��z�݈̂ʒu���X�V
//    t.Position.y += forward.y * status.speed * elapsedTime;
//    t.Position.z += forward.z * status.speed * elapsedTime;
//
//
//    //if (t.Position.y > -26.0f && t.Position.y < 26.0f) return;
//    //
//    //move_state = 1;
//}




EnemyBack_Front_Straight_None::EnemyBack_Front_Straight_None(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
    EnemyPSFilePath::Lambert
)
{
    // �����̐ݒ�
    type = type_;

    // ���x�̐ݒ�
    status.speed = 8.0f;

    // �E�����ɂ���
    r.Angle.y = 0.0f;
}

void EnemyBack_Front_Straight_None::Initialize()
{
    t.Position.z = -60.0f;

    move_state = std::make_unique<MoveState>();
    BaseMove* move_front = new RushFront(status.speed, 1.0f);
    BaseMove* rotate_inverse_Y = new RotateInverseY;

    move_state->SetMove(move_front)
        .AddTransition(move_front, rotate_inverse_Y, [&](float) {return (t.Position.z >= turn_z_axis_size); })
        .SetFinalize(rotate_inverse_Y, [&]() {r.Angle.y = 3.14f; })
        .AddTransition(rotate_inverse_Y, move_front, [&](float) {return (r.Angle.y > 3.14f + 0.01f); });

}

void EnemyBack_Front_Straight_None::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);
}



Test_Enemy::Test_Enemy(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Eye : EnemyModelFilePath::Red_Eye,
    EnemyPSFilePath::Lambert
)
{
    // �����̐ݒ�
    type = type_;

    // ���x�̐ݒ�
    status.speed = 8.0f;

    // �E�����ɂ���
    //r.Angle.y = 0.0f;

    //r.Angle.y = DirectX::XMConvertToRadians(-180.0f);
}


void Test_Enemy::Initialize()
{
    //t.Position.y = -13.0f;
    t.Position.x = -13.0f;
    t.Position.y = -30.0f;
    t.Position.z = 25.0f;
    //t.Position.z = -60.0f;

    int randmm = 0;

    move_state = std::make_unique<MoveState>();
    BaseMove* start_side = new Start_FromYDown_Side(t.Position);
    BaseMove* idle = new Idle(3,&randmm,1);
    BaseMove* end_side = new End_ToDown_Side();
    //BaseMove* end_side = new End_ToRight_Top();

    //move_state->SetMove(start_side).AddTransition(start_side, idle, [&](float) {return (t.Position.z >= turn_z_axis_size); });
    //move_state->SetMove(start_side).AddTransition(start_side, idle, [&](float) {return (t.Position.x >= 0.0f); })
    //    .AddTransition(idle, end_side, [&](float) {return true; });
    move_state->SetMove(start_side)
        .AddTransition(start_side, idle, [&](float) {return (t.Position.x >= 0.0f); })
        .AddTransition(idle, end_side, [&](float) {return true; });


}


void Test_Enemy::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);
}






EnemyRotate_AroundShot_E::EnemyRotate_AroundShot_E(ID3D11Device* Device, const Type& type_) : Enemy(
    Device,
    type_ == Type::BLUE ? EnemyModelFilePath::Blue_Hitode : EnemyModelFilePath::Red_Hitode,
    EnemyPSFilePath::Lambert
)
{
    // �����̐ݒ�
    type = type_;

    // �ˌ����[�g
    status.shotInterval = 0.5f;

    // �̗͂̐ݒ�
    status.hitPoint = EnemyKindsHP::Hitode;

    // �X�P�[���̐ݒ�
    t.Scale = EnemyKindsScale::Hitode;

    // �o�����o����
    is_appear = true;

    // �����A�j���[�V����
    Model->f_PlayAnimation(EnemyAnim::Hitode::AnimationName::no_spin,true);
}

void EnemyRotate_AroundShot_E::Initialize()
{
    // �ˌ��񐔂����Z�b�g
    shot_count = 0;


    // �C�x���g�t���O��ǂݍ���
    Flags.Load("./resources/Models/Characters/Enemies/hitode.flg");


    // ���ԃt���O
    constexpr float seconds = 9.0f;
    timer.Initialize(nullptr, COUNT::DOWN, seconds);


    move_state = std::make_unique<MoveState>();
    BaseMove* idle = new RushFront(status.speed, 5.0f);


    // �J�����̌����̎擾
    using c_d = GameSystem::CAMERA_DIRECTION;
    const c_d& camera_direction = GameSystem::Instance().GetCameraDirection();


    BaseMove* start_back = nullptr;
    BaseMove* rotate = new Idle(1.0f,nullptr,1);
    BaseMove* end = nullptr;
    BaseMove* death = new Death();

    // ��]�����Â��鎞��
    constexpr float rotate_seconds = 5.41f;

    // �J�����̌����ɍ��킹�āA�ˌ����@�Əo���s���̐ݒ������
    if (camera_direction == c_d::SIDE)
    {
        SelectedShot = &EnemyRotate_AroundShot_E::AroundShot_Side;

        if(t.Position.y > 0.0f)
        {
            t.Position.x = -13.0f;
            t.Position.y += -55.0f;
            t.Position.z = 30.0f;

            start_back = new Start_FromYDown_RotateX_Side(t.Position);
            end = new End_ToDown_Side();
        }
        else
        {
            t.Position.x = -13.0f;
            t.Position.y += 55.0f;
            t.Position.z = 30.0f;

            start_back = new Start_FromYUp_RotateX_Side(t.Position);
            end = new End_ToUp_Side();
        }
        //rotate = new RotateX(rotate_seconds);
        //rotate = new RotateZ(rotate_seconds);


        // �J�ڏ����̗��R�ケ��if�����Ŏn�߂̍s���J�ڂ�ݒ肷��
        move_state->AddTransition(start_back, idle, [&](float) {return t.Position.x > -0.1f; })
            .SetInitialize(idle, [&]()
                {
                    t.Position.x = 0.0f;
                    timer.Reset();
                    r.Angle.x = 0.0f;
                    r.Angle.y = -3.14f;
                    is_appear = false;
                    Model->f_PlayAnimation(EnemyAnim::Hitode::AnimationName::spin_begin);
                });


    }

    else if (camera_direction == c_d::TOP)
    {
        SelectedShot = &EnemyRotate_AroundShot_E::AroundShot_Top;

        if(t.Position.x > 0.0f)
        {
            t.Position.x += -90.0f;
            t.Position.y = -13.0f;
            t.Position.z = 25.0f;

            start_back = new Start_FromLeft_RotateY_Top(t.Position);
            end = new End_ToLeft_Top();
        }
        else
        {
            t.Position.x += 90.0f;
            t.Position.y = -13.0f;
            t.Position.z = 25.0f;

            start_back = new Start_FromRight_RotateY_Top(t.Position);
            end = new End_ToRight_Top();
        }
        //rotate = new RotateY(rotate_seconds);
        //rotate = new RotateZ(rotate_seconds);


        // �J�ڏ����̗��R�ケ��if�����Ŏn�߂̍s���J�ڂ�ݒ肷��
        move_state->AddTransition(start_back, idle, [&](float) {return t.Position.y > -0.1f; })
            .SetInitialize(idle, [&]()
                {
                    t.Position.y = 0.0f;
                    timer.Reset();
                    r.Angle.x = 0.0f;
                    r.Angle.y = -3.14f;
                    is_appear = false;
                    Model->f_PlayAnimation(EnemyAnim::Hitode::AnimationName::spin_begin);
                });


    }


    // �J�ڏ��������if�����ł�������ɐݒ肵�Ă���
    move_state->AddTransition(idle, rotate, [&](float) {return timer.NowTime() <= 9.0f/*seconds*/ - 1.5f; })
                .SetInitialize(rotate, [&]()
                    {
                        timer.Reset();
                    }
                )
        .AddTransition(rotate, end, [&](float) {return (timer.NowTime() <= 9.0f/*seconds*/ - 5.41f/*rotate_seconds*/ - 0.8f); })
        .SetFinalize(rotate, [&]() {timer.Reset(); is_appear = true; })
        .AddTransition(end, death, [&](float) {return (timer.NowTime() <= 9.0f - 3.0f); })
        .SetInitialize(death, [&]() {Destroy(); });


    move_state->SetMove(start_back);
}

void EnemyRotate_AroundShot_E::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);

    timer.Update(elapsedTime);



    if (Flags.Check("stop", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(EnemyAnim::Hitode::AnimationName::no_spin,true);
    }
    
    if (timer.NowTime() <= 9.0f - 5.41f - 0.8f + 2.5f && Flags.Check("end", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(EnemyAnim::Hitode::AnimationName::spin_end);
    }

    if (Flags.Check("start", Model->GetCurrentAnimationNumber(), Model->GetCurrentAnimationFrame()))
    {
        Model->f_PlayAnimation(EnemyAnim::Hitode::AnimationName::spin_loop, true);
    }
}

void EnemyRotate_AroundShot_E::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    invTimer -= elapsedTime;


    Shot(elapsedTime);
}

void EnemyRotate_AroundShot_E::Shot(float elapsedTime)
{
    // �N�[���_�E��
    if (CanShot(elapsedTime) == false) return;


    if (timer.NowTime() <= 9.0f/*seconds*/ - 5.41f/*rotate_seconds*/ + 1.5f)return;
    if (move_state->GetNowMove() != STATE::sRotate) return;

    //�e����
    (this->*SelectedShot)(elapsedTime);
}

void EnemyRotate_AroundShot_E::EnergyRegister(ID3D11Device* p_device, DirectX::XMFLOAT3 direction)
{
    if (type == BaseObject::Type::BLUE)
    {
        ProjectileManager::Instance().RegisterProjectile(new Energy(p_device, BaseProjectile::Parent::BLUEENEMY,
            t.Position, direction));
    }
    else if (type == BaseObject::Type::RED)
    {
        ProjectileManager::Instance().RegisterProjectile(new Energy(p_device, BaseProjectile::Parent::REDENEMY,
            t.Position, direction));
    }
}

void EnemyRotate_AroundShot_E::AroundShot_Side(float elapsedTime)
{
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();

    DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();                  // �P�ʍs��
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(transform.r[2]);
    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(transform.r[0]);

    DirectX::XMFLOAT3 direction;
    DirectX::XMStoreFloat3(&direction, front);

    // �p�x72�x�̕ϐ�
    constexpr float radian = DirectX::XMConvertToRadians(360.0f / 5.0f);

    // ���炵�p�̊p�x15�x
    constexpr float shift_radian = DirectX::XMConvertToRadians(15.0f);

    for (short i = 0; i < 5; ++i)
    {
        // �O�����x�N�g����radian��x����]������
        DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(right, radian * static_cast<float>(i) + shift_radian * static_cast<float>(shot_count))));

        // �O�̎΂߂��傢�����
        EnergyRegister(p_device, direction);
    }


    // �ˌ��񐔂𑝉�
    shot_count++;

}

void EnemyRotate_AroundShot_E::AroundShot_Top(float elapsedTime)
{
    ID3D11Device* p_device = SceneManager::Instance().GetDevice();

    DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();                  // �P�ʍs��
    DirectX::XMVECTOR front = DirectX::XMVector3Normalize(transform.r[2]);
    DirectX::XMVECTOR up = DirectX::XMVector3Normalize(transform.r[1]);

    DirectX::XMFLOAT3 direction;
    DirectX::XMStoreFloat3(&direction, front);

    // �p�x72�x�̕ϐ�
    constexpr float radian = DirectX::XMConvertToRadians(360.0f / 5.0f);

    // ���炵�p�̊p�x15�x
    constexpr float shift_radian = DirectX::XMConvertToRadians(15.0f);


    for(short i = 0; i < 5; ++i)
    {
        // �O�����x�N�g����radian��y����]������
        DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Rotate(front, DirectX::XMQuaternionRotationAxis(up, radian * static_cast<float>(i) + shift_radian * static_cast<float>(shot_count))));

        // �O�̎΂߂��傢�����
        EnergyRegister(p_device, direction);
    }


    // �ˌ��񐔂𑝉�
    shot_count++;

}




EnemyMove_Round_TargetShot_Side::EnemyMove_Round_TargetShot_Side(ID3D11Device* Device, const Type& type_, const DirectX::XMFLOAT3* target_, bool hard_mode) : Enemy(
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
    status.shotInterval = 1.5f;

    // ���x�̐ݒ�
    status.speed = 25.0f;

    // �̗͂̐ݒ�
    status.hitPoint = EnemyKindsHP::Move_Shark;

    // �X�P�[���̐ݒ�
    t.Scale = EnemyKindsScale::Move_Shark;

    // �e�̑��x
    bullet_speed = (hard_mode) ? 35.0f : 15.0f;

    // �����A�j���[�V����
    Model->f_PlayAnimation(EnemyAnim::Shark::AnimationName::attack_loop, true);
}


void EnemyMove_Round_TargetShot_Side::Initialize()
{
    if (t.Position.y < 0.0f)
    {
        t.Position.y = -23.0f;
    }
    else
    {
        t.Position.y = 23.0f;
    }

    // �s���J�ڂ̐ݒ�
    move_state = std::make_unique<MoveState>();

    BaseMove* move_round = new RushRound(t.Position.y, status.speed);

    move_state->SetMove(move_round);
}

void EnemyMove_Round_TargetShot_Side::Move(float elapsedTime)
{
    move_state->Update(elapsedTime, t.Position, r.Angle);
}

void EnemyMove_Round_TargetShot_Side::Behavior(float elapsedTime)
{
    Move(elapsedTime);

    //���G���ԍX�V
    invTimer -= elapsedTime;

    Shot(elapsedTime);
}

void EnemyMove_Round_TargetShot_Side::Shot(float elapsedTime)
{
    if (CanShot(elapsedTime) == false)return;


    if (type == Type::NONE) return;

    // �e�̐i�s����
    DirectX::XMVECTOR Direction = Calcu3D::MakeVectorAtoB(t.Position, *target, true);
    DirectX::XMFLOAT3 direction;
    DirectX::XMStoreFloat3(&direction, Direction);


    // �e�̐���
    const BaseProjectile::Parent parent_type = (type == Type::RED) ? BaseProjectile::Parent::REDENEMY : BaseProjectile::Parent::BLUEENEMY;

    BaseProjectile* bullet = new Bullet(SceneManager::Instance().GetDevice(),
        parent_type,
        t.Position,
        direction,
        bullet_speed);

    ProjectileManager::Instance().RegisterProjectile(bullet);
}
