#include"PBR.hlsli"


//--------------------<テクスチャ>--------------------//
Texture2D<float4> Tex_BaseColor : register(t0);
Texture2D<float4> Tex_Emissive  : register(t1);
Texture2D<float4> Tex_Metallic  : register(t2);
Texture2D<float4> Tex_Roughness : register(t3);
Texture2D<float4> Tex_Normal    : register(t4);

//--------------------<サンプラー>--------------------//
SamplerState sampler_states[3] : register(s0);


cbuffer PointLight :register(b3)
{
	float4 PLightPosition;
	float4 PLightColor;
	float4 PLightInfluenceRange;
}

cbuffer PointLight : register(b4)
{
	float4 PLightPosition2;
	float4 PLightColor2;
	float4 PLightInfluenceRange2;
}


///////////////////////////////////////////////////
// 関数
///////////////////////////////////////////////////


// ベックマン分布を計算する
float Beckmann(float m, float t)
{
	float t2 = t * t;
	float t4 = t * t * t * t;
	float m2 = m * m;
	float D = 1.0f / (4.0f * m2 * t4);
	D *= exp((-1.0f / m2) * (1.0f - t2) / t2);
	return D;
}

// フレネルを計算。Schlick近似を使用
float SpcFresnel(float f0, float u)
{
    // from Schlick
	return f0 + (1 - f0) * pow(1 - u, 5);
}

// 法線を計算する
float3 GetNormal(float3 normal, float3 tangent, float3 biNormal, float2 uv)
{
	float3 binSpaceNormal = Tex_Normal.Sample(sampler_states[0], uv);
	binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;

	float3 newNormal = tangent * binSpaceNormal.x + biNormal * binSpaceNormal.y + normal * binSpaceNormal.z;

	return newNormal;
}

/// <summary>
/// Cook-Torranceモデルの鏡面反射を計算
/// </summary>
/// <param name="L">光源に向かうベクトル</param>
/// <param name="V">視点に向かうベクトル</param>
/// <param name="N">法線ベクトル</param>
/// <param name="metallic">金属度</param>
float CookTorranceSpecular(float3 L, float3 V, float3 N, float metallic)
{
	float microfacet = 0.76f;

    // 金属度を垂直入射の時のフレネル反射率として扱う
    // 金属度が高いほどフレネル反射は大きくなる
	float f0 = metallic;

    // ライトに向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
	float3 H = normalize(L + V);

    // 各種ベクトルがどれくらい似ているかを内積を利用して求める
	float NdotH = saturate(dot(N, H));
	float VdotH = saturate(dot(V, H));
	float NdotL = saturate(dot(N, L));
	float NdotV = saturate(dot(N, V));

    // D項をベックマン分布を用いて計算する
	float D = Beckmann(microfacet, NdotH);

    // F項をSchlick近似を用いて計算する
	float F = SpcFresnel(f0, VdotH);

    // G項を求める
	float G = min(1.0f, min(2 * NdotH * NdotV / VdotH, 2 * NdotH * NdotL / VdotH));

	float PI = 3.1415926f; // π

    // m項を求める
	float m = PI * NdotV * NdotH;

    // ここまで求めた、値を利用して、Cook-Torranceモデルの鏡面反射を求める
	return max(F * D * G / m, 0.0);
}

