
//****************************************************************
// 
//  ジオメトリシェーダー
// 
//****************************************************************

#include "../SkinnedMesh/SkinnedMesh.hlsli"

[maxvertexcount(3)]
void main(
	triangle GSInput input[3] ,
	inout TriangleStream< GSOutput > output)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element = input[i];
		output.Append(element);
	}
}