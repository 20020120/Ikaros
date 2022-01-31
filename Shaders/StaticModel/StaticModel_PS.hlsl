#include "StaticModelRender.hlsli"

#define PsInput  VsOutPut



float4 main(PsInput Pin) : SV_TARGET
{
	return float4(Pin.Color);
}