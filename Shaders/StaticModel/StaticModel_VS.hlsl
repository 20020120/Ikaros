#include "StaticModelRender.hlsli"

static const float4 Scale = { 0.3f, 0.3f, 0.3f, 0.0f };


VsOutPut main(in VsInput Vin,in uint InstanceId : SV_InstanceID)
{
	VsOutPut vs_out;
	// 位置をローカル->ワールドに変換

	// スケール行列
    const float4x4 ScaleMat =
	{
		Scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, Scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, Scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// 回転行列
    const float4x4 RotateMat =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
	    0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
 


	// 移動行列
    const float4x4 PosMat =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		Position[InstanceId].xyz,1.0f
	};

	const float4x4 WorldMat = mul(mul(ScaleMat, RotateMat), PosMat);

	
	vs_out.Position = mul(Vin.Position, mul(WorldMat, ViewProjection));
	vs_out.WorldPosition = mul(Vin.Position, WorldMat);

	Vin.Normal.w = 0;

	vs_out.WorldNormal = normalize(mul(Vin.Normal, WorldMat));

	vs_out.Texcoord = Vin.TexCoord;

	vs_out.Color = Color[InstanceId];

	return vs_out;
}