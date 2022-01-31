#pragma once
#include"Fullscreenquad.h"
#include"Framebuffer.h"
#include"ConstantBuffer.h"
#include"TextureLoder.h"

#include<memory>
#include"imgui.h"
//****************************************************************
// 
// ズームブラークラス 
// 
//****************************************************************
class ZoomBlur
{
public:
    ZoomBlur(ID3D11Device* _pDevice_);
    ~ZoomBlur() = default;

    void fBegin(ID3D11DeviceContext* pDeviceContext_);
    void fEnd(ID3D11DeviceContext* pDeviceContext_);
    void fSetZoomPower(float elapsedTIme_);
private:
    std::unique_ptr<Fullscreenquad> mFullScreenQuad{ nullptr };
    std::unique_ptr<FrameBuffer> mMainFrameBuffer{ nullptr };
    Microsoft::WRL::ComPtr<ID3D11PixelShader> mZoomBlurPsShader{ nullptr };

    struct Data
    {
        DirectX::XMFLOAT4 CenterPosition{0.5f,0.5f,0.0f,0.0f};
        float ZoomPower{10};
        int FocusDetail{1};
        DirectX::XMFLOAT2 Pad0{};
    }mData;
    ConstantBuffer mConstantBuffer{};
};

inline ZoomBlur::ZoomBlur(ID3D11Device* pDevice_)
{
    mMainFrameBuffer = std::make_unique<FrameBuffer>(pDevice_, 1280.0f, 720.0f);
    mFullScreenQuad = std::make_unique<Fullscreenquad>(pDevice_, "./resources/Shaders/FullScreen_PS.cso");
    CreatePsFromCso(pDevice_, "./resources/Shaders/ZoomBlurPs.cso",
        mZoomBlurPsShader.GetAddressOf());
    
    mConstantBuffer.Init(pDevice_, &mData);
    mConstantBuffer.Set_PSSlot(9);
    mData.CenterPosition = { 0.5f,0.3f,0.0f,0.0f };
    mData.ZoomPower = 0.0f;
    mData.FocusDetail = 10;
}

inline void ZoomBlur::fBegin(ID3D11DeviceContext* pDeviceContext_)
{
#ifdef _DEBUG
    ImGui::Begin("ZoomBlur");
    ImGui::SliderFloat2("PointX", &mData.CenterPosition.x, 0.0f, 1.0f);
    ImGui::DragFloat("Power", &mData.ZoomPower);
    ImGui::InputInt("FocusDetail", &mData.FocusDetail);
    ImGui::End();
#endif



    mMainFrameBuffer->f_Clear(pDeviceContext_);
    mMainFrameBuffer->f_Activate(pDeviceContext_);
}

inline void ZoomBlur::fEnd(ID3D11DeviceContext* pDeviceContext_)
{
    mMainFrameBuffer->f_Deactivate(pDeviceContext_);

    mConstantBuffer.SetBuffer(pDeviceContext_,&mData,2);
    // 描画
    mFullScreenQuad->Blit(pDeviceContext_, 
        mMainFrameBuffer->GetShaderResourceView(0).GetAddressOf(), 
        0, 1,
        mZoomBlurPsShader.Get());

}

inline void ZoomBlur::fSetZoomPower(float ZoomPower_)
{
    mData.ZoomPower = ZoomPower_;
}
