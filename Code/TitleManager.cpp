
#include "TitleManager.h"

#include "EnemyManager.h"
#include "imgui.h"
#include "SceneGame.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "SceneOver.h"
#include "SceneTutorial.h"
#include "gAudio.h"
#include"SceneRanking.h"
#include "EffectManager.h"

void TitleManager::Initialize(ID3D11Device* device)
{
    // �X�v���C�g
    //SprTitle = new Sprite_Batch(device, L"./resources/Speite/Title/NoTitle.png", 1);
    SprTitle        = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Title/title_logo.png", 1);
    font            = std::make_unique<Font>(device, "./resources/Sprite/Font/Font.fnt", 2048);

    //wstring FontFilePath = L"./resources/Sprite/Font/TitleFont/";
    SprSelect[0]    = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Font/TitleFont/Start.png"       , 1);
    SprSelect[1]    = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Font/TitleFont/Option.png"      , 1);
    SprSelect[2]    = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Font/TitleFont/Ranking.png"     , 1);
    SprSelect[3]    = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Font/TitleFont/Quit.png"        , 1);
    SprSelect[4]    = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Font/TitleFont/Tutorial.png"    , 1);
    SprSelect[5]    = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Font/TitleFont/StageSelect.png" , 1);
    SprSelect[6]    = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Font/TitleFont/Back.png"        , 1);
    SprSelect[7]    = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Font/TitleFont/Stage1.png"      , 1);
    SprSelect[8]    = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Font/TitleFont/Stage2.png"      , 1);
    SprSelect[9]    = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Font/TitleFont/Stage3.png"      , 1);
    SprSelect[10]   = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Font/TitleFont/Extra1.png"      , 1);
    SprSelect[11]   = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Font/TitleFont/Extra2.png"      , 1);
    SprSelect[12]   = std::make_unique<Sprite_Batch>(device, L"./resources/Sprite/Font/TitleFont/Back.png"        , 1);

    // �v���C���[���f��
    ModelRenderInitializer Ini;
    Ini.SetALLPath(
        "./resources/Models/Characters/Players/PlayerModel.nk",
        "./resources/Shaders/SkinnedMesh_VS.cso",
        "./resources/Shaders/TitlePlayerPBR_PS.cso",
        "./resources/Shaders/GeometryShader.cso"
    );
    mdl_player.model = std::make_unique<ModelComponent>(device, Ini);
    mdl_player.model->f_PlayAnimation(5, true);
    mdl_player.model->Update(0.1f);
    mdl_player.transform = {/* �ʒu */{}, /* �X�P�[�� */{ 0.03f, 0.03f, 0.03f } };
    mdl_player.rotate = { { 0.0f,-3.14f,0.0f } };

    // �ԌɃ��f��
    Ini.SetModelPath("./resources/Models/Title/Garage.nk");
    Ini.SetPsPath("./resources/Shaders/TitleGarage_Ps.cso");
    mdl_garage.model = std::make_unique<ModelComponent>(device, Ini);
    mdl_garage.transform = { {0.0f,-2.0f,-10.0f},{0.3f,0.3f,0.3f} };
    mdl_garage.rotate = { {0.0f,1.57f,0.0f} };
    //
    //
    Ini.SetPsPath("./resources/Shaders/LambertPS.cso");
    // ���ˑ䃂�f��
    Ini.SetModelPath("./resources/Models/Title/LaunchBase.nk");
    mdl_launch_pad.model = std::make_unique<ModelComponent>(device, Ini);
    mdl_launch_pad.transform = { {0.0f,-2.0f,0.0f},{0.3f,0.3f,0.3f} };
    mdl_launch_pad.rotate = { {0.0f,1.57f,0.0f} };

    Ini.SetModelPath("./resources/Models/Stages/SkyDome.nk");
    mdl_SkyDome.model = std::make_unique<ModelComponent>(device, Ini);
    mdl_SkyDome.transform = { { 0.0f,0.0f,0.0f},{0.3f,0.3f,0.3f} };
    mdl_SkyDome.rotate = {{}};
    // �X�L�b�v�^�C�}�[ ( 2�b�@)
    skip_timer.Initialize(nullptr, COUNT::DOWN, 0.1f);

    // ��ԗp�̃^�C�}�[ ( 0.5�b )
    lerp_timer = lerp_timer_max;

    // "�^�C�g���p"�I�v�V����
    option = std::make_unique<Option>(device, Option::MODE::TITLE);

    // �f�B���N�V�������C�g��������
    directionLight = std::make_unique<DirectionLight>(device);

    // �|�C���g���C�g��������
    PointLights = std::make_unique<PointLight>(device, 3);

    for (int i = 0; i < 10;i+=2)
    {

        PointLights->SetPosition({ 6.0f,-1.5f,-25.0f * i/2 }, i);
        PointLights->SetColor({ 0.5f,0.0f,0.0f }, i);

        PointLights->SetPosition({ -6.0f,-1.5f,-25.0f *i/2 }, i+1);
        PointLights->SetColor({ 0.5f,0.0f,0.0f }, i+1);
    }

    mSpotLight = std::make_unique<SpotLight>(device, 4);

    // �J����
    camera = std::make_unique<TitleCamera>(&mdl_player.transform.Position);
    camera->Initialize(device);

    // ���͏���
    input = std::make_unique<TitleControl>();
    input->Initialize();
    input->SetStateIndexMax(static_cast<short>(3));

    // ���݂̕`��X�e�[�g (Title, Start, Option, End, None)
    current_state_index = STATE::TITLE;

    // imgui�\���p
    imgui_now_titled = true;

    //Sound
    bgm = (Audio::Instance().LoadAudioSource("./resources/Sounds/BGM/SceneTitle.wav"));
    bgm->Play(true, OptionSystem::Instance().GetBgmVolume() * 0.5f);
    bgm_sand_storm = (Audio::Instance().LoadAudioSource("./resources/Sounds/BGM/Sandstorm.wav"));

    se_decision = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/System/StartSE.wav");
    se_lunch    = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Title/Lunch.wav");
    se_take_off = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/Title/catastoroph.wav");
    se_doom         = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/BossBomb.wav");
    se_sonic_boom   = Audio::Instance().LoadAudioSource("./resources/Sounds/SE/BossBomb2.wav");


    //--------------------<���ˉ��o>--------------------//
    StartPerformanceState = 0;

    efk_smoke   = std::make_unique<Effect>("./resources/Effects/smoke.efk");
    efk_sparkle = std::make_unique<Effect>("./resources/Effects/sparkle.efk");
}


