//--------------------<���C�g�̒萔�o�b�t�@>--------------------//
cbuffer DirectionLight : register(b2)
{
	float3 D_LightDirection; // ���C�g�̌���
	float pad0;
	float4 Color; // ���C�g�̐F
	float Power; //���C�g�̋���
	float3 pad1; //�p�f�B���O
}



struct VS_OUT
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION;
	float4 WorldNormal : NORMAL;
	float4 Tangent : TANGENT;
	float2 Texcoord : TEXCOORD;
	float4 Color : COLOR;
};

SamplerState sampler_states[3] : register(s0);
Texture2D<float4> texture_maps[4] : register(t0);


//--------------------<�|�C���g���C�g�p�萔�o�b�t�@>--------------------//
cbuffer pointRight : register(b3)
{
	float4 PLightPosition[10];
	float4 PLightColor[10];
	float4 PLightInfluenceRange[10];
}



//--------------------<�X�|�b�g���C�g>--------------------//
cbuffer SpotLight : register(b4)
{
	float4 SLightPosition;
	float SLightAngle;
	float3 Lxxx1;
	float4 SLightColor;
	float3 SLightDirection;
	float SLightRange;
}

//****************************************************************
// 
// ���C���֐�
// 
//****************************************************************
float4 main(VS_OUT pin) : SV_TARGET0
{
	float4 ret = 0;
    float4 BaseColor = texture_maps[0].Sample(sampler_states[0], pin.Texcoord);

	// �X�|�b�g���C�g
	{
		float3 Direction = pin.WorldPosition.xyz - SLightPosition.xyz;
		Direction = normalize(Direction);

		float p = dot(pin.WorldNormal.xyz, -Direction.xyz);

        // ���C�g�̋����ɖ��錸�����v�Z����
		const float3 Dist = length(pin.WorldPosition.xyz - SLightPosition.xyz);
		float afrect = max(1.0f - 1.0f / SLightRange * Dist, 0.0f);

		ret += BaseColor * afrect * p;

		float angle = dot(SLightDirection.xyz, Direction);
		angle = abs(acos(angle));

		afrect = max(1.0f - 1.0f / SLightAngle * angle, 0.0f);

		ret += BaseColor * afrect;
	}


	// �|�C���g���C�g���v�Z
	for (int i = 0; i < 10; i++)
	{
		// ���C�g�̌������v�Z
		float4 Direction = pin.WorldPosition - PLightPosition[i];

		float p = dot(pin.WorldNormal.xyz, -Direction.xyz);

		// ���C�g�̋����ɖ��錸�����v�Z����
		const float3 Dist = length(pin.WorldPosition.xyz - PLightPosition[i].xyz);
		const float afrect = max(1.0f - 1.0f / PLightInfluenceRange[i].x * Dist, 0.0f);

		ret += BaseColor*PLightColor[i] * afrect * p;
	}


	ret.a = 1.0f;

	return ret;

}

