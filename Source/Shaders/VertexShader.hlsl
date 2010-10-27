/*
* Vertex shader
*/

#include "Globals.hlsl"
#include "FilterGlobals.hlsl"

PSInput vs_main(VSInput input)
{	
    PSInput output;    
	
	output.position = mul(input.position,mvp);	
	output.texcoord = input.texcoord / tex_scale + texcoord_translation;
	output.scaled_texcoord = output.texcoord * texture_size;

    return output;
}