void TitleManager::Update(float elapsedTime)
{
    // �^�C�g��
    //
    /// �X�^�[�g
    ///     { �`���[�g���A�� }
    ///     { �X�e�[�W�I�� }
    ///     { �߂� }
    // �I�v�V����
    // �I���
    //


    Input(elapsedTime);


    option->GuiMenu();
    option->Update(elapsedTime);

    bgm->SetVolume(OptionSystem::Instance().GetBgmVolume() * 0.5f);

    // ���݂̉�ʏ�Ԃɉ������X�V����
    (this->*states[current_state_index])(elapsedTime);

    camera->Update(elapsedTime);

    EffectManager::Instance().Update(elapsedTime);

    ChangeScene();


#if _DEBUG

    if (GetAsyncKeyState('Z') & 0x8000)
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame()));
        return;
    }
    if (GetAsyncKeyState('X') & 0x8000)
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTutorial()));
        return;
    }
    if (GetAsyncKeyState('C') & 0x8000)
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneOver()));
        return;
    }
    if (GetAsyncKeyState('V') & 0x8000)
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneRanking()));
        return;
    }


#endif
}


void TitleManager::Render(ID3D11DeviceContext* dc)
{
    camera->ConstantBufferUpdate(dc);

    directionLight->Update(dc);

    PointLights->Update(dc);

    mSpotLight->Update(dc);

    mdl_player.model->Render(dc, mdl_player.transform, mdl_player.rotate);
    mdl_garage.model->Render(dc, mdl_garage.transform, mdl_garage.rotate);
    mdl_launch_pad.model->Render(dc, mdl_launch_pad.transform, mdl_launch_pad.rotate);
    mdl_SkyDome.model->Render(dc, mdl_SkyDome.transform, mdl_SkyDome.rotate);



    EffectManager::Instance().Render(camera->GetView(), camera->GetProjection());

    if (current_state_index == STATE::NONE) return;

    // �摜�`��
    SprTitle->Begin(dc);
    SprTitle->Render(dc, LogoPos, { 1.0f,1.0f },{0.0f,0.0f},{512.0f,320.0f},{256.0f,160.0f});
    SprTitle->End(dc);
    //
    ////font�`��
    RenderFonts(dc);

    // �I�v�V�����\��
    option->Render(dc);
}


