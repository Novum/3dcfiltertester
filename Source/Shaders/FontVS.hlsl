/*
* Font renderer vertex shader
*/

#include "Globals.hlsl"

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

FontPSInput vs_main(FontVSInput input)
{
    FontPSInput output;
	
	output.position.x = (input.position.x / viewport_size.x) * 2.0f - 1.0f;
	output.position.y = (input.position.y / viewport_size.y) * -2.0f + 1.0f;
	output.position.zw = input.position.zw;
	output.texcoord = input.texcoord;
    
    return output;
}