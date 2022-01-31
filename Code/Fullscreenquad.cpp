#include "Fullscreenquad.h"
#include"TextureLoder.h"
#include"misc.h"

Fullscreenquad::Fullscreenquad(ID3D11Device* Device, const char* PS_FilePath)
{
    CreateVsFromCso(Device, "./resources/Shaders/FullScreenQuad_VS.cso", embedded_VertexShader.ReleaseAndGetAddressOf(),
        nullptr, nullptr, 0);
    CreatePsFromCso(Device, PS_FilePath, embedded_PixelShader.ReleaseAndGetAddressOf());
}

void Fullscreenquad::Blit(ID3D11DeviceContext* dc, ID3D11ShaderResourceView** shader_resource_view, uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader)
{
    dc->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    dc->IASetInputLayout(nullptr);

    dc->VSSetShader(embedded_VertexShader.Get(), 0, 0);
    if (replaced_pixel_shader)
    {
        dc->PSSetShader(replaced_pixel_shader, 0, 0);
    }
    else
    {
        dc->PSSetShader(embedded_PixelShader.Get(), 0, 0);
    }

    dc->PSSetShaderResources(start_slot, num_views, shader_resource_view);

    dc->Draw(4, 0);
}
