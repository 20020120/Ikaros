
struct PS_IN
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION;
	float4 WorldNormal : NORMAL;
	float4 Tangent : TANGENT;
	float2 Texcoord : TEXCOORD;
	float4 Color : COLOR;
};


float4 main(PS_IN pin) : SV_TARGET
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}