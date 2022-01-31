//****************************************************************
// 
//  StaticModelComponent
// 
//****************************************************************

#define InstanceDraw 0;

static const int MaxValue = 1500; // インスタンス描画の最大値
//
//--------------------<頂点シェーダへの入力>--------------------//
struct VsInput
{
	float4 Position : POSITION; // 位置
	float4 Normal : NORMAL;  // 法線
	float2 TexCoord : TEXCOORD; //	テクスチャの位置
};


//--------------------<頂点シェーダからの出力>--------------------//
struct VsOutPut
{
	float4 Position : SV_POSITION;   // モデルからのローカル座標
	float4 Color    : COLOR;         // 色
	float2 Texcoord : TEXCOORD;      // テクスチャの位置
	float4 WorldPosition : POSITION; // ワールド座標
	float4 WorldNormal : NORMAL;     // ワールドの法線
};



//--------------------<定数バッファ>--------------------//


cbuffer Scene_CBuffer : register(b1)
{
	row_major float4x4 ViewProjection; // カメラ行列
	float4 CameraPosition; // カメラ位置
}

cbuffer Constants : register(b2)
{
	float4 Position[MaxValue];
	float4 Color[MaxValue];

};
