#pragma once
#include<DirectXMath.h>
#include<d3d11.h>
#include<wrl.h>
using namespace Microsoft::WRL;

class GeometricPrimitive
{
public:
	struct  Vertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Nomal;
	};

	struct Constants
	{
		DirectX::XMFLOAT4X4 World;
		DirectX::XMFLOAT4 MaterialColor;
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;

public:
	GeometricPrimitive(ID3D11Device* Device);
	virtual ~GeometricPrimitive() = default;

	void Render(ID3D11DeviceContext* ImmidiateContext,
		const DirectX::XMFLOAT4X4& World, const DirectX::XMFLOAT4& MaterialColor);
	

private:
	

protected:
	void CreateComBuffer(ID3D11Device* Device, Vertex* Vertices, size_t VertexCount,
		uint32_t* Indices, size_t IndexCount);

};
