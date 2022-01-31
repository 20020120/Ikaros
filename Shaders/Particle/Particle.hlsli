//****************************************************************
// 
// パーティクル.h
// 
//****************************************************************


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


//****************************************************************
// 
//	 定数バッファ
// 
//****************************************************************

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

cbuffer MOVEDATA :register(b2)
{
	float3 velocity;  // 速度
	float Pad0;

	float Speed; // はやさ
	float3 Pad1;

	float elapsedTime; // 更新時間
	
	
}
 