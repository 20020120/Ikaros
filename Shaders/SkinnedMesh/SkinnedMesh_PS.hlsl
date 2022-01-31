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


// スペキュラー
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
//  前方宣言
// 
//****************************************************************
float4 CalcLambertDiffuse(float4 LightDirection, float4 LightColor, float4 Normal);
float3 CalcPhongSpecular(float3 LightDirection, float3 LightColor, float3 WorldPos, float3 Normal);


//****************************************************************
// 
// メイン関数
// 
//****************************************************************
float4 main(VS_OUT pin) : SV_TARGET
{
	// 戻り値
	float4 Ret_Color = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	//--------------------<テクスチャ>--------------------//
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
	
	//--------------------<ディレクションライト>--------------------//
	#if L_Direction
	
	float3 N = normalize(pin.WorldNormal.xyz);
	float3 L = normalize(LD.xyz);
	float3 diffuse = max(0, dot(N, L));
	float4 color = float4(float3(pin.Color.rgb + LC.rgb), pin.Color.a);
	
	color.rgb = normalize(color.rgb);
	
	Ret_Color = float4(diffuse, 1) * color * Lp * Texcolor;
	#endif
	
    //--------------------<アンビエントライト>--------------------//
	Ret_Color.rgb += 0.2f;
	
	
	//--------------------<ポイントライト>--------------------//
	#if L_Point
	
	float3 LightDirection = pin.WorldPosition - P_LightPosition;
	LightDirection = normalize(LightDirection);
	
	// ピクセルの法線とライトの方向の内積を計算する
	float d = dot(pin.WorldNormal.xyz, LightDirection) * -1.0f;
    // 内積の値を0以上の値にする
	d = max(0.0f, d);
    // 拡散反射光を計算する
	float4 Lam = p_LightColor * d;
	
	// ポイントライトとの距離を求める
	float3 distance = length(pin.WorldPosition - P_LightPosition);
	// 減衰率を計算する
	float afrect = 1.0f - 1.0f / p_LightRange * distance;
	afrect = max(0.0f, afrect);
	
	Lam *= afrect;
	
	// ポイントライトの計算結果を反映
	Ret_Color += Lam;
	
	#endif
	
	
	
	
	
	////--------------------<スポットライト>--------------------//
	
	//// 入射するスポットライトの向きを計算する
	//float3 SpotLightDirection = pin.WorldPosition.xyz - S_LightPosition.xyz;
	//SpotLightDirection = normalize(SpotLightDirection);
	
	//// 拡散反射を計算する
	//// ピクセルの法線とライトの方向の内積を計算する
	//float d2 = dot(pin.WorldNormal.xyz, SpotLightDirection.xyz) * -1.0f;
 //   // 内積の値を0以上の値にする
	// d2 = max(0.0f, d2);
	//float4 DiffuseSpotLight = S_LightColor * d2;
	
	
	
	//// 距離による減衰を計算する
	//float3 Distance = length(pin.WorldPosition.xyz - S_LightPosition.xyz);
	//afrect = 1.0f - 1.0f / S_LightRange * Distance;
	//afrect = max(0.0f, afrect);
	//afrect = pow(afrect, 3.0f);
	
	//// 距離減衰の影響を反映する
	//DiffuseSpotLight *= afrect;
	
	//// 入射光と反射光の角度を計算する
	//float angle = dot(SpotLightDirection.xyz, S_LightDirection.xyz);
	//angle = abs(acos(angle));
	
	//// 角度による影響率を計算する
	//afrect = 1.0f - 1.0f / S_LightAngle * angle;
	//afrect = max(0.0f, afrect);
	//afrect = pow(afrect, 0.5f);
	
	//// 角度による影響率を反映する
	//DiffuseSpotLight *= afrect;
	
	//float4 Ret = DiffuseSpotLight + DiffuseLig;
	
	//return float4(toonColor.rgb, 1.0f);
	
	return Ret_Color;
	
}

//****************************************************************
// 
//  拡散反射を計算
// 
//****************************************************************
float4 CalcLambertDiffuse(float4 LightDirection,float4 LightColor,float4 Normal)
{
	// ピクセルの法線とライトの方向の内積を計算する
	float t = dot(Normal, LightDirection) * -1.0f;
	
	// 内積の値を矯正
	t = max(0.0f, t);
	
	return LightColor * t;
}