void TitleManager::Finalize()
{
    bgm->Release();
    bgm_sand_storm->Release();
    se_decision->Release();
    se_lunch->Release();
    se_take_off->Release();

    se_doom->Release();
    se_sonic_boom->Release();

    input->Finalize();

    option->ReleaseSounds();

    EffectManager::Instance().GetEffekseerManager()->StopAllEffects();
}



void TitleManager::Input(float elapsedTime)
{
    if (current_state_index == STATE::OPTION) return;
    
    input->InputUpdate(elapsedTime);
}


void TitleManager::ChangeScene()
{
    // �V�[���؂�ւ�
    if (next_scene == nullptr) return;

    bgm->Stop();
    SceneManager::Instance().ChangeScene(next_scene);
}


void TitleManager::RenderFonts(ID3D11DeviceContext* dc)
{
    if (current_state_index == STATE::END) return;
    if (current_state_index == STATE::OPTION) return;

    //font->Begin(dc);


    // �^�C�g����
    constexpr wchar_t* title_text = L"�^�C�g����";
    size_t text_length = wcslen(title_text);
   // font->Draw(400, 400, title_text, text_length);


    // �I����
    constexpr wchar_t* button[] =
    {
        L"�X�^�[�g",        // [0]
        L"�I�v�V����",      // [1]
        L"�����L���O",      // [2]
        L"�����",          // [3]

        L"�`���[�g���A��",
        L"�X�e�[�W�I��",
        L"���ǂ�",

        L"�X�e�[�W�P",
        L"�X�e�[�W�Q",
        L"�X�e�[�W�R",
        L"�X�e�[�W�S",
        L"�X�e�[�W�T",
        //L"�G�N�X�g���P",
        //L"�G�N�X�g���Q",
        //L"�G�N�X�g���R",
        L"���ǂ�",
    };


    float alpha[13] = { 0.5f,0.5f,0.5f,0.5f,0.5f, 0.5f,0.5f,0.5f,0.5f,0.5f, 0.5f,0.5f,0.5f/*,0.5f,0.5f*//*,0.5f*/ };

    constexpr DirectX::XMFLOAT2 pos = { 810.0f,400.0f };
    constexpr DirectX::XMFLOAT2 scale = { 0.9f,0.9f };
    //constexpr float height = 40.0f;

    const float tex_size_x  = SprSelect[0]->GetTextureWidth(); 
    const float tex_size_y  = SprSelect[0]->GetTextureHeight() * 0.167f; // 0.167f = 1 / 6
    const float height      = tex_size_y - 5.0f;

    // �^�C�g����ʂ̃t�H���g
    if(current_state_index == STATE::TITLE)
    {
        alpha[input->GetStateIndex()] = 1.0f;

        
        for(int i = 0; i < 4;++i)
        {
            SprSelect[i]->Begin(dc);
            SprSelect[i]->Render(dc,
                pos.x, pos.y + i * height,
                scale.x, scale.y,
                0.0f,0.0f,
                tex_size_x, tex_size_y,
                0.0f,0.0f,
                0.0f,
                1.0f,1.0f,1.0f,alpha[i]);
            SprSelect[i]->End(dc);

            //text_length = wcslen(button[i]);
            //font->Draw({ pos.x, pos.y + i * height },
            //    { 1.0f,1.0f },
            //    {1.0f,1.0f,1.0f,alpha[i]},
            //    button[i],
            //    text_length);
        }

    }

#if 0
    // �X�^�[�g��ʂ̃t�H���g
    if (current_state_index == STATE::START)
    {
        alpha[input->GetStateIndex() + 3] = 1.0f;

        for (int i = 0; i < 3; ++i)
        {
            text_length = wcslen(button[i + 3]);
            font->Draw({ pos.x, pos.y + i * height },
                { 1.0f,1.0f },
                { 1.0f,1.0f,1.0f,alpha[i + 3] },
                button[i + 3],
                text_length);
        }
    }

    // �X�e�[�W�I����ʂ̃t�H���g
    if (current_state_index == STATE::STAGE_SELECT)
    {
        constexpr DirectX::XMFLOAT2 target_translate = { -210.0f,-height + 10.0f };
        
        for (int i = 0; i < 3; ++i)
        {
            text_length = wcslen(button[i + 3]);
            font->Draw({ pos.x + target_translate.x, pos.y + i * height * 0.75f + target_translate.y  },
                { 0.75f,0.75f },
                { 1.0f,1.0f,1.0f,(i != 1) ? 0.25f : 0.5f },
                button[i + 3],
                text_length);
        }


        alpha[input->GetStateIndex() + 6] = 1.0f;

        for (int i = 0; i < 3; ++i)
        {
            text_length = wcslen(button[i + 6]);
            font->Draw({ pos.x, pos.y + i * height },
                { 1.0f,1.0f },
                { 1.0f,1.0f,1.0f,alpha[i + 6] },
                button[i + 6],
                text_length);
        }
    }
#endif

    // �X�^�[�g��ʂƃX�e�[�W�I����ʂ̃t�H���g
    if (current_state_index == STATE::START || current_state_index == STATE::STAGE_SELECT)
    {

        // �X�^�[�g��ʗp�� alpha�l�̐ݒ�
        if (current_state_index == STATE::START)
        {
            alpha[input->GetStateIndex() + 4] = 1.0f;

            for (int i = 0; i < 6; ++i)
            {
                alpha[i + 7] = 0.5f;
            }

        }

        // �X�e�[�W�I����ʗp�� alpha�l�̐ݒ�
        if (current_state_index == STATE::STAGE_SELECT)
        {
            alpha[input->GetStateIndex() + 7] = 1.0f;
            alpha[1 + 4] = 1.0f;
        }

        // �\���ʒu�̕�ԗp�ϐ�
        const float& time = lerp_timer;
        const float ratio = time / lerp_timer_max;
        const float inverse_ratio = 1.0f - ratio;


        // �`��
        for (int i = 0; i < 6; ++i)
        {
            constexpr DirectX::XMFLOAT2 target_translate = { 510.0f,0.0f };

            // �X�^�[�g���
            if (i < 3)
            {
                //text_length = wcslen(button[i + 4]);
                //font->Draw({ pos.x + -target_translate.x * inverse_ratio, pos.y + i * height + -target_translate.y * inverse_ratio },
                //    { 1.0f,1.0f },
                //    { 1.0f,1.0f,1.0f,alpha[i + 4] },
                //    button[i + 4],
                //    text_length);

                const int j = i + 4;

                SprSelect[j]->Begin(dc);
                SprSelect[j]->Render(dc,
                    pos.x + -target_translate.x * inverse_ratio, pos.y + i * height + -target_translate.y * inverse_ratio,
                    scale.x, scale.y,
                    0.0f, 0.0f,
                    tex_size_x, tex_size_y,
                    0.0f, 0.0f,
                    0.0f,
                    1.0f, 1.0f, 1.0f, alpha[j]);
                SprSelect[j]->End(dc);
            }
            
            constexpr float pudding = -140.0f;

            // �X�e�[�W�I�����
            //text_length = wcslen(button[i + 7]);
            //font->Draw({ pos.x + target_translate.x * ratio, pos.y + i * height + target_translate.y * ratio + pudding },
            //    { 1.0f,1.0f },
            //    { 1.0f,1.0f,1.0f,alpha[i + 7] },
            //    button[i + 7],
            //    text_length);

            const int k = i + 7;

            SprSelect[k]->Begin(dc);
            SprSelect[k]->Render(dc,
                pos.x + target_translate.x * ratio + 75.0f, pos.y + i * height + target_translate.y * ratio + pudding,
                scale.x, scale.y,
                0.0f, 0.0f,
                tex_size_x, tex_size_y,
                0.0f, 0.0f,
                0.0f,
                1.0f, 1.0f, 1.0f, alpha[k]);
            SprSelect[k]->End(dc);

        }
    }

    //font->End(dc);

    
}


