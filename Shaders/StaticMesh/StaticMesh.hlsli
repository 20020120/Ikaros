 struct VS_OUT
  {	
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float2 Texcoord : TEXCOORD;
	float4 WorldPosition : POSITION;
	float4 WorldNormal : NORMAL;
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
	float4 CameraPosition;
};

