

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


//****************************************************************
// 
// ���C���֐�
// 
//****************************************************************
float4 main(VS_OUT pin) : SV_TARGET0
{
	// �n�[�t�����o�[�g
	float p = dot(pin.WorldNormal.xyz, -D_LightDirection.xyz);
	p = p * 0.5f + 0.5;
	p = p * p;

	float4 BaseColor = texture_maps[0].Sample(sampler_states[0], pin.Texcoord);
	float4 ret = (p * BaseColor);

	


	ret.a = BaseColor.a;

	return ret;

}