void TitleManager::SkipMovie(float elapsedTime)
{
    if (input->PushEnterNow() == false) return;


#if 0

    // �����ꂩ�̌���L�[�������ꑱ���Ă���ԁA
    // �X�L�b�v�^�C�}�[���A�b�v�f�[�g����
    skip_timer.Update(elapsedTime);


    // �^�C�}�[��0.0f�ȉ��ɂȂ�����
    if (skip_timer.NowTime() > 0.0f) return;

#endif


    // �V�[���؂�ւ��̃t���O�𗧂Ă�
    if (next_scene) return;

    switch (input->GetStateIndex())
    {
    case 0:
        // �V�[���J�� "�Q�[��"
        next_scene = new SceneLoading(new SceneGame());
        break;

    case 1:
        // �V�[���J�� "�`���[�g���A��"
        next_scene = new SceneLoading(new SceneTutorial());
        break;

    }
}


void TitleManager::SpeedRatio(float elapsedTime, const int& index)
{
    if (index <= 2)
    {
        // ���x�̌���
        mdl_player.speed_ratio = Calcu3D::LerpFloat(mdl_player.speed_ratio, 5.0f, 1.0f * elapsedTime);
    }

    else if (index == 3)
    {

        // ���x�̌���
        if(mdl_player.speed_ratio < 50.0f)

        {
            mdl_player.speed_ratio = Calcu3D::LerpFloat(mdl_player.speed_ratio, 100.0f, 1.0f * elapsedTime);
        }
        else
        {
            mdl_player.speed_ratio = Calcu3D::LerpFloat(mdl_player.speed_ratio, 150.0f, 3.0f * elapsedTime);
        }

        // ���f������Ɍ�����
        //mdl_player.rotate.Angle.x = Calcu3D::LerpFloat(mdl_player.rotate.Angle.x, -0.12f, 1.0f * elapsedTime);

    }
    else if (index == 4)
    {
        // ���x�̌���
        mdl_player.speed_ratio = Calcu3D::LerpFloat(mdl_player.speed_ratio, 200.0f, 2.0f * elapsedTime);

        // ���f������Ɍ�����
        mdl_player.rotate.Angle.x = Calcu3D::LerpFloat(mdl_player.rotate.Angle.x, -0.42f, 1.0f * elapsedTime);
    }
}



