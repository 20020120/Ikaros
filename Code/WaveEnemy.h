#pragma once

#include "Enemy.h"
#include "BaseMovePhase.h"



class EnemyWave_None final : public Enemy
{
public:
    EnemyWave_None(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;

private:
    std::unique_ptr<MoveState> move_state;
};


class EnemyWave_Shot_B final : public Enemy
{
public:
    EnemyWave_Shot_B(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    std::unique_ptr<MoveState> move_state;
};


class EnemyWave_Shot_E final : public Enemy
{
public:
    EnemyWave_Shot_E(ID3D11Device* Device, const Type& type_);
    void Initialize() override;
    void Move(float elapsedTime) override;
    void Behavior(float elapsedTime) override;

private:
    std::unique_ptr<MoveState> move_state;
};