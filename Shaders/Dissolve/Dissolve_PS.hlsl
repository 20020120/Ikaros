
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

//--------------------<ライトの定数バッファ>--------------------//
cbuffer DirectionLight : register(b0)
{
	float3 D_LightDirection; // ライトの向き
	float pad0;
	float4 Color; // ライトの色
	float Power; //ライトの強さ
	float3 pad1; //パディング
}



cbuffer DissolveData:register(b1)
{
	float DissolvePower; // ディゾルブの閾値
	float3 Pad0; // パディング
}
//****************************************************************
// 
// メイン関数
// 
//****************************************************************
float4 main(VS_OUT pin) : SV_TARGET
{
    const float4 srcColor = texture_maps[0].Sample(sampler_states[0], pin.Texcoord);
    const float4 dstColor = { 0.0f, 0.0f, 0.0f, 0.0f }; // 透明
	float4 mask = DissolveMaskTexture.Sample(sampler_states[0], pin.Texcoord) * float4(1, 1, 1, 1);

	float p = dot(pin.WorldNormal.xyz, -D_LightDirection.xyz);
	p = p * 0.5f + 0.5;
	p = p * p;

	// ディゾルブ
	return lerp(srcColor * pin.Color, dstColor, step(mask.r, DissolvePower)) * p;

}

