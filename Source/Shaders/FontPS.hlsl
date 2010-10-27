/*
* Font renderer pixel shader
*/

#include "Globals.hlsl"

Texture2D FontTexture : register(t0);

cbuffer FontCBuffer : register(b1)
{
	float4 FontColor;	
};

SamplerState SamplePoint : register(s0);

struct FontVSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct FontPSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

float4 ps_main(FontPSInput input) : SV_Target
{
	if(FontTexture.Sample(SamplePoint,input.texcoord).x < 0.5) discard;

	return FontColor;
}