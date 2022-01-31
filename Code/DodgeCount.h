#pragma once


#include <memory>

#include "d3d11.h"
#include "Renderer.h"


// 回避クラス

class DodgeCount
{
public:
    DodgeCount() = default;
    ~DodgeCount() = default;

public:
    void Initialize(ID3D11Device* Device, int* dodge);
    void Update(float ElapsedTime);
    void Render(ID3D11DeviceContext* Immediate_Context);
    void Finalize();

public: // Get関数
    bool CanDodge();

public: // Set関数
    void Reset();
    void Add();

private: // 定数の定義 

private: // 変数の定義 
    int* dodge_count;
    int DodgeCountMax; // 最大回避回数

    Renderer spr_dodge;
    Renderer spr_frame;
};