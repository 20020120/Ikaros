#include "Particle.hlsli"
#define GSInput VS_OUT
#define GSOutput VS_OUT



[maxvertexcount(3)]
void main(
	triangle GSInput input[3],
	inout TriangleStream<GSOutput> output)
{
	// OŠpŒ`‚ğì‚é
	float3 pos[3];
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element = input[i];
		pos[i] = input[i].Position.xyz;
		pos[i] += velocity * Speed * elapsedTime;
	}

}

