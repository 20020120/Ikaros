#pragma once
#include "ShipMoveState.h"

class ToHead_RightSide final : public BaseShipMove
{
public:
    void Update(float, DirectX::XMFLOAT3&, DirectX::XMFLOAT3&, float) override;
};


class ToHead_LeftSide final : public BaseShipMove
{
public:
    void Update(float, DirectX::XMFLOAT3&, DirectX::XMFLOAT3&, float) override;
};


class ToBack_RightSide final : public BaseShipMove
{
public:
    void Update(float, DirectX::XMFLOAT3&, DirectX::XMFLOAT3&, float) override;
};


class ToBack_LeftSide final : public BaseShipMove
{
public:
    void Update(float, DirectX::XMFLOAT3&, DirectX::XMFLOAT3&, float) override;
};


class ToRightSide final : public BaseShipMove
{
public:
    void Update(float, DirectX::XMFLOAT3&, DirectX::XMFLOAT3&, float) override;
};


class Rotating final : public BaseShipMove
{
public:
    void Update(float, DirectX::XMFLOAT3&, DirectX::XMFLOAT3&, float) override;
};