#pragma once
#include "Scene.h"
#include<memory>
#include<mutex>

//****************************************************************
// 
// 定数
// 
//****************************************************************

enum class SceneLabel
{
    TITLE,
    GAME,
};

class SceneManager
{
private:
    SceneManager();
    ~SceneManager();
public:
    static SceneManager& Instance()
    {
        static SceneManager manager;
        return manager;
    }

    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************
    void Initialize();
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc);
    void Finalize();


    void ChangeScene(SceneParent* s);
    
    //--------------------<ゲッター>--------------------//
    auto& GetDevice() { return Device; }
    std::mutex& GetMutex() { return mutex; }
    auto GetEnd() { return is_end; }

    //--------------------<セッター>--------------------//
    void SetDevice(ID3D11Device* D) { Device = D; }
    void SetEnd() { is_end = true; }
private:
    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************
    SceneParent* CurrentScene;
    ID3D11Device* Device;
    std::mutex mutex;

    bool is_end = false;
};
