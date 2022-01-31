#pragma once
#include<vector>
#include"SpriteBatch.h"
class Renderer2D
{
 
    Renderer2D();
    ~Renderer2D();
    
    void PlayAnimation(int Index);

    


    std::vector<Sprite_Batch> SpritesVec{};
};