/// <summary>
/// フレネル反射を考慮した拡散反射を計算
/// </summary>
/// <remark>
/// この関数はフレネル反射を考慮した拡散反射率を計算します
/// フレネル反射は、光が物体の表面で反射する現象のとこで、鏡面反射の強さになります
/// 一方拡散反射は、光が物体の内部に入って、内部錯乱を起こして、拡散して反射してきた光のことです
/// つまりフレネル反射が弱いときには、拡散反射が大きくなり、フレネル反射が強いときは、拡散反射が小さくなります
///
/// </remark>
/// <param name="N">法線</param>
/// <param name="L">光源に向かうベクトル。光の方向と逆向きのベクトル。</param>
/// <param name="V">視線に向かうベクトル。</param>
/// <param name="roughness">粗さ。0～1の範囲。</param>
float CalcDiffuseFromFresnel(float3 N, float3 L, float3 V)
{
    // step-1 ディズニーベースのフレネル反射による拡散反射を真面目に実装する。
    // 光源に向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
	float3 H = normalize(L + V);

    // 粗さは0.5で固定。
	float roughness = 0.5f;

	float energyBias = lerp(0.0f, 0.5f, roughness);
	float energyFactor = lerp(1.0, 1.0 / 1.51, roughness);

    // 光源に向かうベクトルとハーフベクトルがどれだけ似ているかを内積で求める
	float dotLH = saturate(dot(L, H));

    // 光源に向かうベクトルとハーフベクトル、
    // 光が平行に入射したときの拡散反射量を求めている
	float Fd90 = energyBias + 2.0 * dotLH * dotLH * roughness;

    // 法線と光源に向かうベクトルwを利用して拡散反射率を求める
	float dotNL = saturate(dot(N, L));
	float FL = (1 + (Fd90 - 1) * pow(1 - dotNL, 5));

    // 法線と視点に向かうベクトルを利用して拡散反射率を求める
	float dotNV = saturate(dot(N, V));
	float FV = (1 + (Fd90 - 1) * pow(1 - dotNV, 5));

    // 法線と光源への方向に依存する拡散反射率と、法線と視点ベクトルに依存する拡散反射率を
    // 乗算して最終的な拡散反射率を求めている。PIで除算しているのは正規化を行うため
	return (FL * FV * energyFactor);
}

