
struct VS_OUT
{
	
	float4 Position : SV_Position;
	float4 Color : COLOR;
	
};
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
	row_major float4x4 World;
	float4 MaterialColor;
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
	row_major float4x4 ViewProjection;
	float4 LightDirection;
};

