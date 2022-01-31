//****************************************************************
// 
//  Physically Based Rendering
// 
//****************************************************************

//--------------------<PS_Shader�ւ̓���>--------------------//
struct PsInput
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION;
	float4 WorldNormal : NORMAL;
	float4 Tangent : TANGENT;
	float2 Texcoord : TEXCOORD;
	float4 Color : COLOR;
};

//--------------------<�s�N�Z���V�F�[�_�[����̏o��>--------------------//
struct PsOutPut
{
	float4 Main_RenderTarget : SV_Target0 ;
	float4 Luminaunce_RenderTarget : SV_Target1;
};

//****************************************************************
// 
//  �萔�o�b�t�@
// 
//****************************************************************

//--------------------<�I�u�W�F�N�g>--------------------//
static const int MAX_BONES = 256;
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
	row_major float4x4 World;
	float4 MaterialColor;
	row_major float4x4 BoneTransforms[MAX_BONES];
};

//--------------------<�J����>--------------------//
cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
	row_major float4x4 ViewProjection;
	float4 CameraPosition;
};

//--------------------<�f�B���N�V�������C�g>--------------------//
cbuffer DirectionLight_Cb : register(b2)
{
	float4 DirLightDirection; // �f�B���N�V�������C�g�̌���
	float4 DirLightColor;     // �f�B���N�V�������C�g�̐F
	float  DirLightPower;      // �f�B���N�V�������C�g�̋���
	float3 Lxxx0;
}


//****************************************************************
// 
//   �e�N�X�`���̎��
// 
//****************************************************************
const int BaseColor = 0; // �x�[�X�J���[ 
const int Emissive = 1;  // �G�~�b�V�u
const int Metallic = 2;  // ���^���b�N
const int Roughness = 3; // ���t�l�X


