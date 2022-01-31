#include"PBR.hlsli"


//--------------------<�e�N�X�`��>--------------------//
Texture2D<float4> Tex_BaseColor : register(t0);
Texture2D<float4> Tex_Emissive  : register(t1);
Texture2D<float4> Tex_Metallic  : register(t2);
Texture2D<float4> Tex_Roughness : register(t3);
Texture2D<float4> Tex_Normal    : register(t4);

//--------------------<�T���v���[>--------------------//
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
// �֐�
///////////////////////////////////////////////////


// �x�b�N�}�����z���v�Z����
float Beckmann(float m, float t)
{
	float t2 = t * t;
	float t4 = t * t * t * t;
	float m2 = m * m;
	float D = 1.0f / (4.0f * m2 * t4);
	D *= exp((-1.0f / m2) * (1.0f - t2) / t2);
	return D;
}

// �t���l�����v�Z�BSchlick�ߎ����g�p
float SpcFresnel(float f0, float u)
{
    // from Schlick
	return f0 + (1 - f0) * pow(1 - u, 5);
}

// �@�����v�Z����
float3 GetNormal(float3 normal, float3 tangent, float3 biNormal, float2 uv)
{
	float3 binSpaceNormal = Tex_Normal.Sample(sampler_states[0], uv);
	binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;

	float3 newNormal = tangent * binSpaceNormal.x + biNormal * binSpaceNormal.y + normal * binSpaceNormal.z;

	return newNormal;
}

/// <summary>
/// Cook-Torrance���f���̋��ʔ��˂��v�Z
/// </summary>
/// <param name="L">�����Ɍ������x�N�g��</param>
/// <param name="V">���_�Ɍ������x�N�g��</param>
/// <param name="N">�@���x�N�g��</param>
/// <param name="metallic">�����x</param>
float CookTorranceSpecular(float3 L, float3 V, float3 N, float metallic)
{
	float microfacet = 0.76f;

    // �����x�𐂒����˂̎��̃t���l�����˗��Ƃ��Ĉ���
    // �����x�������قǃt���l�����˂͑傫���Ȃ�
	float f0 = metallic;

    // ���C�g�Ɍ������x�N�g���Ǝ����Ɍ������x�N�g���̃n�[�t�x�N�g�������߂�
	float3 H = normalize(L + V);

    // �e��x�N�g�����ǂꂭ�炢���Ă��邩����ς𗘗p���ċ��߂�
	float NdotH = saturate(dot(N, H));
	float VdotH = saturate(dot(V, H));
	float NdotL = saturate(dot(N, L));
	float NdotV = saturate(dot(N, V));

    // D�����x�b�N�}�����z��p���Čv�Z����
	float D = Beckmann(microfacet, NdotH);

    // F����Schlick�ߎ���p���Čv�Z����
	float F = SpcFresnel(f0, VdotH);

    // G�������߂�
	float G = min(1.0f, min(2 * NdotH * NdotV / VdotH, 2 * NdotH * NdotL / VdotH));

	float PI = 3.1415926f; // ��

    // m�������߂�
	float m = PI * NdotV * NdotH;

    // �����܂ŋ��߂��A�l�𗘗p���āACook-Torrance���f���̋��ʔ��˂����߂�
	return max(F * D * G / m, 0.0);
}

/// <summary>
/// �t���l�����˂��l�������g�U���˂��v�Z
/// </summary>
/// <remark>
/// ���̊֐��̓t���l�����˂��l�������g�U���˗����v�Z���܂�
/// �t���l�����˂́A�������̂̕\�ʂŔ��˂��錻�ۂ̂Ƃ��ŁA���ʔ��˂̋����ɂȂ�܂�
/// ����g�U���˂́A�������̂̓����ɓ����āA�����������N�����āA�g�U���Ĕ��˂��Ă������̂��Ƃł�
/// �܂�t���l�����˂��ア�Ƃ��ɂ́A�g�U���˂��傫���Ȃ�A�t���l�����˂������Ƃ��́A�g�U���˂��������Ȃ�܂�
///
/// </remark>
/// <param name="N">�@��</param>
/// <param name="L">�����Ɍ������x�N�g���B���̕����Ƌt�����̃x�N�g���B</param>
/// <param name="V">�����Ɍ������x�N�g���B</param>
/// <param name="roughness">�e���B0�`1�͈̔́B</param>
float CalcDiffuseFromFresnel(float3 N, float3 L, float3 V)
{
    // step-1 �f�B�Y�j�[�x�[�X�̃t���l�����˂ɂ��g�U���˂�^�ʖڂɎ�������B
    // �����Ɍ������x�N�g���Ǝ����Ɍ������x�N�g���̃n�[�t�x�N�g�������߂�
	float3 H = normalize(L + V);

    // �e����0.5�ŌŒ�B
	float roughness = 0.5f;

	float energyBias = lerp(0.0f, 0.5f, roughness);
	float energyFactor = lerp(1.0, 1.0 / 1.51, roughness);

    // �����Ɍ������x�N�g���ƃn�[�t�x�N�g�����ǂꂾ�����Ă��邩����ςŋ��߂�
	float dotLH = saturate(dot(L, H));

    // �����Ɍ������x�N�g���ƃn�[�t�x�N�g���A
    // �������s�ɓ��˂����Ƃ��̊g�U���˗ʂ����߂Ă���
	float Fd90 = energyBias + 2.0 * dotLH * dotLH * roughness;

    // �@���ƌ����Ɍ������x�N�g��w�𗘗p���Ċg�U���˗������߂�
	float dotNL = saturate(dot(N, L));
	float FL = (1 + (Fd90 - 1) * pow(1 - dotNL, 5));

    // �@���Ǝ��_�Ɍ������x�N�g���𗘗p���Ċg�U���˗������߂�
	float dotNV = saturate(dot(N, V));
	float FV = (1 + (Fd90 - 1) * pow(1 - dotNV, 5));

    // �@���ƌ����ւ̕����Ɉˑ�����g�U���˗��ƁA�@���Ǝ��_�x�N�g���Ɉˑ�����g�U���˗���
    // ��Z���čŏI�I�Ȋg�U���˗������߂Ă���BPI�ŏ��Z���Ă���̂͐��K�����s������
	return (FL * FV * energyFactor);
}

