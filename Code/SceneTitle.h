#pragma once
#include"Scene.h"

#include "TitleManager.h"

#include<memory>
class SceneTitle:public SceneParent
{
public:
    SceneTitle();
    ~SceneTitle()override;

    void Initialize(ID3D11Device* device)override;
    void Update(float elapsedTime)override;
    void Render(ID3D11DeviceContext* dc)override;
    void Finalize()override;

private:
    void GuiMenu();

private: // 関数ポインタ
    //using pFunc = void(SceneTitle::*)(float);
    //
    //enum class SELECT
    //{
    //    NONE,
    //    START,
    //    OPTION,
    //    END,
    //
    //    TUTORIAL,
    //    STAGE,
    //};
    //
    //std::map<SELECT,pFunc> 


private: // 変数の定義
    std::unique_ptr<TitleManager> title_manager;
};