void TitleManager::GuiMenu()
{
#ifdef _DEBUG

    ImGui::Begin("SceneTitle");

    ImGui::Text("skip_timer %f", skip_timer.NowTime());

    ImGui::Text("camera phase %d", camera->GetPhaseIndex());

    ImGui::Text("now_state %d", current_state_index);
    ImGui::Text("cursor_index %d", input->GetStateIndex());

    const short index = input->GetStateIndex();

    if(imgui_now_titled)
    {
        ImGui::Text("Title");
        if(index == 0)
        {
            ImGui::Text("SELECT start");
        }

        else if (index == 1)
        {
            ImGui::Text("SELECT option");
        }

        else if (index == 2)
        {
            ImGui::Text("SELECT end");
        }
    }

    else
    {
        ImGui::Text("Start");
        if (index == 0)
        {
            ImGui::Text("SELECT game");
        }

        else if (index == 1)
        {
            ImGui::Text("SELECT tutorial");
        }

        else if (index == 2)
        {
            ImGui::Text("SELECT title");
        }
    }



    if (ImGui::Button("GameOver"))
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneOver()));
        return;
    }
    if (ImGui::Button("Game"))
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame()));
        return;
    }

    if (ImGui::Button("Tutorial"))
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTutorial()));
        return;
    }

    ImGui::DragFloat2("LogoPos", &LogoPos.x);

    if (ImGui::Button("Ranking"))
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneRanking()));
        return;
    }

    ImGui::End();

#endif
}


void TitleManager::sTitle(float elapsedTime)
{
    for(int i=0;i<10;i++)
    {
        PointLights->SetRange(0.0f, i);
    }
    // ����L�[�������ꂽ��
    if (input->PushEnterNow() == false) return;

    // ���݂�STATE����A�I������Ă��鍀�ڂ�STATE�ɕς���
    const short index = input->GetStateIndex();

    input->IndexToZero();

    if(index == 0)
    {
        current_state_index = STATE::START;
        camera->SetNextCameraPos(TitleCamera::back_pos);
        StackLightManagerTimer = 0.0f;
        LightNum = 0;
        input->SetStateIndexMax(2);

        imgui_now_titled = false;
    }

    if (index == 1)
    {
        current_state_index = STATE::OPTION;

        imgui_now_titled = false;
    }

    if (index == 2)
    {
        // �V�[���J�� "�����L���O"
        next_scene = new SceneLoading(new SceneRanking());
    }

    if (index == 3)
    {
        current_state_index = STATE::END;
    }
    

    if(current_state_index != STATE::OPTION)    // �I�v�V������ʈȊO�Ȃ�
    {
        se_decision->Stop();
        se_decision->Play(false, OptionSystem::Instance().GetSeVolume() * 0.5f);
    }
}


