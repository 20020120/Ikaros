#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>


class FrameBuffer
{
public:
    FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height);
    virtual ~FrameBuffer() = default;


    void f_Clear(ID3D11DeviceContext* dc,
        float r = 0, float g = 0, float b = 0, float a = 1, float depth = 1);
    void f_Activate(ID3D11DeviceContext* dc);
    void f_Deactivate(ID3D11DeviceContext* dc);

    auto GetRenderTarget() const { return RenderTargetView; }
    auto GetShaderResourceView(int Index) { return ShaderResourceView[Index]; }
    auto GetViewPort() const { return ViewPort; }
private:
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView[2];
    D3D11_VIEWPORT ViewPort;

    UINT ViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
    D3D11_VIEWPORT CachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> CachedRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> CachedDepthStencilView;

};
