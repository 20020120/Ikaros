#pragma once

#include<DirectXMath.h>
#include<d3d11.h>
#include<wrl.h>
#include"misc.h"



enum SetBufferType
{
	ALL,
	VERTEX,
	PIXEL,
	GEOMETRY,
};

class ConstantBuffer
{
public:
    ConstantBuffer()
		:constantbuffer()
	{}

	~ConstantBuffer() {};
	
    template<class T0>
	void Init(ID3D11Device* d, T0* data)
	{
		HRESULT hr = { S_OK };
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = sizeof(T0);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = d->CreateBuffer(&buffer_desc, nullptr, constantbuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


    template<class T1>
	void SetBuffer(ID3D11DeviceContext* dc, T1* data, uint8_t type = 0)
	{
	

		dc->UpdateSubresource(constantbuffer.Get(), 0, 0, data, 0, 0);

		switch (type)
		{
		case ALL:

			assert(PS_Slot != -1);  // ピクセルシェーダーのスロットが設定されていない
			assert(VS_Slot != -1);  // 頂点シェーダのスロットが設定されていない

			//定数バッファをピクセルシェーダーにバインド
			dc->PSSetConstantBuffers(PS_Slot, 1, constantbuffer.GetAddressOf());
			//定数バッファを頂点シェーダにバインド
			dc->VSSetConstantBuffers(VS_Slot, 1, constantbuffer.GetAddressOf());
			break;
		case VERTEX:

			assert(VS_Slot != -1);  // 頂点シェーダのスロットが設定されていない

			//定数バッファを頂点シェーダにバインド
			dc->VSSetConstantBuffers(VS_Slot, 1, constantbuffer.GetAddressOf());
			break;
		case PIXEL:

			assert(PS_Slot != -1);  // ピクセルシェーダーのスロットが設定されていない

			//定数バッファをピクセルシェーダーにバインド
			dc->PSSetConstantBuffers(PS_Slot, 1, constantbuffer.GetAddressOf());
			break;
		case GEOMETRY:
			//定数バッファをジオメトリシェーダーにバインド
			dc->GSSetConstantBuffers(GS_Slot, 1, constantbuffer.GetAddressOf());
			break;
		default:
			assert("SetBuffer() is Failed");
			break;
		}
	}

	void Set_PSSlot(uint16_t Slot) { PS_Slot = Slot; }
	void Set_VSSlot(uint16_t Slot) { VS_Slot = Slot; }
	void Set_GSSlot(uint16_t Slot) { GS_Slot = Slot; }

private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantbuffer{ nullptr };
	int16_t PS_Slot{ -1 };
	int16_t VS_Slot{ -1 };
	int16_t GS_Slot{ -1 };
};

