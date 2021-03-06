#include"Framebuffer.h"
#include"misc.h"

FrameBuffer::FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height)
{
    HRESULT hr{ S_OK };
    Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
    D3D11_TEXTURE2D_DESC texture2d_desc{};
    texture2d_desc.Width = width;
    texture2d_desc.Height = height;
    texture2d_desc.MipLevels = 1;
    texture2d_desc.ArraySize = 1;
    texture2d_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    texture2d_desc.SampleDesc.Count = 1;
    texture2d_desc.SampleDesc.Quality = 0;
    texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
    texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;   // RenderTargetとShaderResourceを同時に使える
    texture2d_desc.CPUAccessFlags = 0;
    texture2d_desc.MiscFlags = 0;
    hr = device->CreateTexture2D(&texture2d_desc, 0, render_target_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



    D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc{};
    render_target_view_desc.Format = texture2d_desc.Format;
    render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = device->CreateRenderTargetView(render_target_buffer.Get(), &render_target_view_desc,
        RenderTargetView.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
    shader_resource_view_desc.Format = texture2d_desc.Format;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shader_resource_view_desc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(render_target_buffer.Get(), &shader_resource_view_desc,
        ShaderResourceView[0].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
    texture2d_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    hr = device->CreateTexture2D(&texture2d_desc, 0, depth_stencil_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
    depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Flags = 0;
    hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc,
        DepthStencilView.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    shader_resource_view_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &shader_resource_view_desc,
        ShaderResourceView[1].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    ViewPort.Width = static_cast<float>(width);
    ViewPort.Height = static_cast<float>(height);
    ViewPort.MinDepth = 0.0f;
    ViewPort.MaxDepth = 1.0f;
    ViewPort.TopLeftX = 0.0f;
    ViewPort.TopLeftY = 0.0f;

}

void FrameBuffer::f_Clear(ID3D11DeviceContext* dc, float r, float g, float b, float a, float depth)
{
    float color[4]{ r, g, b, a };
    dc->ClearRenderTargetView(RenderTargetView.Get(), color);
    dc->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}

void FrameBuffer::f_Activate(ID3D11DeviceContext* dc)
{
    ViewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    dc->RSGetViewports(&ViewportCount, CachedViewports);
    dc->OMGetRenderTargets(1, CachedRenderTargetView.ReleaseAndGetAddressOf(),
        CachedDepthStencilView.ReleaseAndGetAddressOf());

    dc->RSSetViewports(1, &ViewPort);
    dc->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(),
        DepthStencilView.Get());
}

void FrameBuffer::f_Deactivate(ID3D11DeviceContext* dc)
{
    dc->RSSetViewports(ViewportCount, CachedViewports);
    dc->OMSetRenderTargets(1, CachedRenderTargetView.GetAddressOf(),
        CachedDepthStencilView.Get());
}
