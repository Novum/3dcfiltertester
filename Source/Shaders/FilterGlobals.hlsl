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
	bool gamma_correction;
};

float LinearToSRGB(float color)
{
	if (color <= 0.0031308) {
		return color * 12.92;
	}

	return 1.055 * pow(color, 1.0 / 2.4) - 0.055;
}

float4 LinearToSRGB(float4 color)
{
	return float4(LinearToSRGB(color.r), LinearToSRGB(color.g), LinearToSRGB(color.b), LinearToSRGB(color.a));
}