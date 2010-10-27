/*
* The TMU filtering shader
*/

Texture2D albedo : register(t0);
SamplerState sample_state : register(s0);

#include "Globals.hlsl"
#include "FilterGlobals.hlsl"

float4 ps_main(PSInput input) : SV_Target
{	
    return albedo.SampleBias(sample_state, input.texcoord, lod_bias); 
}