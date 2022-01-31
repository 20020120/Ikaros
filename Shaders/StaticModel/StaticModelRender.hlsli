//****************************************************************
// 
//  StaticModelComponent
// 
//****************************************************************

#define InstanceDraw 0;

static const int MaxValue = 1500; // �C���X�^���X�`��̍ő�l
//
//--------------------<���_�V�F�[�_�ւ̓���>--------------------//
struct VsInput
{
	float4 Position : POSITION; // �ʒu
	float4 Normal : NORMAL;  // �@��
	float2 TexCoord : TEXCOORD; //	�e�N�X�`���̈ʒu
};


//--------------------<���_�V�F�[�_����̏o��>--------------------//
struct VsOutPut
{
	float4 Position : SV_POSITION;   // ���f������̃��[�J�����W
	float4 Color    : COLOR;         // �F
	float2 Texcoord : TEXCOORD;      // �e�N�X�`���̈ʒu
	float4 WorldPosition : POSITION; // ���[���h���W
	float4 WorldNormal : NORMAL;     // ���[���h�̖@��
};



//--------------------<�萔�o�b�t�@>--------------------//


cbuffer Scene_CBuffer : register(b1)
{
	row_major float4x4 ViewProjection; // �J�����s��
	float4 CameraPosition; // �J�����ʒu
}

cbuffer Constants : register(b2)
{
	float4 Position[MaxValue];
	float4 Color[MaxValue];

};
