#include"StaticMesh.hlsli"

Texture2D color_map : register(t0);
Texture2D nomal_map : register(t1);


SamplerState point_sampler_state : register(s0);
SamplerState liner_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);


float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = color_map.Sample(anisotropic_sampler_state, pin.Texcoord);
	float alpha = color.a;
	
	float3 N = normalize(pin.WorldNormal.xyz);
	
	float3 T = float3(1.0001, 0, 0);
	float3 B = normalize(cross(N, T));
	T = normalize(cross(B, N));
	
	float4 normal = nomal_map.Sample(liner_sampler_state, pin.Texcoord);
	//float4 normal = nomal_map.Sample(point_sampler_state, pin.Texcoord);
	normal = (normal * 2.0) - 1.0;
	normal.w = 0;
	
	N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));
	
	
	float3 L = normalize(-LightDirection.xyz);
	float3 diffuse = color.rgb * max(0, dot(N, L));
	
	float3 V = normalize(CameraPosition.xyz - pin.WorldPosition.xyz);
	float3 specular = pow(max(0, dot(N, normalize(V + L))), 128);
	
	return float4(diffuse + specular, alpha) * pin.Color;

}