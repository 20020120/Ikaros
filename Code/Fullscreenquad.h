#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <cstdint>


class Fullscreenquad
{
public:
    Fullscreenquad(ID3D11Device* Device, const char* PS_FilePath);
    virtual ~Fullscreenquad() = default;

    
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> embedded_VertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> embedded_PixelShader;

public:
    void Blit(ID3D11DeviceContext* immediate_contextbool, ID3D11ShaderResourceView** shader_resource_view,
        uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader = nullptr);

};