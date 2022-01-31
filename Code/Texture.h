#pragma once
#include <cassert>
#include <d3d11.h>
#include<wrl.h>
#include<WICTextureLoader.h>
#include "misc.h"

//****************************************************************
// 
//  �e�N�X�`���𑗂�
// 
//****************************************************************
struct TextureResource
{
    int Slot = -1; // �Z�b�g����X���b�g
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV=nullptr;
};


class Texture
{
public:
    Texture() = default;
    ~Texture() = default;

    // �e�N�X�`����o�^����
    void Register(ID3D11Device* d, int slot, const wchar_t* texturePath)
    {
        HRESULT hr = S_OK;

        // ���\�[�X�̏�����
        resource.Slot = slot;

        // �e�N�X�`���̓ǂݍ���
        hr = DirectX::CreateWICTextureFromFile(
            d,
            texturePath,
            nullptr,
            resource.SRV.GetAddressOf()
        );

        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    // �s�N�Z���V�F�[�_�[�ɑ���
    void SetTexture(ID3D11DeviceContext* dc)
    {
        assert(resource.Slot != -1); // PixelShader�̂ǂ̃X���b�g�Ƀe�N�X�`���𑗂邩�w�肵�Ă�������

        dc->PSSetShaderResources(resource.Slot, 1, resource.SRV.GetAddressOf());
    }

private:
    TextureResource resource;
};