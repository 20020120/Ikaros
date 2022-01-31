#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

// �ǉ�
class Sprite_Batch;
class Camera;
#include "HealthPoint.h"
#include "DodgeCount.h"
#include "EnergyGauge.h"
#include "Score.h"
#include "Timer.h"
#include "Combo.h"
#include"Renderer.h"
//#include "Player.h"

// player���瑗���Ă���f�[�^�p�\����
struct GameSystemData
{
    int* HP;
    int* Energy;
    int* Dodge;
    DirectX::XMFLOAT3* position;
};

// �Q�[���V�X�e�� ���V���O���g���p�^�[��

class GameSystem
{
private:
    GameSystem() {};
    ~GameSystem() {};

public:
    static GameSystem& Instance()
    {
        static GameSystem instance;
        return instance;
    }

public:
    void Initialize(ID3D11Device* Device, GameSystemData* Data);
    void Update(float ElapsedTime);
    void Render(ID3D11DeviceContext* Immediate_Context);
    void PerformRender(ID3D11DeviceContext* Immediate_Context);// ���o���̘g�`��
    void Finalize();

    void f_Gui();
    void SetFrameDraw(bool a) { FrameDraw = a; }

    void SetIsChangeColor(bool frag) const { Energy_Gauge->SetChangeColor(frag); }

public://�萔(Getter,Setter�̊֌W��)
    enum class CAMERA_DIRECTION
    {
        SIDE,//�i�T�C�h�r���[�j
        TOP,//(�g�b�v�r���[�j
    };


public: // Get�֐�
    bool CanFinisher();     // �K�E�Z���ł��邩
    bool CanDodge();        // ����ł��邩
    int GetHealthPoint();   // �̗͂̎擾
    [[nodiscard]] CAMERA_DIRECTION GetCameraDirection() const { return cameraDir; }
    DirectX::XMFLOAT3* GetPlayerPos() { return PlayerDatas->position; }
    const auto& GetNowCombo() { return ComboManager->GetCombo(); }
    const auto& GetMaxCombo() { return ComboManager->GetMaxCombo(); }
    const auto& GetNowKilledSize() { return killed_size; }
    const auto& GetClearTimer() { return Time->NowTime(); }

public: // Set�֐�
    void Reset();           // �l�̏������̂�
    void SetCamera(Camera* camera) { pCamera = camera; }
    void SetCameraDirection(CAMERA_DIRECTION dir) { cameraDir = dir; }//�J�����̌���
    void TimerStop() { Time->Stop(); }
    void SetRetry() { retry = true; }

    void ChangeView();
    const auto& GetChangingView() { return ChangingView; }
    void SetChangingView(bool s) { ChangingView = s; }
    void RetryBoss(const float& add_time) { Time->Add(add_time); }
    void KilledEnemy()
    {
        killed_size++;
        ComboManager->AddCombo();
    }

private:
    void RenderHitCircle(ID3D11DeviceContext* Immediate_Context,
        const DirectX::XMFLOAT4X4& view,
        const DirectX::XMFLOAT4X4& projection);

    // ���g���C����p�֐�
    void InitializeInSceneGame(ID3D11Device* Device, GameSystemData* Data);

public:
    //�Q�[���p�b�h�̍X�V
    void GamePadUpdate() const{};


private: // �萔�̒�` 

private: // �ϐ��̒�`
    bool defined;
    std::unique_ptr<HealthPoint> HP;
    std::unique_ptr<DodgeCount> DodgeCounter;
    std::unique_ptr<EnergyGauge> Energy_Gauge;
    std::unique_ptr<Score> ScoreCounters[2];
    std::unique_ptr<Timer> Time;
    std::unique_ptr<Combo> ComboManager;
    std::unique_ptr<Sprite_Batch> spr_HitArea;

    Renderer spr_PerformanceFrame;

    // ���j�� (SceneGame���Ƀ��Z�b�g)
    int killed_size = 0;

    GameSystemData* PlayerDatas;

    Renderer spr_CutIn0{}; // �J�b�g�C���̉摜
    Renderer spr_CutIn1{}; // �J�b�g�C���̉摜
    int CutInState = {};

    Camera* pCamera;

    bool FrameDraw = false;
    //���_
	CAMERA_DIRECTION cameraDir{ CAMERA_DIRECTION::SIDE };
    bool ChangingView{};

    // Initialize�̍Ō��false�ɂ���
    bool retry = false;
};