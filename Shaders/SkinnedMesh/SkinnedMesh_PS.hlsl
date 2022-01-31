#include"SkinnedMesh.hlsli"

#define POINT 0;
#define LINER 1
#define ANISOTROPIC 2

#define L_Direction 1
#define L_Point 1

SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[4] : register(t0);
//Texture2D diffuse_map : register(t0);
//Texture2D normal_map : register(t1);
Texture2D ToonMap : register(t2);


// �X�y�L�����[
//float4 main(VS_OUT pin) : SV_TARGET
//{
//	float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.Texcoord);
//	float alpha = color.a;
	
//	const float GAMMA = 2.2;
//	color.rgb = pow(color.rgb, GAMMA);

	
//	float3 N = normalize(pin.WorldNormal.xyz);
//	float3 T = normalize(pin.Tangent.xyz);
	
//	float sigma = pin.Tangent.w;
	
//	T = normalize(T - dot(N, T) * sigma);
	
//	float3 B = normalize(cross(N, T) * sigma);
//	float4 normal=texture_maps[1].Sample(sampler_states[LINER], pin.Texcoord);
//	normal = (normal * 2.0) - 1.0;
//	N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));
	
//	float3 L = normalize(-LightDirection.xyz);
//	float3 diffuse = color.rgb*max(0, dot(N, L));
//	float3 V = normalize(CameraPosition.xyz - pin.WorldPosition.xyz);
//	float3 specular = pow(max(0, dot(N, normalize(V + L))), 128);
	
//	return float4(diffuse + specular, color.a) * pin.Color*2.0f;

//}

//****************************************************************
// 
//  �O���錾
// 
//****************************************************************
float4 CalcLambertDiffuse(float4 LightDirection, float4 LightColor, float4 Normal);
float3 CalcPhongSpecular(float3 LightDirection, float3 LightColor, float3 WorldPos, float3 Normal);


//****************************************************************
// 
// ���C���֐�
// 
//****************************************************************
float4 main(VS_OUT pin) : SV_TARGET
{
	// �߂�l
	float4 Ret_Color = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	//--------------------<�e�N�X�`��>--------------------//
	float4 Texcolor = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.Texcoord);
	//float alpha = Texcolor.a;

	const float GAMMA = 2.2;
	Texcolor.rgb = pow(Texcolor.rgb, GAMMA);
	
	
	//--------------------<toonShader>--------------------//
	float p = dot(pin.WorldNormal.xyz, LD.xyz);
	p = p * 0.5f + 0.5f;
	p = p * p;

	float4 toonColor = ToonMap.Sample(sampler_states[ANISOTROPIC], float2(p, 0.0f));
	
	
	Texcolor.rgb *= toonColor.rgb;
	
	//--------------------<�f�B���N�V�������C�g>--------------------//
	#if L_Direction
	
	float3 N = normalize(pin.WorldNormal.xyz);
	float3 L = normalize(LD.xyz);
	float3 diffuse = max(0, dot(N, L));
	float4 color = float4(float3(pin.Color.rgb + LC.rgb), pin.Color.a);
	
	color.rgb = normalize(color.rgb);
	
	Ret_Color = float4(diffuse, 1) * color * Lp * Texcolor;
	#endif
	
    //--------------------<�A���r�G���g���C�g>--------------------//
	Ret_Color.rgb += 0.2f;
	
	
	//--------------------<�|�C���g���C�g>--------------------//
	#if L_Point
	
	float3 LightDirection = pin.WorldPosition - P_LightPosition;
	LightDirection = normalize(LightDirection);
	
	// �s�N�Z���̖@���ƃ��C�g�̕����̓��ς��v�Z����
	float d = dot(pin.WorldNormal.xyz, LightDirection) * -1.0f;
    // ���ς̒l��0�ȏ�̒l�ɂ���
	d = max(0.0f, d);
    // �g�U���ˌ����v�Z����
	float4 Lam = p_LightColor * d;
	
	// �|�C���g���C�g�Ƃ̋��������߂�
	float3 distance = length(pin.WorldPosition - P_LightPosition);
	// ���������v�Z����
	float afrect = 1.0f - 1.0f / p_LightRange * distance;
	afrect = max(0.0f, afrect);
	
	Lam *= afrect;
	
	// �|�C���g���C�g�̌v�Z���ʂ𔽉f
	Ret_Color += Lam;
	
	#endif
	
	
	
	
	
	////--------------------<�X�|�b�g���C�g>--------------------//
	
	//// ���˂���X�|�b�g���C�g�̌������v�Z����
	//float3 SpotLightDirection = pin.WorldPosition.xyz - S_LightPosition.xyz;
	//SpotLightDirection = normalize(SpotLightDirection);
	
	//// �g�U���˂��v�Z����
	//// �s�N�Z���̖@���ƃ��C�g�̕����̓��ς��v�Z����
	//float d2 = dot(pin.WorldNormal.xyz, SpotLightDirection.xyz) * -1.0f;
 //   // ���ς̒l��0�ȏ�̒l�ɂ���
	// d2 = max(0.0f, d2);
	//float4 DiffuseSpotLight = S_LightColor * d2;
	
	
	
	//// �����ɂ�錸�����v�Z����
	//float3 Distance = length(pin.WorldPosition.xyz - S_LightPosition.xyz);
	//afrect = 1.0f - 1.0f / S_LightRange * Distance;
	//afrect = max(0.0f, afrect);
	//afrect = pow(afrect, 3.0f);
	
	//// ���������̉e���𔽉f����
	//DiffuseSpotLight *= afrect;
	
	//// ���ˌ��Ɣ��ˌ��̊p�x���v�Z����
	//float angle = dot(SpotLightDirection.xyz, S_LightDirection.xyz);
	//angle = abs(acos(angle));
	
	//// �p�x�ɂ��e�������v�Z����
	//afrect = 1.0f - 1.0f / S_LightAngle * angle;
	//afrect = max(0.0f, afrect);
	//afrect = pow(afrect, 0.5f);
	
	//// �p�x�ɂ��e�����𔽉f����
	//DiffuseSpotLight *= afrect;
	
	//float4 Ret = DiffuseSpotLight + DiffuseLig;
	
	//return float4(toonColor.rgb, 1.0f);
	
	return Ret_Color;
	
}

//****************************************************************
// 
//  �g�U���˂��v�Z
// 
//****************************************************************
float4 CalcLambertDiffuse(float4 LightDirection,float4 LightColor,float4 Normal)
{
	// �s�N�Z���̖@���ƃ��C�g�̕����̓��ς��v�Z����
	float t = dot(Normal, LightDirection) * -1.0f;
	
	// ���ς̒l������
	t = max(0.0f, t);
	
	return LightColor * t;
}