/// <summary>
/// ピクセルシェーダー
/// </summary>
PsOutPut main(PsInput psIn)
{


     // 法線を計算
	float3 normal = normalize(psIn.WorldNormal.xyz);
	float3 T = normalize(psIn.Tangent.xyz);
    
    float sigma = psIn.Tangent.w;
    
	T = normalize(T - dot(normal, T) * sigma);
    
	float3 B = normalize(cross(normal, T) * sigma);
    float4 Normal=Tex_Normal.Sample(sampler_states[1], psIn.Texcoord);
    Normal = (Normal * 2.0) - 1.0;
	normal = normalize((Normal.x * T) + (Normal.y * B) + (Normal.z * normal));

    // アルベドカラー、スペキュラカラー、金属度、滑らかさをサンプリングする。
    // アルベドカラー（拡散反射光）

	float4 albedoColor = Tex_BaseColor.Sample(sampler_states[2], psIn.Texcoord);

	// もしテクスチャの色が白なら
    const float WhiteChecker = albedoColor.x + albedoColor.y + albedoColor.z;
	albedoColor.x = lerp(albedoColor.x, PLightColor.x, step(2.9f, WhiteChecker));
	albedoColor.y = lerp(albedoColor.y, PLightColor.y, step(2.9f, WhiteChecker));
	albedoColor.z = lerp(albedoColor.z, PLightColor.z, step(2.9f, WhiteChecker));



    // スペキュラカラーはアルベドカラーと同じにする。
	float3 specColor = albedoColor;

    // 金属度
	float metallic = Tex_Metallic.Sample(sampler_states[0], psIn.Texcoord).r;

    // 滑らかさ
	float smooth =1.0f- Tex_Roughness.Sample(sampler_states[0], psIn.Texcoord).a;

	// エミッシブ
	float4 EmissiveColor = Tex_Emissive.Sample(sampler_states[0], psIn.Texcoord);

   
     const float3 toEye = normalize(CameraPosition.xyz - psIn.WorldPosition.xyz);

	float3 lig = 0;
	{
        // シンプルなディズニーベースの拡散反射を実装する。
        // フレネル反射を考慮した拡散反射を計算
        const float diffuseFromFresnel = CalcDiffuseFromFresnel(
            normal, -DirLightDirection.xyz, toEye);

        const float PI = 3.1415926f; // π

        // 正規化Lambert拡散反射を求める
        const float NdotL = saturate(dot(normal, -DirLightDirection.xyz));
        const float3 lambertDiffuse = DirLightColor.xyz * NdotL / PI;

		
    	float p = dot(psIn.WorldNormal.xyz, -DirLightDirection.xyz);
    	//p = p * 0.5f + 0.5;
    	//p = p * p;

        // 最終的な拡散反射光を計算する
		const float3 diffuse = albedoColor.xyz * diffuseFromFresnel * lambertDiffuse;

        // Cook-Torranceモデルを利用した鏡面反射率を計算する
        // Cook-Torranceモデルの鏡面反射率を計算する
		float3 spec = CookTorranceSpecular(
            -DirLightDirection.xyz, toEye, normal, smooth)
            * DirLightColor.xyz;

        // 金属度が高ければ、鏡面反射はスペキュラカラー、低ければ白
        // スペキュラカラーの強さを鏡面反射率として扱う
		spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic);

        // 滑らかさを使って、拡散反射光と鏡面反射光を合成する
        // 滑らかさが高ければ、拡散反射は弱くなる
		lig += (diffuse * (1.0f - smooth) + spec) * 6.0f;
		
	}



	{
		// ライトの向きを計算
		float4 Direction = psIn.WorldPosition - PLightPosition;

		 // シンプルなディズニーベースの拡散反射を実装する。
        // フレネル反射を考慮した拡散反射を計算
		const float diffuseFromFresnel = CalcDiffuseFromFresnel(
            normal, -Direction.xyz, toEye);

		const float PI = 3.1415926f; // π

        // 正規化Lambert拡散反射を求める
		const float NdotL = saturate(dot(normal, -Direction.xyz));
		const float3 lambertDiffuse = PLightColor.xyz * NdotL / PI;


        // 最終的な拡散反射光を計算する
		const float3 diffuse = albedoColor.xyz * lambertDiffuse * diffuseFromFresnel;

        // Cook-Torranceモデルを利用した鏡面反射率を計算する
        // Cook-Torranceモデルの鏡面反射率を計算する
		float3 spec = CookTorranceSpecular(
            -Direction.xyz, toEye, normal, smooth)
            * PLightColor.xyz;

        // 金属度が高ければ、鏡面反射はスペキュラカラー、低ければ白
        // スペキュラカラーの強さを鏡面反射率として扱う
		spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic);

        // 滑らかさを使って、拡散反射光と鏡面反射光を合成する
        // 滑らかさが高ければ、拡散反射は弱くなる

		// ライトの距離に夜よる減衰を計算する
		const float3 Dist = length(psIn.WorldPosition.xyz - PLightPosition.xyz);
		 float afrect = max(1.0f - 1.0f / PLightInfluenceRange.x * Dist, 0.0f);
		afrect = pow(afrect, 3.0f);
		lig += (diffuse * (1.0f - smooth) + spec) * afrect;

	}

	
	{
		// ライトの向きを計算
		float4 Direction = psIn.WorldPosition - PLightPosition2;

		 // シンプルなディズニーベースの拡散反射を実装する。
        // フレネル反射を考慮した拡散反射を計算
		const float diffuseFromFresnel = CalcDiffuseFromFresnel(
            normal, -Direction.xyz, toEye);

		const float PI = 3.1415926f; // π

        // 正規化Lambert拡散反射を求める
		const float NdotL = saturate(dot(normal, -Direction.xyz));
		const float3 lambertDiffuse = PLightColor2.xyz * NdotL / PI;

		float p = dot(psIn.WorldNormal.xyz, -Direction.xyz);
    	//p = p * 0.5f + 0.5;
    	//p = p * p;

        // 最終的な拡散反射光を計算する
		const float3 diffuse = albedoColor.xyz * lambertDiffuse * diffuseFromFresnel;

        // Cook-Torranceモデルを利用した鏡面反射率を計算する
        // Cook-Torranceモデルの鏡面反射率を計算する
		float3 spec = CookTorranceSpecular(
            -Direction.xyz, toEye, normal, smooth)
            * PLightColor2.xyz;

        // 金属度が高ければ、鏡面反射はスペキュラカラー、低ければ白
        // スペキュラカラーの強さを鏡面反射率として扱う
		spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic);

        // 滑らかさを使って、拡散反射光と鏡面反射光を合成する
        // 滑らかさが高ければ、拡散反射は弱くなる

		// ライトの距離に夜よる減衰を計算する
		const float3 Dist = length(psIn.WorldPosition.xyz - PLightPosition2.xyz);
	    float afrect = max(1.0f - 1.0f / PLightInfluenceRange2.x * Dist, 0.0f);
		afrect = pow(afrect, 3.0f);
		lig += ((diffuse * (1.0f - smooth) + spec) * afrect);

	}


     const float3 ambientLight = { 0.2f, 0.2f, 0.2f };
    // 環境光による底上げ
	lig += ambientLight * albedoColor.xyz;

	// 最後にエミッシブを合成
	lig += EmissiveColor.xyz;
	
	float4 finalColor = 1.0f;
	finalColor.xyz = lig;
	PsOutPut PsOut;

	PsOut.Main_RenderTarget = finalColor;
	PsOut.Luminaunce_RenderTarget = EmissiveColor;

	return PsOut;
}
