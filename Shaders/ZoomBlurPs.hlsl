
struct PsInput
{
	float4 Position : SV_POSITION;
	float2 Texcoord : TEXCOORD;
};

// テクスチャを取得
Texture2D<float4> gMainTexture : register(t0);
SamplerState gSamplerState : register(s0);

cbuffer a : register(b9)
{
	float4 ReferencePosition;
	float ZoomPower;
	int FocusDetail;
	float2 Pad0;
};

float4 main(PsInput PsInput_) : SV_TARGET
{
	float4 color = gMainTexture.Sample(gSamplerState, PsInput_.Texcoord);

	float focusPower = ZoomPower;
	int focusDetail = FocusDetail;
	float2 referencePosition = ReferencePosition.xy;

	float2 uv = PsInput_.Texcoord;
	float2 focus = uv - referencePosition;

	float4 outColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	for (int i = 0; i < focusDetail; i++)
	{
		float power = 1.0 - focusPower * (1.0 / 1280.0f) * float(i);
		outColor.rgb += gMainTexture.Sample(gSamplerState, focus * power + referencePosition).rgb;
	}
    
	outColor.rgb *= 1.0 / float(focusDetail);


	return outColor;
}