/// <summary>
/// �s�N�Z���V�F�[�_�[
/// </summary>
PsOutPut main(PsInput psIn)
{


     // �@�����v�Z
	float3 normal = normalize(psIn.WorldNormal.xyz);
	float3 T = normalize(psIn.Tangent.xyz);
    
    float sigma = psIn.Tangent.w;
    
	T = normalize(T - dot(normal, T) * sigma);
    
	float3 B = normalize(cross(normal, T) * sigma);
    float4 Normal=Tex_Normal.Sample(sampler_states[1], psIn.Texcoord);
    Normal = (Normal * 2.0) - 1.0;
	normal = normalize((Normal.x * T) + (Normal.y * B) + (Normal.z * normal));

    // �A���x�h�J���[�A�X�y�L�����J���[�A�����x�A���炩�����T���v�����O����B
    // �A���x�h�J���[�i�g�U���ˌ��j

	float4 albedoColor = Tex_BaseColor.Sample(sampler_states[2], psIn.Texcoord);

	// �����e�N�X�`���̐F�����Ȃ�
    const float WhiteChecker = albedoColor.x + albedoColor.y + albedoColor.z;
	albedoColor.x = lerp(albedoColor.x, PLightColor.x, step(2.9f, WhiteChecker));
	albedoColor.y = lerp(albedoColor.y, PLightColor.y, step(2.9f, WhiteChecker));
	albedoColor.z = lerp(albedoColor.z, PLightColor.z, step(2.9f, WhiteChecker));



    // �X�y�L�����J���[�̓A���x�h�J���[�Ɠ����ɂ���B
	float3 specColor = albedoColor;

    // �����x
	float metallic = Tex_Metallic.Sample(sampler_states[0], psIn.Texcoord).r;

    // ���炩��
	float smooth =1.0f- Tex_Roughness.Sample(sampler_states[0], psIn.Texcoord).a;

	// �G�~�b�V�u
	float4 EmissiveColor = Tex_Emissive.Sample(sampler_states[0], psIn.Texcoord);

   
     const float3 toEye = normalize(CameraPosition.xyz - psIn.WorldPosition.xyz);

	float3 lig = 0;
	{
        // �V���v���ȃf�B�Y�j�[�x�[�X�̊g�U���˂���������B
        // �t���l�����˂��l�������g�U���˂��v�Z
        const float diffuseFromFresnel = CalcDiffuseFromFresnel(
            normal, -DirLightDirection.xyz, toEye);

        const float PI = 3.1415926f; // ��

        // ���K��Lambert�g�U���˂����߂�
        const float NdotL = saturate(dot(normal, -DirLightDirection.xyz));
        const float3 lambertDiffuse = DirLightColor.xyz * NdotL / PI;

		
    	float p = dot(psIn.WorldNormal.xyz, -DirLightDirection.xyz);
    	//p = p * 0.5f + 0.5;
    	//p = p * p;

        // �ŏI�I�Ȋg�U���ˌ����v�Z����
		const float3 diffuse = albedoColor.xyz * diffuseFromFresnel * lambertDiffuse;

        // Cook-Torrance���f���𗘗p�������ʔ��˗����v�Z����
        // Cook-Torrance���f���̋��ʔ��˗����v�Z����
		float3 spec = CookTorranceSpecular(
            -DirLightDirection.xyz, toEye, normal, smooth)
            * DirLightColor.xyz;

        // �����x��������΁A���ʔ��˂̓X�y�L�����J���[�A�Ⴏ��Δ�
        // �X�y�L�����J���[�̋��������ʔ��˗��Ƃ��Ĉ���
		spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic);

        // ���炩�����g���āA�g�U���ˌ��Ƌ��ʔ��ˌ�����������
        // ���炩����������΁A�g�U���˂͎キ�Ȃ�
		lig += (diffuse * (1.0f - smooth) + spec) * 6.0f;
		
	}



	{
		// ���C�g�̌������v�Z
		float4 Direction = psIn.WorldPosition - PLightPosition;

		 // �V���v���ȃf�B�Y�j�[�x�[�X�̊g�U���˂���������B
        // �t���l�����˂��l�������g�U���˂��v�Z
		const float diffuseFromFresnel = CalcDiffuseFromFresnel(
            normal, -Direction.xyz, toEye);

		const float PI = 3.1415926f; // ��

        // ���K��Lambert�g�U���˂����߂�
		const float NdotL = saturate(dot(normal, -Direction.xyz));
		const float3 lambertDiffuse = PLightColor.xyz * NdotL / PI;


        // �ŏI�I�Ȋg�U���ˌ����v�Z����
		const float3 diffuse = albedoColor.xyz * lambertDiffuse * diffuseFromFresnel;

        // Cook-Torrance���f���𗘗p�������ʔ��˗����v�Z����
        // Cook-Torrance���f���̋��ʔ��˗����v�Z����
		float3 spec = CookTorranceSpecular(
            -Direction.xyz, toEye, normal, smooth)
            * PLightColor.xyz;

        // �����x��������΁A���ʔ��˂̓X�y�L�����J���[�A�Ⴏ��Δ�
        // �X�y�L�����J���[�̋��������ʔ��˗��Ƃ��Ĉ���
		spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic);

        // ���炩�����g���āA�g�U���ˌ��Ƌ��ʔ��ˌ�����������
        // ���炩����������΁A�g�U���˂͎キ�Ȃ�

		// ���C�g�̋����ɖ��錸�����v�Z����
		const float3 Dist = length(psIn.WorldPosition.xyz - PLightPosition.xyz);
		 float afrect = max(1.0f - 1.0f / PLightInfluenceRange.x * Dist, 0.0f);
		afrect = pow(afrect, 3.0f);
		lig += (diffuse * (1.0f - smooth) + spec) * afrect;

	}

	
	{
		// ���C�g�̌������v�Z
		float4 Direction = psIn.WorldPosition - PLightPosition2;

		 // �V���v���ȃf�B�Y�j�[�x�[�X�̊g�U���˂���������B
        // �t���l�����˂��l�������g�U���˂��v�Z
		const float diffuseFromFresnel = CalcDiffuseFromFresnel(
            normal, -Direction.xyz, toEye);

		const float PI = 3.1415926f; // ��

        // ���K��Lambert�g�U���˂����߂�
		const float NdotL = saturate(dot(normal, -Direction.xyz));
		const float3 lambertDiffuse = PLightColor2.xyz * NdotL / PI;

		float p = dot(psIn.WorldNormal.xyz, -Direction.xyz);
    	//p = p * 0.5f + 0.5;
    	//p = p * p;

        // �ŏI�I�Ȋg�U���ˌ����v�Z����
		const float3 diffuse = albedoColor.xyz * lambertDiffuse * diffuseFromFresnel;

        // Cook-Torrance���f���𗘗p�������ʔ��˗����v�Z����
        // Cook-Torrance���f���̋��ʔ��˗����v�Z����
		float3 spec = CookTorranceSpecular(
            -Direction.xyz, toEye, normal, smooth)
            * PLightColor2.xyz;

        // �����x��������΁A���ʔ��˂̓X�y�L�����J���[�A�Ⴏ��Δ�
        // �X�y�L�����J���[�̋��������ʔ��˗��Ƃ��Ĉ���
		spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic);

        // ���炩�����g���āA�g�U���ˌ��Ƌ��ʔ��ˌ�����������
        // ���炩����������΁A�g�U���˂͎キ�Ȃ�

		// ���C�g�̋����ɖ��錸�����v�Z����
		const float3 Dist = length(psIn.WorldPosition.xyz - PLightPosition2.xyz);
	    float afrect = max(1.0f - 1.0f / PLightInfluenceRange2.x * Dist, 0.0f);
		afrect = pow(afrect, 3.0f);
		lig += ((diffuse * (1.0f - smooth) + spec) * afrect);

	}


     const float3 ambientLight = { 0.2f, 0.2f, 0.2f };
    // �����ɂ���グ
	lig += ambientLight * albedoColor.xyz;

	// �Ō�ɃG�~�b�V�u������
	lig += EmissiveColor.xyz;
	
	float4 finalColor = 1.0f;
	finalColor.xyz = lig;
	PsOutPut PsOut;

	PsOut.Main_RenderTarget = finalColor;
	PsOut.Luminaunce_RenderTarget = EmissiveColor;

	return PsOut;
}
