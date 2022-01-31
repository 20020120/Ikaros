
#include"SkinnedMesh.hlsli"

VS_OUT main(VS_IN vin )
{
	
	vin.Normal.w = 0;
	float sigma = vin.Tangent;
	vin.Tangent.w = 0;

	float4 blendedposition = { 0, 0, 0, 1 };
	float4 blendednormal = { 0, 0, 0, 0 };
	float4 blendtangent = { 0, 0, 0, 0 };
	
	for (int boneindex = 0; boneindex < 4; ++boneindex)
	{
		blendedposition += vin.BoneWeights[boneindex] *
        mul(vin.Position, BoneTransforms[vin.BoneIndices[boneindex]]);
		
		blendednormal += vin.BoneWeights[boneindex] *
        mul(vin.Normal, BoneTransforms[vin.BoneIndices[boneindex]]);
		
		blendtangent += vin.BoneWeights[boneindex] *
        mul(vin.Tangent, BoneTransforms[vin.BoneIndices[boneindex]]);
	}
	
	vin.Position = float4(blendedposition.xyz, 1.0f);
    vin.Normal = float4(blendednormal.xyz, 0.0f);
    vin.Tangent = float4(blendtangent.xyz, 0.0f);

    VS_OUT vout;
    vout.Position = mul(vin.Position, mul(World, ViewProjection));
    
    vout.WorldPosition = mul(vin.Position, World);
    vout.WorldNormal = normalize(mul(vin.Normal, World));
    vout.Tangent = normalize(mul(vin.Tangent, World));
    vout.Tangent.w = sigma;
    
    vout.Texcoord = vin.Texcoord;
    vout.Color = MaterialColor;
    
return vout;
}