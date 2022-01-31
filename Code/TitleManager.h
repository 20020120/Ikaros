#pragma once
#include <d3d11.h>
#include <memory>

#include "TitleControl.h"

#include "SpriteBatch.h"
#include "Font.h"
#include "Light.h"
#include "ModelComponent.h"
#include "TitleCamera.h"
#include "Option.h"
#include "Scene.h"
#include "gAudioSource.h"

class TitleManager final
{
public:
    TitleManager() = default;
    ~TitleManager() = default;
public:
    void Initialize(ID3D11Device* device);
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);
    void Finalize();
    void GuiMenu();

private:
    void Input(float elapsedTime);
    void ChangeScene();
    void RenderFonts(ID3D11DeviceContext* dc);
    void SkipMovie(float elapsedTime);
    void SpeedRatio(float elapsedTime, const int& index);

private:
    using pFunc = void(TitleManager::*)(float);

private: // �֐��|�C���^�@�^�C�g���̑I�����̊֐�
    pFunc states[6] =
    {
        &TitleManager::sTitle,
        &TitleManager::sStart,
        &TitleManager::sStageSelect,
        &TitleManager::sOption,
        &TitleManager::sEnd,
        &TitleManager::sNone,
    };

    void sTitle(float elapsedTime);
    void sOption(float elapsedTime);
    void sStart(float elapsedTime);
    void sStageSelect(float elapsedTime);
    void sEnd(float elapsedTime);
    void sNone(float elapsedTime);

private:
    static constexpr float lerp_timer_max = 0.5f;
    static constexpr float lerp_timer_min = 0.0f;

private:
    // �^�C�g���p���͏���
    std::unique_ptr<TitleControl> input;
    // �^�C�g���p�J��������
    std::unique_ptr<TitleCamera> camera;

    // �I�v�V����
    std::unique_ptr<Option> option;
    bool openOption{ false };//�I�v�V�������J���Ă��邩�ǂ���
    bool closeOption{ false };//�I�v�V��������Ă��邩�ǂ���

    // �X�v���C�g
    std::unique_ptr<Sprite_Batch> SprTitle;
    std::unique_ptr<Font> font;
    std::unique_ptr<Sprite_Batch> SprSelect[4];

    // ���f���֌W
    std::unique_ptr<DirectionLight> directionLight{ nullptr };
    struct Model final
    {
        std::unique_ptr<ModelComponent> model;
        Transform transform;
        Rotate rotate;
        float speed_ratio;
    } mdl_player, mdl_garage, mdl_launch_pad,mdl_SkyDome;


    // �X�L�b�v�p�̃^�C�}�[
    Timer skip_timer;

    // ��ԗp�̃^�C�}�[
    float lerp_timer = 0.0f;

    // �V�[���؂�ւ��̃t���O
    SceneParent* next_scene = nullptr;

    //sound
    std::unique_ptr<AudioSource> bgm{ nullptr };
    std::unique_ptr<AudioSource> bgm_sand_storm{ nullptr };
    std::unique_ptr<AudioSource> se_decision;
    std::unique_ptr<AudioSource> se_lunch;
    std::unique_ptr<AudioSource> se_take_off;

    std::unique_ptr<AudioSource> se_doom;
    std::unique_ptr<AudioSource> se_sonic_boom;


    enum STATE
    {
        TITLE,          // �^�C�g����ʒ�
        START,          // �X�^�[�g��ʒ�
        STAGE_SELECT,   // �X�e�[�W�I����ʒ�
        OPTION,         // �I�v�V������ʒ�
        END,            // �Q�[���I��
        NONE,           // ���[�r�[������
    };

    STATE current_state_index;

    bool imgui_now_titled;

    // ���o�p���C�g
    // �|�C���g���C�g
    std::unique_ptr<PointLight> PointLights{}; 

    // �X�|�b�g���C�g
    std::unique_ptr<SpotLight> mSpotLight;
    float StackLightManagerTimer{};
    int LightNum = 0;

    DirectX::XMFLOAT2 LogoPos{};

    std::unique_ptr<Effect> efk_smoke;
    Effekseer::Handle hdl_smoke = -1;
    std::unique_ptr<Effect> efk_sparkle;
    Effekseer::Handle hdl_sparkle = -1;

    // �X�^�[�g���̉��o
    int StartPerformanceState{}; // �J�ڂ��Ǘ�����
    float StartPerformanceTime{}; // ���ԊǗ�

    void CameraShake(float elapsedTime,
                     bool IsShake                  /*�J������U�邩*/,
                     DirectX::XMFLOAT3 SwingWidth, /*�U�蕝*/
                     float OneTermSeconds,         /*���U��b��*/
                     int ShakeTimes                /*����U�邩*/
                     );

    int CameraShakeState{};
};
