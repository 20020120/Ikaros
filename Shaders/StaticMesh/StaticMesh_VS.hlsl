#include"StaticMesh.hlsli"
 VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
{

	VS_OUT vout;
	vout.Position = mul(position, mul(World, ViewProjection));
 
	vout.WorldPosition = mul(position, World);
	normal.w = 0;
	vout.WorldNormal = normalize(mul(normal, World));
	
	//float4 N = normalize(mul(normal, World));
	//float4 L = normalize(-LightDirection);
 
	vout.Color = MaterialColor;
	vout.Texcoord = texcoord;

 
	return vout;

}