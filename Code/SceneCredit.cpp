#include"SceneCredit.h"
#include"SceneTitle.h"
#include"SceneLoading.h"
#include"SceneManager.h"
SceneCredit::SceneCredit()
{
}

SceneCredit::~SceneCredit()
{
}

void SceneCredit::Initialize(ID3D11Device* d)
{
    SceneState = -1;
    StackTimer = 0.0f;
    const wchar_t* LogoPath = L"./resources/Sprite/TeamLogo.png";
    const wchar_t* CreditPath = L"./resources/Sprite/Credit.png";

    sprFirst = std::make_unique<Sprite_Batch>(d,
        L"./resources/Sprite/Shaders/AlphaMask.png", 1,
        L"./resources/Sprite/Shaders/dissolve.png",
        LogoPath);
    sprTeamLogo = std::make_unique<Sprite_Batch>(d,
        LogoPath, 1,
        L"./resources/Sprite/Shaders/dissolve.png",
        CreditPath);
    sprCredit = std::make_unique<Sprite_Batch>(d,
        CreditPath, 1,
        L"./resources/Sprite/Shaders/dissolve.png",
        L"./resources/Sprite/Shaders/AlphaMask.png");
    
}

void SceneCredit::Update(float elapsedTime)
{
    switch (SceneState)
    {
    case -1:
        SceneState++;
        break;
    case 0:
        //  何もなしからチームロゴにディゾルブ
        DissolvePower += elapsedTime * 0.5f;
        sprFirst->SetDissolvePower(DissolvePower);
        if(DissolvePower>=1.0f)
        {
            StackTimer += elapsedTime;
        }
        if(StackTimer>=1.0f)
        {
            SceneState++;
        }

        break;
    case 1:
        // 初期化
        DissolvePower = 0.0f;
        StackTimer = 0.0f;
        SceneState++;
        break;
    case 2:
        // チームロゴからクレジットに遷移
        DissolvePower += elapsedTime * 1.2f;
        sprTeamLogo->SetDissolvePower(DissolvePower);
        if (DissolvePower >= 1.0f)
        {
            StackTimer += elapsedTime;
        }
        if (StackTimer >= 1.0f)
        {
            SceneState++;
        }
        break;
    case 3:
        // 初期化 
        DissolvePower = 0.0f;
        StackTimer = 0.0f;
        SceneState++;
        break;
    case 4:
        // クレジットからロードが面に遷移
        DissolvePower += elapsedTime * 1.0f;
        sprCredit->SetDissolvePower(DissolvePower);
        if (DissolvePower >= 1.0f)
        {
            SceneState++;
        }
        break;
    case 5:
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle()));
        break;
    default: ;
    }
}

void SceneCredit::Render(ID3D11DeviceContext* dc)
{
    switch (SceneState)
    {
    case 0:
    
        sprFirst->Begin(dc);
        sprFirst->Render(dc, { 0.0f,0.0f }, { 1.0f,1.0f }, { 0.0f,0.0f }, { 1280.0f,720.0f }, { 0.0f,0.0f }, 0.0f, { 1.0f,1.0f,1.0f,1.0f });
        sprFirst->End(dc);
        break;
    case 1:
    case 2:
        sprTeamLogo->Begin(dc);
        sprTeamLogo->Render(dc, { 0.0f,0.0f }, { 1.0f,1.0f }, { 0.0f,0.0f }, { 1280.0f,720.0f }, { 0.0f,0.0f }, 0.0f, { 1.0f,1.0f,1.0f,1.0f });
        sprTeamLogo->End(dc);
        break;
    case 3:
    case 4:
        sprCredit->Begin(dc);
        sprCredit->Render(dc, { 0.0f,0.0f }, { 1.0f,1.0f }, { 0.0f,0.0f }, { 1280.0f,720.0f }, { 0.0f,0.0f }, 0.0f, { 1.0f,1.0f,1.0f,1.0f });
        sprCredit->End(dc);
        break;

    }

}

void SceneCredit::Finalize()
{
}
