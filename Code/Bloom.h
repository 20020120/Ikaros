#pragma once
#include"Framebuffer.h"
#include"Fullscreenquad.h"
#include"ConstantBuffer.h"

#include "TextureLoder.h"


#include<memory>
class Bloom
{
public:
    // ������
    Bloom(ID3D11Device* d);
    ~Bloom()=default;

    // �u���[���̊J�n�t���O
    void BeginBloom(ID3D11DeviceContext* dc);

    // �u���[������Ȃ����f���̕`��
    

    // �u���[���̏I���t���O
    void EndBloom(ID3D11DeviceContext* dc);
private:
    std::unique_ptr<FrameBuffer> LuminanceFrameBuffer{ nullptr };  // �P�x���o�p
    std::unique_ptr<FrameBuffer> MainFrameBuffer{ nullptr }; // �`��p
    std::unique_ptr<Fullscreenquad> fullscreenquad{ nullptr };
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaders[2]{ nullptr,nullptr };

};

//--------------------<������>--------------------//
inline Bloom::Bloom(ID3D11Device* d)
{
    // �_�E���T���v�����O���Đݒ�
    LuminanceFrameBuffer = std::make_unique<FrameBuffer>(d, 1280.0f * 0.5f, 720.0f * 0.5f);

    MainFrameBuffer = std::make_unique<FrameBuffer>(d, 1280.0f, 720.0f);

    fullscreenquad = std::make_unique<Fullscreenquad>(d, "./resources/Shaders/FullScreen_PS.cso");

    CreatePsFromCso(d, "./resources/Shaders/LuminateExtraction_PS.cso", pixelShaders[0].GetAddressOf());

    CreatePsFromCso(d, "./resources/Shaders/Blur_PS.cso", pixelShaders[1].GetAddressOf());
}


//--------------------<�`��J�n�O�ɌĂ�>--------------------//
inline void Bloom::BeginBloom(ID3D11DeviceContext* dc)
{

    MainFrameBuffer->f_Clear(dc);
    MainFrameBuffer->f_Activate(dc);

}

inline void Bloom::EndBloom(ID3D11DeviceContext* dc)
{
    MainFrameBuffer->f_Deactivate(dc);

    // �P�x���o�p�o�b�t�@�ɕ`��
    LuminanceFrameBuffer->f_Clear(dc);
    LuminanceFrameBuffer->f_Activate(dc);

    fullscreenquad->Blit(dc, MainFrameBuffer->GetShaderResourceView(0).GetAddressOf(), 0, 1, pixelShaders[0].Get());

    LuminanceFrameBuffer->f_Deactivate(dc);


    //--------------------<�񖇂̉摜������>--------------------//
    ID3D11ShaderResourceView* SRVs[2]
    {
        MainFrameBuffer->GetShaderResourceView(0).Get(),
        LuminanceFrameBuffer->GetShaderResourceView(0).Get()
    };

    fullscreenquad->Blit(dc, SRVs, 0, 2, pixelShaders[1].Get());

}



