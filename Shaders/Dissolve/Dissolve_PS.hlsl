
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
Texture2D texture_maps[4] : register(t0);
Texture2D DissolveMaskTexture : register(t4);

//--------------------<���C�g�̒萔�o�b�t�@>--------------------//
cbuffer DirectionLight : register(b0)
{
	float3 D_LightDirection; // ���C�g�̌���
	float pad0;
	float4 Color; // ���C�g�̐F
	float Power; //���C�g�̋���
	float3 pad1; //�p�f�B���O
}



cbuffer DissolveData:register(b1)
{
	float DissolvePower; // �f�B�]���u��臒l
	float3 Pad0; // �p�f�B���O
}
//****************************************************************
// 
// ���C���֐�
// 
//****************************************************************
float4 main(VS_OUT pin) : SV_TARGET
{
    const float4 srcColor = texture_maps[0].Sample(sampler_states[0], pin.Texcoord);
    const float4 dstColor = { 0.0f, 0.0f, 0.0f, 0.0f }; // ����
	float4 mask = DissolveMaskTexture.Sample(sampler_states[0], pin.Texcoord) * float4(1, 1, 1, 1);

	float p = dot(pin.WorldNormal.xyz, -D_LightDirection.xyz);
	p = p * 0.5f + 0.5;
	p = p * p;

	// �f�B�]���u
	return lerp(srcColor * pin.Color, dstColor, step(mask.r, DissolvePower)) * p;

}

