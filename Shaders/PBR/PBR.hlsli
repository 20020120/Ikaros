//****************************************************************
// 
//  Physically Based Rendering
// 
//****************************************************************

//--------------------<PS_Shaderへの入力>--------------------//
struct PsInput
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION;
	float4 WorldNormal : NORMAL;
	float4 Tangent : TANGENT;
	float2 Texcoord : TEXCOORD;
	float4 Color : COLOR;
};

//--------------------<ピクセルシェーダーからの出力>--------------------//
struct PsOutPut
{
	float4 Main_RenderTarget : SV_Target0 ;
	float4 Luminaunce_RenderTarget : SV_Target1;
};

//****************************************************************
// 
//  定数バッファ
// 
//****************************************************************

//--------------------<オブジェクト>--------------------//
static const int MAX_BONES = 256;
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
	row_major float4x4 World;
	float4 MaterialColor;
	row_major float4x4 BoneTransforms[MAX_BONES];
};

//--------------------<カメラ>--------------------//
cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
	row_major float4x4 ViewProjection;
	float4 CameraPosition;
};

//--------------------<ディレクションライト>--------------------//
cbuffer DirectionLight_Cb : register(b2)
{
	float4 DirLightDirection; // ディレクションライトの向き
	float4 DirLightColor;     // ディレクションライトの色
	float  DirLightPower;      // ディレクションライトの強さ
	float3 Lxxx0;
}


//****************************************************************
// 
//   テクスチャの種類
// 
//****************************************************************
const int BaseColor = 0; // ベースカラー 
const int Emissive = 1;  // エミッシブ
const int Metallic = 2;  // メタリック
const int Roughness = 3; // ラフネス


