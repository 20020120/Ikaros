//****************************************************************
// 
//  �W�I���g���V�F�[�_�[
// 
//****************************************************************
#include"../SkinnedMesh/SkinnedMesh.hlsli"

//--------------------<����>--------------------//
//struct GSInput
//{
//	float4 Position : SV_POSITION;
//	float4 WorldPosition : POSITION;
//	float4 WorldNormal : NORMAL;
//	float4 Tangent : TANGENT;
//	float2 Texcoord : TEXCOORD;
//	float4 Color : COLOR;
//};

//--------------------<�o��>--------------------//
//struct GSOutput
//{
//	float4 Position : SV_POSITION;
//	float4 WorldPosition : POSITION;
//	float4 WorldNormal : NORMAL;
//	float4 Tangent : TANGENT;
//	float2 Texcoord : TEXCOORD;
//	float4 Color : COLOR;
//};




[maxvertexcount(3)]
void main(
	triangle GSInput input[3],
	inout TriangleStream<GSOutput> output)
{
	// �O�p�`�����
	float3 pos[3];
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element = input[i];
		pos[i] = input[i].Position.xyz;
	}
	
	// �O�p�`�̖ʂ̖@�������߂�
	
	float3 VecA = pos[1] - pos[0];
	float3 VecB = pos[2] - pos[0];
	
	float3 NormalVec = cross(VecA, VecB);
	NormalVec = normalize(NormalVec);
	
	for (uint j = 0; j < 3; j++)
	{
		GSOutput element = input[j];
		
		// �@���̕����Ɉړ�������
		element.Position += float4(float3(NormalVec * 0.0f), 0.0f);
		
		// �F�𔖂�����
		element.Color.a -= 0.0f * 0.1f;
		
		output.Append(element);
	}
}

