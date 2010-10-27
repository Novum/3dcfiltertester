/*
* Shared structs
*/

struct VSInput
{
    float4 position : SV_POSITION; 
    float2 texcoord : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;	
    float2 texcoord : TEXCOORD;	
    float2 scaled_texcoord : SCALED_TEXCOORD;        
};

struct CopyDisplayPSInput
{
    float4 position : SV_POSITION;	
};

cbuffer MayChangeEveryFrame : register(b0)
{
	matrix mvp;	
	float4 camera_position;
	float4 camera_left;
	float4 camera_up;
	float4 camera_forward;
	int2 viewport_size;
};