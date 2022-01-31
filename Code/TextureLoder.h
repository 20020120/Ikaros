#pragma once
#include"misc.h"
#include <WICTextureLoader.h>
#include<map>




HRESULT LoadTextureFromFile(ID3D11Device* Device, const wchar_t* FileName,
    ID3D11ShaderResourceView** ShaderResourceView, D3D11_TEXTURE2D_DESC* Texture2dDesc);

HRESULT CreateGsFromCso(ID3D11Device* Device, const char* CsoName, ID3D11GeometryShader** GeometryShader);

HRESULT CreateVsFromCso(ID3D11Device* Device, const char* CsoName, ID3D11VertexShader** VertexShader,
    ID3D11InputLayout** InputLayout, D3D11_INPUT_ELEMENT_DESC* InputElementDesc, UINT NumElements);

HRESULT CreatePsFromCso(ID3D11Device* Device, const char* CsoName, ID3D11PixelShader** PixelShader);

void ReleaseAllTextures();

HRESULT f_MakeDummyTexture(ID3D11Device* Device, ID3D11ShaderResourceView** ShaderResourceView,
    DWORD Value/*0xRRGGBBRR*/, UINT Dimension);
