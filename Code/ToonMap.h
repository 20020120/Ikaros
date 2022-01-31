#pragma once

#include"TextureLoder.h"

#include <cstdint>

#include<wrl.h>
#include"misc.h"


class ToonMap
{
public:
    // ������
    ToonMap(ID3D11Device* d, uint16_t slot);
    ~ToonMap() {}

    // PS_Shader�ɃZ�b�g
    void SetShaderResourceView(ID3D11DeviceContext* dc);
private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv{nullptr};

    int16_t Slot{ -1 };
};


//--------------------<������>--------------------//
inline ToonMap::ToonMap(ID3D11Device* d, uint16_t slot) {
    // �e�N�X�`���̏��𑗂�X���b�g��ݒ�
    Slot = slot;

    HRESULT hr{ S_OK };

    // �e�N�X�`����ǂݍ���
    hr = DirectX::CreateWICTextureFromFile(
        d,                                                     // ID3D11Device
        L"./resources/Sprite/Shaders/ToonMap/ToonMap.png",     // �e�N�X�`���̃p�X
        nullptr,                                               // ID3D11Resource
        srv.GetAddressOf()                                     // �V�F�[�_�[���\�[�X�r���[
    );

    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

inline void ToonMap::SetShaderResourceView(ID3D11DeviceContext* dc)
{
    // PS_Shader�Ƀe�N�X�`����ݒ肷��

    assert(Slot != -1);  // PS_Shader�̂ǂ̃X���b�g�Ƀe�N�X�`���𑗂邩�w�肵�Ă�������

    dc->PSSetShaderResources(Slot, 1, srv.GetAddressOf());
}