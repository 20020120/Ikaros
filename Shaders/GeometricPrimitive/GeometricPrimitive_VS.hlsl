#include"Geometric_Primitive.hlsli"

VS_OUT main(float4 Position:POSITION,float4 Nomal:NORMAL ) 
{
	VS_OUT vout;
	vout.Position = mul(Position, mul(World, ViewProjection));
	
	Nomal.w = 0;
	float4 N = normalize(mul(Nomal, World));
	float4 L = normalize(-LightDirection);
	
	vout.Color.rgb = MaterialColor.rgb * max(0, dot(L, N));
	vout.Color.a = MaterialColor.a;
	return vout;

}