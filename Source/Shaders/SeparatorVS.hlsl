/*
* Separator vertex shader
*/

#include "Globals.hlsl"

float4 vs_main(uint id: SV_VertexID) : SV_POSITION  
{
	if(id == 0) return float4(0.0f, -1.0f, 0.0f, 1.0f);
	else return float4(0.0f, 1.0f, 0.0f, 1.0f);	
}