#include"Sprite.hlsli"
VS_OUT main(float4 pos:POSITION,float4 Color:COLOR,float2 Texcoord:TEXCOORD)
{
	VS_OUT  Vout;
	Vout.pos = pos;
	Vout.Color = Color;
	
	//MY_MEMOÅFUNIT5
	Vout.TexCooad = Texcoord;
	return Vout;
}

