/*
* Shared structs for filter shaders
*/

cbuffer FilterParameters : register(b1)
{	
	float2 texture_size;
	float max_anisotropy;
	float lod_bias;
	float2 texcoord_translation;
	float tex_scale;
	float filter_mode;
	bool do_lerp;	
};