void TitleManager::sStart(float elapsedTime)
{
    // ��ԃ^�C�}�[�̍X�V
    lerp_timer += 2.0f * elapsedTime;
    if (lerp_timer > lerp_timer_max) lerp_timer = lerp_timer_max;


    // ���Ԃɓ_��
    static bool check = false;
    StackLightManagerTimer += elapsedTime;
    if (LightNum < 10)
    {
        if (static_cast<int>(StackLightManagerTimer * 10.0f) % 3 == 0&&!check)
        {
            PointLights->SetRange(13.0f, LightNum);
            PointLights->SetRange(13.0f, LightNum + 1);
            LightNum += 2;
            check = true;
        }
        else if (static_cast<int>(StackLightManagerTimer * 10.0f) % 3 == 2 &&check)
        {
            check = false;
        }
    }

    PointLights->SetRange(0.0f, 0);
    PointLights->SetRange(0.0f, 1);

    // ����L�[�������ꂽ��
    if (input->PushEnterNow() == false) return;

    // �I�΂�Ă���ԍ����擾
    const short index = input->GetStateIndex();


    switch (index)  // �I��ԍ��̈ʒu�ɂ���STATE�ɕύX
    {
    case 0: // �`���[�g���A��
        input->IndexToOne();    // �V�[�����`���[�g���A���ɕς���p�̃t���O
        camera->SetNextCameraPos({});
        current_state_index = STATE::NONE;

        input->SetIsDemo();

        
        bgm_sand_storm->Play(true, OptionSystem::Instance().GetBgmVolume() * 0.01f);
        

        break;

    case 1: // �X�e�[�W�I��
        input->IndexToZero();

        current_state_index = STATE::STAGE_SELECT;

        // TODO: �X�e�[�W���������炱���𑝂₷
        input->SetStateIndexMax(5);

        break;

    case 2: // �^�C�g���֖߂�
        
        input->IndexToZero();

        current_state_index = STATE::TITLE;
        imgui_now_titled = true;
        camera->SetNextCameraPos(TitleCamera::left_front_pos);
        input->SetStateIndexMax(3);
        break;
    }
    se_decision->Stop();
	se_decision->Play(false, OptionSystem::Instance().GetSeVolume());
}


void TitleManager::sStageSelect(float elapsedTime)
{
    // ��ԃ^�C�}�[�̍X�V
    lerp_timer += -2.0f * elapsedTime;
    if (lerp_timer < lerp_timer_min) lerp_timer = lerp_timer_min;

    // ����L�[�������ꂽ��
    if (input->PushEnterNow() == false) return;

    const short index = input->GetStateIndex();


    switch (index)
    {
    case 0:     // �X�e�[�W �P
        input->IndexToZero();  // �V�[�����Q�[���ɕς���p�̃t���O

        camera->SetNextCameraPos({ 1.0f,1.0f,1.0f });
        current_state_index = STATE::NONE;
        EnemyManager::Instance().SetStageIndex(EnemyManager::STAGE::STAGE1);

        input->SetIsDemo();

        break;

    case 1:     // �X�e�[�W 2
        input->IndexToZero();  // �V�[�����Q�[���ɕς���p�̃t���O

        camera->SetNextCameraPos({ 1.0f,1.0f,1.0f });
        current_state_index = STATE::NONE;
        EnemyManager::Instance().SetStageIndex(EnemyManager::STAGE::STAGE2);

        input->SetIsDemo();

        break;

    case 2:     // �X�e�[�W 3
        input->IndexToZero();  // �V�[�����Q�[���ɕς���p�̃t���O

        camera->SetNextCameraPos({ 1.0f,1.0f,1.0f });
        current_state_index = STATE::NONE;
        EnemyManager::Instance().SetStageIndex(EnemyManager::STAGE::STAGE3);

        input->SetIsDemo();

        break;

    case 3:     // �X�e�[�W 4
        input->IndexToZero();  // �V�[�����Q�[���ɕς���p�̃t���O

        camera->SetNextCameraPos({ 1.0f,1.0f,1.0f });
        current_state_index = STATE::NONE;
        EnemyManager::Instance().SetStageIndex(EnemyManager::STAGE::STAGE4);

        input->SetIsDemo();

        break;

    case 4:     // �X�e�[�W 5
        input->IndexToZero();  // �V�[�����Q�[���ɕς���p�̃t���O

        camera->SetNextCameraPos({ 1.0f,1.0f,1.0f });
        current_state_index = STATE::NONE;
        EnemyManager::Instance().SetStageIndex(EnemyManager::STAGE::STAGE5);

        input->SetIsDemo();

        break;

    //case 5:     // �X�e�[�W extra 1
    //    input->IndexToZero();  // �V�[�����Q�[���ɕς���p�̃t���O

    //    camera->SetNextCameraPos({ 1.0f,1.0f,1.0f });
    //    current_state_index = STATE::NONE;
    //    EnemyManager::Instance().SetStageIndex(EnemyManager::STAGE::EXTRA2);

    //    input->SetIsDemo();

    //    break;

    //case 6:     // �X�e�[�W extra 2
    //    input->IndexToZero();  // �V�[�����Q�[���ɕς���p�̃t���O

    //    camera->SetNextCameraPos({ 1.0f,1.0f,1.0f });
    //    current_state_index = STATE::NONE;
    //    EnemyManager::Instance().SetStageIndex(EnemyManager::STAGE::EXTRA3);

    //    input->SetIsDemo();

    //    break;

    //case 7:     // �X�e�[�W extra 3
    //    input->IndexToZero();  // �V�[�����Q�[���ɕς���p�̃t���O

    //    camera->SetNextCameraPos({ 1.0f,1.0f,1.0f });
    //    current_state_index = STATE::NONE;
    //    EnemyManager::Instance().SetStageIndex(EnemyManager::STAGE::EXTRA3);

    //    input->SetIsDemo();

    //    break;

    //case 8:

    //case 7:

    case 5:
        // �X�^�[�g�֖߂�
        input->IndexToOne();

        current_state_index = STATE::START;

        input->SetStateIndexMax(2);

        break;
    }

    if(index != 5)
    {
        bgm_sand_storm->Play(true, OptionSystem::Instance().GetBgmVolume() * 0.01f);
    }

    se_decision->Stop();
    se_decision->Play(false, OptionSystem::Instance().GetSeVolume());
}


