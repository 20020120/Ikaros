#include"Sprite.hlsli"

//Texture2D Color_map : register(t0);
//SamplerState Point_sampler_State : register(s0);
//SamplerState Liner_Sampler_State : register(s1);
//SamplerState Anisotropic_Sampler_State : register(s2);
//float4 main(VS_OUT pin):SV_Target
//{
//	#if 0
//	const float2 Center = float2(1280 / 2, 720 / 2);
//	float Distance = length(Center - pin.pos.xy);
//	if (Distance > 200)
//		return 1;
//	else
//		return float4(1, 0, 0, 1);
//	#else
	
//	//return Color_map.Sample(Point_sampler_State, pin.TexCooad);
//	float4 color = Color_map.Sample(Point_sampler_State, pin.TexCooad);
//	return color * pin.Color;
	
//	#endif
//}

Texture2D Diffuse_Map : register(t0);
Texture2D Dissolve_Map : register(t1);
Texture2D Dst_MapDst_Map : register(t2);
cbuffer Dissolve:register(b0)
{
	float p;
}

SamplerState Diffuse_Map_Sampler_State : register(s0);

float4 main(VS_OUT pin):SV_Target
{
    const float pow = saturate(p);

	float4 SrcColor = Diffuse_Map.Sample(Diffuse_Map_Sampler_State, pin.TexCooad) * pin.Color;
	float4 DstColor = Dst_MapDst_Map.Sample(Diffuse_Map_Sampler_State, pin.TexCooad) * pin.Color;
	float mask = Dissolve_Map.Sample(Diffuse_Map_Sampler_State, pin.TexCooad).r;

	return lerp(SrcColor * pin.Color, DstColor, step(mask, pow));

}