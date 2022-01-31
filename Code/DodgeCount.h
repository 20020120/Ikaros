#pragma once


#include <memory>

#include "d3d11.h"
#include "Renderer.h"


// ����N���X

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

public: // Get�֐�
    bool CanDodge();

public: // Set�֐�
    void Reset();
    void Add();

private: // �萔�̒�` 

private: // �ϐ��̒�` 
    int* dodge_count;
    int DodgeCountMax; // �ő�����

    Renderer spr_dodge;
    Renderer spr_frame;
};