void TitleManager::sOption(float elapsedTime)
{
    // �I�v�V�������J���Ă��āA����t���O�����Ă΁A�I�v�V���������t���O�𗧂Ă�
    if (openOption && option->GetIsCloseOption())openOption = false;


    // ��Ԓ��͉������Ȃ�
    if (!(option->GetIsNowLerp()))
    {
        if(openOption == false)
        {
            openOption = true;
            closeOption = false;
            option->Initialize();
        }

        if (openOption)
        {
            if (GetAsyncKeyState(VK_TAB) & 0x8000)
            {
                openOption = false;
                closeOption = true;
                option->Finalize();

                current_state_index = STATE::TITLE;
            }
            else if(option->GetIsFinalizeFrame())
            {
                openOption = false;
                closeOption = true;
                option->Finalize();

                current_state_index = STATE::TITLE;
            }
        }
    }
}


void TitleManager::sEnd(float elapsedTime)
{
    // �I��
    SceneManager::Instance().SetEnd();
}


void TitleManager::sNone(float elapsedTime)
{
    SkipMovie(elapsedTime);


    const short index = camera->GetPhaseIndex();
    if (index < 2) return;

    constexpr float PlayerSpeed = 150.0f;
    constexpr float sound_ratio = 3.0f;

    switch (StartPerformanceState)
    {
    case 0:
        // ������
        StartPerformanceTime = 0.0f;
        StartPerformanceState++;

        

        // �J�����V�F�C�N�X�^�[�g
        camera->StartDemoCameraShake();

        // ��lunch�X�^�[�g
        se_lunch->Play(false, OptionSystem::Instance().GetSeVolume() * 1.0f);

        /*FallThrough*/
    case 1:
        // ���b�ԃJ�����V�F�C�N���X���[�N�H
        // �Q�lURL
        // https://www.youtube.com/watch?v=HxOs_qc910U

        StartPerformanceTime += elapsedTime;

        {
            float ratio = 1.0f - (StartPerformanceTime / 1.0f);
            if(ratio < 0.0f)
            {
                ratio = 0.0f;
            }

            bgm->SetVolume(OptionSystem::Instance().GetBgmVolume() * 0.5f * ratio);
            
        }
        
        if(hdl_smoke == -1)
        {
            DirectX::XMFLOAT3 pos = mdl_player.transform.Position;
            pos.y += -2.0f;
            pos.z += 2.0f;
            hdl_smoke = efk_smoke->Play(pos, {0.0f,3.14f,0.0f}, 0.2f);
        }


        


        if(StartPerformanceTime>=2.5f)
        {
            StartPerformanceTime = 0.0f;
            StartPerformanceState++;
        }
        break;
    case 2:
        /*���˃G�t�F�N�g�Ȃ�*/

        EffectManager::Instance().GetEffekseerManager()->StopEffect(hdl_smoke);

        bgm->Stop();

        // ��take_off�X�^�[�g
        se_take_off->Play(false, OptionSystem::Instance().GetSeVolume() * 2.0f);

        se_sonic_boom->Play(false, OptionSystem::Instance().GetSeVolume() * 7.3f);

        se_doom->Play(false, OptionSystem::Instance().GetSeVolume() * sound_ratio);

        StartPerformanceState++;
        /*FallThrough*/
    case 3:
        // ����



        StartPerformanceTime += elapsedTime;
        if (StartPerformanceTime >= 3.0f)
        {
            StartPerformanceTime = 3.0f;
        }

        {
            const float ratio = 1.0f - (StartPerformanceTime / 3.4f);

            se_lunch->SetVolume(OptionSystem::Instance().GetSeVolume() * ratio * ratio * 1.0f );

            se_take_off->SetVolume(OptionSystem::Instance().GetSeVolume() * ratio * 1.2f);

            se_sonic_boom->SetVolume(OptionSystem::Instance().GetSeVolume() * ratio * ratio * 7.3f);

            se_doom->SetVolume(OptionSystem::Instance().GetSeVolume() * ratio * sound_ratio);
        }
        
        
        // �O�����x�N�g��
        DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(mdl_player.rotate.Angle.x, mdl_player.rotate.Angle.y, mdl_player.rotate.Angle.z).r[2];
        DirectX::XMFLOAT3 forward{};
        DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));

        // �ʒu���X�V
        mdl_player.transform.Position.x += forward.x * PlayerSpeed * elapsedTime;
        mdl_player.transform.Position.y += forward.y * PlayerSpeed * elapsedTime;
        mdl_player.transform.Position.z += forward.z * PlayerSpeed * elapsedTime;

        mdl_launch_pad.transform.Position.z = mdl_player.transform.Position.z;
        if (mdl_launch_pad.transform.Position.z <= -200.0f)
        {
            mdl_launch_pad.transform.Position.z = -200.0f;

        }
        else
        {
            // �ΉԃG�t�F�N�g
            DirectX::XMFLOAT3 pos = mdl_launch_pad.transform.Position;
            //pos.y += 5.0f;
            pos.z += 10.0f;

            pos.x = 3.0f;
            hdl_sparkle = efk_sparkle->Play(pos, {}, 0.01f);
            pos.x = -3.0f;
            hdl_sparkle = efk_sparkle->Play(pos, {}, 0.01f);
        }
        break;
    }


    //// ���x���߂���
    //SpeedRatio(elapsedTime, index);


    //// �O�����x�N�g��
    //DirectX::XMVECTOR Forward = DirectX::XMMatrixRotationRollPitchYaw(mdl_player.rotate.Angle.x, mdl_player.rotate.Angle.y, mdl_player.rotate.Angle.z).r[2];
    //DirectX::XMFLOAT3 forward;
    //DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(Forward));


    //// �ʒu���X�V
    //mdl_player.transform.Position.x += forward.x * mdl_player.speed_ratio * elapsedTime;
    //mdl_player.transform.Position.y += forward.y * mdl_player.speed_ratio * elapsedTime;
    //mdl_player.transform.Position.z += forward.z * mdl_player.speed_ratio * elapsedTime;

    //mdl_launch_pad.transform.Position.z = mdl_player.transform.Position.z;
    //if (mdl_launch_pad.transform.Position.z <= -200.0f) mdl_launch_pad.transform.Position.z = -200.0f;

    // �V�[���؂�ւ��̏���
    if (camera->GetPhaseIndex() < 5) return;
    if (skip_timer.NowTime() <= 0.0f) return;





    //-------------------------------------�X�e�[�W�I����A�Q�[���X�^�[�g���Ȃ�-----------------------------------//
#ifdef _DEBUG
    if (0)return; //�@���̓X�^�[�g����
#endif
    //-----------------------------------------------------------------------------------------------------//





    // �V�[���؂�ւ��̃t���O�𗧂Ă�
    if (next_scene) return;

    switch (input->GetStateIndex())
    {
    case 0:
        // �V�[���J�� "�Q�[��"
        next_scene = new SceneLoading(new SceneGame());
        break;

    case 1:
        // �V�[���J�� "�`���[�g���A��"
        next_scene = new SceneLoading(new SceneTutorial());
        break;
    }
}


