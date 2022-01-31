
//--------------------<VertexShader>--------------------//

struct VS_IN
{
	float4 Position : POSITION;
	float4 Normal : NORMAL;
	float4 Tangent : TANGENT;
	float2 Texcoord : TEXCOORD;
	float4 BoneWeights : WEIGHTS;
	uint4 BoneIndices : BONES;
};

struct VS_OUT
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION;
	float4 WorldNormal : NORMAL;
	float4 Tangent : TANGENT;
	float2 Texcoord : TEXCOORD;
	float4 Color : COLOR;
};

//--------------------<GeometryShader>--------------------//

#define GSInput VS_OUT

#define GSOutput VS_OUT
//--------------------<ConstantBuffer>--------------------//

static const int MAX_BONES = 256;
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
	row_major float4x4 World;
	float4 MaterialColor;
	row_major float4x4 BoneTransforms[MAX_BONES];
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
	row_major float4x4 ViewProjection;
	float4 CameraPosition;
};

cbuffer DirectionLight_Cb : register(b2)
{
	float4 LD;
	float4 LC;
	float Lp;
	float3 Lxxx0;
};

cbuffer PointLight : register(b3)
{
	float4 P_LightPosition;
	float4 p_LightColor;
	float p_LightRange;
	float3 Lxxx1;
};

cbuffer Expansion : register(b4)
{
	float Pow;
	float3 Lxxx;
}


